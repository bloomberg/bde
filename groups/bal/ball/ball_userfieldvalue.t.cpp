// ball_userfieldvalue.t.cpp                                          -*-C++-*-
#include <ball_userfieldvalue.h>

#include <bdlt_datetime.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// TBD
//
// Primary Manipulators:
//: o void setInt64(bsls::Types::Int64 value);
//: o void setDouble(double value);
//: o void setString(bslstl::StringRef value);
//: o void setDatetimeTz(const bdlt::DatetimeTz& value);
//: o void setCharArray(const bsl::vector<char>& value);
//
// Basic Accessors:
//: o ball::UserFieldType::Enum type() const;
//: o const bsls::Types::Int64& theInt64() const;
//: o const double& theDouble() const;
//: o const bsl::string& theString() const;
//: o const bdlt::DatetimeTz& theDatetimeTz() const;
//: o const bsl::vector<char>& theCharArray() const;
//: o bslma::Allocator *allocator() const;
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR/MANIPULATOR/OPERATOR ptr./ref. parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o No memory is ever allocated from the default allocator.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] UserFieldValue(Allocator *ba = 0);
// [12] UserFieldValue(bsls::Types::Int64       value, Allocator *ba = 0);
// [12] UserFieldValue(double                   value, Allocator *ba = 0);
// [12] UserFieldValue(bslstl::StringRef        value, Allocator *ba = 0);
// [12] UserFieldValue(const bdlt::DatetimeTz&  value, Allocator *ba = 0);
// [12] UserFieldValue(const bsl::vector<char>& value, Allocator *ba = 0);
// [13] UserFieldValue(INTEGRAL_TYPE            value, Allocator *ba = 0);
// [ 7] UserFieldValue(const UserFieldValue& original, Allocator *ba = 0);
// [ 2] ~UserFieldValue() = default;
//
// MANIPULATORS
// [ 9] UserFieldValue& operator=(const UserFieldValue& rhs);
// [11] void reset();
// [ 2] void setInt64(bsls::Types::Int64 value);
// [ 2] void setDouble(double value);
// [ 2] void setString(bslstl::StringRef value);
// [ 2] void setDatetimeTz(const bdlt::DatetimeTz& value);
// [ 2] void setCharArray(const bsl::vector<char>& value);
// [ 8] void swap(UserFieldValue& other);
//
// ACCESSORS
// [11] bool isUnset() const;
// [ 4] ball::UserFieldType::Enum type() const;
// [ 4] const bsls::Types::Int64& theInt64() const;
// [ 4] const double& theDouble() const;
// [ 4] const bsl::string& theString() const;
// [ 4] const bdlt::DatetimeTz& theDatetimeTz() const;
// [ 4] const bsl::vector<char>& theCharArray() const;
// [ 4] bslma::Allocator *allocator() const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const UserFieldValue& lhs, const UserFieldValue& rhs);
// [ 6] bool operator!=(const UserFieldValue& lhs, const UserFieldValue& rhs);
// [ 5] ostream& operator<<(ostream &os, const UserFieldValue& object);
//
// FREE FUNCTIONS
// [ 8] void swap(UserFieldValue& a, UserFieldValue& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [14] USAGE EXAMPLE
// [ 3] int ggg(Obj *object, const char *spec, bool verboseFlag = true);
// [ 3] Obj& gg(Obj *object, const char *spec);
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: In no case does memory come from the default allocator.
// [ 4] CONCERN: All accessor methods are declared 'const'.
// [10] Reserved for BDEX streaming.

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::UserFieldValue Obj;
typedef ball::UserFieldType  Type;

        // Create Two Distinct Exemplars For Each Field Type
        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

// Note that, for example, the "A" in "A1" corresponds to 'Type::e_INT64'.

const bsls::Types::Int64 A1 =  7925;
const bsls::Types::Int64 A2 = -1242;

const double             B1 = 10.5;
const double             B2 = 20.5;

const bsl::string        C1 = "one";
const bsl::string        C2 = SUFFICIENTLY_LONG_STRING;

const bdlt::DatetimeTz   D1(bdlt::Datetime(2000,  1,  1, 0, 1, 2,   3), 240);
const bdlt::DatetimeTz   D2(bdlt::Datetime(2025, 12, 31, 4, 5, 6, 789), -60);

static bsl::vector<char> fE1()
{
    bsl::vector<char> t(C1.rbegin(), C1.rend());
    return t;
}
static bsl::vector<char> fE2()
{
    bsl::vector<char> t(C2.rbegin(), C2.rend());
    return t;
}
const bsl::vector<char>  E1 = fE1();
const bsl::vector<char>  E2 = fE2();

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

Type::Enum charToUserFieldType(char c)
    // Return the user field type corresponding to the specified 'c', or
    // 'Type::e_VOID' if 'c' does not correspond to a user field type.
{
    switch (c) {
      default: {
        return Type::e_VOID;                                          // RETURN
      }
      case 'A': {
        return Type::e_INT64;                                         // RETURN
      }
      case 'B': {
        return Type::e_DOUBLE;                                        // RETURN
      }
      case 'C': {
        return Type::e_STRING;                                        // RETURN
      }
      case 'D': {
        return Type::e_DATETIMETZ;                                    // RETURN
      }
      case 'E': {
        return Type::e_CHAR_ARRAY;                                    // RETURN
      }
    }
    return Type::e_VOID;
}

// ============================================================================
//              GENERATOR FUNCTIONS 'gg' AND 'ggg' FOR TESTING
// ---------------------------------------------------------------------------- 
// TBD doc
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters '[A .. E]' correspond to arbitrary (but unique) 'int' values to be
// assigned to the 'bdlb::Variant' object (thus of type 'INT_TYPE').  Uppercase
// letters '[F .. J]' correspond to arbitrary (but unique) 'TestInt' values to
// be assigned to the 'bdlb::Variant' object (thus of type 'TEST_INT_TYPE').
// Uppercase letters '[S .. W]' correspond to arbitrary (but unique) 'string'
// values to be assigned to the 'bdlb::Variant' object (thus of type
// 'STRING_TYPE').  Uppercase letters '[K .. O]' correspond to arbitrary (but
// unique) 'TestString' values to be assigned to the 'bdlb::Variant' object
// (thus of type 'TEST_STRING_TYPE').  Uppercase letter 'Z' corresponds to an
// object of type 'TestVoid'.  A tilde ('~') indicates that the value of the
// object is to be set to its initial, unset state (via the 'reset' method).
//
// LANGUAGE SPECIFICATION
// ----------------------
// <SPEC>       ::= <EMPTY> | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ITEM>  | <ITEM> <LIST>
//
// <ITEM>       ::= <VALUE> | <RESET>
//
// <VALUE>      ::= <TYPE> <INDEX>
//
// <TYPE>       ::= 'A' | 'B' | 'C' | 'D' | 'E'
//                  // Corresponding to 'e_INT64' 'e_DOUBLE', 'e_STRING',
//                  // 'e_DATETIMETZ', and 'e_CHAR_ARRAY', respectively.
//
// <INDEX>      ::= '1' | '2'
//                  // The first or second test value of the associated type.
//
// <RESET>      ::= '~'
//
// Spec String  Description
// -----------  -----------------------------------------------------------
// "~"          Reset the object to the unset state.
//
// "B1"         Set the object to have the value corresponding to 'B1', i.e.,
//              the first of the two test values for 'e_DOUBLE'.
//
// "A2D1"       Set the object to have the value corresponding to 'A2', then
//              set it to have the value corresponding to 'D1'.
// ------------------------------------------------------------------------

int ggg(Obj *object, const char *spec, bool verboseFlag = true)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the setters and 'reset'.  Optionally specify a 'false'
    // 'verboseFlag' to suppress 'spec' syntax error messages.  Return the
    // index of the first invalid character, and a negative value otherwise.
    // Note that this function is used to implement 'gg' as well as allow for
    // verification of syntax error detection.
{
    ASSERT(object);
    ASSERT(spec);

    enum { POSITION = 0, SUCCESS = -1 };

    const char *input = spec;

    while (*input) {
        if ('~' == *input) {
            object->reset();
            ++input;
        }
        else if (*input >= 'A' && *input <= 'E') {
           const Type::Enum type = charToUserFieldType(*input);
            ++input;

            if ('1' == *input || '2' == *input) {
                const bool isOne = '1' == *input;
                ++input;
                
                switch (type) {
                  case Type::e_VOID: {
                    BSLS_ASSERT_OPT(false);
                  } break;
                  case Type::e_INT64: {
                    isOne ? object->setInt64(A1)
                          : object->setInt64(A2);
                  } break;
                  case Type::e_DOUBLE: {
                    isOne ? object->setDouble(B1)
                          : object->setDouble(B2);
                  } break;
                  case Type::e_STRING: {
                    isOne ? object->setString(C1)
                          : object->setString(C2);
                  } break;
                  case Type::e_DATETIMETZ: {
                    isOne ? object->setDatetimeTz(D1)
                          : object->setDatetimeTz(D2);
                  } break;
                  case Type::e_CHAR_ARRAY: {
                    isOne ? object->setCharArray(E1)
                          : object->setCharArray(E2);
                  } break;
                }
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }

    if (*input) {
        const int idx = static_cast<int>(input - spec);
        if (verboseFlag) {
            printf("Error, bad character ('%c') "
                   "in spec \"%s\" at position %d.\n", *input, spec, idx);
        }
        return idx;  // Discontinue processing this spec.             // RETURN
    }

    return -1;  // All input was consumed.
}

Obj& gg(Obj *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec' according to the custom language
    // described above.
{
    ASSERT(object);
    ASSERT(spec);
    ASSERT(ggg(object, spec) < 0);

    return *object;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: This test driver is reusable w/other, similar components.

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: In no case does memory come from the default allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    switch (test) { case 0:
      case 14: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'ball::UserFieldValue'
/// - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to create and use a
// 'ball::UserFieldValue' object.  Note that 'ball::UserFieldValue' objects
// are typically used in a description of a sequence of user fields (see
// 'ball_userfields').
//
// First, we create a default 'ball::UserFieldValue', 'valueA', and observe
// that it is in the unset state, meaning that 'isUnset' is true and its type
// is 'ball::UserFieldValue::e_VOID':
//..
    ball::UserFieldValue valueA;
//
    ASSERT(true                        == valueA.isUnset());
    ASSERT(ball::UserFieldType::e_VOID == valueA.type());
//..
// Next, we create a second 'ball::UserFieldValue' having the value 5, and then
// confirm its value and observe that it does not compare equal to the
// 'valueA':
//..
    ball::UserFieldValue valueB(5);
//
    ASSERT(false                        == valueB.isUnset());
    ASSERT(ball::UserFieldType::e_INT64 == valueB.type());
    ASSERT(5                            == valueB.theInt64())
//
    ASSERT(valueA != valueB);
//..
// Finally, we call 'reset' of 'valueB' resetting it to the unset state, and
// observe that 'valueA' now compares equal to 'valueB':
//..
    valueB.reset();
//
    ASSERT(valueA == valueB);
//..

      } break;
      case 13: {
        // TBD
      } break;
      case 12: {
        // TBD
      } break;
      case 11: {
        // TBD
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for BDEX streaming.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDEX STREAMING" << endl
                          << "==============" << endl;

        if (verbose) cout << "Not implemented for 'ball::UserFieldValue'."
                          << endl;

      } break;
      case 9: {
        // TBD
      } break;
      case 8: {
        // TBD

        // Negative Testing

      } break;
      case 7: {
        // TBD
      } break;
      case 6: {
        // TBD
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR (<<)
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method's signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values (0 and 4, respectively).
        //:
        //: 7 The output 'operator<<'s signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the destination 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values, and various
        //:     values for the two formatting parameters, along with the
        //:     expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, '(-9, -9)', then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //
        // Testing:
        //   ostream& print(ostream& s, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream &os, const UserFieldValue& object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR (<<)" << endl
                          << "==============================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOut = ball::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOut;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;            // source line number
            int         d_level;
            int         d_spacesPerLevel;

            const char *d_string_p;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL   VALUE         EXP
        //---- - ---   -----         ---

        { L_,  0,  0,  "28JUL1914",  "28JUL1914"              NL },

        { L_,  0,  1,  "28JUL1914",  "28JUL1914"              NL },

        { L_,  0, -1,  "28JUL1914",  "28JUL1914"                 },

        { L_,  0, -8,  "28JUL1914",  "28JUL1914"              NL },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL   VALUE         EXP
        //---- - ---   -----         ---

        { L_,  3,  0,  "28JUL1914",  "28JUL1914"              NL },

        { L_,  3,  2,  "28JUL1914",  "      28JUL1914"        NL },

        { L_,  3, -2,  "28JUL1914",  "      28JUL1914"           },

        { L_,  3, -8,  "28JUL1914",  "            28JUL1914"  NL },

        { L_, -3,  0,  "28JUL1914",  "28JUL1914"              NL },

        { L_, -3,  2,  "28JUL1914",  "28JUL1914"              NL },

        { L_, -3, -2,  "28JUL1914",  "28JUL1914"                 },

        { L_, -3, -8,  "28JUL1914",  "28JUL1914"              NL },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL   VALUE         EXP
        //---- - ---   -----         ---

        { L_,  2,  3,  "09DEC0721",  "      09DEC0721"        NL },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL   VALUE         EXP
        //---- - ---   -----         ---

        { L_, -8, -8,  "28JUL1914",  "28JUL1914"              NL },

        { L_, -8, -8,  "09DEC0721",  "09DEC0721"              NL },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL   VALUE         EXP
        //---- - ---   -----         ---

        { L_, -9, -9,  "28JUL1914",  "28JUL1914"                 },

        { L_, -9, -9,  "09DEC0721",  "09DEC0721"                 },

#undef NL

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const char *const STRING = DATA[ti].d_string_p;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(L) P(SPL)
                    T_ P(STRING)
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X(STRING);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                ostringstream os(&oa);

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    ASSERTV(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    ASSERTV(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        ASSERTV(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        ASSERTV(LINE, &os == &X.print(os, L));
                    }
                    else {
                        ASSERTV(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                {
                    bslma::TestAllocator da("default", veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&da);

                    // Verify output is formatted as expected.

                    if (veryVeryVerbose) { P(os.str()) }

                    ASSERTV(LINE, EXP, os.str(), EXP == os.str());
                }
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 TBD
        //
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   ball::UserFieldType::Enum type() const;
        //   const bsls::Types::Int64& theInt64() const;
        //   const double& theDouble() const;
        //   const bsl::string& theString() const;
        //   const bdlt::DatetimeTz& theDatetimeTz() const;
        //   const bsl::vector<char>& theCharArray() const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        // TBD

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard guard;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mX(&scratch);  const Obj& X = mX;

            ASSERT_SAFE_FAIL(X.theInt64());
            ASSERT_SAFE_FAIL(X.theDouble());
            ASSERT_SAFE_FAIL(X.theString());
            ASSERT_SAFE_FAIL(X.theDatetimeTz());
            ASSERT_SAFE_FAIL(X.theCharArray());

            mX.setInt64(A1);

            ASSERT_SAFE_PASS(X.theInt64());
            ASSERT_SAFE_FAIL(X.theDouble());
            ASSERT_SAFE_FAIL(X.theString());
            ASSERT_SAFE_FAIL(X.theDatetimeTz());
            ASSERT_SAFE_FAIL(X.theCharArray());

            mX.setDouble(B1);

            ASSERT_SAFE_FAIL(X.theInt64());
            ASSERT_SAFE_PASS(X.theDouble());
            ASSERT_SAFE_FAIL(X.theString());
            ASSERT_SAFE_FAIL(X.theDatetimeTz());
            ASSERT_SAFE_FAIL(X.theCharArray());

            mX.setString(C1);

            ASSERT_SAFE_FAIL(X.theInt64());
            ASSERT_SAFE_FAIL(X.theDouble());
            ASSERT_SAFE_PASS(X.theString());
            ASSERT_SAFE_FAIL(X.theDatetimeTz());
            ASSERT_SAFE_FAIL(X.theCharArray());

            mX.setDatetimeTz(D1);

            ASSERT_SAFE_FAIL(X.theInt64());
            ASSERT_SAFE_FAIL(X.theDouble());
            ASSERT_SAFE_FAIL(X.theString());
            ASSERT_SAFE_PASS(X.theDatetimeTz());
            ASSERT_SAFE_FAIL(X.theCharArray());

            mX.setCharArray(E1);

            ASSERT_SAFE_FAIL(X.theInt64());
            ASSERT_SAFE_FAIL(X.theDouble());
            ASSERT_SAFE_FAIL(X.theString());
            ASSERT_SAFE_FAIL(X.theDatetimeTz());
            ASSERT_SAFE_PASS(X.theCharArray());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATOR FUNCTION 'gg'
        //
        // Concerns:
        //: 1 Parsing stops at the first incorrect character of the spec.
        //:
        //: 2 The resulting object has the correct type and value.
        //:
        //: 3 All examples in the documentation are parsed as expected.
        //
        // Plan:
        // TBD doc
        //  Using the table-driven technique, create a table of test vectors
        //  containing the line number, spec, expected return code, expected
        //  type index, and value index.  For concern 1, make sure the return
        //  code is the same as the specified offset where the error occurred.
        //  For concerns 2 and 3, verify that the type index and value index
        //  are as expected.
        //
        // Testing:
        //   int ggg(Obj *object, const char *spec, bool verboseFlag = true);
        //   Obj& gg(Obj *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATOR FUNCTION 'gg'" << endl
                          << "=======================" << endl;

        // TBD

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS & DTOR
        //
        // Concerns:
        //: 1 TBD
        //:
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   UserFieldValue(Allocator *ba = 0);
        //   ~UserFieldValue() = default;
        //   void setInt64(bsls::Types::Int64 value);
        //   void setDouble(double value);
        //   void setString(bslstl::StringRef value);
        //   void setDatetimeTz(const bdlt::DatetimeTz& value);
        //   void setCharArray(const bsl::vector<char>& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS & DTOR" << endl
                          << "===========================" << endl;

        if (verbose) {
            cout <<
                 "\nDefault constructor with various allocator configurations."
                 << endl;
        }
        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr;
            bslma::TestAllocator *objAllocatorPtr;

            switch (CONFIG) {
              case 'a': {
                objPtr = new (fa) Obj();
                objAllocatorPtr = &da;
              } break;
              case 'b': {
                objPtr = new (fa) Obj(
                                  reinterpret_cast<bslma::TestAllocator *>(0));
                objAllocatorPtr = &da;
              } break;
              case 'c': {
                objPtr = new (fa) Obj(&sa);
                objAllocatorPtr = &sa;
              } break;
              default: {
                ASSERTV(CONFIG, !"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

            if (veryVerbose) { P_(CONFIG); P(X); }

            ASSERTV(CONFIG, X.isUnset());
            ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG,  oa.numBlocksTotal(), 0 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksTotal());
            ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
        }

        if (verbose) {
            cout <<
      "\nApply each setter to unset object w/various allocator configurations."
                 << endl;
        }
        for (char ft = 'A'; ft <= 'E'; ++ft) {  // iterate over field types
               const Type::Enum type = charToUserFieldType(ft);

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                    objPtr = new (fa) Obj();
                    objAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    objPtr = new (fa) Obj(
                                  reinterpret_cast<bslma::TestAllocator *>(0));
                    objAllocatorPtr = &da;
                  } break;
                  case 'c': {
                    objPtr = new (fa) Obj(&sa);
                    objAllocatorPtr = &sa;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } break;
                }

                Obj&                   mX = *objPtr;  const Obj& X = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                ASSERTV(CONFIG, X.isUnset());
                ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

                bslma::TestAllocatorMonitor oam(objAllocatorPtr);

                switch (type) {
                  case Type::e_VOID: {
                    BSLS_ASSERT_OPT(false);
                  } break;
                  case Type::e_INT64: {
                    mX.setInt64(A2);
                    ASSERT(A2 == X.theInt64());
                  } break;
                  case Type::e_DOUBLE: {
                    mX.setDouble(B2);
                    ASSERT(B2 == X.theDouble());
                  } break;
                  case Type::e_STRING: {
                    mX.setString(C2);
                    ASSERT(C2 == X.theString());
                  } break;
                  case Type::e_DATETIMETZ: {
                    mX.setDatetimeTz(D2);
                    ASSERT(D2 == X.theDatetimeTz());
                  } break;
                  case Type::e_CHAR_ARRAY: {
                    mX.setCharArray(E2);
                    ASSERT(E2 == X.theCharArray());
                  } break;
                }

                if (veryVerbose) {
                    // Printing an 'e_DATETIMETZ' field value incurs an
                    // allocation from the default allocator.

                    bslma::TestAllocator scratch("scratch",
                                                 veryVeryVeryVerbose);
                    bslma::DefaultAllocatorGuard dag(&scratch);

                    P_(CONFIG); P_(type); P(X);
                }

                ASSERTV(CONFIG, type, type == X.type());
                ASSERTV(CONFIG, type,        !X.isUnset());
                ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

                if (Type::e_STRING == type || Type::e_CHAR_ARRAY == type) {
                    ASSERTV(CONFIG, type, oam.isInUseUp());
                }
                else {
                    ASSERTV(CONFIG, type, oam.isTotalSame());
                }

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(CONFIG, type,fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());

                if (Type::e_STRING == type || Type::e_CHAR_ARRAY == type) {
                    ASSERTV(CONFIG, type, oa.numBlocksInUse(),
                            0 == oa.numBlocksInUse());
                }
                else {
                    ASSERTV(CONFIG, type, oa.numBlocksTotal(),
                            0 == oa.numBlocksTotal());
                }

                ASSERTV(CONFIG, type, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());
            }
        }

        if (verbose) {
            cout << "\nApply each setter to non-unset object." << endl;
        }
        for (char fti = 'A'; fti <= 'E'; ++fti) {  // iterate over field types
               const Type::Enum typeI = charToUserFieldType(fti);

            for (char ftj = 'A'; ftj <= 'E'; ++ftj) {
                   const Type::Enum typeJ = charToUserFieldType(ftj);

                bslma::TestAllocator da("default", veryVeryVeryVerbose);
                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                {
                    Obj mX(&oa);  const Obj& X = mX;

                    switch (typeI) {  // initial type of field
                      case Type::e_VOID: {
                        BSLS_ASSERT_OPT(false);
                      } break;
                      case Type::e_INT64: {
                        mX.setInt64(A1);
                        ASSERT(A1 == X.theInt64());
                      } break;
                      case Type::e_DOUBLE: {
                        mX.setDouble(B1);
                        ASSERT(B1 == X.theDouble());
                      } break;
                      case Type::e_STRING: {
                        mX.setString(C1);
                        ASSERT(C1 == X.theString());
                      } break;
                      case Type::e_DATETIMETZ: {
                        mX.setDatetimeTz(D1);
                        ASSERT(D1 == X.theDatetimeTz());
                      } break;
                      case Type::e_CHAR_ARRAY: {
                        mX.setCharArray(E1);
                        ASSERT(E1 == X.theCharArray());
                      } break;
                    }
                    ASSERTV(typeI, typeJ, typeI == X.type());

                    if (veryVerbose) {
                        // Printing an 'e_DATETIMETZ' field value incurs an
                        // allocation from the default allocator.

                        bslma::TestAllocator scratch("scratch",
                                                     veryVeryVeryVerbose);
                        bslma::DefaultAllocatorGuard dag(&scratch);

                        Q("Before:") P_(typeI); P_(typeJ); P(X);
                    }

                    bslma::TestAllocatorMonitor oam(&oa);

                    switch (typeJ) {  // new type of field
                      case Type::e_VOID: {
                        BSLS_ASSERT_OPT(false);
                      } break;
                      case Type::e_INT64: {
                        mX.setInt64(A2);
                        ASSERT(A2 == X.theInt64());
                      } break;
                      case Type::e_DOUBLE: {
                        mX.setDouble(B2);
                        ASSERT(B2 == X.theDouble());
                      } break;
                      case Type::e_STRING: {
                        mX.setString(C2);
                        ASSERT(C2 == X.theString());
                      } break;
                      case Type::e_DATETIMETZ: {
                        mX.setDatetimeTz(D2);
                        ASSERT(D2 == X.theDatetimeTz());
                      } break;
                      case Type::e_CHAR_ARRAY: {
                        mX.setCharArray(E2);
                        ASSERT(E2 == X.theCharArray());
                      } break;
                    }
                    ASSERTV(typeI, typeJ, typeJ == X.type());

                    if (veryVerbose) {
                        // Printing an 'e_DATETIMETZ' field value incurs an
                        // allocation from the default allocator.

                        bslma::TestAllocator scratch("scratch",
                                                     veryVeryVeryVerbose);
                        bslma::DefaultAllocatorGuard dag(&scratch);

                        Q("After:") P_(typeI); P_(typeJ); P(X);
                    }

                    if ((Type::e_CHAR_ARRAY == typeJ || Type::e_STRING ==typeJ)
                      && Type::e_CHAR_ARRAY != typeI) {
                        ASSERTV(typeI, typeJ, oam.isInUseUp());
                    }
                    else {
                        ASSERTV(typeI, typeJ,
                                oam.isInUseSame() || oam.isTotalSame());
                    }
                }

                // Verify all memory is released on object destruction.

                if (Type::e_STRING == typeJ || Type::e_CHAR_ARRAY == typeI
                                            || Type::e_CHAR_ARRAY == typeJ) {
                    ASSERTV(typeI, typeJ, oa.numBlocksInUse(),
                            0 == oa.numBlocksInUse());
                }
                else {
                    ASSERTV(typeI, typeJ, oa.numBlocksTotal(),
                            0 == oa.numBlocksTotal());
                }

                ASSERTV(typeI, typeJ, da.numBlocksTotal(),
                        0 == da.numBlocksTotal());
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
        //: 1 Create an object 'w' (default ctor).      { w:0             }
        //: 2 Create an object 'x' (copy from 'w').     { w:0 x:0         }
        //: 3 Set 'x' to 'A' (value distinct from 0).   { w:0 x:A         }
        //: 4 Create an object 'y' (init. to 'A').      { w:0 x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').     { w:0 x:A y:A z:A }
        //: 6 Set 'z' to 0 (the default value).         { w:0 x:A y:A z:0 }
        //: 7 Assign 'w' from 'x'.                      { w:A x:A y:A z:0 }
        //: 8 Assign 'w' from 'z'.                      { w:0 x:A y:A z:0 }
        //: 9 Assign 'x' from 'x' (aliasing).           { w:0 x:A y:A z:0 }
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        // This test intentionally uses the default allocator.

        bslma::TestAllocator testAllocator("breathing", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard guard(&testAllocator);

        const Type::Enum        TYPEA = Type::e_STRING;
        const bsl::string       VALUEA("foo");

        const bdlt::DatetimeTz  DATE(bdlt::Datetime(1999,1,1), 0);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:0             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (veryVerbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(Type::e_VOID == W.type());

        if (veryVerbose) cout <<
                          "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:0 x:0         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) cout <<
                                "\ta. Check the initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(Type::e_VOID == W.type());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 0)."
                             "\t\t{ w:0 x:A         }" << endl;

        mX.setString(VALUEA);

        if (veryVerbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(TYPEA  == X.type());
        ASSERT(VALUEA == X.theString());

        if (veryVerbose) cout <<
                     "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' and set to 'A'."
                             "\t\t{ w:0 x:A y:A     }" << endl;

        Obj mY;  const Obj& Y = mY;
        mY.setString(VALUEA);

        if (veryVerbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ P(Y) }

        ASSERT(TYPEA  == Y.type());
        ASSERT(VALUEA == Y.theString());

        if (veryVerbose) cout <<
                "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'." << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:0 x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (veryVerbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ P(Z) }

        ASSERT(TYPEA  == Z.type());
        ASSERT(VALUEA == Z.theString());


        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 0 (the default value)."
                             "\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mZ.reset();

        if (veryVerbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ P(Z) }

        ASSERT(Type::e_VOID == Z.type());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:0 }" << endl;

        mW = X;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(TYPEA  == W.type());
        ASSERT(VALUEA == W.theString());


        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mW = Z;

        if (veryVerbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ P(W) }

        ASSERT(Type::e_VOID == Z.type());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:0 x:A y:A z:0 }" << endl;

        mX = X;

        if (veryVerbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ P(X) }

        ASSERT(TYPEA  == X.type());
        ASSERT(VALUEA == X.theString());

        if (veryVerbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    // CONCERN: In no case does memory come from the default allocator.

    ASSERTV(defaultAllocator.numBlocksTotal(),
            0 == defaultAllocator.numBlocksTotal());

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
