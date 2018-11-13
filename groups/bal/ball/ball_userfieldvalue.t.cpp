// ball_userfieldvalue.t.cpp                                          -*-C++-*-
#include <ball_userfieldvalue.h>

#include <bdlt_datetime.h>

#include <bslalg_swaputil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The standard approach for testing value-semantic types is followed in the
// first 10 test cases, with remaining methods (most notably value constructors
// and the 'reset' method) tested in subsequent cases.  Since two of the five
// possible user field types can allocate memory, 'bslma::TestAllocator' and
// 'bslma::TestAllocatorMonitor' are used extensively to verify that dynamic
// memory use is as expected.  Note that due to the vagaries of member 'swap'
// for 'bdlb::Variant', memory *may* be allocated from the default allocator in
// 'case 8' (member and free 'swap').  Finally, a 'gg' function is implemented
// for this component.
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
//: o bslma::Allocator *allocator() const;  // tested in case 2
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR/MANIPULATOR/OPERATOR ptr./ref. parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Memory comes from the default allocator in 'case 8' only.
//: o Precondition violations are detected in appropriate build modes.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] UserFieldValue(Allocator *ba = 0);
// [12] UserFieldValue(bsls::Types::Int64       value, Allocator *ba = 0);
// [12] UserFieldValue(double                   value, Allocator *ba = 0);
// [13] UserFieldValue(bslstl::StringRef        value, Allocator *ba = 0);
// [12] UserFieldValue(const bdlt::DatetimeTz&  value, Allocator *ba = 0);
// [13] UserFieldValue(const bsl::vector<char>& value, Allocator *ba = 0);
// [14] UserFieldValue(INTEGRAL_TYPE            value, Allocator *ba = 0);
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
// [ 2] bslma::Allocator *allocator() const;
// [ 5] ostream& print(ostream& s, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const UserFieldValue& l, const UserFieldValue& r);
// [ 6] bool operator!=(const UserFieldValue& l, const UserFieldValue& r);
// [ 5] ostream& operator<<(ostream &os, const UserFieldValue& object);
//
// FREE FUNCTIONS
// [ 8] void swap(UserFieldValue& a, UserFieldValue& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
// [ 3] int ggg(Obj *object, const char *spec, bool verboseFlag = true);
// [ 3] Obj& gg(Obj *object, const char *spec);
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ *] CONCERN: In no case does memory come from the default allocator.
// [ *] CONCERN: All accessor methods are declared 'const'.
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

// ============================================================================
//                    EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef ball::UserFieldValue Obj;

typedef ball::UserFieldType  Type;

typedef bsls::Types::Int64   Int64;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

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

const bsl::string        S  = SUFFICIENTLY_LONG_STRING;

const char              *C1 = "one";  // *not* 'bsl::string'
const bslstl::StringRef  C2 = S;      // the interface takes 'StringRef'

const bdlt::DatetimeTz   D1(bdlt::Datetime(2000,  1,  1, 0, 1, 2,   3), 240);
const bdlt::DatetimeTz   D2(bdlt::Datetime(2025, 12, 31, 4, 5, 6, 789), -60);

static bsl::vector<char> fE1()
    // Return, *by* *value*, the 'E1' test value for user field type
    // 'e_CHAR_ARRAY'.
{
    bsl::vector<char> t;
    t.push_back('e');  t.push_back('n');  t.push_back('o');
    return t;
}
static bsl::vector<char> fE2()
    // Return, *by* *value*, the 'E2' test value for user field type
    // 'e_CHAR_ARRAY'.
{
    bsl::vector<char> t(C2.rbegin(), C2.rend());
    return t;
}
const bsl::vector<char>  E1 = fE1();
const bsl::vector<char>  E2 = fE2();

// Define DEFAULT DATA used by test cases 6, 7, 8, and 9.

struct DefaultDataRow {
    int         d_line;    // source line number
    const char *d_spec_p;  // specification string
    char        d_mem;     // allocation expected? -- 'Y' or 'N'
    char        d_type;    // type ('V' if unset)
    char        d_value;   // value ('0' if unset)
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //                  - expected -
    //LINE  SPEC  MEM   TYPE   VALUE
    //----  ----  ---   ----   -----
    { L_,   "",   'N',  'V',   '0'   },  // unset

    { L_,   "A1", 'N',  'A',   '1'   },  // 'e_INT64'
    { L_,   "A2", 'N',  'A',   '2'   },

    { L_,   "B1", 'N',  'B',   '1'   },  // 'e_DOUBLE'
    { L_,   "B2", 'N',  'B',   '2'   },

    { L_,   "C1", 'N',  'C',   '1'   },  // 'e_STRING'
    { L_,   "C2", 'Y',  'C',   '2'   },

    { L_,   "D1", 'N',  'D',   '1'   },  // 'e_DATETIMETZ'
    { L_,   "D2", 'N',  'D',   '2'   },

    { L_,   "E1", 'Y',  'E',   '1'   },  // 'e_CHAR_ARRAY'
    { L_,   "E2", 'Y',  'E',   '2'   },
};
enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

// ============================================================================
//                                 TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(true == bslma::UsesBslmaAllocator<Obj>::value);

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

Type::Enum charToFieldType(char c)
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

bool verifyFieldTypeAndValue(const Obj& object, char type, char value)
    // Return 'true' if the specified field value 'object' has the underlying
    // type and value corresponding to the specified 'gg'-specific 'type' and
    // 'value', and 'false' otherwise.
{
    ASSERT(type >= 'A' && type <= 'E');
    ASSERT('1' == value || '2' == value);

    const bool isOne = '1' == value;

    bool result = false;

    switch (type) {
      case 'A': {
        ASSERT(Type::e_INT64 == object.type());
        result = isOne ? A1 == object.theInt64()
                       : A2 == object.theInt64();
      } break;
      case 'B': {
        ASSERT(Type::e_DOUBLE == object.type());
        result = isOne ? B1 == object.theDouble()
                       : B2 == object.theDouble();
      } break;
      case 'C': {
        ASSERT(Type::e_STRING == object.type());
        result = isOne ? C1 == object.theString()
                       : C2 == object.theString();
      } break;
      case 'D': {
        ASSERT(Type::e_DATETIMETZ == object.type());
        result = isOne ? D1 == object.theDatetimeTz()
                       : D2 == object.theDatetimeTz();
      } break;
      case 'E': {
        ASSERT(Type::e_CHAR_ARRAY == object.type());
        result = isOne ? E1 == object.theCharArray()
                       : E2 == object.theCharArray();
      } break;
      default: {
        BSLS_ASSERT_OPT(false);
      } break;
    }

    return result;
}

// ============================================================================
//              GENERATOR FUNCTIONS 'gg' AND 'ggg' FOR TESTING
// ----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure the object according to a custom language.  Uppercase
// letters '[A .. E]' correspond to the five possible user field types.  Each
// of these uppercase letters must be followed by either '1' or '2' denoting
// which test value for the indicated type should be used to set the value of
// the object.  A tilde ('~') indicates that the value of the object is to be
// set to its initial, unset state (via the 'reset' method).
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
// -----------  ---------------------------------------------------------------
// "~"          Reset the object to the unset state.
//
// "A1"         Set the object to have the value corresponding to 'A1', i.e.,
//              the first of the two test values for 'e_INT64'.
//
// "B2D1"       Set the object to have the value corresponding to 'B2', then
//              set it to have the value corresponding to 'D1'.
// ----------------------------------------------------------------------------

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

    enum { SUCCESS = -1 };

    const char *input = spec;

    bool incompleteSpec = false;

    while (*input) {
        if ('~' == *input) {
            object->reset();
            ++input;
        }
        else if (*input >= 'A' && *input <= 'E') {
            const Type::Enum type = charToFieldType(*input);
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
                if (!*input) {
                    incompleteSpec = true;
                }
                break;
            }
        }
        else {
            break;
        }
    }

    if (*input || incompleteSpec) {
        const int idx = static_cast<int>(input - spec);
        if (verboseFlag) {
            printf("Error, bad character ('%c') "
                   "in spec \"%s\" at position %d.\n", *input, spec, idx);
        }
        return idx;  // Discontinue processing this spec.             // RETURN
    }

    return SUCCESS;  // All input was consumed.
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

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: Memory comes from the default allocator in 'case 8' only.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);

    switch (test) { case 0:
      case 15: {
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
      case 14: {
        // --------------------------------------------------------------------
        // INTEGRAL_TYPE VALUE CTOR
        //
        // Concerns:
        //: 1 The integral-converting value constructor (with or without a
        //:   supplied allocator) can create an object having type 'e_INT64'
        //:   from any value of integral type other than 'Int64'.
        //:
        //: 2 The argument can be 'const'.
        //:
        //: 3 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 4 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 5 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 6 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 7 There is no memory allocation from any allocator.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique)
        //:   object values (one per row) of type 'int'.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-2..7)
        //:
        //:   1 Using the default constructor and the 'setInt64' setter, create
        //:     a control object, 'W', and set it to have the value 'V'.
        //:
        //:   2 Execute an inner loop creating three distinct objects, in turn,
        //:     each object having the same value, 'V', but configured
        //:     differently: (a) without passing an allocator, (b) passing a
        //:     null allocator address explicitly, and (c) passing the address
        //:     of a test allocator distinct from the default allocator.
        //:
        //:   3 For each of the three iterations in P-2.2:  (C-2..7)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object,
        //:       'X', having the value 'V', with its object allocator
        //:       configured appropriately (see P-2.2), supplying the argument
        //:       as 'const'; use a distinct test allocator for the object's
        //:       footprint.  (C-2)
        //:
        //:     3 Verify that 'X' has the expected type and that 'X' and 'W'
        //:       have the same value.
        //:
        //:     4 Use the 'allocator' accessor to verify that the allocator is
        //:       properly installed.  (C-3..6)
        //:
        //:     5 Use the appropriate test allocators to verify that no memory
        //:       is allocated from either allocator.  (C-7)
        //:
        //: 3 Using brute-force, verify that values of the various other
        //:   integral types ('bool', 'char', 'short', etc.) also can be used
        //:   to create 'e_INT64' user fields.  (C-1)
        //
        // Testing:
        //   UserFieldValue(INTEGRAL_TYPE            value, Allocator *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "INTEGRAL_TYPE VALUE CTOR"
                          << endl << "========================" << endl;

        if (verbose) cout << "\nTesting 'int' type." << endl;
        {
            static const struct {
                int d_line;   // source line number
                int d_value;  // value for object
            } DATA[] = {
                //LINE  VALUE
                //----  -----
                { L_,   -2367,   },
                { L_,     777,   },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_line;
                const int VALUE = DATA[ti].d_value;  // note 'int', not 'Int64'

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mW(&scratch);  const Obj& W = mW;  // control

                mW.setInt64(static_cast<Int64>(VALUE));

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
                        objPtr = new (fa) Obj(VALUE);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(VALUE, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(VALUE, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // -----------------------------------
                    // Verify the object's type and value.
                    // -----------------------------------

                    ASSERTV(LINE, CONFIG, Type::e_INT64 == X.type());
                    ASSERTV(LINE, CONFIG,             W == X);

                    // ---------------------------------------
                    // Verify allocator is installed properly.
                    // ---------------------------------------

                    ASSERTV(LINE, CONFIG, &oa, X.allocator(),
                            &oa == X.allocator());

                    // Verify no allocation from either allocator.

                    ASSERTV(LINE, CONFIG, oa.numBlocksTotal(),
                            0 == oa.numBlocksTotal());

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksTotal(),
                            0 == da.numBlocksTotal());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksTotal(),
                            0 == sa.numBlocksTotal());

                }  // end foreach configuration

            }  // end foreach row
        }

        if (verbose) cout << "\nTesting other integral types." << endl;
        {
            Obj mW;  const Obj& W = mW;  // control

            {
                const bool VALUE = true;

                mW.setInt64(static_cast<Int64>(VALUE));
                const Obj X(VALUE);
                ASSERT(Type::e_INT64 == X.type());
                ASSERT(            W == X);
            }

            {
                const char VALUE = 'x';

                mW.setInt64(static_cast<Int64>(VALUE));
                const Obj X(VALUE);
                ASSERT(Type::e_INT64 == X.type());
                ASSERT(            W == X);
            }

            {
                const unsigned char VALUE = 'x';

                mW.setInt64(static_cast<Int64>(VALUE));
                const Obj X(VALUE);
                ASSERT(Type::e_INT64 == X.type());
                ASSERT(            W == X);
            }

            {
                const signed char VALUE = -3;

                mW.setInt64(static_cast<Int64>(VALUE));
                const Obj X(VALUE);
                ASSERT(Type::e_INT64 == X.type());
                ASSERT(            W == X);
            }

            {
                const short VALUE = -22334;

                mW.setInt64(static_cast<Int64>(VALUE));
                const Obj X(VALUE);
                ASSERT(Type::e_INT64 == X.type());
                ASSERT(            W == X);
            }

            {
                const unsigned short VALUE = 1276;

                mW.setInt64(static_cast<Int64>(VALUE));
                const Obj X(VALUE);
                ASSERT(Type::e_INT64 == X.type());
                ASSERT(            W == X);
            }

            {
                const unsigned int VALUE = 1276;

                mW.setInt64(static_cast<Int64>(VALUE));
                const Obj X(VALUE);
                ASSERT(Type::e_INT64 == X.type());
                ASSERT(            W == X);
            }

            {
                const bsls::Types::Uint64 VALUE = 1111222233334444ULL;

                mW.setInt64(static_cast<Int64>(VALUE));
                const Obj X(VALUE);
                ASSERT(Type::e_INT64 == X.type());
                ASSERT(            W == X);
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // ALLOCATING VALUE CTORS ('e_STRING', 'e_CHAR_ARRAY')
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value.
        //:
        //: 2 Any string arguments can be of type 'char *' or 'string'.
        //:
        //: 3 Any argument can be 'const'.
        //:
        //: 4 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 5 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 6 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 7 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 8 Any memory allocation is from the object allocator.
        //:
        //: 9 There is no temporary memory allocation from any allocator.
        //:
        //:10 Every object releases any allocated memory at destruction.
        //:
        //:11 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Specify a small set of unique object values (in turn, for each of
        //:   the allocating field types 'string' and 'vector<char>').
        //:
        //: 2 For each distinct object value, 'V', described in P-1:
        //:   (C-1, 3..10)
        //:
        //:   1 Using the default constructor and the setter for the field type
        //:     under test, create a control object, 'W', and set it to have
        //:     the value 'V'.
        //:
        //:   2 Execute an inner loop creating three distinct objects, in turn,
        //:     each object having the same value, 'V', but configured
        //:     differently: (a) without passing an allocator, (b) passing a
        //:     null allocator address explicitly, and (c) passing the address
        //:     of a test allocator distinct from the default allocator.
        //:
        //:   3 For each of the three iterations in P-2.1:  (C-1, 3..10)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object,
        //:       'X', having the value 'V', with its object allocator
        //:       configured appropriately (see P-2.1), supplying all the
        //:       arguments as 'const' and representing string arguments as
        //:       'char *' and 'string'; use a distinct test allocator for the
        //:       object's footprint.  (P-3)
        //:
        //:     3 Verify that 'X' has the expected type and that 'X' and 'W'
        //:       have the same value.  (C-1, 7)
        //:
        //:     4 Use the 'allocator' accessor to ensure that the object
        //:       allocator is properly installed.  (C-8)
        //:
        //:     5 Use the appropriate test allocators to verify that:
        //:       (C-4..6, 9..10)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-4, 6)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         default allocator doesn't allocate any memory.  (C-5)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-9)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-10)
        //:
        //: 3 Repeat the steps in P-2 for the supplied allocator configuration
        //:   (P-2.1c) on a single value from P-1, but this time create the
        //:   object as an automatic variable in the presence of injected
        //:   exceptions (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros).  (C-2, 11)
        //
        // Testing:
        //   UserFieldValue(bslstl::StringRef        value, Allocator *ba = 0);
        //   UserFieldValue(const bsl::vector<char>& value, Allocator *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "ALLOCATING VALUE CTORS ('e_STRING', 'e_CHAR_ARRAY')"
                 << endl
                 << "==================================================="
                 << endl;
        }

        if (verbose) cout << "\nTesting 'e_STRING' value ctor." << endl;
        {
            for (int ti = 1; ti <= 2; ++ti) {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mW(&scratch);  const Obj& W = mW;  // control
                1 == ti ? mW.setString(C1) : mW.setString(C2);

                const char MEM = 1 == ti ? 'N' : 'Y';

                if (veryVerbose) { T_ P_(MEM) P(W) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        if (1 == ti) {
                            objPtr = new (fa) Obj(C1);
                        }
                        else {
                            objPtr = new (fa) Obj(C2);
                        }
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        if (1 == ti) {
                            objPtr = new (fa) Obj(C1, 0);
                        }
                        else {
                            objPtr = new (fa) Obj(C2, 0);
                        }
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        if (1 == ti) {
                            objPtr = new (fa) Obj(C1, &sa);
                        }
                        else {
                            objPtr = new (fa) Obj(C2, &sa);
                        }
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // -----------------------------------
                    // Verify the object's type and value.
                    // -----------------------------------

                    ASSERTV(CONFIG, Type::e_STRING == X.type());
                    ASSERTV(CONFIG,              W == X);

                    // ---------------------------------------
                    // Verify allocator is installed properly.
                    // ---------------------------------------

                    ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    ASSERTV(CONFIG, MEM, oa.numBlocksInUse(),
                            ('N' == MEM) == (0 == oa.numBlocksTotal()));

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify no allocation from the non-object allocator.

                    ASSERTV(CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());

                }  // end foreach configuration

            }  // end foreach value
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
              if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

              Obj obj(S, &sa);
              ASSERTV(Type::e_STRING == obj.type());
              ASSERTV(             S == obj.theString());

#ifdef BDE_BUILD_TARGET_EXC
              ASSERTV(0 < EXCEPTION_COUNT);
#endif
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting 'e_CHAR_ARRAY' value ctor." << endl;
        {
            for (int ti = 1; ti <= 2; ++ti) {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mW(&scratch);  const Obj& W = mW;  // control
                1 == ti ? mW.setCharArray(E1) : mW.setCharArray(E2);

                if (veryVerbose) { T_ P(W) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        if (1 == ti) {
                            objPtr = new (fa) Obj(E1);
                        }
                        else {
                            objPtr = new (fa) Obj(E2);
                        }
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        if (1 == ti) {
                            objPtr = new (fa) Obj(E1, 0);
                        }
                        else {
                            objPtr = new (fa) Obj(E2, 0);
                        }
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        if (1 == ti) {
                            objPtr = new (fa) Obj(E1, &sa);
                        }
                        else {
                            objPtr = new (fa) Obj(E2, &sa);
                        }
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // -----------------------------------
                    // Verify the object's type and value.
                    // -----------------------------------

                    ASSERTV(CONFIG, Type::e_CHAR_ARRAY == X.type());
                    ASSERTV(CONFIG,                  W == X);

                    // ---------------------------------------
                    // Verify allocator is installed properly.
                    // ---------------------------------------

                    ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

                    // Verify expected object-memory allocations.

                    ASSERTV(CONFIG, oa.numBlocksInUse(),
                            0 < oa.numBlocksInUse());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify no allocation from the non-object allocator.

                    ASSERTV(CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());

                }  // end foreach configuration

            }  // end foreach value
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
              if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

              Obj obj(E2, &sa);
              ASSERTV(Type::e_CHAR_ARRAY == obj.type());
              ASSERTV(                E2 == obj.theCharArray());

#ifdef BDE_BUILD_TARGET_EXC
              ASSERTV(0 < EXCEPTION_COUNT);
#endif
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
            ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // NON-ALLOCATING VALUE CTORS ('e_INT64', 'e_DOUBLE', 'e_DATETIMETZ')
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value.
        //:
        //: 2 Any argument can be 'const'.
        //:
        //: 3 If an allocator is NOT supplied to the value constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 4 If an allocator IS supplied to the value constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 5 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 6 Supplying an allocator to the value constructor has no effect
        //:   on subsequent object values.
        //:
        //: 7 There is no memory allocation from any allocator.
        //
        // Plan:
        //: 1 Using the table-driven technique (in turn, for each of the
        //:   non-allocating field types 'Int64', 'double', and 'DatetimeTz'),
        //:   specify a set of (unique) object values (one per row).
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..7)
        //:
        //:   1 Using the default constructor and the setter for the field type
        //:     under test, create a control object, 'W', and set it to have
        //:     the value 'V'.
        //:
        //:   2 Execute an inner loop creating three distinct objects, in turn,
        //:     each object having the same value, 'V', but configured
        //:     differently: (a) without passing an allocator, (b) passing a
        //:     null allocator address explicitly, and (c) passing the address
        //:     of a test allocator distinct from the default allocator.
        //:
        //:   3 For each of the three iterations in P-2.2:  (C-1..7)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object,
        //:       'X', having the value 'V', with its object allocator
        //:       configured appropriately (see P-2.2), supplying the argument
        //:       as 'const'; use a distinct test allocator for the object's
        //:       footprint.  (C-2)
        //:
        //:     3 Verify that 'X' has the expected type and that 'X' and 'W'
        //:       have the same value.  (C-1)
        //:
        //:     4 Use the 'allocator' accessor to verify that the allocator is
        //:       properly installed.  (C-3..6)
        //:
        //:     5 Use the appropriate test allocators to verify that no memory
        //:       is allocated from either allocator.  (C-7)
        //
        // Testing:
        //   UserFieldValue(bsls::Types::Int64       value, Allocator *ba = 0);
        //   UserFieldValue(double                   value, Allocator *ba = 0);
        //   UserFieldValue(const bdlt::DatetimeTz&  value, Allocator *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl << "NON-ALLOCATING VALUE CTORS "
                            "('e_INT64', 'e_DOUBLE', 'e_DATETIMETZ')"
                 << endl << "==========================="
                            "======================================="
                 << endl;
        }

        if (verbose) cout << "\nTesting 'e_INT64' value ctor." << endl;
        {
            static const struct {
                int   d_line;   // source line number
                Int64 d_value;  // value for object
            } DATA[] = {
                //LINE  VALUE
                //----  -----
                { L_,   A1,   },
                { L_,   A2,   },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_line;
                const Int64 VALUE = DATA[ti].d_value;

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mW(&scratch);  const Obj& W = mW;  // control

                mW.setInt64(VALUE);

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
                        objPtr = new (fa) Obj(VALUE);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(VALUE, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(VALUE, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // -----------------------------------
                    // Verify the object's type and value.
                    // -----------------------------------

                    ASSERTV(LINE, CONFIG, Type::e_INT64 == X.type());
                    ASSERTV(LINE, CONFIG,             W == X);

                    // ---------------------------------------
                    // Verify allocator is installed properly.
                    // ---------------------------------------

                    ASSERTV(LINE, CONFIG, &oa, X.allocator(),
                            &oa == X.allocator());

                    // Verify no allocation from either allocator.

                    ASSERTV(LINE, CONFIG, oa.numBlocksTotal(),
                            0 == oa.numBlocksTotal());

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksTotal(),
                            0 == da.numBlocksTotal());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksTotal(),
                            0 == sa.numBlocksTotal());

                }  // end foreach configuration

            }  // end foreach row
        }

        if (verbose) cout << "\nTesting 'e_DOUBLE' value ctor." << endl;
        {
            static const struct {
                int    d_line;   // source line number
                double d_value;  // value for object
            } DATA[] = {
                //LINE  VALUE
                //----  -----
                { L_,   B1,   },
                { L_,   B2,   },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_line;
                const double VALUE = DATA[ti].d_value;

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mW(&scratch);  const Obj& W = mW;  // control

                mW.setDouble(VALUE);

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
                        objPtr = new (fa) Obj(VALUE);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(VALUE, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(VALUE, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // -----------------------------------
                    // Verify the object's type and value.
                    // -----------------------------------

                    ASSERTV(LINE, CONFIG, Type::e_DOUBLE == X.type());
                    ASSERTV(LINE, CONFIG,              W == X);

                    // ---------------------------------------
                    // Verify allocator is installed properly.
                    // ---------------------------------------

                    ASSERTV(LINE, CONFIG, &oa, X.allocator(),
                            &oa == X.allocator());

                    // Verify no allocation from either allocator.

                    ASSERTV(LINE, CONFIG, oa.numBlocksTotal(),
                            0 == oa.numBlocksTotal());

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksTotal(),
                            0 == da.numBlocksTotal());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksTotal(),
                            0 == sa.numBlocksTotal());

                }  // end foreach configuration

            }  // end foreach row

            if (verbose) cout << "\n\tTesting 'float'." << endl;
            {
                Obj mW;  const Obj& W = mW;  // control

                {
                    const float VALUE = 20.5;

                    mW.setDouble(static_cast<double>(VALUE));
                    const Obj X(VALUE);
                    ASSERT(Type::e_DOUBLE == X.type());
                    ASSERT(             W == X);
                }
            }
        }

        if (verbose) cout << "\nTesting 'e_DATETIMETZ' value ctor." << endl;
        {
            static const struct {
                int              d_line;   // source line number
                bdlt::DatetimeTz d_value;  // value for object
            } DATA[] = {
                //LINE  VALUE
                //----  -----
                { L_,   D1,   },
                { L_,   D2,   },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE  = DATA[ti].d_line;
                const bdlt::DatetimeTz VALUE = DATA[ti].d_value;

                if (veryVerbose) { T_ P(VALUE) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mW(&scratch);  const Obj& W = mW;  // control

                mW.setDatetimeTz(VALUE);

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
                        objPtr = new (fa) Obj(VALUE);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(VALUE, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(VALUE, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(LINE, CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // -----------------------------------
                    // Verify the object's type and value.
                    // -----------------------------------

                    ASSERTV(LINE, CONFIG, Type::e_DATETIMETZ == X.type());
                    ASSERTV(LINE, CONFIG,                  W == X);

                    // ---------------------------------------
                    // Verify allocator is installed properly.
                    // ---------------------------------------

                    ASSERTV(LINE, CONFIG, &oa, X.allocator(),
                            &oa == X.allocator());

                    // Verify no allocation from either allocator.

                    ASSERTV(LINE, CONFIG, oa.numBlocksTotal(),
                            0 == oa.numBlocksTotal());

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksTotal(),
                            0 == da.numBlocksTotal());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksTotal(),
                            0 == sa.numBlocksTotal());

                }  // end foreach configuration

            }  // end foreach row
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'reset' AND 'isUnset' METHODS
        //
        // Concerns:
        //: 1 The 'isUnset' accessor method returns 'true' if and only if the
        //:   object is in the unset state.  In particular, 'isUnset' returns
        //:   'true' if and only if the 'type' method (tested in case 4)
        //:   returns 'Type::e_VOID'.
        //:
        //: 2 The 'isUnset' accessor method is declared 'const'.
        //:
        //: 3 The 'reset' method sets any object to the unset state regardless
        //:   of the object's value before the call.
        //:
        //: 4 The 'reset' method releases all outstanding memory, if any, back
        //:   to the object allocator.
        //
        // Plan:
        //: 1 Using the default constructor, create a control object, 'U', and
        //:   verify that 'isUnset' returns 'true'.
        //:
        //: 2 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) object values (one per row),
        //:     including the "unset" value.
        //:
        //:   2 Additionally, provide a (bi-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for the associated value:
        //:     ('Y') "Yes" or ('N') "No".
        //:
        //: 3 For each row 'R' (representing a distinct object value, 'V') in
        //:   the table described in P-2:  (C-1..4)
        //:
        //:   1 Use the default constructor and the 'gg' function to configure
        //:     an object, 'X', having the value 'V'.
        //:
        //:   2 Invoke 'isUnset' on a 'const' reference to 'X' and verify that
        //:     its return value is consistent with that of the previously
        //:     testing 'type' method.  For good measure, verify that 'X'
        //:     compares equal to 'U' if and only if 'isUnset' returns 'true'.
        //:     (C-1..2)
        //:
        //:   3 Using a test allocator, verify that the memory usage of the
        //:     object allocator is consistent with the value of 'MEM' for 'V'.
        //:
        //:   4 Invoke 'reset' on 'X' and verify that the object is now in the
        //:     unset state, i.e., 'isUnset' returns 'true' and 'X' compares
        //:     equal to 'U'.  (C-3)
        //:
        //:   5 Verify that all memory, if any, is released back to the object
        //:     allocator as a consequence on calling 'reset'.  (C-4)
        //
        // Testing:
        //   void reset();
        //   bool isUnset() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'reset' AND 'isUnset' METHODS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            const Obj U;  // control (unset)

            ASSERTV(U.isUnset());

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char        MEM  = DATA[ti].d_mem;
                const char        TYPE = DATA[ti].d_type;

                if (veryVerbose) { T_ P_(SPEC) P_(MEM) P(TYPE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

                // Verify "unsetness" of the object.

                const bool isVoid = Type::e_VOID == X.type();

                ASSERTV(LINE, isVoid == X.isUnset());
                ASSERTV(LINE, isVoid == (U == X));

                // Verify memory in use from object allocator when expected.

                ASSERTV(LINE, SPEC, MEM,
                        ('Y' == MEM) == (0 < oa.numBlocksInUse()));

                mX.reset();

                // Verify object is now unset.

                ASSERTV(LINE, X.isUnset());
                ASSERTV(LINE, U == X);

                // Verify all memory, if any, is released back to object
                // allocator.

                ASSERTV(LINE, SPEC, MEM, 0 == oa.numBlocksInUse());
            }
            ASSERTV(0 == da.numBlocksTotal());
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // BDEX STREAMING
        //
        // Concerns:
        //: 1 N/A
        //
        // Plan:
        //: 1 N/A (C-1)
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
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The allocator address held by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator address held by the source object is unchanged.
        //:
        //: 8 QoI: Assigning a source object having the default-constructed
        //:   value allocates no memory.
        //:
        //: 9 Any memory allocation is exception neutral.
        //:
        //:10 Assigning an object to itself behaves as expected (alias-safety).
        //:
        //:11 Every object releases any allocated memory at destruction.
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) object values (one per row).
        //:
        //:   2 Additionally, provide a (bi-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for the associated value:
        //:     ('Y') "Yes" or ('N') "No".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the default constructor, a "scratch" allocator, and 'gg' to
        //:     create two 'const' 'Obj', 'Z' and 'ZZ', each having the value
        //:     'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the default constructor, 'oa', and 'gg' to create a
        //:       modifiable 'Obj', 'mX', having the value 'W'.
        //:
        //:     3 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     4 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     5 Use the equality-comparison operator to verify that: (C-1, 6)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-6)
        //:
        //:     6 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that the respective allocator addresses held by the target
        //:       and source objects are unchanged.  (C-2, 7)
        //:
        //:     7 Use the appropriate test allocators to verify that:
        //:       (C-8, 11)
        //:
        //:       1 For an object that (a) is initialized with a value that did
        //:         NOT require memory allocation, and (b) is then assigned a
        //:         value that DID require memory allocation, the target object
        //:         DOES allocate memory from its object allocator only
        //:         (irrespective of the specific number of allocations or the
        //:         total amount of memory allocated); also cross check with
        //:         what is expected for 'mX' and 'Z'.
        //:
        //:       2 An object that is assigned a value that did NOT require
        //:         memory allocation, does NOT allocate memory from its object
        //:         allocator; also cross check with what is expected for 'Z'.
        //:
        //:       3 No additional memory is allocated by the source object.
        //:         (C-8)
        //:
        //:       4 All object memory is released when the object is destroyed.
        //:         (C-11)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-9..10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the default constructor, 'oa', and 'gg' to create a
        //:     modifiable 'Obj' 'mX'; also use the default constructor, a
        //:     distinct "scratch" allocator, and 'gg' to create a 'const'
        //:     'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a 'const' reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   5 Verify that the address of the return value is the same as that
        //:     of 'mX'.
        //:
        //:   6 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-10)
        //:
        //:   7 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.
        //:
        //:   8 Use the appropriate test allocators to verify that:
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   UserFieldValue& operator=(const UserFieldValue& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINEI   = DATA[ti].d_line;
            const char *const SPECI   = DATA[ti].d_spec_p;
            const char        MEMSRCI = DATA[ti].d_mem;
            const char        TYPEI   = DATA[ti].d_type;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj  mZ(&scratch);  const Obj&  Z = gg(&mZ,  SPECI);
            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPECI);

            if (veryVerbose) { T_ P_(LINEI) P_(Z) P(ZZ) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINEJ   = DATA[tj].d_line;
                const char *const SPECJ   = DATA[tj].d_spec_p;
                const char        MEMDSTJ = DATA[tj].d_mem;
                const char        TYPEJ   = DATA[tj].d_type;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X = gg(&mX, SPECJ);

                    if (veryVerbose) { T_ P_(LINEJ) P(X) }

                    ASSERTV(LINEI, LINEJ, Z, X,
                            (Z == X) == (LINEI == LINEJ));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                      if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                      Obj *mR = &(mX = Z);
                      ASSERTV(LINEI, LINEJ,  Z,   X,  Z == X);
                      ASSERTV(LINEI, LINEJ, mR, &mX, mR == &mX);

#ifdef BDE_BUILD_TARGET_EXC
                      if ('N' == MEMDSTJ && 'Y' == MEMSRCI) {
                          ASSERTV(LINEI, LINEJ, 0 < EXCEPTION_COUNT);
                      }
#endif
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINEI, LINEJ, ZZ, Z, ZZ == Z);

                    ASSERTV(LINEI, LINEJ, &oa, X.allocator(),
                            &oa == X.allocator());
                    ASSERTV(LINEI, LINEJ, &scratch, Z.allocator(),
                            &scratch == Z.allocator());

                    if ('N' == MEMDSTJ && 'Y' == MEMSRCI) {
                        ASSERTV(LINEI, LINEJ, oam.isInUseUp());
                    }
                    else if ('Y' == MEMDSTJ && 'Y' == MEMSRCI) {
                        ASSERTV(LINEI, LINEJ, oam.isInUseSame());
                    }
                    else if ('Y' == MEMDSTJ && 'N' == MEMSRCI
                             && TYPEI != TYPEJ) {
                        ASSERTV(LINEI, LINEJ, 0 == oa.numBlocksInUse());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    ASSERTV(LINEI, LINEJ, sam.isInUseSame());

                    ASSERTV(LINEI, LINEJ, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINEI, LINEJ, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj  mX(&oa);                       gg(&mX,  SPECI);
                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPECI);

                const Obj& Z = mX;

                ASSERTV(LINEI, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                  if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                  Obj *mR = &(mX = Z);
                  ASSERTV(LINEI, ZZ,   Z, ZZ == Z);
                  ASSERTV(LINEI, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINEI, &oa, Z.allocator(), &oa == Z.allocator());

                ASSERTV(LINEI, !oam.isInUseUp());

                ASSERTV(LINEI, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINEI, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator address held by both objects is
        //:   unchanged.
        //:
        //: 3 Both functions may allocate memory from the object allocator
        //:   depending on the types of the field values being exchanged and
        //:   whether one or both of those types require allocation (see the
        //:   implementation of member and free 'swap' for 'bdlb::Variant').
        //:
        //: 4 Both functions have standard signatures and return types.
        //:
        //: 5 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 6 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-4)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) object values (one per row).
        //:
        //:   2 Additionally, provide a (bi-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for the associated value:
        //:     ('Y') "Yes" or ('N') "No".
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 5)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the default constructor, 'oa', and 'gg' to create a
        //:     modifiable 'Obj', 'mW', having the value described by 'R1';
        //:     also use the copy constructor and a "scratch" allocator to
        //      create a 'const' 'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-5)
        //:
        //:     1 The value is unchanged.  (C-5)
        //:
        //:     2 The allocator address held by the object is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
        //:
        //:     1 Use the copy constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the default constructor, 'oa', and 'gg' to create a
        //:       modifiable 'Obj', 'mY', having the value described by 'R2';
        //:       also use the copy constructor to create, using a "scratch"
        //:       allocator, a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //:       2 The common object allocator address held by 'mX' and 'mY'
        //:         is unchanged in both objects.  (C-2)
        //:
        //:       3 There was additional object memory allocation only when
        //:         expected.
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-6)
        //:
        //:   1 Specify a value, 'A', distinct from the value corresponding to
        //:     the default-constructed object, choosing a value that allocates
        //:     memory.
        //:
        //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   3 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX', then set it to a value that does not incur
        //:     allocation; also use the copy constructor and a "scratch"
        //:     allocator to create a 'const' 'Obj' 'XX' from 'mX'.
        //:
        //:   4 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mY', then set it to the value described in P-5.1; also
        //:     use the copy constructor and a "scratch" allocator to create a
        //:     'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-6)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There is additional object memory allocation only when
        //:       expected.  (C-6)
        //:
        //: 6 Use the test allocator from P-2 to verify that memory is
        //:   allocated from the default allocator only when expected.  (C-3)
        //:
        //: 7 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to swap objects that do not
        //:   refer to the same allocator, but not when the allocators are the
        //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-7)
        //
        // Testing:
        //   void swap(UserFieldValue& other);
        //   void swap(UserFieldValue& a, UserFieldValue& b);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            typedef void (Obj::*funcPtr)(Obj&);
            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            (void)memberSwap;  // quash potential compiler warnings
            (void)freeSwap;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINEI = DATA[ti].d_line;
            const char *const SPECI = DATA[ti].d_spec_p;
            const char        MEMI  = DATA[ti].d_mem;
            const char        TYPEI = DATA[ti].d_type;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                  Obj mW(&oa);  const Obj& W = gg(&mW, SPECI);
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINEI) P_(W) P(XX) }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                ASSERTV(LINEI, XX, W, XX == W);
                ASSERTV(LINEI, &oa == W.allocator());
                ASSERTV(LINEI, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                ASSERTV(LINEI, XX, W, XX == W);
                ASSERTV(LINEI, &oa == W.allocator());
                ASSERTV(LINEI, oam.isTotalSame());
            }

            // Verify expected ('Y'/'N') object-memory allocations.

            ASSERTV(LINEI, MEMI, oa.numBlocksInUse(),
                    ('N' == MEMI) == (0 == oa.numBlocksInUse()));

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINEJ = DATA[tj].d_line;
                const char *const SPECJ = DATA[tj].d_spec_p;
                const char        MEMJ  = DATA[tj].d_mem;
                const char        TYPEJ = DATA[tj].d_type;

                      Obj mX(XX, &oa);  const Obj& X = mX;

                      Obj mY(&oa);  const Obj& Y = gg(&mY, SPECJ);
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINEJ) P_(X) P_(Y) P(YY) }

                // Under the circumstances where variant's 'swap' resolves to
                // 'std::swap', memory allocations that may be incurred differ
                // between C++03 and C++11.

                const bool allocationsExpected =
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
                ('V' == TYPEI && 'Y' == MEMJ) || ('V' == TYPEJ && 'Y' == MEMI);
#else
                                TYPEI != TYPEJ && ('Y' == MEMI || 'Y' == MEMJ);
#endif

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    ASSERTV(LINEI, LINEJ, YY, X, YY == X);
                    ASSERTV(LINEI, LINEJ, XX, Y, XX == Y);
                    ASSERTV(LINEI, LINEJ, &oa == X.allocator());
                    ASSERTV(LINEI, LINEJ, &oa == Y.allocator());

                    if (allocationsExpected) {
                        ASSERTV(LINEI, LINEJ, oam.isTotalUp());
                    }
                    else {
                        ASSERTV(LINEI, LINEJ, oam.isTotalSame());
                    }
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    ASSERTV(LINEI, LINEJ, XX, X, XX == X);
                    ASSERTV(LINEI, LINEJ, YY, Y, YY == Y);
                    ASSERTV(LINEI, LINEJ, &oa == X.allocator());
                    ASSERTV(LINEI, LINEJ, &oa == Y.allocator());

                    if (allocationsExpected) {
                        ASSERTV(LINEI, LINEJ, oam.isTotalUp());
                    }
                    else {
                        ASSERTV(LINEI, LINEJ, oam.isTotalSame());
                    }
                }
            }

            // Record if some object memory was allocated.

            anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);

        if (verbose) cout <<
                "\nInvoke free 'swap' function in a context where ADL is used."
                                                                       << endl;
        {
            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                  Obj mX(&oa);  const Obj& X = gg(&mX, "A1");
            const Obj XX(X, &scratch);

                  Obj mY(&oa);  const Obj& Y = gg(&mY, "C2");  // allocate
            const Obj YY(Y, &scratch);

            if (veryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

            ASSERTV(YY, X, YY == X);
            ASSERTV(XX, Y, XX == Y);
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
            ASSERT(oam.isTotalSame());
#else
            ASSERT(oam.isTotalUp());
#endif

            if (veryVerbose) { T_ P_(X) P(Y) }
        }

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        // Verify no memory is allocated from the default allocator.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
#endif

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\t'swap' member function." << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(mA.swap(mB));
                ASSERT_SAFE_FAIL(mA.swap(mZ));
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as that of the supplied
        //:   original object.
        //:
        //: 2 If an allocator is NOT supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator IS supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a 'const' reference.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address held by the original object is unchanged.
        //:
        //:12 QoI: Copying an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) object values (one per row).
        //:
        //:   2 Additionally, provide a (bi-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for the associated value:
        //:     ('Y') "Yes" or ('N') "No".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..12)
        //:
        //:   1 Use the default constructor, a "scratch" allocator, and 'gg' to
        //:     create two 'const' 'Obj', 'Z' and 'ZZ', each having the value
        //:     'V'.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator,
        //:     (b) passing a null allocator address explicitly, and (c)
        //:     passing the address of a test allocator distinct from the
        //:     default.
        //:
        //:   3 For each of these three iterations (P-2.2):  (C-1..12)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object 'X',
        //:       with its object allocator configured appropriately (see
        //:       P-2.2), supplying it the 'const' object 'Z' (see P-2.1); use
        //:       a distinct test allocator for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality-comparison operator to verify that:
        //:       (C-1, 5, 10)
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         that of 'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of 'X' to verify that its object
        //:       allocator is properly installed, and use the 'allocator'
        //:       accessor of 'Z' to verify that the allocator address that it
        //:       holds is unchanged.  (C-6, 11)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so from
        //:         the object allocator only (irrespective of the specific
        //:         number of allocations or the total amount of memory
        //:         allocated).  (C-2, 4)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-12)
        //:
        //:       3 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       4 No temporary memory is allocated from the object allocator.
        //:         (C-7)
        //:
        //:       5 All object memory is released when the object is destroyed.
        //:         (C-8)
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:   (C-13)
        //
        // Testing:
        //   UserFieldValue(const UserFieldValue& original, Allocator *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char        MEM  = DATA[ti].d_mem;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj  mZ(&scratch);  const Obj&  Z = gg(&mZ,  SPEC);
                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

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
                        objPtr = new (fa) Obj(Z);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(Z, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(Z, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG,  Z, X,  Z == X);

                    // Verify that the value of 'Z' has not changed.

                    ASSERTV(LINE, CONFIG, ZZ, Z, ZZ == Z);

                    // ---------------------------------------
                    // Verify allocator is installed properly.
                    // ---------------------------------------

                    ASSERTV(LINE, CONFIG, &oa, X.allocator(),
                            &oa == X.allocator());

                    // Also invoke the 'allocator' accessor of 'Z'.

                    ASSERTV(LINE, CONFIG, &scratch, Z.allocator(),
                            &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG, oa.numBlocksTotal(),
                                                           oa.numBlocksInUse(),
                            oa.numBlocksTotal() == oa.numBlocksInUse());

                    // Verify expected ('Y'/'N') object-memory allocations.

                    if ('?' != MEM) {
                        ASSERTV(LINE, CONFIG, MEM, oa.numBlocksInUse(),
                                ('N' == MEM) == (0 == oa.numBlocksInUse()));
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, sa.numBlocksInUse(),
                            0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // value capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char        MEM  = DATA[ti].d_mem;

                if (veryVerbose) { T_ P_(SPEC) P(MEM) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                Obj  mZ(&scratch);  const Obj&  Z = gg(&mZ,  SPEC);
                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                  if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                  Obj obj(Z, &sa);
                  ASSERTV(LINE, Z, obj, Z == obj);

#ifdef BDE_BUILD_TARGET_EXC
                  if ('Y' == MEM) {
                      ASSERTV(LINE, 0 < EXCEPTION_COUNT);
                  }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, ZZ, Z, ZZ == Z);

                ASSERTV(LINE, &scratch, Z.allocator(),
                        &scratch == Z.allocator());
                ASSERTV(LINE, da.numBlocksInUse(), 0 == da.numBlocksInUse());
                ASSERTV(LINE, sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if either
        //:   both are unset, or neither is unset and the underlying variants
        //:   of 'X' and 'Y' hold objects having the same type and value.
        //:
        //: 2 The allocator does not participate in the comparison.
        //:
        //: 3 'true  == (X == X)'  (i.e., identity)
        //:
        //: 4 'false == (X != X)'  (i.e., identity)
        //:
        //: 5 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if '!(X == Y)'
        //:
        //: 8 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 9 Non-modifiable objects can be compared (i.e., 'const' objects and
        //:   'const' references).
        //:
        //:10 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:11 The equality operator's signature and return type are standard.
        //:
        //:12 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-8..9, 11..12)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row).
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..7)
        //:
        //:   1 Create a single object, using a "scratch" allocator, and
        //:     use it to verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-3..4)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..2, 5..7)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 For each of two configurations, 'a' and 'b':  (C-1..2, 5..7)
        //:
        //:       1 Create two (object) allocators, 'oax' and 'oay'.
        //:
        //:       2 Create an object 'X', using 'oax', having the value 'R1'.
        //:
        //:       3 Create an object 'Y', using 'oax' in configuration 'a' and
        //:         'oay' in configuration 'b', having the value 'R2'.
        //:
        //:       4 Verify the commutativity property and expected return value
        //:         for both '==' and '!=', while monitoring both 'oax' and
        //:         'oay' to ensure that no object memory is ever allocated by
        //:         either operator.  (C-1..2, 5..7)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-10)
        //
        // Testing:
        //   bool operator==(const UserFieldValue& l, const UserFieldValue& r);
        //   bool operator!=(const UserFieldValue& l, const UserFieldValue& r);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace ball;
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINEI = DATA[ti].d_line;
            const char *const SPECI = DATA[ti].d_spec_p;
            const char        MEMI  = DATA[ti].d_mem;

            if (veryVerbose) { T_ P_(LINEI) P(SPECI) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&scratch);  const Obj& X = gg(&mX, SPECI);

                ASSERTV(LINEI, X,   X == X);
                ASSERTV(LINEI, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINEJ = DATA[tj].d_line;
                const char *const SPECJ = DATA[tj].d_spec_p;
                const char        MEMJ  = DATA[tj].d_mem;

                if (veryVerbose) { T_ P_(LINEJ) P(SPECJ) }

                const bool EXP = ti == tj;  // expected for equality comparison

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    Obj mX(&xa);  const Obj& X = gg(&mX, SPECI);
                    Obj mY(&ya);  const Obj& Y = gg(&mY, SPECJ);

                    if (veryVerbose) { T_ T_ T_ P_(EXP) P_(CONFIG) P_(X) P(Y) }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&oax), oaym(&oay);

                    ASSERTV(LINEI, LINEJ, CONFIG, X, Y,  EXP == (X == Y));
                    ASSERTV(LINEI, LINEJ, CONFIG, Y, X,  EXP == (Y == X));

                    ASSERTV(LINEI, LINEJ, CONFIG, X, Y, !EXP == (X != Y));
                    ASSERTV(LINEI, LINEJ, CONFIG, Y, X, !EXP == (Y != X));

                    ASSERTV(LINEI, LINEJ, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINEI, LINEJ, CONFIG, oaym.isTotalSame());

                    // Double check that some object memory was allocated when
                    // expected.

                    if ('a' == CONFIG) {  // 'X' and 'Y' use same allocators.
                        ASSERTV(LINEI, LINEJ, CONFIG,
                                ('N' == MEMI && 'N' == MEMJ)
                                 || 1 <= xa.numBlocksInUse());
                    }
                    else {
                        ASSERTV(LINEI, LINEJ, CONFIG,
                                'N' == MEMI || 1 <= xa.numBlocksInUse());
                        ASSERTV(LINEI, LINEJ, CONFIG,
                                'N' == MEMJ || 1 <= ya.numBlocksInUse());
                    }

                    // Note that memory should be independently allocated for
                    // each value capable of allocating memory.
                }
            }

            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

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
        //: 1 The 'type' accessor method returns the type of the field value,
        //:   and 'Type::e_VOID' if the object is in the unset state.
        //:
        //: 2 Each "the" accessor method, when called on an object (necessarily
        //:   not in the unset state) having a value of the appropriate type,
        //:   returns the expected value.
        //:
        //: 3 Each accessor method is declared 'const'.
        //:
        //: 4 No accessor allocates any memory.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row), ensuring that the "unset" state and
        //:   at least one value for all of the possible fields types are
        //:   represented.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..4)
        //:
        //:   1 Use the default constructor, a test allocator 'oa', and 'gg' to
        //:     create a 'const' 'Obj', 'X', having the value 'V'.
        //:
        //:   2 If 'V' represents the unset state, verify that the 'type'
        //:     method returns 'Type::e_VOID'; otherwise, use the helper
        //:     function 'verifyFieldTypeAndValue' to verify that 'type'
        //:     returns the expected value and that the "the" method
        //:     corresponding to that type also returns the expected value.
        //:     (C-1..3)
        //:
        //:   3 Use a test allocator monitor to verify that no accessor
        //:     allocates any memory.  (C-4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to invoke any "the" method on
        //:   an object in the unset state or on an object having a value of a
        //:   type that doesn't correspond to the "the" method (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-5)
        //
        // Testing:
        //   ball::UserFieldType::Enum type() const;
        //   const bsls::Types::Int64& theInt64() const;
        //   const double& theDouble() const;
        //   const bsl::string& theString() const;
        //   const bdlt::DatetimeTz& theDatetimeTz() const;
        //   const bsl::vector<char>& theCharArray() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char        TYPE  = DATA[ti].d_type;
                const char        VALUE = DATA[ti].d_value;

                if (veryVerbose) { T_ P_(SPEC) P_(TYPE) P(VALUE) }

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = gg(&mX, SPEC);

                bslma::TestAllocatorMonitor oam(&oa);

                // Verify type and value of object using basic accessors.

                if ('V' == TYPE) {  // unset
                    ASSERTV(LINE, SPEC, TYPE, VALUE, Type::e_VOID == X.type());
                }
                else {
                    ASSERTV(LINE, SPEC, TYPE, VALUE, Type::e_VOID != X.type());
                    ASSERTV(LINE, SPEC, TYPE, VALUE,
                            verifyFieldTypeAndValue(X, TYPE, VALUE));
                }

                ASSERTV(LINE, SPEC, TYPE, VALUE, oam.isTotalSame());
            }
        }

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
        //:
        //: 4 All specs in the 'DEFAULT_DATA' table parse successfully.
        //
        // Plan:
        //: 1 Using the table-driven technique, create a table of test vectors
        //:   containing the line number, spec, expected return code, and the
        //:   expected field type and value.  For concern 1, make sure the
        //:   return code is the same as the specified offset where the error
        //:   occurred.  For concerns 2 and 3, verify that the resultant field
        //:   type and value are as expected.  (C-1..3)
        //:
        //: 2 Verify that 'ggg' returns -1 for all specs in the 'DEFAULT_DATA'
        //:   table.  (C-4)
        //
        // Testing:
        //   int ggg(Obj *object, const char *spec, bool verboseFlag = true);
        //   Obj& gg(Obj *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATOR FUNCTION 'gg'" << endl
                          << "=======================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nTesting generator on invalid specs." << endl;
        {
            static const struct {
                int         d_line;    // source line number
                const char *d_spec_p;  // specification string
                int         d_index;   // offending character index
            } DATA[] = {
                //LINE  SPEC            iNDEX
                //----  -------------   -----
                { L_,   "",             -1,     }, // control

                { L_,   "~",            -1,     }, // control
                { L_,   " ",             0,     },
                { L_,   ".",             0,     },
                { L_,   "F",             0,     },
                { L_,   "1",             0,     },

                { L_,   "A1",           -1,     }, // control
                { L_,   "a1",            0,     },
                { L_,   "1A",            0,     },
                { L_,   "F1",            0,     },
                { L_,   "f2",            0,     },
                { L_,   "AB",            1,     },
                { L_,   "A3",            1,     },
                { L_,   "B6",            1,     },
                { L_,   "~!",            1,     },
                { L_,   "  ",            0,     },

                { L_,   "B1~",          -1,     }, // control
                { L_,   "B~2",           1,     },
                { L_,   "B12",           2,     },
                { L_,   "A2B",           3,     },
                { L_,   "?C2",           0,     },
                { L_,   "   ",           0,     },

                { L_,   "C1D2",         -1,     }, // control
                { L_,   "C2D0",          3,     },
                { L_,   "A1D~",          3,     },
                { L_,   "A1Dx",          3,     },

                { L_,   "~C1D2",        -1,     }, // control
                { L_,   "A1C2a",         4,     },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         INDEX  = DATA[ti].d_index;
                const int         curLen = strlen(SPEC);

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\tof length "
                                          << curLen << ':' << endl;
                    ASSERTV(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) cout << "\t\tSpec = \"" << SPEC << '"'
                                      << endl;

                Obj mX;

                int result = ggg(&mX, SPEC, veryVerbose);

                ASSERTV(LINE, INDEX, result, INDEX == result);
            }
        }

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            static const struct {
                int         d_line;      // source line number
                const char *d_spec_p;    // specification string
                char        d_expType;   // expected type ('V' if unset)
                char        d_expValue;  // expected value ('0' if unset)
            } DATA[] = {
                //                   - expected -
                //LINE  SPEC         TYPE   VALUE
                //----  ---------    ----   -----
                { L_,   "",          'V',   '0'     },

                { L_,   "~",         'V',   '0'     },

                { L_,   "A1",        'A',   '1'     },
                { L_,   "E2",        'E',   '2'     },
                { L_,   "~~",        'V',   '0'     },

                { L_,   "~C2",       'C',   '2'     },
                { L_,   "D1~",       'V',   '0'     },

                { L_,   "B2D1",      'D',   '1'     },
                { L_,   "A1A1",      'A',   '1'     },

                { L_,   "A1C2~",     'V',   '0'     },
                { L_,   "B2~B1",     'B',   '1'     },
                { L_,   "~C1E2",     'E',   '2'     },

                { L_,   "E1C1D2",    'D',   '2'     },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char        TYPE  = DATA[ti].d_expType;
                const char        VALUE = DATA[ti].d_expValue;

                if (veryVerbose) { P_(LINE) P(SPEC) }

                Obj mX;

                Obj& mmX = gg(&mX, SPEC);  const Obj& X = mmX;

                ASSERTV(LINE, &mX == &X);

                if ('V' == TYPE) {
                    ASSERTV(LINE, Type::e_VOID == X.type());
                }
                else {
                    ASSERTV(LINE, verifyFieldTypeAndValue(X, TYPE, VALUE));
                }

                {
                    Obj mY;  const Obj& Y = mY;

                    ASSERTV(LINE, -1 == ggg(&mY, SPEC));
                    ASSERTV(LINE,  X == Y);
                }
            }
        }

        if (verbose) cout << "\nTesting generator on 'DEFAULT_DATA' specs."
                          << endl;
        {
            const int NUM_DATA                     = DEFAULT_NUM_DATA;
            const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const char *const SPEC = DATA[ti].d_spec_p;

                if (veryVerbose) { P_(LINE) P(SPEC) }

                Obj mX;

                ASSERTV(LINE, SPEC, -1 == ggg(&mX, SPEC));
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS & DTOR
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without
        //:   a supplied allocator) has the contractually specified default
        //:   value (unset).
        //:
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the default constructor has no effect
        //:   on subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 QoI: The default constructor allocates no memory.
        //:
        //:10 Each setter method can set the object to have any value supported
        //:   by the setter's associated type, regardless of the state of the
        //:   object prior to the call (i.e., unset or set to hold a value of
        //:   a different type).
        //:
        //:11 Any string arguments can be of type 'char *' or 'string'.
        //:
        //:12 Any argument can be 'const'.
        //:
        //:13 Any memory allocation is exception neutral.
        //:
        //:14 The 'allocator' accessor returns the address of the object
        //:   allocator.
        //:
        //:15 The 'allocator' accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Use the default constructor to create three distinct objects, in
        //:   turn, each configured differently: (a) without passing an
        //:   allocator, (b) passing a null allocator address explicitly, and
        //:   (c) passing the address of a test allocator distinct from the
        //:   default allocator.  (C-1..4, 9, 14..15)
        //:
        //:   1 Verify that the object is in the unset state using the
        //:     'isUnset' and 'type' accessors, and verify that the 'allocator'
        //:     accessor returns the address of the object allocator.
        //:     (C-1..4, 14..15)
        //:
        //:   2 Verify using test allocators that no memory is allocated from
        //:     any allocator.  (C-9)
        //:
        //: 2 Repeat P-1, but this time invoke each setter, in turn, on the
        //:   unset object.  (C-6..8, 11..12)
        //:
        //:   1 Verify 'isUnset' now returns 'false' and that 'type' returns
        //:     the expected value.  Also verify that the object has the
        //:     expected value using the appropriate "the" accessor method.
        //:     (C-11..12)
        //:
        //:   2 Verify that any memory allocation comes from the object
        //:     allocator and that there is no temporary allocation.  (C-6..7)
        //:
        //:   3 Destroy the object and verify that any allocated memory is
        //:     released.  (C-8)
        //:
        //: 3 Repeat P-2, but this time invoke each setter, in turn, on the
        //:   now non-unset object (thus exercising the setters on the
        //:   cross-product of supported variant types).  (C-5, 10)
        //:
        //:   1 Verify that object state and memory allocation, if any, are
        //:     as expected.  (C-5, 10)
        //:
        //: 4 Test 'setString' and 'setCharArray' in the presence of injected
        //:   exceptions (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*'
        //:   macros).  (C-13)
        //
        // Testing:
        //   UserFieldValue(Allocator *ba = 0);
        //   ~UserFieldValue() = default;
        //   void setInt64(bsls::Types::Int64 value);
        //   void setDouble(double value);
        //   void setString(bslstl::StringRef value);
        //   void setDatetimeTz(const bdlt::DatetimeTz& value);
        //   void setCharArray(const bsl::vector<char>& value);
        //   bslma::Allocator *allocator() const;
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

            if (veryVerbose) { P_(CONFIG) P(X) }

            ASSERTV(CONFIG,                 X.isUnset());
            ASSERTV(CONFIG, Type::e_VOID == X.type());
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
               const Type::Enum TYPE = charToFieldType(ft);

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

                switch (TYPE) {
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

                if (veryVerbose) { P_(CONFIG) P_(TYPE) P(X) }

                ASSERTV(CONFIG, TYPE, TYPE == X.type());
                ASSERTV(CONFIG, TYPE,        !X.isUnset());
                ASSERTV(CONFIG, &oa, X.allocator(), &oa == X.allocator());

                if (Type::e_STRING == TYPE || Type::e_CHAR_ARRAY == TYPE) {
                    ASSERTV(CONFIG, TYPE, oam.isInUseUp());
                }
                else {
                    ASSERTV(CONFIG, TYPE, oam.isTotalSame());
                }

                // Verify no temporary memory is allocated from the object
                // allocator.

                ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                        oa.numBlocksTotal() == oa.numBlocksInUse());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(CONFIG, TYPE, fa.numBlocksInUse(),
                        0 == fa.numBlocksInUse());

                ASSERTV(CONFIG, TYPE, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());

                if (Type::e_STRING != TYPE && Type::e_CHAR_ARRAY != TYPE) {
                    ASSERTV(CONFIG, TYPE, oa.numBlocksTotal(),
                            0 == oa.numBlocksTotal());
                }

                ASSERTV(CONFIG, TYPE, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());
            }
        }

        if (verbose) {
            cout << "\nApply each setter to non-unset object." << endl;
        }

        bool typeIAllocates = false;
        bool typeJAllocates = false;

        for (char fti = 'A'; fti <= 'E'; ++fti) {  // iterate over field types
               const Type::Enum TYPEI = charToFieldType(fti);

            for (char ftj = 'A'; ftj <= 'E'; ++ftj) {
                   const Type::Enum TYPEJ = charToFieldType(ftj);

                bslma::TestAllocator da("default", veryVeryVeryVerbose);
                bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                {
                    Obj mX(&oa);  const Obj& X = mX;

                    switch (TYPEI) {  // initial type of field
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
                    ASSERTV(TYPEI, TYPEJ, TYPEI == X.type());

                    if (veryVerbose) { Q("Before:") P_(TYPEI) P_(TYPEJ) P(X) }

                    bslma::TestAllocatorMonitor oam(&oa);

                    switch (TYPEJ) {  // new type of field
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
                    ASSERTV(TYPEI, TYPEJ, TYPEJ == X.type());

                    if (veryVerbose) { Q("After:") P_(TYPEI) P_(TYPEJ) P(X) }

                    typeIAllocates =
                        Type::e_STRING == TYPEI || Type::e_CHAR_ARRAY == TYPEI;

                    typeJAllocates =
                        Type::e_STRING == TYPEJ || Type::e_CHAR_ARRAY == TYPEJ;

                    if (typeIAllocates && !typeJAllocates) {
                        ASSERTV(TYPEI, TYPEJ, oa.numBlocksInUse(),
                                0 == oa.numBlocksInUse());
                    }
                    else if (Type::e_CHAR_ARRAY != TYPEI && typeJAllocates) {
                        ASSERTV(TYPEI, TYPEJ, oam.isInUseUp());
                    }
                    else if (Type::e_CHAR_ARRAY == TYPEI && typeJAllocates) {
                        ASSERTV(TYPEI, TYPEJ, oam.isInUseSame());
                    }
                    else {
                        ASSERTV(TYPEI, TYPEJ, oam.isTotalSame());
                    }
                }

                // Verify all memory is released on object destruction.

                ASSERTV(TYPEI, TYPEJ, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());

                if (!typeIAllocates && !typeJAllocates) {
                    ASSERTV(TYPEI, TYPEJ, oa.numBlocksTotal(),
                            0 == oa.numBlocksTotal());
                }

                ASSERTV(TYPEI, TYPEJ, da.numBlocksTotal(),
                        0 == da.numBlocksTotal());
            }
        }

        if (verbose) cout << "\nTesting 'setString' with injected exceptions."
                          << endl;
        {
            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
              if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

              Obj obj(&sa);
              obj.setString(S);
              ASSERTV(Type::e_STRING == obj.type());
              ASSERTV(             S == obj.theString());

#ifdef BDE_BUILD_TARGET_EXC
              ASSERTV(0 < EXCEPTION_COUNT);
#endif
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
        }

        if (verbose) cout <<
                          "\nTesting 'setCharArray' with injected exceptions."
                          << endl;
        {
            bslma::TestAllocator da("default",  veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
              if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

              Obj obj(&sa);
              obj.setCharArray(E2);
              ASSERTV(Type::e_CHAR_ARRAY == obj.type());
              ASSERTV(                E2 == obj.theCharArray());

#ifdef BDE_BUILD_TARGET_EXC
              ASSERTV(0 < EXCEPTION_COUNT);
#endif
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
            ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
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

        const bdlt::DatetimeTz  DATE(bdlt::Datetime(1999, 1, 1), 0);

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

    // CONCERN: Memory comes from the default allocator in 'case 8' only.

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
