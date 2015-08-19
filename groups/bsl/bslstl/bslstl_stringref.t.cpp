// bslstl_stringref.t.cpp                                             -*-C++-*-

#include <bslstl_stringref.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_nativestd.h>

#include <bsltf_templatetestfacility.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <iomanip>

#include <stdio.h>       // sprintf()
#include <stdlib.h>      // atoi()
#include <string.h>

#undef ES  // From solaris/x86 2.10 <stdlib.h>
           //       -> sys/wait.h -> signal.h -> sys/ucontext.h -> sys/regset.h

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test provides an in-core pointer-semantic object.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] bslstl::StringRef();
// [ 2] bslstl::StringRef(const char *begin, const char *end);
// [ 2] bslstl::StringRef(const char *begin, int length);
// [ 2] bslstl::StringRef(const char *begin);
// [ 2] bslstl::StringRef(const bsl::string& begin);
// [ 2] bslstl::StringRef(const native_std::string& begin);
// [ 2] bslstl::StringRef(const bslstl::StringRef& original);
// [ 9] bslstl::StringRefImp(const StringRefImp& , int, int)
// [ 2] ~bslstl::StringRef();
//
// MANIPULATORS
// [ 2] bslstl::StringRef& operator=(const bslstl::StringRef& rhs);
// [ 6] void reset();
// [ 6] void assign(const char *begin, const char *end);
// [ 6] void assign(const char *begin, int length);
// [ 6] void assign(const char *begin);
// [ 6] void assign(const bsl::string& begin);
//
// ACCESSORS
// [ 3] const_iterator begin() const;
// [ 3] const_iterator data() const;
// [ 3] const_iterator end() const;
// [ 3] int            length() const;
// [ 3] int            isEmpty() const;
// [ 3]                operator bsl::string() const;
// [ 3]                operator native_std::string() const;
// [ 3] const char&    operator[](int index) const;
//
// FREE OPERATORS
// [ 5] bool operator==(const StringRef& lhs, const StringRef& rhs);
// [ 5] bool operator==(const bsl::string& lhs, const StringRef& rhs);
// [ 5] bool operator==(const StringRef& lhs, const bsl::string& rhs);
// [ 5] bool operator==(const native_std::string& lhs, const StringRef& rhs);
// [ 5] bool operator==(const StringRef& lhs, const native_std::string& rhs);
// [ 5] bool operator==(const char *lhs, const StringRef& rhs);
// [ 5] bool operator==(const StringRef& lhs, const char *rhs);
// [ 5] bool operator!=(const StringRef& lhs, const StringRef& rhs);
// [ 5] bool operator!=(const bsl::string& lhs, const StringRef& rhs);
// [ 5] bool operator!=(const StringRef& lhs, const bsl::string& rhs);
// [ 5] bool operator!=(const native_std::string& lhs, const StringRef& rhs);
// [ 5] bool operator!=(const StringRef& lhs, const native_std::string& rhs);
// [ 5] bool operator!=(const char *lhs, const StringRef& rhs);
// [ 5] bool operator!=(const StringRef& lhs, const char *rhs);
// [ 5] bool operator<(const StringRef& lhs, const StringRef& rhs);
// [ 5] bool operator<(const bsl::string& lhs, const StringRef& rhs);
// [ 5] bool operator<(const StringRef& lhs, const bsl::string& rhs);
// [ 5] bool operator<(const native_std::string& lhs, const StringRef& rhs);
// [ 5] bool operator<(const StringRef& lhs, const native_std::string& rhs);
// [ 5] bool operator<(const char *lhs, const StringRef& rhs);
// [ 5] bool operator<(const StringRef& lhs, const char *rhs);
// [ 5] bool operator>(const StringRef& lhs, const StringRef& rhs);
// [ 5] bool operator>(const bsl::string& lhs, const StringRef& rhs);
// [ 5] bool operator>(const StringRef& lhs, const bsl::string& rhs);
// [ 5] bool operator>(const native_std::string& lhs, const StringRef& rhs);
// [ 5] bool operator>(const StringRef& lhs, const native_std::string& rhs);
// [ 5] bool operator>(const char *lhs, const StringRef& rhs);
// [ 5] bool operator>(const StringRef& lhs, const char *rhs);
// [ 5] bool operator<=(const StringRef& lhs, const StringRef& rhs);
// [ 5] bool operator<=(const bsl::string& lhs, const StringRef& rhs);
// [ 5] bool operator<=(const StringRef& lhs, const bsl::string& rhs);
// [ 5] bool operator<=(const native_std::string& lhs, const StringRef& rhs);
// [ 5] bool operator<=(const StringRef& lhs, const native_std::string& rhs);
// [ 5] bool operator<=(const char *lhs, const StringRef& rhs);
// [ 5] bool operator<=(const StringRef& lhs, const char *rhs);
// [ 5] bool operator>=(const StringRef& lhs, const StringRef& rhs);
// [ 5] bool operator>=(const bsl::string& lhs, const StringRef& rhs);
// [ 5] bool operator>=(const StringRef& lhs, const bsl::string& rhs);
// [ 5] bool operator>=(const native_std::string& lhs, const StringRef& rhs);
// [ 5] bool operator>=(const StringRef& lhs, const native_std::string& rhs);
// [ 5] bool operator>=(const char *lhs, const StringRef& rhs);
// [ 5] bool operator>=(const StringRef& lhs, const char *rhs);
// [ 4] operator<<(ostream&, const StringRef& string);
// [ 7] operator+(const StringRef& lhs, const StringRef& rhs);
// [ 7] operator+(const bsl::string& lhs, const StringRef& rhs);
// [ 7] operator+(const StringRef& lhs, const bsl::string& rhs);
// [ 7] operator+(const native_std::string& lhs, const StringRef& rhs);
// [ 7] operator+(const StringRef& lhs, const native_std::string& rhs);
// [ 7] operator+(const char *lhs, const StringRef& rhs);
// [ 7] operator+(const StringRef& lhs, const char *rhs);
// [ 7] basic_string basic_string::operator+=(const StringRefData& strRf);
// [ 8] bsl::hash<BloombergLP::bslstl::StringRef>
// [ 8] bslh::Hash<>
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU


// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bslstl::StringRef Obj;

template <class CHAR>
struct TestData
{
    static CHAR const * emptyString;
    static CHAR const * nonEmptyString;
    static CHAR const * stringValue1;
    static CHAR const * stringValue2;
};

template <>
char const * TestData<char>::emptyString = "";

template <>
char const * TestData<char>::nonEmptyString = "Tangled Up in Blue - Bob Dylan";

template <>
char const * TestData<char>::stringValue1 = "abcde";

template <>
char const * TestData<char>::stringValue2 = "abcfg";

template <>
wchar_t const * TestData<wchar_t>::emptyString = L"";

template <>
wchar_t const * TestData<wchar_t>::nonEmptyString
    = L"Tangled Up in Blue - Bob Dylan";

template <>
wchar_t const * TestData<wchar_t>::stringValue1 = L"abcde";

template <>
wchar_t const * TestData<wchar_t>::stringValue2 = L"abcfg";

char const * EMPTY_STRING     = TestData<char>::emptyString;
char const * NON_EMPTY_STRING = TestData<char>::nonEmptyString;

// ============================================================================
//                     GLOBAL TEST DRIVER
// ----------------------------------------------------------------------------

void copyStringValue(char    *result, const char *asciiInput);
void copyStringValue(wchar_t *result, const char *asciiInput);
    // Load 'result' with the specified 'asciiInput', where each input
    // character value is converted to the (template parameter) type
    // 'CHAR_TYPE'.  The behavior is undefined unless 'asciiInput' is a valid
    // null terminated ascii string.

void copyStringValue(char *result, const char *asciiInput)
{
    strcpy(result, asciiInput);
}

void copyStringValue(wchar_t *result, const char *asciiInput)
{
    do {
        *result = *asciiInput;
        ++result;
        ++asciiInput;
    } while (*asciiInput);
}


template <class CHAR_TYPE>
class TestDriver {
    // TYPES
    typedef bslstl::StringRefImp<CHAR_TYPE> Obj;
        // Type under testing.

  public:

    static CHAR_TYPE charValue(char c);

    static void testCase9();
        // Testing 'StringRefImp(const StringRefImp& , int, int)'
};

template <class CHAR_TYPE>
void TestDriver<CHAR_TYPE>::testCase9()
{
    // --------------------------------------------------------------------
    // TESTING: StringRefImp(const StringRefImp& , int, int)
    //
    // Concerns:
    //
    //: 1 Supplying an empty string ref, and 'startIndex' 0, 'numCharacters'
    //:   0, results in an empty string ref.
    //:
    //: 2 For a non empty string ref, the resulting string ref's 'begin'
    //:   iterator is at the same address as the 'startIndex' character of the
    //:   original string ref.
    //:
    //: 3 If 'numCharacters' does next extend beyond the end of the original
    //:   string ref, the resulting string ref is 'numCharacters' in length.
    //:
    //: 4 If 'numCharacters' extends beyond the end of the original string
    //:   ref the resulting string ref is the tail of the original string ref.
    //:
    //: 5 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //
    //: 1 Test with an empty string ref as input. (C-1)
    //:
    //: 2 Test a non-empty input through all possible start indices. (C-2)
    //:
    //: 3 Test a non-empty input through all possible lengths of the input and
    //:   beyond the end of the input range. (C-3,4)
    //:
    //: 4 Test a non-empty input through possibly start indices and lengths (a
    //:   cross product of tests 2 & 3). (C-2..4)
    //:
    //: 5 Test the asserted preconditions using the 'BSLS_ASSERTTEST_*'
    //:   macros. (C-5)
    //
    // Testing:
    //   StringRefImp(const StringRefImp& , int, int)
    // --------------------------------------------------------------------


    if (verbose) printf("\tTest empty string\n");
    {
        Obj x;              const Obj& X = x;
        Obj y(X, 0, 0);     const Obj& Y = y;
        Obj z(X, 0, 32768); const Obj& Z = z;

        ASSERT(Y.isEmpty());
        ASSERT(Z.isEmpty());
    }

    if (verbose) printf("\tTest various values for 'startIndex'\n");
    {
        const char value[] = "abcdefg";
        CHAR_TYPE  input[sizeof(value)];
        copyStringValue(input, value);

        const native_std::size_t LENGTH = sizeof(value);

        Obj original(input, LENGTH); const Obj& ORIGINAL = original;
        for (native_std::size_t i = 0; i <= LENGTH; ++i) {
            Obj x(ORIGINAL, i, INT_MAX); const Obj& X = x;

            ASSERT(ORIGINAL.begin() + i == X.begin());
            ASSERT(ORIGINAL.end()       == X.end());

            if (veryVerbose && i!= LENGTH) {
                P_(i); P_(ORIGINAL[i]); P(*X.begin());
            }
            else if (veryVerbose) {
                P(i);
            }
        }
    }

    if (verbose) printf("\tTest various values for 'numCharacters'\n");
    {
        const char value[] = "abcdefg";
        CHAR_TYPE  input[sizeof(value)];
        copyStringValue(input, value);

        const native_std::size_t LENGTH = sizeof(value);

        Obj original(input, LENGTH); const Obj& ORIGINAL = original;
        for (native_std::size_t i = 0; i < LENGTH; ++i) {
            Obj x(ORIGINAL, 0, i); const Obj& X = x;

            ASSERT(ORIGINAL.begin() == X.begin());
            ASSERT(ORIGINAL.end()   == X.end() + (ORIGINAL.length() - i));

            if (veryVerbose) {
                P_(i); P_(ORIGINAL.length()); P(X.length())
            }
        }
        for (native_std::size_t i = LENGTH; i < LENGTH + 10; ++i) {
            Obj x(ORIGINAL, 0, i); const Obj& X = x;

            ASSERT(ORIGINAL.begin() == X.begin());
            ASSERT(ORIGINAL.end()   == X.end());

            if (veryVerbose) {
                P_(i); P_(ORIGINAL.length()); P(X.length())
            }

        }
        {
            // Test INT_MAX
            Obj x(ORIGINAL, 0, INT_MAX); const Obj& X = x;

            ASSERT(ORIGINAL.begin() == X.begin());
            ASSERT(ORIGINAL.end()   == X.end());

            if (veryVerbose) {
                P_(ORIGINAL.length()); P(X.length())
            }

        }
    }

    if (verbose) printf("\tTest both 'startIndex' and 'numCharacters'\n");
    {
        const char value[] = "abcdefg";
        CHAR_TYPE  input[sizeof(value)];
        copyStringValue(input, value);

        const native_std::size_t LENGTH = sizeof(value);
        Obj original(input, LENGTH); const Obj& ORIGINAL = original;

        for (native_std::size_t startIdx = 0; startIdx <= LENGTH; ++startIdx) {
            for (native_std::size_t length = 0; length < LENGTH + 1; ++length){

                Obj x(ORIGINAL, startIdx, length); const Obj& X = x;

                ASSERT(ORIGINAL.begin() + startIdx == X.begin());
                if (startIdx + length < LENGTH) {
                    ASSERT(ORIGINAL.end() ==
                           X.end() + LENGTH - (length + startIdx));
                }
                else {
                    ASSERT(ORIGINAL.end() == X.end());
                }

                if (veryVerbose && startIdx != LENGTH) {
                    P_(startIdx); P_(length);
                    P_(ORIGINAL[startIdx]); P_(*X.begin()); P(X.length())
                }
                else if (veryVerbose) {
                    P_(startIdx); P_(length); P(X.length())

                }
            }
        }
    }

    if (verbose) printf("\tNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        const char value[] = "abcdefg";
        CHAR_TYPE  input[sizeof(value)];
        copyStringValue(input, value);
        const native_std::size_t LENGTH = sizeof(value);

        Obj original(input, LENGTH); const Obj& ORIGINAL = original;

        if (verbose) printf("\tNegative testing '0 <= startIndex'.\n");
        {
            ASSERT_SAFE_PASS_RAW(Obj(ORIGINAL, 0, 0));
            ASSERT_SAFE_FAIL_RAW(Obj(ORIGINAL, -1, 0));
            ASSERT_SAFE_FAIL_RAW(Obj(ORIGINAL, INT_MIN, 0));
        }

        if (verbose) printf("\tNegative testing 'startIndex <= or.length'.\n");
        {
            ASSERT_SAFE_PASS_RAW(Obj(ORIGINAL, ORIGINAL.length(), 0));
            ASSERT_SAFE_FAIL_RAW(Obj(ORIGINAL, ORIGINAL.length() + 1, 0));
        }

        if (verbose) printf("\tNegative testing '0 <= numCharacters'.\n");
        {
            ASSERT_SAFE_PASS_RAW(Obj(ORIGINAL, 0, 0));
            ASSERT_SAFE_FAIL_RAW(Obj(ORIGINAL, 0, -1));
            ASSERT_SAFE_FAIL_RAW(Obj(ORIGINAL, 0, INT_MIN));
        }
    }
}


//=============================================================================
//                 HELPER FUNCTIONS FOR TESTING USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Basic Operations
///- - - - - - - - - - - - - -
// The following snippets of code illustrate basic and varied use of the
// 'bslstl::StringRef' class.
//
// First, we define a function, 'getNumBlanks', that returns the number of
// blank (' ') characters contained in the string referenced by a specified
// 'bslstl::StringRef':
//..
//  #include <algorithm>

    int getNumBlanks(const bslstl::StringRef& stringRef)
        // Return the number of blank (' ') characters in the string referenced
        // by the specified 'stringRef'.
    {
#ifdef BSLS_PLATFORM_CMP_SUN
        std::size_t n = 0;
        std::count(stringRef.begin(), stringRef.end(), ' ', n);
        return n;
#else
        return std::count(stringRef.begin(), stringRef.end(), ' ');
#endif
    }
//..
// Notice that the function delegates the work to the 'std::count' STL
// algorithm.  This delegation is made possible by the STL-compatible iterators
// provided by the 'begin' and 'end' accessors.

template <class CHAR>
void testBasicAccessors(bool verbose)
{
    if (verbose) std::cout << "\nTESTING BASIC ACCESSORS"
                           << "\n======================="
                           << std::endl;

    if (verbose) std::cout << "\nTesting:\n\t'begin'\n\t'data'"
                              "\n\t'end'\n\t'length'\n\t'empty'"
                              "\n\t'operator bsl::string'"
                              "\n\t'operator native_std::string'"
                           << "\n= = = = = = = = = = = = = = = = = = = ="
                           << std::endl;

    {
        // EMPTY STRING
        bslstl::StringRefImp<CHAR> es(TestData<CHAR>::emptyString);
        const bslstl::StringRefImp<CHAR>& ES = es;

        ASSERT(ES.begin()   == TestData<CHAR>::emptyString);
        ASSERT(ES.data()    == TestData<CHAR>::emptyString);
        ASSERT(ES.data()    == ES.begin());
        ASSERT(ES.end()     == TestData<CHAR>::emptyString);
        ASSERT(ES.length()  ==
           native_std::char_traits<CHAR>::length(TestData<CHAR>::emptyString));
        ASSERT(ES.empty());
        ASSERT(ES.isEmpty());

        bsl::basic_string<CHAR> EString(TestData<CHAR>::emptyString);
        ASSERT(EString  == static_cast<bsl::basic_string<CHAR> >(ES));

        native_std::basic_string<CHAR> EString2(TestData<CHAR>::emptyString);
        ASSERT(EString2 == static_cast<native_std::basic_string<CHAR> >(ES));

        // NON-EMPTY STRING
        bslstl::StringRefImp<CHAR> nes(TestData<CHAR>::nonEmptyString);
        const bslstl::StringRefImp<CHAR>& NES = nes;
        std::size_t LEN = native_std::char_traits<CHAR>::length(
                                               TestData<CHAR>::nonEmptyString);

        ASSERT(NES.begin()   == TestData<CHAR>::nonEmptyString);
        ASSERT(NES.data()    == TestData<CHAR>::nonEmptyString);
        ASSERT(NES.data()    == NES.begin());
        ASSERT(NES.end()     == TestData<CHAR>::nonEmptyString + LEN);
        ASSERT(NES.length()  == LEN);
        ASSERT(!NES.empty());
        ASSERT(!NES.isEmpty());

        bsl::basic_string<CHAR> NEString(TestData<CHAR>::nonEmptyString);
        ASSERT(NEString  == static_cast<bsl::basic_string<CHAR> >(NES));

        native_std::basic_string<CHAR>
            NEString2(TestData<CHAR>::nonEmptyString);
        ASSERT(NEString2 == static_cast<native_std::basic_string<CHAR> >(NES));
    }

    if (verbose) std::cout << "\nTesting: 'compare'"
                           << "\n= = = = = = = = = "
                           << std::endl;

    {
        bslstl::StringRefImp<CHAR> s1(TestData<CHAR>::stringValue1);
        bslstl::StringRefImp<CHAR> s2(TestData<CHAR>::stringValue2);
        ASSERT(s1.compare(s1) == 0);
        ASSERT(s1.compare(s2) < 0);

        bslstl::StringRefImp<CHAR> s3(TestData<CHAR>::stringValue2, 3);
        ASSERT(s1.compare(s3) > 0);

        bslstl::StringRefImp<CHAR> s4(TestData<CHAR>::stringValue1, 3);
        ASSERT(s3.compare(s4) == 0);

        bslstl::StringRefImp<CHAR> s5;
        ASSERT(s1.compare(s5) > 0);
        ASSERT(s5.compare(s5) == 0);
    }

    if (verbose) std::cout << "\nTesting: 'operator[]()'"
                           << "\n= = = = = = = = = = = ="
                           << std::endl;

    {
        bslstl::StringRefImp<CHAR> x2(TestData<CHAR>::nonEmptyString);
        const bslstl::StringRefImp<CHAR>& X2 = x2;

        // NON-EMPTY STRING
        int Len = static_cast<int>(native_std::char_traits<CHAR>::length(
                                              TestData<CHAR>::nonEmptyString));
        for (int idx = 0; idx < Len; ++idx) {
            LOOP_ASSERT(idx, X2[idx] == TestData<CHAR>::nonEmptyString[idx]);
        }
    }
}


//=============================================================================
//                             MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    std::cout << "TEST " << __FILE__ << " CASE " << test << std::endl;

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nTesting Usage Example"
                               << "\n=====================" << std::endl;

// Then, call 'getNumBlanks' on a default constructed 'bslstl::StringRef':
//..
    bslstl::StringRef emptyRef;
    int numBlanks = getNumBlanks(emptyRef);
    ASSERT(0 == numBlanks);

    ASSERT(""         == emptyRef);
    ASSERT("anything" >= emptyRef);
//..
// Notice that the behavior a default constructed 'bslstl::StringRef' object
// behaves the same as if it referenced an empty string.
//
// Next, we (implicitly) construct a 'bsl::string' object from
// 'bslstl::StringRef':
//..
    bsl::string empty(emptyRef);
    ASSERT(0 == empty.size());
//..
// Then, we call 'getNumBlanks' on a string literal and assert that the number
// of blanks returned is as expected:
//..
    numBlanks = getNumBlanks("Good things come to those who wait.");
    ASSERT(6 == numBlanks);
//..
// Next, we define a longer string literal, 'poem', that we will use in the
// rest of this usage example:
//..
    const char poem[] =                  // by William Butler Yeats (1865-1939)
//      |....5....|....5....|....5....|....5....|   //  length  blanks
                                                    //
        "O love is the crooked thing,\n"            //    29      5
        "There is nobody wise enough\n"             //    28      4
        "To find out all that is in it,\n"          //    31      7
        "For he would be thinking of love\n"        //    33      6
        "Till the stars had run away\n"             //    28      5
        "And the shadows eaten the moon.\n"         //    32      5
        "Ah, penny, brown penny, brown penny,\n"    //    37      5
        "One cannot begin it too soon.";            //    29      5
                                                    //          ----
                                                    //    total: 42
    numBlanks = getNumBlanks(poem);
    ASSERT(42 == numBlanks);
//..
// Then, we construct a 'bslstl::StringRef' object, 'line', that refers to only
// the first line of the 'poem':
//..
    bslstl::StringRef line(poem, 29);
    numBlanks = getNumBlanks(line);

    ASSERT( 5 == numBlanks);
    ASSERT(29 == line.length());
    ASSERT( 0 == std::strncmp(poem, line.data(), line.length()));
//..
// Next, we use the 'assign' method to make 'line' refer to the second line of
// the 'poem':
//..
    line.assign(poem + 29, poem + 57);
    numBlanks = getNumBlanks(line);
    ASSERT(4 == numBlanks);
    ASSERT((57 - 29) == line.length());
    ASSERT("There is nobody wise enough\n" == line);
//..
// Then, we call 'getNumBlanks' with a 'bsl::string' initialized to the
// contents of the 'poem':
//..
    const bsl::string poemString(poem);
    numBlanks = getNumBlanks(poemString);
    ASSERT(42 == numBlanks);
    ASSERT(bslstl::StringRef(poemString) == poemString);
    ASSERT(bslstl::StringRef(poemString) == poemString.c_str());
//..
// Next, we make a 'bslstl::StringRef' object that refers to a string that will
// be able to hold embedded null characters:
//..
    char poemWithNulls[512];
    const int poemLength = std::strlen(poem);
    ASSERT(poemLength < 512);

    std::memcpy(poemWithNulls, poem, poemLength + 1);
    ASSERT(0 == std::strcmp(poem, poemWithNulls));
//..
// Now, we replace each occurrence of a '\n' in 'poemWithNulls' with a yielding
// '\0':
//..
    std::replace(poemWithNulls, poemWithNulls + poemLength, '\n', '\0');
    ASSERT(0 != std::strcmp(poem, poemWithNulls));
//..
// Finally, we observe that 'poemWithNulls' has the same number of blank
// characters as the original 'poem':
//..
    numBlanks = getNumBlanks(bslstl::StringRef(poemWithNulls, poemLength));
    ASSERT(42 == numBlanks);
//..
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // StringRefImp(const StringRefImp& , int, int)
        //
        // --------------------------------------------------------------------

        //  See 'TestDriver::testCase9' for concerns and plan.

        if (verbose) printf("\nTesting: StringRefImp(StringRefImp&, int, int)"
                            "\n=============================================="
                            "\n");


        RUN_EACH_TYPE(TestDriver, testCase9, char, wchar_t);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING HASH FUNCTION
        //
        // Concerns:
        //: 1 The hash function in versions of this component prior to
        //:   /main/bb/dev/10 returned '0' for all input strings.  This test
        //:   will verify that the hash function returns acceptably distinct
        //:   values for a set of input strings, allowing for at most one
        //:   collision.
        //:
        //: 2 The 'hashAppend' function should be picked up and used by
        //;   bslh:Hash. The whole string should be used in this hash.
        //:   'bsl::hash' specialization has been deleted, so calls to
        //:   'bsl::hash' should automatically forward to 'bslh::Hash'.
        //
        // Plan:
        //: 1 Hash a reasonably large number of strings, capturing the hash
        //:   values.  Make sure re-hashing the same strings in a different
        //:   order returns the same values.  Then make sure that each
        //:   resulting hash was not encountered more than twice (so we're
        //:   allowing SOME collisions, but not too many).
        //:
        //:   The strings to be hashed will include some "typical" short strings
        //:   including the names of current and past members of the BDE team
        //:   and the tickers for the members of the S&P 500 index.
        //:
        //:   While there are no guarantees that these data sets are
        //:   representative, this at least allows us to make sure that our hash
        //:   performs in a reasonable manner.
        //:
        //: 2 Test using both bslh::Hash<> and bsl::hash<StringRef> (both of
        //:    which should now give the same result). Hash strings where only
        //:    the final value differs to ensure that the full length of the
        //:    string is being hashed. Also hash multiple copies of the same
        //:    string and ensure they produce the same hash to make sure
        //:    nothing beyond the end of the string is being hashed.
        //
        // Testing:
        //   bsl::hash<BloombergLP::bslstl::StringRef>
        //   bslh::Hash<>
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nTesting Hash Function"
                               << "\n=====================" << std::endl;

        static const struct {
            int         d_line;
            const char *d_str;
        } DATA[] = {
            //line string
            //---- ------

            // the empty string
            { L_,  ""                           },

            // some simple keys
            { L_,  "a"                          },
            { L_,  "aa"                         },
            { L_,  "aaa"                        },
            { L_,  "aaaa"                       },
            { L_,  "aaaaa"                      },
            { L_,  "aaaaaa"                     },
            { L_,  "b"                          },
            { L_,  "bb"                         },
            { L_,  "bbb"                        },
            { L_,  "bbbb"                       },
            { L_,  "bbbbb"                      },
            { L_,  "bbbbbb"                     },
          // Skipped, A and AA are also stock tickers listed below in the "SPX
          // Index".
          //{ L_,  "A"                          },
          //{ L_,  "AA"                         },
            { L_,  "AAA"                        },
            { L_,  "AAAA"                       },
            { L_,  "AAAAA"                      },
            { L_,  "AAAAAA"                     },
          // skipped, also potentially a stock ticker
          //{ L_,  "B"                          },
            { L_,  "BB"                         },
            { L_,  "BBB"                        },
            { L_,  "BBBB"                       },
            { L_,  "BBBBB"                      },
            { L_,  "BBBBBB"                     },
            { L_,  "0"                          },
            { L_,  "00"                         },
            { L_,  "000"                        },
            { L_,  "0000"                       },
            { L_,  "00000"                      },
            { L_,  "000000"                     },
            { L_,  "1"                          },
            { L_,  "11"                         },
            { L_,  "111"                        },
            { L_,  "1111"                       },
            { L_,  "11111"                      },
            { L_,  "111111"                     },
            // Some personal names.  In this case, current and
            // former BDE team members, captured via
            // cat /etc/passwd| grep -i bde | \$
            //  'BEGIN{FS=":"} {print $5}'  | \$
            //   awk '{print $1; print $2; print $1,$2}'|sort -u
            // .
            { L_,  "Case"                       },
            { L_,  "Clear"                      },
            { L_,  "Clear Case"                 },
            { L_,  "Role"                       },
            { L_,  "Verity"                     },
            { L_,  "Verity Role"                },
            { L_,  "abdelaziz"                  },
            { L_,  "abdelmalek"                 },
            { L_,  "ahmed"                      },
            { L_,  "ahmed abdelaziz"            },
            { L_,  "andrei"                     },
            { L_,  "andrei basov"               },
            { L_,  "anthony"                    },
            { L_,  "anthony comerico"           },
            { L_,  "basov"                      },
            { L_,  "bhindwale"                  },
            { L_,  "bill"                       },
            { L_,  "bill chapman"               },
            { L_,  "bob"                        },
            { L_,  "bob deanna"                 },
            { L_,  "breitstein"                 },
            { L_,  "bronnimann"                 },
            { L_,  "bruce"                      },
            { L_,  "bruce szablak"              },
            { L_,  "chapman"                    },
            { L_,  "chen"                       },
            { L_,  "clay"                       },
            { L_,  "clay wilson"                },
            { L_,  "comerico"                   },
            { L_,  "cushing"                    },
            { L_,  "david"                      },
            { L_,  "david rubin"                },
            { L_,  "david schumann"             },
            { L_,  "david winsor"               },
            { L_,  "day"                        },
            { L_,  "deanna"                     },
            { L_,  "dorfman"                    },
            { L_,  "etra"                       },
            { L_,  "farber"                     },
            { L_,  "gang"                       },
            { L_,  "gang chen"                  },
            { L_,  "giroux"                     },
            { L_,  "guillaume"                  },
            { L_,  "guillaume morin"            },
            { L_,  "halpern"                    },
            { L_,  "henry"                      },
            { L_,  "henry verschell"            },
            { L_,  "herve"                      },
            { L_,  "herve bronnimann"           },
            { L_,  "hugh"                       },
            { L_,  "hugh cushing"               },
            { L_,  "hutton"                     },
            { L_,  "jeff"                       },
            { L_,  "jeff hutton"                },
            { L_,  "john"                       },
            { L_,  "john lakos"                 },
            { L_,  "lakos"                      },
            { L_,  "lindrick"                   },
            { L_,  "lindrick outerbridge"       },
            { L_,  "manny"                      },
            { L_,  "manny farber"               },
            { L_,  "marshall"                   },
            { L_,  "matthew"                    },
            { L_,  "matthew millett"            },
            { L_,  "michael"                    },
            { L_,  "michael nusimow"            },
            { L_,  "mike"                       },
            { L_,  "mike giroux"                },
            { L_,  "millett"                    },
            { L_,  "morin"                      },
            { L_,  "nathan"                     },
            { L_,  "nathan dorfman"             },
            { L_,  "nusimow"                    },
            { L_,  "oleg"                       },
            { L_,  "oleg semenov"               },
            { L_,  "olga"                       },
            { L_,  "olga zykova"                },
            { L_,  "outerbridge"                },
            { L_,  "pablo"                      },
            { L_,  "pablo halpern"              },
            { L_,  "revzin"                     },
            { L_,  "robert"                     },
            { L_,  "robert day"                 },
            { L_,  "rohan"                      },
            { L_,  "rohan bhindwale"            },
            { L_,  "roman"                      },
            { L_,  "roman revzin"               },
            { L_,  "rubin"                      },
            { L_,  "schumann"                   },
            { L_,  "semenov"                    },
            { L_,  "sherry"                     },
            { L_,  "sherry sontag"              },
            { L_,  "sontag"                     },
            { L_,  "stan"                       },
            { L_,  "stan etra"                  },
            { L_,  "steven"                     },
            { L_,  "steven breitstein"          },
            { L_,  "szablak"                    },
            { L_,  "thomas"                     },
            { L_,  "thomas marshall"            },
            { L_,  "verschell"                  },
            { L_,  "wilson"                     },
            { L_,  "winsor"                     },
            { L_,  "yasser"                     },
            { L_,  "yasser abdelmalek"          },
            { L_,  "zykova"                     },

            // The first part of the stock tickers of the 499 members of the
            // S&P 500 as reported by {SPX Index MEMB<GO>} on 20081112
            { L_,  "A"                          },
            { L_,  "AA"                         },
            { L_,  "AAPL"                       },
            { L_,  "ABC"                        },
            { L_,  "ABI"                        },
            { L_,  "ABT"                        },
            { L_,  "ACAS"                       },
            { L_,  "ACS"                        },
            { L_,  "ADBE"                       },
            { L_,  "ADI"                        },
            { L_,  "ADM"                        },
            { L_,  "ADP"                        },
            { L_,  "ADSK"                       },
            { L_,  "AEE"                        },
            { L_,  "AEP"                        },
            { L_,  "AES"                        },
            { L_,  "AET"                        },
            { L_,  "AFL"                        },
            { L_,  "AGN"                        },
            { L_,  "AIG"                        },
            { L_,  "AIV"                        },
            { L_,  "AIZ"                        },
            { L_,  "AKAM"                       },
            { L_,  "AKS"                        },
            { L_,  "ALL"                        },
            { L_,  "ALTR"                       },
            { L_,  "AMAT"                       },
            { L_,  "AMD"                        },
            { L_,  "AMGN"                       },
            { L_,  "AMP"                        },
            { L_,  "AMT"                        },
            { L_,  "AMZN"                       },
            { L_,  "AN"                         },
            { L_,  "ANF"                        },
            { L_,  "AOC"                        },
            { L_,  "APA"                        },
            { L_,  "APC"                        },
            { L_,  "APD"                        },
            { L_,  "APH"                        },
            { L_,  "APOL"                       },
            { L_,  "ASH"                        },
            { L_,  "ATI"                        },
            { L_,  "AVB"                        },
            { L_,  "AVP"                        },
            { L_,  "AVY"                        },
            { L_,  "AW"                         },
            { L_,  "AXP"                        },
            { L_,  "AYE"                        },
            { L_,  "AZO"                        },
            { L_,  "BA"                         },
            { L_,  "BAC"                        },
            { L_,  "BAX"                        },
            { L_,  "BBBY"                       },
            { L_,  "BBT"                        },
            { L_,  "BBY"                        },
            { L_,  "BCR"                        },
            { L_,  "BDK"                        },
            { L_,  "BDX"                        },
            { L_,  "BEN"                        },
            { L_,  "BF/B"                       },
            { L_,  "BHI"                        },
            { L_,  "BIG"                        },
            { L_,  "BIIB"                       },
            { L_,  "BJS"                        },
            { L_,  "BK"                         },
            { L_,  "BLL"                        },
            { L_,  "BMC"                        },
            { L_,  "BMS"                        },
            { L_,  "BMY"                        },
            { L_,  "BNI"                        },
            { L_,  "BRCM"                       },
            { L_,  "BRL"                        },
            { L_,  "BSX"                        },
            { L_,  "BTU"                        },
            { L_,  "BUD"                        },
            { L_,  "BXP"                        },
            { L_,  "C"                          },
            { L_,  "CA"                         },
            { L_,  "CAG"                        },
            { L_,  "CAH"                        },
            { L_,  "CAM"                        },
            { L_,  "CAT"                        },
            { L_,  "CB"                         },
            { L_,  "CBE"                        },
            { L_,  "CBG"                        },
            { L_,  "CBS"                        },
            { L_,  "CCE"                        },
            { L_,  "CCL"                        },
            { L_,  "CEG"                        },
            { L_,  "CELG"                       },
            { L_,  "CF"                         },
            { L_,  "CHK"                        },
            { L_,  "CHRW"                       },
            { L_,  "CI"                         },
            { L_,  "CIEN"                       },
            { L_,  "CINF"                       },
            { L_,  "CIT"                        },
            { L_,  "CL"                         },
            { L_,  "CLX"                        },
            { L_,  "CMA"                        },
            { L_,  "CMCSA"                      },
            { L_,  "CME"                        },
            { L_,  "CMI"                        },
            { L_,  "CMS"                        },
            { L_,  "CNP"                        },
            { L_,  "CNX"                        },
            { L_,  "COF"                        },
            { L_,  "COG"                        },
            { L_,  "COH"                        },
            { L_,  "COL"                        },
            { L_,  "COP"                        },
            { L_,  "COST"                       },
            { L_,  "COV"                        },
            { L_,  "CPB"                        },
            { L_,  "CPWR"                       },
            { L_,  "CRM"                        },
            { L_,  "CSC"                        },
            { L_,  "CSCO"                       },
            { L_,  "CSX"                        },
            { L_,  "CTAS"                       },
            { L_,  "CTL"                        },
            { L_,  "CTSH"                       },
            { L_,  "CTX"                        },
            { L_,  "CTXS"                       },
            { L_,  "CVG"                        },
            { L_,  "CVH"                        },
            { L_,  "CVS"                        },
            { L_,  "CVX"                        },
            { L_,  "D"                          },
            { L_,  "DD"                         },
            { L_,  "DDR"                        },
            { L_,  "DE"                         },
            { L_,  "DELL"                       },
            { L_,  "DF"                         },
            { L_,  "DFS"                        },
            { L_,  "DGX"                        },
            { L_,  "DHI"                        },
            { L_,  "DHR"                        },
            { L_,  "DIS"                        },
            { L_,  "DOV"                        },
            { L_,  "DOW"                        },
            { L_,  "DPS"                        },
            { L_,  "DRI"                        },
            { L_,  "DTE"                        },
            { L_,  "DTV"                        },
            { L_,  "DUK"                        },
            { L_,  "DVA"                        },
            { L_,  "DVN"                        },
            { L_,  "DYN"                        },
            { L_,  "EBAY"                       },
            { L_,  "ECL"                        },
            { L_,  "ED"                         },
            { L_,  "EFX"                        },
            { L_,  "EIX"                        },
            { L_,  "EK"                         },
            { L_,  "EL"                         },
            { L_,  "EMC"                        },
            { L_,  "EMN"                        },
            { L_,  "EMR"                        },
            { L_,  "EOG"                        },
            { L_,  "EP"                         },
            { L_,  "EQ"                         },
            { L_,  "EQR"                        },
            { L_,  "ERTS"                       },
            { L_,  "ESRX"                       },
            { L_,  "ESV"                        },
            { L_,  "ETFC"                       },
            { L_,  "ETN"                        },
            { L_,  "ETR"                        },
            { L_,  "EXC"                        },
            { L_,  "EXPD"                       },
            { L_,  "EXPE"                       },
            { L_,  "F"                          },
            { L_,  "FAST"                       },
            { L_,  "FCX"                        },
            { L_,  "FDO"                        },
            { L_,  "FDX"                        },
            { L_,  "FE"                         },
            { L_,  "FHN"                        },
            { L_,  "FII"                        },
            { L_,  "FIS"                        },
            { L_,  "FISV"                       },
            { L_,  "FITB"                       },
            { L_,  "FLR"                        },
            { L_,  "FLS"                        },
            { L_,  "FO"                         },
            { L_,  "FPL"                        },
            { L_,  "FRX"                        },
            { L_,  "FTR"                        },
            { L_,  "GAS"                        },
            { L_,  "GCI"                        },
            { L_,  "GD"                         },
            { L_,  "GE"                         },
            { L_,  "GENZ"                       },
            { L_,  "GGP"                        },
            { L_,  "GILD"                       },
            { L_,  "GIS"                        },
            { L_,  "GLW"                        },
            { L_,  "GM"                         },
            { L_,  "GME"                        },
            { L_,  "GNW"                        },
            { L_,  "GOOG"                       },
            { L_,  "GPC"                        },
            { L_,  "GPS"                        },
            { L_,  "GR"                         },
            { L_,  "GS"                         },
            { L_,  "GT"                         },
            { L_,  "GWW"                        },
            { L_,  "HAL"                        },
            { L_,  "HAR"                        },
            { L_,  "HAS"                        },
            { L_,  "HBAN"                       },
            { L_,  "HCBK"                       },
            { L_,  "HCP"                        },
            { L_,  "HD"                         },
            { L_,  "HES"                        },
            { L_,  "HIG"                        },
            { L_,  "HNZ"                        },
            { L_,  "HOG"                        },
            { L_,  "HON"                        },
            { L_,  "HOT"                        },
            { L_,  "HPC"                        },
            { L_,  "HPQ"                        },
            { L_,  "HRB"                        },
            { L_,  "HRS"                        },
            { L_,  "HSP"                        },
            { L_,  "HST"                        },
            { L_,  "HSY"                        },
            { L_,  "HUM"                        },
            { L_,  "IBM"                        },
            { L_,  "ICE"                        },
            { L_,  "IFF"                        },
            { L_,  "IGT"                        },
            { L_,  "INTC"                       },
            { L_,  "INTU"                       },
            { L_,  "IP"                         },
            { L_,  "IPG"                        },
            { L_,  "IR"                         },
            { L_,  "ISRG"                       },
            { L_,  "ITT"                        },
            { L_,  "ITW"                        },
            { L_,  "IVZ"                        },
            { L_,  "JAVA"                       },
            { L_,  "JBL"                        },
            { L_,  "JCI"                        },
            { L_,  "JCP"                        },
            { L_,  "JDSU"                       },
            { L_,  "JEC"                        },
            { L_,  "JNJ"                        },
            { L_,  "JNPR"                       },
            { L_,  "JNS"                        },
            { L_,  "JNY"                        },
            { L_,  "JPM"                        },
            { L_,  "JWN"                        },
            { L_,  "K"                          },
            { L_,  "KBH"                        },
            { L_,  "KEY"                        },
            { L_,  "KFT"                        },
            { L_,  "KG"                         },
            { L_,  "KIM"                        },
            { L_,  "KLAC"                       },
            { L_,  "KMB"                        },
            { L_,  "KO"                         },
            { L_,  "KR"                         },
            { L_,  "KSS"                        },
            { L_,  "L"                          },
            { L_,  "LEG"                        },
            { L_,  "LEN"                        },
            { L_,  "LH"                         },
            { L_,  "LIZ"                        },
            { L_,  "LLL"                        },
            { L_,  "LLTC"                       },
            { L_,  "LLY"                        },
            { L_,  "LM"                         },
            { L_,  "LMT"                        },
            { L_,  "LNC"                        },
            { L_,  "LO"                         },
            { L_,  "LOW"                        },
            { L_,  "LSI"                        },
            { L_,  "LTD"                        },
            { L_,  "LUK"                        },
            { L_,  "LUV"                        },
            { L_,  "LXK"                        },
            { L_,  "M"                          },
            { L_,  "MA"                         },
            { L_,  "MAR"                        },
            { L_,  "MAS"                        },
            { L_,  "MAT"                        },
            { L_,  "MBI"                        },
            { L_,  "MCD"                        },
            { L_,  "MCHP"                       },
            { L_,  "MCK"                        },
            { L_,  "MCO"                        },
            { L_,  "MDP"                        },
            { L_,  "MDT"                        },
            { L_,  "MEE"                        },
            { L_,  "MER"                        },
            { L_,  "MET"                        },
            { L_,  "MHP"                        },
            { L_,  "MHS"                        },
            { L_,  "MI"                         },
            { L_,  "MIL"                        },
            { L_,  "MKC"                        },
            { L_,  "MMC"                        },
            { L_,  "MMM"                        },
            { L_,  "MO"                         },
            { L_,  "MOLX"                       },
            { L_,  "MON"                        },
            { L_,  "MOT"                        },
            { L_,  "MRK"                        },
            { L_,  "MRO"                        },
            { L_,  "MS"                         },
            { L_,  "MSFT"                       },
            { L_,  "MTB"                        },
            { L_,  "MTW"                        },
            { L_,  "MU"                         },
            { L_,  "MUR"                        },
            { L_,  "MWV"                        },
            { L_,  "MWW"                        },
            { L_,  "MYL"                        },
            { L_,  "NBL"                        },
            { L_,  "NBR"                        },
            { L_,  "NCC"                        },
            { L_,  "NDAQ"                       },
            { L_,  "NE"                         },
            { L_,  "NEM"                        },
            { L_,  "NI"                         },
            { L_,  "NKE"                        },
            { L_,  "NOC"                        },
            { L_,  "NOV"                        },
            { L_,  "NOVL"                       },
            { L_,  "NSC"                        },
            { L_,  "NSM"                        },
            { L_,  "NTAP"                       },
            { L_,  "NTRS"                       },
            { L_,  "NUE"                        },
            { L_,  "NVDA"                       },
            { L_,  "NVLS"                       },
            { L_,  "NWL"                        },
            { L_,  "NWS/A"                      },
            { L_,  "NYT"                        },
            { L_,  "NYX"                        },
            { L_,  "ODP"                        },
            { L_,  "OMC"                        },
            { L_,  "ORCL"                       },
            { L_,  "OXY"                        },
            { L_,  "PAYX"                       },
            { L_,  "PBG"                        },
            { L_,  "PBI"                        },
            { L_,  "PCAR"                       },
            { L_,  "PCG"                        },
            { L_,  "PCL"                        },
            { L_,  "PCP"                        },
            { L_,  "PDCO"                       },
            { L_,  "PEG"                        },
            { L_,  "PEP"                        },
            { L_,  "PFE"                        },
            { L_,  "PFG"                        },
            { L_,  "PG"                         },
            { L_,  "PGN"                        },
            { L_,  "PGR"                        },
            { L_,  "PH"                         },
            { L_,  "PHM"                        },
            { L_,  "PKI"                        },
            { L_,  "PLD"                        },
            { L_,  "PLL"                        },
            { L_,  "PM"                         },
            { L_,  "PNC"                        },
            { L_,  "PNW"                        },
            { L_,  "POM"                        },
            { L_,  "PPG"                        },
            { L_,  "PPL"                        },
            { L_,  "PRU"                        },
            { L_,  "PSA"                        },
            { L_,  "PTV"                        },
            { L_,  "PX"                         },
            { L_,  "PXD"                        },
            { L_,  "Q"                          },
            { L_,  "QCOM"                       },
            { L_,  "QLGC"                       },
            { L_,  "R"                          },
            { L_,  "RAI"                        },
            { L_,  "RDC"                        },
            { L_,  "RF"                         },
            { L_,  "RHI"                        },
            { L_,  "RIG"                        },
            { L_,  "RL"                         },
            { L_,  "ROH"                        },
            { L_,  "ROK"                        },
            { L_,  "RRC"                        },
            { L_,  "RRD"                        },
            { L_,  "RSH"                        },
            { L_,  "RTN"                        },
            { L_,  "RX"                         },
            { L_,  "S"                          },
            { L_,  "SBUX"                       },
            { L_,  "SCHW"                       },
            { L_,  "SE"                         },
            { L_,  "SEE"                        },
            { L_,  "SGP"                        },
            { L_,  "SHLD"                       },
            { L_,  "SHW"                        },
            { L_,  "SIAL"                       },
            { L_,  "SII"                        },
            { L_,  "SJM"                        },
            { L_,  "SLB"                        },
            { L_,  "SLE"                        },
            { L_,  "SLM"                        },
            { L_,  "SNA"                        },
            { L_,  "SNDK"                       },
            { L_,  "SNI"                        },
            { L_,  "SO"                         },
            { L_,  "SOV"                        },
            { L_,  "SPG"                        },
            { L_,  "SPLS"                       },
            { L_,  "SRE"                        },
            { L_,  "STI"                        },
            { L_,  "STJ"                        },
            { L_,  "STR"                        },
            { L_,  "STT"                        },
            { L_,  "STZ"                        },
            { L_,  "SUN"                        },
            { L_,  "SVU"                        },
            { L_,  "SWK"                        },
            { L_,  "SWN"                        },
            { L_,  "SWY"                        },
            { L_,  "SYK"                        },
            { L_,  "SYMC"                       },
            { L_,  "SYY"                        },
            { L_,  "T"                          },
            { L_,  "TAP"                        },
            { L_,  "TDC"                        },
            { L_,  "TE"                         },
            { L_,  "TEG"                        },
            { L_,  "TEL"                        },
            { L_,  "TER"                        },
            { L_,  "TGT"                        },
            { L_,  "THC"                        },
            { L_,  "TIE"                        },
            { L_,  "TIF"                        },
            { L_,  "TJX"                        },
            { L_,  "TLAB"                       },
            { L_,  "TMK"                        },
            { L_,  "TMO"                        },
            { L_,  "TROW"                       },
            { L_,  "TRV"                        },
            { L_,  "TSN"                        },
            { L_,  "TSO"                        },
            { L_,  "TSS"                        },
            { L_,  "TWX"                        },
            { L_,  "TXN"                        },
            { L_,  "TXT"                        },
            { L_,  "TYC"                        },
            { L_,  "UNH"                        },
            { L_,  "UNM"                        },
            { L_,  "UNP"                        },
            { L_,  "UPS"                        },
            { L_,  "USB"                        },
            { L_,  "UST"                        },
            { L_,  "UTX"                        },
            { L_,  "VAR"                        },
            { L_,  "VFC"                        },
            { L_,  "VIA/B"                      },
            { L_,  "VLO"                        },
            { L_,  "VMC"                        },
            { L_,  "VNO"                        },
            { L_,  "VRSN"                       },
            { L_,  "VZ"                         },
            { L_,  "WAG"                        },
            { L_,  "WAT"                        },
            { L_,  "WB"                         },
            { L_,  "WEC"                        },
            { L_,  "WFC"                        },
            { L_,  "WFMI"                       },
            { L_,  "WFR"                        },
            { L_,  "WFT"                        },
            { L_,  "WHR"                        },
            { L_,  "WIN"                        },
            { L_,  "WLP"                        },
            { L_,  "WMB"                        },
            { L_,  "WMI"                        },
            { L_,  "WMT"                        },
            { L_,  "WPI"                        },
            { L_,  "WPO"                        },
            { L_,  "WU"                         },
            { L_,  "WY"                         },
            { L_,  "WYE"                        },
            { L_,  "WYN"                        },
            { L_,  "X"                          },
            { L_,  "XEL"                        },
            { L_,  "XL"                         },
            { L_,  "XLNX"                       },
            { L_,  "XOM"                        },
            { L_,  "XRX"                        },
            { L_,  "XTO"                        },
            { L_,  "YHOO"                       },
            { L_,  "YUM"                        },
            { L_,  "ZION"                       },
            { L_,  "ZMH"                        },


            // The full Bloomberg stock tickers of the 499 members of the S&P
            // 500 as reported by {SPX Index MEMB<GO>} on 20081112
            { L_,  "A UN Equity"                },
            { L_,  "AA UN Equity"               },
            { L_,  "AAPL UW Equity"             },
            { L_,  "ABC UN Equity"              },
            { L_,  "ABI UN Equity"              },
            { L_,  "ABT UN Equity"              },
            { L_,  "ACAS UW Equity"             },
            { L_,  "ACS UN Equity"              },
            { L_,  "ADBE UW Equity"             },
            { L_,  "ADI UN Equity"              },
            { L_,  "ADM UN Equity"              },
            { L_,  "ADP UW Equity"              },
            { L_,  "ADSK UW Equity"             },
            { L_,  "AEE UN Equity"              },
            { L_,  "AEP UN Equity"              },
            { L_,  "AES UN Equity"              },
            { L_,  "AET UN Equity"              },
            { L_,  "AFL UN Equity"              },
            { L_,  "AGN UN Equity"              },
            { L_,  "AIG UN Equity"              },
            { L_,  "AIV UN Equity"              },
            { L_,  "AIZ UN Equity"              },
            { L_,  "AKAM UW Equity"             },
            { L_,  "AKS UN Equity"              },
            { L_,  "ALL UN Equity"              },
            { L_,  "ALTR UW Equity"             },
            { L_,  "AMAT UW Equity"             },
            { L_,  "AMD UN Equity"              },
            { L_,  "AMGN UW Equity"             },
            { L_,  "AMP UN Equity"              },
            { L_,  "AMT UN Equity"              },
            { L_,  "AMZN UW Equity"             },
            { L_,  "AN UN Equity"               },
            { L_,  "ANF UN Equity"              },
            { L_,  "AOC UN Equity"              },
            { L_,  "APA UN Equity"              },
            { L_,  "APC UN Equity"              },
            { L_,  "APD UN Equity"              },
            { L_,  "APH UN Equity"              },
            { L_,  "APOL UW Equity"             },
            { L_,  "ASH UN Equity"              },
            { L_,  "ATI UN Equity"              },
            { L_,  "AVB UN Equity"              },
            { L_,  "AVP UN Equity"              },
            { L_,  "AVY UN Equity"              },
            { L_,  "AW UN Equity"               },
            { L_,  "AXP UN Equity"              },
            { L_,  "AYE UN Equity"              },
            { L_,  "AZO UN Equity"              },
            { L_,  "BA UN Equity"               },
            { L_,  "BAC UN Equity"              },
            { L_,  "BAX UN Equity"              },
            { L_,  "BBBY UW Equity"             },
            { L_,  "BBT UN Equity"              },
            { L_,  "BBY UN Equity"              },
            { L_,  "BCR UN Equity"              },
            { L_,  "BDK UN Equity"              },
            { L_,  "BDX UN Equity"              },
            { L_,  "BEN UN Equity"              },
            { L_,  "BF/B UN Equity"             },
            { L_,  "BHI UN Equity"              },
            { L_,  "BIG UN Equity"              },
            { L_,  "BIIB UW Equity"             },
            { L_,  "BJS UN Equity"              },
            { L_,  "BK UN Equity"               },
            { L_,  "BLL UN Equity"              },
            { L_,  "BMC UN Equity"              },
            { L_,  "BMS UN Equity"              },
            { L_,  "BMY UN Equity"              },
            { L_,  "BNI UN Equity"              },
            { L_,  "BRCM UW Equity"             },
            { L_,  "BRL UN Equity"              },
            { L_,  "BSX UN Equity"              },
            { L_,  "BTU UN Equity"              },
            { L_,  "BUD UN Equity"              },
            { L_,  "BXP UN Equity"              },
            { L_,  "C UN Equity"                },
            { L_,  "CA UW Equity"               },
            { L_,  "CAG UN Equity"              },
            { L_,  "CAH UN Equity"              },
            { L_,  "CAM UN Equity"              },
            { L_,  "CAT UN Equity"              },
            { L_,  "CB UN Equity"               },
            { L_,  "CBE UN Equity"              },
            { L_,  "CBG UN Equity"              },
            { L_,  "CBS UN Equity"              },
            { L_,  "CCE UN Equity"              },
            { L_,  "CCL UN Equity"              },
            { L_,  "CEG UN Equity"              },
            { L_,  "CELG UW Equity"             },
            { L_,  "CF UN Equity"               },
            { L_,  "CHK UN Equity"              },
            { L_,  "CHRW UW Equity"             },
            { L_,  "CI UN Equity"               },
            { L_,  "CIEN UW Equity"             },
            { L_,  "CINF UW Equity"             },
            { L_,  "CIT UN Equity"              },
            { L_,  "CL UN Equity"               },
            { L_,  "CLX UN Equity"              },
            { L_,  "CMA UN Equity"              },
            { L_,  "CMCSA UW Equity"            },
            { L_,  "CME UW Equity"              },
            { L_,  "CMI UN Equity"              },
            { L_,  "CMS UN Equity"              },
            { L_,  "CNP UN Equity"              },
            { L_,  "CNX UN Equity"              },
            { L_,  "COF UN Equity"              },
            { L_,  "COG UN Equity"              },
            { L_,  "COH UN Equity"              },
            { L_,  "COL UN Equity"              },
            { L_,  "COP UN Equity"              },
            { L_,  "COST UW Equity"             },
            { L_,  "COV UN Equity"              },
            { L_,  "CPB UN Equity"              },
            { L_,  "CPWR UW Equity"             },
            { L_,  "CRM UN Equity"              },
            { L_,  "CSC UN Equity"              },
            { L_,  "CSCO UW Equity"             },
            { L_,  "CSX UN Equity"              },
            { L_,  "CTAS UW Equity"             },
            { L_,  "CTL UN Equity"              },
            { L_,  "CTSH UW Equity"             },
            { L_,  "CTX UN Equity"              },
            { L_,  "CTXS UW Equity"             },
            { L_,  "CVG UN Equity"              },
            { L_,  "CVH UN Equity"              },
            { L_,  "CVS UN Equity"              },
            { L_,  "CVX UN Equity"              },
            { L_,  "D UN Equity"                },
            { L_,  "DD UN Equity"               },
            { L_,  "DDR UN Equity"              },
            { L_,  "DE UN Equity"               },
            { L_,  "DELL UW Equity"             },
            { L_,  "DF UN Equity"               },
            { L_,  "DFS UN Equity"              },
            { L_,  "DGX UN Equity"              },
            { L_,  "DHI UN Equity"              },
            { L_,  "DHR UN Equity"              },
            { L_,  "DIS UN Equity"              },
            { L_,  "DOV UN Equity"              },
            { L_,  "DOW UN Equity"              },
            { L_,  "DPS UN Equity"              },
            { L_,  "DRI UN Equity"              },
            { L_,  "DTE UN Equity"              },
            { L_,  "DTV UW Equity"              },
            { L_,  "DUK UN Equity"              },
            { L_,  "DVA UN Equity"              },
            { L_,  "DVN UN Equity"              },
            { L_,  "DYN UN Equity"              },
            { L_,  "EBAY UW Equity"             },
            { L_,  "ECL UN Equity"              },
            { L_,  "ED UN Equity"               },
            { L_,  "EFX UN Equity"              },
            { L_,  "EIX UN Equity"              },
            { L_,  "EK UN Equity"               },
            { L_,  "EL UN Equity"               },
            { L_,  "EMC UN Equity"              },
            { L_,  "EMN UN Equity"              },
            { L_,  "EMR UN Equity"              },
            { L_,  "EOG UN Equity"              },
            { L_,  "EP UN Equity"               },
            { L_,  "EQ UN Equity"               },
            { L_,  "EQR UN Equity"              },
            { L_,  "ERTS UW Equity"             },
            { L_,  "ESRX UW Equity"             },
            { L_,  "ESV UN Equity"              },
            { L_,  "ETFC UW Equity"             },
            { L_,  "ETN UN Equity"              },
            { L_,  "ETR UN Equity"              },
            { L_,  "EXC UN Equity"              },
            { L_,  "EXPD UW Equity"             },
            { L_,  "EXPE UW Equity"             },
            { L_,  "F UN Equity"                },
            { L_,  "FAST UW Equity"             },
            { L_,  "FCX UN Equity"              },
            { L_,  "FDO UN Equity"              },
            { L_,  "FDX UN Equity"              },
            { L_,  "FE UN Equity"               },
            { L_,  "FHN UN Equity"              },
            { L_,  "FII UN Equity"              },
            { L_,  "FIS UN Equity"              },
            { L_,  "FISV UW Equity"             },
            { L_,  "FITB UW Equity"             },
            { L_,  "FLR UN Equity"              },
            { L_,  "FLS UN Equity"              },
            { L_,  "FO UN Equity"               },
            { L_,  "FPL UN Equity"              },
            { L_,  "FRX UN Equity"              },
            { L_,  "FTR UN Equity"              },
            { L_,  "GAS UN Equity"              },
            { L_,  "GCI UN Equity"              },
            { L_,  "GD UN Equity"               },
            { L_,  "GE UN Equity"               },
            { L_,  "GENZ UW Equity"             },
            { L_,  "GGP UP Equity"              },
            { L_,  "GILD UW Equity"             },
            { L_,  "GIS UN Equity"              },
            { L_,  "GLW UN Equity"              },
            { L_,  "GM UN Equity"               },
            { L_,  "GME UN Equity"              },
            { L_,  "GNW UN Equity"              },
            { L_,  "GOOG UW Equity"             },
            { L_,  "GPC UN Equity"              },
            { L_,  "GPS UN Equity"              },
            { L_,  "GR UN Equity"               },
            { L_,  "GS UN Equity"               },
            { L_,  "GT UN Equity"               },
            { L_,  "GWW UN Equity"              },
            { L_,  "HAL UN Equity"              },
            { L_,  "HAR UN Equity"              },
            { L_,  "HAS UN Equity"              },
            { L_,  "HBAN UW Equity"             },
            { L_,  "HCBK UW Equity"             },
            { L_,  "HCP UN Equity"              },
            { L_,  "HD UN Equity"               },
            { L_,  "HES UN Equity"              },
            { L_,  "HIG UN Equity"              },
            { L_,  "HNZ UN Equity"              },
            { L_,  "HOG UN Equity"              },
            { L_,  "HON UN Equity"              },
            { L_,  "HOT UN Equity"              },
            { L_,  "HPC UN Equity"              },
            { L_,  "HPQ UN Equity"              },
            { L_,  "HRB UN Equity"              },
            { L_,  "HRS UN Equity"              },
            { L_,  "HSP UN Equity"              },
            { L_,  "HST UN Equity"              },
            { L_,  "HSY UN Equity"              },
            { L_,  "HUM UN Equity"              },
            { L_,  "IBM UN Equity"              },
            { L_,  "ICE UN Equity"              },
            { L_,  "IFF UN Equity"              },
            { L_,  "IGT UN Equity"              },
            { L_,  "INTC UW Equity"             },
            { L_,  "INTU UW Equity"             },
            { L_,  "IP UN Equity"               },
            { L_,  "IPG UN Equity"              },
            { L_,  "IR UN Equity"               },
            { L_,  "ISRG UW Equity"             },
            { L_,  "ITT UN Equity"              },
            { L_,  "ITW UN Equity"              },
            { L_,  "IVZ UN Equity"              },
            { L_,  "JAVA UW Equity"             },
            { L_,  "JBL UN Equity"              },
            { L_,  "JCI UN Equity"              },
            { L_,  "JCP UN Equity"              },
            { L_,  "JDSU UW Equity"             },
            { L_,  "JEC UN Equity"              },
            { L_,  "JNJ UN Equity"              },
            { L_,  "JNPR UW Equity"             },
            { L_,  "JNS UN Equity"              },
            { L_,  "JNY UN Equity"              },
            { L_,  "JPM UN Equity"              },
            { L_,  "JWN UN Equity"              },
            { L_,  "K UN Equity"                },
            { L_,  "KBH UN Equity"              },
            { L_,  "KEY UN Equity"              },
            { L_,  "KFT UN Equity"              },
            { L_,  "KG UN Equity"               },
            { L_,  "KIM UN Equity"              },
            { L_,  "KLAC UW Equity"             },
            { L_,  "KMB UN Equity"              },
            { L_,  "KO UN Equity"               },
            { L_,  "KR UN Equity"               },
            { L_,  "KSS UN Equity"              },
            { L_,  "L UN Equity"                },
            { L_,  "LEG UN Equity"              },
            { L_,  "LEN UN Equity"              },
            { L_,  "LH UN Equity"               },
            { L_,  "LIZ UN Equity"              },
            { L_,  "LLL UN Equity"              },
            { L_,  "LLTC UW Equity"             },
            { L_,  "LLY UN Equity"              },
            { L_,  "LM UN Equity"               },
            { L_,  "LMT UN Equity"              },
            { L_,  "LNC UN Equity"              },
            { L_,  "LO UN Equity"               },
            { L_,  "LOW UN Equity"              },
            { L_,  "LSI UN Equity"              },
            { L_,  "LTD UN Equity"              },
            { L_,  "LUK UN Equity"              },
            { L_,  "LUV UN Equity"              },
            { L_,  "LXK UN Equity"              },
            { L_,  "M UN Equity"                },
            { L_,  "MA UN Equity"               },
            { L_,  "MAR UN Equity"              },
            { L_,  "MAS UN Equity"              },
            { L_,  "MAT UN Equity"              },
            { L_,  "MBI UN Equity"              },
            { L_,  "MCD UN Equity"              },
            { L_,  "MCHP UW Equity"             },
            { L_,  "MCK UN Equity"              },
            { L_,  "MCO UN Equity"              },
            { L_,  "MDP UN Equity"              },
            { L_,  "MDT UN Equity"              },
            { L_,  "MEE UN Equity"              },
            { L_,  "MER UN Equity"              },
            { L_,  "MET UN Equity"              },
            { L_,  "MHP UN Equity"              },
            { L_,  "MHS UN Equity"              },
            { L_,  "MI UN Equity"               },
            { L_,  "MIL UN Equity"              },
            { L_,  "MKC UN Equity"              },
            { L_,  "MMC UN Equity"              },
            { L_,  "MMM UN Equity"              },
            { L_,  "MO UN Equity"               },
            { L_,  "MOLX UW Equity"             },
            { L_,  "MON UN Equity"              },
            { L_,  "MOT UN Equity"              },
            { L_,  "MRK UN Equity"              },
            { L_,  "MRO UN Equity"              },
            { L_,  "MS UN Equity"               },
            { L_,  "MSFT UW Equity"             },
            { L_,  "MTB UN Equity"              },
            { L_,  "MTW UN Equity"              },
            { L_,  "MU UN Equity"               },
            { L_,  "MUR UN Equity"              },
            { L_,  "MWV UN Equity"              },
            { L_,  "MWW UN Equity"              },
            { L_,  "MYL UN Equity"              },
            { L_,  "NBL UN Equity"              },
            { L_,  "NBR UN Equity"              },
            { L_,  "NCC UN Equity"              },
            { L_,  "NDAQ UW Equity"             },
            { L_,  "NE UN Equity"               },
            { L_,  "NEM UN Equity"              },
            { L_,  "NI UN Equity"               },
            { L_,  "NKE UN Equity"              },
            { L_,  "NOC UN Equity"              },
            { L_,  "NOV UN Equity"              },
            { L_,  "NOVL UW Equity"             },
            { L_,  "NSC UN Equity"              },
            { L_,  "NSM UN Equity"              },
            { L_,  "NTAP UW Equity"             },
            { L_,  "NTRS UW Equity"             },
            { L_,  "NUE UN Equity"              },
            { L_,  "NVDA UW Equity"             },
            { L_,  "NVLS UW Equity"             },
            { L_,  "NWL UN Equity"              },
            { L_,  "NWS/A UN Equity"            },
            { L_,  "NYT UN Equity"              },
            { L_,  "NYX UN Equity"              },
            { L_,  "ODP UN Equity"              },
            { L_,  "OMC UN Equity"              },
            { L_,  "ORCL UW Equity"             },
            { L_,  "OXY UN Equity"              },
            { L_,  "PAYX UW Equity"             },
            { L_,  "PBG UN Equity"              },
            { L_,  "PBI UN Equity"              },
            { L_,  "PCAR UW Equity"             },
            { L_,  "PCG UN Equity"              },
            { L_,  "PCL UN Equity"              },
            { L_,  "PCP UN Equity"              },
            { L_,  "PDCO UW Equity"             },
            { L_,  "PEG UN Equity"              },
            { L_,  "PEP UN Equity"              },
            { L_,  "PFE UN Equity"              },
            { L_,  "PFG UN Equity"              },
            { L_,  "PG UN Equity"               },
            { L_,  "PGN UN Equity"              },
            { L_,  "PGR UN Equity"              },
            { L_,  "PH UN Equity"               },
            { L_,  "PHM UN Equity"              },
            { L_,  "PKI UN Equity"              },
            { L_,  "PLD UN Equity"              },
            { L_,  "PLL UN Equity"              },
            { L_,  "PM UN Equity"               },
            { L_,  "PNC UN Equity"              },
            { L_,  "PNW UN Equity"              },
            { L_,  "POM UN Equity"              },
            { L_,  "PPG UN Equity"              },
            { L_,  "PPL UN Equity"              },
            { L_,  "PRU UN Equity"              },
            { L_,  "PSA UN Equity"              },
            { L_,  "PTV UN Equity"              },
            { L_,  "PX UN Equity"               },
            { L_,  "PXD UN Equity"              },
            { L_,  "Q UN Equity"                },
            { L_,  "QCOM UW Equity"             },
            { L_,  "QLGC UW Equity"             },
            { L_,  "R UN Equity"                },
            { L_,  "RAI UN Equity"              },
            { L_,  "RDC UN Equity"              },
            { L_,  "RF UN Equity"               },
            { L_,  "RHI UN Equity"              },
            { L_,  "RIG UN Equity"              },
            { L_,  "RL UN Equity"               },
            { L_,  "ROH UN Equity"              },
            { L_,  "ROK UN Equity"              },
            { L_,  "RRC UN Equity"              },
            { L_,  "RRD UN Equity"              },
            { L_,  "RSH UN Equity"              },
            { L_,  "RTN UN Equity"              },
            { L_,  "RX UN Equity"               },
            { L_,  "S UN Equity"                },
            { L_,  "SBUX UW Equity"             },
            { L_,  "SCHW UW Equity"             },
            { L_,  "SE UN Equity"               },
            { L_,  "SEE UN Equity"              },
            { L_,  "SGP UN Equity"              },
            { L_,  "SHLD UW Equity"             },
            { L_,  "SHW UN Equity"              },
            { L_,  "SIAL UW Equity"             },
            { L_,  "SII UN Equity"              },
            { L_,  "SJM UN Equity"              },
            { L_,  "SLB UN Equity"              },
            { L_,  "SLE UN Equity"              },
            { L_,  "SLM UN Equity"              },
            { L_,  "SNA UN Equity"              },
            { L_,  "SNDK UW Equity"             },
            { L_,  "SNI UN Equity"              },
            { L_,  "SO UN Equity"               },
            { L_,  "SOV UN Equity"              },
            { L_,  "SPG UN Equity"              },
            { L_,  "SPLS UW Equity"             },
            { L_,  "SRE UN Equity"              },
            { L_,  "STI UN Equity"              },
            { L_,  "STJ UN Equity"              },
            { L_,  "STR UN Equity"              },
            { L_,  "STT UN Equity"              },
            { L_,  "STZ UN Equity"              },
            { L_,  "SUN UN Equity"              },
            { L_,  "SVU UN Equity"              },
            { L_,  "SWK UN Equity"              },
            { L_,  "SWN UN Equity"              },
            { L_,  "SWY UN Equity"              },
            { L_,  "SYK UN Equity"              },
            { L_,  "SYMC UW Equity"             },
            { L_,  "SYY UN Equity"              },
            { L_,  "T UN Equity"                },
            { L_,  "TAP UN Equity"              },
            { L_,  "TDC UN Equity"              },
            { L_,  "TE UN Equity"               },
            { L_,  "TEG UN Equity"              },
            { L_,  "TEL UN Equity"              },
            { L_,  "TER UN Equity"              },
            { L_,  "TGT UN Equity"              },
            { L_,  "THC UN Equity"              },
            { L_,  "TIE UN Equity"              },
            { L_,  "TIF UN Equity"              },
            { L_,  "TJX UN Equity"              },
            { L_,  "TLAB UW Equity"             },
            { L_,  "TMK UN Equity"              },
            { L_,  "TMO UN Equity"              },
            { L_,  "TROW UW Equity"             },
            { L_,  "TRV UN Equity"              },
            { L_,  "TSN UN Equity"              },
            { L_,  "TSO UN Equity"              },
            { L_,  "TSS UN Equity"              },
            { L_,  "TWX UN Equity"              },
            { L_,  "TXN UN Equity"              },
            { L_,  "TXT UN Equity"              },
            { L_,  "TYC UN Equity"              },
            { L_,  "UNH UN Equity"              },
            { L_,  "UNM UN Equity"              },
            { L_,  "UNP UN Equity"              },
            { L_,  "UPS UN Equity"              },
            { L_,  "USB UN Equity"              },
            { L_,  "UST UN Equity"              },
            { L_,  "UTX UN Equity"              },
            { L_,  "VAR UN Equity"              },
            { L_,  "VFC UN Equity"              },
            { L_,  "VIA/B UN Equity"            },
            { L_,  "VLO UN Equity"              },
            { L_,  "VMC UN Equity"              },
            { L_,  "VNO UN Equity"              },
            { L_,  "VRSN UW Equity"             },
            { L_,  "VZ UN Equity"               },
            { L_,  "WAG UN Equity"              },
            { L_,  "WAT UN Equity"              },
            { L_,  "WB UN Equity"               },
            { L_,  "WEC UN Equity"              },
            { L_,  "WFC UN Equity"              },
            { L_,  "WFMI UW Equity"             },
            { L_,  "WFR UN Equity"              },
            { L_,  "WFT UN Equity"              },
            { L_,  "WHR UN Equity"              },
            { L_,  "WIN UN Equity"              },
            { L_,  "WLP UN Equity"              },
            { L_,  "WMB UN Equity"              },
            { L_,  "WMI UN Equity"              },
            { L_,  "WMT UN Equity"              },
            { L_,  "WPI UN Equity"              },
            { L_,  "WPO UN Equity"              },
            { L_,  "WU UN Equity"               },
            { L_,  "WY UN Equity"               },
            { L_,  "WYE UN Equity"              },
            { L_,  "WYN UN Equity"              },
            { L_,  "X UN Equity"                },
            { L_,  "XEL UN Equity"              },
            { L_,  "XL UN Equity"               },
            { L_,  "XLNX UW Equity"             },
            { L_,  "XOM UN Equity"              },
            { L_,  "XRX UN Equity"              },
            { L_,  "XTO UN Equity"              },
            { L_,  "YHOO UW Equity"             },
            { L_,  "YUM UN Equity"              },
            { L_,  "ZION UW Equity"             },
            { L_,  "ZMH UN Equity"              },
            // Some data only differing by the final element to ensure the
            // whole string is hashed
            { L_,  "AAAAAAAAAAAAAAAAAAAAAAAA"   },
            { L_,  "AAAAAAAAAAAAAAAAAAAAAAAB"   },
            { L_,  "AAAAAAAAAAAAAAAAAAAAAAAC"   },
            { L_,  "AAAAAAAAAAAAAAAAAAAAAAAD"   },
            { L_,  "AAAAAAAAAAAAAAAAAAAAAAAE"   },
            { L_,  "AAAAAAAAAAAAAAAAAAAAAAAF"   },
            { L_,  "AAAAAAAAAAAAAAAAAAAAAAAG"   },
            { L_,  "AAAAAAAAAAAAAAAAAAAAAAAH"   },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        std::map<Obj, std::size_t> hash_results;
        std::map<std::size_t, int> hash_value_counts;

        bsl::hash<Obj>        bsl_hash_function;
        bslh::Hash<>          bslh_hash_function;

        // Capture all the hash values.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE         = DATA[ti].d_line;
            const char *STR          = DATA[ti].d_str;
            Obj o(STR);

            std::size_t hash_value =
                                    static_cast<size_t>(bslh_hash_function(o));
            std::size_t bsl_hash_value =
                                     static_cast<size_t>(bsl_hash_function(o));

            // Ensure bslh::Hash and bsl::hash produce the same value
            ASSERT(hash_value == bsl_hash_value);

            if (veryVerbose) {
                printf("%4d: STR=%-20s, HASH=" ZU "\n",LINE, STR, hash_value);
            }

            hash_results[o] = hash_value;
            hash_value_counts[hash_value]++;
        }

        // Repeat all hashes in reverse order, making sure we get the same
        // values as last time. Copy the data to ensure we are hashing the same
        // data from different memory locations, ensuring that we get the same
        // hash even when the data is stored elsewhere (will also spot if we
        // are hashing beyond the end of the string).
        for (int ti = NUM_DATA - 1; ti >= 0; --ti) {
            const int   LINE         = DATA[ti].d_line;
            const char *STR          = DATA[ti].d_str;
            char        strCopy [40];
            Obj o(strcpy(strCopy, STR));

            std::size_t hash_value = bslh_hash_function(o);
            LOOP_ASSERT(LINE, hash_results[o] == hash_value);
        }

        // Make sure that no particular hash value occurred more than twice.
        //
        std::map<std::size_t, int>::iterator i   = hash_value_counts.begin();
        std::map<std::size_t, int>::iterator end = hash_value_counts.end();
        for (; i != end; ++i) {
            if (verbose && i->second > 1) {
                std::cout << "Hash value "
                          << std::hex << i->first << std::dec
                          << " occurs "
                          << i->second
                          << " time(s)"
                          << std::endl;
            }

            ASSERT(3 > i->second);
        }
      } break;

      case 7: {
        // --------------------------------------------------------------------
        // TESTING ADDITION OPERATORS:
        //
        // Concerns:
        //   Each function must return the expected value (consistent with its
        //   nominal primitive-operator counterpart).
        //
        // Plan:
        //   Specify a set of strings and the assert addition operators
        //   return the correct results. The the basic_string operator+= is
        //   being tested here becuase the bslstl_string test driver can not
        //   test using StringRef without introducing cyclic dependencies
        //
        // Testing:
        //   int operator+(const StringRef& lhs, const StringRef& rhs);
        //   int operator+(const bsl::string& lhs, const StringRef& rhs);
        //   int operator+(const StringRef& lhs, const bsl::string& rhs);
        //   int operator+(const native_std::string& l, const StringRef& r);
        //   int operator+(const StringRef& l, const native_std::string& r);
        //   int operator+(const char *lhs, const StringRef& rhs);
        //   int operator+(const StringRef& lhs, const char *rhs);
        //   int operator+(const StringRef& lhs, const StringRef& rhs);
        //   basic_string basic_string::operator+=(const StringRefData& strRf);
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nTESTING ADDITION OPERATORS"
                               << "\n=========================="
                               << std::endl;

        static const struct {
            int         d_line;
            const char *d_ca1;  // char array 1
            const char *d_ca2;  // char array 2
            const char *d_result;
        } DATA[] = {
            //line string1 string2 result
            //---- ------- ------- ------
            { L_,    ""  ,   ""  , ""     },
            { L_,    " " ,   " " , "  "   },
            { L_,    "a" ,   ""  , "a"    },
            { L_,    "a" ,   " " , "a "   },
            { L_,    ""  ,   "a" , "a"    },
            { L_,    " " ,   "a" , " a"   },
            { L_,    "a" ,   "a" , "aa"   },
            { L_,    "a" ,   "b" , "ab"   },
            { L_,    "b" ,   "a" , "ba"   },
            { L_,    "ab",   ""  , "ab"   },
            { L_,    "ab",   " " , "ab "  },
            { L_,    "ab",   "c" , "abc"  },
            { L_,    "a" ,   "bc", "abc"  },
            { L_,    "ab",   "cd", "abcd" }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE         = DATA[ti].d_line;
            const char *CA1          = DATA[ti].d_ca1;
            const char *CA2          = DATA[ti].d_ca2;
            const bsl::string RESULT = bsl::string(DATA[ti].d_result);

            if (veryVerbose) {
                std::cout << "\nRESULT  = \"" << RESULT  << "\"" << std::endl;
            }

            Obj x1(CA1);  const Obj& X1 = x1;
            Obj x2(CA2);  const Obj& X2 = x2;

            if (veryVerbose) {
              std::cout << "\tX1 = \"" << X1 << "\", "
                        << "X2 = \"" << X2 << "\", " << std::endl;
            }

            // StringRef versus StringRef
            LOOP_ASSERT(LINE, RESULT == (X1  + X2));

            if (veryVerbose) {
              std::cout << "\tCA1 = \"" << CA1 << "\", "
                        << "CA2 = \"" << CA2 << "\", " << std::endl;
            }

            // char * versus StringRef
            LOOP_ASSERT(LINE, RESULT == (CA1 + X2));

            // StringRef versus char *
            LOOP_ASSERT(LINE, RESULT == (X1  + CA2));

            bsl::string s1(CA1);  const bsl::string& S1 = s1;
            bsl::string s2(CA2);  const bsl::string& S2 = s2;

            if (veryVerbose) {
              std::cout << "\tS1 = \"" << S1 << "\", "
                        << "S2 = \"" << S2 << "\", " << std::endl;
            }

            // bsl::string versus StringRef
            LOOP_ASSERT(LINE, RESULT == (S1  + X2));

            // StringRef versus bsl::string
            LOOP_ASSERT(LINE, RESULT == (X1  + S2));

            native_std::string s3(CA1);  const native_std::string& S3 = s3;
            native_std::string s4(CA2);  const native_std::string& S4 = s4;

            if (veryVerbose) {
              std::cout << "\tS3 = \"" << S3 << "\", "
                        << "S4 = \"" << S4 << "\", " << std::endl;
            }

            // 'native_std::string' versus StringRef
            LOOP_ASSERT(LINE, RESULT == (S3  + X2));

            // StringRef versus 'native_std::string'
            LOOP_ASSERT(LINE, RESULT == (X1  + S4));

            // 'bsl::string' versus 'bsl::string'
            // This test is to ensure no overloading ambiguity was introduced.
            LOOP_ASSERT(LINE, RESULT == (S1  + S2));

            // 'native_std::string' versus 'bsl::string'
            // This test is to ensure no overloading ambiguity was introduced.
            LOOP_ASSERT(LINE, RESULT == (S3  + S4));

            // bsl::string with StringRef concatenated on. See comments at top
            // of section for explanation
            // Ensure += returns correctly
            LOOP_ASSERT(LINE, RESULT == (s1 += X2));
            // Ensure += left operand has proper value afterwards
            LOOP_ASSERT(LINE, RESULT == s1);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS:
        //
        // Concerns:
        //   The begin and end members must be correctly set by the different
        //   assignment manipulators.
        //
        // Plan:
        //   Test each of the assignment functions with empty and non-empty
        //   strings.
        //
        // Testing:
        // void reset();
        // void assign(const char *begin, const char *end);
        // void assign(const char *begin, int length);
        // void assign(const char *begin);
        // void assign(const bsl::string& begin);
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nTESTING MANIPULATORS ACCESSORS"
                               << "\n=============================="
                               << std::endl;

        if (verbose) std::cout << "\nTesting:\n\t'reset()'\n\t'assign()'"
                               << "\n= = = = = = = = = = = ="
                               << std::endl;
        if (veryVerbose)
            std::cout << "\nreset()"
                      << "\n=  =  ="
                      << std::endl;
        {
          // Empty string (default constructor)
          Obj es1;  const Obj& ES1 = es1;

          ASSERT(ES1.isEmpty());
          ASSERT(ES1.length()  == 0);
          ASSERT(ES1.begin()   == ES1.end());

          es1.reset();

          ASSERT(ES1.isEmpty());
          ASSERT(ES1.length()  == 0);
          ASSERT(ES1.begin()   == ES1.end());

          // Empty string
          Obj es2(EMPTY_STRING);  const Obj& ES2 = es2;

          ASSERT(ES2.isEmpty());
          ASSERT(ES2.length()  == 0);
          ASSERT(ES2.begin()   == ES2.end());

          es2.reset();

          ASSERT(ES2.isEmpty());
          ASSERT(ES2.length()  == 0);
          ASSERT(ES2.begin()   == ES2.end());

          // Non-empty string
          Obj nes(NON_EMPTY_STRING);  const Obj& NES = nes;

          ASSERT(!NES.isEmpty());
          ASSERT(NES.length()  != 0);
          ASSERT(NES.begin()   != NES.end());

          nes.reset();

          ASSERT(NES.isEmpty());
          ASSERT(NES.length()  == 0);
          ASSERT(NES.begin()   == NES.end());

          // Non-empty sub-string
          Obj ness(NON_EMPTY_STRING + 2, NON_EMPTY_STRING + 6);
          const Obj& NESS = ness;

          ASSERT(!NESS.isEmpty());
          ASSERT(NESS.length()  != 0);
          ASSERT(NESS.begin()   != NESS.end());

          ness.reset();

          ASSERT(NESS.isEmpty());
          ASSERT(NESS.length()  == 0);
          ASSERT(NESS.begin()   == NESS.end());
        }

        if (verbose) std::cout << "\nTesting assign functions"
                               << "\n= = = = = = = = = = = = " << std::endl;
        if (veryVerbose)
            std::cout << "\nassign(const char *begin, const char *end)"
                      << "\n=  =  =  =  =  =  =  =  =  =  =  =  =  =  "
                      << std::endl;
        {
          // Empty string to non-empty string
          Obj x1(EMPTY_STRING, EMPTY_STRING + std::strlen(EMPTY_STRING));
          const Obj& X1 = x1;
          ASSERT(X1.isEmpty());
          ASSERT(X1.length()  == 0);
          ASSERT(X1.begin()   == X1.end());
          ASSERT(X1.begin()   == EMPTY_STRING);
          ASSERT(X1.end()     == EMPTY_STRING + std::strlen(EMPTY_STRING));

          x1.assign(NON_EMPTY_STRING,
                    NON_EMPTY_STRING + std::strlen(NON_EMPTY_STRING));

          ASSERT(!X1.isEmpty());
          ASSERT(X1.length()  != 0);
          ASSERT(X1.begin()   != X1.end());
          ASSERT(X1.begin()   == NON_EMPTY_STRING);
          ASSERT(X1.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));

          // Non-empty string to non-empty sub-string
          Obj x2(NON_EMPTY_STRING,
                 NON_EMPTY_STRING + std::strlen(NON_EMPTY_STRING));
          const Obj& X2 = x2;
          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  != 0);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(X2.begin()   == NON_EMPTY_STRING);
          ASSERT(X2.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));

          x2.assign(NON_EMPTY_STRING + 2,
                    NON_EMPTY_STRING + 6);

          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  != 0);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(X2.begin()   == NON_EMPTY_STRING + 2);
          ASSERT(X2.end()     == NON_EMPTY_STRING + 6);
        }

        if (veryVerbose)
            std::cout << "\nassign(const char *begin, int length)"
                      << "\n=  =  =  =  =  =  =  =  =  =  =  =  ="
                      << std::endl;
        {
          // Empty string to non-empty string
          Obj x1(EMPTY_STRING, EMPTY_STRING + std::strlen(EMPTY_STRING));
          const Obj& X1 = x1;
          ASSERT(X1.isEmpty());
          ASSERT(X1.length()  == 0);
          ASSERT(X1.begin()   == X1.end());
          ASSERT(X1.begin()   == EMPTY_STRING);
          ASSERT(X1.end()     == EMPTY_STRING + std::strlen(EMPTY_STRING));

          x1.assign(NON_EMPTY_STRING,
                    static_cast<int>(std::strlen(NON_EMPTY_STRING)));

          ASSERT(!X1.isEmpty());
          ASSERT(X1.length()  != 0);
          ASSERT(X1.begin()   != X1.end());
          ASSERT(X1.begin()   == NON_EMPTY_STRING);
          ASSERT(X1.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));

          // Non-empty string to non-empty sub-string
          Obj x2(NON_EMPTY_STRING,
                 NON_EMPTY_STRING + std::strlen(NON_EMPTY_STRING));
          const Obj& X2 = x2;
          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  != 0);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(X2.begin()   == NON_EMPTY_STRING);
          ASSERT(X2.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));

          x2.assign(NON_EMPTY_STRING + 2, 4);

          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  != 0);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(X2.begin()   == NON_EMPTY_STRING + 2);
          ASSERT(X2.end()     == NON_EMPTY_STRING + 6);
        }

        if (veryVerbose)
            std::cout << "\nassign(const char *begin)"
                      << "\n=  =  =  =  =  =  =  =  ="
                      << std::endl;
        {
          // Empty string to Null string
          Obj x1(EMPTY_STRING);
          const Obj& X1 = x1;
          ASSERT(X1.isEmpty());
          ASSERT(X1.length()  == 0);
          ASSERT(X1.begin()   == X1.end());
          ASSERT(X1.begin()   == EMPTY_STRING);
          ASSERT(X1.end()     == EMPTY_STRING + std::strlen(EMPTY_STRING));

          // non-empty string to Null string
          Obj x2(NON_EMPTY_STRING);
          const Obj& X2 = x2;
          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  != 0);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(X2.begin()   == NON_EMPTY_STRING);
          ASSERT(X2.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));

          // Empty string to non-empty string
          Obj x3(EMPTY_STRING, EMPTY_STRING + std::strlen(EMPTY_STRING));
          const Obj& X3 = x3;
          ASSERT(X3.isEmpty());
          ASSERT(X3.length()  == 0);
          ASSERT(X3.begin()   == X3.end());
          ASSERT(X3.begin()   == EMPTY_STRING);
          ASSERT(X3.end()     == EMPTY_STRING + std::strlen(EMPTY_STRING));

          x3.assign(NON_EMPTY_STRING);

          ASSERT(!X3.isEmpty());
          ASSERT(X3.length()  != 0);
          ASSERT(X3.begin()   != X3.end());
          ASSERT(X3.begin()   == NON_EMPTY_STRING);
          ASSERT(X3.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));

          // Non-empty string to non-empty sub-string
          Obj x4(NON_EMPTY_STRING,
                 NON_EMPTY_STRING + std::strlen(NON_EMPTY_STRING));
          const Obj& X4 = x4;
          ASSERT(!X4.isEmpty());
          ASSERT(X4.length()  != 0);
          ASSERT(X4.begin()   != X4.end());
          ASSERT(X4.begin()   == NON_EMPTY_STRING);
          ASSERT(X4.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));

          x4.assign(NON_EMPTY_STRING + 2);

          ASSERT(!X4.isEmpty());
          ASSERT(X4.length()  != 0);
          ASSERT(X4.begin()   != X4.end());
          ASSERT(X4.begin()   == NON_EMPTY_STRING + 2);
          ASSERT(X4.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));
        }

        if (veryVerbose)
            std::cout << "\nassign(const bsl::string& begin)"
                      << "\n=  =  =  =  =  =  =  =  =  =  = "
                      << std::endl;
        {
          // Empty string to non-empty string
          Obj x1(EMPTY_STRING, EMPTY_STRING + std::strlen(EMPTY_STRING));
          const Obj& X1 = x1;
          ASSERT(X1.isEmpty());
          ASSERT(X1.length()  == 0);
          ASSERT(X1.begin()   == X1.end());
          ASSERT(X1.begin()   == EMPTY_STRING);
          ASSERT(X1.end()     == EMPTY_STRING + std::strlen(EMPTY_STRING));

          const bsl::string S1(NON_EMPTY_STRING);
          x1.assign(S1);

          ASSERT(!X1.isEmpty());
          ASSERT(X1.length()    != 0);
          ASSERT(X1.begin()     != X1.end());
          ASSERT(&*X1.begin()   == &*S1.begin());
          ASSERT((&*X1.begin() + (X1.end() - X1.begin())) ==
                 (&*S1.begin() + (S1.end() - S1.begin())));

          // Non-empty string to non-empty sub-string
          Obj x2(NON_EMPTY_STRING,
                 NON_EMPTY_STRING + std::strlen(NON_EMPTY_STRING));
          const Obj& X2 = x2;
          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  != 0);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(X2.begin()   == NON_EMPTY_STRING);
          ASSERT(X2.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));

          const bsl::string S2(NON_EMPTY_STRING + 2);
          x2.assign(S2);

          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()   != 0);
          ASSERT(X2.begin()    != X2.end());
          ASSERT(&*X2.begin()   == &*S2.begin());
          ASSERT((&*X2.begin() + (X2.end() - X2.begin())) ==
                 (&*S2.begin() + (S2.end() - S2.begin())));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COMPARISON OPERATORS:
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the comparison
        //   operators.
        //
        // Plan:
        //   Specify a set of strings and the assert comparison operators
        //   return the correct results.
        //
        // Testing:
        //   bool operator==(const StringRef& lhs, const StringRef& rhs);
        //   bool operator==(const bsl::string& lhs, const StringRef& rhs);
        //   bool operator==(const StringRef& lhs, const bsl::string& rhs);
        //   bool operator==(const native_std::string& l, const StringRef& r);
        //   bool operator==(const StringRef& l, const native_std::string& r);
        //   bool operator==(const char *lhs, const StringRef& rhs);
        //   bool operator==(const StringRef& lhs, const char *rhs);
        //   bool operator!=(const StringRef& lhs, const StringRef& rhs);
        //   bool operator!=(const bsl::string& lhs, const StringRef& rhs);
        //   bool operator!=(const StringRef& lhs, const bsl::string& rhs);
        //   bool operator!=(const native_std::string& l, const StringRef& r);
        //   bool operator!=(const StringRef& l, const native_std::string& r);
        //   bool operator!=(const char *lhs, const StringRef& rhs);
        //   bool operator!=(const StringRef& lhs, const char *rhs);
        //   bool operator<(const StringRef& lhs, const StringRef& rhs);
        //   bool operator<(const bsl::string& lhs, const StringRef& rhs);
        //   bool operator<(const StringRef& lhs, const bsl::string& rhs);
        //   bool operator<(const native_std::string& l, const StringRef& r);
        //   bool operator<(const StringRef& l, const native_std::string& r);
        //   bool operator<(const char *lhs, const StringRef& rhs);
        //   bool operator<(const StringRef& lhs, const char *rhs);
        //   bool operator>(const StringRef& lhs, const StringRef& rhs);
        //   bool operator>(const bsl::string& lhs, const StringRef& rhs);
        //   bool operator>(const StringRef& lhs, const bsl::string& rhs);
        //   bool operator>(const native_std::string& l, const StringRef& r);
        //   bool operator>(const StringRef& l, const native_std::string& r);
        //   bool operator>(const char *lhs, const StringRef& rhs);
        //   bool operator>(const StringRef& lhs, const char *rhs);
        //   bool operator<=(const StringRef& lhs, const StringRef& rhs);
        //   bool operator<=(const bsl::string& lhs, const StringRef& rhs);
        //   bool operator<=(const StringRef& lhs, const bsl::string& rhs);
        //   bool operator<=(const native_std::string& l, const StringRef& r);
        //   bool operator<=(const StringRef& l, const native_std::string& r);
        //   bool operator<=(const char *lhs, const StringRef& rhs);
        //   bool operator<=(const StringRef& lhs, const char *rhs);
        //   bool operator>=(const StringRef& lhs, const StringRef& rhs);
        //   bool operator>=(const bsl::string& lhs, const StringRef& rhs);
        //   bool operator>=(const StringRef& lhs, const bsl::string& rhs);
        //   bool operator>=(const native_std::string& l, const StringRef& r);
        //   bool operator>=(const StringRef& l, const native_std::string& r);
        //   bool operator>=(const char *lhs, const StringRef& rhs);
        //   bool operator>=(const StringRef& lhs, const char *rhs);
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nTesting Comparison Operators"
                               << "\n============================"
                               << std::endl;

        static const struct {
            int         d_line;
            const char *d_ca1;  // char array
            const char *d_ca2;  // char array
            bool        d_lt;
            bool        d_gt;
            bool        d_lteq;
            bool        d_gteq;
            bool        d_eq;
        } DATA[] = {
            //line string1 string2   lt     gt    lteq   gteq
            //---- ------- ------- ------ ------ ------ ------
            { L_,    ""  ,   ""  , false, false, true , true  , true  },
            { L_,    " " ,   " " , false, false, true , true  , true  },
            { L_,    "a" ,   ""  , false, true , false, true  , false },
            { L_,    "a" ,   " " , false, true , false, true  , false },
            { L_,    ""  ,   "a" , true , false, true , false , false },
            { L_,    " " ,   "a" , true , false, true , false , false },
            { L_,    "a" ,   "a" , false, false, true , true  , true  },
            { L_,    "a" ,   "b" , true , false, true , false , false },
            { L_,    "b" ,   "a" , false, true , false, true  , false },
            { L_,    "ab",   ""  , false, true , false, true  , false },
            { L_,    "ab",   " " , false, true , false, true  , false },
            { L_,    "ab",   "a" , false, true , false, true  , false },
            { L_,    "a" ,   "ab", true , false, true,  false , false },
            { L_,    "ab",   "ab", false, false, true,  true  , true  }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE = DATA[ti].d_line;
            const char *CA1   = DATA[ti].d_ca1;
            const char *CA2   = DATA[ti].d_ca2;
            const bool  LT   = DATA[ti].d_lt;
            const bool  GT   = DATA[ti].d_gt;
            const bool  LTEQ = DATA[ti].d_lteq;
            const bool  GTEQ = DATA[ti].d_gteq;
            const bool  EQ   = DATA[ti].d_eq;

            if (veryVerbose) {
              std::cout << std::endl;
              P_(LT) P_(GT) P_(LTEQ) P_(GTEQ) P(EQ);
            }

            Obj x1(CA1);  const Obj& X1 = x1;
            Obj x2(CA2);  const Obj& X2 = x2;

            if (veryVerbose) {
              std::cout << "\tX1 = \"" << X1 << "\", "
                        << "X2 = \"" << X2 << "\", " << std::endl;
            }

            // StringRef versus StringRef
            LOOP_ASSERT(LINE, LT   == (X1 <  X2));
            LOOP_ASSERT(LINE, GT   == (X1 >  X2));
            LOOP_ASSERT(LINE, LTEQ == (X1 <= X2));
            LOOP_ASSERT(LINE, GTEQ == (X1 >= X2));
            LOOP_ASSERT(LINE, EQ   == (X1 == X2));
            LOOP_ASSERT(LINE, EQ   != (X1 != X2));

            if (veryVerbose) {
              std::cout << "\tCA1 = \"" << CA1 << "\", "
                        << "CA2 = \"" << CA2 << "\", " << std::endl;
            }

            // char * versus StringRef
            LOOP_ASSERT(LINE, LT   == (CA1 <  X2));
            LOOP_ASSERT(LINE, GT   == (CA1 >  X2));
            LOOP_ASSERT(LINE, LTEQ == (CA1 <= X2));
            LOOP_ASSERT(LINE, GTEQ == (CA1 >= X2));
            LOOP_ASSERT(LINE, EQ   == (CA1 == X2));
            LOOP_ASSERT(LINE, EQ   != (CA1 != X2));

            // StringRef versus char *
            LOOP_ASSERT(LINE, LT   == (X1 <  CA2));
            LOOP_ASSERT(LINE, GT   == (X1 >  CA2));
            LOOP_ASSERT(LINE, LTEQ == (X1 <= CA2));
            LOOP_ASSERT(LINE, GTEQ == (X1 >= CA2));
            LOOP_ASSERT(LINE, EQ   == (X1 == CA2));
            LOOP_ASSERT(LINE, EQ   != (X1 != CA2));

            bsl::string s1(CA1);  const bsl::string& S1 = s1;
            bsl::string s2(CA2);  const bsl::string& S2 = s2;

            if (veryVerbose) {
              std::cout << "\tS1 = \"" << S1 << "\", "
                        << "S2 = \"" << S2 << "\", " << std::endl;
            }

            // bsl::string versus StringRef
            LOOP_ASSERT(LINE, LT   == (S1 <  X2));
            LOOP_ASSERT(LINE, GT   == (S1 >  X2));
            LOOP_ASSERT(LINE, LTEQ == (S1 <= X2));
            LOOP_ASSERT(LINE, GTEQ == (S1 >= X2));
            LOOP_ASSERT(LINE, EQ   == (S1 == X2));
            LOOP_ASSERT(LINE, EQ   != (S1 != X2));

            // StringRef versus bsl::string
            LOOP_ASSERT(LINE, LT   == (X1 <  S2));
            LOOP_ASSERT(LINE, GT   == (X1 >  S2));
            LOOP_ASSERT(LINE, LTEQ == (X1 <= S2));
            LOOP_ASSERT(LINE, GTEQ == (X1 >= S2));
            LOOP_ASSERT(LINE, EQ   == (X1 == S2));
            LOOP_ASSERT(LINE, EQ   != (X1 != S2));

            native_std::string s3(CA1);  const native_std::string& S3 = s3;
            native_std::string s4(CA2);  const native_std::string& S4 = s4;

            if (veryVerbose) {
              std::cout << "\tS3 = \"" << S3 << "\", "
                        << "S4 = \"" << S4 << "\", " << std::endl;
            }

            // 'native_std::string' versus StringRef
            LOOP_ASSERT(LINE, LT   == (S3 <  X2));
            LOOP_ASSERT(LINE, GT   == (S3 >  X2));
            LOOP_ASSERT(LINE, LTEQ == (S3 <= X2));
            LOOP_ASSERT(LINE, GTEQ == (S3 >= X2));
            LOOP_ASSERT(LINE, EQ   == (S3 == X2));
            LOOP_ASSERT(LINE, EQ   != (S3 != X2));

            // StringRef versus 'native_std::string'
            LOOP_ASSERT(LINE, LT   == (X1 <  S4));
            LOOP_ASSERT(LINE, GT   == (X1 >  S4));
            LOOP_ASSERT(LINE, LTEQ == (X1 <= S4));
            LOOP_ASSERT(LINE, GTEQ == (X1 >= S4));
            LOOP_ASSERT(LINE, EQ   == (X1 == S4));
            LOOP_ASSERT(LINE, EQ   != (X1 != S4));

            // bsl::string versus bsl::string
            // This test is to ensure no overloading ambiguity was introduced.
            LOOP_ASSERT(LINE, LT   == (S1 <  S2));
            LOOP_ASSERT(LINE, GT   == (S1 >  S2));
            LOOP_ASSERT(LINE, LTEQ == (S1 <= S2));
            LOOP_ASSERT(LINE, GTEQ == (S1 >= S2));
            LOOP_ASSERT(LINE, EQ   == (S1 == S2));
            LOOP_ASSERT(LINE, EQ   != (S1 != S2));

            // 'native_std::string' versus 'native_std::string'
            // This test is to ensure no overloading ambiguity was introduced.
            LOOP_ASSERT(LINE, LT   == (S3 <  S4));
            LOOP_ASSERT(LINE, GT   == (S3 >  S4));
            LOOP_ASSERT(LINE, LTEQ == (S3 <= S4));
            LOOP_ASSERT(LINE, GTEQ == (S3 >= S4));
            LOOP_ASSERT(LINE, EQ   == (S3 == S4));
            LOOP_ASSERT(LINE, EQ   != (S3 != S4));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   Object created from the ty string, non-empty string and substring
        //   stream correctly.  Width reset after output.
        //
        // Plan:
        //   For an empty string, non-empty string and substring, use
        //   'ostringstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   operator<<(ostream&, const bslstl::StringRef& string);
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nTesting Output (<<) Operator"
                               << "\n============================"
                               << std::endl;

        if (verbose) std::cout << "\nTesting 'operator<<' (ostream)."
                               << "\n= = = = = = = = = = = = = = = ="
                               << std::endl;
        {
          const size_t SIZE = 1000;     // max length of output string
          const char XX = (char) 0xFF;  // value representing an unset 'char'
          char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);

          Obj es(EMPTY_STRING);  const Obj& ES = es;

          if (veryVerbose) std::cout << "\tEXPECTED FORMAT: "
                                     << static_cast<bsl::string>(ES)
                                     << std::endl;
          std::ostringstream out;  out << ES;
          if (veryVerbose) std::cout << "\tACTUAL FORMAT:   "
                                     << out.str() << std::endl;

          const Obj::size_type ESSZ = ES.length() + 1;
          ASSERT(ESSZ < SIZE);           // Check buffer is large enough.
          ASSERT(out.str() == ES);
          ASSERT(out.good());

          // test default-constructed empty string ref
          Obj es1;  const Obj& ES1 = es1;

          if (veryVerbose) std::cout << "\tEXPECTED FORMAT: "
                                     << static_cast<bsl::string>(ES1)
                                     << std::endl;
          std::ostringstream out1;  out1 << ES1;
          if (veryVerbose) std::cout << "\tACTUAL FORMAT:   "
                                     << out1.str() << std::endl;

          const Obj::size_type ESSZ1 = ES1.length() + 1;
          ASSERT(ESSZ1 < SIZE);           // Check buffer is large enough.
          ASSERT(out1.str() == ES1);
          ASSERT(out1.good());

          Obj nes(NON_EMPTY_STRING);  const Obj& NES = nes;

          if (veryVerbose) std::cout << "\tEXPECTED FORMAT: "
                                     << NON_EMPTY_STRING << std::endl;
          std::ostringstream nesOut;  nesOut << NES;
          if (veryVerbose) std::cout << "\tACTUAL FORMAT:   "
                                     << nesOut.str() << std::endl;

          const Obj::size_type NESSZ = NES.length() + 1;
          ASSERT(NESSZ < SIZE);           // Check buffer is large enough.
          ASSERT(nesOut.str() == NES);
          ASSERT(nesOut.good());

          Obj ness(NON_EMPTY_STRING + 2, NON_EMPTY_STRING + 6);
          const Obj& NESS = ness;

          if (veryVerbose) std::cout << "\tEXPECTED FORMAT: "
                                     << static_cast<bsl::string>(NESS)
                                     << std::endl;
          std::ostringstream nessOut;  nessOut << NESS;
          if (veryVerbose) std::cout << "\tACTUAL FORMAT:   "
                                     << nessOut.str() << std::endl;

          const Obj::size_type NESSSZ = NESS.length() + 1;
          ASSERT(NESSSZ < SIZE);          // Check buffer is large enough.
          ASSERT(nessOut.str() == NESS);
          ASSERT(nessOut.good());

          // test formatting features
          std::ostringstream fmtOut;
          fmtOut << std::left << ES;
          ASSERT(fmtOut.str() == ES);

          fmtOut.str(bsl::string());
          fmtOut << std::right << ES;
          ASSERT(fmtOut.str() == ES);

          fmtOut.str(bsl::string());
          fmtOut << std::left << NES;
          ASSERT(fmtOut.str() == NES);

          fmtOut.str(bsl::string());
          fmtOut << std::right << NES;
          ASSERT(fmtOut.str() == NES);

          fmtOut.str(bsl::string());
          fmtOut << std::left
                 << std::setfill('-')
                 << std::setw(10)
                 << ES;
          ASSERT(fmtOut.str() == bsl::string(10, '-'));

          fmtOut.str(bsl::string());
          fmtOut << std::right
                 << std::setfill('*')
                 << std::setw(10)
                 << ES;
          ASSERT(fmtOut.str() == bsl::string(10, '*'));

          fmtOut.str(bsl::string());
          fmtOut << std::left
                 << std::setfill(' ')
                 << std::setw(static_cast<int>(NES.length() + 10))
                 << NES;
          ASSERT(fmtOut.str() == NES + bsl::string(10, ' '));

          fmtOut.str(bsl::string());
          fmtOut << std::right
                 << std::setfill('?')
                 << std::setw(static_cast<int>(NES.length() + 10))
                 << NES;
          ASSERT(fmtOut.str() == bsl::string(10, '?') + NES);

          fmtOut.str(bsl::string());
          fmtOut << std::left
                 << std::setfill('?')
                 << std::setw(static_cast<int>(NES.length() + 10))
                 << NES
                 << NES;
          ASSERT(fmtOut.str() == NES + bsl::string(10, '?') + NES);

          fmtOut.str(bsl::string());
          fmtOut << std::right
                 << std::setfill('?')
                 << std::setw(static_cast<int>(NES.length() + 10))
                 << NES
                 << NES;
          ASSERT(fmtOut.str() == bsl::string(10, '?') + NES + NES);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //
        // Concerns:
        //   Each individual 'stringRef' field must be correctly forwarded from
        //   the fully-tested underlying constructor.
        //
        // Plan:
        //   For each of a sequence of unique string values, verify that each
        //   of the direct accessors returns the correct value.
        //
        // Testing:
        //      const_iterator begin() const;
        //      const_iterator data() const;
        //      const_iterator end() const;
        //      int            length() const;
        //      int            empty() const;
        //      int            isEmpty() const;
        //      int            compare(other) const;
        //                     operator bsl::string() const;
        //      const char&    operator[](int index) const;
        // --------------------------------------------------------------------

        testBasicAccessors<char>(verbose);
        testBasicAccessors<wchar_t>(verbose);
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC MANIPULATORS (BOOTSTRAP):
        //
        // Concerns:
        //   The begin and end members must be correctly set by the different
        //   constructors.
        //
        // Plan:
        //   Test each of the constructors with empty and non-empty strings.
        //
        // Testing:
        //   bslstl::StringRef();
        //   bslstl::StringRef(const char *begin, const char *end);
        //   bslstl::StringRef(const char *begin, int length);
        //   bslstl::StringRef(const char *begin);
        //   bslstl::StringRef(const bsl::string& begin);
        //   bslstl::StringRef(const native_std::string& begin);
        //   bslstl::StringRef(const bslstl::StringRef& original);
        //   ~bslstl::StringRef();
        //   bslstl::StringRef& operator=(const bslstl::StringRef&);
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nTesting Primary Manipulator"
                               << "\n===========================" << std::endl;

        if (verbose) std::cout << "\nTesting default constructor"
                               << "\n= = = = = = = = = = = = = =" << std::endl;
        if (veryVerbose)
            std::cout << "\nbslstl_StringRef()"
                 << "\n=  =  =  =  =  = "
                 << std::endl;

        {
          Obj x;  const Obj& X =x;

          ASSERT(X.isEmpty());
          ASSERT(X.length()  == 0);
          ASSERT(X.begin()   == X.end());
        }

        if (verbose) std::cout << "\nTesting other constructors"
                               << "\n= = = = = = = = = = = = = " << std::endl;
        if (veryVerbose)
            std::cout
               << "\nbslstl_StringRef(const char *begin, const char *end)"
               << "\n=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  "
               << std::endl;
        {
          // Empty string
          Obj x1(EMPTY_STRING, EMPTY_STRING + std::strlen(EMPTY_STRING));
          const Obj& X1 = x1;
          ASSERT(X1.isEmpty());
          ASSERT(X1.length()  == 0);
          ASSERT(X1.begin()   == X1.end());
          ASSERT(X1.begin()   == EMPTY_STRING);
          ASSERT(X1.end()     == EMPTY_STRING + std::strlen(EMPTY_STRING));

          // Non-empty string
          Obj x2(NON_EMPTY_STRING,
                 NON_EMPTY_STRING + std::strlen(NON_EMPTY_STRING));
          const Obj& X2 = x2;
          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  == 30);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(X2.begin()   == NON_EMPTY_STRING);
          ASSERT(X2.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));
        }

        if (veryVerbose)
            std::cout
                << "\nbslstl_StringRef(const char *begin, int length)"
                << "\n=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  ="
                << std::endl;

        {
          // Empty string
          Obj x1(EMPTY_STRING, static_cast<int>(std::strlen(EMPTY_STRING)));
          const Obj& X1 = x1;
          ASSERT(X1.isEmpty());
          ASSERT(X1.length()  == 0);
          ASSERT(X1.begin()   == X1.end());
          ASSERT(X1.begin()   == EMPTY_STRING);
          ASSERT(X1.end()     == EMPTY_STRING + std::strlen(EMPTY_STRING));

          // Non-empty string
          Obj x2(NON_EMPTY_STRING,
                 static_cast<int>(std::strlen(NON_EMPTY_STRING)));
          const Obj& X2 = x2;
          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  == 30);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(X2.begin()   == NON_EMPTY_STRING);
          ASSERT(X2.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));
        }

        if (veryVerbose)
            std::cout << "\nbslstl_StringRef(const char *begin)"
                 << "\n=  =  =  =  =  =  =  =  =  =  =  ="
                 << std::endl;

        {
          // Empty string
          Obj x1(EMPTY_STRING);  const Obj& X1 = x1;
          ASSERT(X1.isEmpty());
          ASSERT(X1.length()  == 0);
          ASSERT(X1.begin()   == X1.end());
          ASSERT(X1.begin()   == EMPTY_STRING);
          ASSERT(X1.end()     == EMPTY_STRING + std::strlen(EMPTY_STRING));

          // Non-empty string
          Obj x2(NON_EMPTY_STRING);  const Obj& X2 = x2;
          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  == 30);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(X2.begin()   == NON_EMPTY_STRING);
          ASSERT(X2.end()     == NON_EMPTY_STRING +
                                 std::strlen(NON_EMPTY_STRING));
          ASSERT(X1           != X2);
        }

        if (veryVerbose)
            std::cout << "\nbslstl_StringRef(const bsl::string& begin)"
                 << "\n=  =  =  =  =  =  =  =  =  =  =  =  =  = "
                 << std::endl;

        {
          // Empty string
          bsl::string emptyString(EMPTY_STRING);
          Obj x1(emptyString);  const Obj& X1 = x1;
          ASSERT(X1.isEmpty());
          ASSERT(X1.length()    == 0);
          ASSERT(X1.begin()     == X1.end());
          ASSERT(emptyString.c_str() == X1.end());

          // Non-empty string
          bsl::string nonEmptyString(NON_EMPTY_STRING);
          Obj x2(nonEmptyString);  const Obj& X2 = x2;
          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  == 30);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(&*X2.begin()   == &*nonEmptyString.begin());
          ASSERT((&*X2.begin() + (X2.end() - X2.begin())) ==
                 (&*nonEmptyString.begin() + (nonEmptyString.end() -
                                             nonEmptyString.begin())));
        }

        if (veryVerbose)
            std::cout
                << "\nbslstl_StringRef(const native_std::string& begin)"
                << "\n  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  ="
                << std::endl;

        {
          // Empty string
          const native_std::string emptyString(EMPTY_STRING);
          Obj x1(emptyString);  const Obj& X1 = x1;
          ASSERT(X1.isEmpty());
          ASSERT(X1.length()    == 0);
          ASSERT(X1.begin()     == X1.end());

// DRQS 24793537: When built in optimized mode on IBM, the following assert
// fails.  The address returned from data() here is different from the address
// returned in the constructor of StringRef.  data() returns a pointer to
// a function local static char variable and the compiler generates multiple
// copies of that static variable.
#if !defined(BSLS_PLATFORM_CMP_IBM) || !defined(BDE_BUILD_TARGET_OPT)
          ASSERT(emptyString.data() + emptyString.length() == X1.end());
#endif

          // Non-empty string
          native_std::string nonEmptyString(NON_EMPTY_STRING);
          Obj x2(nonEmptyString);  const Obj& X2 = x2;
          ASSERT(!X2.isEmpty());
          ASSERT(X2.length()  == 30);
          ASSERT(X2.begin()   != X2.end());
          ASSERT(&*X2.begin()   == &*nonEmptyString.begin());
          ASSERT((&*X2.begin() + (X2.end() - X2.begin())) ==
                 (&*nonEmptyString.begin() + (nonEmptyString.end() -
                                             nonEmptyString.begin())));
        }

        if (verbose) std::cout << "\nTesting copy constructor"
                               << "\n= = = = = = = = = = = = " << std::endl;
        if (veryVerbose)
            std::cout
                << "\nbslstl_StringRef(const bslstl::StringRef&)"
                << "\n=  =  =  =  =  =  =  =  =  =  =  =  =  =  =  =  "
                << std::endl;
        {
          // Empty string
          Obj x1(EMPTY_STRING, EMPTY_STRING + std::strlen(EMPTY_STRING));
                                             const Obj& X1  = x1;
          Obj xc1(X1);                       const Obj& XC1 = xc1;
          ASSERT(XC1.isEmpty());
          ASSERT(XC1.length()  == 0);
          ASSERT(XC1.begin()   == XC1.end());
          ASSERT(XC1.begin()   == EMPTY_STRING);
          ASSERT(XC1.end()     == EMPTY_STRING + std::strlen(EMPTY_STRING));

          // Non-empty string
          Obj x2(NON_EMPTY_STRING,
                 NON_EMPTY_STRING + std::strlen(NON_EMPTY_STRING));
                                             const Obj& X2  = x2;
          Obj xc2(X2);                       const Obj& XC2 = xc2;
          ASSERT(!XC2.isEmpty());
          ASSERT(XC2.length()  == 30);
          ASSERT(XC2.begin()   != XC2.end());
          ASSERT(XC2.begin()   == NON_EMPTY_STRING);
          ASSERT(XC2.end()     == NON_EMPTY_STRING
                                + std::strlen(NON_EMPTY_STRING));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   Exercise each method lightly.
        //
        // Plan:
        //   Create several 'bsl::string' objects and use them to test various
        //   methods.
        //
        // Testing:
        //   This Test Case exercises basic functionality.
        // --------------------------------------------------------------------

        if (verbose) std::cout << "\nBREATHING TEST"
                               << "\n==============" << std::endl;

        bsl::string  s1  = "hello";
        bsl::string  s2  = "Hello";
        bsl::string  s3  = "good-bye";
        bsl::string  s4  = "Good-Bye";

        const char  *cs1 = s1.c_str();
        const char  *cs2 = s2.c_str();
        const char  *cs3 = s3.c_str();

        {
            Obj x1(cs1, cs1 + 3);      const Obj& X1 = x1;
            Obj x2(cs1, 3);            const Obj& X2 = x2;
                                       const Obj  X3(X1);

            ASSERT(3 == X1.length());
            ASSERT(3 == X2.length());
            ASSERT(3 == X3.length());

            ASSERT(1 == (X1 == X1));
            ASSERT(0 == (X1 != X1));
            ASSERT(1 == (X1 == X2));
            ASSERT(0 == (X1 != X2));
            ASSERT(1 == (X1 == X3));
            ASSERT(0 == (X1 != X3));

            ASSERT(0 == (X1 <  X1));
            ASSERT(1 == (X1 <= X1));
            ASSERT(0 == (X1 >  X1));
            ASSERT(1 == (X1 >= X1));

            ASSERT(0 == (X1 <  X2));
            ASSERT(1 == (X1 <= X2));
            ASSERT(0 == (X1 >  X2));
            ASSERT(1 == (X1 >= X2));

            ASSERT(0 == (X1 >  X3));
            ASSERT(1 == (X1 >= X3));
            ASSERT(0 == (X3 >  X2));
            ASSERT(1 == (X3 >= X2));
        }

        {
            Obj x1(s1);               const Obj& X1 = x1;
            Obj x2(cs1);              const Obj& X2 = x2;
            Obj x3(cs1, cs1 + 3);     const Obj& X3 = x3;

            ASSERT(5 == X1.length());
            ASSERT(5 == X2.length());

            ASSERT(1 == (X1 == X1));
            ASSERT(0 == (X1 != X1));
            ASSERT(1 == (X1 == X2));
            ASSERT(0 == (X1 != X2));

            ASSERT(0 == (X1 <  X2));
            ASSERT(1 == (X1 <= X2));
            ASSERT(0 == (X1 >  X2));
            ASSERT(1 == (X1 >= X2));

            ASSERT(0 == (X1 <  X3));
            ASSERT(1 == (X1 >= X3));
            ASSERT(0 == (X1 <= X3));
            ASSERT(1 == (X1 >  X3));
        }

        {
            Obj x1(cs1, cs1 + 3);      const Obj& X1 = x1;
            Obj x2(cs1 + 3, cs1 + 5);  const Obj& X2 = x2;
            Obj x3(X1);                const Obj& X3 = x3;

            ASSERT(3 == X1.length());
            ASSERT(2 == X2.length());
            ASSERT(3 == X3.length());

            ASSERT(X1 == X1);
            ASSERT(X2 == X2);
            ASSERT(X3 == X3);
            ASSERT(X1 != X2);
            ASSERT(X2 != X3);
            ASSERT(X1 == X3);

            x3 = x2;

            ASSERT(X3 == X3);
            ASSERT(X2 == X3);
            ASSERT(X1 != X3);

            bsl::string sx1 = (bsl::string) X1;
            if (verbose) P(sx1.c_str());
            ASSERT(3 == sx1.size());

            if (verbose) { P_(X1.begin());  P(X2.begin()); }

            bsl::string sx1x2 = X1 + X2;
            ASSERT(s1 == sx1x2);
            ASSERT(s1 == X1 + X2);
        }

        {
            Obj x1;  const Obj& X1 = x1;
            if (verbose) { P(X1.begin()); }
            ASSERT(0 == X1.length());

            bsl::string s1 = X1;
            if (verbose) { P_(s1.c_str());  P(X1.begin()); }

            Obj x2(cs2, cs2 + 3);  const Obj& X2 = x2;
            if (verbose) { P(X2.begin()); }

            ASSERT(X2 != X1);
            x2.reset();
            ASSERT(X2 == X1);
        }

        {
            Obj x1(cs3, cs3 + 8);  const Obj& X1 = x1;
            Obj x2(cs3, cs3 + 8);  const Obj& X2 = x2;

            ASSERT(8 == X1.length());
            ASSERT(8 == X2.length());

            ASSERT(X1 == X2);
        }
      } break;
      default: {
        std::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << std::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << std::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
