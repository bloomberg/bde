// bdljsn_jsonnumber.t.cpp                                            -*-C++-*-
#include <bdljsn_jsonnumber.h>

#include <bdlb_stringviewutil.h>

#include <bdldfp_decimal.h>
#include <bdldfp_decimalutil.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_stdallocator.h>  // 'operator!='
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_compilerfeatures.h>
#include <bsls_log.h>
#include <bsls_nameof.h>
#include <bsls_review.h>

#include <bsl_cassert.h>
#include <bsl_cstddef.h>     // 'bsl::size_t'
#include <bsl_cstdlib.h>     // 'bsl::atoi'
#include <bsl_cstring.h>     // 'bsl::memcpy', 'bsl:strstr'
#include <bsl_iostream.h>
#include <bsl_limits.h>      // 'bsl::numeric_limits'
#include <bsl_ostream.h>      // 'operator<<'
#include <bsl_sstream.h>     // 'bsl::ostringstream'
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class, having a single attribute, a 'bsl::string'
// value, that is constrained by a complicated grammar that is validated by the
// class 'static' method, 'bdljsn::JsonNumber::isValidNumber'.  Atypically for
// a value-semantic class, there is no "setter" method.  A value is acquired on
// construction and can only be changed by assignment or swap.  The value
// constructor is used in role of Primary Manipulator for these tests.
//
// Primary Manipulators:
//: o 'JsonNumber' (the value CTOR)
//
// Basic Accessors:
//: o 'allocator' (orthogonal to value)
//: o 'value'
//
// Much of the functionality of this class is achieved by forwarding to
// related, already tested, functions in 'bdljsn::NumberUtil'.  In these cases
// our concern is that this classes correctly delegates to the other component.
// Duplication of comprehensive tests would be superfluous.
//
// Similarly, the sole attribute of this class is implemented by 'bsl::string'.
// Once we have established (see TC 2) that all allocation of this class occurs
// from that data member, there is no need to reduplicate tests of exception
// safety, etc.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Any allocated memory is always from the object allocator.
//: o An object's value is independent of the allocator used to supply memory.
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
// ----------------------------------------------------------------------------
// CLASS METHODS
// [12] static bool isValidNumber(const bsl::string_view& text);
//
// CREATORS
// [ 2] JsonNumber();
// [ 2] JsonNumber(bslma::Allocator *basicAllocator);
// [ 2] JsonNumber(const char *text, *bA = 0);
// [ 2] JsonNumber(const bsl::string_view& text, *bA= 0);
// [ 7] JsonNumber(MovableRef<bsl::string> text);
// [ 7] JsonNumber(MovableRef<bsl::string> text, *bA = 0);
// [11] JsonNumber(int                 value, *bA =0);
// [11] JsonNumber(unsigned int        value, *bA = 0);
// [11] JsonNumber(bsls::Types::Int64  value, *bA = 0);
// [11] JsonNumber(bsls::Types::Uint64 value, *bA = 0);
// [11] JsonNumber(float               value, *bA = 0);
// [11] JsonNumber(double              value, *bA = 0);
// [11] JsonNumber(bdldfp::Decimal64   value, *bA = 0);
// [ 5] JsonNumber(const JsonNumber&      original, *bA = 0);
// [ 6] JsonNumber(MovableRef<JsonNumber> original);
// [ 6] JsonNumber(MovableRef<JsonNumber> original, *bA);
// [ 2] ~JsonNumber();
//
// MANIPULATORS
// [ 9] JsonNumber& operator=(const JsonNumber& rhs);
// [10] JsonNumber& operator=(MovableRef<JsonNumber> rhs);
// [11] JsonNumber& operator=(int                 rhs);
// [11] JsonNumber& operator=(unsigned int        rhs);
// [11] JsonNumber& operator=(bsls::Types::Int64  rhs);
// [11] JsonNumber& operator=(bsls::Types::Uint64 rhs);
// [11] JsonNumber& operator=(float             rhs);
// [11] JsonNumber& operator=(double            rhs);
// [11] JsonNumber& operator=(bdldfp::Decimal64 rhs);
//
// [ 8] void swap(JsonNumber& other);
//
// ACCESSORS
// [14] bool isEqual(const JsonNumber& other) const;
// [13] bool isIntegral() const;
// [ 2] const bsl::string& value() const;
//
// [11] int asInt   (int                 *result) const;
// [11] int asInt64 (bsls::Types::Int64  *result) const;
// [11] int asUint  (unsigned int        *result) const;
// [11] int asUint64(bsls::Types::Uint64 *result) const;
// [11] float              asFloat()     const;
// [11] double             asDouble()    const;
// [11] bdldfp::Decimal64  asDecimal64() const;
//
// [15] int asDecimal64Exact(bdldfp::Decimal64 *result) const;
//
// [11] explicit operator float()             const;
// [11] explicit operator double()            const;
// [11] explicit operator bdldfp::Decimal64() const;
//
// [ 2] bslma::Allocator *allocator() const;
// [ 3] ostream& print(ostream& stream, int level = 0, int sPL = 4) const;
//
// FREE OPERATORS
// [ 3] ostream& operator<<(ostream& stream, const JsonNumber& object);
// [ 4] bool operator==(const JsonNumber& lhs, const JsonNumber& rhs);
// [ 4] bool operator!=(const JsonNumber& lhs, const JsonNumber& rhs);
//
// FREE FUNCTIONS
// [16] void hashAppend(HASHALG& hashAlgorithm, const JsonNumber& object);
// [ 8] void swap(JsonNumber& a, JsonNumber& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [17] USAGE EXAMPLE
// [  ] CONCERN: Unexpected 'BSLS_REVIEW' failures should lead to test failures
// [  ] CONCERN: Only expected bsls log messages occur.

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

// For use in 'ASSERTV' macro invocations to print allocator.
#define ALLOC_OF(EXPR) (EXPR).allocator()

// ============================================================================
//                    EXCEPTION TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define EXCEPTION_COUNT bslmaExceptionCounter

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdljsn::JsonNumber Obj;
typedef bdljsn::NumberUtil NU;
typedef bdldfp::Decimal64  Deci64;

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

typedef bsl::string_view    SV;

// ============================================================================
//                                TYPE TRAITS
// ----------------------------------------------------------------------------

BSLMF_ASSERT(bslmf::IsBitwiseMoveable<Obj>::value);
BSLMF_ASSERT(bslma::UsesBslmaAllocator<Obj>::value);

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

const char *const LONG_STRING    = SUFFICIENTLY_LONG_STRING "0";
const char *const LONGER_STRING  = SUFFICIENTLY_LONG_STRING "00";
const char *const LONGEST_STRING = SUFFICIENTLY_LONG_STRING "000";

// Note that each '*_STRING' passes 'Obj::isValidNumber'.

struct DefaultDataRow {
    int         d_line;  // source line number
    char        d_mem;   // expected allocation: 'Y', 'N', '?'
    const char *d_text_p;
};

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //  LINE  MEM  TEXT
    //  ----  ---  ----------------
      { L_  , 'N',            "0" }  // default value
    , { L_  , 'N',            "1" }
    , { L_  , 'N',           "11" }
    , { L_  , 'N',          "111" }
    , { L_  , 'Y',    LONG_STRING }
    , { L_  , 'Y',  LONGER_STRING }
    , { L_  , 'Y', LONGEST_STRING }
};
enum { DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA };

// ============================================================================
//                HELPERS FOR REVIEW & LOG MESSAGE HANDLING
// ----------------------------------------------------------------------------

static bool containsCaseless(const bsl::string_view& string,
                             const bsl::string_view& subString)
    // Return 'true' if the specified 'subString' is present in the specified
    // 'string' disregarding case of alphabet characters '[a-zA-Z]', otherwise
    // return 'false'.
{
    if (subString.empty()) {
        return true;                                                  // RETURN
    }

    typedef bdlb::StringViewUtil SVU;
    const bsl::string_view rsv = SVU::strstrCaseless(string, subString);

    return !rsv.empty();
}

// ============================================================================
//                   EXPECTED 'BSLS_REVIEW' TEST HANDLERS
// ----------------------------------------------------------------------------

// These handlers are needed only temporarily until we determine how to fix the
// broken contract of 'bdlb::NumericParseUtil::parseDouble()' that says under-
// and overflow is not allowed yet the function supports it.

bool isBdlbNumericParseUtilReview(const bsls::ReviewViolation& reviewViolation)
    // Return 'true' if the specified 'reviewViolation' has been raised by the
    // 'bdlb_numericparseutil' component or no source file names are supported
    // by the build, otherwise return 'false'.
{
    const char *fn = reviewViolation.fileName();
    const bool fileOk = ('\0' == fn[0] // empty or has the component name
                             || containsCaseless(fn, "bdlb_numericparseutil"));
    return fileOk;
}

bool isUnderflowReview(const bsls::ReviewViolation& reviewViolation)
    // Return 'true' if the specified 'reviewViolation' is an underflow message
    // from the 'bdlb_numericparseutil' component (or no source file names are
    // supported by the build), otherwise return 'false'.
{

    return containsCaseless(reviewViolation.comment(), "underflow")
        && isBdlbNumericParseUtilReview(reviewViolation);
}

bool isOverflowReview(const bsls::ReviewViolation& reviewViolation)
    // Return 'true' if the specified 'reviewViolation' is an overflow message
    // from the 'bdlb_numericparseutil' component (or no source file names are
    // supported by the build), otherwise return 'false'.
{

    return containsCaseless(reviewViolation.comment(), "overflow")
        && isBdlbNumericParseUtilReview(reviewViolation);
}

void expectUnderflow(const bsls::ReviewViolation& reviewViolation)
    // If the specified 'reviewViolation' is an expected underflow-related
    // message from 'parseDouble' do nothing, otherwise call
    // 'bsls::Review::failByAbort()'.
{
    if (!isUnderflowReview(reviewViolation)) {
        bsls::Review::failByAbort(reviewViolation);
    }
}

void expectOverflow(const bsls::ReviewViolation& reviewViolation)
    // If the specified 'reviewViolation' is an expected overflow-related
    // message from 'parseDouble' do nothing, otherwise call
    // 'bsls::Review::failByAbort()'.
{
    if (!isOverflowReview(reviewViolation)) {
        bsls::Review::failByAbort(reviewViolation);
    }
}

// ============================================================================
//                          CLASSES FOR TESTING
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
//                      TEST FUNCTION TEMPLATES
// ----------------------------------------------------------------------------

template <class NUMERIC_TYPE>
struct DATA_TC10 {
    int          d_line;  // source line number
    char         d_mem;   // expected allocation: 'Y', 'N', '?'
    NUMERIC_TYPE d_value;
};

template <class INTEGRAL_TYPE>
void testCase11I(int        argc,
                 char      *argv[],
                 int (Obj::*accessor)(INTEGRAL_TYPE*) const)
    // Test value constructors that accept a value of the (template parameter)
    // 'INTEGRAL_TYPE'.  Create a series of objects from different allocator
    // configurations and having values of 'INTEGRAL_TYPE' that include the
    // maximum and minimum of that type.  Use the specified 'accessor' to
    // verify the value of the constructed object.  Confirm that the expected
    // status is returned for success and for all supported error states.  Set
    // the verbosity level according to the specified 'argc' and 'argv'.
{
    BSLS_ASSERT(accessor);

    BSLA_MAYBE_UNUSED const int                 test = argc > 1
                                                     ? bsl::atoi(argv[1])
                                                     : 0;
    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    if (veryVerbose) {
        P(bsls::NameOf<INTEGRAL_TYPE>());
    }

    const INTEGRAL_TYPE ZERO = static_cast<INTEGRAL_TYPE>(0);
    const INTEGRAL_TYPE ONE  = static_cast<INTEGRAL_TYPE>(1);
    const INTEGRAL_TYPE MAX  = bsl::numeric_limits<INTEGRAL_TYPE>::max();
    const INTEGRAL_TYPE MIN  = bsl::numeric_limits<INTEGRAL_TYPE>::min();

    const bool IS_SIGNED     = bsl::numeric_limits<INTEGRAL_TYPE>::is_signed;

    const DATA_TC10<INTEGRAL_TYPE> DATA[] = {

        //  LINE  MEM  VALUE
        //  ----  ---  ----
          { L_  , 'N', ZERO  }

        , { L_  , '?', MIN   }  // some types/platforms exceed SSO
        , { L_  , 'N', ONE   }
        , { L_  , '?', MAX   }  // some types/platforms exceed SSO
    };
        // Note that for unsigned types, the minimum value equals the default
        // value.

    bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

    if (veryVerbose) cout << "Test CTORs and ACCESSORs" << endl;
    {
        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int           LINE  = DATA[ti].d_line;
            const char          MEM   = DATA[ti].d_mem;
            const INTEGRAL_TYPE VALUE = DATA[ti].d_value;

            if (veryVerbose) {
                T_ P_(LINE) P_(MEM) P(VALUE)
            }

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) {
                    T_ T_ P(CONFIG)
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr          = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(VALUE);                       // TEST
                  } break;
                  case 'b': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(VALUE, 0);                    // TEST
                  } break;
                  case 'c': {
                    objAllocatorPtr = &sa;
                    objPtr = new (fa) Obj(VALUE, objAllocatorPtr);      // TEST
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(!"reachable");
                  } break;
                }

                Obj&                   mX = *objPtr;  const Obj& X = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                ASSERTV(CONFIG, &oa == X.value().get_allocator().mechanism());

                // Also invoke the object's 'get_allocator' accessor.

                ASSERTV(CONFIG, &oa, ALLOC_OF(X), &oa == X.allocator());

                switch (MEM) {
                  case 'Y': {
                    ASSERTV(CONFIG,  oa.numBlocksTotal(),
                            1 ==  oa.numBlocksTotal());
                  } break;
                  case 'N': {
                    ASSERTV(CONFIG,  oa.numBlocksTotal(),
                            0 ==  oa.numBlocksTotal());
                  } break;
                  case '?': {
                   // Skip test
                  } break;
                  default : {
                    BSLS_ASSERT_OPT(!"reachable");
                  } break;
                }
                ASSERTV(CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                INTEGRAL_TYPE asValue;
                int           rc = (X.*accessor)(&asValue);             // TEST

                ASSERTV(CONFIG, rc,             0     == rc);
                ASSERTV(CONFIG, asValue, VALUE, VALUE == asValue);

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
                ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
                ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
            }
        }
    }

    if (veryVerbose) cout << "Test CTORs and ACCESSORs" << endl;

    const bsl::string MAX_AS_STRING = Obj(MAX).value();

    if (veryVerbose) cout << "Test overflow behavior" << endl;
    {
        bsls::ReviewFailureHandlerGuard reviewGuard(&expectOverflow);

        bsl::string text(MAX_AS_STRING);
        text += "0";  // Multiply maximum value by 10.

        Obj mX(text); const Obj& X = mX;

        INTEGRAL_TYPE asValue;
        int           rc = (X.*accessor)(&asValue);                     // TEST

        ASSERTV(rc,      Obj::k_OVERFLOW == rc);
        ASSERTV(asValue, MAX             == asValue);
    }

    if (veryVerbose) cout << "Test underflow behavior" << endl;
    {
        bsls::ReviewFailureHandlerGuard reviewGuard(&expectUnderflow);

        if (IS_SIGNED) {
            bsl::string text(MAX_AS_STRING);
            text =       text + "0";  // Multiply maximum value by 10.
            text = "-" + text;        // Negate the too large value

            Obj mX(text); const Obj& X = mX;

            INTEGRAL_TYPE asValue;
            int           rc = (X.*accessor)(&asValue);                 // TEST

            ASSERTV(rc,      Obj::k_UNDERFLOW == rc);
            ASSERTV(asValue, MIN              == asValue);
        }
        else {
            if (veryVeryVerbose) cout << "Skip for signed type" << endl;
        }
    }

    if (veryVerbose) cout << "Test handling of non-integral value" << endl;
    {
        const INTEGRAL_TYPE MAX_M1 = MAX - 1;            // Nearly 'MAX' value
        const bsl::string   text   = Obj(MAX_M1).value() // Append fraction
                                   + "."
                                   + "14159265359";      // "Stolen" from PI

        Obj mX(text); const Obj& X = mX;

        INTEGRAL_TYPE asValue;
        int           rc = (X.*accessor)(&asValue);                     // TEST

        ASSERTV(rc,      Obj::k_NOT_INTEGRAL == rc);
        ASSERTV(asValue, MAX_M1              == asValue);
    }

    if (veryVerbose) cout << "Test value assignment" << endl;
    {
        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int           LINE1   = DATA[ti].d_line;
            const char          MEMDST1 = DATA[ti].d_mem;
            const INTEGRAL_TYPE VALUE1  = DATA[ti].d_value;

            if (veryVerbose) {
                T_ P_(LINE1) P_(MEMDST1) P(VALUE1)
            }

            for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int           LINE2   = DATA[tj].d_line;
                const char          MEMSRC2 = DATA[tj].d_mem;
                const INTEGRAL_TYPE VALUE2  = DATA[tj].d_value;

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(MEMSRC2) P(VALUE2)
                }

                Obj mX(VALUE1); const Obj& X = mX;

                Obj *mR = &(mX = VALUE2);                               // TEST
                ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX)

                INTEGRAL_TYPE asValue;
                int           rc = (X.*accessor)(&asValue);

                ASSERTV(rc,              0      == rc);
                ASSERTV(asValue, VALUE2, VALUE2 == asValue);
            }
        }
    }
}

static void times10(bsl::string *result, const bsl::string& input)
    // Load to the specified 'result' the textual representation of the value
    // represented by the specified 'input'.  Note that calculation of the
    // larger value is achieved by textually inserting a '0' character before
    // the decimal point, if present, or before the 'e' (for exponent) if there
    // is no decimal point, or by appending a '0' if neither decimal point nor
    // 'e' is present.  The behavior is undefined unless
    // 'bdljsn::NumberUtil::isValidNumber(input)' is 'true'.
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(bdljsn::NumberUtil::isValidNumber(input));

    bsl::size_t posDot = input.find('.');
    bsl::size_t posExp = input.find('e');

    bsl::string output = input;

    if        (bsl::string::npos != posDot) { output.insert(posDot, 1, '0');
    } else if (bsl::string::npos != posExp) { output.insert(posExp, 1, '0');
    } else                                  { output.append(        1, '0');
    }

    *result = output;
}

template <class FLOATING_TYPE>
void testCase11F(int                  argc,
                 char                *argv[],
                 FLOATING_TYPE (Obj::*accessor)() const)
    // Test value constructors that accept a value of the (template parameter)
    // 'INTEGRAL_TYPE'.  Create a series of objects from different allocator
    // configurations and having values of 'INTEGRAL_TYPE' that include the
    // maximum and minimum of that type.  Use the specified 'accessor' to
    // verify the value of the constructed object.  Confirm that the expected
    // status is returned for success and for all supported error states.  Set
    // the verbosity level according to the specified 'argc' and 'argv'.
{
    BSLS_ASSERT(accessor);

    BSLA_MAYBE_UNUSED const int                 test = argc > 1
                                                     ? bsl::atoi(argv[1])
                                                     : 0;
    BSLA_MAYBE_UNUSED const bool             verbose = argc > 2;
    BSLA_MAYBE_UNUSED const bool         veryVerbose = argc > 3;
    BSLA_MAYBE_UNUSED const bool     veryVeryVerbose = argc > 4;
    BSLA_MAYBE_UNUSED const bool veryVeryVeryVerbose = argc > 5;

    if (veryVerbose) {
        P(bsls::NameOf<FLOATING_TYPE>());
    }

    const FLOATING_TYPE ZERO = static_cast<FLOATING_TYPE>(0);
    const FLOATING_TYPE ONE  = static_cast<FLOATING_TYPE>(1);
    const FLOATING_TYPE MAX  = bsl::numeric_limits<FLOATING_TYPE>::max();
    const FLOATING_TYPE MIN  = bsl::numeric_limits<FLOATING_TYPE>::min();

    const DATA_TC10<FLOATING_TYPE> DATA[] = {

        //  LINE  MEM  VALUE
        //  ----  ---  -----
          { L_  , 'N', ZERO   }

        , { L_  , '?',  MIN   } // temporary allocation for some types
        , { L_  , '?',  ONE   } // "                                 "
        , { L_  , '?',  MAX   } // "                                 "

        , { L_  , '?', -MIN   } // temporary allocation for some types
        , { L_  , '?', -ONE   } // "                                 "
        , { L_  , '?', -MAX   } // "                                 "
    };

    bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
    #define STATIC_CAST(X) static_cast<FLOATING_TYPE>((X))
#else
    #define STATIC_CAST(X) (X.*accessor)()
#endif

    if (veryVerbose) cout << "Test CTORs and ACCESSORs" << endl;
    {

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int           LINE  = DATA[ti].d_line;
            const char          MEM   = DATA[ti].d_mem;
            const FLOATING_TYPE VALUE = DATA[ti].d_value;

            if (veryVerbose) {
                T_ P_(LINE) P_(MEM) P(VALUE)
            }

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) {
                    T_ T_ P(CONFIG)
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr          = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(VALUE);                       // TEST
                  } break;
                  case 'b': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(VALUE, 0);                    // TEST
                  } break;
                  case 'c': {
                    objAllocatorPtr = &sa;
                    objPtr = new (fa) Obj(VALUE, objAllocatorPtr);      // TEST
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(!"reachable");
                  } break;
                }

                Obj&                   mX = *objPtr;  const Obj& X = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                ASSERTV(CONFIG, &oa == X.value().get_allocator().mechanism());

                // Also invoke the object's 'allocator' accessor.

                ASSERTV(CONFIG, &oa, ALLOC_OF(X), &oa == X.allocator());

                switch (MEM) {
                  case 'Y': {
                    ASSERTV(CONFIG,  oa.numBlocksTotal(),
                            1 ==  oa.numBlocksTotal());
                  } break;
                  case 'N': {
                    ASSERTV(CONFIG,  oa.numBlocksTotal(),
                            0 ==  oa.numBlocksTotal());
                  } break;
                  case '?': {
                   // Skip test
                  } break;
                  default : {
                    BSLS_ASSERT_OPT(!"reachable");
                  } break;
                }
                ASSERTV(CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                const FLOATING_TYPE asValueM = (X.*accessor)();         // TEST
                const FLOATING_TYPE asValueO = STATIC_CAST(X);          // TEST

                ASSERTV(CONFIG, asValueM, VALUE, VALUE == asValueM);
                ASSERTV(CONFIG, asValueO, VALUE, VALUE == asValueO);

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
                ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
                ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
            }
        }
    }

#undef STATIC_CAST

    const bsl::string MAX_AS_STRING = Obj(MAX).value();
    const bsl::string MIN_AS_STRING = Obj(MIN).value();

    bsl::string tmp; times10(&tmp, MAX_AS_STRING);

    const bsl::string SUPER_MAX_AS_STRING = tmp;
    const bsl::string SUPER_MIN_AS_STRING = MIN_AS_STRING  + "0";
                                           // Multiply negative exponent by 10.

    if (veryVeryVerbose) {
        P_(MAX_AS_STRING); P(SUPER_MAX_AS_STRING);
        P_(MIN_AS_STRING); P(SUPER_MIN_AS_STRING);
    }

    const FLOATING_TYPE k_INF = bsl::numeric_limits<FLOATING_TYPE>::infinity();

    if (veryVerbose) cout << "Test overflow behavior" << endl;
    {
        bsls::ReviewFailureHandlerGuard reviewGuard(&expectOverflow);

        Obj mX(      SUPER_MAX_AS_STRING); const Obj& X = mX;
        Obj mY("-" + SUPER_MAX_AS_STRING); const Obj& Y = mY;

        const FLOATING_TYPE asValueP = (X.*accessor)();                 // TEST
        const FLOATING_TYPE asValueN = (Y.*accessor)();                 // TEST

        ASSERTV(asValueP,  k_INF == asValueP);
        ASSERTV(asValueN, -k_INF == asValueN);
    }

    if (veryVerbose) cout << "Test underflow  behavior" << endl;
    {
        bsls::ReviewFailureHandlerGuard reviewGuard(&expectUnderflow);

        Obj mX(      SUPER_MIN_AS_STRING); const Obj& X = mX;
        Obj mY("-" + SUPER_MIN_AS_STRING); const Obj& Y = mY;

        const FLOATING_TYPE asValueP = (X.*accessor)();                 // TEST
        const FLOATING_TYPE asValueN = (Y.*accessor)();                 // TEST

        ASSERTV(asValueP, ZERO == asValueP);
        ASSERTV(asValueN, ZERO == asValueN);
    }

    if (veryVerbose) cout << "Test value assignment" << endl;
    {
        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int           LINE1   = DATA[ti].d_line;
            const char          MEMDST1 = DATA[ti].d_mem;
            const FLOATING_TYPE VALUE1  = DATA[ti].d_value;

            if (veryVerbose) {
                T_ P_(LINE1) P_(MEMDST1) P(VALUE1)
            }

            for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int           LINE2   = DATA[tj].d_line;
                const char          MEMSRC2 = DATA[tj].d_mem;
                const FLOATING_TYPE VALUE2  = DATA[tj].d_value;

                if (veryVerbose) {
                    T_ T_ P_(LINE2) P_(MEMSRC2) P(VALUE2)
                }

                Obj mX(VALUE1); const Obj& X = mX;

                Obj *mR = &(mX = VALUE2);                               // TEST
                ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX)

                const FLOATING_TYPE asValue = (X.*accessor)();

                ASSERTV(asValue, VALUE2, VALUE2 == asValue);
            }
        }
    }

    const FLOATING_TYPE k_NAN = bsl::numeric_limits<FLOATING_TYPE>::
                                                                   quiet_NaN();

    if (veryVerbose) cout << "Negative tests" << endl;
    {
        bsls::AssertTestHandlerGuard hG;

        ASSERT_FAIL((Obj( k_NAN)));  // (avoid) most-vexing-parse
        ASSERT_FAIL((Obj( k_INF)));  // (avoid) most-vexing-parse
        ASSERT_FAIL( Obj(-k_NAN) );
        ASSERT_FAIL( Obj(-k_INF) );

        ASSERT_FAIL(Obj() =  k_NAN);
        ASSERT_FAIL(Obj() =  k_INF);
        ASSERT_FAIL(Obj() = -k_NAN);
        ASSERT_FAIL(Obj() = -k_INF);
    }
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? bsl::atoi(argv[1]) : 0;

    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: Unexpected 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    // CONCERN: In no case does memory come from the global allocator.
    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 17: {
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

        if (verbose) { cout << endl
                            << "USAGE EXAMPLE" << endl
                            << "=============" << endl;
        }

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating JSON Number Object from User Input
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The specification of values for JSON numbers often starts with user input
// textual representations of those values.  As the specifications for valid
// representation are complicated and not always intuitive it is prudent to
// validate that input using the 'bdljsn::JsonNumber::isValidNumber' function;
// otherwise, one might try to create a 'bdljsn::JsonNumber' object from an
// invalid specification and that leads to undefined behavior.
//
// First, as a expedient for this example, we organize in an array input that
// might well be entered by some user:
//..
    struct {
        const char *d_text_p;
        const char *d_description_p;
        bool        d_expected;
    } USER_INPUT[] = {

    //  VALUE                   DESCRIPTION                             EXP
    //  ----------------------  --------------------------------------  ---

      // Invalid Input (that is valid in other contexts).

      { "1.",                   "Not uncommon way to write '1'."      , 0  }
    , { "1,000",                "No commas allowed"                   , 0  }
    , { "01",                   "Leading '0',  disallowed by JSON."   , 0  }
    , { "",                     "0 per 'atoi', disallowed by JSON."   , 0  }
    , { "Hello, world!",        "0 per 'atoi', disallowed by JSON."   , 0  }
    , { "NaN",                  "invalid number"                      , 0  }
    , { "INF",                  "invalid number"                      , 0  }
    , { "-INF",                 "invalid number"                      , 0  }
    , { "+INF",                 "invalid number"                      , 0  }

      // Valid input (some surprising)

    , { "1234567890",           "Integral value"                      , 1  }
    , { "1234567890.123456",    "Non-integral value"                  , 1  }
    , { "1234567890.1234567",   "Beyond Decimal64 precision"          , 1  }
    , { "-9223372036854775809", "INT64_MIN, underflow, but valid JSON", 1  }
    , { "1.5e27",               "INT64_MAX,  overflow, but valid JSON", 1  }
    , { "999999999999999999999999999999999999999999999999999999999999"
        "e"
        "999999999999999999999999999999999999999999999999999999999999",
                                "astronomic value"                    , 1 }
    };

    const bsl::size_t NUM_USER_INPUT = sizeof USER_INPUT / sizeof *USER_INPUT;
//..
// Now, if and only if the input is valid, we use the input to construct a
// 'bdljsn::JsonNumber' object and add that object to a vector for later
// processing.
//..
    bsl::vector<bdljsn::JsonNumber> userInput; // when valid input

    for (bsl::size_t ti = 0; ti < NUM_USER_INPUT; ++ti) {
        const char *TEXT = USER_INPUT[ti].d_text_p;
        const char *DESC = USER_INPUT[ti].d_description_p; (void) DESC;
        const bool  EXP  = USER_INPUT[ti].d_expected;

        const bool isValid  = bdljsn::JsonNumber::isValidNumber(TEXT);
        ASSERT(EXP == isValid);

        if (isValid) {
            userInput.push_back(bdljsn::JsonNumber(TEXT));
        }
    }
//..
// Finally, we confirm that the vector has the expected number of elements:
//..
    ASSERT(6 == userInput.size());
//..
//
///Example 2: Using 'bdljsn::JsonNumber' Objects
///- - - - - - - - - - - - - - - - - - - - - - -
// We saw in {Example 1} that 'bdljsn::JsonNumber' objects can validly hold
// values numeric values that cannot be converted to any of the supported types
// (e.g., the "astronomic value") for arithmetic operations.  Applications that
// accept arbitrary 'bdljsn::JsonNumber' objects should be prepared to
// categorize the contained value and adapt their handling accordingly.  In
// practice, applications may have some assurances of the contents of received
// 'bdljsn::JsonNumber' objects.  Here, we intentionally avoid such assumptions
// to explore the wide range of variations that can arise.
//
// Legend, in the output below:
//
//: o "OK":
//:   o Means "OKay to use".  In some cases, the numeric value of the
//:     arithmetic type is an approximation of JSON number and the application
//:     may have to allow for that difference.
//:
//: o "NG":
//:   o Means "No Good" (do not use).  The JSON number is outside of the valid
//:     range of the arithmetic type.
//
// First, we set up a framework (in this case, a 'for' loop) for examining our
// input, the same 'userInput' vector created in {Example 1}:
//..
    for (bsl::size_t i = 0; i < userInput.size(); ++i) {
        const bdljsn::JsonNumber obj = userInput[i];
//..
// Then, we categorize the value as integral or not:
//..
        if (obj.isIntegral()) {
            bsl::cout << "Integral: ";
//..
// If integral, we check if the value is a usable range.  Let us assume that
// 'bslsl::Type::Int64' is as large a number as we can accept.
//
// Then, we convert the JSON number to that type and check for overflow and
// underflow:
//..
            bsls::Types::Int64 value;
            int                rc = obj.asInt64(&value);
            switch (rc) {
                case 0: {
                    bsl::cout << value      << " : OK to USE" << bsl::endl;
                } break;
                case bdljsn::JsonNumber::k_OVERFLOW: {
                    bsl::cout << obj.value() << ": NG too large" << bsl::endl;
                } break;
                case bdljsn::JsonNumber::k_UNDERFLOW: {
                    bsl::cout << obj.value() << ": NG too small" << bsl::endl;
                } break;
                case bdljsn::JsonNumber::k_NOT_INTEGRAL: {
                  ASSERT(!"reached");
                } break;
            }
//..
// Next, if the value is not integral, we try to handle it as a floating point
// value -- a 'bdldfp::Decimal64' in this example -- and further categorize it
// as exact/inexact, too large/small.
//..
        } else {
            bsl::cout << "Not-Integral: ";

            bdldfp::Decimal64 value;
            int               rc = obj.asDecimal64Exact(&value);
            switch (rc) {
                case 0: {
                    bsl::cout << value << " :  exact: OK to USE";
                } break;
                case bdljsn::JsonNumber::k_INEXACT: {
                    bsl::cout << value << ": inexact: USE approximation";
                } break;
                case bdljsn::JsonNumber::k_NOT_INTEGRAL: {
                  ASSERT(!"reached");
                } break;
            }

            const bdldfp::Decimal64 INF =
                            bsl::numeric_limits<bdldfp::Decimal64>::infinity();

            if        ( INF == value) {
                bsl::cout << ": NG too large" << bsl::endl;
            } else if (-INF == value) {
                bsl::cout << ": NG too small" << bsl::endl;
            } else {
                bsl::cout << bsl::endl;
            }
        }
    }
//..
// Finally, we observe for particular input:
//..
//  Integral: 1234567890 : OK to USE
//  Not-Integral: 1234567890.123456 :  exact: OK to USE
//  Not-Integral: 1234567890.123457: inexact: USE approximation
//  Integral: -9223372036854775809: NG too small
//  Integral: 1.5e27: NG too large
//  Integral: 999999999999999999999999999999999999999999999999999999999999e9999
//  99999999999999999999999999999999999999999999999999999999: NG too large
//..
      } break;
      case 16: {
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
        //   void hashAppend(HASHALG& hashAlgorithm, const JsonNumber& object);
        // --------------------------------------------------------------------

        if (verbose) cout << "TEST 'hashAppend'" << endl
                          << "=================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::TestAllocatorMonitor  dam(&da);
        bslma::DefaultAllocatorGuard dag(&da);

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE = DATA[ti].d_line;
            const char *const TEXT = DATA[ti].d_text_p;

            Obj mX(TEXT); const Obj& X = mX;

            if (veryVerbose) { T_ P_(LINE) P(X) }

            MockAccumulatingHashingAlgorithm expd;
            MockAccumulatingHashingAlgorithm calc;

            using bslh::hashAppend;
            hashAppend(expd, bsl::string(TEXT));

            bdljsn::hashAppend(calc, X);                                // TEST

            ASSERTV(LINE, TEXT, expd == calc);
        }

        ASSERT(dam.isInUseSame());
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TEST 'asDecimal64Exact'
        //
        // Concerns:
        //: 1 The method correctly converts JSON numbers in the supported range
        //:   to 'bdldfp::Decimal64' values.  When the JSON number is outside
        //:   of that range (e.g., to many decimal places) an inexact value is
        //:   delivered and the return status is 'k_INEXACT'.
        //:
        //: 2 The method under test is 'const' qualified and has return type
        //:   'int'
        //:
        //: 3 There is no allocation from the object allocator.
        //:
        //:   1 There may be allocation directly from the default allocator.
        //:
        //: 4 The behavior of this class method matches that of
        //:   'bdljsn::NumericUtil::asDecimal64Exact', an "oracle" for the
        //:   function under test.
        //
        // Plan:
        //: 1 Use the address of the 'asDecimal64Exact' non-'static' method to
        //:   initialize a pointer to a function having the appropriate
        //:   signature and return type.  (C-2)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   object allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Select a sample of (valid) JSON numbers that include the
        //:     extremes of supported values.  For each value, define a
        //:     "neighboring" values that has too many decimal places for exact
        //:     conversion.
        //:
        //:   2 For each row in the table of P-3, verify that return status
        //:     matches the expected value.  (C-1)
        //:
        //:   3 Confirm that the oracle function produces the same result as
        //:     the function under test.  (C-4)
        //:
        //: 4 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the object allocator.  (C-3)
        //
        // Testing:
        //   int asDecimal64Exact(bdldfp::Decimal64 *result) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST 'asDecimal64Exact'" << endl
                          << "=======================" << endl;

        if (verbose) cout <<
                    "\nAssign the address of 'isEqual' to a variable." << endl;
        {
            typedef int (Obj::*funcPtr)(Deci64 * ) const;

            // Verify that the signature and return type are as expected.

            funcPtr memberFunc = &Obj::asDecimal64Exact;

            (void)memberFunc;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
            "\nCreate a table of distinct candidate attribute values." << endl;

#define Sixteen9s "9999999999999999"
        const Obj MaxNeg("-" Sixteen9s "e" "+369");
        const Obj MaxPos(    Sixteen9s "e" "+369");

        Deci64 maxNegDeci64; const Deci64& MaxNegDeci64 = maxNegDeci64;
        Deci64 maxPosDeci64; const Deci64& MaxPosDeci64 = maxPosDeci64;

        int maxNegDeci64Rc = MaxNeg.asDecimal64Exact(&maxNegDeci64);
        int maxPosDeci64Rc = MaxPos.asDecimal64Exact(&maxPosDeci64);

        ASSERTV(maxNegDeci64Rc, 0 == maxNegDeci64Rc);
        ASSERTV(maxPosDeci64Rc, 0 == maxPosDeci64Rc);

#define TooManyDigits "12345678901234567"   // 17 digits

// Return Codes
#define OK 0
#define IE Obj::k_INEXACT // -4

// Behavioral realms
#define XAC        0 // Exact
#define XXX        1 // Inexact, but no value of interest
#define RT_MaxNeg  2 // Inexact, round toward 'MaxNegDeci64'
#define RT_MaxPos  3 // Inexact, round toward 'MaxPosDeci64'
#define OVF_pos    4 // Inexact, overflow toward +INF
#define OVF_neg    5 // Inexact, overflow toward -INF
#define RTZ        6 // Inexact, round to zero.

        const struct {
            int         d_line;   // line
            int         d_rc;     // expected status
            const char *d_text_p; // input text
            int         d_realm;
        } DATA[] = {
           // LINE  RC  TEXT                              REALM
           // ----  --  --------------------------------  -------
            { L_  , IE, "-" Sixteen9s           "e+3690", OVF_neg   }

        ,   { L_  , IE, "-" Sixteen9s ".9"      "e+369" , OVF_neg   }
        ,   { L_  , IE, "-" Sixteen9s ".8"      "e+369" , OVF_neg   }
        ,   { L_  , IE, "-" Sixteen9s ".7"      "e+369" , OVF_neg   }
        ,   { L_  , IE, "-" Sixteen9s ".6"      "e+369" , OVF_neg   }
        ,   { L_  , IE, "-" Sixteen9s ".5"      "e+369" , OVF_neg   }
        ,   { L_  , IE, "-" Sixteen9s ".499999" "e+369" , RT_MaxNeg }
        ,   { L_  , IE, "-" Sixteen9s ".4"      "e+369" , RT_MaxNeg }
        ,   { L_  , IE, "-" Sixteen9s ".3"      "e+369" , RT_MaxNeg }
        ,   { L_  , IE, "-" Sixteen9s ".2"      "e+369" , RT_MaxNeg }
        ,   { L_  , IE, "-" Sixteen9s ".1"      "e+369" , RT_MaxNeg }
        ,   { L_  , OK, "-" Sixteen9s           "e+369" , XAC       } // MaxNeg

        ,   { L_  , OK,           "1"                   , XAC       }
        ,   { L_  , IE,           "1" "." TooManyDigits , XXX       }

        ,   { L_  , OK, "-" Sixteen9s           "e-398" , XAC       }
        ,   { L_  , IE, "-" Sixteen9s ".1"      "e-398" , XXX       }

        ,   { L_  , OK,           "0"                   , XAC       }
        ,   { L_  , IE,           "0" "." TooManyDigits , XXX       }

        ,   { L_  , OK,     Sixteen9s           "e-398" , XAC       }
        ,   { L_  , IE,     Sixteen9s ".1"      "e-398" , XXX       }

        ,   { L_  , OK,           "1"                   , XAC       }
        ,   { L_  , IE,           "1" "." TooManyDigits , XAC       }

        ,   { L_  , OK,     Sixteen9s           "e+369" , XAC       } // MaxPos
        ,   { L_  , IE,     Sixteen9s ".1"      "e+369" , RT_MaxPos }
        ,   { L_  , IE,     Sixteen9s ".2"      "e+369" , RT_MaxPos }
        ,   { L_  , IE,     Sixteen9s ".3"      "e+369" , RT_MaxPos }
        ,   { L_  , IE,     Sixteen9s ".4"      "e+369" , RT_MaxPos }
        ,   { L_  , IE,     Sixteen9s ".499999" "e+369" , RT_MaxPos }
        ,   { L_  , IE,     Sixteen9s ".5"      "e+369" , OVF_pos   }
        ,   { L_  , IE,     Sixteen9s ".6"      "e+369" , OVF_pos   }
        ,   { L_  , IE,     Sixteen9s ".7"      "e+369" , OVF_pos   }
        ,   { L_  , IE,     Sixteen9s ".8"      "e+369" , OVF_pos   }
        ,   { L_  , IE,     Sixteen9s ".9"      "e+369" , OVF_pos   }

        ,   { L_  , IE,     Sixteen9s           "e+3690", OVF_pos   }
        };

        const bsl::size_t  NUM_DATA = sizeof DATA / sizeof *DATA;

#undef Sixteen9s
#undef OK
#undef IE

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE   = DATA[ti].d_line;
            const int         EXP_RC = DATA[ti].d_rc;
            const char *const TEXT   = DATA[ti].d_text_p;
            const int         REALM  = DATA[ti].d_realm;

            if (veryVerbose) {
                P_(LINE) P_(EXP_RC) P_(REALM) P(TEXT)
            }

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            const Obj X(TEXT, &sa);

            bslma::TestAllocatorMonitor sam(&sa);

            Deci64 result;
            Deci64 resultOracle;

            int rc       =   X.asDecimal64Exact(&result);               // TEST
            int rcOracle = NU::asDecimal64Exact(&resultOracle, TEXT);

            ASSERTV(LINE, EXP_RC, rc,       EXP_RC == rc);
            ASSERTV(LINE, EXP_RC, rcOracle, EXP_RC == rcOracle);

            ASSERTV(LINE, resultOracle,   result,
                          resultOracle == result);

            const Deci64 k_INF = bsl::numeric_limits<Deci64>::infinity();

            switch (REALM) {
              case OVF_neg: { // overflow toward -INF
                ASSERTV(LINE, -k_INF,   result,
                              -k_INF == result);
              } break;
              case XAC:                                         // FALL THROUGH
              case XXX: {
                // Checked against oracle (above).  No other check needed.
              } break;
              case RT_MaxNeg: { // Inexact and round toward 'MaxNeg'
                ASSERTV(LINE,  MaxNegDeci64,   result,
                               MaxNegDeci64 == result);
              } break;
              case RT_MaxPos: { // Inexact and round toward 'MaxPos'
                ASSERTV(LINE,  MaxPosDeci64,   result,
                               MaxPosDeci64 == result);
              } break;
              case OVF_pos: {  // overflow toward +INF
                ASSERTV(LINE,  k_INF,   result,
                               k_INF == result);
              } break;
              case RTZ: {     // Inexact and round to zero.
                ASSERTV(LINE,  Deci64(0),   result,
                               Deci64(0) == result);
              } break;
              default: {
                BSLS_ASSERT_OPT(!"reachable");
              } break;
            }

            ASSERT(sam.isTotalSame());
        }
#undef XAC
#undef XXX
#undef RT_MaxNeg
#undef RT_MaxPos
#undef OVF_pos
#undef OVF_neg
#undef RTZ
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TEST 'isEqual'
        //
        // Concerns:
        //: 1 The method correctly identifies JSON numbers that have different
        //:   textual representations (values) but equal numeric values, even
        //:   if the numeric value cannot be represented by any support numeric
        //:   type (e.g., 'int', 'double').
        //:
        //: 2 The (predicate) method under test is 'const' qualified and has
        //:   return type 'bool'
        //:
        //: 3 There is no allocation from any allocator.
        //:
        //: 4 The behavior of this class method matches that of
        //:   'bdljsn::NumericUtil::areEqual', an "oracle" for the function
        //:   under test.
        //
        // Plan:
        //: 1 Use the address of the 'isIntegral' non-'static' method to
        //:   initialize a pointer to a function having the appropriate
        //:   signature and return type.  (C-2)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Select a representative sample of (valid) JSON numbers that are
        //:     organized into equivalence groups.  Within each equivalence
        //:     group, the JSON numbers have textual differences (e.g., adding
        //:     a trailing '0' to a factional part) but conceptually the same
        //:     numeric value.
        //:
        //:   2 For each row in the table of P-3, verify that the method
        //:     returns the expected value.  (C-1)
        //:
        //:   3 Confirm that the oracle function produces the same result as
        //:     the function under test.  (C-4)
        //:
        //: 4 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   bool isEqual(const JsonNumber& other) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST 'isEqual'" << endl
                          << "==============" << endl;

        if (verbose) cout <<
                    "\nAssign the address of 'isEqual' to a variable." << endl;
        {
            typedef bool (Obj::*funcPtr)(const Obj& ) const;

            // Verify that the signature and return type are as expected.

            funcPtr memberFunc = &Obj::isEqual;

            (void)memberFunc;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
            "\nCreate a table of distinct candidate attribute values." << endl;

        struct {
            int         d_line;              // line
            int         d_equalityGroup;     // group of equal numbers
            const char *d_text_p;            // input text
        } DATA[] = {
          // LINE GROUP  TEXT
          // ---- -----  --------------------------
            { L_,     0,                         "0" }
          , { L_,     0,                        "-0" }
          , { L_,     0,                       "0.0" }
          , { L_,     0,                    "0.0000" }
          , { L_,     0,             "0.0000e100000" }
          , { L_,     0,               "0.0e-100000" }
          , { L_,     0,              "0.00e+100000" }
          , { L_,     0,             "0.000e-100000" }
          , { L_,     0,    "0e18446744073709551615" }
          , { L_,     0,   "0e-18446744073709551615" }

          , { L_,     1,                         "1" }
          , { L_,     1,                     "1.0e0" }
          , { L_,     1,                     "0.1e1" }
          , { L_,     1,                    "0.1e+1" }
          , { L_,     1,                    "0.01e2" }
          , { L_,     1,                     "10e-1" }
          , { L_,     1,                   "10.0e-1" }

          , { L_,     2,                         "2" }
          , { L_,     2,                     "2.0e0" }
          , { L_,     2,                     "0.2e1" }
          , { L_,     2,                    "0.2e+1" }
          , { L_,     2,                    "0.02e2" }
          , { L_,     2,                     "20e-1" }
          , { L_,     2,                   "20.0e-1" }

          , { L_,     3,                        "-1" }
          , { L_,     3,                    "-1.0e0" }
          , { L_,     3,                    "-0.1e1" }
          , { L_,     3,                   "-0.1e+1" }
          , { L_,     3,                   "-0.01e2" }
          , { L_,     3,                    "-10e-1" }
          , { L_,     3,                  "-10.0e-1" }

          , { L_,     4,      "18446744073709551615" }
          , { L_,     4, "0.18446744073709551615e20" }

          , { L_,     5,    "1e18446744073709551615" }
          , { L_,     5,   "1e+18446744073709551615" }
          , { L_,     6,   "10e18446744073709551614" }

        };
        const bsl::size_t  NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int   LHS_LINE  = DATA[ti].d_line;
            const int   LHS_GROUP = DATA[ti].d_equalityGroup;
            const char *LHS       = DATA[ti].d_text_p;

            if (veryVerbose) {
                P_(LHS_LINE) P_(LHS_GROUP) P_(LHS)
            }

            const Obj X(LHS);

            for (bsl::size_t tj = 0; tj < NUM_DATA; ++tj) {
                const int   RHS_LINE  = DATA[tj].d_line;
                const int   RHS_GROUP = DATA[tj].d_equalityGroup;
                const char *RHS       = DATA[tj].d_text_p;

                if (veryVerbose) {
                    T_ P_(RHS_LINE) P_(RHS_GROUP) P_(RHS)
                }

                const Obj Y(RHS);

                bool EXP = RHS_GROUP == LHS_GROUP;

                bslma::TestAllocatorMonitor dam(&da);

                const bool rv       =    Y.isEqual(X);
                const bool rvOracle = NU::areEqual(LHS, RHS);

                ASSERTV(LHS, RHS, LHS_GROUP, RHS_GROUP, EXP == rv);
                ASSERTV(LHS, RHS, LHS_GROUP, RHS_GROUP, EXP == rvOracle);

                ASSERT(dam.isTotalSame());
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TEST 'isIntegral'
        //
        // Concerns:
        //: 1 The method correctly distinguishes JSON number values that have a
        //:   fractional part, and those whose fractional part evaluates to
        //:   zero.
        //:
        //: 2 The (predicate) method under test is 'const' qualified and has
        //:   return type 'bool'
        //:
        //: 3 There is no allocation from any allocator.
        //:
        //: 4 The behavior of this class method matches that of
        //:   'bdljsn::NumericUtil::isIntergalNumber', an "oracle" for the
        //:   function under test.
        //
        // Plan:
        //: 1 Use the address of the 'isIntegral' non-'static' method to
        //:   initialize a pointer to a function having the appropriate
        //:   signature and return type.  (C-2)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Select a representative sample of valid JSON numbers, some
        //:     having factional parts to their values, and others not.
        //
        //:     1 Note that the representation of an integer value can have
        //:       digits to the right of decimal point if the exponent value is
        //:       greater than the number of digits.
        //:
        //:   2 For each row in the table of P-3, verify that the method
        //:     returns the expected value.  (C-1)
        //:
        //:   3 Confirm that the oracle function produces the same result as
        //:     the function under test.  (C-4)
        //:
        //: 4 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   bool isIntegral() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST 'isIntegral'" << endl
                          << "=================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of 'isIntegral' to a variable." << endl;
        {
            typedef bool (Obj::*funcPtr)() const;

            // Verify that the signature and return type are as expected.

            funcPtr memberFunc = &Obj::isIntegral;

            (void)memberFunc;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
            "\nCreate a table of distinct candidate attribute values." << endl;

        struct {
            int         d_line;
            bool        d_isIntegral;
            const char *d_text_p;
        } DATA[] = {
           // LINE IS_INTEGRAL TEXT
           // ---  ----------- ---------------------------
            { L_,  true ,                              "0" }
          , { L_,  true ,                             "-0" }
          , { L_,  true ,                            "0.0" }
          , { L_,  true ,                         "0.0000" }
          , { L_,  true ,                  "0.0000e100000" }
          , { L_,  true ,                    "0.0e-100000" }
          , { L_,  true ,                   "0.00e+100000" }
          , { L_,  true ,                  "0.000e-100000" }
          , { L_,  true ,         "0e18446744073709551615" }
          , { L_,  true ,        "0e-18446744073709551615" }

          , { L_,  true ,                              "1" }
          , { L_,  true ,                          "1.0e0" }
          , { L_,  true ,                          "0.1e1" }
          , { L_,  true ,                         "0.1e+1" }
          , { L_,  true ,                         "0.01e2" }
          , { L_,  true ,                          "10e-1" }
          , { L_,  true ,                        "10.0e-1" }

          , { L_,  false,                            "0.1" }
          , { L_,  false,                         "1.0e-1" }
          , { L_,  false,                          "0.1e0" }
          , { L_,  false,                        "0.01e+1" }
          , { L_,  false,                         "0.01e1" }
          , { L_,  false,                          "10e-2" }
          , { L_,  false,                        "10.0e-2" }

          , { L_,  true ,                             "-1" }
          , { L_,  true ,          "-18446744073709551615" }
          , { L_,  true ,           "18446744073709551615" }
          , { L_,  true ,      "-184467440737095516151234" }
          , { L_,  true ,       "184467440737095516151234" }
          , { L_,  true ,        "18446744073709551610e-1" }
          , { L_,  true ,       "10000000000000000000e-19" }
          , { L_,  false,       "10000000000000000000e-20" }
          , { L_,  false,        "18446744073709551615e-1" }
        };

        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
            const int   LINE        = DATA[i].d_line;
            const bool  IS_INTEGRAL = DATA[i].d_isIntegral;
            const char *TEXT        = DATA[i].d_text_p;

            if (veryVerbose) {
                T_ P_(LINE) P_(IS_INTEGRAL) P(TEXT)
            }

            Obj mX(TEXT); const Obj& X = mX;

            const bool rc       =    X.isIntegral();                    // TEST
            const bool rcOracle = NU ::isIntegralNumber(TEXT);

            bslma::TestAllocatorMonitor dam(&da);

            ASSERTV(LINE, TEXT, rc, IS_INTEGRAL, IS_INTEGRAL == rc);
            ASSERTV(LINE, TEXT, rc, IS_INTEGRAL, IS_INTEGRAL == rcOracle);

            ASSERT(dam.isTotalSame());
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'isValidNumber'
        //
        // Concerns:
        //: 1 The method correctly distinguishes between valid and invalid
        //:   textual representation of JSON number values.
        //:
        //: 2 The (predicate) method under test has return type 'bool'.
        //:
        //: 3 There is no allocation from any allocator.
        //:
        //: 4 The behavior of this class method matches that of
        //:   'bdljsn::NumericUtil::isVaidNumber', an "oracle" for the function
        //:   under test.
        //
        // Plan:
        //: 1 Use the address of the 'isValidNumber' 'static' class method to
        //:   initialize a pointer to a function having the appropriate
        //:   signature and return type.  (C-2)
        //:
        //: 2 Create a 'bslma::TestAllocator' object, and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Select a representative sample of valid JSON numbers and text
        //:     strings consisting of JSON numbers with common typographical
        //:     errors.
        //:
        //:   2 For each row in the table of P-3, verify that the class method
        //:     returns the expected value.  (C-1)
        //:
        //:   3 Confirm that the oracle function produces the same result as
        //:     the function under test.  (C-4)
        //:
        //: 4 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   static bool isValidNumber(const bsl::string_view& text);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "CLASS METHOD 'isValidNumber'" << endl
                 << "============================" << endl;

        if (verbose) cout <<
              "\nAssign the address of 'isValidNumber' to a variable." << endl;
        {
            typedef bool (*freeFuncPtr)(const SV&);

            // Verify that the signature and return type are as expected.

            freeFuncPtr funcPtr = &Obj::isValidNumber;

            (void)funcPtr;  // quash potential compiler warning
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout <<
            "\nCreate a table of distinct candidate attribute values." << endl;

        const char *HUGE_INT =
            "11111111111111111111111111111111111111111111111111111111111111111"
            "1111111111111111111111111111111111111.0";

        const char *HUGE_FRAC =
            "0."
            "11111111111111111111111111111111111111111111111111111111111111111"
            "1111111111111111111111111111111111111";

        const char *HUGE_EXP = "1e11111111111111111111111111111111111111111111"
                               "111111111111111111111"
                               "1111111111111111111111111111111111111";

        if (verbose)
            bsl::cout << "\tTest with hand created test data"
                      << bsl::endl;
        struct {
            const int   d_line;
            const bool  d_isValid;
            const char *d_text_p;
        } DATA[] = {
            // LINE IS_VALID TEXT
            // ---- -------- --------------
            {  L_,  true ,               "0" }
          , {  L_,  true ,              "-0" }
          , {  L_,  true ,              "10" }
          , {  L_,  true ,             "-10" }
          , {  L_,  true ,      "1234567890" }
          , {  L_,  true ,     "-1234567890" }
          , {  L_,  true ,             "0e0" }
          , {  L_,  true ,             "1e0" }
          , {  L_,  true ,             "1e1" }
          , {  L_,  true ,           "1.2e1" }
          , {  L_,  true ,            "1e-1" }
          , {  L_,  true ,          "1.2e-1" }
          , {  L_,  true ,          "1.2e+1" }
          , {  L_,  true ,      "1.2e+00001" }
          , {  L_,  true ,      "1.2e-00001" }
          , {  L_,  true ,       "1.2e00001" }
          , {  L_,  true ,             "0.0" }
          , {  L_,  true ,         HUGE_INT  }
          , {  L_,  true ,         HUGE_FRAC }
          , {  L_,  true ,         HUGE_EXP  }
          , {  L_,  false,              "1e" }
          , {  L_,  false,            "1.2e" }
          , {  L_,  false,              "0-" }
          , {  L_,  false,              "0-" }
          , {  L_,  false,              "00" }
          , {  L_,  false,              "01" }
          , {  L_,  false,              "0." }
          , {  L_,  false,              ".0" }
          , {  L_,  false,         "0.0e++1" }
          , {  L_,  false,         "0.0e+-1" }
          , {  L_,  false,         "0.0e--1" }
          , {  L_,  false,          "0.0ee1" }
          , {  L_,  false,        "++0.0ee1" }
          , {  L_,  false,            "0..0" }
          , {  L_,  false,              "+1" }
          , {  L_,  false,              " 1" }
          , {  L_,  false,              "1 " }
          , {  L_,  false,              "0-" }
          , {  L_,  false,         "1a0.0e1" }
          , {  L_,  false,         "100.ae1" }
          , {  L_,  false,        "100.0e1a" }
        };

        const bsl::size_t NUM_DATA = sizeof DATA / sizeof *DATA;

        for (bsl::size_t i = 0; i < NUM_DATA; ++i) {
            const int   LINE     = DATA[i].d_line;
            const bool  IS_VALID = DATA[i].d_isValid;
            const char *TEXT     = DATA[i].d_text_p;

            if (veryVerbose) {
                T_ P_(LINE) P_(IS_VALID) P(TEXT)
            }

            const bool rc       = Obj::isValidNumber(TEXT);             // TEST
            const bool rcOracle = NU ::isValidNumber(TEXT);

            ASSERTV(LINE, TEXT, rc, IS_VALID, IS_VALID == rc);
            ASSERTV(LINE, TEXT, rc, IS_VALID, IS_VALID == rcOracle);
        }

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // NUM. VALUE CTORS, ACCS, ASSIGNS, & 'explicit' OPS
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value of the particular
        //:   numeric type.
        //:
        //: 2 The created object uses the intended allocator, if needed.
        //:
        //:   1 There is no use of the default allocator unless that is also
        //:     the supplied allocator.
        //:
        //:   2 All allocation are occur in the 'bsl::string' member.
        //:
        //:     1 Note that in almost every case the textual representation
        //:       of the supported numeric values fits within the short string
        //:       optimization of 'bsl::string'.
        //:
        //: 3 The basic accessors are 'const'-qualified and return attribute
        //:   values matching those specified by the value constructor.
        //:
        //: 4 The accessors of the numeric types return the expected value
        //:   and status when the contained numeric value is outside of the
        //:   expected range.
        //:
        //: 5 Each 'explicit' convertion 'operator' returns the same value as
        //:   the corresponding 'as*' accessor.
        //:
        //: 6 Irrespective of the initial value of an object, each value
        //:   assignment operator returns a non-'const' reference to the object
        //:   under test, and that object contains the assigned value.
        //
        // Plan:
        //: 1 For each of the supported numeric types, replicate the test
        //:   framework (using the "footprint" idiom) used in test case 2.
        //:
        //:   1 To facilitate the replication of test frameworks, helper
        //:     function templates 'testCase11I' (integral types) and
        //:     'testCase11F' (non-integral types) are defined.  Two function
        //:     templates are defined because the accessors for integral and
        //:     non-integral types have different signatures and expected
        //:     behavior.
        //:
        //:   2 The values used test the extremes of the value types under test
        //:     but not not intended to be comprehensive.  The goal is to
        //:     confirm that the intended functions from 'bdljsn::NumberUtil'
        //:     are invoked with the intended arguments.
        //
        //: 2 Define ad hoc tests to show that the accessors return the
        //:   expected value (and status) when the object contains are value
        //:   that cannot be represented by the value under test.
        //:
        //: 3 When 'explicit' operators are defined (for non-integral types),
        //:   mirror each use of an accessor with the analogous use of the
        //:   'explicit' operator.
        //:
        //: 4 For a table of distinct values that include the default and
        //:   extremes of the type being tested, use each value as the initial
        //:   value and each value as the assigned value (i.e., a cross product
        //:   of the table).  Confirm that the return value and final state of
        //:   the object match the expected results.
        //:
        //:   o Note that the implementation of this test assumes that the
        //:     numeric value constructor works correctly.  That assumption was
        //:     established in P-1 and P-2.
        //
        // Testing:
        //   JsonNumber(int                 value, *bA =0);
        //   JsonNumber(unsigned int        value, *bA = 0);
        //   JsonNumber(bsls::Types::Int64  value, *bA = 0);
        //   JsonNumber(bsls::Types::Uint64 value, *bA = 0);
        //   JsonNumber(float               value, *bA = 0);
        //   JsonNumber(double              value, *bA = 0);
        //   JsonNumber(bdldfp::Decimal64   value, *bA = 0);
        //   JsonNumber& operator=(int                 rhs);
        //   JsonNumber& operator=(unsigned int        rhs);
        //   JsonNumber& operator=(bsls::Types::Int64  rhs);
        //   JsonNumber& operator=(bsls::Types::Uint64 rhs);
        //   JsonNumber& operator=(float             rhs);
        //   JsonNumber& operator=(double            rhs);
        //   JsonNumber& operator=(bdldfp::Decimal64 rhs);
        //   int asInt   (int                 *result) const;
        //   int asInt64 (bsls::Types::Int64  *result) const;
        //   int asUint  (unsigned int        *result) const;
        //   int asUint64(bsls::Types::Uint64 *result) const;
        //   float              asFloat()     const;
        //   double             asDouble()    const;
        //   bdldfp::Decimal64  asDecimal64() const;
        //   explicit operator float()             const;
        //   explicit operator double()            const;
        //   explicit operator bdldfp::Decimal64() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                << "NUM. VALUE CTORS, ACCS, ASSIGNS, & 'explicit' OPS" << endl
                << "=================================================" << endl;
        }

        testCase11I<                int>(argc, argv, &Obj::asInt);
        testCase11I<bsls::Types:: Int64>(argc, argv, &Obj::asInt64);
        testCase11I<       unsigned int>(argc, argv, &Obj::asUint);
        testCase11I<bsls::Types::Uint64>(argc, argv, &Obj::asUint64);

        testCase11F<              float>(argc, argv, &Obj::asFloat);
        testCase11F<             double>(argc, argv, &Obj::asDouble);
        testCase11F<  bdldfp::Decimal64>(argc, argv, &Obj::asDecimal64);

      } break;
      case 10: {
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
        //:     7 Use the 'get_allocator' accessor of both 'mX' and 'mF' to
        //:       verify that the respective allocators used by the target and
        //:       source objects are unchanged.  (C-2, 7)
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
        //:     8 Use the 'get_allocator' accessor of both 'mX' and 'mF' to
        //:       verify that the respective allocators used by the target and
        //:       source objects are unchanged.  (C-2, 7)
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
        //:   7 Use the 'get_allocator' accessor of 'mX' to verify that it is
        //:     still the object allocator.
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
        //   JsonNumber& operator=(MovableRef<JsonNumber> rhs);
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

        if (verbose) {
            cout <<
           "\nUse a table of distinct object values and expected memory usage."
                                                                       << endl;
        }

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                    // this test allocates
                                                    // some object memory.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1   = DATA[ti].d_line;
            const char        MEMSRC1 = DATA[ti].d_mem;
            const char *const TEXT1   = DATA[ti].d_text_p;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj Z (TEXT1, &scratch);
            const Obj ZZ(TEXT1, &scratch);

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
                const int         LINE2 = DATA[tj].d_line;
                const char *const TEXT2 = DATA[tj].d_text_p;

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);

                {
                    // Test move assignment with same allocator.

                    Obj mF(TEXT1, &s1); const Obj& F=mF;
                    Obj mX(TEXT2, &s1); const Obj& X=mX;

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
                const int         LINE2   = DATA[tj].d_line;
                const char        MEMDST2 = DATA[tj].d_mem;
                const char *const TEXT2   = DATA[tj].d_text_p;

                bslma::TestAllocator s1("scratch1", veryVeryVeryVerbose);
                bslma::TestAllocator s2("scratch2", veryVeryVeryVerbose);

                {
                    // Test move assignment with different allocator

                    Obj mF(TEXT1, &s1); const Obj& F=mF;
                    Obj mX(TEXT2, &s2); const Obj& X=mX;

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

                Obj mX(TEXT1, &oa); const Obj ZZ(TEXT1, &scratch);

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
        //:     6 Use the 'get_allocator' accessor of both 'mX' and 'Z' to
        //:       verify that the respective allocators used by the target and
        //:       source objects are unchanged.  (C-2, 7)
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
        //:   7 Use the 'get_allocator' accessor of 'mX' to verify that it is
        //:     still the object allocator.
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
        //   JsonNumber& operator=(const JsonNumber& rhs);
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
            const int         LINE1   = DATA[ti].d_line;
            const char        MEMSRC1 = DATA[ti].d_mem;
            const char *const TEXT1   = DATA[ti].d_text_p;

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj Z (TEXT1, &scratch);
            const Obj ZZ(TEXT1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                ASSERTV(LINE1, Obj(), Z, Obj() == Z);
                firstFlag = false;
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2   = DATA[tj].d_line;
                const char        MEMDST2 = DATA[tj].d_mem;
                const char *const TEXT2   = DATA[tj].d_text_p;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(TEXT2, &oa);  const Obj& X = mX;

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

                Obj mX(TEXT1, &oa); const Obj ZZ(TEXT1, &scratch);

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
      case 8: {
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
        //   void swap(JsonNumber& other);
        //   void swap(JsonNumber& a, JsonNumber& b);
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
            const int         LINE1 = DATA[ti].d_line;
            const char        MEM1  = DATA[ti].d_mem;
            const char *const TEXT1 = DATA[ti].d_text_p;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mW(TEXT1, &oa);  const Obj& W = mW; const Obj XX(W, &scratch);

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
                const int         LINE2 = DATA[tj].d_line;
                const char *const TEXT2 = DATA[tj].d_text_p;

                Obj mX(XX,    &oa);  const Obj& X = mX;
                Obj mY(TEXT2, &oa);  const Obj& Y = mY; const Obj YY(Y,
                                                                     &scratch);

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

            const SV A1 = LONG_STRING;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mX(    &oa);  const Obj& X = mX; const Obj XX(X, &scratch);
            Obj mY(A1, &oa);  const Obj& Y = mY; const Obj YY(Y, &scratch);

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
            const int         LINE1 = DATA[ti].d_line;
            const char *const TEXT1 = DATA[ti].d_text_p;

            bslma::TestAllocator      oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator     oa2("object2", veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const Obj XX(TEXT1, &scratch);

            if (veryVerbose) { T_ P_(LINE1) P(XX) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2 = DATA[tj].d_line;
                const char *const TEXT2 = DATA[tj].d_text_p;

                Obj mX(XX,    &oa);  const Obj& X = mX;
                Obj mY(TEXT2, &oa2); const Obj& Y = mY; const Obj YY(Y,
                                                                     &scratch);

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
      case 7: {
        // --------------------------------------------------------------------
        // MOVE FROM STRING CONSTRUCTOR
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
        //
        //:10 QoI: Asserted precondition violations are detected when enabled.
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
        //
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //
        // Testing:
        //   JsonNumber(MovableRef<bsl::string> text);
        //   JsonNumber(MovableRef<bsl::string> text, *bA = 0);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "MOVE FROM STRING CONSTRUCTOR" << endl
                 << "============================" << endl;
        }

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
                const int         LINE = DATA[ti].d_line;
                const char        MEM  = DATA[ti].d_mem;
                const char *const TEXT = DATA[ti].d_text_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj Z (TEXT, &scratch);
                const Obj ZZ(TEXT, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator s1("supplied1", veryVeryVeryVerbose);
                    bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    bsl::string *fromPtr = new (fa) bsl::string(TEXT, &s1);

                    bsl::string &mF = *fromPtr; const bsl::string& F = mF;

                    bsls::Types::Int64 s1Alloc = s1.numBytesInUse();

                    Obj                  *objPtr          = 0;
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

                        ASSERTV(LINE, CONFIG, Z, F, Z.value() == F);

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
                            &oa == X.value().get_allocator());

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

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            bsl::string ngAvecA("");
            bsl::string ngSansA("");

            ASSERT_FAIL(Obj(bslmf::MovableRefUtil::move(ngSansA)));
            ASSERT_FAIL(Obj(bslmf::MovableRefUtil::move(ngAvecA), &sa));

            bsl::string okAvecA("0");
            bsl::string okSansA("0");

            ASSERT_PASS(Obj(bslmf::MovableRefUtil::move(okSansA)));
            ASSERT_PASS(Obj(bslmf::MovableRefUtil::move(okAvecA), &sa));
        }

      } break;
      case 6: {
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
        //
        // Testing:
        //   JsonNumber(MovableRef<JsonNumber> original);
        //   JsonNumber(MovableRef<JsonNumber> original, *bA);
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
                const int         LINE = DATA[ti].d_line;
                const char        MEM  = DATA[ti].d_mem;
                const char *const TEXT = DATA[ti].d_text_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj Z (TEXT, &scratch);
                const Obj ZZ(TEXT, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'd'; ++cfg) {
                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator s1("supplied1", veryVeryVeryVerbose);
                    bslma::TestAllocator s2("supplied2", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj *fromPtr = new (fa) Obj(TEXT, &s1);

                    Obj &mF = *fromPtr; const Obj& F = mF;

                    bsls::Types::Int64 s1Alloc = s1.numBytesInUse();

                    Obj                  *objPtr          = 0;
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
                            &oa == X.value().get_allocator());

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

      } break;
      case 5: {
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
        //:       allocator is properly installed; also use the 'get_allocator'
        //:       accessor of 'X' to verify that its object allocator is
        //:       properly installed, and use the 'get_allocator' accessor of
        //:       'Z' to verify that the allocator that it uses is unchanged.
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
        //
        // Testing:
        //   JsonNumber(const JsonNumber&      original, *bA = 0);
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
                const int         LINE = DATA[ti].d_line;
                const char        MEM  = DATA[ti].d_mem;
                const char *const TEXT = DATA[ti].d_text_p;

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj Z (TEXT, &scratch);
                const Obj ZZ(TEXT, &scratch);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    Obj                  *objPtr          = 0;
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
                            &oa == X.value().get_allocator());

                    // Also invoke the object's 'get_allocator' accessor, as
                    // well as that of 'Z'.

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
      } break;
      case 4: {
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
        //   bool operator==(const JsonNumber& lhs, const JsonNumber& rhs);
        //   bool operator!=(const JsonNumber& lhs, const JsonNumber& rhs);
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

        typedef const char *T1;                // 'value'

        // Attribute 1 Values: 'value', both allocating

        const T1 A1 = SUFFICIENTLY_LONG_STRING;
        const T1 B1 = SUFFICIENTLY_LONG_STRING SUFFICIENTLY_LONG_STRING;

        if (verbose) cout <<
            "\nCreate a table of distinct, but similar object values." << endl;

        static const struct {
            int         d_line;           // source line number
            const char *d_value_p;
        } DATA[] = {

        // The first row of the table below represents an object value
        // consisting of "baseline" attribute values (A1..An).  Each subsequent
        // row differs (slightly) from the first in exactly one attribute
        // value (Bi).

        //LINE  VALUE
        //----  -----

        { L_,      A1 },          // baseline
        { L_,      B1 }

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nCompare every value with every value." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE1 = DATA[ti].d_line;
            const char *const VAL1  = DATA[ti].d_value_p;

            if (veryVerbose) { T_ P_(LINE1) P(VAL1) }

            // Ensure an object compares correctly with itself (alias test).
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                const Obj X(VAL1, &scratch);

                ASSERTV(LINE1, X,   X == X);
                ASSERTV(LINE1, X, !(X != X));
            }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int         LINE2 = DATA[tj].d_line;
                const char *const VAL2  = DATA[tj].d_value_p;

                if (veryVerbose) {
                               T_ T_ P_(LINE2) P(VAL2) }

                const bool EXP = ti == tj;  // expected for equality comparison

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // Determines 'Y's allocator.

                    // Create two distinct test allocators, 'oax' and 'oay'.

                    bslma::TestAllocator oax("objectx", veryVeryVeryVerbose);
                    bslma::TestAllocator oay("objecty", veryVeryVeryVerbose);

                    // Map allocators above to objects 'X' and 'Y' below.

                    bslma::TestAllocator& xa = oax;
                    bslma::TestAllocator& ya = 'a' == CONFIG ? oax : oay;

                    const Obj X(VAL1, &xa);
                    const Obj Y(VAL2, &ya);

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
      case 3: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method and the output 'operator<<' have standard
        //:   signatures and return types.
        //
        //: 2 The 'print' method and the output 'operator<<' return the
        //:   supplied 'ostream'.
        //:
        //: 3 The 'print' method writes the object's value in decimal format to
        //:   the specified 'ostream' at the intended indentation and followed
        //:   by '\n' unless the third argument ('spacesPerLevel') is negative.
        //:
        //: 4 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 5 There is no output when the stream is invalid.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-1)
        //:
        //: 2 Using the table-driven technique: (C-2..5)
        //:
        //:   1 Create a table having combinations of the two formatting
        //:     parameters, 'level' and 'spacesPerLevel', and the expected
        //:     output.
        //:
        //:   2 The formatting parameter '-9' and '-8' are "magic".  They are
        //:     never used as arguments.  Rather they direct flow of the test
        //:     case to code that confirms that the default argument values are
        //:     correct and confirms that 'operator<<' works as expected.
        //:
        //:   3 Each table entry is used twice: First with a valid output
        //:     stream and then again with the output stream in a 'bad' state.
        //:     In the former case, the output should match that specified in
        //:     in the table.  In the later case, there should be no output.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream& stream, const JsonNumber& object);
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

        if (veryVerbose) cout <<
             "\nCreate a table of distinct format combinations." << endl;

#define NL "\n"
#define SP " "
#define TEST_DFLT -8
#define TEST_OPER -9
        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;

            const char *d_value_p;

            const char *d_expected_p;
        } DATA[] = {

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE  LEVEL      SPL        VALUE     EXPECTED
        //----  --------   ---------  --------  ----------------------------

        { L_,          0,          0,   "89.1", "89.1"                    NL },
        { L_,          0,          1,   "89.1", "89.1"                    NL },
        { L_,          0,         -1,   "89.1", "89.1"                       },
        { L_,          0,  TEST_DFLT,   "89.1", "89.1"                    NL },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE  LEVEL      SPL        VALUE     EXPECTED
        //----  --------   ---------  --------  ------------------------------

        { L_,          3,          0,   "89.1", "89.1"                    NL },
        { L_,          3,          2,   "89.1", "      89.1"              NL },
        { L_,          3,         -2,   "89.1", "      89.1"                 },
        { L_,          3,  TEST_DFLT,   "89.1", "            89.1"        NL },
        { L_,         -3,          0,   "89.1", "89.1"                    NL },
        { L_,         -3,          2,   "89.1", "89.1"                    NL },
        { L_,         -3,         -2,   "89.1", "89.1"                       },
        { L_,         -3,  TEST_DFLT,   "89.1", "89.1"                    NL },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE  LEVEL      SPL        VALUE   EXPECTED
        //----  --------   ---------  ------  ------------------------------

        { L_,          2,          3,   "-7e1", "      -7e1"              NL },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE  LEVEL      SPL        VALUE   EXPECTED
        //----  --------   ---------  ------  ------------------------------

        { L_,  TEST_DFLT,  TEST_DFLT,   "89.1", "89.1"                    NL },
        { L_,  TEST_DFLT,  TEST_DFLT,   "-7e1", "-7e1"                    NL },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE  LEVEL      SPL        VALUE   EXPECTED
        //----  --------   ---------  ------  ------------------------------

        { L_,  TEST_OPER,  TEST_OPER,   "89.1", "89.1"                       },
        { L_,  TEST_OPER,  TEST_OPER,   "-7e1", "-7e1"                       }

        };
#undef SP
#undef NL
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const int         LVL   = DATA[ti].d_level;
                const int         SPL   = DATA[ti].d_spacesPerLevel;

                const char *const VALUE = DATA[ti].d_value_p;
                const char *const EXP   = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(LVL) P(SPL) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                const Obj X(VALUE);

                for (int pass = 1; pass <= 2; ++pass) {

                    bslma::TestAllocator         da("default",
                                                    veryVeryVeryVerbose);
                    bslma::TestAllocatorMonitor  dam(&da);
                    bslma::DefaultAllocatorGuard dag(&da);

                    bslma::TestAllocator printAllocator("print",
                                                         veryVeryVeryVerbose);
                    bsl::ostringstream   os(&printAllocator);

                    if (2 == pass) {
                        os.setstate(bsl::ios::badbit);
                    }

                    // Verify supplied stream is returned by reference.

                    if (TEST_OPER == LVL && TEST_OPER == SPL) {
                        ASSERTV(LINE, pass, &os == &(os << X));         // TEST

                        if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                    }
                    else {
                        ASSERTV(LINE, pass, TEST_DFLT == SPL
                                         || TEST_DFLT != LVL);

                        if (TEST_DFLT != SPL) {
                            ASSERTV(LINE, pass, &os == &X.print(os, LVL, SPL));
                                                                        // TEST
                        }
                        else if (TEST_DFLT != LVL) {
                            ASSERTV(LINE, pass, &os == &X.print(os, LVL));
                                                                        // TEST
                        }
                        else {
                            ASSERTV(LINE, pass, &os == &X.print(os));   // TEST
                        }

                        ASSERT(dam.isInUseSame());
#undef TEST_OPER
#undef TEST_DFLT
                        if (veryVeryVerbose) { T_ T_ Q(print) }
                    }

                    // Verify output is formatted as expected.

                    if (veryVeryVerbose) { P(os.str()) }

                    if (2 == pass) {  // Writing to a "bad" stream.
                        ASSERTV(LINE, EXP, os.str(), ""  == os.str());
                    } else {          // Should match table entry.
                        ASSERTV(LINE, EXP, os.str(), EXP == os.str());
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // VALUE CTOR, BASIC ACCESSOR, DEFAULT CTOR, AND DTOR
        //   Ensure that we can use the value constructor to create an object
        //   having any state relevant for thorough testing, and use the
        //   destructor to destroy it safely.
        //
        // Concerns:
        //: 1 The value constructor (with or without a supplied allocator) can
        //:   create an object having any value that does not violate the
        //:   constructor's documented preconditions.
        //:
        //: 2 The created object uses the intended allocator, if needed.
        //:
        //:   1 There is no use of the default allocator unless that is also
        //:     the supplied allocator.
        //:
        //:   2 All allocation are occur in the 'bsl::string' member.
        //:
        //:   3 Every object releases any allocated memory at destruction.
        //:
        //: 3 The basic accessors are 'const'-qualified and return attribute
        //:   values matching those specified by the value constructor.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the "footprint" idiom to create test objects using the three
        //:   different formulations of the value constructor: no specified
        //:   allocator (default allocator), allocator defined by 0 (also
        //:   default allocator), and an explicit, user-supplied allocator.
        //:   Use test allocators to:
        //:
        //:   1 Confirm the that expected allocator is installed and used.
        //:   2 Confirm that none of the other allocators are used.
        //:   3 Confirm that allocations occur only for values that
        //:     exceed the short-string-optimization (SSO) limit.
        //:
        //: 2 Create objects based on a table of representative values,
        //:   including some that are exceed the short-string-optimization
        //:   limit.
        //:
        //: 3 Confirm that the 'allocator' and 'value' accessors show the
        //:   values expected from the value constructor.  Demonstrate that
        //:   they are 'const'-qualified by invoking them on 'const'-references
        //:   to the test objects.
        //:
        //: 4 Confirm (by test allocator) that all allocated memory is
        //:   recovered on destruction.
        //:
        //: 5 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //
        // Testing:
        //   JsonNumber();
        //   JsonNumber(bslma::Allocator *basicAllocator);
        //   JsonNumber(const char *text, *bA = 0);
        //   JsonNumber(const bsl::string_view& text, *bA= 0);
        //   ~JsonNumber();
        //   const bsl::string& value() const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
               << "VALUE CTOR, BASIC ACCESSOR, DEFAULT CTOR, AND DTOR" << endl
               << "==================================================" << endl;

        if (verbose) cout << "\nTest Default Constructor" << endl;
        {
            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) { P(CONFIG); }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr          = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj;
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
                    BSLS_ASSERT_OPT(!"reachable");
                  } break;
                }

                Obj&                   mX = *objPtr;  const Obj& X = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                ASSERTV(CONFIG, &oa == X.value().get_allocator().mechanism());

                // Also invoke the object's 'get_allocator' accessor.

                ASSERTV(CONFIG, &oa, ALLOC_OF(X), &oa == X.allocator());

                ASSERTV(CONFIG,  oa.numBlocksTotal(),
                        0 ==  oa.numBlocksTotal());

                ASSERTV(CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                ASSERTV(CONFIG, "0",   X.value(),
                                "0" == X.value());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
                ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
                ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
            }
        }

        if (verbose) cout << "\nTest Value Constructor" << endl;

        const int              NUM_DATA        = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        for (bsl::size_t ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE = DATA[ti].d_line;
            const char        MEM  = DATA[ti].d_mem;
            const char *const TEXT = DATA[ti].d_text_p;

            if (veryVerbose) {
                T_ P_(LINE) P_(MEM) P(TEXT)
            }

            for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                if (veryVerbose) { P(CONFIG); }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj                  *objPtr          = 0;
                bslma::TestAllocator *objAllocatorPtr = 0;

                switch (CONFIG) {
                  case 'a': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(TEXT);
                  } break;
                  case 'b': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(TEXT, 0);
                  } break;
                  case 'c': {
                    objAllocatorPtr = &sa;
                    objPtr = new (fa) Obj(TEXT, objAllocatorPtr);
                  } break;
                  case 'd': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(SV(TEXT));
                  } break;
                  case 'e': {
                    objAllocatorPtr = &da;
                    objPtr = new (fa) Obj(SV(TEXT), 0);
                  } break;
                  case 'f': {
                    objAllocatorPtr = &sa;
                    objPtr = new (fa) Obj(SV(TEXT), objAllocatorPtr);
                  } break;
                  default: {
                    BSLS_ASSERT_OPT(!"reachable");
                  } break;
                }

                Obj&                   mX = *objPtr;  const Obj& X = mX;
                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = (&da == &oa) ? sa : da;

                // -------------------------------------------------------
                // Verify any attribute allocators are installed properly.
                // -------------------------------------------------------

                ASSERTV(CONFIG, &oa == X.value().get_allocator().mechanism());

                // Also invoke the object's 'get_allocator' accessor.

                ASSERTV(CONFIG, &oa, ALLOC_OF(X), &oa == X.allocator());

                switch (MEM) {
                  case 'Y': {
                    ASSERTV(CONFIG,  oa.numBlocksTotal(),
                            1 ==  oa.numBlocksTotal());
                  } break;
                  case 'N': {
                    ASSERTV(CONFIG,  oa.numBlocksTotal(),
                            0 ==  oa.numBlocksTotal());
                  } break;
                  default : {
                    BSLS_ASSERT_OPT(!"reachable");
                  } break;
                }
                ASSERTV(CONFIG, noa.numBlocksTotal(),
                        0 == noa.numBlocksTotal());

                ASSERTV(CONFIG, TEXT,   X.value(),
                                TEXT == X.value());

                // Reclaim dynamically allocated object under test.

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.

                ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
                ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
                ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_FAIL(Obj(""));
            ASSERT_PASS(Obj("0"));
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
        //: 1 Ad hoc use of the class under test.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (veryVerbose) cout << "Default CTOR sets expected value" << endl;

        Obj mX; const Obj& X = mX;

        ASSERT(      "0" == X.value());
        ASSERT(       0. == X.asFloat());
        ASSERT(       0. == X.asDouble());
        ASSERT(Deci64(0) == X.asDecimal64());

        int      xInt;
        unsigned xUint;
        Int64    xInt64;
        Uint64   xUint64;

        ASSERT(0 == X.asInt(&xInt));
        ASSERT(0 == X.asUint(&xUint));
        ASSERT(0 == X.asInt64(&xInt64));
        ASSERT(0 == X.asUint64(&xUint64));

        ASSERT(0 == xInt);
        ASSERT(0 == xUint);
        ASSERT(0 == xInt64);
        ASSERT(0 == xUint64);

        if (veryVerbose) cout << "Value CTORs set expected value" << endl;

        Obj mY(                 "0"   ); const Obj& Y = mY;
        Obj mZ(bsl::string_view("0.0")); const Obj& Z = mZ;

        ASSERT(      "0" == Y.value());
        ASSERT(       0. == Y.asFloat());
        ASSERT(       0. == Y.asDouble());
        ASSERT(Deci64(0) == Y.asDecimal64());

        int      yInt;
        unsigned yUint;
        Int64    yInt64;
        Uint64   yUint64;

        ASSERT(0 == Y.asInt(&yInt));
        ASSERT(0 == Y.asUint(&yUint));
        ASSERT(0 == Y.asInt64(&yInt64));
        ASSERT(0 == Y.asUint64(&yUint64));

        ASSERT(0 == yInt);
        ASSERT(0 == yUint);
        ASSERT(0 == yInt64);
        ASSERT(0 == yUint64);

        ASSERT(      "0.0" == Z.value());
        ASSERT(       0.   == Z.asFloat());
        ASSERT(       0.   == Z.asDouble());
        ASSERT(Deci64(0)   == Z.asDecimal64());

        int      zInt;
        unsigned zUint;
        Int64    zInt64;
        Uint64   zUint64;

        ASSERT(0 == Z.asInt(&zInt));
        ASSERT(0 == Z.asUint(&zUint));
        ASSERT(0 == Z.asInt64(&zInt64));
        ASSERT(0 == Z.asUint64(&zUint64));

        ASSERT(0 == zInt);
        ASSERT(0 == zUint);
        ASSERT(0 == zInt64);
        ASSERT(0 == zUint64);

        if (veryVerbose) cout
                        << "Equality per string, not arithmetic value" << endl;
        ASSERT(X == Y);
        ASSERT(X != Z);

        if (veryVerbose) cout << "Test swap" << endl;

        swap(mY, mZ);

        ASSERT(X != Y);
        ASSERT(X == Z);

        if (veryVerbose) cout << "Test assignment" << endl;

        mY = Z;

        ASSERT(X == Y);
        ASSERT(X == Z);

        if (veryVerbose) cout << "Test conversion and exact conversion"
                              << endl;

        Obj mU("1.0"); const Obj& U = mU;
        Obj mV("1.1"); const Obj& V = mV;

        int uInt = 0, vInt = 0;

        ASSERT( U.isIntegral());   ASSERT(!V.isIntegral());
        ASSERT(!U.asInt(&uInt));   ASSERT( V.asInt(&vInt));
        ASSERT( 1 == uInt);        ASSERT( 1 == vInt);
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
