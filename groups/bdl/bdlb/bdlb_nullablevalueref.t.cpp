// bdlb_nullablevalueref.t.cpp                                        -*-C++-*-

#include <bdlb_nullablevalueref.h>
#include <bdlb_nullablevalue.h>           // NullableValue
#include <bdlb_nullableallocatedvalue.h>  // NullableAllocatedValue

#include <bslim_testutil.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_types.h>

#include <bslstl_span.h>
#include <bslstl_optional.h>              // bsl::optional, bsl::nullopt_t

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component implements a wrapper for a value-semantic type, and itself
// exhibits value-semantic properties.
//
// Global Concerns:
//   o No memory is allocated from the global-allocator.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] NullableValueRef(bsl::optional<TYPE> &);
// [ 2] NullableValueRef(NullableValue<TYPE> &);
// [ 2] NullableValueRef(NullableAllocatedValue<TYPE> &);
// [ 2] ConstNullableValueRef(bsl::optional<TYPE> &);
// [ 2] ConstNullableValueRef(NullableValue<TYPE> &);
// [ 2] ConstNullableValueRef(NullableAllocatedValue<TYPE> &);
//
// MANIPULATORS
// [ 3] operator=(const TYPE &);
// [ 3] operator=(const NullableValue &);
// [ 3] operator=(const NullableAllocatedValue &);
// [ 3] operator=(const NullableWrapper &);
// [ 3] operator=(const bsl::nullopt_t &);
// [ 3] void reset();
// [ 4] TYPE &emplace(ARGS args...);
// [ 4] TYPE* operator->();
// [ 4] TYPE &operator*();
// [ 4] void reset();
// [ 4] TYPE &value();
// [ 6] const TYPE *addressOr(const TYPE *address) const;
// [ 6] TYPE& makeValueInplace(ARGS&&... args);
// [ 6] TYPE valueOr(const TYPE& otherValue) const;
// [ 6] const TYPE *valueOrNull() const;
//
// ACCESSORS
// [ 2] bool has_value() const;
// [ 2] const TYPE& value() const;
// [ 2] operator->() const;
// [ 2] operator*() const;
//
// FREE OPERATORS
// [ 5] bool operator==(NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator!=(NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator< (NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator<=(NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator> (NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator>=(NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator==(NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator==(NVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator==(LHS_TYPE&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator!=(NVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator!=(LHS_TYPE&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator< (NVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator< (LHS_TYPE&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator<=(NVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator<=(LHS_TYPE&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator> (NVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator> (LHS_TYPE&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator>=(NVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator>=(LHS_TYPE&, NVWrapper<RHS_TYPE>&);
// [ 5] bool operator==(NVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator==(nullopt_t&, NVWrapper<TYPE>&);
// [ 5] bool operator!=(NVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator!=(nullopt_t&, NVWrapper<TYPE>&);
// [ 5] bool operator< (NVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator< (nullopt_t&, NVWrapper<TYPE>&);
// [ 5] bool operator<=(NVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator<=(nullopt_t&, NVWrapper<TYPE>&);
// [ 5] bool operator> (NVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator> (nullopt_t&, NVWrapper<TYPE>&);
// [ 5] bool operator>=(NVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator>=(nullopt_t&, NVWrapper<TYPE>&);
// [ 5] bool operator==(CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator!=(CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator< (CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator<=(CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator> (CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator>=(CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator==(CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator==(CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator==(LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator!=(CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator!=(LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator< (CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator< (LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator<=(CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator<=(LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator> (CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator> (LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator>=(CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
// [ 5] bool operator>=(LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator==(CNVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator==(nullopt_t&, CNVWrapper<TYPE>&);
// [ 5] bool operator!=(CNVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator!=(nullopt_t&, CNVWrapper<TYPE>&);
// [ 5] bool operator< (CNVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator< (nullopt_t&, CNVWrapper<TYPE>&);
// [ 5] bool operator<=(CNVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator<=(nullopt_t&, CNVWrapper<TYPE>&);
// [ 5] bool operator> (CNVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator> (nullopt_t&, CNVWrapper<TYPE>&);
// [ 5] bool operator>=(CNVWrapper<TYPE>&, nullopt_t&);
// [ 5] bool operator>=(nullopt_t&, CNVWrapper<TYPE>&);
// [ 5] bool operator==(CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
// [ 5] bool operator==(VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator!=(CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
// [ 5] bool operator!=(VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator< (CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
// [ 5] bool operator< (VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator<=(CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
// [ 5] bool operator<=(VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator> (CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
// [ 5] bool operator> (VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// [ 5] bool operator>=(CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
// [ 5] bool operator>=(VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE

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

//=============================================================================
//                  COMPONENT-SPECIFIC MACROS FOR TESTING
//-----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_IBM)                                            \
 ||(defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130)
# define BDLB_FUNCTION_DOES_NOT_DECAY_TO_POINTER_TO_FUNCTION 1
#endif

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));


//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//..
bdlb::NullableValue<int> &getNV()
    // Return a reference to a NullableValue for processing
{
    static bdlb::NullableValue<int> nv(23);
    return nv;
}

bdlb::NullableAllocatedValue<int> &getNAV()
    // Return a reference to a NullableAllocatedValue for processing
{
    static bdlb::NullableAllocatedValue<int> nav(34);
    return nav;
}
//..

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

struct SimpleStruct {
    int d_value;

    SimpleStruct()
        // Construct an object
    : d_value(100) {}

    SimpleStruct(int i)
        // Construct an object from the specified 'i'
    : d_value(i) {}

    int getValue()
        // return the value that the object was constructed with.
    {
        return d_value;
    }
    int getValue() const
        // Return a value signaling that this was called with a const object.
    {
        return -1;
    }
};


template <class TYPE, bool IS_CONST, bool IS_NAV>
void comparisonTest(bsl::span<TYPE> values)
    // Test the comparison operators for ConstNullableValueRef<TYPE>.  Uses
    // the specified 'values' as test cases.
{
    typedef typename bsl::remove_const<TYPE>::type  ValueType;
    typedef typename bsl::conditional<IS_NAV,
                              typename bdlb::NullableAllocatedValue<ValueType>,
                              typename bsl::optional<ValueType>
                              >::type Obj;

    typedef typename bsl::conditional<IS_CONST,
                           typename bdlb::ConstNullableValueRef<ValueType>,
                           typename bdlb::NullableValueRef<ValueType>
                           >::type Wrapper;

    // Comparison between two non-null values.
    for (size_t ii = 0; ii < values.size(); ++ii) {
        const ValueType RU = values[ii];
        Obj             OU(RU);
        Wrapper         WU(OU);
        ASSERT( WU.has_value());

        if (veryVerbose) { T_ P_(ii) P(RU) }

        for (size_t jj = 0; jj < values.size(); ++jj) {
            const ValueType RV = values[jj];
            Obj             OV(RV);
            Wrapper         WV(OV);
            ASSERT( WV.has_value());

            if (veryVeryVerbose) { T_ T_ P_(jj) P(values[jj]) }

            // lhs is nullable, but non-null, RV is raw value
            ASSERTV(ii, jj, WU.value(), RV, (ii == jj) == (WU == RV));
            ASSERTV(ii, jj, WU.value(), RV, (ii != jj) == (WU != RV));

            ASSERTV(ii, jj, WU.value(), RV, (ii <  jj) == (WU <  RV));
            ASSERTV(ii, jj, WU.value(), RV, (ii <= jj) == (WU <= RV));
            ASSERTV(ii, jj, WU.value(), RV, (ii >  jj) == (WU >  RV));
            ASSERTV(ii, jj, WU.value(), RV, (ii >= jj) == (WU >= RV));

            // lhs is raw value, rhs is nullable, but non-null
            ASSERTV(ii, jj, RU, WV.value(), (ii == jj) == (RU == WV));
            ASSERTV(ii, jj, RU, WV.value(), (ii != jj) == (RU != WV));

            ASSERTV(ii, jj, RU, WV.value(), (ii <  jj) == (RU <  WV));
            ASSERTV(ii, jj, RU, WV.value(), (ii <= jj) == (RU <= WV));
            ASSERTV(ii, jj, RU, WV.value(), (ii >  jj) == (RU >  WV));
            ASSERTV(ii, jj, RU, WV.value(), (ii >= jj) == (RU >= WV));

            // both sides are nullable, but non-null
            ASSERTV(ii, jj, WU.value(), WV.value(), (ii == jj) == (WU == WV));
            ASSERTV(ii, jj, WU.value(), WV.value(), (ii != jj) == (WU != WV));

            ASSERTV(ii, jj, WU.value(), WV.value(), (ii <  jj) == (WU <  WV));
            ASSERTV(ii, jj, WU.value(), WV.value(), (ii <= jj) == (WU <= WV));
            ASSERTV(ii, jj, WU.value(), WV.value(), (ii >  jj) == (WU >  WV));
            ASSERTV(ii, jj, WU.value(), WV.value(), (ii >= jj) == (WU >= WV));
        }
    }

    // Comparison between null nullable wrapper and raw value.
    {
        Obj     N0;
        Wrapper W0(N0);
        ASSERT(!W0.has_value());

        if (verbose) cout <<
            "\tComparison between null values and raw values" <<
            endl;

        for (size_t ii = 0; ii < values.size(); ++ii) {
            const ValueType RV = values[ii];    // Raw Value

            ASSERT(!(W0 == RV));
            ASSERT(!(RV == W0));

            ASSERT(  W0 != RV );
            ASSERT(  RV != W0 );

            ASSERT(  W0 <  RV );
            ASSERT(!(RV <  W0));

            ASSERT(  W0 <= RV );
            ASSERT(!(RV <= W0));

            ASSERT(!(W0 >  RV));
            ASSERT(  RV >  W0 );

            ASSERT(!(W0 >= RV));
            ASSERT(  RV >= W0 );
        }
    }

    // Comparison between null nullable wrapper and non-null value.
    {
        Obj     N0;
        Wrapper W0(N0);
        ASSERT(!W0.has_value());

        if (verbose) cout <<
              "\tComparison between null values and non-null values" <<
              endl;

        for (size_t ii = 0; ii < values.size(); ++ii) {
            Obj     NV(values[ii]);
            Wrapper WV(NV);
            ASSERT( WV.has_value());

            if (veryVerbose) { T_ P_(ii) P_(values[ii]) }

            ASSERT(!(W0 == WV));
            ASSERT(!(WV == W0));

            ASSERT(  W0 != WV );
            ASSERT(  WV != W0 );

            ASSERT(  W0 <  WV );
            ASSERT(!(WV <  W0));

            ASSERT(  W0 <= WV );
            ASSERT(!(WV <= W0));

            ASSERT(!(W0 >  WV));
            ASSERT(  WV >  W0 );

            ASSERT(!(W0 >= WV));
            ASSERT(  WV >= W0 );
        }
    }

    // Comparison between two null values.
    {
        Obj     LN, RN;
        Wrapper LW(LN), RW(RN);

        ASSERT(!LW.has_value());
        ASSERT(!RW.has_value());

        ASSERT(  LW == RW );
        ASSERT(!(LW != RW));

        ASSERT(!(LW <  RW));
        ASSERT(  LW <= RW );

        ASSERT(!(LW >  RW));
        ASSERT(  LW >= RW );
    }
}

template <class TYPE>
void mixedComparisonTest(bsl::span<TYPE> values)
    // Test the comparison operators for ConstNullableValueRef<TYPE>.  Uses
    // the specified 'values' as test cases.
{
    typedef typename bsl::remove_const<TYPE>::type  ValueType;
    typedef bdlb::NullableAllocatedValue<ValueType> Obj;
    typedef bdlb::NullableValueRef<ValueType> Wrapper;
    typedef bdlb::ConstNullableValueRef<ValueType> CWrapper;

    // Comparison between two non-null values.
    for (size_t ii = 0; ii < values.size(); ++ii) {
        const ValueType RU = values[ii];
        Obj             OU(RU);
        Wrapper         WU(OU);
        ASSERT( WU.has_value());

        if (veryVerbose) { T_ P_(ii) P(RU) }

        for (size_t jj = 0; jj < values.size(); ++jj) {
            const ValueType RV = values[jj];
            Obj             OV(RV);
            CWrapper        WV(OV);
            ASSERT( WV.has_value());

            if (veryVeryVerbose) { T_ T_ P_(jj) P(values[jj]) }

            // lhs is nullable, but non-null, RV is raw value
            ASSERTV(ii, jj, WU.value(), RV, (ii == jj) == (WU == RV));
            ASSERTV(ii, jj, WU.value(), RV, (ii != jj) == (WU != RV));

            ASSERTV(ii, jj, WU.value(), RV, (ii <  jj) == (WU <  RV));
            ASSERTV(ii, jj, WU.value(), RV, (ii <= jj) == (WU <= RV));
            ASSERTV(ii, jj, WU.value(), RV, (ii >  jj) == (WU >  RV));
            ASSERTV(ii, jj, WU.value(), RV, (ii >= jj) == (WU >= RV));

            // lhs is raw value, rhs is nullable, but non-null
            ASSERTV(ii, jj, RU, WV.value(), (ii == jj) == (RU == WV));
            ASSERTV(ii, jj, RU, WV.value(), (ii != jj) == (RU != WV));

            ASSERTV(ii, jj, RU, WV.value(), (ii <  jj) == (RU <  WV));
            ASSERTV(ii, jj, RU, WV.value(), (ii <= jj) == (RU <= WV));
            ASSERTV(ii, jj, RU, WV.value(), (ii >  jj) == (RU >  WV));
            ASSERTV(ii, jj, RU, WV.value(), (ii >= jj) == (RU >= WV));

            // both sides are nullable, but non-null
            ASSERTV(ii, jj, WU.value(), WV.value(), (ii == jj) == (WU == WV));
            ASSERTV(ii, jj, WU.value(), WV.value(), (ii != jj) == (WU != WV));

            ASSERTV(ii, jj, WU.value(), WV.value(), (ii <  jj) == (WU <  WV));
            ASSERTV(ii, jj, WU.value(), WV.value(), (ii <= jj) == (WU <= WV));
            ASSERTV(ii, jj, WU.value(), WV.value(), (ii >  jj) == (WU >  WV));
            ASSERTV(ii, jj, WU.value(), WV.value(), (ii >= jj) == (WU >= WV));
        }
    }

    // Comparison between null nullable wrapper and non-null value.
    {
        Obj      N0;
        Wrapper  W0(N0);
        CWrapper C0(N0);

        ASSERT(!W0.has_value());
        ASSERT(!C0.has_value());

        if (verbose) cout <<
              "\tComparison between null values and non-null values" <<
              endl;

        for (size_t ii = 0; ii < values.size(); ++ii) {
            Obj      NV(values[ii]);
            Wrapper  WV(NV);
            CWrapper CV(NV);

            ASSERT( WV.has_value());
            ASSERT( CV.has_value());

            if (veryVerbose) { T_ P_(ii) P_(values[ii]) }

            ASSERT(!(W0 == CV));
            ASSERT(!(CV == W0));
            ASSERT(!(C0 == WV));
            ASSERT(!(WV == C0));

            ASSERT(  W0 != CV );
            ASSERT(  CV != W0 );
            ASSERT(  C0 != WV );
            ASSERT(  WV != C0 );

            ASSERT(  W0 <  CV );
            ASSERT(!(CV <  W0));
            ASSERT(  C0 <  WV );
            ASSERT(!(WV <  C0));

            ASSERT(  W0 <= CV );
            ASSERT(!(CV <= W0));
            ASSERT(  C0 <= WV );
            ASSERT(!(WV <= C0));

            ASSERT(!(W0 >  CV));
            ASSERT(  CV >  W0 );
            ASSERT(!(C0 >  WV));
            ASSERT(  WV >  C0 );

            ASSERT(!(W0 >= CV));
            ASSERT(  CV >= W0 );
            ASSERT(!(C0 >= WV));
            ASSERT(  WV >= C0 );
        }
    }

    // Comparison between two null values.
    {
        Obj      LN, RN;
        Wrapper  WR(LN);
        CWrapper CW(RN);

        ASSERT(!WR.has_value());
        ASSERT(!CW.has_value());

        ASSERT(  WR == CW );
        ASSERT(  CW == WR );

        ASSERT(!(WR != CW));
        ASSERT(!(CW != WR));

        ASSERT(!(WR <  CW));
        ASSERT(!(CW <  WR));

        ASSERT(  WR <= CW );
        ASSERT(  CW <= WR );

        ASSERT(!(WR >  CW));
        ASSERT(!(CW >  WR));

        ASSERT(  WR >= CW );
        ASSERT(  CW >= WR );
    }
}


// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int            test = argc > 1 ? atoi(argv[1]) : 0;
                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
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

        // Suppose we have a data structure that contains optional components
        // that are implemented with 'NullableValue', and client code that uses
        // them.  Now we want to change the data structure to use
        // 'NullableAllocatedValue', but without requiring simultaneous changes
        // to the client code.  If the client code uses 'NullableValueRef' to
        // access the optional values, the change will not require any source
        // changes in the clients; a recompile is sufficient.
        //
        // Given the following functions:
        //..
        // bdlb::NullableValue<int> &getNV()
        //     // Return a reference to a NullableValue for processing
        // {
        //     static bdlb::NullableValue<int> nv(23);
        //     return nv;
        // }
        //
        // bdlb::NullableAllocatedValue<int> &getNAV()
        //     // Return a reference to a NullableAllocatedValue for processing
        // {
        //     static bdlb::NullableAllocatedValue<int> nav(34);
        //     return nav;
        // }
        //..
        //  We can wrap both of these types into a wrapper, and then tread them
        //  indentically.
        //..
        bdlb::NullableValueRef<int> w1(getNV());
        bdlb::NullableValueRef<int> w2(getNAV());

        ASSERT(23 == w1.value());
        ASSERT(34 == w2.value());
        //..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // DEPRECATED FUNCTIONS BROUGHT OVER FROM 'NullableValue'
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
        //   const TYPE *addressOr(const TYPE *address) const;
        //   TYPE& makeValueInplace(ARGS&&... args);
        //   TYPE valueOr(const TYPE& otherValue) const;
        //   const TYPE *valueOrNull() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
           << "DEPRECATED FUNCTIONS BROUGHT OVER FROM 'NullableValue'" << endl
           << "======================================================" << endl;

        int                                  v = 123;
        bsl::optional<int>                   op0;
        bsl::optional<int>                   op1(32);
        bdlb::NullableValue<int>             nv0;
        bdlb::NullableValue<int>             nv1(33);
        bdlb::NullableAllocatedValue<int>    av0;
        bdlb::NullableAllocatedValue<int>    av1(34);
        bdlb::NullableValueRef<int>          nw0(op0);
        bdlb::ConstNullableValueRef<int>     nw1(op1);
        bdlb::NullableValueRef<int>          nw2(nv0);
        bdlb::ConstNullableValueRef<int>     nw3(nv1);
        bdlb::NullableValueRef<int>          nw4(av0);
        bdlb::ConstNullableValueRef<int>     nw5(av1);

        ASSERT(!nw0.has_value());
        ASSERT(nw0.addressOr(&v) == &v);
        ASSERT(nw0.valueOr(v)    == v);
        ASSERT(nw0.valueOrNull() == NULL);

        ASSERT( nw1.has_value());
        ASSERT(nw1.addressOr(&v) != &v);
        ASSERT(nw1.valueOr(v)    == 32);
        ASSERT(nw1.valueOrNull() != NULL);

        ASSERT(!nw2.has_value());
        ASSERT(nw2.addressOr(&v) == &v);
        ASSERT(nw2.valueOr(v)    == v);
        ASSERT(nw2.valueOrNull() == NULL);

        ASSERT( nw3.has_value());
        ASSERT(nw3.addressOr(&v) != &v);
        ASSERT(nw3.valueOr(v)    == 33);
        ASSERT(nw3.valueOrNull() != NULL);

        ASSERT(!nw4.has_value());
        ASSERT(nw4.addressOr(&v) == &v);
        ASSERT(nw4.valueOr(v)    == v);
        ASSERT(nw4.valueOrNull() == NULL);

        ASSERT( nw5.has_value());
        ASSERT(nw5.addressOr(&v) != &v);
        ASSERT(nw5.valueOr(v)    == 34);
        ASSERT(nw5.valueOrNull() != NULL);

        nw0.makeValueInplace(4);
        ASSERT( nw0.has_value());
        ASSERTV(nw0.value(), 4 == nw0);

        nw2.makeValueInplace(5);
        ASSERT( nw2.has_value());
        ASSERTV(nw2.value(), 5 == nw2);

        nw4.makeValueInplace(6);
        ASSERT( nw4.has_value());
        ASSERTV(nw4.value(), 6 == nw4);

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY AND RELATIONAL OPERATORS
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if their
        //:   corresponding values compare equal.
        //:
        //: 2 'true  == (X == X)' (i.e., identity).
        //:
        //: 3 'false == (X != X)' (i.e., identity).
        //:
        //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity).
        //:
        //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity).
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'.
        //:
        //: 7 Null nullable object does not compare equal to any other
        //:   non-nullable object.
        //:
        //: 8 An object 'X' is in relation to an object 'Y' as the value of 'X'
        //:   is in relation to the value of 'Y'.
        //:
        //: 9 'false == (X <  X)' (i.e., irreflexivity).
        //:
        //:10 'true  == (X <= X)' (i.e., reflexivity).
        //:
        //:11 'false == (X >  X)' (i.e., irreflexivity).
        //:
        //:12 'true  == (X >= X)' (i.e., reflexivity).
        //:
        //:13 If 'X < Y', then '!(Y < X)' (i.e., asymmetry).
        //:
        //:14 'X <= Y' if and only if 'X < Y' exclusive-or 'X == Y'.
        //:
        //:15 If 'X > Y', then '!(Y > X)' (i.e., asymmetry).
        //:
        //:16 'X >= Y' if and only if 'X > Y' exclusive-or 'X == Y'.
        //:
        //:17 Non-modifiable objects can be compared (i.e., objects or
        //:   providing only non-modifiable access).
        //:
        //
        // Plan:
        //: 1 Create a set of values and test each of the wrapper/target
        //: combinations against this set of values.
        //
        // Testing:
        //   bool operator==(NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
        //   bool operator!=(NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
        //   bool operator< (NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
        //   bool operator<=(NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
        //   bool operator> (NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
        //   bool operator>=(NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
        //   bool operator==(NVWrapper<LHS_TYPE>&, NVWrapper<RHS_TYPE>&);
        //   bool operator==(NVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator==(LHS_TYPE&, NVWrapper<RHS_TYPE>&);
        //   bool operator!=(NVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator!=(LHS_TYPE&, NVWrapper<RHS_TYPE>&);
        //   bool operator< (NVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator< (LHS_TYPE&, NVWrapper<RHS_TYPE>&);
        //   bool operator<=(NVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator<=(LHS_TYPE&, NVWrapper<RHS_TYPE>&);
        //   bool operator> (NVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator> (LHS_TYPE&, NVWrapper<RHS_TYPE>&);
        //   bool operator>=(NVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator>=(LHS_TYPE&, NVWrapper<RHS_TYPE>&);
        //   bool operator==(NVWrapper<TYPE>&, nullopt_t&);
        //   bool operator==(nullopt_t&, NVWrapper<TYPE>&);
        //   bool operator!=(NVWrapper<TYPE>&, nullopt_t&);
        //   bool operator!=(nullopt_t&, NVWrapper<TYPE>&);
        //   bool operator< (NVWrapper<TYPE>&, nullopt_t&);
        //   bool operator< (nullopt_t&, NVWrapper<TYPE>&);
        //   bool operator<=(NVWrapper<TYPE>&, nullopt_t&);
        //   bool operator<=(nullopt_t&, NVWrapper<TYPE>&);
        //   bool operator> (NVWrapper<TYPE>&, nullopt_t&);
        //   bool operator> (nullopt_t&, NVWrapper<TYPE>&);
        //   bool operator>=(NVWrapper<TYPE>&, nullopt_t&);
        //   bool operator>=(nullopt_t&, NVWrapper<TYPE>&);
        //   bool operator==(CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator!=(CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator< (CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator<=(CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator> (CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator>=(CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator==(CNVWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator==(CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator==(LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
        //   bool operator!=(CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator!=(LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
        //   bool operator< (CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator< (LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
        //   bool operator<=(CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator<=(LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
        //   bool operator> (CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator> (LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
        //   bool operator>=(CNVWrapper<LHS_TYPE>&, RHS_TYPE&);
        //   bool operator>=(LHS_TYPE&, CNVWrapper<RHS_TYPE>&);
        //   bool operator==(CNVWrapper<TYPE>&, nullopt_t&);
        //   bool operator==(nullopt_t&, CNVWrapper<TYPE>&);
        //   bool operator!=(CNVWrapper<TYPE>&, nullopt_t&);
        //   bool operator!=(nullopt_t&, CNVWrapper<TYPE>&);
        //   bool operator< (CNVWrapper<TYPE>&, nullopt_t&);
        //   bool operator< (nullopt_t&, CNVWrapper<TYPE>&);
        //   bool operator<=(CNVWrapper<TYPE>&, nullopt_t&);
        //   bool operator<=(nullopt_t&, CNVWrapper<TYPE>&);
        //   bool operator> (CNVWrapper<TYPE>&, nullopt_t&);
        //   bool operator> (nullopt_t&, CNVWrapper<TYPE>&);
        //   bool operator>=(CNVWrapper<TYPE>&, nullopt_t&);
        //   bool operator>=(nullopt_t&, CNVWrapper<TYPE>&);
        //   bool operator==(CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
        //   bool operator==(VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator!=(CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
        //   bool operator!=(VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator< (CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
        //   bool operator< (VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator<=(CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
        //   bool operator<=(VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator> (CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
        //   bool operator> (VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        //   bool operator>=(CNVWrapper<LHS_TYPE>&, VWrapper<RHS_TYPE>&);
        //   bool operator>=(VWrapper<LHS_TYPE>&, CNVWrapper<RHS_TYPE>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING EQUALITY AND RELATIONAL OPERATORS"
                          << "\n========================================="
                          << endl;

        typedef int ValueType;

        const ValueType values[] =
                             { INT_MIN, -1000, -123, 0, +123, +1000, INT_MAX };
        enum { k_NUM_VALUES = sizeof values / sizeof *values };
        bsl::span<const ValueType> sp(values);

        comparisonTest<const ValueType, false, false>(sp);
        comparisonTest<const ValueType, false, true> (sp);
        comparisonTest<const ValueType, true,  false>(sp);
        comparisonTest<const ValueType, true,  true> (sp);
        mixedComparisonTest(sp);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS
        //
        // Concerns:
        //:  1 The manipulators should change the state of the targeted
        //:  nullable object, and those changes should be reflected in the
        //:  wrapper object.
        //
        // Plan:
        //:  1 Create an empty object and a wrapper targeting it.  Use
        //:  'emplace' to put a value into it, and then the other manipulators
        //:  to modify the value.  Finally, use 'reset' to remove the value.
        //
        // Testing:
        //   TYPE &emplace(ARGS args...);
        //   TYPE* operator->();
        //   TYPE &operator*();
        //   void reset();
        //   TYPE &value();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING MANIPULATORS"
                          << "\n===================="
                          << endl;


        if (veryVerbose)
            cout << "Testing Manipulators on optional" << endl;
        {
            bsl::optional<SimpleStruct>                   op;
            bdlb::NullableValueRef<SimpleStruct>          nw(op);

            ASSERT(!nw.has_value());

            nw.emplace(34);
            ASSERT( op.has_value());
            ASSERT( nw.has_value());
            ASSERT(34 == op.value().d_value);
            ASSERT(34 == nw.value().d_value);

            nw->d_value = 45;
            ASSERT( op.has_value());
            ASSERT( nw.has_value());
            ASSERT(45 == op.value().d_value);
            ASSERT(45 == nw.value().d_value);

            (*nw).d_value = 56;
            ASSERT( op.has_value());
            ASSERT( nw.has_value());
            ASSERT(56 == op.value().d_value);
            ASSERT(56 == nw.value().d_value);

            nw.value().d_value = 67;
            ASSERT( op.has_value());
            ASSERT( nw.has_value());
            ASSERT(67 == op.value().d_value);
            ASSERT(67 == op.value().d_value);

            nw.reset();
            ASSERT(!op.has_value());
            ASSERT(!nw.has_value());
        }

        if (veryVerbose)
            cout << "Testing Manipulators on NullableValue" << endl;
        {
            bdlb::NullableValue<SimpleStruct>             nv;
            bdlb::NullableValueRef<SimpleStruct>          nw(nv);

            ASSERT(!nw.has_value());

            nw.emplace(34);
            ASSERT( nv.has_value());
            ASSERT( nw.has_value());
            ASSERT(34 == nv.value().d_value);
            ASSERT(34 == nw.value().d_value);

            nw->d_value = 45;
            ASSERT( nv.has_value());
            ASSERT( nw.has_value());
            ASSERT(45 == nv.value().d_value);
            ASSERT(45 == nw.value().d_value);

            (*nw).d_value = 56;
            ASSERT( nv.has_value());
            ASSERT( nw.has_value());
            ASSERT(56 == nv.value().d_value);
            ASSERT(56 == nw.value().d_value);

            nw.value().d_value = 67;
            ASSERT( nv.has_value());
            ASSERT( nw.has_value());
            ASSERT(67 == nv.value().d_value);
            ASSERT(67 == nw.value().d_value);

            nw.reset();
            ASSERT(!nv.has_value());
            ASSERT(!nw.has_value());
        }


        if (veryVerbose)
            cout << "Testing Manipulators on NullableAllocatedValue" << endl;
        {
            bdlb::NullableAllocatedValue<SimpleStruct>    av;
            bdlb::NullableValueRef<SimpleStruct>          nw(av);

            ASSERT(!nw.has_value());

            nw.emplace(34);
            ASSERT( av.has_value());
            ASSERT( nw.has_value());
            ASSERT(34 == av.value().d_value);
            ASSERT(34 == nw.value().d_value);

            nw->d_value = 45;
            ASSERT( av.has_value());
            ASSERT( nw.has_value());
            ASSERT(45 == av.value().d_value);
            ASSERT(45 == nw.value().d_value);

            (*nw).d_value = 56;
            ASSERT( av.has_value());
            ASSERT( nw.has_value());
            ASSERT(56 == av.value().d_value);
            ASSERT(56 == nw.value().d_value);

            nw.value().d_value = 67;
            ASSERT( av.has_value());
            ASSERT( nw.has_value());
            ASSERT(67 == av.value().d_value);
            ASSERT(67 == nw.value().d_value);

            nw.reset();
            ASSERT(!av.has_value());
            ASSERT(!nw.has_value());
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATORS
        //
        // Concerns:
        //: 1 That assigning a value to a wrapper updates the targeted nullable
        //: object.
        //: 2 That assigning a value to a wrapper whose target is empty "fills"
        //: the target.
        //: 3 That assigning a nullopt_t (or calling 'reset') to a wrapper
        //  whose target is not empty "empties" the target.
        //: 4 That assigning a nullopt_t (or calling 'reset') to a wrapper
        //   whose target is empty is a no-op.
        //
        // Plan:
        //:  1 Create a set of four wrappers; two targeting NullableValue and
        //:  the other two targeting NullableAllocatedValue.  Assign various
        //:  values (nullopt_t, TYPE, NullableValue, NullableAllocatedValue)
        //:  to the wrappers, and verify that the wrapper and the targeted
        //:  object are updated as expected.
        //
        // Testing:
        //   operator=(const TYPE &);
        //   operator=(const NullableValue &);
        //   operator=(const NullableAllocatedValue &);
        //   operator=(const NullableWrapper &);
        //   operator=(const bsl::nullopt_t &);
        //   void reset();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING ASSIGNMENT OPERATORS"
                          << "\n============================"
                          << endl;

        bdlb::NullableValue<int>                nv0;
        bdlb::NullableValue<int>                nv1(33);
        bdlb::NullableAllocatedValue<int>       av0;
        bdlb::NullableAllocatedValue<int>       av1(34);
        bdlb::NullableValueRef<int>             nw0(nv0);
        bdlb::NullableValueRef<int>             nw1(nv1);
        bdlb::NullableValueRef<int>             nw2(av0);
        bdlb::NullableValueRef<int>             nw3(av1);
        const bdlb::NullableValue<int>          nvN(98);
        const bdlb::NullableAllocatedValue<int> avN(99);

        // Assignment to a NullableValueRef holding a NullableValue

        // Assignment from a TYPE &
        ASSERT( nw1.has_value());
        nw1 = 35;
        ASSERT( nw1.has_value());
        ASSERT(35 == nw1.value());
        ASSERT( nv1.has_value());
        ASSERT(35 == nv1.value());

        // Assignment from a NullableValue
        ASSERT( nw1.has_value());
        ASSERT(35 == nw1.value());
        nw1 = nvN;
        ASSERT( nw1.has_value());
        ASSERT(nvN.value() == nw1.value());
        ASSERT( nv1.has_value());
        ASSERT(nvN.value() == nv1.value());

        // Assignment from an empty NullableValue
        ASSERT(!nw0.has_value());
        ASSERT(!nv0.has_value());
        nw1 = nv0;
        ASSERT(!nw1.has_value());
        ASSERT(!nv1.has_value());

        // Assignment from a NullableAllocatedValue
        ASSERT(!nw1.has_value());
        nw1 = avN;
        ASSERT( nw1.has_value());
        ASSERT(avN.value() == nw1.value());
        ASSERT( nv1.has_value());
        ASSERT(avN.value() == nv1.value());

        // Assignment from an empty NullableAllocatedValue
        ASSERT( nw1.has_value());
        ASSERT(!av0.has_value());
        nw1 = av0;
        ASSERT(!nv1.has_value());
        ASSERT(!nw1.has_value());

        // Assignment from a NullableValueRef holding a NullableValue
        nv1 = 36;
        ASSERT( nw1.has_value());
        nw1 = nw0;
        ASSERT(!nw1.has_value());
        ASSERT(!nv1.has_value());

        // Assignment from a NullableValueRef holding a NullableAllocatedValue
        av1 = 37;
        ASSERT( nw3.has_value());
        ASSERT(37 == nw3.value());
        nw1 = nw3;
        ASSERT( nw1.has_value());
        ASSERT(37 == nw1.value());
        ASSERT( nv1.has_value());
        ASSERT(37 == nv1.value());

        // Assignment from an empty NullableValueRef
        ASSERT( nw1.has_value());
        ASSERT(37 == nw1.value());
        ASSERT(!nw2.has_value());
        nw1 = nw2;
        ASSERT(!nw1.has_value());

        // Assignment to a NullableValueRef holding a NullableAllocatedValue

        // Assignment from a TYPE &
        ASSERT( nw3.has_value());
        nw3 = 35;
        ASSERT( nw3.has_value());
        ASSERT(35 == nw3.value());
        ASSERT( av1.has_value());
        ASSERT(35 == av1.value());

        // Assignment from a NullableValue
        ASSERT( nw3.has_value());
        ASSERT(35 == nw3.value());
        nw3 = nvN;
        ASSERT( nw3.has_value());
        ASSERT(nvN.value() == nw3.value());
        ASSERT( av1.has_value());
        ASSERT(nvN.value() == av1.value());

        // Assignment from an empty NullableValue
        ASSERT(!nw0.has_value());
        ASSERT(!nv0.has_value());
        nw3 = nv0;
        ASSERT(!nw3.has_value());
        ASSERT(!av1.has_value());

        // Assignment from a NullableAllocatedValue
        ASSERT(!nw3.has_value());
        nw3 = avN;
        ASSERT( nw3.has_value());
        ASSERT(avN.value() == nw3.value());
        ASSERT( av1.has_value());
        ASSERT(avN.value() == av1.value());

        // Assignment from an empty NullableAllocatedValue
        ASSERT( nw3.has_value());
        ASSERT(!av0.has_value());
        nw3 = av0;
        ASSERT(!av1.has_value());
        ASSERT(!nw3.has_value());

        // Assignment from a NullableValueRef holding a NullableValue
        nv1 = 12;
        ASSERT( nw1.has_value());
        ASSERT(12 == nw1.value());
        nw3 = nw1;
        ASSERT( nw3.has_value());
        ASSERT(12 == nw3.value());
        ASSERT( av1.has_value());
        ASSERT(12 == av1.value());

        // Assignment from a NullableValueRef holding a NullableAllocatedValue
        ASSERT(!nw2.has_value());
        nw3 = nw2;
        ASSERT(!nw3.has_value());
        ASSERT(!av1.has_value());

        // Assignment from nullopt_t
        nw0 = bsl::nullopt;
        ASSERT(!nw0.has_value());
        ASSERT(!nv0.has_value());

        nw2 = bsl::nullopt;
        ASSERT(!nw2.has_value());
        ASSERT(!av0.has_value());

        // reset
        nw0.reset();
        ASSERT(!nw0.has_value());
        ASSERT(!nv0.has_value());

        nw2.reset();
        ASSERT(!nw2.has_value());
        ASSERT(!av0.has_value());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CONSTRUCTORS AND ACCESSORS
        //   This test will verify that the primary manipulators are working as
        //   expected.  Also, we test that the basic accessors are working as
        //   expected.
        //
        // Concerns:
        //:  1 The wrapper object should report the same state as the target.
        //:  2 The constructor must not copy the target object.
        //
        // Plan:
        //:  1 Create six objects; two each of 'bsl::optiona', 'NullableValue'
        //:  and 'NullableAllocatedValue'.  Create a 'NullableValueRef' and a
        //:  'ConstNullableValueRef' for each of them.  Verify that the
        //:  contents of the wrappers match the contents of the wrapped
        //:  objects.  Then make changes to the target, and verify that both
        //:  the wrappers and the target objects change.
        //
        // Testing:
        //   NullableValueRef(bsl::optional<TYPE> &);
        //   NullableValueRef(NullableValue<TYPE> &);
        //   NullableValueRef(NullableAllocatedValue<TYPE> &);
        //   ConstNullableValueRef(bsl::optional<TYPE> &);
        //   ConstNullableValueRef(NullableValue<TYPE> &);
        //   ConstNullableValueRef(NullableAllocatedValue<TYPE> &);
        //   bool has_value() const;
        //   const TYPE& value() const;
        //   operator->() const;
        //   operator*() const;
        // --------------------------------------------------------------------

        if (verbose) cout <<
                           "\nTESTING CONSTRUCTORS AND ACCESSORS"
                           "\n=================================="
                          << endl;

        bsl::optional<int>                   op0;
        bsl::optional<int>                   op1(12);
        bdlb::NullableValue<int>             nv0;
        bdlb::NullableValue<int>             nv1(13);
        bdlb::NullableAllocatedValue<int>    av0;
        bdlb::NullableAllocatedValue<int>    av1(14);
        bdlb::NullableValueRef<int>          nw0(op0);
        bdlb::NullableValueRef<int>          nw1(op1);
        bdlb::NullableValueRef<int>          nw2(nv0);
        bdlb::NullableValueRef<int>          nw3(nv1);
        bdlb::NullableValueRef<int>          nw4(av0);
        bdlb::NullableValueRef<int>          nw5(av1);
        bdlb::ConstNullableValueRef<int>     cw0(op0);
        bdlb::ConstNullableValueRef<int>     cw1(op1);
        bdlb::ConstNullableValueRef<int>     cw2(nv0);
        bdlb::ConstNullableValueRef<int>     cw3(nv1);
        bdlb::ConstNullableValueRef<int>     cw4(av0);
        bdlb::ConstNullableValueRef<int>     cw5(av1);

        ASSERT(!nw0.has_value());
        ASSERT( nw1.has_value());
        ASSERT(!nw2.has_value());
        ASSERT( nw3.has_value());
        ASSERT(!nw4.has_value());
        ASSERT( nw5.has_value());

        ASSERT(12 == nw1.value());
        ASSERT(13 == nw3.value());
        ASSERT(14 == nw5.value());

        op0 = -1;
        ASSERT( op0.has_value());
        ASSERT( nw0.has_value());
        ASSERT(-1 == op0.value());
        ASSERT(-1 == nw0.value());

        nv0 = -2;
        ASSERT( nv0.has_value());
        ASSERT( nw2.has_value());
        ASSERT(-2 == nv0.value());
        ASSERT(-2 == nw2.value());

        av0 = -3;
        ASSERT( av0.has_value());
        ASSERT( nw4.has_value());
        ASSERT(-3 == av0.value());
        ASSERT(-3 == nw4.value());

        bsl::optional<SimpleStruct>                   opC(22);
        bdlb::NullableValue<SimpleStruct>             nvC(23);
        bdlb::NullableAllocatedValue<SimpleStruct>    avC(24);
        bdlb::NullableValueRef<SimpleStruct>          nwC0(opC);
        bdlb::NullableValueRef<SimpleStruct>          nwC1(nvC);
        bdlb::NullableValueRef<SimpleStruct>          nwC2(avC);
        bdlb::ConstNullableValueRef<SimpleStruct>     cwC0(opC);
        bdlb::ConstNullableValueRef<SimpleStruct>     cwC1(nvC);
        bdlb::ConstNullableValueRef<SimpleStruct>     cwC2(avC);

        ASSERT(22 == nwC0->getValue());   // calls non-const method
        ASSERT(23 == nwC1->getValue());   // calls non-const method
        ASSERT(24 == nwC2->getValue());   // calls non-const method
        ASSERT(-1 == cwC0->getValue());   // calls     const method
        ASSERT(-1 == cwC0->getValue());   // calls     const method
        ASSERT(-1 == cwC0->getValue());   // calls     const method

        ASSERT(22 == (*nwC0).getValue());   // calls non-const method
        ASSERT(23 == (*nwC1).getValue());   // calls non-const method
        ASSERT(24 == (*nwC2).getValue());   // calls non-const method

        ASSERT(-1 == (*cwC0).getValue());   // calls     const method
        ASSERT(-1 == (*cwC1).getValue());   // calls     const method
        ASSERT(-1 == (*cwC2).getValue());   // calls     const method

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //:  1 We want to demonstrate a base-line level of correct operation of
        //:  the following methods and operators:
        //:    - construction from NullableValue and NullableAllocatedValue
        //:    - basic accessors: 'value' and 'has_value'.
        //:    - the assignment operator
        //
        // Plan:
        //:  1 Create four objects; two each of 'NullableValue' and
        //:  'NullableAllocatedValue'.  Create a 'NullableValueRef' and a
        //:  'ConstNullableValueRef' for each of them.  Verify that the
        //:  contents of the wrappers match the contents of the wrapped
        //:  Then make changes to the wrappers, and verify that both the
        //:  wrappers and the wrapped objects change.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============" << endl;

        bsl::optional<int>                op0;
        bsl::optional<int>                op1(2);
        bdlb::NullableValue<int>          nv0;
        bdlb::NullableValue<int>          nv1(3);
        bdlb::NullableAllocatedValue<int> av0;
        bdlb::NullableAllocatedValue<int> av1(4);
        bdlb::NullableValueRef<int>       nw0(op0);
        bdlb::NullableValueRef<int>       nw1(op1);
        bdlb::NullableValueRef<int>       nw2(nv0);
        bdlb::NullableValueRef<int>       nw3(nv1);
        bdlb::NullableValueRef<int>       nw4(av0);
        bdlb::NullableValueRef<int>       nw5(av1);
        bdlb::ConstNullableValueRef<int>  cw0(op0);
        bdlb::ConstNullableValueRef<int>  cw1(op1);
        bdlb::ConstNullableValueRef<int>  cw2(nv0);
        bdlb::ConstNullableValueRef<int>  cw3(nv1);
        bdlb::ConstNullableValueRef<int>  cw4(av0);
        bdlb::ConstNullableValueRef<int>  cw5(av1);

        ASSERT(!op0.has_value());
        ASSERT( op1.has_value());
        ASSERT(!nv0.has_value());
        ASSERT( nv1.has_value());
        ASSERT(!av0.has_value());
        ASSERT( av1.has_value());
        ASSERT(op0.has_value() == nw0.has_value());
        ASSERT(op1.has_value() == nw1.has_value());
        ASSERT(nv0.has_value() == nw2.has_value());
        ASSERT(nv1.has_value() == nw3.has_value());
        ASSERT(av0.has_value() == nw4.has_value());
        ASSERT(av1.has_value() == nw5.has_value());
        ASSERT(op0.has_value() == cw0.has_value());
        ASSERT(op1.has_value() == cw1.has_value());
        ASSERT(nv0.has_value() == cw2.has_value());
        ASSERT(nv1.has_value() == cw3.has_value());
        ASSERT(av0.has_value() == nw4.has_value());
        ASSERT(av1.has_value() == nw5.has_value());

        ASSERT(op1.value() == nw1.value());
        ASSERT(nv1.value() == nw3.value());
        ASSERT(av1.value() == nw5.value());
        ASSERT(op1.value() == cw1.value());
        ASSERT(nv1.value() == cw3.value());
        ASSERT(av1.value() == cw5.value());

        nw0 = 42;
        ASSERT( op0.has_value());
        ASSERT( nw0.has_value());
        ASSERT(42 == op0.value());
        ASSERT(42 == nw0.value());
        // Note that even though 'cw0' provides const access, the underlying
        // object can be changed via another method.
        ASSERT( cw0.has_value());
        ASSERT(42 == cw0.value());

        nw2 = 43;
        ASSERT( nv0.has_value());
        ASSERT( nw2.has_value());
        ASSERT(43 == nv0.value());
        ASSERT(43 == nw2.value());
        // Note that even though 'cw2' provides const access, the underlying
        // object can be changed via another method.
        ASSERT( cw2.has_value());
        ASSERT(43 == cw2.value());

        nw4 = 44;
        ASSERT( av0.has_value());
        ASSERT( nw4.has_value());
        ASSERT(44 == av0.value());
        ASSERT(44 == nw4.value());
        // Note that even though 'cw4' provides const access, the underlying
        // object can be changed via another method.
        ASSERT( cw4.has_value());
        ASSERT(44 == cw4.value());

        nw5.reset();
        ASSERT(!nw5.has_value());
        ASSERT(!av1.has_value());
        ASSERT(!cw5.has_value());

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(0 == defaultAllocator.numBlocksInUse());
    ASSERT(0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
