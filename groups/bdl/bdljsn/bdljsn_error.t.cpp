// bdljsn_error.t.cpp                                                 -*-C++-*-
#include <bdljsn_error.h>

#include <bdljsn_location.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslh_hash.h>       // 'bslh::hashAppend'

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>
#include <bsls_types.h>      // 'bsls::Types::Int64'

#include <bsl_cstddef.h>     // 'bsl::size_t'
#include <bsl_cstdint.h>     // 'bsl::uint64_t', 'UINT64_MAX'
#include <bsl_cstdlib.h>     // 'bsl::realloc', 'bsl::free', 'bsl::atoi'
#include <bsl_cstring.h>     // 'bsl::memcmp'
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>     // 'operator<<'
#include <bsl_sstream.h>     // 'bsl::ostringstream'
#include <bsl_string_view.h>
#include <bsl_utility.h>     // 'bsl::pair', 'bsl::make_pair'

using namespace BloombergLP;
using bsl::cout;
using bsl::endl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a single, unconstrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulators:
//: o 'setLocation'
//: o 'setMessage'
//
// Basic Accessors:
//: o 'allocator' (orthogonal to value)
//: o 'location'
//: o 'message'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will test the value constructor in case 3 (in lieu of
// the generator function), with the default constructor and primary
// manipulators tested fully in case 2.
//
// Certain standard value-semantic-type test cases are omitted:
//: o [ x] -- BSLX streaming is not (yet) implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
//: o Injected exceptions are safely propagated during memory allocation.
//: o Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//: o All explicit memory allocations are presumed to use the global, default,
//:   or object allocator.
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
//:   o swap
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] Error();
// [ 2] explicit Error(bslma::Allocator *basicAllocator);
// [ 3] Error(const Location& l, const bsl::string_view& m, bA = 0);
// [ 7] Error(const Error& original, *bA = 0);
// [ 8] Error(bslmf::MovableRef<Error> original)
// [ 8] Error(bslmf::MovableRef<Error> original, ba = 0);
// [ 2] ~Error();
//
// MANIPULATORS
// [10] Error& operator=(const Error& rhs);
// [11] Error& operator=(bslmf::MovableRef<Error> rhs);
// [12] Error& reset();
// [ 2] Error& setLocation(const Location& value);
// [ 2] Error& setMessage(const bsl::string_view& value);
//
// [ 9] void swap(Error& other);
//
// ACCESSORS
// [ 4] const Location& location() const;
// [ 4] const bsl::string& message() const;
//
// [ 4] bslma::Allocator *allocator() const;
// [ 5] ostream& print(ostream& stream, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const Error& lhs, const Error& rhs);
// [ 6] bool operator!=(const Error& lhs, const Error& rhs);
// [ 5] ostream& operator<<(ostream& stream, const Error& object);
//
// FREE FUNCTIONS
// [13] void hashAppend(HASHALG& algorithm, const Error& object);
// [ 9] void swap(Error& a, Error& b);
// ----------------------------------------------------------------------------
// [14] USAGE EXAMPLE
// [ 1] BREATHING TEST
// [ *] CONCERN: This test driver is reusable w/other, similar components.
// [ *] CONCERN: In no case does memory come from the global allocator.
// [ 3] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 5] CONCERN: All accessor methods are declared 'const'.
// [ 3] CONCERN: String arguments can be either 'char *' or 'string'.
// [10] CONCERN: All memory allocation is from the object's allocator.
// [10] CONCERN: All memory allocation is exception neutral.
// [10] CONCERN: Object value is independent of the object allocator.
// [10] CONCERN: There is no temporary allocation from any allocator.
// [ 9] CONCERN: Precondition violations are detected when enabled.
// [ x] Reserved for 'bslx' streaming.

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

// ============================================================================
//                      CONVENIENCE MACROS
// ----------------------------------------------------------------------------

// For use in ASSERTV macro invocations to print allocator.
#define ALLOC_OF(EXPR) (EXPR).allocator()

// ============================================================================
//                    EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdljsn::Error    Obj;
typedef bdljsn::Location Loc;
typedef bsl::uint64_t    Uint64;
typedef bsl::int64_t     Int64;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
BSLMF_ASSERT(bslmf::IsBitwiseMoveable<Obj>::value);

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

const char *const LONG_STRING    = "a_"   SUFFICIENTLY_LONG_STRING;
const char *const LONGER_STRING  = "ab_"  SUFFICIENTLY_LONG_STRING;
const char *const LONGEST_STRING = "abc_" SUFFICIENTLY_LONG_STRING;

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

class MockAccumulatingHashingAlgorithm {
    // This class implements a mock hashing algorithm that provides a way to
    // accumulate and then examine data that is being passed into hashing
    // algorithms by 'hashAppend'.

    void   *d_data_p;  // Data we were asked to hash
    size_t  d_length;  // Length of the data we were asked to hash

  public:
    MockAccumulatingHashingAlgorithm()
    : d_data_p(0)
    , d_length(0)
        // Create an object of this type.
    {
    }

    ~MockAccumulatingHashingAlgorithm()
        // Destroy this object
    {
        bsl::free(d_data_p);
    }

    void operator()(const void *voidPtr, size_t length)
        // Append the data of the specified 'length' at 'voidPtr' for later
        // inspection.
    {
        d_data_p = bsl::realloc(d_data_p, d_length += length);
        bsl::memcpy(getData() + d_length - length, voidPtr, length);
    }

    char *getData()
        // Return a pointer to the stored data.
    {
        return static_cast<char *>(d_data_p);
    }

    size_t getLength() const
        // Return the length of the stored data.
    {
        return d_length;
    }
};

// FREE OPERATORS
bool operator==(MockAccumulatingHashingAlgorithm& lhs,
                MockAccumulatingHashingAlgorithm& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value and
    // 'false' otherwise.  Two objects have the same value if they both have
    // the same length and if they both point to data that compares equal.
    // Note that for simplicity of implementation the arguments are
    // (unconventionally) non-'const'.
{
    if (lhs.getLength() != rhs.getLength()) {
        return false;                                                 // RETURN
    }

    return 0 == bsl::memcmp(lhs.getData(), rhs.getData(), rhs.getLength());
}

// ============================================================================
//                              USAGE EXAMPLES
// ----------------------------------------------------------------------------

namespace Example1 {

struct MyParseStatus {

    // TYPES
    enum Enum {
        e_OK,
        e_NG
    };

    // CLASS METHODS
    static const char *toAscii(MyParseStatus::Enum value);
        // Return the non-modifiable string representation corresponding to the
        // specified enumeration 'value', if it exists, and a unique (error)
        // string otherwise.  The string representation of 'value' matches its
        // corresponding enumerator name with the "e_" prefix elided.  For
        // example:
        //..
        //  bsl::cout << JsonType::toAscii(MyParseStatus::e_OK);
        //..
        // will print the following on standard output:
        //..
        //  OK
        //..
        // Note that specifying a 'value' that does not match any of the
        // enumerators will result in a string representation that is distinct
        // from any of those corresponding to the enumerators, but is otherwise
        // unspecified.
};

const char *MyParseStatus::toAscii(MyParseStatus::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
        CASE(OK)
        CASE(NG)

      default: return "(* UNKNOWN *)";
    }
#undef CASE
}

struct MyNumericUtil {

    static bsl::pair<MyParseStatus::Enum, unsigned> parseInt(
                                                       int              *value,
                                                       bsl::string_view  text);
        // Load to the specified 'value' the 'int' value represented in the
        // specified 'text'.  Return
        // 'bsl::make_pair(MyParseStatus::e_OK, text.length())' on success, and
        // a pair consisting of some other, appropriate 'MyParseStatus::Enum'
        // value and the position in 'text' where the parse error was detected.
};

bsl::pair<MyParseStatus::Enum, unsigned>
MyNumericUtil::parseInt(int              *value,
                        bsl::string_view  text)
{
    BSLS_ASSERT(value);
    BSLS_ASSERT("-99" == text);

    *value = -99;
    return bsl::make_pair(MyParseStatus::e_OK, 3u);
}

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Populating an 'bdljsn::Error' Object
///- - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to describe an error that occured when processing
// a (JSON) document.  Suppose we are implementing a function,
// 'extractIntegerToken', that parses a numeric token and obtains an 'int'
// value:
//
// First, we define the function signature:
//..
    int extractIntegerToken(int              *value,
                            bdljsn::Error    *error,
                            bsl::string_view  inputText)
        // Load to the specified 'value' the 'int' value represented by the
        // specified 'inputText'.  Return 0 on success, and a non-zero value
        // otherwise with no effect on '*value' and the specified 'error' is
        // set.
    {
        BSLS_ASSERT(value);
        BSLS_ASSERT(error);

        enum { e_SUCCESS, e_FAILURE };
        // ...
//..
// Then, we attempt to exact a 'int' value from the 'inputText':
//..
        int                                      result;
        bsl::pair<MyParseStatus::Enum, unsigned> status =
                                   MyNumericUtil::parseInt(&result, inputText);
//..
// Now, we check the parse status and if unsuccessful, we use the status
// information to set the 'bsljsn::Error' object expected by our caller:
//..
        if (MyParseStatus::e_OK != status.first) {
            unsigned position = status.second;
            error->setLocation(bdljsn::Location(static_cast<bsl::uint64_t>(
                                                                   position)));
            error->setMessage(MyParseStatus::toAscii(status.first));
            return e_FAILURE;                                         // RETURN
        }
//..
// Finally, if the parse was successful, set the output parameter and return
// with status value that indicates success.
//..
        *value = result;
        return e_SUCCESS;
    }
//..

}  // close namespace Example1

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // Define 'DEFAULT_DATA' that is used by:
    //..
    //   +-------+--------------------------------+
    //   | Case# | Description                    |
    //   +-------+--------------------------------+
    //   |     3 | VALUE CONSTRUCTOR              |
    //   |     7 | COPY  CONSTRUCTOR              |
    //         8 | MOVE  CONSTRUCTOR              |
    //   |     9 | SWAP MEMBER AND FREE FUNCTIONS |
    //   |    10 | COPY-ASSIGNMENT OPERATOR       |
    //   |    11 | MOVE-ASSIGNMENT OPERATOR       |
    //   |    12 | TEST 'reset'                   |
    //   |    13 | TEST 'hashAppend'              |
    //   +-------+--------------------------------+
    //..

    const Int64 MAX_INT64 = bsl::numeric_limits<Int64>::max();

    struct DefaultDataRow {
        int         d_line;           // source line number
        char        d_mem;            // expected allocation: 'Y', 'N', '?'
        Uint64      d_offset;
        const char *d_message_p;
    };

    const DefaultDataRow DEFAULT_DATA[] =
    {
        //LINE  MEM   OFFSET            MESSAGE
        //----  ---   ----------------  -------------

        // default (must be first)
        { L_,   'N',              0ULL, ""             },

        // 'offset'
    //  { L_,   'N',              0ULL, ""             }, // default is "min"
        { L_,   'N',              1ULL, ""             },
        { L_,   'N',  MAX_INT64 - 1ULL, ""             },
        { L_,   'N',  MAX_INT64 - 0ULL, ""             },

        // 'message'
        { L_,   '?',              0ULL, "a"            },
        { L_,   '?',              0ULL, "AB"           },
        { L_,   '?',              0ULL, "1234567890"   },
        { L_,   'Y',              0ULL, LONG_STRING    },
        { L_,   'Y',              0ULL, LONGER_STRING  },
        { L_,   'Y',              0ULL, LONGEST_STRING },

        // other
        { L_,   '?',              2ULL, "a"            },
        { L_,   'Y',  MAX_INT64 - 0ULL, LONG_STRING    },
    };
    enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

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

        if (verbose) {
            bsl::cout << bsl::endl
                      << "USAGE EXAMPLE" << bsl::endl
                      << "=============" << bsl::endl;
        }

        int value;
        Obj error;
        int rc = Example1::extractIntegerToken(&value,
                                               &error,
                                               "-99");
        ASSERT(    0 == rc);
        ASSERT(  -99 == value);
        ASSERT(Obj() == error);

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TEST 'hashAppend'
        //   Verify that the 'hashAppend' free functions have been implemented
        //   for all of the fundamental types and do not truncate or pass extra
        //   data into the algorithms.
        //
        // Concerns:
        //: 1 The 'hashAppend' function ("the function") uses the specified
        //:   algorithm (and no other).
        //:
        //: 2 The function passes the object's value to the supplied algorithm.
        //:
        //: 3 No temporary memory is allocated from the default allocator.
        //
        // Plan:
        //: 1 Use a locally defined algorithm class,
        //:   'MockAccumulatingHashingAlgorithm', to show that 'hashAppend'
        //:   uses the supplied algorithm object.  (C-1)
        //:
        //: 2 The 'MockAccumulatingHashingAlgorithm' functor stores a
        //:   concatenation of each of the input supplied.  This allows one to
        //:   compare the results of independent invocations of the algorithm
        //:   on the object's value with invocations on the object itself.
        //:
        //: 4 In P-2 use elements of different representative values that
        //:   include the extremes of the allowed range.  Any omission,
        //:   duplication, or mis-ordering of element visits would become
        //:   manifest when comparing the concatenations of the two
        //:   computations.  (C-2)
        //:
        //: 5 A test allocator is installed as the default allocator and
        //:   checked to confirm that it received no allocation requests.
        //:   (C-3).
        //
        // Testing:
        //   void hashAppend(HASHALG& algorithm, const Error& object);
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST 'hashAppend'" << endl
                          << "=================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const Uint64      OFFSET  = DATA[ti].d_offset;
            const char *const MESSAGE = DATA[ti].d_message_p;
            const Loc         LOC(OFFSET);

            Obj mX(LOC, MESSAGE); const Obj& X = mX;

            if (veryVerbose) { T_ P_(LINE) P(X) }

            MockAccumulatingHashingAlgorithm expd;
            MockAccumulatingHashingAlgorithm calc;

            bdljsn::hashAppend(expd, LOC);
            using bslh::hashAppend;
            hashAppend(expd, bsl::string(MESSAGE));

            bdljsn::hashAppend(calc, X);                                // TEST

            ASSERTV(LINE, OFFSET, MESSAGE, expd == calc);
        }

        ASSERT(dam.isInUseSame());
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TEST 'reset'
        //
        // Concerns
        //: 1 A reset object equals the default constructed object,
        //:   irrespective of the object's initial state.
        //:
        //: 2 The 'reset' method returns a non-'const' reference to the target
        //:   object (i.e., '*this').
        //
        //: 3 The object uses the same allocator before and after 'reset'.
        //:
        //: 4 No memory held by the object is deallocated.
        //
        //: 5 No additional (e.g., temporary) object memory is allocated from
        //:   the default allocator.
        //
        //  Plan
        //: 1 Install a test allocator as the default allocator.
        //
        //: 2 For each set of values in 'DEFAULT_VALUES' (which include
        //:   the default value and other values that represent
        //:   the maximum offset and both allocating and non-allocating
        //:   message values).
        //:
        //:   1 Create an test object that uses a test allocator to supply
        //:     memory.
        //:
        //:   2 Invoke the 'reset method'.
        //:
        //:   3 Confirm that return value (reference) is non-'const' and
        //:     shows the same address as the test object.
        //:
        //:   4 Confirm that the reset object equals the default object.
        //:
        //:   5 Confirm that the object's allocator is the same.
        //:
        //:   6 Confirm that the object has not deallocated memory.
        //
        //:   7 There are no outstanding allocations from the object (test)
        //:     allocator.
        //:
        //: 3 Confirm that there have been no allocations from the default
        //:   (test) allocator.
        //
        // Testing:
        //   Error& reset();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST 'reset'" << endl
                          << "============" << endl;

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE    = DATA[ti].d_line;
            const char        MEM     = DATA[ti].d_mem;
            const Uint64      OFFSET  = DATA[ti].d_offset;
            const char *const MESSAGE = DATA[ti].d_message_p;
            const Loc         LOC(OFFSET);

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(LOC, MESSAGE, &oa); const Obj& X = mX;

            if (veryVerbose) { T_ P_(LINE) P(X) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE, Obj(), X, Obj() == X);
                firstFlag = false;
            }

            const bsls::Types::Int64 numBlocksInUse = oa.numBlocksInUse();

            if (veryVeryVerbose) {
                P(numBlocksInUse);
            }

            if ('Y' == MEM) {
                ASSERT(0 < numBlocksInUse);
            }

            anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

            Obj *mR = &(mX.reset());

            ASSERTV(LINE, mR, &mX, mR    == &mX);
            ASSERTV(LINE,          Obj() ==   X);
            ASSERTV(LINE,          &oa   ==   X.allocator());

            if (veryVeryVerbose) {
                P(numBlocksInUse);
            }

            ASSERTV(LINE, numBlocksInUse,   oa.numBlocksInUse(),
                          numBlocksInUse == oa.numBlocksInUse());
        }

        ASSERT(anyObjectMemoryAllocatedFlag);
        ASSERTV(0 == da.numBlocksTotal());
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // MOVE-ASSIGNMENT OPERATOR
        //   Ensure that we can move the value of any object of the class to
        //   any object of the class, such that the target object subsequently
        //   has the source value, and there are no additional allocations if
        //   only one allocator is being used, and the source object is
        //   unchanged if allocators are different.
        //
        // Concerns:
        //: 1 The move assignment operator can change the value of any
        //:   modifiable target object to that of any source object.
        //:
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 If the allocators are different, the value of the source object
        //:   is not modified.
        //:
        //: 7 If the allocators are the same, no new allocations happen when
        //:   the move assignment happens.
        //:
        //: 8 The allocator used by the source object is unchanged.
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
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..3, 5-6,8-11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' objects 's1'.
        //:
        //:     2 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mF', having the value 'V'.
        //:
        //:     3 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     4 Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mF)'.
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'Z'.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'mF' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //:     8 Use the appropriate test allocators to verify that no new
        //:       allocations were made by the move assignment operation.
        //:
        //:   4 For each of the iterations (P-4.2):  (C-1..2, 5, 7-9, 11)
        //:
        //:     1 Create two 'bslma::TestAllocator' objects 's1' and 's2'.
        //:
        //:     2 Use the value constructor and 's1' to create a modifiable
        //:       'Obj', 'mF', having the value 'V'.
        //:
        //:     3 Use the value constructor and 's2' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
        //:
        //:     4 Move-assign 'mX' from 'bslmf::MovableRefUtil::move(mF)'.
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality-comparison operator to verify that the
        //:       target object, 'mX', now has the same value as that of 'Z'.
        //:
        //:     7 Use the equality-comparison operator to verify that the
        //:       source object, 'mF', now has the same value as that of 'Z'.
        //:
        //:     8 Use the 'allocator' accessor of both 'mX' and 'mF' to verify
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3:  (C-10)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   3 Let 'Z' be a  reference to 'mX'.
        //:
        //:   4 Assign 'mX' from 'bslmf::MovableRefUtil::move(Z)'.
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
        //   Error& operator=(bslmf::MovableRef<Error> rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(bslmf::MovableRef<Obj>);

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
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1    = DATA[ti].d_line;
            const char        MEMSRC1  = DATA[ti].d_mem;
            const Uint64      OFFSET1  = DATA[ti].d_offset;
            const char *const MESSAGE1 = DATA[ti].d_message_p;
            const Loc         LOC1(OFFSET1);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj Z (LOC1, MESSAGE1, &scratch);
            const Obj ZZ(LOC1, MESSAGE1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            // move assignment with the same allocator

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2    = DATA[tj].d_line;
                const Uint64      OFFSET2  = DATA[tj].d_offset;
                const char *const MESSAGE2 = DATA[tj].d_message_p;
                const Loc         LOC2(OFFSET2);

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);

                {
                    // Test move assignment with same allocator.

                    Obj mF(LOC1, MESSAGE1, &s1); const Obj& F=mF;
                    Obj mX(LOC2, MESSAGE2, &s1); const Obj& X=mX;

                    if (veryVerbose) { T_ P_(LINE2) P(F) P(X) }

                    ASSERTV(LINE1, LINE2, F, X, (F == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor s1m(&s1);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));
                    ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                    ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                    ASSERTV(LINE1, LINE2, s1m.isTotalSame());

                    ASSERTV(LINE1, LINE2, &s1, ALLOC_OF(X),
                            &s1 == X.allocator());
                    ASSERTV(LINE1, LINE2, &s1, ALLOC_OF(F),
                            &s1 == F.allocator());

                    anyObjectMemoryAllocatedFlag |= !!s1.numBlocksInUse();
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, s1.numBlocksInUse(),
                        0 == s1.numBlocksInUse());
            }

            // move assignment with different allocators

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2    = DATA[tj].d_line;
                const char        MEMDST2  = DATA[tj].d_mem;
                const Uint64      OFFSET2  = DATA[tj].d_offset;
                const char *const MESSAGE2 = DATA[tj].d_message_p;
                const Loc         LOC2(OFFSET2);

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);
                bslma::TestAllocator s2("scratch2", veryVeryVeryVerbose);

                {
                    // Test move assignment with different allocator

                    Obj mF(LOC1, MESSAGE1, &s1); const Obj& F=mF;
                    Obj mX(LOC2, MESSAGE2, &s2); const Obj& X=mX;

                    if (veryVerbose) { T_ P_(LINE2) P(F) P(X) }

                    ASSERTV(LINE1, LINE2, F, X, (F == X) == (LINE1 == LINE2));

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mF));
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                        ASSERTV(LINE1, LINE2,  Z,   F,  Z == F);

                        ASSERTV(LINE1, LINE2, &s2, ALLOC_OF(X),
                               &s2 == X.allocator());
                        ASSERTV(LINE1, LINE2, &s1, ALLOC_OF(F),
                               &s1 == F.allocator());

#ifdef BDE_BUILD_TARGET_EXC
                        if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                            ASSERTV(LINE1, LINE2, 0 < EXCEPTION_COUNT);
                        }
#endif
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    anyObjectMemoryAllocatedFlag |= !!s1.numBlocksInUse();
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, s1.numBlocksInUse(),
                        0 == s1.numBlocksInUse());
                ASSERTV(LINE1, LINE2, s2.numBlocksInUse(),
                        0 == s2.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj       mX(LOC1, MESSAGE1, &oa);
                const Obj ZZ(LOC1, MESSAGE1, &scratch);

                Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(Z));
                ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);

                ASSERTV(LINE1, &oa, ALLOC_OF(Z), &oa == Z.allocator());

                ASSERTV(LINE1, oam.isTotalSame());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);
      } break;
      case 10: {
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
        //: 2 The allocator used by the target object is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator used by the source object is unchanged.
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
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1..2, 5..8, 11)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):  (C-1..2, 5..8, 11)
        //:
        //:     1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', having the value 'W'.
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
        //:       that the respective allocators used by the target and source
        //:       objects are unchanged.  (C-2, 7)
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
        //:   2 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mX'; also use the value constructor and a distinct "scratch"
        //:     allocator to create a 'const' 'Obj' 'ZZ'.
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
        //   Error& operator=(const Error& rhs);
        //   CONCERN: All memory allocation is from the object's allocator.
        //   CONCERN: All memory allocation is exception neutral.
        //   CONCERN: Object value is independent of the object allocator.
        //   CONCERN: There is no temporary allocation from any allocator.
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
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1    = DATA[ti].d_line;
            const char        MEMSRC1  = DATA[ti].d_mem;
            const Uint64      OFFSET1  = DATA[ti].d_offset;
            const char *const MESSAGE1 = DATA[ti].d_message_p;
            const Loc         LOC1(OFFSET1);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj Z (LOC1, MESSAGE1, &scratch);
            const Obj ZZ(LOC1, MESSAGE1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2    = DATA[tj].d_line;
                const char        MEMDST2  = DATA[tj].d_mem;
                const Uint64      OFFSET2  = DATA[tj].d_offset;
                const char *const MESSAGE2 = DATA[tj].d_message_p;
                const Loc         LOC2(OFFSET2);

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(LOC2, MESSAGE2, &oa);  const Obj& X = mX;

                    if (veryVerbose) { T_ P_(LINE2) P(X) }

                    ASSERTV(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        Obj *mR = &(mX = Z);
                        ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                        ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

#ifdef BDE_BUILD_TARGET_EXC
                        if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                            ASSERTV(LINE1, LINE2, 0 < EXCEPTION_COUNT);
                        }
#endif

                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);

                    ASSERTV(LINE1, LINE2, &oa, ALLOC_OF(X),
                            &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, &scratch, ALLOC_OF(Z),
                            &scratch == Z.allocator());

                    if ('N' == MEMDST2 && 'Y' == MEMSRC1) {
                        ASSERTV(LINE1, LINE2, oam.isInUseUp());
                    }
                    else if ('Y' == MEMDST2) {
                        ASSERTV(LINE1, LINE2, oam.isInUseSame());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    ASSERTV(LINE1, LINE2, sam.isInUseSame());

                    ASSERTV(LINE1, LINE2, 0 == da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                ASSERTV(LINE1, LINE2, oa.numBlocksInUse(),
                        0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj       mX(LOC1, MESSAGE1, &oa);
                const Obj ZZ(LOC1, MESSAGE1, &scratch);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = Z);
                    ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                    ASSERTV(LINE1, mR, &mX, mR == &mX);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE1, &oa, ALLOC_OF(Z), &oa == Z.allocator());

                ASSERTV(LINE1, !oam.isInUseUp());

                ASSERTV(LINE1, 0 == da.numBlocksTotal());
            }

            // Verify all object memory is released on destruction.

            ASSERTV(LINE1, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }

        // Double check that some object memory was allocated.

        ASSERT(anyObjectMemoryAllocatedFlag);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that the free 'swap' function is implemented and can
        //   exchange the values of any two objects.  Ensure that member
        //   'swap' is implemented and can exchange the values of any two
        //   objects that use the same allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator used by both objects is unchanged.
        //:
        //: 3 The member function does not allocate memory from any allocator;
        //:   nor does the free function when the two objects being swapped use
        //:   the same allocator.
        //:
        //: 4 The free function can be called with two objects that use
        //:   different allocators.
        //:
        //: 5 Both functions have standard signatures and return types.
        //:
        //: 6 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 7 The free 'swap' function is discoverable through ADL (Argument
        //:   Dependent Lookup).
        //:
        //: 8 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, member-function
        //:   and free-function pointers having the appropriate signatures and
        //:   return types.  (C-5)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..2, 6)
        //:
        //:   1 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor and 'oa' to create a modifiable
        //:     'Obj', 'mW', having the value described by 'R1'; also use the
        //:     copy constructor and a "scratch" allocator to create a 'const'
        //:     'Obj' 'XX' from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself; verify, after each swap, that:  (C-6)
        //:
        //:     1 The value is unchanged.  (C-6)
        //:
        //:     2 The allocator used by the object is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:  (C-1..2)
        //:
        //:     1 Use the copy constructor and 'oa' to create a modifiable
        //:       'Obj', 'mX', from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor and 'oa' to create a modifiable
        //:       'Obj', 'mY', having the value described by 'R2'; also use
        //:       the copy constructor to create, using a "scratch" allocator,
        //:       a 'const' 'Obj', 'YY', from 'Y'.
        //:
        //:     3 Use, in turn, the member and free 'swap' functions to swap
        //:       the values of 'mX' and 'mY'; verify, after each swap, that:
        //:       (C-1..2)
        //:
        //:       1 The values have been exchanged.  (C-1)
        //:
        //:       2 The common object allocator used by 'mX' and 'mY'
        //:         is unchanged in both objects.  (C-2)
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Verify that the free 'swap' function is discoverable through ADL:
        //:   (C-7)
        //:
        //:   1 Create a set of attribute values, 'A', distinct from the values
        //:     corresponding to the default-constructed object, choosing
        //:     values that allocate memory if possible.
        //:
        //:   2 Create a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   3 Use the default constructor and 'oa' to create a modifiable
        //:     'Obj' 'mX' (having default attribute values); also use the copy
        //:     constructor and a "scratch" allocator to create a 'const' 'Obj'
        //:     'XX' from 'mX'.
        //:
        //:   4 Use the value constructor and 'oa' to create a modifiable 'Obj'
        //:     'mY' having the value described by the 'Ai' attributes; also
        //:     use the copy constructor and a "scratch" allocator to create a
        //:     'const' 'Obj' 'YY' from 'mY'.
        //:
        //:   5 Use the 'bslalg::SwapUtil' helper function template to swap the
        //:     values of 'mX' and 'mY', using the free 'swap' function defined
        //:     in this component, then verify that:  (C-7)
        //:
        //:     1 The values have been exchanged.
        //:
        //:     2 There was no additional object memory allocation.  (C-7)
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory was
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 7 Verify that free 'swap' exchanges the values of any two objects
        //:   that use different allocators.  (C-4)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when, using the member 'swap' function, an attempt is
        //:   made to swap objects that do not refer to the same allocator, but
        //:   not when the allocators are the same (using the
        //:   'BSLS_ASSERTTEST_*' macros).  (C-8)
        //
        // Testing:
        //   void swap(Error& a, Error& b);
        //   void swap(Error& other);
        //   CONCERN: Precondition violations are detected when enabled.
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
            freeFuncPtr freeSwap   = bsl::swap;

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
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1    = DATA[ti].d_line;
            const char        MEM1     = DATA[ti].d_mem;
            const Uint64      OFFSET1  = DATA[ti].d_offset;
            const char *const MESSAGE1 = DATA[ti].d_message_p;
            const Loc         LOC1(OFFSET1);

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj       mW(LOC1, MESSAGE1, &oa);  const Obj& W = mW;
            const Obj XX(W, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(W) P(XX) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), W, Obj() == W);
                firstFlag = false;
            }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                ASSERTV(LINE1, XX, W, XX == W);
                ASSERTV(LINE1, &oa == W.allocator());
                ASSERTV(LINE1, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                ASSERTV(LINE1, XX, W, XX == W);
                ASSERTV(LINE1, &oa == W.allocator());
                ASSERTV(LINE1, oam.isTotalSame());
            }

            // Verify expected ('Y'/'N') object-memory allocations.

            if ('?' != MEM1) {
                ASSERTV(LINE1, MEM1, oa.numBlocksInUse(),
                        ('N' == MEM1) == (0 == oa.numBlocksInUse()));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2    = DATA[tj].d_line;
                const Uint64      OFFSET2  = DATA[tj].d_offset;
                const char *const MESSAGE2 = DATA[tj].d_message_p;
                const Loc         LOC2(OFFSET2);

                Obj       mX(XX, &oa);              const Obj& X = mX;
                Obj       mY(LOC2, MESSAGE2, &oa);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    ASSERTV(LINE1, LINE2, YY, X, YY == X);
                    ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                    ASSERTV(LINE1, LINE2, &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, &oa == Y.allocator());
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                }

                // free function 'swap', same allocator
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    ASSERTV(LINE1, LINE2, XX, X, XX == X);
                    ASSERTV(LINE1, LINE2, YY, Y, YY == Y);
                    ASSERTV(LINE1, LINE2, &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, &oa == Y.allocator());
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
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
            // 'A' values: Should cause memory allocation if possible.

            const Loc  A1   = Loc(bsl::numeric_limits<Uint64>::max());
            const char A2[] = "a_" SUFFICIENTLY_LONG_STRING;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj       mX(&oa);          const Obj& X = mX;
            const Obj XX(X, &scratch);

            Obj       mY(A1, A2, &oa);  const Obj& Y = mY;
            const Obj YY(Y, &scratch);

            if (veryVeryVerbose) { T_ P_(X) P(Y) }

            bslma::TestAllocatorMonitor oam(&oa);

            bslalg::SwapUtil::swap(&mX, &mY);

            ASSERTV(YY, X, YY == X);
            ASSERTV(XX, Y, XX == Y);
            ASSERT(oam.isTotalSame());

            if (veryVeryVerbose) { T_ P_(X) P(Y) }
        }

        // Verify no memory is allocated from the default allocator.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        if (verbose) cout <<
                   "\nFree 'swap' function with different allocators." << endl;
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1    = DATA[ti].d_line;
            const Uint64      OFFSET1  = DATA[ti].d_offset;
            const char *const MESSAGE1 = DATA[ti].d_message_p;
            const Loc         LOC1(OFFSET1);

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator     oa2("object2", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj XX(LOC1, MESSAGE1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P(XX) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2    = DATA[tj].d_line;
                const Uint64      OFFSET2  = DATA[tj].d_offset;
                const char *const MESSAGE2 = DATA[tj].d_message_p;
                const Loc         LOC2(OFFSET2);

                Obj       mX(XX, &oa);               const Obj& X = mX;
                Obj       mY(LOC2, MESSAGE2, &oa2);  const Obj& Y = mY;
                const Obj YY(Y, &scratch);

                if (veryVerbose) { T_ P_(LINE2) P_(X) P_(Y) P(YY) }

                // free function 'swap', different allocators
                {
                    swap(mX, mY);

                    ASSERTV(LINE1, LINE2, YY, X, YY == X);
                    ASSERTV(LINE1, LINE2, XX, Y, XX == Y);
                    ASSERTV(LINE1, LINE2, &oa  == X.allocator());
                    ASSERTV(LINE1, LINE2, &oa2 == Y.allocator());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (verbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_PASS(mA.swap(mB));
                ASSERT_FAIL(mA.swap(mZ));
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MOVE CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the new object has the original value.
        //   Verify that if the same allocator is used there have been no new
        //   allocations, and if a different allocator is used the source
        //   object has the original value.
        //
        // Concerns:
        //: 1 The move constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the original object
        //:   started with.
        //:
        //: 2 If an allocator is NOT supplied, the allocator of the new object
        //:   is the same as the original object, and no new allocations occur.
        //:
        //: 3 If an allocator is supplied that is the same as the original
        //:   object, then no new allocations occur.
        //:
        //: 4 If an allocator is supplied that is different from the original
        //:   object, then the original object's value remains unchanged.
        //:
        //: 5 Supplying a default-constructed allocator explicitly is the same
        //:   as supplying the default allocator.
        //:
        //: 6 Any memory allocation is from the object allocator.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The allocator used by the original object is unchanged.
        //:
        //:10 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..9)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that creates an object by
        //:     move-constructing from a newly created object with value V,
        //:     but invokes the move constructor differently in each
        //:     iteration: (a) using the standard single-argument move
        //:     constructor, (b) using the extended move constructor with a
        //:     default-constructed allocator argument (to use the default
        //:     allocator), (c) using the extended move constructor with the
        //:     same allocator as the moved-from object, and (d) using the
        //:     extended move constructor with a different allocator than the
        //:     moved-from object.
        //:
        //: 3 For each of these iterations (P-2.2):
        //:
        //:   1 Create four 'bslma::TestAllocator' objects, and install one as
        //:     the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Dynamically allocate another object 'F" using the 's1'
        //:     allocator having the same value V, using a distinct allocator
        //:     for the object's footprint.
        //:
        //:   3 Dynamically allocate an object 'X' using the appropriate move
        //:     constructor to move from 'F', passing as a second argument
        //:     (a) nothing, (b) 'allocator_type()', (c) '&s1', or (d)
        //:     'allocator_type(&s2)'.
        //:
        //:   4 Record the allocator expected to be used by the new object and
        //:     how much memory it used before the move constructor.
        //:
        //:   5 Verify that space for 2 objects is used in the footprint
        //:     allocator
        //:
        //:   6 Verify that the moved-to object has the expected value 'V' by
        //:     comparing to 'Z'.
        //:
        //:   7 If the allocators of 'F' and 'X' are different, verify that the
        //:     value of 'F' is still 'V', and that the amount of memory
        //:     used in the allocator for 'X' is the same as the amount of
        //:     that was used by 'F'.
        //:
        //:   8 If the allocators of 'F' and 'X' are the same, verify that no
        //:     extra memory was used by the move constructor.
        //:
        //:   9 Verify that no memory was used by the move constructor as
        //:     temporary memory, and no unused allocators have had any memory
        //:     used.
        //:
        //:  10 Delete both dynamically allocated objects and verify that all
        //:     temporary allocators have had all memory returned to them.
        //:
        //: 3 Test again, using the data of P-1, but this time just for the
        //:   supplied allocator configuration (P-2.2c), and create the object
        //:   as an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  Do
        //:   this by creating one object with one test allocator ('s1') and
        //:   then using the move constructor with a separate test allocator
        //:   that is injecting exceptions ('s2').
        //:   (C-10)
        //
        // Testing:
        //   Error(bslmf::MovableRef<Error> original)
        //   Error(bslmf::MovableRef<Error> original, ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "MOVE CONSTRUCTOR" << endl
                          << "================" << endl;

        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE    = DATA[ti].d_line;
                const char        MEM     = DATA[ti].d_mem;
                const Uint64      OFFSET  = DATA[ti].d_offset;
                const char *const MESSAGE = DATA[ti].d_message_p;
                const Loc         LOC(OFFSET);

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj Z (LOC, MESSAGE, &scratch);
                const Obj ZZ(LOC, MESSAGE, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator s1("supplied",  veryVeryVeryVerbose);
                    bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj *fromPtr = new (fa) Obj(LOC, MESSAGE, &s1);
                    Obj &mF      = *fromPtr; const Obj& F = mF;

                    bsls::Types::Int64 s1Alloc = s1.numBytesInUse();

                    Obj                  *objPtr = 0;
                    bsls::Types::Int64    objAlloc;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        // normal move constructor
                        objAllocatorPtr = &s1;
                        objAlloc = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF));
                      } break;
                      case 'b': {
                        // allocator move constructor, default allocator
                        objAllocatorPtr = &da;
                        objAlloc = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              0);
                      } break;
                      case 'c': {
                        // allocator move constructor, same allocator
                        objAllocatorPtr = &s1;
                        objAlloc = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              objAllocatorPtr);
                      } break;
                      case 'd': {
                        // allocator move constructor, different allocator
                        objAllocatorPtr = &s2;
                        objAlloc = objAllocatorPtr->numBytesInUse();
                        objPtr = new (fa) Obj(bslmf::MovableRefUtil::move(mF),
                                              objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, 2*sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(mF) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        ASSERTV(LINE, CONFIG, Obj(), *objPtr,
                                Obj() == *objPtr);
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG,  Z, X,  Z == X);

                    if (objAllocatorPtr != F.allocator()) {
                        // If the allocators are different, verify that the
                        // value of 'fX' has not changed.

                        ASSERTV(LINE, CONFIG, Z, F, Z == F);

                        // If memory was used, verify that the same amount was
                        // used by the moved-to object.

                        bsls::Types::Int64 moveBytesUsed =
                                   objAllocatorPtr->numBytesInUse() - objAlloc;
                        ASSERTV(LINE, CONFIG,
                                s1.numBytesInUse(), moveBytesUsed,
                                s1.numBytesInUse() == moveBytesUsed);
                    }
                    else {
                        // If the allocators are the same, verify that no new
                        // bytes were allocated by moving.

                        ASSERTV(LINE, CONFIG, s1Alloc, s1.numBytesInUse(),
                                s1Alloc == s1.numBytesInUse());
                    }

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG,
                            &oa == X.message().get_allocator());

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    ASSERTV(LINE, CONFIG, &oa, ALLOC_OF(X),
                            &oa == X.allocator());

                    ASSERTV(LINE, CONFIG, &scratch, ALLOC_OF(Z),
                            &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocators.
                    if (objAllocatorPtr != &da) {
                        ASSERTV(LINE, CONFIG, da.numBlocksTotal(),
                                0 == da.numBlocksTotal());
                    }

                    if (objAllocatorPtr != &s2) {
                        ASSERTV(LINE, CONFIG, s2.numBlocksTotal(),
                                0 == s2.numBlocksTotal());
                    }

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

                    // Reclaim dynamically allocated objects under test.

                    fa.deleteObject(fromPtr);
                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    ASSERTV(LINE, CONFIG, da.numBlocksInUse(),
                            0 == da.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, fa.numBlocksInUse(),
                            0 == fa.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, s1.numBlocksInUse(),
                            0 == s1.numBlocksInUse());
                    ASSERTV(LINE, CONFIG, s2.numBlocksInUse(),
                            0 == s2.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

            // Double check that some object memory was allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE    = DATA[ti].d_line;
                const char        MEM     = DATA[ti].d_mem;
                const Uint64      OFFSET  = DATA[ti].d_offset;
                const char *const MESSAGE = DATA[ti].d_message_p;
                const Loc         LOC(OFFSET);

                if (veryVerbose) { T_ P_(MEM) P_(LOC) P(MESSAGE) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator s1("supplied1", veryVeryVeryVerbose);
                bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                const Obj Z (LOC, MESSAGE, &scratch);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(s2) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj from(LOC, MESSAGE, &s1);

                    Obj obj(bslmf::MovableRefUtil::move(from), &s2);
                    ASSERTV(LINE, Z, obj, Z == obj);

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        ASSERTV(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, &scratch, ALLOC_OF(Z),
                        &scratch == Z.allocator());
                ASSERTV(LINE, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, s1.numBlocksInUse(),
                        0 == s1.numBlocksInUse());
                ASSERTV(LINE, s2.numBlocksInUse(),
                        0 == s2.numBlocksInUse());
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
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
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
        //:11 The allocator used by the original object is unchanged.
        //:
        //:12 QoI: Copying an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:13 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..12)
        //:
        //:   1 Use the value constructor and a "scratch" allocator to create
        //:     two 'const' 'Obj', 'Z' and 'ZZ', each having the value 'V'.
        //:
        //:   2 Execute an inner loop that creates an object by
        //:     copy-constructing from value 'Z' from P-2.1, but invokes the
        //:     copy constructor differently in each iteration: (a) without
        //:     passing an allocator, (b) passing a default-constructed
        //:     allocator explicitly, (c) passing the address of a test
        //:     allocator distinct from the default, and (d) passing in an
        //:     allocator constructed from the address of a test allocator
        //:     distinct from the default.
        //:
        //:   3 For each of these iterations (P-2.2):  (C-1..12)
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
        //:     4 Use the 'get_allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also use the 'allocator'
        //:       accessor of 'X' to verify that its object allocator is
        //:       properly installed, and use the 'allocator' accessor of 'Z'
        //:       to verify that the allocator that it uses is unchanged.
        //:       (C-6, 11)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-2..4,
        //:       7..8, 12)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-2, 4)
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
        //   Error(const Error& original, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;
        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE    = DATA[ti].d_line;
                const char        MEM     = DATA[ti].d_mem;
                const Uint64      OFFSET  = DATA[ti].d_offset;
                const char *const MESSAGE = DATA[ti].d_message_p;
                const Loc         LOC(OFFSET);

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj Z (LOC, MESSAGE, &scratch);
                const Obj ZZ(LOC, MESSAGE, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Z);
                      } break;
                      case 'b': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(Z, 0);
                      } break;
                      case 'c': {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(Z, objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG, sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        ASSERTV(LINE, CONFIG, Obj(), *objPtr,
                                Obj() == *objPtr);
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    ASSERTV(LINE, CONFIG,  Z, X,  Z == X);

                    // Verify that the value of 'Z' has not changed.

                    ASSERTV(LINE, CONFIG, ZZ, Z, ZZ == Z);

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG,
                            &oa == X.message().get_allocator());

                    // Also invoke the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    ASSERTV(LINE, CONFIG, &oa, ALLOC_OF(X),
                            &oa == X.allocator());

                    ASSERTV(LINE, CONFIG, &scratch, ALLOC_OF(Z),
                            &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG,
                            oa.numBlocksTotal(),
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
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE    = DATA[ti].d_line;
                const char        MEM     = DATA[ti].d_mem;
                const Uint64      OFFSET  = DATA[ti].d_offset;
                const char *const MESSAGE = DATA[ti].d_message_p;
                const Loc         LOC(OFFSET);

                if (veryVerbose) { T_ P_(MEM) P_(LOC) P(MESSAGE) }

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                const Obj Z (LOC, MESSAGE, &scratch);
                const Obj ZZ(LOC, MESSAGE, &scratch);

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

                ASSERTV(LINE, &scratch, ALLOC_OF(Z),
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
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 All salient attributes participate in the comparison.
        //:
        //: 3 No non-salient attributes (i.e., 'allocator') participate.
        //:
        //: 4 'true  == (X == X)'  (i.e., identity)
        //:
        //: 5 'false == (X != X)'  (i.e., identity)
        //:
        //: 6 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 7 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 8 'X != Y' if and only if '!(X == Y)'
        //:
        //: 9 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //:10 Non-modifiable objects can be compared (i.e., 'const' objects and
        //:   'const' references).
        //:
        //:11 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //:
        //:12 The equality operator's signature and return type are standard.
        //:
        //:13 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-9..10, 12..13)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their individual salient
        //:   attributes such that (a) for each salient attribute, there exists
        //:   a pair of rows that differ (slightly) in only the column
        //:   corresponding to that attribute, and (b) all attribute values
        //:   that can allocate memory on construction do so.
        //:
        //: 4 For each row 'R1' in the table of P-3:  (C-1..8)
        //:
        //:   1 Create a single object, using a "scratch" allocator, and
        //:     use it to verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-4..5)
        //:
        //:   2 For each row 'R2' in the table of P-3:  (C-1..3, 6..8)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects created
        //:       from 'R1' and 'R2', respectively, are expected to have the
        //:       same value.
        //:
        //:     2 For each of two configurations, 'a' and 'b':  (C-1..3, 6..8)
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
        //:         either operator.  (C-1..3, 6..8)
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-11)
        //
        // Testing:
        //   bool operator==(const Error& lhs, const Error& rhs);
        //   bool operator!=(const Error& lhs, const Error& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;
        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            using namespace bdljsn;
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

        if (verbose) cout <<
            "\nDefine appropriate individual attribute values, 'Ai' and 'Bi'."
                                                                       << endl;

        // Attribute Types

        typedef Loc         T1;                // 'location'
        typedef const char *T2;                // 'message'

        // Attribute 1 Values: 'utcOffsetInSeconds'

        const T1 A1 = Loc(0ULL);                 // baseline
        const T1 B1 = Loc(bsl::numeric_limits<Int64>::max());

        // Attribute 3 Values: 'description'

        const T2 A2 = LONG_STRING;             // baseline
        const T2 B2 = LONGER_STRING;

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int         d_line;           // source line number
            Loc         d_location;
            const char *d_message_p;
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute
        // value (Bi).

        //LINE  LOCATION  MSG
        //----  --------  ---

        { L_,         A1, A2 },          // baseline

        { L_,         B1, A2 },
        { L_,         A1, B2 }

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1 = DATA[ti].d_line;
            const Loc         LOC1  = DATA[ti].d_location;
            const char *const MSG1  = DATA[ti].d_message_p;

            if (veryVerbose) { T_ P_(LINE1) P_(LOC1) P(MSG1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj X(LOC1, MSG1, &scratch);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2 = DATA[tj].d_line;
                const Loc         LOC2  = DATA[tj].d_location;
                const char *const MSG2  = DATA[tj].d_message_p;

                if (veryVerbose) {
                               T_ T_ P_(LINE2) P_(LOC2) P(MSG2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    const Obj X(LOC1, MSG1, &xa);
                    const Obj Y(LOC2, MSG2, &ya);

                    if (veryVerbose) {
                        T_ T_ T_ P_(EXP) P_(CONFIG) P_(X) P(Y) }

                    // Verify value, commutativity, and no memory allocation.

                    bslma::TestAllocatorMonitor oaxm(&oax), oaym(&oay);

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, CONFIG, Y, X,  EXP == (Y == X));

                    ASSERTV(LINE1, LINE2, CONFIG, X, Y, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, CONFIG, Y, X, !EXP == (Y != X));

                    ASSERTV(LINE1, LINE2, CONFIG, oaxm.isTotalSame());
                    ASSERTV(LINE1, LINE2, CONFIG, oaym.isTotalSame());

                    // Double check that some object memory was allocated.

                    ASSERTV(LINE1, LINE2, CONFIG,
                            1 <= xa.numBlocksInUse());
                    ASSERTV(LINE1, LINE2, CONFIG,
                            1 <= ya.numBlocksInUse());

                    // Note that memory should be independently allocated for
                    // each attribute capable of allocating memory.
                }
            }

            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
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
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the
        //:     two formatting parameters, along with the expected output.
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
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
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
        //   ostream& print(ostream& stream, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream& stream, const Error& object);
        //   CONCERN: All accessor methods are declared 'const'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR" << endl
                          << "=========================" << endl;
        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            using namespace bdljsn;
            using bsl::ostream;

            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;

            Uint64      d_offset;
            const char *d_message_p;

            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"
#define SP " "

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  OFF  MSG   EXP
        //---- - ---  ---  ----  ---

        { L_,  0,  0,  89, "a",  "["                                         NL
                                 "location = ["                              NL
                                 "offset = 89"                               NL
                                 "]"                                         NL
                                 "message = \"a\""                           NL
                                 "]"                                         NL
                                                                             },

        { L_,  0,  1,  89, "a",  "["                                         NL
                                 " location = ["                             NL
                                 "  offset = 89"                             NL
                                 " ]"                                        NL
                                 " message = \"a\""                          NL
                                 "]"                                         NL
                                                                             },

        { L_,  0, -1,  89, "a",  "["                                         SP
                                 "location = ["                              SP
                                 "offset = 89"                               SP
                                  "]"                                        SP
                                 "message = \"a\""                           SP
                                 "]"
                                                                             },

        { L_,  0, -8,  89, "a",  "["                                         NL
                                 "    location = ["                          NL
                                 "        offset = 89"                       NL
                                 "    ]"                                     NL
                                 "    message = \"a\""                       NL
                                 "]"                                         NL
                                                                             },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  OFF  MSG   EXP
        //---- - ---  ---  ----  ---

        { L_,  3,  0,  89, "a",  "["                                         NL
                                 "location = ["                              NL
                                 "offset = 89"                               NL
                                 "]"                                         NL
                                 "message = \"a\""                           NL
                                 "]"                                         NL
                                                                             },

        { L_,  3,  2,  89, "a",  "      ["                                   NL
                                 "        location = ["                      NL
                                 "          offset = 89"                     NL
                                 "        ]"                                 NL
                                 "        message = \"a\""                   NL
                                 "      ]"                                   NL
                                                                             },

        { L_,  3, -2,  89, "a",  "      ["                                   SP
                                 "location = ["                              SP
                                 "offset = 89"                               SP
                                 "]"                                         SP
                                 "message = \"a\""                           SP
                                 "]"
                                                                             },

        { L_,  3, -8,  89, "a",  "            ["                             NL
                                 "                location = ["              NL
                                 "                    offset = 89"           NL
                                 "                ]"                         NL
                                 "                message = \"a\""           NL
                                 "            ]"                             NL
                                                                             },

        { L_, -3,  0,  89, "a",  "["                                         NL
                                 "location = ["                              NL
                                 "offset = 89"                               NL
                                 "]"                                         NL
                                 "message = \"a\""                           NL
                                 "]"                                         NL
                                                                             },

        { L_, -3,  2,  89, "a",  "["                                         NL
                                 "        location = ["                      NL
                                 "          offset = 89"                     NL
                                 "        ]"                                 NL
                                 "        message = \"a\""                   NL
                                 "      ]"                                   NL
                                                                             },

        { L_, -3, -2,  89, "a",  "["                                         SP
                                 "location = ["                              SP
                                 "offset = 89"                               SP
                                 "]"                                         SP
                                 "message = \"a\""                           SP
                                 "]"
                                                                             },

        { L_, -3, -8,  89, "a",  "["                                         NL
                                 "                location = ["              NL
                                 "                    offset = 89"           NL
                                 "                ]"                         NL
                                 "                message = \"a\""           NL
                                 "            ]"                             NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL  OFF  MSG   EXP
        //---- - ---  ---  ----  ---

        { L_,  2,  3,   7, "bc", "      ["                                   NL
                                        "         location = ["              NL
                                        "            offset = 7"             NL
                                        "         ]"                         NL
                                        "         message = \"bc\""          NL
                                        "      ]"                            NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  OFF  MSG   EXP
        //---- - ---  ---  ----  ---

        { L_, -8, -8,  89,  "a", "["                                         NL
                                 "    location = ["                          NL
                                 "        offset = 89"                       NL
                                 "    ]"                                     NL
                                 "    message = \"a\""                       NL
                                 "]"                                         NL
                                                                             },

        { L_, -8, -8,   7, "bc", "["                                         NL
                                 "    location = ["                          NL
                                 "        offset = 7"                        NL
                                 "    ]"                                     NL
                                 "    message = \"bc\""                      NL
                                 "]"                                         NL
                                                                             },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  OFF  MSG   EXP
        //---- - ---  ---  ----  ---

        { L_, -9, -9,  89, "a",  "[ [ offset = 89 ] \"a\" ]"                 },

        { L_, -9, -9,   7, "bc", "[ [ offset = 7 ] \"bc\" ]"                 }

#undef NL
#undef SP

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_line;
                const int         L    = DATA[ti].d_level;
                const int         SPL  = DATA[ti].d_spacesPerLevel;
                const Uint64      OFF  = DATA[ti].d_offset;
                const char *const MSG  = DATA[ti].d_message_p;
                const char *const EXP  = DATA[ti].d_expected_p;
                const Loc         LOC(OFF);

                if (veryVerbose) { T_ P_(L) P_(SPL) P_(OFF) P(MSG) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X(LOC, MSG);

                bsl::ostringstream os;

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

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                ASSERTV(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        //   In case 3 we demonstrated that all basic accessors work properly
        //   with respect to attributes initialized by the value constructor.
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //:
        //: 3 No accessor allocates any memory.
        //:
        //: 4 Accessors for attributes that can allocate memory (i.e., those
        //:   that take an allocator in their constructor) return a 'const'
        //:   reference.
        //
        // Plan:
        //: 1 Create two 'bslma::TestAllocator' objects, and install one as
        //:   the current default allocator (note that a ubiquitous test
        //:   allocator is already installed as the global allocator).
        //:
        //: 2 Use the default constructor, using the other test allocator
        //:   from P-1, to create an object (having default attribute values).
        //:
        //: 3 Verify that each basic accessor, invoked on a 'const' reference
        //:   to the object created in P-2, returns the expected value.  (C-2)
        //:
        //: 4 For each salient attribute (contributing to value):  (C-1, 3..4)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value, making sure to allocate memory if possible.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //:
        //:   3 Monitor the memory allocated from both the default and object
        //:     allocators before and after calling the accessor; verify that
        //:     there is no change in total memory allocation.  (C-3..4)
        //
        // Testing:
        //   bslma::Allocator *allocator() const;
        //   const bsl::string& message() const;
        //   const Location& location() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;
        // Attribute Types

        typedef Loc         T1;     // 'location'
        typedef bsl::string T2;     // 'message'

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const Loc              D1;  // 'location'
        const bsl::string_view D2;  // 'message'

        // -------------------------------------------------------
        // 'A' values: Should cause memory allocation if possible.
        // -------------------------------------------------------

        const Loc  A1   = Loc(bsl::numeric_limits<Uint64>::max());
        const char A2[] = "a_" SUFFICIENTLY_LONG_STRING;

        if (verbose) cout <<
           "\nCreate two test allocators; install one as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
                 "\nCreate an object, passing in the other allocator." << endl;

        Obj mX(&oa);  const Obj& X = mX;

        if (verbose) cout <<
                "\nVerify all basic accessors report expected values." << endl;
        {
            const T1& location = X.location();
            ASSERTV(D1, location, D1 == location);

            const T2& message = X.message();
            ASSERTV(D2, message, D2 == message);

            ASSERTV(X.allocator(), &oa == X.allocator());
        }

        if (verbose) cout <<
            "\nApply primary manipulators and verify expected values." << endl;

        if (verbose) { T_ Q(location) }
        {
            mX.setLocation(A1);

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            const T1& location = X.location();
            ASSERTV(A1, location, A1 == location);

            ASSERT(oam.isTotalSame());  ASSERT(dam.isTotalSame());
        }

        if (verbose) { T_ Q(message) }
        {
            mX.setMessage(A2);

            bslma::TestAllocatorMonitor oam(&oa), dam(&da);

            const T2& message = X.message();
            ASSERTV(A2, message, A2 == message);

            ASSERT(oam.isTotalSame());  ASSERT(dam.isTotalSame());
        }

        // Double check that some object memory was allocated.

        ASSERT(1 <= oa.numBlocksTotal());

        // Note that memory should be independently allocated for each
        // attribute capable of allocating memory.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // VALUE CTOR
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value that does not violate the
        //:   constructor's documented preconditions.
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
        //: 6 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
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
        //:11 QoI: Creating an object having the default-constructed value
        //:   allocates no memory.
        //:
        //:12 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //:   2 Additionally, provide a (tri-valued) column, 'MEM', indicating
        //:     the expectation of memory allocation for all typical
        //:     implementations of individual attribute types: ('Y') "Yes",
        //:     ('N') "No", or ('?') "implementation-dependent".
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1, 3..11)
        //:
        //:   1 Execute an inner loop that creates an object having value 'V',
        //:     but invokes the constructor differently on each iteration: (a)
        //:     without passing an allocator, (b) passing a
        //:     default-constructed allocator explicitly (c) passing the
        //:     address of a test allocator distinct from the default
        //:     allocator, and (d) passing in an allocator constructed from
        //:     the address of a test allocator distinct from the default.
        //:
        //:   2 For each of the iterations in P-2.1:  (C-1, 4..11)
        //:
        //:     1 Create three 'bslma::TestAllocator' objects, and install one
        //:       as the current default allocator (note that a ubiquitous test
        //:       allocator is already installed as the global allocator).
        //:
        //:     2 Use the value constructor to dynamically create an object
        //:       having the value 'V', with its object allocator configured
        //:       appropriately (see P-2.1), supplying all the arguments as
        //:       'const' and representing any string arguments as 'char *';
        //:       use a distinct test allocator for the object's footprint.
        //:
        //:     3 Use the (as yet unproven) salient attribute accessors to
        //:       verify that all of the attributes of each object have their
        //:       expected values.  (C-1, 7)
        //:
        //:     4 Use the 'get_allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also invoke the (as yet
        //:       unproven) 'allocator' accessor of the object under test.
        //:       (C-8)
        //:
        //:     5 Use the appropriate test allocators to verify that:  (C-4..6,
        //:       9..11)
        //:
        //:       1 An object that IS expected to allocate memory does so
        //:         from the object allocator only (irrespective of the
        //:         specific number of allocations or the total amount of
        //:         memory allocated).  (C-4, 6)
        //:
        //:       2 An object that is expected NOT to allocate memory doesn't.
        //:         (C-11)
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
        //:   (P-2.1c) on the data of P-1, but this time create the object as
        //:   an automatic variable in the presence of injected exceptions
        //:   (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros);
        //:   represent any string arguments in terms of 'string' using a
        //:   "scratch" allocator.  (C-2, 12)
        //
        // Testing:
        //   Error(const Location& l, const bsl::string_view& m, bA = 0);
        //   CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
        //   CONCERN: String arguments can be either 'char *' or 'string'.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "VALUE CTOR" << endl
                          << "==========" << endl;
        if (verbose) cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) cout <<
             "\nCreate objects with various allocator configurations." << endl;
        {
            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE    = DATA[ti].d_line;
                const char        MEM     = DATA[ti].d_mem;
                const Uint64      OFFSET  = DATA[ti].d_offset;
                const char *const MESSAGE = DATA[ti].d_message_p;
                const Loc         LOC(OFFSET);

                if (veryVerbose) { T_ P_(MEM) P_(OFFSET) P(MESSAGE) }

                ASSERTV(LINE, MEM, MEM && strchr("YN?", MEM));

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr = 0;
                    bslma::TestAllocator *objAllocatorPtr = 0;

                    switch (CONFIG) {
                      case 'a': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(LOC, MESSAGE);
                      } break;
                      case 'b': {
                        objAllocatorPtr = &da;
                        objPtr = new (fa) Obj(LOC, MESSAGE, 0);
                      } break;
                      case 'c': {
                        objAllocatorPtr = &sa;
                        objPtr = new (fa) Obj(LOC, MESSAGE, objAllocatorPtr);
                      } break;
                      default: {
                        BSLS_ASSERT_OPT(!"Bad allocator config.");
                      } break;
                    }
                    ASSERTV(LINE, CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                    // Use untested functionality to help ensure the first row
                    // of the table contains the default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        ASSERTV(LINE, CONFIG, Obj(), *objPtr,
                                     Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // -------------------------------------
                    // Verify the object's attribute values.
                    // -------------------------------------

                    ASSERTV(LINE, CONFIG, OFFSET, X.location().offset(),
                            OFFSET == X.location().offset());

                    ASSERTV(LINE, CONFIG, MESSAGE, X.message(),
                            MESSAGE == X.message());

                    // -------------------------------------------------------
                    // Verify any attribute allocators are installed properly.
                    // -------------------------------------------------------

                    ASSERTV(LINE, CONFIG,
                            &oa == X.message().get_allocator());

                    // Also invoke the object's 'allocator' accessor.

                    ASSERTV(LINE, CONFIG, &oa, ALLOC_OF(X),
                            &oa == X.allocator());

                    // Verify no allocation from the non-object allocator.

                    ASSERTV(LINE, CONFIG, noa.numBlocksTotal(),
                            0 == noa.numBlocksTotal());

                    // Verify no temporary memory is allocated from the object
                    // allocator.

                    ASSERTV(LINE, CONFIG,
                            oa.numBlocksTotal(),
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
            // attribute capable of allocating memory.
        }

        if (verbose) cout << "\nTesting with injected exceptions." << endl;
        {
            // Note that any string arguments are now of type 'string', which
            // require their own "scratch" allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE    = DATA[ti].d_line;
                const char        MEM     = DATA[ti].d_mem;
                const Uint64      OFFSET  = DATA[ti].d_offset;
                const char *const MESSAGE = DATA[ti].d_message_p;
                const Loc         LOC(OFFSET);

                if (veryVerbose) { T_ P_(MEM) P_(LOC) P(MESSAGE) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(LOC, MESSAGE, &sa);
                    ASSERTV(LINE, OFFSET, obj.location().offset(),
                            OFFSET == obj.location().offset());
                    ASSERTV(LINE, MESSAGE, obj.message(),
                            MESSAGE == obj.message());

#ifdef BDE_BUILD_TARGET_EXC
                    if ('Y' == MEM) {
                        ASSERTV(LINE, 0 < EXCEPTION_COUNT);
                    }
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(LINE, da.numBlocksInUse(),
                        0 == da.numBlocksInUse());
                ASSERTV(LINE, sa.numBlocksInUse(),
                        0 == sa.numBlocksInUse());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor (with or without
        //:   a supplied allocator) has the contractually specified default
        //:   value.
        //:
        //: 2 If an allocator is NOT supplied to the default constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object.
        //:
        //: 3 If an allocator IS supplied to the default constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a default-constructed allocator has the same effect as
        //:   not supplying an allocator.
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
        //:10 Each attribute is modifiable independently.
        //:
        //:   1 Each primary manipulator returns a non-'const' reference to the
        //:     target object (i.e., '*this').
        //:
        //:11 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //:
        //:12 Any string arguments can be of type 'char *' or 'string'.
        //:
        //:13 Any argument can be 'const'.
        //:
        //:14 Any memory allocation is exception neutral.
        //:
        //:15 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: ('D')
        //:   values corresponding to the default-constructed object, ('A')
        //:   values that allocate memory if possible, and ('B') other values
        //:   that do not cause additional memory allocation beyond that which
        //:   may be incurred by 'A'.  Both the 'A' and 'B' attribute values
        //:   should be chosen to be boundary values where possible.  If an
        //:   attribute can be supplied via alternate C++ types (e.g., 'string'
        //:   instead of 'char *'), use the alternate type for 'B'.
        //:
        //: 2 Execute an inner loop that creates an object by
        //:   default-construction, but invokes the default constructor
        //:   differently in each iteration: (a) without passing an allocator,
        //:   (b) passing a default-constructed allocator explicitly (c)
        //:   passing the address of a test allocator distinct from the
        //:   default, and (d) passing in an allocator constructed from the
        //:   address of a test allocator distinct from the default.  For each
        //:   of these iterations: (C-1..14)
        //:
        //:   1 Create three 'bslma::TestAllocator' objects, and install one as
        //:     as the current default allocator (note that a ubiquitous test
        //:     allocator is already installed as the global allocator).
        //:
        //:   2 Use the default constructor to dynamically create an object
        //:     'X', with its object allocator configured appropriately (see
        //:     P-2); use a distinct test allocator for the object's footprint.
        //:
        //:   3 Use the 'get_allocator' accessor of each underlying attribute
        //:     capable of allocating memory to ensure that its object
        //:     allocator is properly installed; also invoke the (as yet
        //:     unproven) 'allocator' accessor of the object under test.
        //:     (C-2..4)
        //:
        //:   4 Use the appropriate test allocators to verify that no memory
        //:     is allocated by the default constructor.  (C-9)
        //:
        //:   5 Use the individual (as yet unproven) salient attribute
        //:     accessors to verify the default-constructed value.  (C-1)
        //:
        //:   6 For each attribute 'i', in turn, create a local block.  Then
        //:     inside the block, using brute force, set that attribute's
        //:     value, passing a 'const' argument representing each of the
        //:     three test values, in turn (see P-1), first to 'Ai', then to
        //:     'Bi', and finally back to 'Di'.  If attribute 'i' can allocate
        //:     memory, verify that it does so on the first value transition
        //:     ('Di' -> 'Ai'), and that the corresponding primary manipulator
        //:     is exception neutral (using the
        //:     'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).  In all other
        //:     cases, verify that no memory allocation occurs.  After each
        //:     transition, use the (as yet unproven) basic accessors to verify
        //:     that only the intended attribute value changed.  (C-5..6,
        //:     11..14)
        //:
        //:   7 Corroborate that attributes are modifiable independently by
        //:     first setting all of the attributes to their 'A' values.  Then
        //:     incrementally set each attribute to it's corresponding  'B'
        //:     value and verify after each manipulation that only that
        //:     attribute's value changed.  (C-10)
        //:
        //:     1 Confirm that the address of the returned object (reference)
        //:       can be stored in an non-'const' pointer and is the same as
        //:       the address of the target object.  (C-10.1)
        //:
        //:   8 Verify that no temporary memory is allocated from the object
        //:     allocator.  (C-7)
        //:
        //:   9 Verify that all object memory is released when the object is
        //:     destroyed.  (C-8)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-15)
        //
        // Testing:
        //   Error& setMessage(const bsl::string_view& value);
        //   Error& setLocation(const Location& value);
        //   ~Error();
        //   explicit Error(bslma::Allocator *basicAllocator);
        //   Error();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                       << "DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR" << endl
                       << "==========================================" << endl;

        if (verbose) cout << "\nEstablish suitable attribute values." << endl;

        // 'D' values: These are the default-constructed values.

        const Loc              D1   = Loc(0ULL);     // 'location'
        const bsl::string_view D2;              // 'message''

        // 'A' values: Should cause memory allocation if possible.

        const Loc    A1   = Loc(0ULL);
        const char   A2[] = "a_" SUFFICIENTLY_LONG_STRING;

        // 'B' values: Should NOT cause allocation (use alternate string type).

        const Loc              B1 = Loc(bsl::numeric_limits<Uint64>::max());
        const bsl::string_view B2 = "shortMessage";

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;

        for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

            const char CONFIG = cfg;  // how we specify the allocator

            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj                  *objPtr = 0;
            bslma::TestAllocator *objAllocatorPtr = 0;

            switch (CONFIG) {
              case 'a': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj();
              } break;
              case 'b': {
                objAllocatorPtr = &da;
                objPtr = new (fa) Obj(0);
              } break;
              case 'c': {
                objAllocatorPtr = &sa;
                objPtr = new (fa) Obj(objAllocatorPtr);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"Bad allocator config.");
              } break;
            }

            Obj&                   mX = *objPtr;  const Obj& X = mX;
            bslma::TestAllocator&  oa = *objAllocatorPtr;
            bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

            // -------------------------------------------------------
            // Verify any attribute allocators are installed properly.
            // -------------------------------------------------------

            ASSERTV(CONFIG, &oa == X.message().get_allocator());

            // Also invoke the object's 'allocator' accessor.

            ASSERTV(CONFIG, &oa, ALLOC_OF(X), &oa == X.allocator());

            // Verify no allocation from the object/non-object allocators.

            ASSERTV(CONFIG,  oa.numBlocksTotal(),
                    0 ==  oa.numBlocksTotal());
            ASSERTV(CONFIG, noa.numBlocksTotal(),
                    0 == noa.numBlocksTotal());

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            ASSERTV(CONFIG, D1, X.location(),
                    D1 == X.location());
            ASSERTV(CONFIG, D2, X.message(),
                    D2 == X.message());

            // -----------------------------------------------------
            // Verify that each attribute is independently settable.
            // -----------------------------------------------------

            // 'location'
            {
                bslma::TestAllocatorMonitor tam(&oa);

                Obj *mRA1 = &(mX.setLocation(A1));
                ASSERTV(CONFIG, mRA1, &mX, mRA1 == &mX);

                ASSERTV(CONFIG, A1 == X.location());
                ASSERTV(CONFIG, D2 == X.message());

                Obj *mRB1 = &(mX.setLocation(B1));
                ASSERTV(CONFIG, mRB1, &mX, mRB1 == &mX);

                ASSERTV(CONFIG, B1 == X.location());
                ASSERTV(CONFIG, D2 == X.message());

                Obj *mRD1 = &(mX.setLocation(D1));
                ASSERTV(CONFIG, mRD1, &mX, mRD1 == &mX);

                ASSERTV(CONFIG, D1 == X.location());
                ASSERTV(CONFIG, D2 == X.message());

                ASSERTV(CONFIG, tam.isTotalSame());
            }

            // 'message'
            {
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    bslma::TestAllocatorMonitor tam(&oa);

                    Obj *mRA2 = &(mX.setMessage(A2));
                    ASSERTV(CONFIG, mRA2, &mX, mRA2 == &mX);

                    ASSERTV(CONFIG, tam.isInUseUp());

#ifdef BDE_BUILD_TARGET_EXC
                    ASSERT(0 < EXCEPTION_COUNT);
#endif
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                ASSERTV(CONFIG, D1 == X.location());
                ASSERTV(CONFIG, A2 == X.message());

                bslma::TestAllocatorMonitor tam(&oa);

                Obj *mRB2 = &(mX.setMessage(B2));
                ASSERTV(CONFIG, mRB2, &mX, mRB2 == &mX);

                ASSERTV(CONFIG, D1 == X.location());
                ASSERTV(CONFIG, B2 == X.message());

                Obj *mRD2 = &(mX.setMessage(D2));
                ASSERTV(CONFIG, mRD2, &mX, mRD2 == &mX);

                ASSERTV(CONFIG, D1 == X.location());
                ASSERTV(CONFIG, D2 == X.message());

                ASSERTV(CONFIG, tam.isTotalSame());
            }

            // Corroborate attribute independence.
            {
                // Set all attributes to their 'A' values.

                Obj *mRA1 = &(mX.setLocation(A1));
                Obj *mRA2 = &(mX.setMessage(A2));

                ASSERTV(CONFIG, mRA1, &mX, mRA1 == &mX);
                ASSERTV(CONFIG, mRA2, &mX, mRA2 == &mX);

                ASSERTV(CONFIG, A1 == X.location());
                ASSERTV(CONFIG, A2 == X.message());

                // Set all attributes to their 'B' values.

                Obj *mRB1 = &(mX.setLocation(B1));
                ASSERTV(CONFIG, mRB1, &mX, mRB1 == &mX);

                ASSERTV(CONFIG, B1 == X.location());
                ASSERTV(CONFIG, A2 == X.message());

                Obj *mRB2 = &(mX.setMessage(B2));
                ASSERTV(CONFIG, mRB2, &mX, mRB2 == &mX);

                ASSERTV(CONFIG, B1 == X.location());
                ASSERTV(CONFIG, B2 == X.message());
            }

            // Verify no temporary memory is allocated from the object
            // allocator.

            ASSERTV(CONFIG, oa.numBlocksMax(), 1 == oa.numBlocksMax());

            // Reclaim dynamically allocated object under test.

            fa.deleteObject(objPtr);

            // Verify all memory is released on object destruction.

            ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
            ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
            ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

            // Double check that some object memory was allocated.

            ASSERTV(CONFIG, 1 <= oa.numBlocksTotal());

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 Create an object 'w' (default ctor).       { w:D             }
        //: 2 Create an object 'x' (copy from 'w').      { w:D x:D         }
        //: 3 Set 'x' to 'A' (value distinct from 'D').  { w:D x:A         }
        //: 4 Create an object 'y' (init. to 'A').       { w:D x:A y:A     }
        //: 5 Create an object 'z' (copy from 'y').      { w:D x:A y:A z:A }
        //: 6 Set 'z' to 'D' (the default value).        { w:D x:A y:A z:D }
        //: 7 Assign 'w' from 'x'.                       { w:A x:A y:A z:D }
        //: 8 Assign 'w' from 'z'.                       { w:D x:A y:A z:D }
        //: 9 Assign 'x' from 'x' (aliasing).            { w:D x:A y:A z:D }
        //
        // Plan:
        //: 1 An sequence of ad-hoc tests of the object' methods.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "BREATHING TEST" << bsl::endl
                               << "==============" << bsl::endl;

        // Attribute Types

        typedef bdljsn::Location T1;       // 'location'
        typedef bsl::string      T2;       // 'message'

        // Attribute 1 Values: 'location'

        const T1 D1 = bdljsn::Location(0); // default value
        const T1 A1 = bdljsn::Location(1);

        // Attribute 2 Values: 'message'

        const T2 D2 = "";                  // default value
        const T2 A2 = "some error";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 1. Create an object 'w' (default ctor)."
                             "\t\t{ w:D             }" << endl;

        Obj mW;  const Obj& W = mW;

        if (verbose) cout << "\ta. Check initial value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.location());
        ASSERT(D2 == W.message());

        if (verbose) cout <<
                  "\tb. Try equality operators: 'w' <op> 'w'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 2. Create an object 'x' (copy from 'w')."
                             "\t\t{ w:D x:D         }" << endl;

        Obj mX(W);  const Obj& X = mX;

        if (verbose) cout << "\ta. Check initial value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.location());
        ASSERT(D2 == X.message());

        if (verbose) cout <<
                   "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 3. Set 'x' to 'A' (value distinct from 'D')."
                             "\t\t{ w:D x:A         }" << endl;

        mX.setLocation(A1);
        mX.setMessage(A2);

        if (verbose) cout << "\ta. Check new value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.location());
        ASSERT(A2 == X.message());

        if (verbose) cout <<
             "\tb. Try equality operators: 'x' <op> 'w', 'x'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 4. Create an object 'y' (init. to 'A')."
                             "\t\t{ w:D x:A y:A     }" << endl;

        Obj mY(A1, A2);  const Obj& Y = mY;

        if (verbose) cout << "\ta. Check initial value of 'y'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Y) }

        ASSERT(A1 == Y.location());
        ASSERT(A2 == Y.message());

        if (verbose) cout <<
             "\tb. Try equality operators: 'y' <op> 'w', 'x', 'y'" << endl;

        ASSERT(0 == (Y == W));        ASSERT(1 == (Y != W));
        ASSERT(1 == (Y == X));        ASSERT(0 == (Y != X));
        ASSERT(1 == (Y == Y));        ASSERT(0 == (Y != Y));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 5. Create an object 'z' (copy from 'y')."
                             "\t\t{ w:D x:A y:A z:A }" << endl;

        Obj mZ(Y);  const Obj& Z = mZ;

        if (verbose) cout << "\ta. Check initial value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(A1 == Z.location());
        ASSERT(A2 == Z.message());

        if (verbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (Z == W));        ASSERT(1 == (Z != W));
        ASSERT(1 == (Z == X));        ASSERT(0 == (Z != X));
        ASSERT(1 == (Z == Y));        ASSERT(0 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 6. Set 'z' to 'D' (the default value)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;

        mZ.setLocation(D1);
        mZ.setMessage(D2);

        if (verbose) cout << "\ta. Check new value of 'z'." << endl;
        if (veryVeryVerbose) { T_ T_ P(Z) }

        ASSERT(D1 == Z.location());
        ASSERT(D2 == Z.message());

        if (verbose) cout <<
           "\tb. Try equality operators: 'z' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (Z == W));        ASSERT(0 == (Z != W));
        ASSERT(0 == (Z == X));        ASSERT(1 == (Z != X));
        ASSERT(0 == (Z == Y));        ASSERT(1 == (Z != Y));
        ASSERT(1 == (Z == Z));        ASSERT(0 == (Z != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 7. Assign 'w' from 'x'."
                             "\t\t\t\t{ w:A x:A y:A z:D }" << endl;
        mW = X;

        if (verbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(A1 == W.location());
        ASSERT(A2 == W.message());

        if (verbose) cout <<
           "\tb. Try equality operators: 'w' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(1 == (W == X));        ASSERT(0 == (W != X));
        ASSERT(1 == (W == Y));        ASSERT(0 == (W != Y));
        ASSERT(0 == (W == Z));        ASSERT(1 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 8. Assign 'w' from 'z'."
                             "\t\t\t\t{ w:D x:A y:A z:D }" << endl;
        mW = Z;

        if (verbose) cout << "\ta. Check new value of 'w'." << endl;
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.location());
        ASSERT(D2 == W.message());

        if (verbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));
        ASSERT(0 == (W == X));        ASSERT(1 == (W != X));
        ASSERT(0 == (W == Y));        ASSERT(1 == (W != Y));
        ASSERT(1 == (W == Z));        ASSERT(0 == (W != Z));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\n 9. Assign 'x' from 'x' (aliasing)."
                             "\t\t\t{ w:D x:A y:A z:D }" << endl;
        mX = X;

        if (verbose) cout << "\ta. Check (same) value of 'x'." << endl;
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.location());
        ASSERT(A2 == X.message());

        if (verbose) cout <<
           "\tb. Try equality operators: 'x' <op> 'w', 'x', 'y', 'z'." << endl;

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));
        ASSERT(0 == (X == Z));        ASSERT(1 == (X != Z));

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      } break;
    }

    // CONCERN: In no case does memory come from the global allocator.

    ASSERTV(globalAllocator.numBlocksTotal(),
            0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = "
                  << testStatus
                  << "."
                  << bsl::endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
