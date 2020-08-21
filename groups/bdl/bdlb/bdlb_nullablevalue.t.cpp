// bdlb_nullablevalue.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlb_nullablevalue.h>

#include <bslalg_constructorproxy.h>

#include <bslh_hash.h>

#include <bslim_testutil.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_matchanytype.h>
#include <bslmf_usesallocator.h>

#include <bsls_asserttest.h>
#include <bsls_nameof.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <bslx_testinstream.h>
#include <bslx_testoutstream.h>

#include <bsl_cstdlib.h>    // 'atoi', 'abs'
#include <bsl_exception.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_typeinfo.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;
using bsls::NameOf;

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
// TYPEDEFS
// [ 3] typedef TYPE ValueType;
//
// CREATORS
// [ 3] NullableValue();
// [ 3] NullableValue(const allocator_type& allocator);
// [ 6] NullableValue(const NullableValue& original);
// [ 6] NullableValue(const NullableValue& original, allocator);
// [20] NullableValue(NullableValue&& original);
// [20] NullableValue(NullableValue&& original, allocator);
// [ 9] NullableValue(const TYPE& value);
// [ 9] NullableValue(const TYPE& value, allocator);
// [22] NullableValue(TYPE&& value);
// [22] NullableValue(TYPE&& value, allocator);
// [11] NullableValue(const OTHER_TYPE& value);
// [11] NullableValue(const OTHER_TYPE& value, allocator);
// [11] NullableValue(const NullableValue<OTHER_TYPE>&o);
// [11] NullableValue(const NullableValue<OTHER_TYPE>&o, allocator);
// [26] NullableValue(const NullOptType&);
// [26] NullableValue(const NullOptType&, allocator);
// [ 3] ~NullableValue();
//
// MANIPULATORS
// [ 7] NullableValue& operator=(const NullableValue& rhs);
// [21] NullableValue& operator=(NullableValue&& rhs);
// [12] NullableValue& operator=(const NullableValue<OTHER_TYPE>& rhs);
// [10] NullableValue& operator=(const TYPE& rhs);
// [23] NullableValue& operator=(TYPE&& rhs);
// [12] NullableValue& operator=(const OTHER_TYPE& rhs);
// [26] NullableValue& operator=(const NullOptType& rhs);
// [13] void swap(NullableValue<TYPE>& other);
// [ 3] TYPE& makeValue(const TYPE& value);
// [12] TYPE& makeValue(const OTHER_TYPE& value);
// [ 3] TYPE& makeValue();
// [19] TYPE& makeValueInplace(ARGS&&... args);
// [ 8] STREAM& bdexStreamIn(STREAM& stream, int version);
// [10] void reset();
// [10] TYPE& value();
//
// ACCESSORS
// [25] const TYPE *addressOr(const TYPE *address) const;
// [ 8] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 3] bool isNull() const;
// [ 8] int maxSupportedBdexVersion(int) const;
// [ 4] ostream& print(ostream& s, int l=0, int spl=4) const;
// [ 3] const TYPE& value() const;
// [14] TYPE valueOr(const TYPE& value) const;
// [15] const TYPE *valueOr(const TYPE *value) const;
// [16] const TYPE *valueOrNull() const;
//
// FREE OPERATORS
// [ 5] bool operator==(const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
// [ 5] bool operator!=(const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
// [ 5] bool operator< (const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
// [ 5] bool operator<=(const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
// [ 5] bool operator> (const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
// [ 5] bool operator>=(const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
// [ 5] bool operator==(const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
// [ 5] bool operator!=(const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
// [ 5] bool operator< (const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
// [ 5] bool operator<=(const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
// [ 5] bool operator> (const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
// [ 5] bool operator>=(const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
// [27] bool operator==(const NullableValue<LHS_TYPE>&, NullOptType);
// [27] bool operator!=(const NullableValue<LHS_TYPE>&, NullOptType);
// [27] bool operator< (const NullableValue<LHS_TYPE>&, NullOptType);
// [27] bool operator<=(const NullableValue<LHS_TYPE>&, NullOptType);
// [27] bool operator>=(const NullableValue<LHS_TYPE>&, NullOptType);
// [27] bool operator> (const NullableValue<LHS_TYPE>&, NullOptType);
// [ 5] bool operator==(const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
// [ 5] bool operator!=(const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
// [ 5] bool operator< (const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
// [ 5] bool operator<=(const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
// [ 5] bool operator> (const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
// [ 5] bool operator>=(const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
// [27] bool operator==(NullOptType, const NullableValue<LHS_TYPE>&);
// [27] bool operator!=(NullOptType, const NullableValue<LHS_TYPE>&);
// [27] bool operator< (NullOptType, const NullableValue<LHS_TYPE>&);
// [27] bool operator<=(NullOptType, const NullableValue<LHS_TYPE>&);
// [27] bool operator>=(NullOptType, const NullableValue<LHS_TYPE>&);
// [27] bool operator> (NullOptType, const NullableValue<LHS_TYPE>&);
// [ 4] ostream& operator<<(ostream&, const NullableValue<TYPE>&);
//
// FREE FUNCTIONS
// [20] void hashAppend(HASHALG& hashAlg, NullableValue<TYPE>& input);
// [13] void swap(NullableValue<TYPE>& a, b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST 1: Using 'bsl::string'
// [ 2] BREATHING TEST 2: Using 'int'
// [28] USAGE EXAMPLE
// [24] Concern: Types that are not copy-assignable can be used.
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// [ 8] int maxSupportedBdexVersion() const;
#endif
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

const int   MAX_NUM_PARAMS = 5; // max in simulation of variadic templates

// Define 'bsl::string' value long enough to ensure dynamic memory allocation.

#ifdef BSLS_PLATFORM_CPU_32_BIT
#define SUFFICIENTLY_LONG_STRING "123456789012345678901234567890123"
#else  // 64_BIT
#define SUFFICIENTLY_LONG_STRING "12345678901234567890123456789012" \
                                 "123456789012345678901234567890123"
#endif
BSLMF_ASSERT(sizeof SUFFICIENTLY_LONG_STRING > sizeof(bsl::string));

// NOTE: A bug in the IBM xlC compiler (Version: 12.01.0000.0012) was worked
// around with the following otherwise unnecessary overload added to the
// interface:
//..
//  TYPE& makeValue(const TYPE& value);
//..
// However, it was decided that we would not change the interface to cater to
// xlC and had the client modify their code instead.
//
// The obscure test case (distilled from DRQS 98587609) that demonstrates the
// issue could not be replicated in the test driver because it apparently
// requires two translation units ('paramutil.cpp' and 'client.cpp' below):
//..
//  // paramutil.h
//  namespace ParamUtil {
//      extern const char L_SOME_STRING[];
//  }
//
//  // paramutil.cpp
//  #include <paramutil.h>
//  namespace ParamUtil {
//      const char L_SOME_STRING[] = "L_SOME_STRING";
//  }
//
//  // client.cpp
//  #include <paramutil.h>
//  ...
//      bdlb::NullableValue<bsl::string> mX;
//      mX.makeValue(ParamUtil::L_SOME_STRING);
//  ...
//..

//=============================================================================
//                      GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class FIRST_TYPE, class SECOND_TYPE, class INIT_TYPE>
void testRelationalOperationsNonNull(const INIT_TYPE& lesserVal,
                                     const INIT_TYPE& greaterVal)
    // Test all the relational operations for two types, which objects can be
    // initialized with the specified 'lesserVal' and 'greaterVal' values and
    // compared each other.
{

    if (veryVerbose) {
        bsl::cout << "\t\t       FIRST_TYPE:  "
                  << bsl::setw(50)
                  << bsl::left
                  << NameOf<FIRST_TYPE>().name()
                  << "       SECOND_TYPE:  "
                  << bsl::setw(50)
                  << bsl::left
                  << NameOf< SECOND_TYPE>().name()
                  << bsl::endl;
    }

    FIRST_TYPE  firstLesser(lesserVal);
    FIRST_TYPE  firstGreater(greaterVal);
    SECOND_TYPE secondLesser(lesserVal);
    SECOND_TYPE secondGreater(greaterVal);

    ASSERTV( (firstLesser   == secondLesser ));
    ASSERTV(!(firstLesser   == secondGreater));

    ASSERTV(!(firstLesser   != secondLesser ));
    ASSERTV( (firstLesser   != secondGreater));

    ASSERTV(!(firstLesser   <  secondLesser ));
    ASSERTV( (firstLesser   <  secondGreater));

    ASSERTV( (firstLesser   <= secondLesser ));
    ASSERTV( (firstLesser   <= secondGreater));

    ASSERTV(!(firstLesser   >  secondLesser ));
    ASSERTV(!(firstLesser   >  secondGreater));

    ASSERTV( (firstLesser   >= secondLesser ));
    ASSERTV(!(firstLesser   >= secondGreater));

    ASSERTV(!(firstGreater  == secondLesser ));
    ASSERTV( (firstGreater  == secondGreater));

    ASSERTV( (firstGreater  != secondLesser ));
    ASSERTV(!(firstGreater  != secondGreater));

    ASSERTV(!(firstGreater  <  secondLesser ));
    ASSERTV(!(firstGreater  <  secondGreater));

    ASSERTV(!(firstGreater  <= secondLesser ));
    ASSERTV( (firstGreater  <= secondGreater));

    ASSERTV( (firstGreater  >  secondLesser ));
    ASSERTV(!(firstGreater  >  secondGreater));

    ASSERTV( (firstGreater  >= secondLesser ));
    ASSERTV( (firstGreater  >= secondGreater));

    ASSERTV( (secondLesser  == firstLesser  ));
    ASSERTV(!(secondLesser  == firstGreater ));

    ASSERTV(!(secondLesser  != firstLesser  ));
    ASSERTV( (secondLesser  != firstGreater ));

    ASSERTV(!(secondLesser  <  firstLesser  ));
    ASSERTV( (secondLesser  <  firstGreater ));

    ASSERTV( (secondLesser  <= firstLesser  ));
    ASSERTV( (secondLesser  <= firstGreater ));

    ASSERTV(!(secondLesser  >  firstLesser  ));
    ASSERTV(!(secondLesser  >  firstGreater ));

    ASSERTV( (secondLesser  >= firstLesser  ));
    ASSERTV(!(secondLesser  >= firstGreater ));

    ASSERTV(!(secondGreater == firstLesser  ));
    ASSERTV( (secondGreater == firstGreater ));

    ASSERTV( (secondGreater != firstLesser  ));
    ASSERTV(!(secondGreater != firstGreater ));

    ASSERTV(!(secondGreater <  firstLesser  ));
    ASSERTV(!(secondGreater <  firstGreater ));

    ASSERTV(!(secondGreater <= firstLesser  ));
    ASSERTV( (secondGreater <= firstGreater ));

    ASSERTV( (secondGreater >  firstLesser  ));
    ASSERTV(!(secondGreater >  firstGreater ));

    ASSERTV( (secondGreater >= firstLesser  ));
    ASSERTV( (secondGreater >= firstGreater ));
}


template <class FIRST_NV_TYPE, class SECOND_TYPE, class INIT_TYPE>
void testRelationalOperationsOneNull(const INIT_TYPE& initValue)
    // Test all the relational operations for the null 'bdlb::NullableValue'
    // object and an object of the 'SECOND_TYPE' that is initiated by the
    // specified 'initValue'.  Note that it is expected that the
    // 'FIRST_NV_TYPE' is the 'bdlb::NullableValue' type.

{
    if (veryVerbose) {

        bsl::cout << "\t\t(NULL) FIRST_TYPE:  "
                  << bsl::setw(50)
                  << bsl::left
                  << NameOf<FIRST_NV_TYPE>().name()
                  << "       SECOND_TYPE:  "
                  << bsl::setw(50)
                  << bsl::left
                  << NameOf<SECOND_TYPE>().name()
                  << bsl::endl;
    }

    const FIRST_NV_TYPE firstNullNV;
    const SECOND_TYPE   secondValue(initValue);

    ASSERTV(!(firstNullNV == secondValue));
    ASSERTV( (firstNullNV != secondValue));
    ASSERTV( (firstNullNV <  secondValue));
    ASSERTV( (firstNullNV <= secondValue));
    ASSERTV(!(firstNullNV >  secondValue));
    ASSERTV(!(firstNullNV >= secondValue));

    ASSERTV(!(secondValue == firstNullNV));
    ASSERTV( (secondValue != firstNullNV));
    ASSERTV(!(secondValue <  firstNullNV));
    ASSERTV(!(secondValue <= firstNullNV));
    ASSERTV( (secondValue >  firstNullNV));
    ASSERTV( (secondValue >= firstNullNV));
}

template <class FIRST_NV_TYPE, class SECOND_NV_TYPE>
void testRelationalOperationsBothNull()
    // Test all the relational operations for two null 'bdlb::NullableValue'
    // objects.  Note that it is expected that the 'FIRST_NV_TYPE' and the
    // 'SECOND_NV_TYPE' both are the 'bdlb::NullableValue' type.
{
    if (veryVerbose) {
        bsl::cout << "\t\t(NULL) FIRST_TYPE:  "
                  << bsl::setw(50)
                  << bsl::left
                  << NameOf<FIRST_NV_TYPE>().name()
                  << "(NULL) SECOND_TYPE:  "
                  << bsl::setw(50)
                  << bsl::left
                  << NameOf<SECOND_NV_TYPE>().name()
                  << bsl::endl;
    }

    const  FIRST_NV_TYPE  firstNull;
    const SECOND_NV_TYPE secondNull;


    ASSERTV( (firstNull == secondNull));
    ASSERTV(!(firstNull != secondNull));
    ASSERTV(!(firstNull <  secondNull));
    ASSERTV( (firstNull <= secondNull));
    ASSERTV(!(firstNull >  secondNull));
    ASSERTV( (firstNull >= secondNull));

    ASSERTV( (secondNull == firstNull));
    ASSERTV(!(secondNull != firstNull));
    ASSERTV(!(secondNull <  firstNull));
    ASSERTV( (secondNull <= firstNull));
    ASSERTV(!(secondNull >  firstNull));
    ASSERTV( (secondNull >= firstNull));
}

template <class FIRST_TYPE, class SECOND_TYPE, class INIT_TYPE>
void testRelationalOperations(const INIT_TYPE& lesserVal,
                              const INIT_TYPE& greaterVal)
{
    typedef bdlb::NullableValue< FIRST_TYPE>  FIRST_NV_TYPE;
    typedef bdlb::NullableValue<SECOND_TYPE> SECOND_NV_TYPE;

    testRelationalOperationsNonNull<FIRST_TYPE,    SECOND_NV_TYPE>(lesserVal,
                                                                   greaterVal);
    testRelationalOperationsNonNull<FIRST_NV_TYPE, SECOND_TYPE   >(lesserVal,
                                                                   greaterVal);
    testRelationalOperationsNonNull<FIRST_NV_TYPE, SECOND_NV_TYPE>(lesserVal,
                                                                   greaterVal);

    testRelationalOperationsOneNull<FIRST_NV_TYPE,  SECOND_TYPE   >(lesserVal);
    testRelationalOperationsOneNull<FIRST_NV_TYPE,  SECOND_NV_TYPE>(lesserVal);
    testRelationalOperationsOneNull<SECOND_NV_TYPE, FIRST_TYPE    >(lesserVal);
    testRelationalOperationsOneNull<SECOND_NV_TYPE, FIRST_NV_TYPE >(lesserVal);

    testRelationalOperationsBothNull<FIRST_NV_TYPE, SECOND_NV_TYPE>();
}

// ============================================================================
//                      GLOBAL HELPER CLASSES FOR TESTING
// ----------------------------------------------------------------------------

                        // =============
                        // class TmvipSa
                        // =============

template <class TYPE>
class TmvipSa {
    // Test Make Value In Place Sans (without) Allocator

    // CLASS DATA
    static int s_ctorCalled;
    static int s_dtorCount;

    // DATA
    TYPE       d_a1;
    TYPE       d_a2;
    TYPE       d_a3;
    TYPE       d_a4;
    TYPE       d_a5;

  public:
    // CLASS METHODS
    static void resetCtorCalled();
        // Set a negative value to the static value that is returned by the
        // 'ctorCalled' class method.  Note that a negative values corresponds
        // to none of the constructors of this class.

    static void resetDtorCount();
        // Set to 0 the static value that is incremented when an object of this
        // class is destroyed.

    static int  ctorCalled();
        // Return the value set by by the most recent of the following actions:
        //: o The negative value set by a call to 'resetCtorCalled' class
        //:   method.
        //: o The number of arguments in the most recently called of the
        //:   overloaded value constructors of this class.

    static int  dtorCount();
        // Return the number of objects of this class destroyed since the most
        // recent call to the 'resetDtorCount' class method.

    // CREATORS
             TmvipSa();
    explicit TmvipSa(const TYPE& a1);
             TmvipSa(const TYPE& a1,
                     const TYPE& a2);
             TmvipSa(const TYPE& a1,
                     const TYPE& a2,
                     const TYPE& a3);
             TmvipSa(const TYPE& a1,
                     const TYPE& a2,
                     const TYPE& a3,
                     const TYPE& a4);
             TmvipSa(const TYPE& a1,
                     const TYPE& a2,
                     const TYPE& a3,
                     const TYPE& a4,
                     const TYPE& a5);
        // Create a test object having the default value (i.e., 'TYPE()') for
        // attributes 1 to N, where N is the maximum number of parameters we
        // are supporting in our simulation of C++11 variadic templates.
        // Optionally specify values for attributes 1, 1 and 2, 1 to 3, ..., or
        // 1 to N.  Set the value returned by the 'ctorCalled' class method to
        // the number of parameters in the overload called.

    TmvipSa(const TmvipSa& original);
        // Create an object having the same attribute values as the specified
        // 'original'.

   ~TmvipSa();
        // Destroy this object and increment the value reported by the
        // 'dtorCount()' class method.

    // ACCESSORS
    const TYPE& a1() const;
    const TYPE& a2() const;
    const TYPE& a3() const;
    const TYPE& a4() const;
    const TYPE& a5() const;
        // Return a reference providing non-modifiable access to the 'aN'
        // attribute of this object, where N is the maximum number of
        // parameters we are supporting in our simulation of C++11 variadic
        // templates.
};

                        // -------------
                        // class TmvipSa
                        // -------------
// CLASS DATA
template <class TYPE>
int TmvipSa<TYPE>::s_ctorCalled = -1;

template <class TYPE>
int TmvipSa<TYPE>::s_dtorCount  =  0;

// CLASS METHODS
template <class TYPE>
int TmvipSa<TYPE>::ctorCalled()
{
    return s_ctorCalled;
}

template <class TYPE>
int TmvipSa<TYPE>::dtorCount()
{
    return s_dtorCount;
}

template <class TYPE>
void TmvipSa<TYPE>::resetCtorCalled()
{
    s_ctorCalled = -1;
}

template <class TYPE>
void TmvipSa<TYPE>::resetDtorCount()
{
    s_dtorCount = 0;
}

// CREATORS
template <class TYPE>
TmvipSa<TYPE>::TmvipSa()
: d_a1()
, d_a2()
, d_a3()
, d_a4()
, d_a5()
{
    s_ctorCalled = 0;
}

template <class TYPE>
TmvipSa<TYPE>::TmvipSa(const TYPE& a1)
: d_a1(a1)
, d_a2()
, d_a3()
, d_a4()
, d_a5()
{
    s_ctorCalled = 1;
}

template <class TYPE>
TmvipSa<TYPE>::TmvipSa(const TYPE& a1,
                       const TYPE& a2)
: d_a1(a1)
, d_a2(a2)
, d_a3()
, d_a4()
, d_a5()
{
    s_ctorCalled = 2;
}

template <class TYPE>
TmvipSa<TYPE>::TmvipSa(const TYPE& a1,
                       const TYPE& a2,
                       const TYPE& a3)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4()
, d_a5()
{
    s_ctorCalled = 3;
}

template <class TYPE>
TmvipSa<TYPE>::TmvipSa(const TYPE& a1,
                       const TYPE& a2,
                       const TYPE& a3,
                       const TYPE& a4)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5()
{
    s_ctorCalled = 4;
}

template <class TYPE>
TmvipSa<TYPE>::TmvipSa(const TYPE& a1,
                       const TYPE& a2,
                       const TYPE& a3,
                       const TYPE& a4,
                       const TYPE& a5)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
{
    s_ctorCalled = 5;
}

template <class TYPE>
TmvipSa<TYPE>::TmvipSa(const TmvipSa& original)
: d_a1(original.d_a1)
, d_a2(original.d_a2)
, d_a3(original.d_a3)
, d_a4(original.d_a4)
, d_a5(original.d_a5)
{
}

template <class TYPE>
TmvipSa<TYPE>::~TmvipSa()
{
    ++s_dtorCount;
}

// ACCESSORS
template <class TYPE>
const TYPE& TmvipSa<TYPE>::a1() const
{
    return d_a1;
}

template <class TYPE>
const TYPE& TmvipSa<TYPE>::a2() const
{
    return d_a2;
}

template <class TYPE>
const TYPE& TmvipSa<TYPE>::a3() const
{
    return d_a3;
}

template <class TYPE>
const TYPE& TmvipSa<TYPE>::a4() const
{
    return d_a4;
}

template <class TYPE>
const TYPE& TmvipSa<TYPE>::a5() const
{
    return d_a5;
}

                        // =============
                        // class TmvipAa
                        // =============

template <class TYPE>
class TmvipAa {
    // Test Make Value In Place Avec (with) Allocator

    // CLASS DATA
    static int        s_ctorCalled;
    static int        s_dtorCount;

    // DATA
    TYPE              d_a1;
    TYPE              d_a2;
    TYPE              d_a3;
    TYPE              d_a4;
    TYPE              d_a5;

    TYPE             *d_data_p;       // owned
    bslma::Allocator *d_allocator_p;  // held

    // PRIVATE MANIPULATORS
    void destroyData() const;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TmvipAa,
                                   bslma::UsesBslmaAllocator);
    // CLASS METHODS
    static int ctorCalled();
        // Return the value set by by the most recent of the following actions:
        //: o The negative value set by a call to 'resetCtorCalled' class
        //:   method.
        //: o The number of arguments in the most recently called of the
        //:   overloaded value constructors of this class.

    static int dtorCount();
        // Return the number of objects of this class destroyed since the most
        // recent call to the 'resetDtorCount' class method.

    static void resetCtorCalled();
        // Set a negative value to the static value that is returned by the
        // 'ctorCalled' class method.  Note that a negative value corresponds
        // to none of the constructors of this class.

    static void resetDtorCount();
        // Set to 0 the static value that is incremented when an object of this
        // class is destroyed.

    // CREATORS
    explicit TmvipAa(bslma::Allocator *basicAllocator = 0);
    explicit TmvipAa(const TYPE&       a1,
                     bslma::Allocator *basicAllocator = 0);
             TmvipAa(const TYPE&       a1,
                     const TYPE&       a2,
                     bslma::Allocator *basicAllocator = 0);
             TmvipAa(const TYPE&       a1,
                     const TYPE&       a2,
                     const TYPE&       a3,
                     bslma::Allocator *basicAllocator = 0);
             TmvipAa(const TYPE&       a1,
                     const TYPE&       a2,
                     const TYPE&       a3,
                     const TYPE&       a4,
                     bslma::Allocator *basicAllocator = 0);
             TmvipAa(const TYPE&       a1,
                     const TYPE&       a2,
                     const TYPE&       a3,
                     const TYPE&       a4,
                     const TYPE&       a5,
                     bslma::Allocator *basicAllocator = 0);
        // Create a test object having the default value (i.e., 'TYPE()') for
        // attributes 1 to N, where N is the maximum number of parameters we
        // are supporting in our simulation of C++11 variadic templates.
        // Optionally specify values for attributes 1, 1 and 2, 1 to 3, ..., or
        // 1 to N.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Set the value returned by the 'ctorCalled' class
        // method to the number of parameters -- not counting the optional
        // allocator argument -- in the overload called.  Use 'basicAllocator'
        // to allocate a single 'TYPE()' object so the behavior of
        // 'bdlb::NullableValue' after an exception can be checked for each
        // constructor (including the default constructor).  The allocated
        // object is destroyed by '~TmvipAa()'.

    TmvipAa(const TmvipAa& original, bslma::Allocator *basicAllocator = 0);
        // Create an object having the same attribute values as the specified
        // 'original'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'bbasicAllocator' is 0, the currently installed default
        // allocator is used.

    ~TmvipAa();
        // Destroy this object and increment the value reported by the
        // 'dtorCount' class method and the value reported by the
        // 'TmvipSA::dtorCount' class method (in the base class).

    // ACCESSORS
    const TYPE& a1() const;
    const TYPE& a2() const;
    const TYPE& a3() const;
    const TYPE& a4() const;
    const TYPE& a5() const;
        // Return a reference providing non-modifiable access to the 'aN'
        // attribute of this object, where N is the maximum number of
        // parameters we are supporting in our simulation of C++11 variadic
        // templates.

                        // Aspect

    bslma::Allocator *allocator() const;
        // Return the address of the allocator specified on construction.
};

                        // -------------
                        // class TmvipAa
                        // -------------

// CLASS DATA
template <class TYPE>
int TmvipAa<TYPE>::s_ctorCalled = -3;
template <class TYPE>
int TmvipAa<TYPE>::s_dtorCount  =  0;

// CLASS METHODS
template <class TYPE>
int TmvipAa<TYPE>::ctorCalled()
{
    return s_ctorCalled;
}

template <class TYPE>
int TmvipAa<TYPE>::dtorCount()
{
    return s_dtorCount;
}

template <class TYPE>
void TmvipAa<TYPE>::resetCtorCalled()
{
    s_ctorCalled = -3;
}

template <class TYPE>
void TmvipAa<TYPE>::resetDtorCount()
{
    s_dtorCount = 0;
}

// PRIVATE MANIPULATORS
template <class TYPE>
void TmvipAa<TYPE>::destroyData() const
{
    d_allocator_p->deleteObject(d_data_p);
}

// CREATORS
template <class TYPE>
TmvipAa<TYPE>::TmvipAa(bslma::Allocator *basicAllocator)
: d_a1(basicAllocator)
, d_a2(basicAllocator)
, d_a3(basicAllocator)
, d_a4(basicAllocator)
, d_a5(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p     = new (*d_allocator_p) TYPE(d_allocator_p);
    s_ctorCalled = 0;
}

template <class TYPE>
TmvipAa<TYPE>::TmvipAa(const TYPE&       a1,
                       bslma::Allocator *basicAllocator)
: d_a1(a1, basicAllocator)
, d_a2(basicAllocator)
, d_a3(basicAllocator)
, d_a4(basicAllocator)
, d_a5(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p     = new (*d_allocator_p) TYPE(d_allocator_p);
    s_ctorCalled = 1;
}

template <class TYPE>
TmvipAa<TYPE>::TmvipAa(const TYPE&       a1,
                       const TYPE&       a2,
                       bslma::Allocator *basicAllocator)
: d_a1(a1, basicAllocator)
, d_a2(a2, basicAllocator)
, d_a3(basicAllocator)
, d_a4(basicAllocator)
, d_a5(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p     = new (*d_allocator_p) TYPE(d_allocator_p);
    s_ctorCalled = 2;
}

template <class TYPE>
TmvipAa<TYPE>::TmvipAa(const TYPE&       a1,
                       const TYPE&       a2,
                       const TYPE&       a3,
                       bslma::Allocator *basicAllocator)
: d_a1(a1, basicAllocator)
, d_a2(a2, basicAllocator)
, d_a3(a3, basicAllocator)
, d_a4(basicAllocator)
, d_a5(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p     = new (*d_allocator_p) TYPE(d_allocator_p);
    s_ctorCalled = 3;
}

template <class TYPE>
TmvipAa<TYPE>::TmvipAa(const TYPE&       a1,
                       const TYPE&       a2,
                       const TYPE&       a3,
                       const TYPE&       a4,
                       bslma::Allocator *basicAllocator)
: d_a1(a1, basicAllocator)
, d_a2(a2, basicAllocator)
, d_a3(a3, basicAllocator)
, d_a4(a4, basicAllocator)
, d_a5(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p     = new (*d_allocator_p) TYPE(d_allocator_p);
    s_ctorCalled = 4;
}

template <class TYPE>
TmvipAa<TYPE>::TmvipAa(const TYPE&       a1,
                       const TYPE&       a2,
                       const TYPE&       a3,
                       const TYPE&       a4,
                       const TYPE&       a5,
                       bslma::Allocator *basicAllocator)
: d_a1(a1, basicAllocator)
, d_a2(a2, basicAllocator)
, d_a3(a3, basicAllocator)
, d_a4(a4, basicAllocator)
, d_a5(a5, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p     = new (*d_allocator_p) TYPE(d_allocator_p);
    s_ctorCalled = 5;
}

template <class TYPE>
TmvipAa<TYPE>::TmvipAa(const TmvipAa&    original,
                       bslma::Allocator *basicAllocator)
: d_a1(original.d_a1, basicAllocator)
, d_a2(original.d_a2, basicAllocator)
, d_a3(original.d_a3, basicAllocator)
, d_a4(original.d_a3, basicAllocator)
, d_a5(original.d_a3, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p = new (*d_allocator_p) TYPE(d_allocator_p);
}

template <class TYPE>
TmvipAa<TYPE>::~TmvipAa()
{
    destroyData();
    ++s_dtorCount;
}

// ACCESSORS
template <class TYPE>
const TYPE& TmvipAa<TYPE>::a1() const
{
    return d_a1;
}

template <class TYPE>
const TYPE& TmvipAa<TYPE>::a2() const
{
    return d_a2;
}

template <class TYPE>
const TYPE& TmvipAa<TYPE>::a3() const
{
    return d_a3;
}

template <class TYPE>
const TYPE& TmvipAa<TYPE>::a4() const
{
    return d_a4;
}

template <class TYPE>
const TYPE& TmvipAa<TYPE>::a5() const
{
    return d_a5;
}

template <class TYPE>
bslma::Allocator *TmvipAa<TYPE>::allocator() const
{
    return d_allocator_p;
}


#ifdef BDE_BUILD_TARGET_EXC

                        // ==============================
                        // class TmvipSa_WithThrowingCtor
                        // ==============================

template <class TYPE>
class TmvipSa_WithThrowingCtor {
    // Test Make Value In Place Sans (without) Allocator with Throwing
    // Constructor

    // CLASS DATA
    static int s_ctorCalled;
    static int s_dtorCount;

    // DATA
    TYPE       d_a1;
    TYPE       d_a2;
    TYPE       d_a3;
    TYPE       d_a4;
    TYPE       d_a5;

  public:
    // CLASS METHODS
    static int  ctorCalled();
        // Return the value set by by the most recent of the following actions:
        //: o The negative value set by a call to 'resetCtorCalled' class
        //:   method.
        //: o The number of arguments in the most recently called of the
        //:   overloaded value constructors of this class.

    static int  dtorCount();
        // Return the value the number of objects of this class destroyed since
        // the most recent call to the 'resetDtorCount' class method.

    static void resetCtorCalled();
        // Set a negative value to the static value that is returned by the
        // 'ctorCalled' class method.  Note that a negative values corresponds
        // to none of the constructors of this class.

    static void resetDtorCount();
        // Set to 0 the static value that is incremented when an object of this
        // class is destroyed.

    // CREATORS
             TmvipSa_WithThrowingCtor();
    explicit TmvipSa_WithThrowingCtor(const TYPE& a1);
             TmvipSa_WithThrowingCtor(const TYPE& a1,
                                      const TYPE& a2);
             TmvipSa_WithThrowingCtor(const TYPE& a1,
                                      const TYPE& a2,
                                      const TYPE& a3);
             TmvipSa_WithThrowingCtor(const TYPE& a1,
                                      const TYPE& a2,
                                      const TYPE& a3,
                                      const TYPE& a4);
             TmvipSa_WithThrowingCtor(const TYPE& a1,
                                      const TYPE& a2,
                                      const TYPE& a3,
                                      const TYPE& a4,
                                      const TYPE& a5);
        // Create a test object having the default value (i.e., 'TYPE()') for
        // attributes 1 to N, where N is the maximum number of parameters we
        // are supporting in our simulation of C++11 variadic templates.
        // Optionally specify values for attributes 1, 1 and 2, 1 to 3, ..., or
        // 1 to N.  Set the value returned by the 'ctorCalled' class method to
        // the number of parameters in the overload called *and* throw
        // 'bsl::exception'.

             TmvipSa_WithThrowingCtor(const TYPE& a1,
                                      const TYPE& a2,
                                      const TYPE& a3,
                                      const TYPE& a4,
                                      const TYPE& a5,
                                      const TYPE& a6);
        // Create a test object having the default value (i.e., 'TYPE()') for
        // attributes 1 to N, where N is the maximum number of parameters we
        // are supporting in our simulation of C++11 variadic templates.
        // Optionally specify values for attributes 1, 1 and 2, 1 to 3, ..., or
        // 1 to N.  The value of parameter N+1 is ignored.  Set the value
        // returned by the 'ctorCalled' class method to the number of
        // parameters in the overload called.  Note that this constructor does
        // *not* thrown any exception.

    TmvipSa_WithThrowingCtor(const TmvipSa_WithThrowingCtor& original);
        // Create an object having the same attribute values as the specified
        // 'original'.

   ~TmvipSa_WithThrowingCtor();
        // Destroy this object and increment the value reported by the
        // 'dtorCount()' class method.

    // ACCESSORS
    const TYPE& a1() const;
    const TYPE& a2() const;
    const TYPE& a3() const;
    const TYPE& a4() const;
    const TYPE& a5() const;
        // Return a reference providing non-modifiable access to the 'aN'
        // attribute of this object, where N is the maximum number of
        // parameters we are supporting in our simulation of C++11 variadic
        // templates.
};

                        // ------------------------------
                        // class TmvipSa_WithThrowingCtor
                        // ------------------------------

// CLASS DATA
template <class TYPE>
int TmvipSa_WithThrowingCtor<TYPE>::s_ctorCalled = -2;

template <class TYPE>
int TmvipSa_WithThrowingCtor<TYPE>::s_dtorCount  =  0;

// CLASS METHODS
template <class TYPE>
int TmvipSa_WithThrowingCtor<TYPE>::ctorCalled()
{
    return s_ctorCalled;
}

template <class TYPE>
int TmvipSa_WithThrowingCtor<TYPE>::dtorCount()
{
    return s_dtorCount;
}

template <class TYPE>
void TmvipSa_WithThrowingCtor<TYPE>::resetCtorCalled()
{
    s_ctorCalled = -1;
}

template <class TYPE>
void TmvipSa_WithThrowingCtor<TYPE>::resetDtorCount()
{
    s_dtorCount = 0;
}

// CREATORS
template <class TYPE>
TmvipSa_WithThrowingCtor<TYPE>::TmvipSa_WithThrowingCtor()
: d_a1()
, d_a2()
, d_a3()
, d_a4()
, d_a5()
{
    s_ctorCalled = 0;
    throw bsl::exception();
}

template <class TYPE>
TmvipSa_WithThrowingCtor<TYPE>::TmvipSa_WithThrowingCtor(const TYPE& a1)
: d_a1(a1)
, d_a2()
, d_a3()
, d_a4()
, d_a5()
{
    s_ctorCalled = 1;
    throw bsl::exception();
}

template <class TYPE>
TmvipSa_WithThrowingCtor<TYPE>::TmvipSa_WithThrowingCtor(const TYPE& a1,
                                                         const TYPE& a2)
: d_a1(a1)
, d_a2(a2)
, d_a3()
, d_a4()
, d_a5()
{
    s_ctorCalled = 2;
    throw bsl::exception();
}

template <class TYPE>
TmvipSa_WithThrowingCtor<TYPE>::TmvipSa_WithThrowingCtor(const TYPE& a1,
                                                         const TYPE& a2,
                                                         const TYPE& a3)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4()
, d_a5()
{
    s_ctorCalled = 3;
    throw bsl::exception();
}

template <class TYPE>
TmvipSa_WithThrowingCtor<TYPE>::TmvipSa_WithThrowingCtor(const TYPE& a1,
                                                         const TYPE& a2,
                                                         const TYPE& a3,
                                                         const TYPE& a4)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5()
{
    s_ctorCalled = 4;
    throw bsl::exception();
}

template <class TYPE>
TmvipSa_WithThrowingCtor<TYPE>::TmvipSa_WithThrowingCtor(const TYPE& a1,
                                                         const TYPE& a2,
                                                         const TYPE& a3,
                                                         const TYPE& a4,
                                                         const TYPE& a5)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
{
    s_ctorCalled = 5;
    throw bsl::exception();
}

template <class TYPE>
TmvipSa_WithThrowingCtor<TYPE>::TmvipSa_WithThrowingCtor(const TYPE& a1,
                                                         const TYPE& a2,
                                                         const TYPE& a3,
                                                         const TYPE& a4,
                                                         const TYPE& a5,
                                                         const TYPE& a6)
: d_a1(a1)
, d_a2(a2)
, d_a3(a3)
, d_a4(a4)
, d_a5(a5)
{
    (void) a6;
    s_ctorCalled = 6;
}

template <class TYPE>
TmvipSa_WithThrowingCtor<TYPE>::TmvipSa_WithThrowingCtor(
                                      const TmvipSa_WithThrowingCtor& original)
: d_a1(original.d_a1)
, d_a2(original.d_a2)
, d_a3(original.d_a3)
, d_a4(original.d_a4)
, d_a5(original.d_a5)
{
}

template <class TYPE>
TmvipSa_WithThrowingCtor<TYPE>::~TmvipSa_WithThrowingCtor()
{
    ++s_dtorCount;
}

// ACCESSORS
template <class TYPE>
const TYPE& TmvipSa_WithThrowingCtor<TYPE>::a1() const
{
    return d_a1;
}

template <class TYPE>
const TYPE& TmvipSa_WithThrowingCtor<TYPE>::a2() const
{
    return d_a2;
}

template <class TYPE>
const TYPE& TmvipSa_WithThrowingCtor<TYPE>::a3() const
{
    return d_a3;
}

template <class TYPE>
const TYPE& TmvipSa_WithThrowingCtor<TYPE>::a4() const
{
    return d_a4;
}

template <class TYPE>
const TYPE& TmvipSa_WithThrowingCtor<TYPE>::a5() const
{
    return d_a5;
}

#endif // BDE_BUILD_TARGET_EXC

                        // ================================
                        // class NonAssignableAllocTestType
                        // ================================

class NonAssignableAllocTestType {
    // This unconstrained (value-semantic) attribute class that uses a
    // 'bslma::Allocator' to allocate memory and defines the type trait
    // 'bslma::UsesBslmaAllocator'.  This class is primarily provided to
    // facilitate testing of templates by defining a simple type representative
    // of user-defined types having an allocator.  See the Attributes section
    // under @DESCRIPTION in the component-level documentation for information
    // on the class attributes.

    // DATA
    int              *d_data_p;       // pointer to the data value

    bslma::Allocator *d_allocator_p;  // allocator used to supply memory (held,
                                      // not owned)

    NonAssignableAllocTestType
                     *d_self_p;       // pointer to self (to verify this object
                                      // is not bitwise moved)

  private:
    // NOT IMPLEMENTED
    NonAssignableAllocTestType& operator=(const NonAssignableAllocTestType&);

  public:
    // CREATORS
    explicit NonAssignableAllocTestType(bslma::Allocator *basicAllocator = 0);
        // Create a 'NonAssignableAllocTestType' object having the (default)
        // attribute values:
        //..
        //  data() == 0
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit NonAssignableAllocTestType(int               data,
                                        bslma::Allocator *basicAllocator = 0);
        // Create a 'NonAssignableAllocTestType' object having the specified
        // 'data' attribute value.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    NonAssignableAllocTestType(
                        const NonAssignableAllocTestType&  original,
                        bslma::Allocator                  *basicAllocator = 0);
        // Create a 'NonAssignableAllocTestType' object having the same value
        // as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~NonAssignableAllocTestType();
        // Destroy this object.

    // MANIPULATORS
    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE OPERATORS
bool operator==(const NonAssignableAllocTestType& lhs,
                const NonAssignableAllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'NonAssignableAllocTestType' objects
    // have the same if their 'data' attributes are the same.

bool operator!=(const NonAssignableAllocTestType& lhs,
                const NonAssignableAllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'NonAssignableAllocTestType'
    // objects do not have the same value if their 'data' attributes are not
    // the same.

                        // --------------------------------
                        // class NonAssignableAllocTestType
                        // --------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma warning(disable:4355) // ctor uses 'this' used in member-initializer
#endif

// CREATORS
inline
NonAssignableAllocTestType::NonAssignableAllocTestType(
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
{
    d_data_p  = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = 0;
}

inline
NonAssignableAllocTestType::NonAssignableAllocTestType(
                                              int               data,
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
{
    d_data_p  = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = data;
}

inline
NonAssignableAllocTestType::NonAssignableAllocTestType(
                             const NonAssignableAllocTestType&  original,
                             bslma::Allocator                  *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
{
    d_data_p  = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = *original.d_data_p;
}

inline
NonAssignableAllocTestType::~NonAssignableAllocTestType()
{
    d_allocator_p->deallocate(d_data_p);

    // Ensure that this object has not been bitwise moved.

    BSLS_ASSERT_OPT(this == d_self_p);
}

// MANIPULATORS
inline
void NonAssignableAllocTestType::setData(int value)
{
    *d_data_p = value;
}

// ACCESSORS
inline
int NonAssignableAllocTestType::data() const
{
    return *d_data_p;
}

                                  // Aspects

inline
bslma::Allocator *NonAssignableAllocTestType::allocator() const
{
    return d_allocator_p;
}

// FREE OPERATORS
inline
bool operator==(const NonAssignableAllocTestType& lhs,
                const NonAssignableAllocTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const NonAssignableAllocTestType& lhs,
                const NonAssignableAllocTestType& rhs)
{
    return lhs.data() != rhs.data();
}

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<NonAssignableAllocTestType> : bsl::true_type {
};

}  // close namespace bslma
}  // close enterprise namespace

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
class DeprecatedBdex {
  public:
    static int maxSupportedBdexVersion() {  return 17;  }
};
#endif

                   // ======================================
                   // class ConvertibleFromAllocatorTestType
                   // ======================================

class ConvertibleFromAllocatorTestType {
    // This unconstrained (value-semantic) attribute class uses a
    // 'bslma::Allocator' to supply memory and defines the type trait
    // 'bslma::UsesBslmaAllocator'.  Objects of this class are *implictly*
    // constructible from a 'bslma::Allocator *'.  See DRQS 109738646.

    // DATA
    int              *d_data_p;       // pointer to the data value

    bslma::Allocator *d_allocator_p;  // allocator used to supply memory (held,
                                      // not owned)

  public:
    // CREATORS
    ConvertibleFromAllocatorTestType(
                            bslma::Allocator *basicAllocator = 0);  // IMPLICIT
        // Create a 'ConvertibleFromAllocatorTestTypeAllocTestType' object
        // having the (default) attribute values:
        //..
        //  data() == 0
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Note that 'ConvertibleFromAllocatorTestTypeAllocTestType' is
        // *implicitly* convertible from 'bslma::Allocator *' by design.

    explicit ConvertibleFromAllocatorTestType(
                                         int               data,
                                         bslma::Allocator *basicAllocator = 0);
        // Create a 'ConvertibleFromAllocatorTestType' object having the
        // specified 'data' attribute value.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ConvertibleFromAllocatorTestType(
                  const ConvertibleFromAllocatorTestType&  original,
                  bslma::Allocator                        *basicAllocator = 0);
        // Create a 'ConvertibleFromAllocatorTestType' object having the same
        // value as the specified 'original' object.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    ~ConvertibleFromAllocatorTestType();
        // Destroy this object.

    // MANIPULATORS
    ConvertibleFromAllocatorTestType& operator=(
                                  const ConvertibleFromAllocatorTestType& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.
};

// FREE OPERATORS
bool operator==(const ConvertibleFromAllocatorTestType& lhs,
                const ConvertibleFromAllocatorTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ConvertibleFromAllocatorTestType'
    // objects have the same if their 'data' attributes are the same.

bool operator!=(const ConvertibleFromAllocatorTestType& lhs,
                const ConvertibleFromAllocatorTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two
    // 'ConvertibleFromAllocatorTestType' objects do not have the same value if
    // their 'data' attributes are not the same.

                   // --------------------------------------
                   // class ConvertibleFromAllocatorTestType
                   // --------------------------------------

// CREATORS
inline
ConvertibleFromAllocatorTestType::ConvertibleFromAllocatorTestType(
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p  = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = 0;
}

inline
ConvertibleFromAllocatorTestType::ConvertibleFromAllocatorTestType(
                                              int               data,
                                              bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p  = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = data;
}

inline
ConvertibleFromAllocatorTestType::ConvertibleFromAllocatorTestType(
                       const ConvertibleFromAllocatorTestType&  original,
                       bslma::Allocator                        *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data_p  = reinterpret_cast<int *>(d_allocator_p->allocate(sizeof(int)));
    *d_data_p = *original.d_data_p;
}

inline
ConvertibleFromAllocatorTestType::~ConvertibleFromAllocatorTestType()
{
    d_allocator_p->deallocate(d_data_p);
}

// MANIPULATORS
inline
ConvertibleFromAllocatorTestType&
ConvertibleFromAllocatorTestType::operator=(
                                   const ConvertibleFromAllocatorTestType& rhs)
{
    if (&rhs != this) {
        int *newData = reinterpret_cast<int *>(
                                         d_allocator_p->allocate(sizeof(int)));
        d_allocator_p->deallocate(d_data_p);
        d_data_p  = newData;
        *d_data_p = *rhs.d_data_p;
    }
    return *this;
}

inline
void ConvertibleFromAllocatorTestType::setData(int value)
{
    *d_data_p = value;
}

// ACCESSORS
inline
int ConvertibleFromAllocatorTestType::data() const
{
    return *d_data_p;
}

                                  // Aspects

inline
bslma::Allocator *ConvertibleFromAllocatorTestType::allocator() const
{
    return d_allocator_p;
}

// FREE OPERATORS
inline
bool operator==(const ConvertibleFromAllocatorTestType& lhs,
                const ConvertibleFromAllocatorTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const ConvertibleFromAllocatorTestType& lhs,
                const ConvertibleFromAllocatorTestType& rhs)
{
    return lhs.data() != rhs.data();
}

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<ConvertibleFromAllocatorTestType> : bsl::true_type {

};

}  // close namespace bslma
}  // close enterprise namespace

namespace {

class FirstMethodComparableType;
class SecondMethodComparableType;
class FirstFunctionComparableType;
class SecondFunctionComparableType;

                   // ===============================
                   // class FirstMethodComparableType
                   // ===============================

class FirstMethodComparableType {
  private:
    // DATA
    int d_data;  // object value

  public:
    // CREATORS
    FirstMethodComparableType(int data);                            // IMPLICIT
        // Create a 'FirstMethodComparableType' object having the specified
        // 'data' attribute value.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

    bool operator==(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as "this"
        // object, and 'false' otherwise.

    bool operator==(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as "this"
        // object, and 'false' otherwise.

    bool operator!=(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'rhs' does not have the same value as
        // "this" object, and 'false' otherwise.

    bool operator!=(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'rhs' does not have the same value as
        // "this" object, and 'false' otherwise.

    bool operator<(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered before the specified
        // 'rhs' nullable object, and 'false' otherwise.

    bool operator<(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered before the specified
        // 'rhs' nullable object, and 'false' otherwise.

    bool operator<=(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered before the specified
        // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
        // 'false' otherwise.

    bool operator<=(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered before the specified
        // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
        // 'false' otherwise.

    bool operator>(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered after the specified
        // 'rhs' nullable object, and 'false' otherwise.

    bool operator>(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered after the specified
        // 'rhs' nullable object, and 'false' otherwise.

    bool operator>=(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered after the specified
        // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
        // 'false' otherwise.

    bool operator>=(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered after the specified
        // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
        // 'false' otherwise.
 };

                   // ================================
                   // class SecondMethodComparableType
                   // ================================

class SecondMethodComparableType {
  private:
    // DATA
    int d_data;  // object value

  public:
    // CREATORS
    SecondMethodComparableType(int data);                           // IMPLICIT
        // Create a 'SecondMethodComparableType' object having the specified
        // 'data' attribute value.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.

    bool operator==(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as "this"
        // object, and 'false' otherwise.

    bool operator==(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'rhs' has the same value as "this"
        // object, and 'false' otherwise.

    bool operator!=(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'rhs' does not have the same value as
        // "this" object, and 'false' otherwise.

    bool operator!=(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'rhs' does not have the same value as
        // "this" object, and 'false' otherwise.

    bool operator<(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered before the specified
        // 'rhs' nullable object, and 'false' otherwise.

    bool operator<(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered before the specified
        // 'rhs' nullable object, and 'false' otherwise.

    bool operator<=(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered before the specified
        // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
        // 'false' otherwise.

    bool operator<=(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered before the specified
        // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
        // 'false' otherwise.

    bool operator>(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered after the specified
        // 'rhs' nullable object, and 'false' otherwise.

    bool operator>(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered after the specified
        // 'rhs' nullable object, and 'false' otherwise.

    bool operator>=(const SecondMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered after the specified
        // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
        // 'false' otherwise.

    bool operator>=(const FirstMethodComparableType& rhs) const;
        // Return 'true' if the specified 'lhs' is ordered after the specified
        // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
        // 'false' otherwise.
 };

                   // -------------------------------
                   // class FirstMethodComparableType
                   // -------------------------------
// CREATORS
FirstMethodComparableType::FirstMethodComparableType(int data)
: d_data(data)
{}

// ACCESSORS
int FirstMethodComparableType::data() const
{
    return d_data;
}

bool FirstMethodComparableType::operator==(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data == rhs.data();
}

bool FirstMethodComparableType::operator==(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data == rhs.data();
}

bool FirstMethodComparableType::operator!=(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data != rhs.data();
}

bool FirstMethodComparableType::operator!=(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data != rhs.data();
}

bool FirstMethodComparableType::operator<(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data < rhs.data();
}

bool FirstMethodComparableType::operator<(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data < rhs.data();
}

bool FirstMethodComparableType::operator<=(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data <= rhs.data();
}

bool FirstMethodComparableType::operator<=(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data <= rhs.data();
}

bool FirstMethodComparableType::operator>(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data > rhs.data();
}

bool FirstMethodComparableType::operator>(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data > rhs.data();
}

bool FirstMethodComparableType::operator>=(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data >= rhs.data();
}

bool FirstMethodComparableType::operator>=(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data >= rhs.data();
}


                   // --------------------------------
                   // class SecondMethodComparableType
                   // --------------------------------
// CREATORS
SecondMethodComparableType::SecondMethodComparableType(int data)
: d_data(data)
{}

// ACCESSORS
int SecondMethodComparableType::data() const
{
    return d_data;
}

bool SecondMethodComparableType::operator==(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data == rhs.data();
}

bool SecondMethodComparableType::operator==(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data == rhs.data();
}

bool SecondMethodComparableType::operator!=(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data != rhs.data();
}

bool SecondMethodComparableType::operator!=(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data != rhs.data();
}

bool SecondMethodComparableType::operator<(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data < rhs.data();
}

bool SecondMethodComparableType::operator<(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data < rhs.data();
}

bool SecondMethodComparableType::operator<=(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data <= rhs.data();
}

bool SecondMethodComparableType::operator<=(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data <= rhs.data();
}

bool SecondMethodComparableType::operator>(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data > rhs.data();
}

bool SecondMethodComparableType::operator>(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data > rhs.data();
}

bool SecondMethodComparableType::operator>=(
                                   const SecondMethodComparableType& rhs) const
{
    return d_data >= rhs.data();
}

bool SecondMethodComparableType::operator>=(
                                    const FirstMethodComparableType& rhs) const
{
    return d_data >= rhs.data();
}


                   // =================================
                   // class FirstFunctionComparableType
                   // =================================

class FirstFunctionComparableType {
  private:
    // DATA
    int d_data;  // object value

  public:
    // CREATORS
    FirstFunctionComparableType(int data);                          // IMPLICIT
        // Create a 'FirstFunctionComparableType' object having the specified
        // 'data' attribute value.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
 };

// FREE OPERATORS
bool operator==(const FirstFunctionComparableType&  lhs,
                const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' has the same value as the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator!=(const FirstFunctionComparableType& lhs,
                const FirstFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' does not have the same value as the
    // specified 'rhs' nullable object, and 'false' otherwise.

bool operator==(const FirstFunctionComparableType& lhs,
                const FirstFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' has the same value as the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator!=(const FirstFunctionComparableType&  lhs,
                const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' does not have the same value as the
    // specified 'rhs' nullable object, and 'false' otherwise.

bool operator<(const FirstFunctionComparableType& lhs,
               const FirstFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator<(const FirstFunctionComparableType&  lhs,
               const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator<=(const FirstFunctionComparableType& lhs,
                const FirstFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.

bool operator<=(const FirstFunctionComparableType&  lhs,
                const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.

bool operator>(const FirstFunctionComparableType& lhs,
               const FirstFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator>(const FirstFunctionComparableType&  lhs,
               const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator>=(const FirstFunctionComparableType& lhs,
                const FirstFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.

bool operator>=(const FirstFunctionComparableType&  lhs,
                const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.

                   // ==================================
                   // class SecondFunctionComparableType
                   // ==================================

class SecondFunctionComparableType {
  private:
    // DATA
    int d_data;  // object value

  public:
    // CREATORS
    SecondFunctionComparableType(int data);                         // IMPLICIT
        // Create a 'SecondFunctionComparableType' object having the specified
        // 'data' attribute value.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
 };

// FREE OPERATORS
bool operator==(const SecondFunctionComparableType& lhs,
                const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' has the same value as the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator==(const SecondFunctionComparableType& lhs,
                const FirstFunctionComparableType&  rhs);
    // Return 'true' if the specified 'lhs' has the same value as the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator!=(const SecondFunctionComparableType& lhs,
                const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' does not have the same value as the
    // specified 'rhs' nullable object, and 'false' otherwise.

bool operator!=(const SecondFunctionComparableType& lhs,
                const FirstFunctionComparableType&  rhs);
    // Return 'true' if the specified 'lhs' does not have the same value as the
    // specified 'rhs' nullable object, and 'false' otherwise.

bool operator<(const SecondFunctionComparableType& lhs,
               const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator<(const SecondFunctionComparableType& lhs,
               const FirstFunctionComparableType&  rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator<=(const SecondFunctionComparableType& lhs,
                const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.

bool operator<=(const SecondFunctionComparableType& lhs,
                const FirstFunctionComparableType&  rhs);
    // Return 'true' if the specified 'lhs' is ordered before the specified
    // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.

bool operator>(const SecondFunctionComparableType& lhs,
               const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator>(const SecondFunctionComparableType& lhs,
               const FirstFunctionComparableType&  rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' nullable object, and 'false' otherwise.

bool operator>=(const SecondFunctionComparableType& lhs,
                const SecondFunctionComparableType& rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.

bool operator>=(const SecondFunctionComparableType& lhs,
                const FirstFunctionComparableType&  rhs);
    // Return 'true' if the specified 'lhs' is ordered after the specified
    // 'rhs' nullable object or 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.

                   // ---------------------------------
                   // class FirstFunctionComparableType
                   // ---------------------------------
// CREATORS
FirstFunctionComparableType::FirstFunctionComparableType(int data)
: d_data(data)
{}

// ACCESSORS
int FirstFunctionComparableType::data() const
{
    return d_data;
}

// FREE OPERATORS
inline
bool operator==(const FirstFunctionComparableType& lhs,
                const FirstFunctionComparableType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator==(const FirstFunctionComparableType&  lhs,
                const SecondFunctionComparableType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const FirstFunctionComparableType& lhs,
                const FirstFunctionComparableType& rhs)
{
    return lhs.data() != rhs.data();
}

inline
bool operator!=(const FirstFunctionComparableType&  lhs,
                const SecondFunctionComparableType& rhs)
{
    return lhs.data() != rhs.data();
}

inline
bool operator<(const FirstFunctionComparableType& lhs,
               const FirstFunctionComparableType& rhs)
{
    return lhs.data() < rhs.data();
}

inline
bool operator<(const FirstFunctionComparableType&  lhs,
               const SecondFunctionComparableType& rhs)
{
    return lhs.data() < rhs.data();
}

inline
bool operator<=(const FirstFunctionComparableType& lhs,
                const FirstFunctionComparableType& rhs)
{
    return lhs.data() <= rhs.data();
}

inline
bool operator<=(const FirstFunctionComparableType&  lhs,
                const SecondFunctionComparableType& rhs)
{
    return lhs.data() <= rhs.data();
}

inline
bool operator>(const FirstFunctionComparableType& lhs,
               const FirstFunctionComparableType& rhs)
{
    return lhs.data() > rhs.data();
}

inline
bool operator>(const FirstFunctionComparableType&  lhs,
               const SecondFunctionComparableType& rhs)
{
    return lhs.data() > rhs.data();
}

inline
bool operator>=(const FirstFunctionComparableType& lhs,
                const FirstFunctionComparableType& rhs)
{
    return lhs.data() >= rhs.data();
}

inline
bool operator>=(const FirstFunctionComparableType&  lhs,
                const SecondFunctionComparableType& rhs)
{
    return lhs.data() >= rhs.data();
}

                   // ----------------------------------
                   // class SecondFunctionComparableType
                   // ----------------------------------
// CREATORS
SecondFunctionComparableType::SecondFunctionComparableType(int data)
: d_data(data)
{}

// ACCESSORS
int SecondFunctionComparableType::data() const
{
    return d_data;
}

// FREE OPERATORS
inline
bool operator==(const SecondFunctionComparableType& lhs,
                const SecondFunctionComparableType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator==(const SecondFunctionComparableType& lhs,
                const FirstFunctionComparableType&  rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const SecondFunctionComparableType& lhs,
                const SecondFunctionComparableType& rhs)
{
    return lhs.data() != rhs.data();
}

inline
bool operator!=(const SecondFunctionComparableType& lhs,
                const FirstFunctionComparableType&  rhs)
{
    return lhs.data() != rhs.data();
}

inline
bool operator<(const SecondFunctionComparableType& lhs,
               const SecondFunctionComparableType& rhs)
{
    return lhs.data() < rhs.data();
}

inline
bool operator<(const SecondFunctionComparableType& lhs,
               const FirstFunctionComparableType&  rhs)
{
    return lhs.data() < rhs.data();
}

inline
bool operator<=(const SecondFunctionComparableType& lhs,
                const SecondFunctionComparableType& rhs)
{
    return lhs.data() <= rhs.data();
}

inline
bool operator<=(const SecondFunctionComparableType& lhs,
                const FirstFunctionComparableType&  rhs)
{
    return lhs.data() <= rhs.data();
}

inline
bool operator>(const SecondFunctionComparableType& lhs,
               const SecondFunctionComparableType& rhs)
{
    return lhs.data() > rhs.data();
}

inline
bool operator>(const SecondFunctionComparableType& lhs,
               const FirstFunctionComparableType&  rhs)
{
    return lhs.data() > rhs.data();
}

inline
bool operator>=(const SecondFunctionComparableType& lhs,
                const SecondFunctionComparableType& rhs)
{
    return lhs.data() >= rhs.data();
}

inline
bool operator>=(const SecondFunctionComparableType& lhs,
                const FirstFunctionComparableType&  rhs)
{
    return lhs.data() >= rhs.data();
}

}  // close unnamed namespace

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

enum MessageType {
    // Type used for testing 'makeValue' in case 12.

    JUNK,
    IMPORTANT
};

class Recipient {
    // This 'class' is used for testing 'makeValue' in case 12.  The 'class'
    // has an explicit conversion constructor such that if the implementation
    // of 'makeValue' relies on implicit conversion from 'OTHER_TYPE' to
    // 'TYPE', the code will not compile.

    // DATA
    MessageType d_msgType;

    // FRIENDS
    friend bool operator==(const Recipient&, const Recipient&);

  public:
    // CREATORS
    explicit Recipient(const MessageType& msgType)
        // Create a 'Recipient' object using the specified 'msgType'.
    : d_msgType(msgType)
    {
    }

    // MANIPULATORS
    Recipient& operator=(const MessageType& rhs)
    {
        d_msgType = rhs;

        return *this;
    }

    // ACCESSORS
    MessageType msgType() const
    {
        return d_msgType;
    }
};

// FREE OPERATORS
bool operator==(const Recipient& lhs, const Recipient& rhs)
{
    return lhs.d_msgType == rhs.d_msgType;
}

class Swappable {
    // 'Swappable', used for testing 'swap', does not take an allocator.

    // CLASS DATA
    static bool s_swapCalledFlag;  // 'true' if 'swap' free function called
                                   // since last 'reset'; 'false' otherwise

    // DATA
    int d_value;

    // FRIENDS
    friend void swap(Swappable&, Swappable&);

  private:
    // NOT IMPLEMENTED
    void swap(Swappable&);

  public:
    // CLASS METHODS
    static void reset()
    {
        s_swapCalledFlag = false;
    }

    static bool swapCalled()
    {
        return s_swapCalledFlag;
    }

    // CREATORS
    explicit Swappable(int v)
    : d_value(v)
    {
    }

    Swappable(const Swappable& original)
    : d_value(original.d_value)
    {
    }

    // MANIPULATORS
    // Swappable& operator=(const Swappable&);  // default

    // ACCESSORS
    int value() const
    {
        return d_value;
    }
};

// FREE OPERATORS
bool operator==(const Swappable& lhs, const Swappable& rhs)
{
    return lhs.value() == rhs.value();
}

bool operator!=(const Swappable& lhs, const Swappable& rhs)
{
    return !(lhs == rhs);
}

// CLASS DATA
bool Swappable::s_swapCalledFlag = false;

void swap(Swappable& a, Swappable& b)
{
    Swappable::s_swapCalledFlag = true;

    bslalg::SwapUtil::swap(&a.d_value, &b.d_value);
}

class SwappableWithAllocator {
    // 'SwappableWithAllocator', used for testing 'swap', takes an allocator.

    // CLASS DATA
    static bool s_swapCalledFlag;  // 'true' if 'swap' free function called
                                   // since last 'reset'; 'false' otherwise

    // DATA
    int               d_value;
    bsl::string       d_string;
    bslma::Allocator *d_allocator_p;  // held, not owned

    // FRIENDS
    friend void swap(SwappableWithAllocator&, SwappableWithAllocator&);

  private:
    // NOT IMPLEMENTED
    void swap(SwappableWithAllocator&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(SwappableWithAllocator,
                                   bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static void reset()
    {
        s_swapCalledFlag = false;
    }

    static bool swapCalled()
    {
        return s_swapCalledFlag;
    }

    // CREATORS
    explicit
    SwappableWithAllocator(int v, bslma::Allocator *basicAllocator = 0)
    : d_value(v)
    , d_string(bsl::abs(v), 'x', basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    SwappableWithAllocator(const SwappableWithAllocator&  original,
                           bslma::Allocator              *basicAllocator = 0)
    : d_value(original.d_value)
    , d_string(original.d_string, basicAllocator)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    ~SwappableWithAllocator()
    {
        BSLS_ASSERT_OPT(allocator() == d_string.get_allocator().mechanism());
    }

    // MANIPULATORS
    SwappableWithAllocator& operator=(const SwappableWithAllocator& rhs)
    {
        d_value  = rhs.d_value;
        d_string = rhs.d_string;

        return *this;
    }

    // ACCESSORS
    bslma::Allocator *allocator() const
    {
        return d_allocator_p;
    }

    const bsl::string& string() const
    {
        return d_string;
    }

    int value() const
    {
        return d_value;
    }
};

// FREE OPERATORS
bool operator==(const SwappableWithAllocator& lhs,
                const SwappableWithAllocator& rhs)
{
    return lhs.value() == rhs.value() && lhs.string() == rhs.string();
}

bool operator!=(const SwappableWithAllocator& lhs,
                const SwappableWithAllocator& rhs)
{
    return !(lhs == rhs);
}

// CLASS DATA
bool SwappableWithAllocator::s_swapCalledFlag = false;

void swap(SwappableWithAllocator& a, SwappableWithAllocator& b)
{
    SwappableWithAllocator::s_swapCalledFlag = true;

    if (a.allocator() == b.allocator()) {
        bslalg::SwapUtil::swap(&a.d_value, &b.d_value);

        a.d_string.swap(b.d_string);

        return;                                                       // RETURN
    }

    SwappableWithAllocator futureA(b, a.allocator());
    SwappableWithAllocator futureB(a, b.allocator());

    bslalg::SwapUtil::swap(&a.d_value, &futureA.d_value);
    bslalg::SwapUtil::swap(&b.d_value, &futureB.d_value);

    a.d_string.swap(futureA.d_string);
    b.d_string.swap(futureB.d_string);
}

void dummyFunction()
    // Do nothing.
{
}

// ASPECTS
namespace BloombergLP {
namespace bslh {

template <class HASHALG, class RETURN, class CLASS>
void hashAppend(HASHALG& hashAlg, RETURN (CLASS::*member)())
{
    hashAlg(&member, sizeof(member));
}

template <class HASHALG, class RETURN, class CLASS>
void hashAppend(HASHALG& hashAlg, RETURN (CLASS::*member)() const)
{
    hashAlg(&member, sizeof(member));
}

}  // close namespace bslh
}  // close enterprise namespace

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

#define TEST_TYPES_NOT_ALLOCATOR_ENABLED                                      \
        signed char,                                                          \
        size_t,                                                               \
        bsltf::TemplateTestFacility::ObjectPtr,                               \
        bsltf::TemplateTestFacility::FunctionPtr,                             \
        bsltf::TemplateTestFacility::MethodPtr,                               \
        bsltf::EnumeratedTestType::Enum,                                      \
        bsltf::UnionTestType,                                                 \
        bsltf::SimpleTestType,                                                \
        bsltf::MovableTestType,                                               \
        bsltf::BitwiseMoveableTestType


#define TEST_TYPES_ALLOCATOR_ENABLED                                          \
        bsltf::AllocTestType,                                                 \
        bsltf::MovableAllocTestType,                                          \
        bsltf::AllocBitwiseMoveableTestType

#define TEST_TYPES                                                            \
        TEST_TYPES_NOT_ALLOCATOR_ENABLED,                                     \
        TEST_TYPES_ALLOCATOR_ENABLED
    // The list of test types is a combination of
    // 'BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE' and '*_USER_DEFINED',
    // minus 'NonTypicalOverloadsTestType' which does not work with
    // 'bslalg::ConstructorProxy'.

template <class TEST_TYPE>
class TestDriver {
    // This template class provides a namespace for testing
    // 'bdlb::NullableValue'.  The template parameter 'TEST_TYPE' specifies the
    // type contained in the nullable object.

  private:
    // PRIVATE TYPES
    typedef bdlb::NullableValue<TEST_TYPE>    Obj;
        // Type under test.

    typedef bslalg::ConstructorProxy<Obj>     ObjWithAllocator;
        // Wrapper for 'Obj' whose constructor takes an allocator.

    typedef bsltf::TestValuesArray<TEST_TYPE> TestValues;
        // Array of test values of 'TEST_TYPE'.

    typedef bsltf::TemplateTestFacility       Util;
    typedef bslmf::MovableRefUtil             MoveUtil;
    typedef bsltf::MoveState                  MoveState;

    // TEST APPARATUS
    // ------------------------------------------------------------------------
    // The generating functions interpret the given 'spec' in order from left
    // to right to configure the object according to a custom language.
    // Uppercase letters [A..Z] correspond to arbitrary (but unique) 'char'
    // values to assign to the 'bdlb::NullableValue<TEST_TYPE>' object.
    //
    // LANGUAGE SPECIFICATION
    // ----------------------
    //
    // <SPEC>       ::= <VALUE-SPEC> | <NULL-SPEC>
    //
    // <VALUE-SPEC> ::= 'A' | 'B' | 'C' | 'D' | 'E' | ... | 'Z'
    //                                      // unique but otherwise arbitrary
    //
    // <NULL-SPEC>  ::= '~'
    //
    // Spec String  Description
    // -----------  -----------------------------------------------------------
    // "~"          Make the object null.
    // "A"          Set the object to have the value corresponding to 'A'.
    // ------------------------------------------------------------------------

  private:
    // PRIVATE CLASS METHODS
    static int ggg(Obj *object, const char *spec, int verbose = 1);
        // Configure the specified 'object' according to the specified 'spec',
        // using only the primary manipulator function 'makeValue' and
        // white-box manipulator 'reset'.  Optionally specify a zero 'verbose'
        // to suppress 'spec' syntax error messages.  Return the index of the
        // first invalid character, and a negative value otherwise.  Note that
        // this function is used to implement 'gg' as well as allow for
        // verification of syntax error detection.

    static Obj& gg(Obj *object, const char *spec);
        // Return, by reference, the specified 'object' with its value adjusted
        // according to the specified 'spec'.

    static void primaryManipulator(Obj              *object,
                                   int               identifier,
                                   bslma::Allocator *basicAllocator = 0);
        // Assign to the specified 'object' the value indicated by the
        // specified 'identifier', ensuring that the overload of the primary
        // manipulator taking a modifiable rvalue is invoked (rather than the
        // one taking an lvalue).  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

  public:
    // CLASS METHODS
    static void testCase14();
        // Test 'T valueOr(const T&)'.

    static void testCase15();
        // Test 'const T *valueOr(const T *)'.

    static void testCase16();
        // Test 'valueOrNull'.

    static void testCase18_withoutAllocator();
        // Test 'makeValueInplace' methods using a contained 'TYPE' that does
        // not have the 'bslma::UsesBslmaAllocator' trait.

    static void testCase18_withAllocator();
        // Test 'makeValueInplace' methods using a contained 'TYPE' that has
        // the 'bslma::UsesBslmaAllocator' trait.

    static void testCase19();
        // Test 'hashAppend' on 'nullableValue' objects.

    static void testCase20_withoutAllocator();
        // Test move constructor using a contained 'TYPE' that does not have
        // the 'bslma::UsesBslmaAllocator' trait.

    static void testCase20_withAllocator();
        // Test move constructor using a contained 'TYPE' that has the
        // 'bslma::UsesBslmaAllocator' trait.

    static void testCase21_withoutAllocator();
        // Test move-assignment using a contained 'TYPE' that does not have the
        // 'bslma::UsesBslmaAllocator' trait.

    static void testCase21_withAllocator();
        // Test move-assignment using a contained 'TYPE' that has the
        // 'bslma::UsesBslmaAllocator' trait.

    static void testCase22_withoutAllocator();
        // Test value move constructor using a contained 'TYPE' that does not
        // have the 'bslma::UsesBslmaAllocator' trait.

    static void testCase22_withAllocator();
        // Test value move constructor using a contained 'TYPE' that has the
        // 'bslma::UsesBslmaAllocator' trait.

    static void testCase23_withoutAllocator();
        // Test value move-assignment using a contained 'TYPE' that does not
        // have the 'bslma::UsesBslmaAllocator' trait.

    static void testCase23_withAllocator();
        // Test value move-assignment using a contained 'TYPE' that has the
        // 'bslma::UsesBslmaAllocator' trait.

    static void testCase25();
        // Test 'const T *addressOr(const T *)'.
};

// PRIVATE CLASS METHODS
template <class TEST_TYPE>
int TestDriver<TEST_TYPE>::ggg(Obj *object, const char *spec, int verbose)
{
    enum { POSITION = 0, SUCCESS = -1 };

    if ('~' == *spec) {
        object->reset();
    }
    else if (*spec >= 'A' && *spec <= 'Z') {
        primaryManipulator(object, *spec);
    }
    else {
        if (verbose) {
            printf("Error, bad character ('%c') "
                   "in spec \"%s\" at position %d.\n", *spec, spec, POSITION);
        }
        return POSITION;                                              // RETURN
    }
    return SUCCESS;
}

template <class TEST_TYPE>
bdlb::NullableValue<TEST_TYPE>& TestDriver<TEST_TYPE>::gg(Obj        *object,
                                                          const char *spec)
{
    ASSERTV(ggg(object, spec) < 0);
    return *object;
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::primaryManipulator(
                                              Obj              *object,
                                              int               identifier,
                                              bslma::Allocator *basicAllocator)
{
    bslma::TestAllocator         da("default");
    bslma::DefaultAllocatorGuard dag(&da);

    bsls::ObjectBuffer<TEST_TYPE> buffer;
    Util::emplace(buffer.address(), identifier, basicAllocator);

    bslma::DestructorGuard<TEST_TYPE> guard(buffer.address());

    object->makeValue(MoveUtil::move(buffer.object()));
}

// CLASS METHODS
template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase23_withoutAllocator()
{
    // ------------------------------------------------------------------------
    // TESTING VALUE MOVE-ASSIGNMENT (without allocator)
    //
    // Concerns:
    //: 1 The signature and return type are standard.
    //:
    //: 2 The reference returned is to the target object (i.e., '*this').
    //:
    //: 3 The move-assignment operator can change the value of a modifiable
    //:   target object to that of any source object.
    //:
    //: 4 The move-assignment operator is called on the contained value if the
    //:   target is not null; otherwise, the move constructor is called.
    //:
    //: 5 The source object is left in a valid but unspecified state.
    //:
    //: 6 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //: 7 There is no memory allocated.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   move-assignment operator defined in this component.             (C-1)
    //:
    //: 2 Iterate over a set of object values with substantial and varied
    //:   differences, ordered by increasing length, and create for each a
    //:   control object representing the source of the assignment.       (C-3)
    //:
    //: 3 Iterate again over the same set of object values and create an object
    //:   representing the target of the assignment.
    //:
    //: 4 Create a source object (a copy of the control object in P-2).
    //:
    //: 5 Call the move-assignment operator and verify the following:
    //:
    //:   1 The address of the return value is the same as that of the target
    //:     object.                                                       (C-2)
    //:
    //:   2 The object being assigned to has the same value as that of the
    //:     source object before assignment (i.e., the control object).   (C-3)
    //:
    //:   3 The move constructor or move-assignment operator is called on the
    //:     contained value according to whether or not the source value is
    //:     null.                                                         (C-4)
    //:
    //:   4 Manipulate the source object (after assignment) to ensure that it
    //:     is in a valid state, destroy it, and then manipulate the target
    //:     object to ensure that it is in a valid state.               (C-5,6)
    //:
    //:   5 Verify no memory is allocated using the default allocator.    (C-7)
    //
    // Testing:
    //   NullableValue& operator=(TYPE&& rhs);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "~",
        "A",
        "B",
        "C"
    };
    const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    Obj& (Obj::*operatorMAg) (bslmf::MovableRef<TEST_TYPE>) = &Obj::operator=;
    (void) operatorMAg;  // quash potential compiler warning

    if (verbose) printf("\nTesting move assignment (no allocator).\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC1 = SPECS[ti];

            if ('~' == *SPEC1) continue;

            if (veryVerbose) {
                P(SPEC1);
            }

            // Create control object 'W' ('emplace' requires an allocator).

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bsls::ObjectBuffer<TEST_TYPE> bufferW;

            TEST_TYPE *pW = bufferW.address();
            Util::emplace(pW, *SPEC1, &scratch);

            bslma::DestructorGuard<TEST_TYPE> guard(pW);

            const TEST_TYPE& W = *pW;

            for (int tj = 0; tj < NUM_SPECS; ++tj) {
                const char *const SPEC2 = SPECS[tj];
                const char *const SET2  = '~' == *SPEC2 ? "null" : "set";

                if (veryVerbose) {
                    printf("\n\tFor a target object that is %s.\n", SET2);
                    P(SPEC2);
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                // Create source object 'Y'.
                bsls::ObjectBuffer<TEST_TYPE> bufferY;

                TEST_TYPE *pY = bufferY.address();
                Util::emplace(pY, *SPEC1, &scratch);

                TEST_TYPE& mY = *pY;  const TEST_TYPE& Y = mY;

                ASSERTV(SPEC1, SPEC2, Y == W);

                // Create target object 'X'
                Obj *objPtr = new (fa) Obj();
                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC2);

                ASSERTV(SPEC1, SPEC2, (X == Y) == (ti == tj));

                // Verify initial move state is correct.
                MoveState::Enum mState = Util::getMovedFromState(Y);
                ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                             || MoveState::e_NOT_MOVED == mState);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(mY));
                ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                // Verify the value of the object.
                ASSERTV(SPEC1, SPEC2, X == W);

                // Verify the move state is correct.
                mState = Util::getMovedFromState(Y);
                ASSERTV(mState, MoveState::e_UNKNOWN == mState
                             || MoveState::e_MOVED   == mState);

                pY->~TEST_TYPE();
                fa.deleteObject(objPtr);

                // Verify no memory was ever allocated.
                ASSERTV(SPEC1, SPEC2, 0 == da.numBlocksTotal());
            }
        }
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase23_withAllocator()
{
    // ------------------------------------------------------------------------
    // TESTING VALUE MOVE-ASSIGNMENT (with allocator)
    //
    // Concerns:
    //: 1 The signature and return type are standard.
    //:
    //: 2 The reference returned is to the target object (i.e., '*this').
    //:
    //: 3 The move-assignment operator can change the value of a modifiable
    //:   target object to that of any source object.
    //:
    //: 4 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 5 The allocator address held by the target object is unchanged.
    //:
    //: 6 The move-assignment operator is called on the contained value if the
    //:   target is not null; otherwise, the move constructor is called.
    //:
    //: 7 The source object is left in a valid but unspecified state and the
    //:   allocator address held by the original object is unchanged.
    //:
    //: 8 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-assigned object and vice-versa.
    //:
    //: 9 Every object releases any allocated memory at destruction.
    //:
    //:10 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   move-assignment operator defined in this component.             (C-1)
    //:
    //: 2 Iterate over a set of object values with substantial and varied
    //:   differences, ordered by increasing length, and create for each a
    //:   control object representing the source of the assignment, with its
    //:   own scratch allocator.
    //:
    //: 3 Iterate again over the same set of object values and create an object
    //:   representing the target of the assignment, with its own unique object
    //:   allocator.
    //:
    //: 4 In a loop consisting of two iterations, create a source object (a
    //:   copy of the control object in P-1) with 1) a different allocator than
    //:   that of target, and 2) the same allocator as that of the target.
    //:
    //: 5 Call the move-assignment operator in the presence of exceptions
    //:   during memory allocations (using a 'bslma::TestAllocator' and varying
    //:   its allocation limit) and verify the following:                (C-12)
    //:
    //:   1 The address of the return value is the same as that of the target
    //:     object.                                                       (C-2)
    //:
    //:   2 The object being assigned to has the same value as that of the
    //:     source object before assignment (i.e., the control object).   (C-3)
    //:
    //:   3 Ensure that the source, target, and control objects continue to
    //:     have the correct allocator and that all memory allocations come
    //:     from the appropriate allocators.                              (C-4)
    //:
    //:   4 Manipulate the source object (after assignment) to ensure that it
    //:     is in a valid state, destroy it, and then manipulate the target
    //:     object to ensure that it is in a valid state.                 (C-8)
    //:
    //:   5 Verify all memory is released when the source and target objects
    //:     are destroyed.                                               (C-11)
    //:
    //: 6 Use a test allocator installed as the default allocator to verify
    //:   that no memory is ever allocated from the default allocator.
    //
    // Testing:
    //   NullableValue& operator=(TYPE&& rhs);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "~",
        "A",
        "B",
        "C"
    };
    const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    Obj& (Obj::*operatorMAg) (bslmf::MovableRef<TEST_TYPE>) = &Obj::operator=;
    (void) operatorMAg;  // quash potential compiler warning

    if (verbose) printf("\nTesting move-assignment (with allocator).\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC1 = SPECS[ti];

            if ('~' == *SPEC1) continue;

            if (veryVerbose) {
                P(SPEC1);
            }

            // Create control object 'W'.
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bsls::ObjectBuffer<TEST_TYPE> bufferW;

            TEST_TYPE *pW = bufferW.address();
            Util::emplace(pW, *SPEC1, &scratch);

            bslma::DestructorGuard<TEST_TYPE> guard(pW);

            const TEST_TYPE& W = *pW;

            for (int tj = 0; tj < NUM_SPECS; ++tj) {
                const char *const SPEC2 = SPECS[tj];
                const char *const SET2  = '~' == *SPEC2 ? "null" : "set";

                if (veryVerbose) {
                    printf("\n\tFor a target object that is %s.\n", SET2);
                    P(SPEC2);
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);

                    // Create source object 'Y'.
                    bsls::ObjectBuffer<TEST_TYPE> bufferY;

                    TEST_TYPE *srcPtr = bufferY.address();

                    bslma::TestAllocator *srcAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        Util::emplace(srcPtr, *SPEC1, &za);
                        srcAllocatorPtr = &za;
                      } break;
                      case 'b': {
                        Util::emplace(srcPtr, *SPEC1, &oa);
                        srcAllocatorPtr = &oa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }

                    bslma::TestAllocator& sa = *srcAllocatorPtr;

                    TEST_TYPE& mY = *srcPtr;  const TEST_TYPE& Y = mY;

                    ASSERTV(SPEC1, SPEC2, Y == W);

                    // Create target object 'X'.
                    Obj *objPtr = new (fa) Obj(&oa);
                    Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC2);

                    if ('~' != *SPEC2) {
                        ASSERTV(SPEC1, SPEC2, (X.value() == Y) == (ti == tj));
                    }

                    // Verify initial move state is correct.
                    MoveState::Enum mState = Util::getMovedFromState(Y);
                    ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                                 || MoveState::e_NOT_MOVED == mState);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mY));
                    ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                    // Verify the value of the object.
                    ASSERTV(SPEC1, SPEC2, X.value() == W);

                    // Verify the move state is correct.
                    mState = Util::getMovedFromState(Y);
                    ASSERTV(mState, MoveState::e_UNKNOWN == mState
                                 || MoveState::e_MOVED   == mState);

                    srcPtr->~TEST_TYPE();
                    fa.deleteObject(objPtr);

                    ASSERTV(SPEC1, SPEC2, 0 == oa.numBlocksInUse());
                    ASSERTV(SPEC1, SPEC2, 0 == sa.numBlocksInUse());
                }
            }
        }
        // Verify no memory was ever allocated from the default allocator.
        ASSERTV(0 == da.numBlocksTotal());
    }

#if defined(BDE_BUILD_TARGET_EXC)
    if (verbose)
        printf("\nTesting move assignment (w/allocator) with exceptions.\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC1 = SPECS[ti];

            if ('~' == *SPEC1) continue;

            if (veryVerbose) {
                P(SPEC1);
            }

            // Create control object 'W'.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bsls::ObjectBuffer<TEST_TYPE> bufferW;

            TEST_TYPE *pW = bufferW.address();
            Util::emplace(pW, *SPEC1, &scratch);

            bslma::DestructorGuard<TEST_TYPE> guard(pW);

            const TEST_TYPE& W = *pW;

            for (int tj = 0; tj < NUM_SPECS; ++tj) {
                const char *const SPEC2 = SPECS[tj];
                const char *const SET2  = '~' == *SPEC2 ? "null" : "set";

                if (veryVerbose) {
                    printf("\n\tFor a target object that is %s.\n", SET2);
                    P(SPEC2);
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    if (veryVerbose) { P(CONFIG); }

                    bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);

                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        // Create source object 'Y'.
                        bslma::TestAllocator *srcAllocatorPtr;

                        switch (CONFIG) {
                          case 'a': {
                            srcAllocatorPtr = &za;
                          } break;
                          case 'b': {
                            srcAllocatorPtr = &oa;
                          } break;
                          default: {
                            ASSERTV(CONFIG, !"Bad allocator config.");
                          } return;                                   // RETURN
                        }
                        bslma::TestAllocator& sa = *srcAllocatorPtr;

                        const bsls::Types::Int64 taLimit =
                                                          oa.allocationLimit();
                        oa.setAllocationLimit(-1);

                        bsls::ObjectBuffer<TEST_TYPE> bufferY;

                        TEST_TYPE *pY = bufferY.address();
                        Util::emplace(pY, *SPEC1, &sa);

                        bslma::DestructorGuard<TEST_TYPE> guard(pY);

                        TEST_TYPE& mY = *pY;  const TEST_TYPE& Y = mY;

                        oa.setAllocationLimit(taLimit);

                        ASSERTV(SPEC1, SPEC2, Y == W);

                        // Create target object 'X'.
                        Obj mX(&oa);  const Obj& X = gg(&mX, SPEC2);

                        if ('~' != *SPEC2) {
                            ASSERTV(SPEC1, SPEC2,
                                    (X.value() == Y) == (ti == tj));
                        }

                        // Verify initial move state is correct.
                        MoveState::Enum mState = Util::getMovedFromState(Y);
                        ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                                     || MoveState::e_NOT_MOVED == mState);

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mY));
                        ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                        // Verify the value of the object.
                        ASSERTV(SPEC1, SPEC2, X == W);

                        // Verify the move state is correct.
                        mState = Util::getMovedFromState(Y);
                        ASSERTV(mState, MoveState::e_UNKNOWN == mState
                                     || MoveState::e_MOVED   == mState);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERTV(0 == tj || numPasses > 1);
                }
            }
        }
    }
#endif
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase22_withoutAllocator()
{
    // ------------------------------------------------------------------------
    // TESTING VALUE MOVE CONSTRUCTOR (without allocator)
    //
    // Concerns:
    //: 1 The newly created object has the same value (using the equality
    //:   operator) as that of the original object before the call.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value, including the null state.
    //:
    //: 3 The move constructor is called on the contained value if the source
    //:   value is not null.
    //:
    //: 4 The original object is left in a valid state.
    //:
    //: 5 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //
    // Plan:
    //: 1 Specify a set 'S' of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used sequentially in
    //:   the following tests; for each entry, create a control object.   (C-2)
    //:
    //: 2 For each of the object values (P-1), verify the following:
    //:
    //:   1 Verify the newly created object has the same value as that of the
    //:     original object before the call to the move constructor (control
    //:     value).                                                       (C-1)
    //:
    //:   2 Ensure that the move state reflects a call to the move constructor
    //:     for the contained value if the source value was not null.     (C-3)
    //:
    //:   3 Manipulate the original object (after the move construction) to
    //:     ensure it is in a valid state.                                (C-4)
    //:
    //:   4 Destroy the original object, and then manipulate the newly created
    //:     object to ensure that it is in a valid state.                 (C-5)
    //
    // Testing:
    //   NullableValue(TYPE&& value);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "A",
        "B",
        "C"
    };
    const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    if (verbose)
        printf("\nTesting move constructor (no allocator).\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC = SPECS[ti];

            if (veryVerbose) {
                P(SPEC);
            }

            // Create control object 'W' ('emplace' requires an allocator).

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bsls::ObjectBuffer<TEST_TYPE> bufferW;

            TEST_TYPE *pW = bufferW.address();
            Util::emplace(pW, *SPEC, &scratch);

            bslma::DestructorGuard<TEST_TYPE> guard(pW);

            const TEST_TYPE& W = *pW;

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // Create source object 'Y'.
            bsls::ObjectBuffer<TEST_TYPE> bufferY;

            TEST_TYPE *pY = bufferY.address();
            Util::emplace(pY, *SPEC, &scratch);
            TEST_TYPE& mY = *pY;  const TEST_TYPE& Y = mY;

            ASSERTV(SPEC, Y == W);

            // Verify initial move state is correct.
            MoveState::Enum mState = Util::getMovedFromState(Y);
            ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                         || MoveState::e_NOT_MOVED == mState);

            Obj *objPtr = new (fa) Obj(MoveUtil::move(mY));
            Obj& mX = *objPtr;  const Obj& X = mX;

            // Verify the value of the object.
            ASSERTV(SPEC, X.value() == W);

            // Verify the move state is correct.
            mState = Util::getMovedFromState(Y);
            ASSERTV(mState, MoveState::e_UNKNOWN == mState
                         || MoveState::e_MOVED   == mState);

            pY->~TEST_TYPE();
            fa.deleteObject(objPtr);

            // Verify no memory was ever allocated.
            ASSERTV(SPEC, 0 == da.numBlocksTotal());
        }
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase22_withAllocator()
{
    // ------------------------------------------------------------------------
    // TESTING VALUE MOVE CONSTRUCTOR (with allocator)
    //
    // Concerns:
    //: 1 The newly created object has the same value (using the equality
    //:   operator) as that of the original object before the call.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The allocator is propagated to the newly created object if (and only
    //:   if) no allocator is specified in the call to the move constructor.
    //:
    //: 4 The move constructor is called on the contained value if the source
    //:   value is not null.
    //:
    //: 5 The original object is always left in a valid state and the allocator
    //:   address held by the original object is unchanged.
    //:
    //: 6 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //: 7 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //
    //: 9 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 Specify a set 'S' of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used sequentially in
    //:   the following tests; for each entry, create a control object.   (C-2)
    //:
    //: 2 Call the move constructor to create the container in all relevant use
    //:   cases involving the allocator: 1) no allocator passed in, 2) a '0' is
    //    explicitly passed in as the allocator argument, 3) the same allocator
    //:   as that of the original object is explicitly passed in, and 4) a
    //:   different allocator than that of the original object is passed in.
    //:
    //: 3 For each of the object values (P-1) and for each configuration (P-2),
    //:   verify the following:
    //:
    //:   1 Verify the newly created object has the same value as that of the
    //:     original object before the call to the move constructor (control
    //:     value).                                                       (C-1)
    //:
    //:   2 Ensure that the move state reflects a call to the move constructor
    //:     for the contained value if the source value was not null.     (C-4)
    //:
    //:   3 Ensure that the new original, and control object continue to have
    //:     the correct allocator and that all memory allocations come from the
    //:     appropriate allocator.                                      (C-3,7)
    //:
    //:   7 Manipulate the original object (after the move construction) to
    //:     ensure it is in a valid state, destroy it, and then manipulate the
    //:     newly created object to ensure that it is in a valid state. (C-5,6)
    //:
    //:   8 Verify all memory is released when the object is destroyed.   (C-8)
    //:
    //: 4 Perform tests in the presence of exceptions during memory allocations
    //:   using a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //:                                                                   (C-9)
    //
    // Testing:
    //   NullableValue(TYPE&& value);
    //   NullableValue(TYPE&& value, allocator);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "A",
        "B",
        "C"
    };
    const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    if (verbose)
        printf("\nTesting both versions of move constructor (with alloc).\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC = SPECS[ti];

            if (veryVerbose) {
                P(SPEC);
            }

            // Create control object 'W' with a scratch allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bsls::ObjectBuffer<TEST_TYPE> bufferW;

            TEST_TYPE *pW = bufferW.address();
            Util::emplace(pW, *SPEC, &scratch);

            bslma::DestructorGuard<TEST_TYPE> guard(pW);

            const TEST_TYPE& W = *pW;

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                // Create source object 'Y'.
                bsls::ObjectBuffer<TEST_TYPE> bufferY;

                TEST_TYPE *pY = bufferY.address();
                Util::emplace(pY, *SPEC, &sa);

                TEST_TYPE& mY = *pY;  const TEST_TYPE& Y = mY;

                ASSERTV(SPEC, CONFIG, Y == W);

                // Verify initial move state is correct.
                MoveState::Enum mState = Util::getMovedFromState(Y);
                ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                             || MoveState::e_NOT_MOVED == mState);

                bslma::TestAllocatorMonitor oam(&da), sam(&sa);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;
                bslma::TestAllocator *othAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mY));
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    oam.reset(&da);
                    objPtr = new (fa) Obj(MoveUtil::move(mY),
                                          (bslma::Allocator *)0);
                    objAllocatorPtr = &da;
                    othAllocatorPtr = &za;
                  } break;
                  case 'c': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mY), &sa);
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'd': {
                    oam.reset(&za);
                    objPtr = new (fa) Obj(MoveUtil::move(mY), &za);
                    objAllocatorPtr = &za;
                    othAllocatorPtr = &da;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = *othAllocatorPtr;

                Obj& mX = *objPtr;  const Obj& X = mX;

                // Verify the value of the object.
                ASSERTV(SPEC, CONFIG, X.value() == W);

                // Verify the move state is correct.
                mState = Util::getMovedFromState(Y);
                ASSERTV(mState, MoveState::e_UNKNOWN == mState
                             || MoveState::e_MOVED   == mState);

                // Verify that 'X' and 'Y' have the correct allocator.  (which
                // we can't, since its not exposed by the interface)

                // Verify no allocation from the non-object allocator and that
                // the object allocator is hooked up.
                if ('a' != CONFIG) {
                    ASSERTV(SPEC, CONFIG, 0 == noa.numBlocksTotal());
                }
                ASSERTV(SPEC, CONFIG, 0 < oa.numBlocksTotal());

                pY->~TEST_TYPE();
                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.
                ASSERTV(SPEC, 0 == fa.numBlocksInUse());
                ASSERTV(SPEC, 0 == da.numBlocksInUse());
                ASSERTV(SPEC, 0 == sa.numBlocksInUse());
                ASSERTV(SPEC, 0 == za.numBlocksInUse());
            }
        }
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase21_withoutAllocator()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT (without allocator)
    //
    // Concerns:
    //: 1 The signature and return type are standard.
    //:
    //: 2 The reference returned is to the target object (i.e., '*this').
    //:
    //: 3 The move-assignment operator can change the value of a modifiable
    //:   target object to that of any source object.
    //:
    //: 4 The move-assignment operator is called on the contained value if the
    //:   target is not null; otherwise, the move constructor is called.
    //:
    //: 5 The source object is left in a valid but unspecified state.
    //:
    //: 6 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //: 7 There is no memory allocated.
    //:
    //: 8 Assigning an object to itself behaves as expected (alias-safety).
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   move-assignment operator defined in this component.             (C-1)
    //:
    //: 2 Iterate over a set of object values with substantial and varied
    //:   differences, ordered by increasing length, and create for each a
    //:   control object representing the source of the assignment.       (C-3)
    //:
    //: 3 Iterate again over the same set of object values and create an object
    //:   representing the target of the assignment.                      (C-8)
    //:
    //: 4 Create a source object (a copy of the control object in P-2).
    //:
    //: 5 Call the move-assignment operator and verify the following:
    //:
    //:   1 The address of the return value is the same as that of the target
    //:     object.                                                       (C-2)
    //:
    //:   2 The object being assigned to has the same value as that of the
    //:     source object before assignment (i.e., the control object).   (C-3)
    //:
    //:   3 The move constructor or move-assignment operator is called on the
    //:     contained value according to whether or not the source value is
    //:     null.                                                         (C-4)
    //:
    //:   4 Manipulate the source object (after assignment) to ensure that it
    //:     is in a valid state, destroy it, and then manipulate the target
    //:     object to ensure that it is in a valid state.               (C-5,6)
    //:
    //:   5 Verify no memory is allocated using the default allocator.    (C-7)
    //
    // Testing:
    //   NullableValue& operator=(NullableValue&& rhs);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "~",
        "A",
        "B",
        "C"
    };
    const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    Obj& (Obj::*operatorMAg) (bslmf::MovableRef<Obj>) = &Obj::operator=;
    (void) operatorMAg;  // quash potential compiler warning

    if (verbose) printf("\nTesting move assignment (no allocator).\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC1 = SPECS[ti];
            const char *const SET1  = '~' == *SPEC1 ? "null" : "set";

            if (veryVerbose) {
                printf("\nFor a source object that is %s.\n", SET1);
                P(SPEC1);
            }

            // Create control object 'W' and alternate value 'Z'.
            Obj mW;  const Obj& W = gg(&mW, SPEC1);
            Obj mZ;  const Obj& Z = gg(&mZ, "Z");

            for (int tj = 0; tj < NUM_SPECS; ++tj) {
                const char *const SPEC2 = SPECS[tj];
                const char *const SET2  = '~' == *SPEC2 ? "null" : "set";

                if (veryVerbose) {
                    printf("\n\tFor a target object that is %s.\n", SET2);
                    P(SPEC2);
                }

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                // Create source object 'Y'.
                Obj *srcPtr = new (fa) Obj();
                Obj& mY = *srcPtr;  const Obj& Y = gg(&mY, SPEC1);

                ASSERTV(SPEC1, SPEC2, Y == W);

                // Create target object 'X'
                Obj *objPtr = new (fa) Obj();
                Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC2);

                ASSERTV(SPEC1, SPEC2, (X == Y) == (ti == tj));

                // Verify initial move state is correct.
                MoveState::Enum mState = Util::getMovedFromState(Y);
                ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                             || MoveState::e_NOT_MOVED == mState);

                Obj *mR = &(mX = bslmf::MovableRefUtil::move(mY));
                ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                // Verify the value of the object.
                ASSERTV(SPEC1, SPEC2, X == W);

                // Verify the move state is correct.
                mState = Util::getMovedFromState(Y);
                ASSERTV(mState, MoveState::e_UNKNOWN == mState
                             || MoveState::e_MOVED   == mState);

                // Manipulate source object 'Y' to ensure it is in a valid
                // state and is independent of 'X'.
                primaryManipulator(&mY, 'Z');
                ASSERTV(SPEC1, SPEC2, Y != W);
                ASSERTV(SPEC1, SPEC2, X == W);
                ASSERTV(SPEC1, SPEC2, Y == Z);

                fa.deleteObject(srcPtr);

                // Verify subsequent manipulation of new object 'X'.
                ASSERTV(SPEC1, SPEC2, X == W);

                primaryManipulator(&mX, 'Z');
                ASSERTV(SPEC1, SPEC2, X != W);
                ASSERTV(SPEC1, SPEC2, X == Z);

                fa.deleteObject(objPtr);

                // Verify no memory was ever allocated.
                ASSERTV(SPEC1, SPEC2, 0 == da.numBlocksTotal());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            {
                Obj mZ;  const Obj& Z = gg(&mZ, SPEC1);

                // Create target object.
                Obj mX;  const Obj& X = gg(&mX, SPEC1);

                ASSERTV(SPEC1, X == Z);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }
                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mX));
                    ASSERTV(SPEC1, X == Z);
                    ASSERTV(SPEC1, mR == &X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
            }
        }
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase21_withAllocator()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE-ASSIGNMENT (with allocator)
    //
    // Concerns:
    //: 1 The signature and return type are standard.
    //:
    //: 2 The reference returned is to the target object (i.e., '*this').
    //:
    //: 3 The move-assignment operator can change the value of a modifiable
    //:   target object to that of any source object.
    //:
    //: 4 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 5 The allocator address held by the target object is unchanged.
    //:
    //: 6 The move-assignment operator is called on the contained value if the
    //:   target is not null; otherwise, the move constructor is called.
    //:
    //: 7 The source object is left in a valid but unspecified state and the
    //:   allocator address held by the original object is unchanged.
    //:
    //: 8 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-assigned object and vice-versa.
    //:
    //: 9 Every object releases any allocated memory at destruction.
    //:
    //:10 Any memory allocation is exception neutral.
    //:
    //:11 Assigning an object to itself behaves as expected (alias-safety).
    //
    // Plan:
    //: 1 Use the address of 'operator=' to initialize a member-function
    //:   pointer having the appropriate signature and return type for the
    //:   move-assignment operator defined in this component.             (C-1)
    //:
    //: 2 Iterate over a set of object values with substantial and varied
    //:   differences, ordered by increasing length, and create for each a
    //:   control object representing the source of the assignment, with its
    //:   own scratch allocator.
    //:
    //: 3 Iterate again over the same set of object values and create an object
    //:   representing the target of the assignment, with its own unique object
    //:   allocator.
    //:
    //: 4 In a loop consisting of two iterations, create a source object (a
    //:   copy of the control object in P-1) with 1) a different allocator than
    //:   that of target, and 2) the same allocator as that of the target.
    //:
    //: 5 Call the move-assignment operator in the presence of exceptions
    //:   during memory allocations (using a 'bslma::TestAllocator' and varying
    //:   its allocation limit) and verify the following:                (C-12)
    //:
    //:   1 The address of the return value is the same as that of the target
    //:     object.                                                       (C-2)
    //:
    //:   2 The object being assigned to has the same value as that of the
    //:     source object before assignment (i.e., the control object).   (C-3)
    //:
    //:   3 Ensure that the source, target, and control objects continue to
    //:     have the correct allocator and that all memory allocations come
    //:     from the appropriate allocators.                              (C-4)
    //:
    //:   4 Manipulate the source object (after assignment) to ensure that it
    //:     is in a valid state, destroy it, and then manipulate the target
    //:     object to ensure that it is in a valid state.                 (C-8)
    //:
    //:   5 Verify all memory is released when the source and target objects
    //:     are destroyed.                                               (C-11)
    //:
    //: 6 Use a test allocator installed as the default allocator to verify
    //:   that no memory is ever allocated from the default allocator.
    //
    // Testing:
    //   NullableValue& operator=(NullableValue&& rhs);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "~",
        "A",
        "B",
        "C"
    };
    const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    Obj& (Obj::*operatorMAg) (bslmf::MovableRef<Obj>) = &Obj::operator=;
    (void) operatorMAg;  // quash potential compiler warning

    if (verbose) printf("\nTesting move-assignment (with allocator).\n");
    {
        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC1 = SPECS[ti];
            const char *const SET1  = '~' == *SPEC1 ? "null" : "set";

            if (veryVerbose) {
                printf("\nFor a source object that is %s.\n", SET1);
                P(SPEC1);
            }

            // Create control object 'W' and alternate value 'Z'.
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mW(&scratch);  const Obj& W = gg(&mW, SPEC1);
            Obj mZ(&scratch);  const Obj& Z = gg(&mZ, "Z");

            for (int tj = 0; tj < NUM_SPECS; ++tj) {
                const char *const SPEC2 = SPECS[tj];
                const char *const SET2  = '~' == *SPEC2 ? "null" : "set";

                if (veryVerbose) {
                    printf("\n\tFor a target object that is %s.\n", SET2);
                    P(SPEC2);
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);

                    // Create source object 'Y'.
                    Obj *srcPtr = 0;

                    bslma::TestAllocator *srcAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        srcPtr = new (fa) Obj(&za); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &za;
                      } break;
                      case 'b': {
                        srcPtr = new (fa) Obj(&oa); gg(srcPtr, SPEC1);
                        srcAllocatorPtr = &oa;
                      } break;
                      default: {
                        ASSERTV(CONFIG, !"Bad allocator config.");
                      } return;                                       // RETURN
                    }
                    bslma::TestAllocator& sa = *srcAllocatorPtr;

                    Obj& mY = *srcPtr;  const Obj& Y = mY;

                    ASSERTV(SPEC1, SPEC2, Y == W);

                    // Create target object 'X'.
                    Obj *objPtr = new (fa) Obj(&oa);
                    Obj& mX = *objPtr;  const Obj& X = gg(&mX, SPEC2);

                    ASSERTV(SPEC1, SPEC2, (X == Y) == (ti == tj));

                    // Verify initial move state is correct.
                    MoveState::Enum mState = Util::getMovedFromState(Y);
                    ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                                 || MoveState::e_NOT_MOVED == mState);

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mY));
                    ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                    // Verify the value of the object.
                    ASSERTV(SPEC1, SPEC2, X == W);

                    // Verify the move state is correct.
                    mState = Util::getMovedFromState(Y);
                    ASSERTV(mState, MoveState::e_UNKNOWN == mState
                                 || MoveState::e_MOVED   == mState);

                    // Manipulate source object 'Y' to ensure it is in a valid
                    // state and is independent of 'X'.
                    primaryManipulator(&mY, 'Z');
                    ASSERTV(SPEC1, SPEC2, Y != W);
                    ASSERTV(SPEC1, SPEC2, X == W);
                    ASSERTV(SPEC1, SPEC2, Y == Z);

                    fa.deleteObject(srcPtr);

                    // Verify subsequent manipulation of new object 'X'.
                    ASSERTV(SPEC1, SPEC2, X == W);

                    primaryManipulator(&mX, 'Z');
                    ASSERTV(SPEC1, SPEC2, X != W);
                    ASSERTV(SPEC1, SPEC2, X == Z);

                    fa.deleteObject(objPtr);

                    ASSERTV(SPEC1, SPEC2, 0 == oa.numBlocksInUse());
                    ASSERTV(SPEC1, SPEC2, 0 == sa.numBlocksInUse());
                }
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);
            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mZ(&scratch);  const Obj& Z = gg(&mZ,  SPEC1);

                // Create target object.
                Obj mX(&oa);  const Obj& X = gg(&mX,  SPEC1);

                ASSERTV(SPEC1, X == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj *mR = &(mX = bslmf::MovableRefUtil::move(mX));
                    ASSERTV(SPEC1, X == Z);
                    ASSERTV(SPEC1, mR == &X);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                ASSERTV(SPEC1, sam.isTotalSame());
                ASSERTV(SPEC1, oam.isTotalSame());
            }
        }
        // Verify no memory was ever allocated from the default allocator.
        ASSERTV(0 == da.numBlocksTotal());
    }

#if defined(BDE_BUILD_TARGET_EXC)
    if (verbose)
        printf("\nTesting move assignment (w/allocator) with exceptions.\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC1 = SPECS[ti];
            const char *const SET1  = '~' == *SPEC1 ? "null" : "set";

            if (veryVerbose) {
                printf("\nFor a source object that is %s.\n", SET1);
                P(SPEC1);
            }

            // Create control object 'W' and alternate value 'Z'.
            Obj mW;  const Obj& W = gg(&mW, SPEC1);
            Obj mZ;  gg(&mZ, "Z");

            for (int tj = 0; tj < NUM_SPECS; ++tj) {
                const char *const SPEC2 = SPECS[tj];
                const char *const SET2  = '~' == *SPEC2 ? "null" : "set";

                if (veryVerbose) {
                    printf("\n\tFor a target object that is %s.\n", SET2);
                    P(SPEC2);
                }

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator oa(   "object", veryVeryVeryVerbose);
                    bslma::TestAllocator za("different", veryVeryVeryVerbose);

                    int numPasses = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        ++numPasses;
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        // Create source object 'Y'.
                        bslma::TestAllocator *srcAllocatorPtr;

                        switch (CONFIG) {
                          case 'a': {
                            srcAllocatorPtr = &za;
                          } break;
                          case 'b': {
                            srcAllocatorPtr = &oa;
                          } break;
                          default: {
                            ASSERTV(CONFIG, !"Bad allocator config.");
                          } return;                                   // RETURN
                        }
                        bslma::TestAllocator& sa = *srcAllocatorPtr;

                        Obj mY(&sa);  const Obj& Y = gg(&mY, SPEC1);

                        ASSERTV(SPEC1, SPEC2, Y == W);

                        // Create target object 'X'.
                        Obj mX(&oa);  const Obj& X = gg(&mX, SPEC2);

                        ASSERTV(SPEC1, SPEC2, (X == Y) == (ti == tj));

                        // Verify initial move state is correct.
                        MoveState::Enum mState = Util::getMovedFromState(Y);
                        ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                                     || MoveState::e_NOT_MOVED == mState);

                        Obj *mR = &(mX = bslmf::MovableRefUtil::move(mY));
                        ASSERTV(SPEC1, SPEC2, mR, &mX, mR == &mX);

                        // Verify the value of the object.
                        ASSERTV(SPEC1, SPEC2, X == W);

                        // Verify the move state is correct.
                        mState = Util::getMovedFromState(Y);
                        ASSERTV(mState, MoveState::e_UNKNOWN == mState
                                     || MoveState::e_MOVED   == mState);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    ASSERTV(0 == ti || numPasses > 1);
                }
            }
        }
    }
#endif
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase20_withoutAllocator()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTOR (without allocator)
    //
    // Concerns:
    //: 1 The newly created object has the same value (using the equality
    //:   operator) as that of the original object before the call.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value, including the null state.
    //:
    //: 3 The move constructor is called on the contained value if the source
    //:   value is not null.
    //:
    //: 4 The original object is left in a valid state.
    //:
    //: 5 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //
    // Plan:
    //: 1 Specify a set 'S' of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used sequentially in
    //:   the following tests; for each entry, create a control object.   (C-2)
    //:
    //: 2 For each of the object values (P-1), verify the following:
    //:
    //:   1 Verify the newly created object has the same value as that of the
    //:     original object before the call to the move constructor (control
    //:     value).                                                       (C-1)
    //:
    //:   2 Ensure that the move state reflects a call to the move constructor
    //:     for the contained value if the source value was not null.     (C-3)
    //:
    //:   3 Manipulate the original object (after the move construction) to
    //:     ensure it is in a valid state.                                (C-4)
    //:
    //:   4 Destroy the original object, and then manipulate the newly created
    //:     object to ensure that it is in a valid state.                 (C-5)
    //
    // Testing:
    //   NullableValue(NullableValue&& original);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "~",
        "A",
        "B",
        "C"
    };
    const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    if (verbose)
        printf("\nTesting move constructor (no allocator).\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC = SPECS[ti];
            const char *const SET  = '~' == *SPEC ? "null" : "set";

            if (veryVerbose) {
                printf("\nFor an object that is %s.\n", SET);
                P(SPEC);
            }

            // Create control object 'W' and alternate value 'Z'.
            Obj mW;  const Obj& W = gg(&mW, SPEC);
            Obj mZ;  const Obj& Z = gg(&mZ, "Z");

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            // Create source object 'Y'.
            Obj *srcPtr = new (fa) Obj();
            Obj& mY = *srcPtr;  const Obj& Y = gg(&mY, SPEC);

            ASSERTV(SPEC, Y == W);

            // Verify initial move state is correct.
            MoveState::Enum mState = Util::getMovedFromState(Y);
            ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                         || MoveState::e_NOT_MOVED == mState);

            Obj *objPtr = new (fa) Obj(MoveUtil::move(mY));
            Obj& mX = *objPtr;  const Obj& X = mX;

            // Verify the value of the object.
            ASSERTV(SPEC, X == W);

            // Verify the move state is correct.
            mState = Util::getMovedFromState(Y);
            ASSERTV(mState, MoveState::e_UNKNOWN == mState
                         || MoveState::e_MOVED   == mState);

            // Manipulate source object 'Y' to ensure it is in a valid state
            // and is independent of 'X'.
            primaryManipulator(&mY, 'Z');
            ASSERTV(SPEC, Y != W);
            ASSERTV(SPEC, X == W);
            ASSERTV(SPEC, Y == Z);

            fa.deleteObject(srcPtr);

            // Verify subsequent manipulation of new object 'X'.
            ASSERTV(SPEC, X == W);

            primaryManipulator(&mX, 'Z');
            ASSERTV(SPEC, X != W);
            ASSERTV(SPEC, X == Z);

            fa.deleteObject(objPtr);

            // Verify no memory was ever allocated.
            ASSERTV(SPEC, 0 == da.numBlocksTotal());
        }
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase20_withAllocator()
{
    // ------------------------------------------------------------------------
    // TESTING MOVE CONSTRUCTOR (with allocator)
    //
    // Concerns:
    //: 1 The newly created object has the same value (using the equality
    //:   operator) as that of the original object before the call.
    //:
    //: 2 All internal representations of a given value can be used to create a
    //:   new object of equivalent value.
    //:
    //: 3 The allocator is propagated to the newly created object if (and only
    //:   if) no allocator is specified in the call to the move constructor.
    //:
    //: 4 The move constructor is called on the contained value if the source
    //:   value is not null.
    //:
    //: 5 The original object is always left in a valid state and the allocator
    //:   address held by the original object is unchanged.
    //:
    //: 6 Subsequent changes to or destruction of the original object have no
    //:   effect on the move-constructed object and vice-versa.
    //:
    //: 7 The object has its internal memory management system hooked up
    //:   properly so that *all* internally allocated memory draws from a
    //:   user-supplied allocator whenever one is specified.
    //:
    //: 8 Every object releases any allocated memory at destruction.
    //
    //: 9 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 Specify a set 'S' of object values with substantial and varied
    //:   differences, ordered by increasing length, to be used sequentially in
    //:   the following tests; for each entry, create a control object.   (C-2)
    //:
    //: 2 Call the move constructor to create the container in all relevant use
    //:   cases involving the allocator: 1) no allocator passed in, 2) a '0' is
    //    explicitly passed in as the allocator argument, 3) the same allocator
    //:   as that of the original object is explicitly passed in, and 4) a
    //:   different allocator than that of the original object is passed in.
    //:
    //: 3 For each of the object values (P-1) and for each configuration (P-2),
    //:   verify the following:
    //:
    //:   1 Verify the newly created object has the same value as that of the
    //:     original object before the call to the move constructor (control
    //:     value).                                                       (C-1)
    //:
    //:   2 Ensure that the move state reflects a call to the move constructor
    //:     for the contained value if the source value was not null.     (C-4)
    //:
    //:   3 Ensure that the new original, and control object continue to have
    //:     the correct allocator and that all memory allocations come from the
    //:     appropriate allocator.                                      (C-3,7)
    //:
    //:   7 Manipulate the original object (after the move construction) to
    //:     ensure it is in a valid state, destroy it, and then manipulate the
    //:     newly created object to ensure that it is in a valid state. (C-5,6)
    //:
    //:   8 Verify all memory is released when the object is destroyed.   (C-8)
    //:
    //: 4 Perform tests in the presence of exceptions during memory allocations
    //:   using a 'bslma::TestAllocator' and varying its *allocation* *limit*.
    //:                                                                   (C-9)
    //
    // Testing:
    //   NullableValue(NullableValue&& original);
    //   NullableValue(NullableValue&& original, allocator);
    // ------------------------------------------------------------------------

    static const char *SPECS[] = {
        "~",
        "A",
        "B",
        "C"
    };
    const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

    if (verbose)
        printf("\nTesting both versions of move constructor (with alloc).\n");
    {
        for (int ti = 0; ti < NUM_SPECS; ++ti) {
            const char *const SPEC = SPECS[ti];
            const char *const SET  = '~' == *SPEC ? "null" : "set";

            if (veryVerbose) {
                printf("\nFor an object that is %s.\n", SET);
                P(SPEC);
            }

            // Create control object 'W' and alternate value 'Z' with a scratch
            // allocator.

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mW(&scratch);  const Obj& W = gg(&mW, SPEC);
            Obj mZ(&scratch);  const Obj& Z = gg(&mZ, "Z");

            for (char cfg = 'a'; cfg <= 'd'; ++cfg) {

                const char CONFIG = cfg;  // how we specify the allocator

                bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);
                bslma::TestAllocator za("different", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                // Create source object 'Y'.
                Obj *srcPtr = new (fa) Obj(&sa);
                Obj& mY = *srcPtr;  const Obj& Y = gg(&mY, SPEC);

                ASSERTV(SPEC, CONFIG, Y == W);

                // Verify initial move state is correct.
                MoveState::Enum mState = Util::getMovedFromState(Y);
                ASSERTV(mState, MoveState::e_UNKNOWN   == mState
                             || MoveState::e_NOT_MOVED == mState);

                bslma::TestAllocatorMonitor oam(&da), sam(&sa);

                Obj                  *objPtr;
                bslma::TestAllocator *objAllocatorPtr;
                bslma::TestAllocator *othAllocatorPtr;

                switch (CONFIG) {
                  case 'a': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mY));
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'b': {
                    oam.reset(&da);
                    objPtr = new (fa) Obj(MoveUtil::move(mY),
                                          (bslma::Allocator *)0);
                    objAllocatorPtr = &da;
                    othAllocatorPtr = &za;
                  } break;
                  case 'c': {
                    oam.reset(&sa);
                    objPtr = new (fa) Obj(MoveUtil::move(mY), &sa);
                    objAllocatorPtr = &sa;
                    othAllocatorPtr = &da;
                  } break;
                  case 'd': {
                    oam.reset(&za);
                    objPtr = new (fa) Obj(MoveUtil::move(mY), &za);
                    objAllocatorPtr = &za;
                    othAllocatorPtr = &da;
                  } break;
                  default: {
                    ASSERTV(CONFIG, !"Bad allocator config.");
                  } return;                                           // RETURN
                }

                bslma::TestAllocator&  oa = *objAllocatorPtr;
                bslma::TestAllocator& noa = *othAllocatorPtr;

                Obj& mX = *objPtr;  const Obj& X = mX;

                // Verify the value of the object.
                ASSERTV(SPEC, CONFIG, X == W);

                // Verify the move state is correct.
                mState = Util::getMovedFromState(Y);
                ASSERTV(mState, MoveState::e_UNKNOWN == mState
                             || MoveState::e_MOVED   == mState);

                // Verify that 'X', 'Y', and 'Z' have the correct allocator.
                // (which we can't, since its not exposed by the interface)

                // Verify no allocation from the non-object allocator and that
                // the object allocator is hooked up.
                ASSERTV(SPEC, CONFIG, 0 == noa.numBlocksTotal());
                ASSERTV(SPEC, CONFIG, 0 < oa.numBlocksTotal() || '~' == *SPEC);

                // Manipulate source object 'Y' to ensure it is in a valid
                // state and is independent of 'X'.
                primaryManipulator(&mY, 'Z');
                ASSERTV(SPEC, CONFIG, Y != W);
                ASSERTV(SPEC, CONFIG, Y == Z);
                ASSERTV(SPEC, CONFIG, X == W);

                fa.deleteObject(srcPtr);

                ASSERTV(SPEC, CONFIG, X == W);

                // Verify subsequent manipulation of new object 'X'.
                primaryManipulator(&mX, 'Z');
                ASSERTV(SPEC, X != W);
                ASSERTV(SPEC, X == Z);

                fa.deleteObject(objPtr);

                // Verify all memory is released on object destruction.
                ASSERTV(SPEC, 0 == fa.numBlocksInUse());
                ASSERTV(SPEC, 0 == da.numBlocksInUse());
                ASSERTV(SPEC, 0 == sa.numBlocksInUse());
                ASSERTV(SPEC, 0 == za.numBlocksInUse());
            }
        }
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase19()
{
    // ------------------------------------------------------------------------
    // TESTING: Hashing
    //
    // Concerns:
    //: 1 Hashing a value with a null value is equivalent to appending 'false'
    //:   to the hash.
    //:
    //: 2 Hashing a value with a nullable value is equivalent to appending
    //:   'true' to the hash followed by the value.
    //
    // Plan:
    //: 1 Create a null nullable value and verify that hashing it yields the
    //:   same value as hashing 'false'.
    //:
    //: 2 Create a non-null nullable value for a series of test values and
    //:   verify that hashing it produces the same result as hashing 'true' and
    //:   then the test values themselves.
    //
    // Testing:
    //   void hashAppend(HASHALG& hashAlg, NullableValue<TYPE>& input);
    // ------------------------------------------------------------------------

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) {
        cout << "\tVerify hashing null nullable values is equivalent to\n"
                "\tappending 'false' to the hash.\n";
    }
    {
        ObjWithAllocator object(&oa);

        Obj& x = object.object();  const Obj& X = x;

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        const size_t hashValue_1 = bslh::Hash<>()(X);
        const size_t hashValue_2 = bslh::Hash<>()(false);
        ASSERTV(hashValue_1, hashValue_2, hashValue_1 == hashValue_2);
    }

    if (veryVerbose) {
        cout << "\tVerify hashing non-null nullable values is equivalent to\n"
                "\tappending 'true' to the hash followed by the value.\n";
    }
    {
        ObjWithAllocator object(&oa);

        Obj& x = object.object();  const Obj& X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            for (int j = 0; j < NUM_VALUES; ++j) {

                ASSERT(0 == oa.numBlocksInUse());
                ASSERT(0 == da.numBlocksInUse());

                x = VALUES[i];

                bslma::TestAllocatorMonitor oam(&oa);

                bool areSame = i == j;

                bslh::DefaultHashAlgorithm hasher;

                const size_t hashValue_1 = bslh::Hash<>()(X);
                hashAppend(hasher, true);
                hashAppend(hasher, VALUES[j]);
                const size_t hashValue_2 =
                                     static_cast<size_t>(hasher.computeHash());

                ASSERTV(areSame, hashValue_1, hashValue_2,
                        areSame == (hashValue_2 == hashValue_1));

                ASSERT(oam.isInUseSame());
                ASSERT(0 == da.numBlocksInUse());

                x.reset();
            }
        }
        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase18_withoutAllocator()
{
    // TESTING: 'makeValueInplace' for Non-Allocating Types
    //
    //: 1 We use 'bsltf::TemplateTestFacility::create' method to create a
    //:   sequence of N distinct values of 'TEST_TYPE'.  Note that N is the
    //:   maximum number of parameters we are supporting in our
    //:   simulation of C++11 variadic templates.
    //
    //: 2 Using the non-allocating helper class template, 'TmvipSa', and the
    //:   values created in P-1, we use 'makeValueInplace' to set the value of
    //:   a nullable object, repeating for '[0 .. N]' arguments.
    //:
    //: 3 For each use of 'makeValueInplace' we:
    //:   1 Check the state (i.e., 'isNull') of the nullable object.
    //:   2 Compare the return value to that returned by the 'value' method.
    //:   3 Use the class methods of 'TmvipSa' to confirm that:
    //:     1 The expected constructor (for the number of arguments) was
    //:       called.
    //:     2 The number of objects destroyed matches the expected value.
    //:
    //: 4 Steps P-2 and P-3 are repeated for nullable objects that are
    //:   initially null, and nullable objects that are initially not null.
    //:
    //: 5 The entire test case checks that neither the default nor global
    //:   allocator is used.

    if (veryVerbose) { cout << "\ttestCase18_withoutAllocator: "
                               "Non-Allocating Type: "
                            << typeid(TEST_TYPE).name() << endl;
                     }

    BSLMF_ASSERT(!bslma::UsesBslmaAllocator<TEST_TYPE>::value);

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocatorMonitor  dam(&da);
    bslma::DefaultAllocatorGuard dag(&da);

    typedef TmvipSa<TEST_TYPE>                       Helper;
    typedef bdlb::NullableValue<TmvipSa<TEST_TYPE> > Obj;
    typedef bsltf::TemplateTestFacility              Util;

    ASSERT(!bslma::UsesBslmaAllocator<Obj>::value);
    ASSERT(!(bsl::uses_allocator<Obj, bsl::allocator<char> >::value));

    const TEST_TYPE v1 = Util::create<TEST_TYPE>(1);
    const TEST_TYPE v2 = Util::create<TEST_TYPE>(2);
    const TEST_TYPE v3 = Util::create<TEST_TYPE>(3);
    const TEST_TYPE v4 = Util::create<TEST_TYPE>(4);
    const TEST_TYPE v5 = Util::create<TEST_TYPE>(5);

    ASSERT(true == dam.isTotalSame());

    for (int numParams = 0; numParams <= MAX_NUM_PARAMS; ++numParams) {

        if (veryVeryVerbose) { T_ T_ P_(numParams) Q(Null Object) }

        Obj mX;  const Obj& X = mX;
        ASSERT(X.isNull());

        Helper *addr;
        Helper::resetDtorCount();
        {
            switch (numParams) {
              case 0: addr = &mX.makeValueInplace();                   break;
              case 1: addr = &mX.makeValueInplace(v1);                 break;
              case 2: addr = &mX.makeValueInplace(v1, v2);             break;
              case 3: addr = &mX.makeValueInplace(v1, v2, v3);         break;
              case 4: addr = &mX.makeValueInplace(v1, v2, v3, v4);     break;
              case 5: addr = &mX.makeValueInplace(v1, v2, v3, v4, v5); break;
              default:
                ASSERT(!"Too many parameters.");
                return;                                               // RETURN
            }
        }
        ASSERT(false     == X.isNull());
        ASSERT(numParams == Helper::ctorCalled());
        ASSERT(addr      == &X.value());
        ASSERT(0         == Helper::dtorCount());

        if (veryVeryVerbose) { T_ T_ P_(numParams) Q(non-Null Object) }

        ASSERT(false     == X.isNull());
        {
            switch (numParams) {
              case 0: addr = &mX.makeValueInplace();                   break;
              case 1: addr = &mX.makeValueInplace(v1);                 break;
              case 2: addr = &mX.makeValueInplace(v1, v2);             break;
              case 3: addr = &mX.makeValueInplace(v1, v2, v3);         break;
              case 4: addr = &mX.makeValueInplace(v1, v2, v3, v4);     break;
              case 5: addr = &mX.makeValueInplace(v1, v2, v3, v4, v5); break;
              default:
                ASSERT(!"Too many parameters.");
                return;                                               // RETURN
            }
        }
        ASSERT(false     == X.isNull());
        ASSERT(numParams == Helper::ctorCalled());
        ASSERT(addr      == &X.value());
        ASSERT(1         == Helper::dtorCount());
    }

    ASSERT(dam.isTotalSame());
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase18_withAllocator()
{
    // TESTING: 'makeValueInplace' for Allocating Types
    //
    //: 1 We use 'bsltf::TemplateTestFacility::create' method to create a
    //:   sequence of N distinct values of 'TEST_TYPE'.  Note that N is the
    //:   maximum number of parameters we are supporting in our simulation of
    //:   C++11 variadic templates.
    //
    //: 2 Using the allocating helper class template, 'TmvipAa', and the values
    //:   created in P-1, we use 'makeValueInplace' to set the value of a
    //:   nullable object created with a test allocator, repeating for
    //:   '[0 .. N]' arguments.
    //:
    //: 3 For each use of 'makeValueInplace' we:
    //:   1 Check the state (i.e., 'isNull') of the nullable object.
    //:   2 Compare the return value to that returned by the 'value' method.
    //:   3 Use the class methods of 'TmvipSa' to confirm that:
    //:     1 The expected constructor (for the number of arguments) was
    //:       called.
    //:     2 The number of objects destroyed matches the expected value.
    //:   4 The object allocator -- and no other --is used.
    //:
    //: 4 Steps P-2 and P-3 are repeated for nullable objects that are
    //:   initially null, and nullable objects that are initially not null.
    //:
    //: 5 Step P-4 is repeated using the TestAllocator to throw an exception
    //:   for each of the many allocations triggered by 'makeValueInplace'.
    //:   The state of the nullable object is checked ('true == isNull'), and
    //:   test allocator monitors are used to confirm that no memory is leaked.
    //:
    //: 6 The entire test case checks that neither the default nor global
    //:   allocator is used.

    if (veryVerbose) { cout << "\ttestCase18_withAllocator: "
                               "Allocating Type: "
                            << typeid(TEST_TYPE).name() << endl;
                     }

    BSLMF_ASSERT(bslma::UsesBslmaAllocator<TEST_TYPE>::value);

    typedef TmvipAa<TEST_TYPE>                       Helper;
    typedef bdlb::NullableValue<TmvipAa<TEST_TYPE> > Obj;
    typedef bsltf::TemplateTestFacility              Util;

    ASSERT(bslma::UsesBslmaAllocator<Obj>::value);
    ASSERT((bsl::uses_allocator<Obj, bsl::allocator<char> >::value));

    const TEST_TYPE v1 = Util::create<TEST_TYPE>(1);
    const TEST_TYPE v2 = Util::create<TEST_TYPE>(2);
    const TEST_TYPE v3 = Util::create<TEST_TYPE>(3);
    const TEST_TYPE v4 = Util::create<TEST_TYPE>(4);
    const TEST_TYPE v5 = Util::create<TEST_TYPE>(5);

    // Set the default allocator and capture state after the possible copy of
    // the above return values 'v1', ..., 'v5'.

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocatorMonitor  dam(&da);
    bslma::DefaultAllocatorGuard dag(&da);

    bslma::TestAllocator ta("makeValueInplace", veryVeryVeryVerbose);

    for (int numParams = 0; numParams <= MAX_NUM_PARAMS; ++numParams) {

        Obj mX(&ta);  const Obj& X = mX;
        if (veryVeryVerbose) { T_ T_ P_(numParams)
                                                 Q(Null Object No Exceptions) }
        ASSERT(X.isNull());

        Helper *addr;
        Helper::resetDtorCount();
        {
            bslma::TestAllocatorMonitor tam(&ta);
            switch (numParams) {
              case 0: addr = &mX.makeValueInplace();                   break;
              case 1: addr = &mX.makeValueInplace(v1);                 break;
              case 2: addr = &mX.makeValueInplace(v1, v2);             break;
              case 3: addr = &mX.makeValueInplace(v1, v2, v3);         break;
              case 4: addr = &mX.makeValueInplace(v1, v2, v3, v4);     break;
              case 5: addr = &mX.makeValueInplace(v1, v2, v3, v4, v5); break;
              default:
                ASSERT(!"Too many parameters.");
                return;                                               // RETURN
            }
            ASSERT(true == tam.isTotalUp());
            ASSERT(true == tam.isInUseUp());  // Initially null, now not-null.
        }
        ASSERT(false     == X.isNull());
        ASSERT(numParams == Helper::ctorCalled());
        ASSERT(addr      == &X.value());
        ASSERT(0         == Helper::dtorCount());

        if (veryVeryVerbose) { T_ T_ P_(numParams)
                                             Q(non-Null Object No exceptions) }

        ASSERT(!X.isNull());
        {
            bslma::TestAllocatorMonitor tam(&ta);
            switch (numParams) {
              case 0: addr = &mX.makeValueInplace();                   break;
              case 1: addr = &mX.makeValueInplace(v1);                 break;
              case 2: addr = &mX.makeValueInplace(v1, v2);             break;
              case 3: addr = &mX.makeValueInplace(v1, v2, v3);         break;
              case 4: addr = &mX.makeValueInplace(v1, v2, v3, v4);     break;
              case 5: addr = &mX.makeValueInplace(v1, v2, v3, v4, v5); break;
              default:
                ASSERT(!"Too many parameters.");
                return;                                               // RETURN
            }
            ASSERT(tam.isTotalUp());
        }
        ASSERT(false     == X.isNull());
        ASSERT(numParams == Helper::ctorCalled());
        ASSERT(addr      == &X.value());
        ASSERT(1         == Helper::dtorCount());

        if (veryVeryVerbose) { T_ T_ P_(numParams)
                                             Q(Null Object w & w/o exception) }

        mX.reset();

        ASSERT(X.isNull());
        Helper::resetDtorCount();

        int loopCount = 0;

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
            bslma::TestAllocatorMonitor tam(&ta);

            ++loopCount;

            if (veryVeryVerbose) { T_ T_ T_ P_(loopCount) P(X.isNull()) }

            ASSERT(X.isNull());

            switch (numParams) {
              case 0: addr = &mX.makeValueInplace();                   break;
              case 1: addr = &mX.makeValueInplace(v1);                 break;
              case 2: addr = &mX.makeValueInplace(v1, v2);             break;
              case 3: addr = &mX.makeValueInplace(v1, v2, v3);         break;
              case 4: addr = &mX.makeValueInplace(v1, v2, v3, v4);     break;
              case 5: addr = &mX.makeValueInplace(v1, v2, v3, v4, v5); break;
              default:
                ASSERT(!"Too many parameters.");
            }
            ASSERT(0    <  loopCount);
            ASSERT(true == tam.isTotalUp());
            ASSERT(true == tam.isInUseUp());  // Initially null, now not-null.
            ASSERT(0    == Helper::dtorCount());  // no reset needed
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERT(false     == X.isNull());
        ASSERT(numParams == Helper::ctorCalled());
        ASSERT(addr      == &X.value());

        if (veryVeryVerbose) { T_ T_ P_(numParams)
                                         Q(non-Null Object w & w/o exception) }

        ASSERT(!X.isNull());
        Helper::resetDtorCount();
        loopCount = 0;

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(ta) {
            bslma::TestAllocatorMonitor tam(&ta);

            ++loopCount;

            if (veryVeryVerbose) { T_ T_ T_ P_(loopCount) P(X.isNull()) }

            if (1 == loopCount) {
                ASSERT(!X.isNull());
            } else {
                ASSERT( X.isNull());
            }

            switch (numParams) {
              case 0: addr = &mX.makeValueInplace();                   break;
              case 1: addr = &mX.makeValueInplace(v1);                 break;
              case 2: addr = &mX.makeValueInplace(v1, v2);             break;
              case 3: addr = &mX.makeValueInplace(v1, v2, v3);         break;
              case 4: addr = &mX.makeValueInplace(v1, v2, v3, v4);     break;
              case 5: addr = &mX.makeValueInplace(v1, v2, v3, v4, v5); break;
              default:
                ASSERT(!"Too many parameters.");
            }
            ASSERT(0    <  loopCount);
            ASSERT(true == tam.isTotalUp());
            ASSERT(1    == Helper::dtorCount());  // reset of prior object

        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERT(false     == X.isNull());
        ASSERT(numParams == Helper::ctorCalled());
        ASSERT(addr      == &X.value());
    }

    ASSERT(dam.isTotalSame());
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase14()
{
    // ------------------------------------------------------------------------
    // TESTING: 'T valueOr(const T&)'
    //
    // Concerns:
    //: 1 'valueOr' returns the supplied value if the nullable value is null.
    //:
    //: 2 'valueOr' returns the contained value if the nullable value is
    //:   non-null.
    //:
    //: 3 'valueOr' returns by value.
    //:
    //: 4 'valueOr' can be called on a 'const' object.
    //
    // Plan:
    //: 1 Create a member-function pointer matching the expected signature, and
    //:   assign 'valueOr' to the function.  (C-3)
    //:
    //: 2 Call 'valueOr' for a null nullable value and verify that it returns
    //:   the supplied value.  (C-2)
    //:
    //: 3 For a series of test values, assign the nullable value to the test
    //:   value, call 'valueOr', and verify the return value is the test value.
    //:   (C-2, C-4)
    //
    // Testing:
    //   TYPE valueOr(const TYPE& value) const;
    // ------------------------------------------------------------------------

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) {
        cout << "\tCompile-time verify the function returns by value.\n";
    }
    {
        typedef TEST_TYPE (Obj::*MemberFunction)(const TEST_TYPE& type) const;

        MemberFunction memberFunction = &Obj::valueOr;
        (void)&memberFunction;
    }

    if (veryVerbose) cout << "\tVerify null nullable values return 0.\n";
    {
        ObjWithAllocator object(&oa);

        Obj& x = object.object();  const Obj& X = x;

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        for (int i = 0; i < NUM_VALUES; ++i) {

            ASSERT(VALUES[i] == x.valueOr(VALUES[i]));
            ASSERT(VALUES[i] == X.valueOr(VALUES[i]));

            ASSERT(true == X.isNull());

            ASSERT(0 == oa.numBlocksInUse());
        }
    }

    if (veryVerbose) {
        cout << "\tVerify non-null nullable values return underlying value.\n";
    }
    {
        ObjWithAllocator object(&oa);

        Obj& x = object.object();  const Obj& X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == oa.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());

            ASSERT(VALUES[i] == x.valueOr(VALUES[i]));
            ASSERT(VALUES[i] == X.valueOr(VALUES[i]));

            x = VALUES[0];

            bslma::TestAllocatorMonitor oam(&oa);
            bslma::TestAllocatorMonitor dam(&da);

            ASSERT(VALUES[0] == x.valueOr(VALUES[i]));
            ASSERT(VALUES[0] == X.valueOr(VALUES[i]));

            ASSERT(i == 0 || VALUES[i] != x.valueOr(VALUES[i]));
            ASSERT(i == 0 || VALUES[i] != X.valueOr(VALUES[i]));

            bool usesAllocator = bslma::UsesBslmaAllocator<TEST_TYPE>::value;
            ASSERT(usesAllocator ? dam.isTotalUp() : dam.isTotalSame());
            ASSERT(oam.isInUseSame());

            x.reset();
        }

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase15()
{
    // ------------------------------------------------------------------------
    // TESTING: 'const T *valueOr(const T *)'
    //
    // Concerns:
    //: 1 'valueOr' returns the supplied value if the nullable value is null.
    //:
    //: 2 'valueOr' returns the contained value if the nullable value is
    //:   non-null.
    //:
    //: 3 'valueOr' returns an address.
    //:
    //: 4 'valueOr' can be called on a 'const' object.
    //:
    //: 5 No memory is requested of any allocator (global, default, this
    //:   object, supplied object).
    //
    // Plan:
    //: 1 Create a member-function pointer matching the expected signature,
    //:   and assign 'valueOr' to the function.  (C-3)
    //:
    //: 2 Call 'valueOr' for a null nullable value and verify that it
    //:   returns a reference to the supplied value.  (C-2)
    //:
    //: 3 For a series of test values, assign the nullable value to the test
    //:   value, call 'valueOr', and verify the return value is a reference to
    //:   the contained value.  (C-2, C-4)
    //
    // Testing:
    //   const TYPE *valueOr(const TYPE *value) const;
    // ------------------------------------------------------------------------

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) {
        cout << "\tCompile-time verify the function returns an address.\n";
    }
    {
        typedef const TEST_TYPE *
                              (Obj::*MemberFunction)(const TEST_TYPE *) const;

        MemberFunction memberFunction = &Obj::valueOr;
        (void)&memberFunction;
    }

    if (veryVerbose) cout << "\tVerify null nullable values return 0.\n";
    {
        ObjWithAllocator object(&oa);

        Obj& x = object.object();  const Obj& X = x;

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        for (int i = 0; i < NUM_VALUES; ++i) {

            ASSERT(VALUES[i] == *x.valueOr(&VALUES[i]));
            ASSERT(VALUES[i] == *X.valueOr(&VALUES[i]));

            ASSERT(&VALUES[i] == x.valueOr(&VALUES[i]));
            ASSERT(&VALUES[i] == X.valueOr(&VALUES[i]));

            ASSERT(true == X.isNull());

            ASSERT(0 == oa.numBlocksInUse());
        }
    }

    if (veryVerbose) {
        cout << "\tVerify non-null nullable values return underlying value.\n";
    }
    {
        ObjWithAllocator object(&oa);

        Obj& x = object.object();  const Obj& X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == oa.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());

            ASSERT(VALUES[i] == *x.valueOr(&VALUES[i]));
            ASSERT(VALUES[i] == *X.valueOr(&VALUES[i]));

            ASSERT(&VALUES[i] == x.valueOr(&VALUES[i]));
            ASSERT(&VALUES[i] == X.valueOr(&VALUES[i]));

            x = VALUES[0];

            bslma::TestAllocatorMonitor oam(&oa);

            ASSERT(VALUES[0] == *x.valueOr(&VALUES[i]));
            ASSERT(VALUES[0] == *X.valueOr(&VALUES[i]));

            ASSERT(i == 0 || VALUES[i] != *x.valueOr(&VALUES[i]));
            ASSERT(i == 0 || VALUES[i] != *X.valueOr(&VALUES[i]));

            ASSERT(&VALUES[i] != x.valueOr(&VALUES[i]));
            ASSERT(&VALUES[i] != X.valueOr(&VALUES[i]));

            ASSERT(&X.value() == x.valueOr(&VALUES[i]));
            ASSERT(&X.value() == X.valueOr(&VALUES[i]));

            ASSERT(oam.isInUseSame());
            ASSERT(0 == da.numBlocksInUse());

            x.reset();
        }

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase16()
{
    // ------------------------------------------------------------------------
    // TESTING: 'valueOrNull'
    //
    // Concerns:
    //: 1 'valueOrNull' returns 0 if the nullable value is null.
    //:
    //: 2 'valueOrNull' returns the address of the non-modifiable value if the
    //:   nullable value is non-null.
    //:
    //: 3 The returned address, if not 0, remains valid until the nullable
    //:   value is destroyed.
    //:
    //: 4 No memory allocation is performed.
    //:
    //: 5 'valueOrNull' can be called on a 'const' object.
    //
    // Plan:
    //: 1 Call 'valueOrNull' for a null nullable value and verify that it
    //:   returns 0.  (C-1)
    //:
    //: 2 For a series of test values, assign the nullable value to the test
    //:   value, call 'valueOrNull', and verify the return value.  (C-2..5)
    //
    // Testing:
    //   const TYPE *valueOrNull() const;
    // ------------------------------------------------------------------------

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) cout << "\tVerify null nullable values return 0.\n";
    {
        ObjWithAllocator object(&oa);

        Obj& x = object.object();  const Obj& X = x;

        ASSERT(0 == x.valueOrNull());
        ASSERT(0 == X.valueOrNull());

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        x = VALUES[0];

        ASSERT(0 != x.valueOrNull());
        ASSERT(0 != X.valueOrNull());

        x.reset();

        ASSERT(0 == x.valueOrNull());
        ASSERT(0 == X.valueOrNull());

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());
    }

    if (veryVerbose) {
        cout << "\tVerify non-null nullable values return underlying value.\n";
    }
    {
        ObjWithAllocator object(&oa);

        Obj& x = object.object();  const Obj& X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == x.valueOrNull());
            ASSERT(0 == X.valueOrNull());

            ASSERT(0 == oa.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());

            x = VALUES[i];

            ASSERT(0 != x.valueOrNull());
            ASSERT(0 != X.valueOrNull());

            bslma::TestAllocatorMonitor oam(&oa);

            ASSERT(X.valueOrNull() == x.valueOrNull());
            ASSERT(&X.value()      == X.valueOrNull());

            const TEST_TYPE *valuePtr = X.valueOrNull();

            ASSERT(VALUES[i] == *valuePtr);

            ASSERT(oam.isInUseSame());
            ASSERT(0 == da.numBlocksInUse());

            x.reset();
        }
    }
}

template <class TEST_TYPE>
void TestDriver<TEST_TYPE>::testCase25()
{
    // ------------------------------------------------------------------------
    // TESTING 'const T *addressOr(const T *)'
    //
    // Concerns:
    //: 1 'addressOr' returns the supplied value if the nullable value is null.
    //:
    //: 2 'addressOr' returns the contained value if the nullable value is
    //:   non-null.
    //:
    //: 3 'addressOr' returns an address.
    //:
    //: 4 'addressOr' can be called on a 'const' object.
    //:
    //: 5 No memory is requested of any allocator (global, default, this
    //:   object, supplied object).
    //
    // Plan:
    //: 1 Create a member-function pointer matching the expected signature,
    //:   and assign 'addressOr' to the function.  (C-3)
    //:
    //: 2 Call 'addressOr' for a null nullable value and verify that it
    //:   returns a reference to the supplied value.  (C-2)
    //:
    //: 3 For a series of test values, assign the nullable value to the test
    //:   value, call 'addressOr', and verify the return value is a reference
    //:   to the contained value.  (C-2, C-4)
    //
    // Testing:
    //   const TYPE *addressOr(const TYPE *address) const;
    // ------------------------------------------------------------------------

    const TestValues VALUES;
    const int        NUM_VALUES = static_cast<int>(VALUES.size());

    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

    bslma::DefaultAllocatorGuard dag(&da);

    if (veryVerbose) {
        cout << "\tCompile-time verify the function returns an address.\n";
    }
    {
        typedef const TEST_TYPE *
                               (Obj::*MemberFunction)(const TEST_TYPE *) const;

        MemberFunction memberFunction = &Obj::addressOr;
        (void)&memberFunction;
    }

    if (veryVerbose) cout << "\tVerify null nullable values return 0.\n";
    {
        ObjWithAllocator object(&oa);
        Obj&             x = object.object();
        const Obj&       X = x;

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());

        for (int i = 0; i < NUM_VALUES; ++i) {

            ASSERT(VALUES[i] == *x.addressOr(&VALUES[i]));
            ASSERT(VALUES[i] == *X.addressOr(&VALUES[i]));

            ASSERT(&VALUES[i] == x.addressOr(&VALUES[i]));
            ASSERT(&VALUES[i] == X.addressOr(&VALUES[i]));

            ASSERT(true == X.isNull());

            ASSERT(0 == oa.numBlocksInUse());
        }
    }

    if (veryVerbose) {
        cout << "\tVerify non-null nullable values return underlying value.\n";
    }
    {
        ObjWithAllocator object(&oa);
        Obj&             x = object.object();
        const Obj&       X = x;

        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(0 == oa.numBlocksInUse());
            ASSERT(0 == da.numBlocksInUse());

            ASSERT(VALUES[i] == *x.addressOr(&VALUES[i]));
            ASSERT(VALUES[i] == *X.addressOr(&VALUES[i]));

            ASSERT(&VALUES[i] == x.addressOr(&VALUES[i]));
            ASSERT(&VALUES[i] == X.addressOr(&VALUES[i]));

            x = VALUES[0];

            bslma::TestAllocatorMonitor oam(&oa);

            ASSERT(VALUES[0] == *x.addressOr(&VALUES[i]));
            ASSERT(VALUES[0] == *X.addressOr(&VALUES[i]));

            ASSERT(i == 0 || VALUES[i] != *x.addressOr(&VALUES[i]));
            ASSERT(i == 0 || VALUES[i] != *X.addressOr(&VALUES[i]));

            ASSERT(&VALUES[i] != x.addressOr(&VALUES[i]));
            ASSERT(&VALUES[i] != X.addressOr(&VALUES[i]));

            ASSERT(&X.value() == x.addressOr(&VALUES[i]));
            ASSERT(&X.value() == X.addressOr(&VALUES[i]));

            ASSERT(oam.isInUseSame());
            ASSERT(0 == da.numBlocksInUse());

            x.reset();
        }

        ASSERT(0 == oa.numBlocksInUse());
        ASSERT(0 == da.numBlocksInUse());
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
      case 28: {
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
// The following snippets of code illustrate use of this component:
//
// First, create a nullable 'int' object:
//..
    bdlb::NullableValue<int> nullableInt;
    ASSERT( nullableInt.isNull());
//..
// Next, give the 'int' object the value 123 (making it non-null):
//..
    nullableInt.makeValue(123);
    ASSERT(!nullableInt.isNull());
    ASSERT(123 == nullableInt.value());
//..
// Finally, reset the object to its default constructed state (i.e., null):
//..
    nullableInt.reset();
    ASSERT( nullableInt.isNull());
//..

      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING 'bdlb::nullOpt' COMPARISONS
        //  The type 'bdlb::NullOptType' is not a type suitable for arbitrary
        //  user-created objects, but a proxy for conversion from the literal
        //  value 'bdlb::nullOpt'.  As such, all testing concerns will be
        //  phrased in terms of the 'bdlb::nullOpt' literal, rather than
        //  'bdlb::NullOptType'.
        //
        // Concerns:
        //: 1 That 'bdlb::nullOpt' orders as a null 'NullableValue'
        //: o 'bdlb::nullOpt' orders before any non-null 'NullableValue'
        //: o 'bdlb::nullOpt' compares equal to a null 'NullableValue'
        //:
        //: 2 If the compiler supports 'noexcept', then none of the comparisons
        //:   with 'bdlb::nullOpt' can throw an exception.
        //
        // Plan:
        //   Conduct the regular test using 'int' and 'bsl::string'.
        //
        // Testing:
        //   bool operator==(const NullableValue<LHS_TYPE>&, NullOptType);
        //   bool operator!=(const NullableValue<LHS_TYPE>&, NullOptType);
        //   bool operator< (const NullableValue<LHS_TYPE>&, NullOptType);
        //   bool operator<=(const NullableValue<LHS_TYPE>&, NullOptType);
        //   bool operator>=(const NullableValue<LHS_TYPE>&, NullOptType);
        //   bool operator> (const NullableValue<LHS_TYPE>&, NullOptType);
        //   bool operator==(NullOptType, const NullableValue<LHS_TYPE>&);
        //   bool operator!=(NullOptType, const NullableValue<LHS_TYPE>&);
        //   bool operator< (NullOptType, const NullableValue<LHS_TYPE>&);
        //   bool operator<=(NullOptType, const NullableValue<LHS_TYPE>&);
        //   bool operator>=(NullOptType, const NullableValue<LHS_TYPE>&);
        //   bool operator> (NullOptType, const NullableValue<LHS_TYPE>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'bdlb::nullOpt' COMPARISONS"
                             "\n===================================" << endl;


#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
# define ASSERT_N(         EXPRESSION  )        \
         ASSERT  (         EXPRESSION  );       \
         ASSERT  (noexcept(EXPRESSION) );
#else
# define ASSERT_N(         EXPRESSION  )        \
         ASSERT  (         EXPRESSION  );
#endif

        if (verbose) cout << "\tfor simple 'NullableValue<int>" << endl;
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            Obj mX(bdlb::nullOpt);          const Obj& X = mX;

            ASSERT_N(X.isNull());

            ASSERT_N( (X == bdlb::nullOpt) );
            ASSERT_N(!(X != bdlb::nullOpt) );
            ASSERT_N(!(X <  bdlb::nullOpt) );
            ASSERT_N( (X <= bdlb::nullOpt) );
            ASSERT_N( (X >= bdlb::nullOpt) );
            ASSERT_N(!(X >  bdlb::nullOpt) );

            ASSERT_N( (bdlb::nullOpt == X) );
            ASSERT_N(!(bdlb::nullOpt != X) );
            ASSERT_N(!(bdlb::nullOpt <  X) );
            ASSERT_N( (bdlb::nullOpt <= X) );
            ASSERT_N( (bdlb::nullOpt >= X) );
            ASSERT_N(!(bdlb::nullOpt >  X) );

            Obj mY(0);                      const Obj& Y = mY;
            ASSERT_N(!Y.isNull());

            ASSERT_N(!(Y == bdlb::nullOpt) );
            ASSERT_N( (Y != bdlb::nullOpt) );
            ASSERT_N(!(Y <  bdlb::nullOpt) );
            ASSERT_N(!(Y <= bdlb::nullOpt) );
            ASSERT_N( (Y >= bdlb::nullOpt) );
            ASSERT_N( (Y >  bdlb::nullOpt) );

            ASSERT_N(!(bdlb::nullOpt == Y) );
            ASSERT_N( (bdlb::nullOpt != Y) );
            ASSERT_N( (bdlb::nullOpt <  Y) );
            ASSERT_N( (bdlb::nullOpt <= Y) );
            ASSERT_N(!(bdlb::nullOpt >= Y) );
            ASSERT_N(!(bdlb::nullOpt >  Y) );
        }

        if (verbose) cout << "\tfor simple 'NullableValue<double>" << endl;
        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            Obj mX(bdlb::nullOpt);          const Obj& X = mX;

            ASSERT_N(X.isNull());

            ASSERT_N( (X == bdlb::nullOpt) );
            ASSERT_N(!(X != bdlb::nullOpt) );
            ASSERT_N(!(X <  bdlb::nullOpt) );
            ASSERT_N( (X <= bdlb::nullOpt) );
            ASSERT_N( (X >= bdlb::nullOpt) );
            ASSERT_N(!(X >  bdlb::nullOpt) );

            ASSERT_N( (bdlb::nullOpt == X) );
            ASSERT_N(!(bdlb::nullOpt != X) );
            ASSERT_N(!(bdlb::nullOpt <  X) );
            ASSERT_N( (bdlb::nullOpt <= X) );
            ASSERT_N( (bdlb::nullOpt >= X) );
            ASSERT_N(!(bdlb::nullOpt >  X) );

            Obj        mY("Long string literal: no short string optimization");
            const Obj& Y = mY;

            ASSERT(!Y.isNull());

            ASSERT_N(!(Y == bdlb::nullOpt) );
            ASSERT_N( (Y != bdlb::nullOpt) );
            ASSERT_N(!(Y <  bdlb::nullOpt) );
            ASSERT_N(!(Y <= bdlb::nullOpt) );
            ASSERT_N( (Y >= bdlb::nullOpt) );
            ASSERT_N( (Y >  bdlb::nullOpt) );

            ASSERT_N(!(bdlb::nullOpt == Y) );
            ASSERT_N( (bdlb::nullOpt != Y) );
            ASSERT_N( (bdlb::nullOpt <  Y) );
            ASSERT_N( (bdlb::nullOpt <= Y) );
            ASSERT_N(!(bdlb::nullOpt >= Y) );
            ASSERT_N(!(bdlb::nullOpt >  Y) );
        }
#undef ASSERT_N
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING 'bdlb::nullOpt' CONVERSION
        //  The type 'bdlb::NullOptType' is not a type suitable for arbitrary
        //  user-created objects, but a proxy for conversion from the literal
        //  value 'bdlb::nullOpt'.  As such, all testing concerns will be
        //  phrased in terms of the 'bdlb::nullOpt' literal, rather than
        //  'bdlb::NullOptType'.
        //
        // Concerns:
        //: 1 That 'NullableValue' objects implicit convert from the literal
        //:   value 'bdlb::nullOpt', producing a nulll value.
        //:
        //: 2 That memory is allocated for subsequent values using the supplied
        //:   allocator, or the default allocator if no allocator is supplied.
        //:
        //: 3 That there are no surprising ambiguities when instantiating
        //:   'NullableValue' with gregarious types, types that convert to
        //:   'NullOptType', or passing such types to a 'NullableValue'.
        //:
        //: 4 If the compiler supports 'noexcept', then none of the conversions
        //:   from 'bdlb::nullOpt' can throw an exception.
        //
        // Plan:
        //   Conduct the regular test using 'int' and 'double'.  Then try
        //   'bsl::string' with 'char *' to observe with allocators involved
        //   Finally, try 'bslmf::MatchAnyType' to show that there are no
        //   surprising ambiguities.
        //
        // Testing:
        //   NullableValue(const NullOptType&);
        //   NullableValue(const NullOptType&, allocator);
        //   NullableValue& operator=(const NullOptType& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'bdlb::nullOpt' CONVERSION"
                             "\n==================================" << endl;

        if (verbose) cout << "\tfor simple 'NullableValue<int>" << endl;
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            Obj mX(bdlb::nullOpt);          const Obj& X = mX;

            ASSERT(X.isNull());

            Obj mY(0);                      const Obj& Y = mY;
            ASSERT(!Y.isNull());

            mY = bdlb::nullOpt;
            ASSERT(Y.isNull());

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
            ASSERTV( noexcept(  mY = bdlb::nullOpt      ) );
            ASSERTV( noexcept( (void)Obj()              ) );
            ASSERTV( noexcept( (void)Obj(bdlb::nullOpt) ) );
#endif
        }

        if (verbose) cout << "\tfor simple 'NullableValue<double>" << endl;
        {
            typedef double                         ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            Obj mX(bdlb::nullOpt);          const Obj& X = mX;

            ASSERT(X.isNull());

            Obj mY(0);                      const Obj& Y = mY;
            ASSERT(!Y.isNull());

            mY = bdlb::nullOpt;
            ASSERT(Y.isNull());

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
            ASSERTV( noexcept(  mY = bdlb::nullOpt      ) );
            ASSERTV( noexcept( (void)Obj()              ) );
            ASSERTV( noexcept( (void)Obj(bdlb::nullOpt) ) );
#endif
        }

        if (verbose) cout << "\tfor allocator-aware type 'bsl::string'"
                          << endl;
        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            bslma::TestAllocator ta("test 'string'",  veryVeryVeryVerbose);

            Obj mX(bdlb::nullOpt);          const Obj& X = mX;

            ASSERT(X.isNull());

            Obj mY(bdlb::nullOpt, &ta);     const Obj& Y = mY;
            ASSERT(Y.isNull());

            Obj mZ("Big string literals evade short string optimization", &ta);
            const Obj& Z = mZ;

            ASSERT(!Z.isNull());
            ASSERT(Z.value().allocator() == &ta);

            mZ = bdlb::nullOpt;
            ASSERT(Z.isNull());

            mZ.makeValueInplace(
                  "Big string literals still evade short string optimization");

            ASSERT(!Z.isNull());
            ASSERT(Z.value().allocator() == &ta);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
            ASSERTV( noexcept(  mY = bdlb::nullOpt      ) );
            ASSERTV( noexcept( (void)Obj()              ) );
            ASSERTV( noexcept( (void)Obj(bdlb::nullOpt) ) );
#endif
        }

        if (verbose) cout << "\tambiguity concerns for awkward types" << endl;
        {
            typedef bslmf::MatchAnyType            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            Obj mX(bdlb::nullOpt);          const Obj& X = mX;

            ASSERT(X.isNull());

            Obj mY(0);                      const Obj& Y = mY;
            ASSERT(!Y.isNull());

            mY = bdlb::nullOpt;
            ASSERT(Y.isNull());

#if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
            ASSERTV( noexcept(  mY = bdlb::nullOpt      ) );
            ASSERTV( noexcept( (void)Obj()              ) );
            ASSERTV( noexcept( (void)Obj(bdlb::nullOpt) ) );
#endif
        }

      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING 'addressOr'
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'addressOr'"
                             "\n===================\n";

        RUN_EACH_TYPE(TestDriver, testCase25, TEST_TYPES);
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING WITH NON-COPY-ASSIGNABLE TYPES
        //
        // Concerns:
        //: 1 That nullable values of non-assignable types can be created.
        //
        // Plan:
        //: 1 Using ad hoc testing, verify that nullable values of
        //:   non-assignable (allocating and non-allocating) test types can be
        //:   created with the value and copy constructors.  (C-1)
        //
        // Testing:
        //   Concern: Types that are not copy-assignable can be used.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING WITH NON-COPY-ASSIGNABLE TYPES"
                             "\n======================================"
                          << endl;

        if (verbose) cout << "\tNon-allocating non-assignable test type."
                          << endl;
        {
            typedef bsltf::NonAssignableTestType   ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType VALUE(29);

            const Obj X(VALUE);

            ASSERT(        !X.isNull());
            ASSERT(VALUE == X.value());

            const Obj Y(X);

            ASSERT(        !X.isNull());
            ASSERT(VALUE == X.value());

            ASSERT(        !Y.isNull());
            ASSERT(VALUE == Y.value());
        }

        if (verbose) cout << "\tAllocating non-assignable test type." << endl;
        {
            typedef NonAssignableAllocTestType     ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            bslma::TestAllocator da     ("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa     ("object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            const ValueType VALUE(37, &scratch);

            {
                const Obj X(VALUE);

                ASSERT(        !X.isNull());
                ASSERT(VALUE == X.value());

                const Obj Y(X);

                ASSERT(        !Y.isNull());
                ASSERT(VALUE == Y.value());
            }
            ASSERT(0 == da.numBlocksInUse());

            {
                bslma::TestAllocatorMonitor dam(&da);

                const Obj X(VALUE, &oa);

                ASSERT(        !X.isNull());
                ASSERT(VALUE == X.value());

                const Obj Y(X, &oa);

                ASSERT(        !Y.isNull());
                ASSERT(VALUE == Y.value());

                ASSERT(dam.isTotalSame());
            }
            ASSERT(0 == oa.numBlocksInUse());
        }

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING VALUE MOVE-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING VALUE MOVE-ASSIGNMENT OPERATOR"
                             "\n======================================\n";

        if (verbose) cout << "\nRun Each Test Type"
                             "\n==================" << endl;

        RUN_EACH_TYPE(TestDriver,
                      testCase23_withoutAllocator,
                      TEST_TYPES_NOT_ALLOCATOR_ENABLED);

        RUN_EACH_TYPE(TestDriver,
                      testCase23_withAllocator,
                      TEST_TYPES_ALLOCATOR_ENABLED);

        RUN_EACH_TYPE(TestDriver,
                      testCase23_withAllocator,
                      bsltf::MoveOnlyAllocTestType);
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING VALUE MOVE CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING VALUE MOVE CONSTRUCTOR"
                             "\n==============================\n";

        if (verbose) cout << "\nRun Each Test Type"
                             "\n==================" << endl;

        RUN_EACH_TYPE(TestDriver,
                      testCase22_withoutAllocator,
                      TEST_TYPES_NOT_ALLOCATOR_ENABLED);

        RUN_EACH_TYPE(TestDriver,
                      testCase22_withAllocator,
                      TEST_TYPES_ALLOCATOR_ENABLED);

        RUN_EACH_TYPE(TestDriver,
                      testCase22_withAllocator,
                      bsltf::MoveOnlyAllocTestType);
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING MOVE-ASSIGNMENT OPERATOR
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING MOVE-ASSIGNMENT OPERATOR"
                             "\n================================\n";

        if (verbose) cout << "\nRun Each Test Type"
                             "\n==================" << endl;

        RUN_EACH_TYPE(TestDriver,
                      testCase21_withoutAllocator,
                      TEST_TYPES_NOT_ALLOCATOR_ENABLED);

        RUN_EACH_TYPE(TestDriver,
                      testCase21_withAllocator,
                      TEST_TYPES_ALLOCATOR_ENABLED);

        RUN_EACH_TYPE(TestDriver,
                      testCase21_withAllocator,
                      bsltf::MoveOnlyAllocTestType);
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING MOVE CONSTRUCTOR
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING MOVE CONSTRUCTOR"
                             "\n========================\n";

        if (verbose) cout << "\nRun Each Test Type"
                             "\n==================" << endl;

        RUN_EACH_TYPE(TestDriver,
                      testCase20_withoutAllocator,
                      TEST_TYPES_NOT_ALLOCATOR_ENABLED);

        RUN_EACH_TYPE(TestDriver,
                      testCase20_withAllocator,
                      TEST_TYPES_ALLOCATOR_ENABLED);

        RUN_EACH_TYPE(TestDriver,
                      testCase20_withAllocator,
                      bsltf::MoveOnlyAllocTestType);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING HASHING
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING HASHING"
                               "\n===============\n";

        RUN_EACH_TYPE(TestDriver,
                      testCase19,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_PRIMITIVE);
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'makeValueInplace'
        //   The interface of this method features variadic templates and
        //   rvalue references, both features of C++11; however, the method
        //   must be provided in the absence of either or both of those
        //   features.  Overloads provided by the 'sim_cpp11_features.pl'
        //   simulate those features when when not available (as determined by
        //   the build mode).
        //
        //: o Variadic Templates can be simulated by a suite of method
        //:   overloads, each having an additional parameter (up to some
        //:   practical limit).
        //:
        //: o Rvalue References can be replaced (though less efficiently) by
        //:   'const' references.
        //
        //   This test driver must pass in each of the build modes.  Caveat:
        //   Some types, (e.g., 'bslma::ManagedPtr') are not functionally
        //   useful when accessed via 'const' references.  Such types are
        //   avoided in out test cases.
        //
        // Concerns:
        //: 1 The returned reference provides modifiable access to a
        //:   'TEST_TYPE' object having the same value as a 'TEST_TYPE' object
        //:   constructed with the specified (variadic) arguments.
        //:
        //: 2 The value returned by this method matches that returned by the
        //:   'value' method.
        //:
        //: 3 The object state is not null after the return of this method.
        //:
        //: 4 The prior value of the object (if any) is destroyed once.
        //:
        //: 5 Allocator concerns:
        //:   1 The value is created using the allocator specified at the
        //:     creation of this object, and uses no other allocator.
        //:   2 Non-allocator-enabled types "work".
        //:
        //: 6 All of the variadic arguments contribute to the creation of the
        //:   value.  In particular:
        //:
        //:   1 The 'TEST_TYPE' value can be default constructed (i.e., no
        //:     arguments).
        //:
        //:   2 If 'TEST_TYPE' takes an allocator argument, then specifying an
        //:     allocator is prevented by compilation error.
        //:
        //:   3 If 'TEST_TYPE' takes non-terminal allocator argument, that
        //:     value is accepted.
        //:
        //:   4 The variadic template parameters can be of different types.
        //:
        //: 7 Exception guarantee: Exceptions during construction of the
        //:   object, both allocator-enabled types and non-allocator-enabled
        //:   types, leave the nullable object in a null state.
        //:
        //: 8 Nullable nullable objects can be constructed.
        //
        // Plan:
        //: 1 The many of concerns of this test case require check that the
        //:   class under test, 'bdlb::NullableValue', correctly forward
        //:   parameters to the intended parameter of the contained
        //:   'TEST_TYPE'.
        //:
        //:   o Accordingly we design and test two test helper classes (one
        //:     taking an allocator, another that does not) whose instances can
        //:     report their most recently called constructor and the values of
        //:     the arguments received.
        //:
        //:   o Additionally, for the scenarios in which the currently
        //:     contained object must be destroyed, our test classes report the
        //:     number of times its destructor is called, so we can check for
        //:     multiple destructions of an object.
        //:
        //: 2 Using the 'TEST_TYPES' macro, which defines the set of types in
        //:   the other cases of this test driver, for each type we run either
        //:   the 'testCase18_withoutAllocator' or the
        //:   'testCase18_withAllocator' (as appropriate) of the 'TestDriver'
        //:   class.  See the function-level documentation of those functions
        //:   for details.
        //:
        //: 3 Ad-hoc Test: We run 'TestCase19_withoutAllocator' for the type
        //:   'bslma::Allocator *' to show that acceptable when the user
        //:   is not attempting to avoid using the allocator specified on
        //:   construction of the nullable object.
        //:
        //: 4 Ad-hoc Test: All of the tests have used different numbers of
        //:   parameters of a single type.  We confirm that the nullable object
        //:   works as expected for a type with heterogeneous constructor
        //:   parameters.  'bsl::vector<double>' is used.
        //:
        //: 5 Ad-hoc Test: In P-2, exception guarantees were tested for the
        //:   allocating types; however, thought it is not BDE practice,
        //:   arbitrary non-allocating types can also throw exceptions.  Thus,
        //:   we define and test a helper class, 'TmvipSa_WithThrowingCtor',
        //:   and use it to show that 'bdlb::NullableValue' objects are left in
        //:   a null state when they execute the code path for a non-allocating
        //:   'TEST_TYPE'.
        //
        // Testing:
        //   TEST_TYPE& makeValueInplace(ARGS&&... args);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'makeValueInplace'"
                             "\n==========================\n";

        if (verbose) cout << "\nTest Helper Class: 'TmipSa'"
                             "\n===========================" << endl;

        if (veryVerbose) cout
                     << "\nTest Helper Class: 'TmipSa': class methods"
                        "\n==========================================" << endl;

        ASSERT(-1 == TmvipSa<int>::ctorCalled());
        TmvipSa<int>::resetCtorCalled();
        ASSERT(-1 == TmvipSa<int>::ctorCalled());
        {
            TmvipSa<int> helperObj;
        }
        ASSERT( 0 == TmvipSa<int>::ctorCalled());
        TmvipSa<int>::resetCtorCalled();
        ASSERT(-1 == TmvipSa<int>::ctorCalled());

        ASSERT( 1 == TmvipSa<int>::dtorCount()); // above "ctor" statics usage
        TmvipSa<int>::resetDtorCount();
        ASSERT( 0 == TmvipSa<int>::dtorCount());
        {
            TmvipSa<int> helperObj;
        }
        ASSERT( 1 == TmvipSa<int>::dtorCount());
        TmvipSa<int>::resetDtorCount();
        ASSERT( 0 == TmvipSa<int>::dtorCount());

        if (veryVerbose) cout
             << "\nTest Helper Class: 'TmvipSa': default & value constructors"
                "\n=========================================================="
             << endl;

        TmvipSa<int>::resetDtorCount();ASSERT(0 == TmvipSa<int>::dtorCount());

        {
            TmvipSa<int> testObj0;
                                       ASSERT(0 == TmvipSa<int>::ctorCalled());

                                       ASSERT(0 == testObj0.a1());
                                       ASSERT(0 == testObj0.a2());
                                       ASSERT(0 == testObj0.a3());
                                       ASSERT(0 == testObj0.a4());
                                       ASSERT(0 == testObj0.a5());
        }
                                       ASSERT(1 == TmvipSa<int>::dtorCount());
        {
            TmvipSa<int> testObj1(1);
                                       ASSERT(1 == TmvipSa<int>::ctorCalled());
                                       ASSERT(1 == testObj1.a1());
                                       ASSERT(0 == testObj1.a2());
                                       ASSERT(0 == testObj1.a3());
                                       ASSERT(0 == testObj1.a4());
                                       ASSERT(0 == testObj1.a5());
        }
                                       ASSERT(2 == TmvipSa<int>::dtorCount());

        {
            TmvipSa<int> testObj2(1, 2);
                                       ASSERT(2 == TmvipSa<int>::ctorCalled());
                                       ASSERT(1 == testObj2.a1());
                                       ASSERT(2 == testObj2.a2());
                                       ASSERT(0 == testObj2.a3());
                                       ASSERT(0 == testObj2.a4());
                                       ASSERT(0 == testObj2.a5());
        }
                                       ASSERT(3 == TmvipSa<int>::dtorCount());

        {
            TmvipSa<int> testObj3(1, 2, 3);
                                       ASSERT(3 == TmvipSa<int>::ctorCalled());
                                       ASSERT(1 == testObj3.a1());
                                       ASSERT(2 == testObj3.a2());
                                       ASSERT(3 == testObj3.a3());
                                       ASSERT(0 == testObj3.a4());
                                       ASSERT(0 == testObj3.a5());
        }
                                       ASSERT(4 == TmvipSa<int>::dtorCount());

        {
            TmvipSa<int> testObj4(1, 2, 3, 4);
                                       ASSERT(4 == TmvipSa<int>::ctorCalled());
                                       ASSERT(1 == testObj4.a1());
                                       ASSERT(2 == testObj4.a2());
                                       ASSERT(3 == testObj4.a3());
                                       ASSERT(4 == testObj4.a4());
                                       ASSERT(0 == testObj4.a5());
        }
                                       ASSERT(5 == TmvipSa<int>::dtorCount());

        {
            TmvipSa<int> testObj5(1, 2, 3, 4, 5);
                                       ASSERT(5 == TmvipSa<int>::ctorCalled());
                                       ASSERT(1 == testObj5.a1());
                                       ASSERT(2 == testObj5.a2());
                                       ASSERT(3 == testObj5.a3());
                                       ASSERT(4 == testObj5.a4());
                                       ASSERT(5 == testObj5.a5());
        }
                                       ASSERT(6 == TmvipSa<int>::dtorCount());

        if (veryVerbose) cout
             << "\nTest Helper Class: 'TmvipSa': copy constructor"
                "\n==============================================" << endl;

        {
            TmvipSa<int> objX(1, 2, 3, 4, 5);  const TmvipSa<int>& X = objX;
            TmvipSa<int> objY(X);              const TmvipSa<int>& Y = objY;
            ASSERT(Y.a1() == X.a1());
            ASSERT(Y.a2() == X.a2());
            ASSERT(Y.a3() == X.a3());
            ASSERT(Y.a4() == X.a4());
            ASSERT(Y.a5() == X.a5());
        }

        if (veryVerbose) cout
                           << "\nTest Helper Class: 'TmvipSa': traits"
                              "\n====================================" << endl;

        BSLMF_ASSERT(!bslma::UsesBslmaAllocator<int>::value);

        if (verbose) cout
             << "\nTest Helper Class: 'TmvipAa'"
                "\n============================" << endl;

        if (veryVerbose) cout
             << "\nTest Helper Class: 'TmvipAa': class methods"
                "\n===========================================" << endl;

        bslma::TestAllocator        ta;
        bslma::TestAllocatorMonitor tam(&ta);

        typedef bsl::string Str;

        TmvipAa<Str>::resetCtorCalled();
        ASSERT(0 >  TmvipAa<Str>::ctorCalled());
        {
            TmvipAa<Str> obj(&ta);
        }
        ASSERT(0 == TmvipAa<Str>::ctorCalled());
        TmvipAa<Str>::resetCtorCalled();
        ASSERT(0 >  TmvipAa<Str>::ctorCalled());

        TmvipAa<Str>::resetDtorCount();
        ASSERT(0 == TmvipAa<Str>::dtorCount());
        {
            TmvipAa<Str> obj(&ta);
        }
        ASSERT(1 == TmvipAa<Str>::dtorCount());
        TmvipAa<Str>::resetDtorCount();
        ASSERT(0 == TmvipAa<Str>::dtorCount());

        if (veryVerbose) cout
             << "\nTest Helper Class: 'TmvipAa': value constructors"
                "\n================================================" << endl;

        TmvipAa<Str>::resetDtorCount(); ASSERT(0 == TmvipAa<Str>::dtorCount());

        {
            TmvipAa<Str> testObj0(&ta);
                                       ASSERT(0 == TmvipAa<Str>::ctorCalled());

                                       ASSERT("" == testObj0.a1());
                                       ASSERT("" == testObj0.a2());
                                       ASSERT("" == testObj0.a3());
                                       ASSERT("" == testObj0.a4());
                                       ASSERT("" == testObj0.a5());

                                       ASSERT(&ta == testObj0.allocator());
        }
                                       ASSERT(tam.isTotalUp());
                                       ASSERT(tam.isInUseSame());
                                       ASSERT(1 == TmvipAa<Str>::dtorCount());

        {
            TmvipAa<Str> testObj1("1", &ta);
                                       ASSERT(1 == TmvipAa<Str>::ctorCalled());
                                       ASSERT("1" == testObj1.a1());
                                       ASSERT(""  == testObj1.a2());
                                       ASSERT(""  == testObj1.a3());
                                       ASSERT(""  == testObj1.a4());
                                       ASSERT(""  == testObj1.a5());

                                       ASSERT(&ta  == testObj1.allocator());
        }
                                       ASSERT(tam.isTotalUp());
                                       ASSERT(tam.isInUseSame());
                                       ASSERT(2 == TmvipAa<Str>::dtorCount());

        {
            TmvipAa<Str> testObj2("1", "2", &ta);
                                       ASSERT(2 == TmvipAa<Str>::ctorCalled());

                                       ASSERT("1"  == testObj2.a1());
                                       ASSERT("2"  == testObj2.a2());
                                       ASSERT(""   == testObj2.a3());
                                       ASSERT(""   == testObj2.a4());
                                       ASSERT(""   == testObj2.a5());

                                       ASSERT(&ta  == testObj2.allocator());
        }
                                       ASSERT(tam.isTotalUp());
                                       ASSERT(tam.isInUseSame());
                                       ASSERT(3 == TmvipAa<Str>::dtorCount());

        {
            TmvipAa<Str> testObj3("1", "2", "3", &ta);
                                       ASSERT(3 == TmvipAa<Str>::ctorCalled());

                                       ASSERT("1"  == testObj3.a1());
                                       ASSERT("2"  == testObj3.a2());
                                       ASSERT("3"  == testObj3.a3());
                                       ASSERT(""   == testObj3.a4());
                                       ASSERT(""   == testObj3.a5());

                                       ASSERT(&ta  == testObj3.allocator());
        }
                                       ASSERT(tam.isTotalUp());
                                       ASSERT(tam.isInUseSame());
                                       ASSERT(4 == TmvipAa<Str>::dtorCount());

        {
            TmvipAa<Str> testObj4("1", "2", "3", "4", &ta);
                                       ASSERT(4 == TmvipAa<Str>::ctorCalled());

                                       ASSERT("1"  == testObj4.a1());
                                       ASSERT("2"  == testObj4.a2());
                                       ASSERT("3"  == testObj4.a3());
                                       ASSERT("4"  == testObj4.a4());
                                       ASSERT(""   == testObj4.a5());

                                       ASSERT(&ta  == testObj4.allocator());
        }
                                       ASSERT(tam.isTotalUp());
                                       ASSERT(tam.isInUseSame());
                                       ASSERT(5 == TmvipAa<Str>::dtorCount());

        {
            TmvipAa<Str> testObj5("1", "2", "3", "4", "5", &ta);
                                       ASSERT(5 == TmvipAa<Str>::ctorCalled());

                                       ASSERT("1"  == testObj5.a1());
                                       ASSERT("2"  == testObj5.a2());
                                       ASSERT("3"  == testObj5.a3());
                                       ASSERT("4"  == testObj5.a4());
                                       ASSERT("5"  == testObj5.a5());

                                       ASSERT(&ta  == testObj5.allocator());
        }
                                       ASSERT(tam.isTotalUp());
                                       ASSERT(tam.isInUseSame());
                                       ASSERT(6 == TmvipAa<Str>::dtorCount());

        if (veryVerbose) cout
                           << "\nTest Helper Class: 'TmvipAa': traits"
                              "\n====================================" << endl;

        BSLMF_ASSERT(bslma::UsesBslmaAllocator<TmvipAa<Str> >::value);

        if (veryVerbose) cout
                 << "\nTest Helper Class: 'TmvipAa': copy constructor"
                    "\n==============================================" << endl;

        {
            bslma::TestAllocator taX;
            bslma::TestAllocator taY;

            TmvipAa<Str> objX("1", "2", &taX);  const TmvipAa<Str>& X = objX;
            TmvipAa<Str> objY(X,        &taY);  const TmvipAa<Str>& Y = objY;

            ASSERT(Y.a1() == X.a1());
            ASSERT(Y.a2() == X.a2());
            ASSERT(Y.a3() == X.a3());
            ASSERT(Y.a4() == X.a4());
            ASSERT(Y.a5() == X.a5());
            ASSERT(&taX   == X.allocator());
            ASSERT(&taY   == Y.allocator());
        }

        if (veryVerbose) cout
                 << "\nTest Helper Class: 'TmvipAa': Use in standard container"
                    "\n======================================================="
                 << endl;

        {
            bslma::TestAllocator va;
            bslma::TestAllocator oa;

            bsl::vector<TmvipAa<Str> > v(&va);
            ASSERT(&va == v.get_allocator());

            const TmvipAa<Str> obj("1", "2", &oa);
            ASSERT(&oa == obj.allocator());

            v.push_back(obj);
            ASSERT(obj.a1() == v.front().a1());
            ASSERT(obj.a2() == v.front().a2());
            ASSERT(obj.a3() == v.front().a3());
            ASSERT(obj.a4() == v.front().a4());
            ASSERT(obj.a5() == v.front().a5());
            ASSERT(&va      == v.front().allocator());
        }

        if (verbose) cout << "\nRun Each Test Type"
                             "\n==================" << endl;

        RUN_EACH_TYPE(TestDriver,
                     testCase18_withoutAllocator,
                     TEST_TYPES_NOT_ALLOCATOR_ENABLED);

        RUN_EACH_TYPE(TestDriver,
                      testCase18_withAllocator,
                      TEST_TYPES_ALLOCATOR_ENABLED);

        TestDriver<bslma::Allocator *>::testCase18_withoutAllocator();

        if (verbose) cout << "\nTest With Heterogeneous Parameters"
                             "\n==================================" << endl;
        {
            typedef bdlb::NullableValue<bsl::vector<double> > Obj;
            Obj mX;  const Obj& X = mX;

            const bsl::vector<double> value(5, 1.0);

            bsl::vector<double>& retValue = mX.makeValueInplace(5, 1.0);

            ASSERT(value    == X.value());
            ASSERT(retValue == X.value());

            const bsl::vector<double> otherValue(50, 10.0);
            retValue = otherValue;
            ASSERT(otherValue == X.value());
        }

        if (verbose) cout << "\nTest Nullable Nullable Objects"
                             "\n==============================" << endl;
        {
            typedef bdlb::NullableValue<int> Obj1;
            Obj1 obj1(1);
            ASSERT(1 == obj1.value());

            obj1.value() = 2;
            obj1.makeValueInplace(obj1.value());
            ASSERT(2 == obj1.value());

            typedef bdlb::NullableValue<Obj1> Obj2;

            Obj2 obj2;
            ASSERT(    obj2.isNull());

            Obj1 valueObj;
            ASSERT(    valueObj.isNull());

            obj2.makeValueInplace(valueObj);
            ASSERT(!obj2.isNull());
            ASSERT( obj2.value().isNull());
            ASSERT(valueObj == obj2.value());
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout
             << "\nTest Helper Class: 'TmvipSa_WithThrowingCtor'"
                "\n=============================================" << endl;
        {
            if (veryVerbose) cout
                 << "\nTest Helper Class: 'TmvipSa_WithThrowingCtor'"
                    ": default & value constructors"
                    "\n============================================="
                    "=============================="
                 << endl;

            typedef TmvipSa_WithThrowingCtor<int>       ThrowingHelper;

            ThrowingHelper::resetDtorCount();
            ASSERT(0 == ThrowingHelper::dtorCount());

            for (int numParams = 0; numParams <= MAX_NUM_PARAMS; ++numParams) {

                try {
                    switch (numParams) {
                      case  0: { ThrowingHelper helper; }                break;
                      case  1: { ThrowingHelper helper(1); }             break;
                      case  2: { ThrowingHelper helper(2, 2); }          break;
                      case  3: { ThrowingHelper helper(3, 3, 3); }       break;
                      case  4: { ThrowingHelper helper(4, 4, 4, 4); }    break;
                      case  5: { ThrowingHelper helper(5, 5, 5, 5, 5); } break;
                      default: { ASSERT(!"Unexpected argument count"); } break;
                    }

                    ASSERTV(numParams, !"expected exception missing");

                } catch (const bsl::exception&) {
                    if (veryVeryVerbose) {
                        P_(numParams) Q(Caught expected exception)
                    }
                } catch (...) {
                    ASSERTV(numParams, !"unexpected exception type");
                }

                ASSERTV(numParams, numParams == ThrowingHelper::ctorCalled());
                ASSERTV(numParams, 0         == ThrowingHelper::dtorCount());
            }

            if (veryVerbose) cout
                   << "\nTest Helper Class: 'TmvipSa_WithThrowingCtor': traits"
                      "\n====================================================="
                   << endl;

            BSLMF_ASSERT(!bslma::UsesBslmaAllocator<ThrowingHelper>::value);
        }

        if (verbose) cout
                   << "\nTest Using Class: 'TmvipSa_WithThrowingCtor'"
                      "\n============================================" << endl;

        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::TestAllocatorMonitor  dam(&da);
            bslma::DefaultAllocatorGuard dag(&da);

            typedef TmvipSa_WithThrowingCtor<int>       ThrowingHelper;
            typedef bdlb::NullableValue<ThrowingHelper> Obj;

            Obj mA;
            Obj mB(ThrowingHelper(1, 2, 3, 4, 5, 0xCAFEFACE));

            const Obj& A = mA;
            const Obj& B = mB;

            const struct {
                int  d_line;
                Obj  d_obj;
                bool d_isInitiallyNull;
            } DATA[] = {
                //LINE  OBJ  IS_INITIALLY_NULL
                //----  ---  -----------------
                { L_,   A,   true             },
                { L_,   B,   false            }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE                = DATA[ti].d_line;
                const Obj  OBJ                 = DATA[ti].d_obj;
                const bool IS_INITIALLY_NULL   = DATA[ti].d_isInitiallyNull;

                if (veryVeryVerbose) {
                    T_ P(IS_INITIALLY_NULL)
                    if (!IS_INITIALLY_NULL) {
                        T_ P(OBJ.value().a1())
                        T_ P(OBJ.value().a2())
                        T_ P(OBJ.value().a3())
                        T_ P(OBJ.value().a4())
                        T_ P(OBJ.value().a5())
                    }
                }

                ASSERTV(LINE, IS_INITIALLY_NULL == OBJ.isNull());

                const int EXPECTED_DTOR_COUNT = static_cast<int>(
                                                           !IS_INITIALLY_NULL);

                for (int numParams  = 0;
                         numParams <= MAX_NUM_PARAMS;
                       ++numParams) {

                    if (veryVeryVerbose) { T_ T_ P_(numParams) }

                    Obj obj(OBJ);

                    ASSERTV(numParams, IS_INITIALLY_NULL == obj.isNull());
                    ThrowingHelper::resetDtorCount();

                    try {
                        switch (numParams) {
                          case 0: obj.makeValueInplace();              break;
                          case 1: obj.makeValueInplace(1);             break;
                          case 2: obj.makeValueInplace(1, 2);          break;
                          case 3: obj.makeValueInplace(1, 2, 3);       break;
                          case 4: obj.makeValueInplace(1, 2, 3, 4);    break;
                          case 5: obj.makeValueInplace(1, 2, 3, 4, 5); break;
                          default:
                            ASSERT(!"Too many parameters.");
                        }

                        ASSERTV(numParams, !"Expected exception missing");

                    } catch (const bsl::exception&) {
                        if (veryVeryVerbose) {
                            P_(numParams) Q(Caught expected exception)
                        }
                    } catch (...) {
                        ASSERTV(numParams, !"Unexpected exception type");
                    }

                    ASSERTV(numParams, true      == obj.isNull());
                    ASSERTV(numParams, numParams ==
                                                 ThrowingHelper::ctorCalled());
                    ASSERTV(numParams,
                            EXPECTED_DTOR_COUNT  ==
                                                  ThrowingHelper::dtorCount());
                }
            }

            ASSERT(dam.isTotalSame());
        }
#else  // BDE_BUILD_TARGET_EXC
        if (verbose) {
            cout << "\nNon-Exception Build: Skip 'TmvipSa_WithThrowingCtor'"
                    "\n===================================================="
                 << endl;
        }
#endif // BDE_BUILD_TARGET_EXC

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // NEGATIVE TESTING OF ASSERT ON INVALID USE OF NULL VALUE
        // --------------------------------------------------------------------

        if (verbose) cout <<
                 "\nNEGATIVE TESTING OF ASSERT ON INVALID USE OF NULL VALUE"
                 "\n=======================================================\n";

        typedef int                            ValueType;
        typedef bdlb::NullableValue<ValueType> Obj;

        Obj mX1;  const Obj& X1 = mX1;

        bsls::AssertTestHandlerGuard guard;

        ASSERT(X1.isNull());

        ASSERT_SAFE_FAIL(X1.value());

        mX1 = 5;

        ASSERT_SAFE_PASS(X1.value());
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'valueOrNull'
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'valueOrNull'"
                             "\n=====================\n";

        RUN_EACH_TYPE(TestDriver, testCase16, TEST_TYPES);

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'valueOr(const T *)'
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'valueOr(const T *)'"
                             "\n============================\n";

        RUN_EACH_TYPE(TestDriver, testCase15, TEST_TYPES);

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'valueOr(const T&)'
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'valueOr(const T&)'"
                             "\n===========================\n";

        RUN_EACH_TYPE(TestDriver, testCase14, TEST_TYPES);

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //
        // Concerns:
        //   1. Swap of two null objects is a no-op,
        //   2. Swap of null and non-null moves the value from one object to
        //      another without calling swap for the value type,
        //   3. Swap of two non-null objects calls swap for the value type.
        //
        // Plan:
        //   Create a value type class, 'Swappable', with a swap method
        //   instrumented to track swap calls.  Instantiate
        //   'bdlb::NullableValue' with that type and execute operations needed
        //   to verify the concerns.
        //
        // Testing:
        //   void swap(NullableValue<TYPE>& other);
        //   void swap(NullableValue<TYPE>& a, b);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nSWAP MEMBER AND FREE FUNCTIONS"
                             "\n==============================" << endl;

        if (verbose) cout << "Testing w/type not taking an allocator." << endl;
        {
            if (veryVerbose) cout << "\tSanity test 'Swappable' type." << endl;
            {
                ASSERT(!Swappable::swapCalled());

                Swappable obj1(1);
                Swappable obj2(2);

                const Swappable Zobj1(obj1);
                const Swappable Zobj2(obj2);

                ASSERT(obj1 == Zobj1);
                ASSERT(obj2 == Zobj2);

                ASSERT(!Swappable::swapCalled());
                swap(obj1, obj2);
                ASSERT( Swappable::swapCalled());

                ASSERT(obj2 == Zobj1);
                ASSERT(obj1 == Zobj2);

                Swappable::reset();
                ASSERT(!Swappable::swapCalled());
            }

            typedef bdlb::NullableValue<Swappable> Obj;

            if (veryVerbose) cout << "\tSwap two null objects." << endl;
            {
                // Swap of two null objects is a no-op.

                Obj mX;  const Obj& X = mX;
                Obj mY;  const Obj& Y = mY;
                ASSERT(X.isNull());
                ASSERT(Y.isNull());

                // member 'swap'

                Swappable::reset();

                mX.swap(mY);
                ASSERT(!Swappable::swapCalled());
                ASSERT(X.isNull());
                ASSERT(Y.isNull());

                // free 'swap'

                Swappable::reset();

                swap(mX, mY);
                ASSERT(!Swappable::swapCalled());
                ASSERT(X.isNull());
                ASSERT(Y.isNull());
            }

            if (veryVerbose) cout << "\tSwap null with non-null." << endl;
            {
                // Swap of null and non-null moves the value from one object to
                // the other without calling swap for the value type.

                const Swappable VV(10);

                // 'swap' member called on non-null object.
                {
                    Obj mX(VV);  const Obj& X = mX;
                    Obj mY;      const Obj& Y = mY;
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());

                    // member 'swap'

                    Swappable::reset();

                    mX.swap(mY);
                    ASSERT(!Swappable::swapCalled());
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());

                    // free 'swap'

                    Swappable::reset();

                    swap(mX, mY);
                    ASSERT(!Swappable::swapCalled());
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());
                }

                // 'swap' member called on null object.
                {
                    Obj mX;      const Obj& X = mX;
                    Obj mY(VV);  const Obj& Y = mY;
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());

                    // member 'swap'

                    Swappable::reset();

                    mX.swap(mY);
                    ASSERT(!Swappable::swapCalled());
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());

                    // free 'swap'

                    Swappable::reset();

                    swap(mX, mY);
                    ASSERT(!Swappable::swapCalled());
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());
                }
            }

            if (veryVerbose) cout << "\tSwap two non-null objects." << endl;
            {
                // Swap of two non-null objects calls swap for the value type.

                const Swappable UU(10);
                const Swappable VV(20);

                Obj mX(UU);  const Obj& X = mX;
                Obj mY(VV);  const Obj& Y = mY;
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( UU == X.value());
                ASSERT( VV == Y.value());

                // member 'swap'

                Swappable::reset();

                mX.swap(mY);
                ASSERT( Swappable::swapCalled());
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == X.value());
                ASSERT( UU == Y.value());

                // free 'swap'

                Swappable::reset();

                swap(mX, mY);
                ASSERT( Swappable::swapCalled());
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( UU == X.value());
                ASSERT( VV == Y.value());
            }
        }

        if (verbose) cout << "Testing w/type taking an allocator." << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            if (veryVerbose) cout
                << "\tSanity test 'SwappableWithAllocator' type." << endl;
            {
                ASSERT(!SwappableWithAllocator::swapCalled());

                SwappableWithAllocator obj1(1, &scratch);
                SwappableWithAllocator obj2(2, &scratch);

                const SwappableWithAllocator Zobj1(obj1, &scratch);
                const SwappableWithAllocator Zobj2(obj2, &scratch);

                ASSERT(obj1 == Zobj1);
                ASSERT(obj2 == Zobj2);

                ASSERT(!SwappableWithAllocator::swapCalled());
                swap(obj1, obj2);
                ASSERT( SwappableWithAllocator::swapCalled());

                ASSERT(obj2 == Zobj1);
                ASSERT(obj1 == Zobj2);

                SwappableWithAllocator::reset();
                ASSERT(!SwappableWithAllocator::swapCalled());
            }

            typedef bdlb::NullableValue<SwappableWithAllocator> Obj;

            if (veryVerbose) cout << "\tSwap two null objects." << endl;
            {
                // Swap of two null objects is a no-op.

                Obj mX(&oa);  const Obj& X = mX;
                Obj mY(&oa);  const Obj& Y = mY;
                ASSERT(X.isNull());
                ASSERT(Y.isNull());

                // member 'swap'

                SwappableWithAllocator::reset();

                mX.swap(mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(X.isNull());
                ASSERT(Y.isNull());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(X.isNull());
                ASSERT(Y.isNull());
            }

            if (veryVerbose) cout << "\tSwap null with non-null." << endl;
            {
                // Swap of null and non-null moves the value from one object to
                // the other without calling swap for the value type.

                const SwappableWithAllocator VV(88, &scratch);

                // 'swap' member called on non-null object.
                {
                    Obj mX(VV, &oa);  const Obj& X = mX;
                    Obj mY(&oa);      const Obj& Y = mY;
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());

                    // member 'swap'

                    SwappableWithAllocator::reset();

                    mX.swap(mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());

                    // free 'swap'

                    SwappableWithAllocator::reset();

                    swap(mX, mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());
                }

                // 'swap' member called on null object.
                {
                    Obj mX(&oa);      const Obj& X = mX;
                    Obj mY(VV, &oa);  const Obj& Y = mY;
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());

                    // member 'swap'

                    SwappableWithAllocator::reset();

                    mX.swap(mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());

                    // free 'swap'

                    SwappableWithAllocator::reset();

                    swap(mX, mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());
                }
            }

            if (veryVerbose) cout << "\tSwap two non-null objects." << endl;
            {
                // Swap of two non-null objects calls swap for the value type.

                const SwappableWithAllocator UU(88, &scratch);
                const SwappableWithAllocator VV(99, &scratch);

                Obj mX(UU, &oa);  const Obj& X = mX;
                Obj mY(VV, &oa);  const Obj& Y = mY;
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( UU == X.value());
                ASSERT( VV == Y.value());

                // member 'swap'

                SwappableWithAllocator::reset();

                mX.swap(mY);
                ASSERT( SwappableWithAllocator::swapCalled());
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == X.value());
                ASSERT( UU == Y.value());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT( SwappableWithAllocator::swapCalled());
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( UU == X.value());
                ASSERT( VV == Y.value());
            }
        }

        if (verbose) cout << "Testing free 'swap' w/different allocators."
                          << endl;
        {
            bslma::TestAllocator da("default",   veryVeryVeryVerbose);
            bslma::TestAllocator oa("object",    veryVeryVeryVerbose);
            bslma::TestAllocator za("different", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            typedef bdlb::NullableValue<SwappableWithAllocator> Obj;

            if (veryVerbose) cout << "\tSwap two null objects." << endl;
            {
                // Swap of two null objects is a no-op.

                Obj mX(&oa);  const Obj& X = mX;
                Obj mY(&za);  const Obj& Y = mY;
                ASSERT(X.isNull());
                ASSERT(Y.isNull());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT(!SwappableWithAllocator::swapCalled());
                ASSERT(X.isNull());
                ASSERT(Y.isNull());
            }

            if (veryVerbose) cout << "\tSwap null with non-null." << endl;
            {
                // Swap of null and non-null moves the value from one object to
                // the other without calling swap for the value type.

                const SwappableWithAllocator VV(88, &scratch);

                // non-null object as first argument
                {
                    Obj mX(VV, &oa);  const Obj& X = mX;
                    Obj mY(&za);      const Obj& Y = mY;
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());

                    // free 'swap'

                    SwappableWithAllocator::reset();

                    swap(mX, mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());
                }

                // null object as first argument
                {
                    Obj mX(&oa);      const Obj& X = mX;
                    Obj mY(VV, &za);  const Obj& Y = mY;
                    ASSERT( X.isNull());
                    ASSERT(!Y.isNull());
                    ASSERT( VV == Y.value());

                    // free 'swap'

                    SwappableWithAllocator::reset();

                    swap(mX, mY);
                    ASSERT(!SwappableWithAllocator::swapCalled());
                    ASSERT(!X.isNull());
                    ASSERT( Y.isNull());
                    ASSERT( VV == X.value());
                }
            }

            if (veryVerbose) cout << "\tSwap two non-null objects." << endl;
            {
                // Swap of two non-null objects calls swap for the value type.

                const SwappableWithAllocator UU(88, &scratch);
                const SwappableWithAllocator VV(99, &scratch);

                Obj mX(UU, &oa);  const Obj& X = mX;
                Obj mY(VV, &za);  const Obj& Y = mY;
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( UU == X.value());
                ASSERT( VV == Y.value());

                // free 'swap'

                SwappableWithAllocator::reset();

                swap(mX, mY);
                ASSERT( SwappableWithAllocator::swapCalled());
                ASSERT(!X.isNull());
                ASSERT(!Y.isNull());
                ASSERT( VV == X.value());
                ASSERT( UU == Y.value());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            typedef bdlb::NullableValue<SwappableWithAllocator> Obj;

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
      case 12: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION ASSIGNMENT OPERATIONS
        //
        // Concerns:
        //   - That convertible underlying types convert.
        //   - That types with an explicit conversion constructor will properly
        //     compile and work with 'bdlb::NullableValue'.
        //   - That no unnecessary temporaries are created.
        //   - That types for which there is no conversion do not compile
        //     (we will do this test by hand for now, but could use template
        //     magic later, perhaps, to ensure that non-compile is enforced
        //     by the compiler).
        //
        // Plan:
        //   Conduct the regular test using 'int' and 'double'.  Then try
        //   'bsl::string' and 'char *' to observe with allocators involved.
        //   Then try 'Recipient' and 'Message' to test for explicit conversion
        //   constructors.  Finally, try 'int' and 'bsl::string' by hand and
        //   observe that it fails to compile.
        //
        // Testing:
        //   NullableValue& operator=(const NullableValue<OTHER_TYPE>& rhs);
        //   NullableValue& operator=(const OTHER_TYPE& rhs);
        //   TYPE& makeValue(const OTHER_TYPE& value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING CONVERSION ASSIGNMENT OPERATIONS"
                             "\n========================================"
                          << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUsing 'int' and 'double'." << endl;
        {
            typedef int    ValueType1;
            typedef double ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1a = 123;
            const ValueType1 VALUE1b = 456;

            ValueType1  mVALUE2  = 789;  const ValueType1& VALUE2 = mVALUE2;
            ValueType1& mRVALUE2 = mVALUE2;

            if (verbose) cout << "\tcopy assignment" << endl;
            {
                const ObjType1 OBJ1a(VALUE1a);
                      ObjType1 obj1b(VALUE1b);
                const ObjType1 OBJ1n;

                      ObjType2 obj2;  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ObjType2 *mR2 = &(obj2 = OBJ1a);  // null = non-null

                ASSERT(VALUE1a == OBJ1a.value());
                ASSERT(VALUE1a == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = obj1b);            // non-null = non-null

                ASSERT(VALUE1b == obj1b.value());
                ASSERT(VALUE1b == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = OBJ1n);            // non-null = null

                ASSERT(OBJ1n.isNull());
                ASSERT(OBJ2.isNull());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = OBJ1n);            // null = null

                ASSERT(OBJ1n.isNull());
                ASSERT(OBJ2.isNull());
                ASSERT(    mR2 == &obj2);
            }

            if (verbose) cout << "\tvalue assignment" << endl;
            {
                ObjType2 obj2;  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ObjType2 *mR2 = &(obj2 = VALUE1a);

                ASSERT(VALUE1a == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = VALUE1b);

                ASSERT(VALUE1b == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                // testing non-'const' source object

                obj2.reset();
                ASSERT(OBJ2.isNull());

                obj2 = mVALUE2;
                ASSERT(VALUE2 == OBJ2.value());

                obj2.reset();
                ASSERT(OBJ2.isNull());

                obj2 = mRVALUE2;
                ASSERT(VALUE2 == OBJ2.value());
            }

            if (verbose) cout << "\tmake value" << endl;
            {
                ObjType2 obj2;  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ValueType2 *addr2 = &obj2.makeValue(VALUE1a);

                ASSERT(VALUE1a == OBJ2.value());
                ASSERT(  addr2 == &obj2.value());

                addr2 = &obj2.makeValue(VALUE1b);

                ASSERT(VALUE1b == OBJ2.value());
                ASSERT(  addr2 == &obj2.value());
            }
        }
        ASSERT(0 == da.numBlocksTotal());

        if (verbose) cout << "\nUsing 'double' and 'int'." << endl;
        {
            typedef int    ValueType1;
            typedef double ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1a = 123;
            const ValueType1 VALUE1b = 456;

            ValueType1  mVALUE2  = 789;  const ValueType1& VALUE2 = mVALUE2;
            ValueType1& mRVALUE2 = mVALUE2;

            if (verbose) cout << "\tcopy assignment" << endl;
            {
                const ObjType1 OBJ1a(VALUE1a);
                      ObjType1 obj1b(VALUE1b);
                const ObjType1 OBJ1n;

                      ObjType2 obj2;  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ObjType2 *mR2 = &(obj2 = OBJ1a);  // null = non-null

                ASSERT(VALUE1a == OBJ1a.value());
                ASSERT(VALUE1a == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = obj1b);            // non-null = non-null

                ASSERT(VALUE1b == obj1b.value());
                ASSERT(VALUE1b == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = OBJ1n);            // non-null = null

                ASSERT(OBJ1n.isNull());
                ASSERT(OBJ2.isNull());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = OBJ1n);            // null = null

                ASSERT(OBJ1n.isNull());
                ASSERT(OBJ2.isNull());
                ASSERT(    mR2 == &obj2);
            }

            if (verbose) cout << "\tvalue assignment" << endl;
            {
                ObjType2 obj2;  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ObjType2 *mR2 = &(obj2 = VALUE1a);

                ASSERT(VALUE1a == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = VALUE1b);

                ASSERT(VALUE1b == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                // testing non-'const' source object

                obj2.reset();
                ASSERT(OBJ2.isNull());

                obj2 = mVALUE2;
                ASSERT(VALUE2 == OBJ2.value());

                obj2.reset();
                ASSERT(OBJ2.isNull());

                obj2 = mRVALUE2;
                ASSERT(VALUE2 == OBJ2.value());
            }

            if (verbose) cout << "\tmake value" << endl;
            {
                ObjType2 obj2;  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ValueType2 *addr2 = &obj2.makeValue(VALUE1a);

                ASSERT(VALUE1a == OBJ2.value());
                ASSERT(  addr2 == &obj2.value());

                addr2 = &obj2.makeValue(VALUE1b);

                ASSERT(VALUE1b == OBJ2.value());
                ASSERT(  addr2 == &obj2.value());
            }
        }
        ASSERT(0 == da.numBlocksTotal());

        if (verbose) cout << "\nUsing 'bsl::string' and 'char *' + ALLOC."
                          << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            typedef const char  *ValueType1;
            typedef bsl::string  ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1a = "abc";
            const ValueType1 VALUE1b = SUFFICIENTLY_LONG_STRING;

            ValueType1  mVALUE2  = "xyz";  const ValueType1& VALUE2 = mVALUE2;
            ValueType1& mRVALUE2 = mVALUE2;

            if (verbose) cout << "\tcopy assignment" << endl;
            {
                const ObjType1 OBJ1a(VALUE1a);
                      ObjType1 obj1b(VALUE1b);
                const ObjType1 OBJ1n;

                      ObjType2 obj2(&oa);  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ObjType2 *mR2 = &(obj2 = OBJ1a);  // null = non-null

                ASSERT(VALUE1a == OBJ1a.value());
                ASSERT(VALUE1a == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = obj1b);            // non-null = non-null

                ASSERT(VALUE1b == obj1b.value());
                ASSERT(VALUE1b == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = OBJ1n);            // non-null = null

                ASSERT(OBJ1n.isNull());
                ASSERT(OBJ2.isNull());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = OBJ1n);            // null = null

                ASSERT(OBJ1n.isNull());
                ASSERT(OBJ2.isNull());
                ASSERT(    mR2 == &obj2);
            }

            if (verbose) cout << "\tvalue assignment" << endl;
            {
                ObjType2 obj2(&oa);  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ObjType2 *mR2 = &(obj2 = VALUE1a);

                ASSERT(VALUE1a == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = VALUE1b);

                ASSERT(VALUE1b == OBJ2.value());
                ASSERT(    mR2 == &obj2);

                // testing non-'const' source object

                obj2.reset();
                ASSERT(OBJ2.isNull());

                obj2 = mVALUE2;
                ASSERT(VALUE2 == OBJ2.value());

                obj2.reset();
                ASSERT(OBJ2.isNull());

                obj2 = mRVALUE2;
                ASSERT(VALUE2 == OBJ2.value());
            }

            if (verbose) cout << "\tmake value" << endl;
            {
                ObjType2 obj2(&oa);  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ValueType2 *addr2 = &obj2.makeValue(VALUE1a);

                ASSERT(VALUE1a == OBJ2.value());
                ASSERT(  addr2 == &obj2.value());

                addr2 = &obj2.makeValue(VALUE1b);

                ASSERT(VALUE1b == OBJ2.value());
                ASSERT(  addr2 == &obj2.value());
            }

            {
                char VALUE[] = "1914-1918";

                const char (&RVALUE)[sizeof VALUE] = VALUE;

                ObjType2 mX(&oa);  const ObjType2& X = mX;
                ObjType2 mY(&oa);  const ObjType2& Y = mY;

                mX = RVALUE;
                ASSERT(VALUE == X.value());

                mY.makeValue(RVALUE);
                ASSERT(VALUE == Y.value());
            }
        }
        ASSERT(0 == da.numBlocksTotal());  // no temporaries

        // Make sure 'makeValue' works with explicit constructors.

        if (verbose) cout << "\nUsing 'Recipient' and 'MessageType'." << endl;
        {
            typedef MessageType ValueType1;
            typedef Recipient   ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1a(IMPORTANT);
            const ValueType1 VALUE1b(JUNK);

            ValueType1  mVALUE2  = JUNK;  const ValueType1& VALUE2 = mVALUE2;
            ValueType1& mRVALUE2 = mVALUE2;

            if (verbose) cout << "\tcopy assignment" << endl;
            {
                const ObjType1 OBJ1a(VALUE1a);
                      ObjType1 obj1b(VALUE1b);
                const ObjType1 OBJ1n;

                      ObjType2 obj2;  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ObjType2 *mR2 = &(obj2 = OBJ1a);  // null = non-null

                ASSERT(VALUE1a == OBJ1a.value());
                ASSERT(VALUE1a == OBJ2.value().msgType());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = obj1b);            // non-null = non-null

                ASSERT(VALUE1b == obj1b.value());
                ASSERT(VALUE1b == OBJ2.value().msgType());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = OBJ1n);            // non-null = null

                ASSERT(OBJ1n.isNull());
                ASSERT(OBJ2.isNull());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = OBJ1n);            // null = null

                ASSERT(OBJ1n.isNull());
                ASSERT(OBJ2.isNull());
                ASSERT(    mR2 == &obj2);
            }

            if (verbose) cout << "\tvalue assignment" << endl;
            {
                ObjType2 obj2;  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ObjType2 *mR2 = &(obj2 = VALUE1a);

                ASSERT(VALUE1a == OBJ2.value().msgType());
                ASSERT(    mR2 == &obj2);

                mR2 = &(obj2 = VALUE1b);

                ASSERT(VALUE1b == OBJ2.value().msgType());
                ASSERT(    mR2 == &obj2);

                // testing non-'const' source object

                obj2.reset();
                ASSERT(OBJ2.isNull());

                obj2 = mVALUE2;
                ASSERT(VALUE2 == OBJ2.value().msgType());

                obj2.reset();
                ASSERT(OBJ2.isNull());

                obj2 = mRVALUE2;
                ASSERT(VALUE2 == OBJ2.value().msgType());
            }

            if (verbose) cout << "\tmake value" << endl;
            {
                ObjType2 obj2;  const ObjType2& OBJ2 = obj2;
                ASSERT(OBJ2.isNull());

                ValueType2 *addr2 = &obj2.makeValue(VALUE1a);

                ASSERT(VALUE1a == OBJ2.value().msgType());
                ASSERT(  addr2 == &obj2.value());

                addr2 = &obj2.makeValue(VALUE1b);

                ASSERT(VALUE1b == OBJ2.value().msgType());
                ASSERT(  addr2 == &obj2.value());
            }
        }
        ASSERT(0 == da.numBlocksTotal());

        if (verbose) cout << "\tDecay of function to pointer-to-function."
                          << endl;
        {
            typedef bdlb::NullableValue<void(*)()> ObjType;

            {
                ObjType mX;  const ObjType& X = mX;
                ASSERT( X.isNull());

                mX = &dummyFunction;  // explicitly take address
                ASSERT(!X.isNull());

                ObjType mY;  const ObjType& Y = mY;
                ASSERT( Y.isNull());

                mY = dummyFunction;   // decay
                ASSERT(!Y.isNull());
            }

            {
                ObjType mX;  const ObjType& X = mX;
                ASSERT( X.isNull());

                mX.makeValue(&dummyFunction);  // explicitly take address
                ASSERT(!X.isNull());

                ObjType mY;  const ObjType& Y = mY;
                ASSERT( Y.isNull());

#if !defined(BDLB_FUNCTION_DOES_NOT_DECAY_TO_POINTER_TO_FUNCTION)
                mY.makeValue(dummyFunction);   // decay
                ASSERT(!Y.isNull());
#endif
            }
        }

//#define SOMETHING_ELSE_THAT_SHOULD_NOT_WORK
#ifdef SOMETHING_ELSE_THAT_SHOULD_NOT_WORK
        if (verbose) cout << "\nUsing 'bsl::string' and 'int'." << endl;
        {
            typedef int         ValueType1;
            typedef bsl::string ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1 = 123;
            const ValueType2 VALUE2 = "def";

            if (verbose) cout << "\tcopy assignment" << endl;

            const ObjType1 OBJ1(VALUE1);
                  ObjType2 obj2(VALUE2, &oa);

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = OBJ1;

            ASSERT(VALUE1 == OBJ1.value());
            //ASSERT(VALUE1 == obj2.value());

            if (verbose) cout << "\tvalue assignment" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2 = VALUE1;

            ASSERT(VALUE1 == OBJ1.value());
            //ASSERT(VALUE1 == obj2.value());

            if (verbose) cout << "\tmake value" << endl;

            obj2 = VALUE2;

            ASSERT(VALUE1 == OBJ1.value());
            ASSERT(VALUE2 == obj2.value());

            obj2.makeValue(VALUE1);

            ASSERT(VALUE1 == OBJ1.value());
            //ASSERT(VALUE1 == obj2.value());
        }
#endif

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING CONVERSION CONSTRUCTORS
        //
        // Concerns:
        //: 1 That convertible underlying types convert.
        //:
        //: 2 That types implicitly convertible from 'bslma::Allocator *' are
        //:   handled correctly.
        //:
        //: 3 That types for which there is no conversion do not compile (we
        //:   will do this test by hand for now, but could use template magic
        //:   later, perhaps, to ensure that non-compilation is enforced by the
        //:   compiler).
        //
        // Plan:
        //   Conduct the regular test using 'int' and 'double'.  Then try
        //   'bsl::string' and 'char *' to observe with allocators involved
        //   Finally, try 'int' and 'bsl::string' by hand and observe that it
        //   fails to compile.
        //
        // Testing:
        //   NullableValue(const OTHER_TYPE& value);
        //   NullableValue(const OTHER_TYPE& value, allocator);
        //   NullableValue(const NullableValue<OTHER_TYPE>&o);
        //   NullableValue(const NullableValue<OTHER_TYPE>&o, allocator);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING CONVERSION CONSTRUCTORS"
                             "\n===============================" << endl;

        if (verbose) cout << "\nConversion from 'OTHER_TYPE'." << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            if (verbose) cout << "\tUsing 'int' and 'double'." << endl;
            {
                typedef int                             ValueType1;
                typedef double                          ValueType2;

                typedef bdlb::NullableValue<ValueType2> ObjType2;

                const ValueType1 VALUE1 = 123;

                const ObjType2 OBJ2(VALUE1);
                ASSERT(VALUE1 == OBJ2.value());

                // testing non-'const' source object

                ValueType1  mVALUE1a  = 456;
                ValueType1& mRVALUE1a = mVALUE1a;

                const ValueType1& VALUE1a = mVALUE1a;

                const ObjType2 OBJ2a(mVALUE1a);
                ASSERT(VALUE1a == OBJ2a.value());

                const ObjType2 OBJ2b(mRVALUE1a);
                ASSERT(VALUE1a == OBJ2b.value());
            }
            ASSERT(0 == da.numBlocksTotal());

            if (verbose) cout << "\tUsing 'double' and 'int'." << endl;
            {
                typedef int                             ValueType1;
                typedef double                          ValueType2;

                typedef bdlb::NullableValue<ValueType2> ObjType2;

                const ValueType1 VALUE1 = 123;

                const ObjType2 OBJ2(VALUE1);
                ASSERT(VALUE1 == OBJ2.value());

                // testing non-'const' source object

                ValueType1  mVALUE1a  = 456;
                ValueType1& mRVALUE1a = mVALUE1a;

                const ValueType1& VALUE1a = mVALUE1a;

                const ObjType2 OBJ2a(mVALUE1a);
                ASSERT(VALUE1a == OBJ2a.value());

                const ObjType2 OBJ2b(mRVALUE1a);
                ASSERT(VALUE1a == OBJ2b.value());
            }
            ASSERT(0 == da.numBlocksTotal());

            if (verbose) cout << "\tUsing 'bsl::string' and 'char *' + ALLOC."
                              << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bslma::TestAllocatorMonitor dam(&da);
                bslma::TestAllocatorMonitor oam(&oa);

                typedef bsl::string                     ValueType2;

                typedef bdlb::NullableValue<ValueType2> ObjType2;

                char VALUE1[] = SUFFICIENTLY_LONG_STRING;

                ASSERT(dam.isTotalSame());
                {
                    const ObjType2 OBJ2(VALUE1);
                    ASSERT(VALUE1 == OBJ2.value());

                    ASSERT(dam.isInUseUp());
                }

                dam.reset();

                {
                    const ObjType2 OBJ2(VALUE1, &oa);
                    ASSERT(VALUE1 == OBJ2.value());

                    ASSERT(dam.isTotalSame());
                    ASSERT(0 != oa.numBlocksInUse());
                }
                ASSERT(0 == oa.numBlocksInUse());

                {
                    const char (&RVALUE1)[sizeof VALUE1] = VALUE1;

                    const ObjType2 OBJ2(RVALUE1, &oa);
                    ASSERT(VALUE1 == OBJ2.value());

                    ASSERT(dam.isTotalSame());
                    ASSERT(0 != oa.numBlocksInUse());
                }
                ASSERT(0 == oa.numBlocksInUse());
            }

            if (verbose) cout << "\tDecay of function to pointer-to-function."
                              << endl;
            {
                typedef bdlb::NullableValue<void(*)()> ObjType;

                const ObjType X(&dummyFunction);  // explicitly take address
                ASSERT(!X.isNull());

#if !defined(BDLB_FUNCTION_DOES_NOT_DECAY_TO_POINTER_TO_FUNCTION)
                const ObjType Y(dummyFunction);   // decay
                ASSERT(!Y.isNull());
#endif
            }

            if (verbose) cout <<
              "\tTest w/type implicitly convertible from 'bslma::Allocator *'."
                              << endl;
            {
                typedef ConvertibleFromAllocatorTestType ValueType;
                typedef bdlb::NullableValue<ValueType>   ObjType;

                bslma::TestAllocator oa("default", veryVeryVeryVerbose);

                ObjType mX(&oa);  const ObjType& X = mX;
                ASSERT(X.isNull());
            }
        }

        if (verbose) cout << "\nConversion from 'NullableValue<OTHER_TYPE>'."
                          << endl;
        {
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            if (verbose) cout << "\tUsing 'int' and 'double'." << endl;
            {
                typedef int                             ValueType1;
                typedef double                          ValueType2;

                typedef bdlb::NullableValue<ValueType1> ObjType1;
                typedef bdlb::NullableValue<ValueType2> ObjType2;

                const ValueType1 VALUE1 = 123;

                const ObjType1 OBJ1(VALUE1);
                const ObjType2 OBJ2(OBJ1);

                ASSERT(VALUE1             == OBJ1.value());
                ASSERT(ValueType2(VALUE1) == OBJ2.value());

                // testing non-'const' source object

                const ValueType1 VALUE1a = 456;

                ObjType1  mOBJ1a(VALUE1a);  const ObjType1& OBJ1a = mOBJ1a;
                ObjType1& mROBJ1a = mOBJ1a;

                const ObjType2 OBJ2a(mOBJ1a);

                ASSERT(VALUE1a             == OBJ1a.value());
                ASSERT(ValueType2(VALUE1a) == OBJ2a.value());

                const ObjType2 OBJ2b(mROBJ1a);

                ASSERT(VALUE1a             == OBJ1a.value());
                ASSERT(ValueType2(VALUE1a) == OBJ2b.value());
            }
            ASSERT(0 == da.numBlocksTotal());

            if (verbose) cout << "\tUsing 'double' and 'int'." << endl;
            {
                typedef int                             ValueType1;
                typedef double                          ValueType2;

                typedef bdlb::NullableValue<ValueType1> ObjType1;
                typedef bdlb::NullableValue<ValueType2> ObjType2;

                const ValueType1 VALUE1 = 123;

                const ObjType1 OBJ1(VALUE1);
                const ObjType2 OBJ2(OBJ1);

                ASSERT(VALUE1             == OBJ1.value());
                ASSERT(ValueType2(VALUE1) == OBJ2.value());

                // testing non-'const' source object

                const ValueType1 VALUE1a = 456;

                ObjType1  mOBJ1a(VALUE1a);  const ObjType1& OBJ1a = mOBJ1a;
                ObjType1& mROBJ1a = mOBJ1a;

                const ObjType2 OBJ2a(mOBJ1a);

                ASSERT(VALUE1a             == OBJ1a.value());
                ASSERT(ValueType2(VALUE1a) == OBJ2a.value());

                const ObjType2 OBJ2b(mROBJ1a);

                ASSERT(VALUE1a             == OBJ1a.value());
                ASSERT(ValueType2(VALUE1a) == OBJ2b.value());
            }
            ASSERT(0 == da.numBlocksTotal());

            if (verbose) cout << "\tUsing 'bsl::string' and 'char *' + ALLOC."
                              << endl;
            {
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bslma::TestAllocatorMonitor dam(&da);
                bslma::TestAllocatorMonitor oam(&oa);

                typedef char *                          ValueType1;
                typedef bsl::string                     ValueType2;

                typedef bdlb::NullableValue<ValueType1> ObjType1;
                typedef bdlb::NullableValue<ValueType2> ObjType2;

                char p[] = SUFFICIENTLY_LONG_STRING;

                const ValueType1 VALUE1 = p;

                const ObjType1 OBJ1(VALUE1);

                ASSERT(dam.isTotalSame());
                {
                    const ObjType2 OBJ2(OBJ1);

                    ASSERT(dam.isInUseUp());

                    ASSERT(VALUE1             == OBJ1.value());
                    ASSERT(ValueType2(VALUE1) == OBJ2.value());
                }

                dam.reset();

                {
                    const ObjType2 OBJ2(OBJ1, &oa);

                    ASSERT(dam.isTotalSame());
                    ASSERT(0 != oa.numBlocksInUse());

                    ASSERT(VALUE1             == OBJ1.value());
                    ASSERT(ValueType2(VALUE1) == OBJ2.value());
                }
                ASSERT(0 == oa.numBlocksInUse());

                // testing non-'const' source object
                {
                    char p[] = "abc";

                    const ValueType1 VALUE1a = p;

                    ObjType1  mOBJ1a(VALUE1a);  const ObjType1& OBJ1a = mOBJ1a;
                    ObjType1& mROBJ1a = mOBJ1a;

                    const ObjType2 OBJ2a(mOBJ1a);

                    ASSERT(VALUE1a             == OBJ1a.value());
                    ASSERT(ValueType2(VALUE1a) == OBJ2a.value());

                    const ObjType2 OBJ2b(mROBJ1a);

                    ASSERT(VALUE1a             == OBJ1a.value());
                    ASSERT(ValueType2(VALUE1a) == OBJ2b.value());
                }
            }
        }

//#define SOMETHING_THAT_SHOULD_NOT_WORK
#ifdef SOMETHING_THAT_SHOULD_NOT_WORK

        if (verbose) cout << "\tUsing 'bsl::string' and 'int'." << endl;
        {
            typedef int                             ValueType1;
            typedef bsl::string                     ValueType2;

            typedef bdlb::NullableValue<ValueType1> ObjType1;
            typedef bdlb::NullableValue<ValueType2> ObjType2;

            const ValueType1 VALUE1 = 123;

            const ObjType1 OBJ1(VALUE1);
            const ObjType2 OBJ2(OBJ1);
            const ObjType2 OBJ3(OBJ1, &oa);

            ASSERT(VALUE1             == OBJ1.value());
            //ASSERT(ValueType2(VALUE1) == OBJ2.value());
        }
#endif

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING VALUE ASSIGNMENT METHODS
        //
        // Concerns:
        //   - Make sure we can change values.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.
        //
        // Testing:
        //   NullableValue& operator=(const TYPE& rhs);
        //   void reset();
        //   TYPE& value();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING VALUE ASSIGNMENT METHODS"
                             "\n================================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<int>." << endl;
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            Obj mA;  Obj& A = mA;
            ASSERT(A.isNull());

            Obj *mR = &(mA = VALUE1);
            ASSERT(!A.isNull());
            ASSERT(VALUE1 == A.value());
            ASSERT(mR     == &mA);

            mR = &(mA = VALUE2);
            ASSERT(!A.isNull());
            ASSERT(VALUE2 == A.value());
            ASSERT(mR     == &mA);

            mA.value() = VALUE1;
            ASSERT(!A.isNull());
            ASSERT(VALUE1 == A.value());

            ValueType *addr = &mA.makeValue();
            ASSERT(!A.isNull());
            ASSERT(ValueType() == A.value());
            ASSERT(addr        == &A.value());

            mA.reset();
            ASSERT(A.isNull());
        }
        ASSERT(0 == da.numBlocksTotal());

        if (verbose) cout << "\nUsing bdlb::NullableValue<bsl::string>."
                          << endl;
        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            bslma::TestAllocator oa(     "object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            const ValueType VALUE1 = "abc";
            const ValueType VALUE2(SUFFICIENTLY_LONG_STRING, &scratch);

            Obj mA(&oa);  Obj& A = mA;
            ASSERT(A.isNull());

            Obj *mR = &(mA = VALUE1);
            ASSERT(!A.isNull());
            ASSERT(VALUE1 == A.value());
            ASSERT(mR     == &mA);
            ASSERT(0 == da.numBlocksTotal());  // no temporaries

            mR = &(mA = VALUE2);
            ASSERT(!A.isNull());
            ASSERT(VALUE2 == A.value());
            ASSERT(mR     == &mA);
            ASSERT(0 == da.numBlocksTotal());  // no temporaries

            mA.value() = VALUE1;
            ASSERT(!A.isNull());
            ASSERT(VALUE1 == A.value());

            ValueType *addr = &mA.makeValue();
            ASSERT(!A.isNull());
            ASSERT(ValueType() == A.value());
            ASSERT(addr        == &A.value());

            mA.reset();
            ASSERT(A.isNull());
        }
        ASSERT(0 == da.numBlocksTotal());

        if (verbose) cout << "\nWith non-'const' source object." << endl;

        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            ValueType mN = 77;  const ValueType& N = mN;

            Obj mY;  const Obj& Y = mY;
            ASSERT(     Y.isNull());

            mY = mN;
            ASSERT(N == Y.value());

            ValueType& rmN = mN;

            mY.reset();
            ASSERT(     Y.isNull());

            mY = rmN;
            ASSERT(N == Y.value());
        }

        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            ValueType mS = "abc";  const ValueType& S = mS;

            Obj mY;  const Obj& Y = mY;
            ASSERT(     Y.isNull());

            mY = mS;
            ASSERT(S == Y.value());

            ValueType& rmS = mS;

            mY.reset();
            ASSERT(     Y.isNull());

            mY = rmS;
            ASSERT(S == Y.value());
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING VALUE CONSTRUCTORS
        //
        // Concerns:
        //   - Any value must be able to be copy constructed without affecting
        //     its argument.
        //   - Should work with and without a supplied allocator.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.
        //
        // Testing:
        //   NullableValue(const TYPE& value);
        //   NullableValue(const TYPE& value, allocator);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING VALUE CONSTRUCTORS"
                             "\n==========================" << endl;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<int>." << endl;
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            const Obj A(VALUE1);
            ASSERT(VALUE1 == A.value());

            const Obj B(VALUE2);
            ASSERT(VALUE2 == B.value());
        }
        ASSERT(0 == da.numBlocksTotal());

        if (verbose) cout << "\nUsing bdlb::NullableValue<bsl::string>."
                          << endl;
        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            bslma::TestAllocator oa(     "object",  veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            bslma::TestAllocatorMonitor dam(&da);

            const ValueType VALUE1 = "abc";
            const ValueType VALUE2(SUFFICIENTLY_LONG_STRING, &scratch);

            {
                const Obj A(VALUE1);
                ASSERT(VALUE1 == A.value());
                ASSERT(dam.isTotalSame());

                const Obj B(VALUE2);
                ASSERT(VALUE2 == B.value());
                ASSERT(dam.isInUseUp());
            }
            ASSERT(0 == da.numBlocksInUse());

            dam.reset();

            {
                const Obj A(VALUE1, &oa);
                ASSERT(VALUE1 == A.value());
                ASSERT(&oa == A.get_allocator());
                ASSERT(dam.isTotalSame());         // no temporaries
                ASSERT(0 == oa.numBlocksTotal());

                const Obj B(VALUE2, &oa);
                ASSERT(VALUE2 == B.value());
                ASSERT(&oa == B.get_allocator());
                ASSERT(dam.isTotalSame());         // no temporaries
                ASSERT(0 != oa.numBlocksInUse());
            }
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(0 == oa.numBlocksInUse());
        }

        if (verbose) cout << "\nWith non-'const' source object." << endl;

        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            ValueType mN = 77;  const ValueType& N = mN;

            const Obj U(mN);
            ASSERT(N == U.value());

            ValueType& rmN = mN;

            const Obj V(rmN);
            ASSERT(N == V.value());
        }

        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            ValueType mS = "abc";  const ValueType& S = mS;

            const Obj U(mS);
            ASSERT(S == U.value());

            ValueType& rmS = mS;

            const Obj V(rmS);
            ASSERT(S == V.value());
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING OPERATIONS
        //
        // Concerns:
        //   That the stream operations work.
        //
        // Plan:
        //   Use 'int' for 'TYPE' - stream it out and stream it back in.
        //
        // Testing:
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        //   int maxSupportedBdexVersion(int) const;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   int maxSupportedBdexVersion() const;
#endif
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BDEX STREAMING OPERATIONS"
                             "\n=================================" << endl;

        typedef bslx::TestInStream  In;
        typedef bslx::TestOutStream Out;
        const int VERSION_SELECTOR = 20140601;

        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const Obj X(123);

            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion(VERSION_SELECTOR);

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t;                        ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const Obj X;
            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion(VERSION_SELECTOR);

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t(123);                   ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const Obj X(123);

            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion();

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t;                        ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const Obj X;
            Out       out(VERSION_SELECTOR);
            const int VERSION = X.maxSupportedBdexVersion();

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t(123);                   ASSERT(X != t);

            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }

        {
            typedef bdlb::NullableValue<DeprecatedBdex> Obj;

            const Obj X;

            ASSERT(17 == X.maxSupportedBdexVersion());
        }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY ASSIGNMENT OPERATOR
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Use 'bsl::string' for 'TYPE'.
        //
        //   Specify a set of unique values.  Construct and initialize all
        //   combinations (u, v) in the cross product.  Copy construct a
        //   control w from v, assign v to u, and assert that w == u and
        //   w == v.  Then test aliasing by copy constructing a control w from
        //   each u, assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //    NullableValue& operator=(const NullableValue& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COPY ASSIGNMENT OPERATOR"
                             "\n================================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<bsl::string>."
                          << endl;
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];

            const ValueType VALUE1 = "123";
            const ValueType VALUE2 = SUFFICIENTLY_LONG_STRING;

            mX[1].makeValue(VALUE1);
            mX[2].makeValue(VALUE2);

            bslma::TestAllocatorMonitor dam(&da);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], &oa);  const Obj& U = mU;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj mV(mX[j], &oa);  const Obj& V = mV;

                    Obj mW(V, &oa);  const Obj& W = mW;

                    Obj *mR = &(mU = V);

                    ASSERTV( U,  W,  U == W);
                    ASSERTV( V,  W,  V == W);
                    ASSERTV(mR, mU, mR == &mU);
                    ASSERT(dam.isTotalSame());  // no temporaries
                }
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i], &oa);  const Obj& U = mU;
                Obj mW(U,     &oa);  const Obj& W = mW;

                Obj *mR = &(mU = U);

                ASSERTV( U,  W,  U == W);
                ASSERTV(mR, mU, mR == &mU);
                ASSERT(dam.isTotalSame());  // no temporaries
            }
        }
        ASSERT(0 == da.numBlocksInUse());

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<int>." << endl;
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            mX[1].makeValue(VALUE1);
            mX[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i]);  const Obj& U = mU;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj mV(mX[j]);  const Obj& V = mV;

                    Obj mW(V);  const Obj& W = mW;

                    Obj *mR = &(mU = V);

                    ASSERTV( U,  W,  U == W);
                    ASSERTV( V,  W,  V == W);
                    ASSERTV(mR, mU, mR == &mU);
                }
            }

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj mU(mX[i]);  const Obj& U = mU;
                Obj mW(U    );  const Obj& W = mW;

                Obj *mR = &(mU = U);

                ASSERTV( U,  W,  U == W);
                ASSERTV(mR, mU, mR == &mU);
            }
        }

        if (verbose) cout << "\nWith non-'const' source object." << endl;

        // In the following, 'mY = mX' failed to compile in C++11 with an
        // earlier version of the overload set for 'operator='.

        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType N = 77;

            Obj mX;  const Obj& X = mX;  mX.makeValue(N);
            Obj mY;  const Obj& Y = mY;
            ASSERT(     Y.isNull());

            mY = X;
            ASSERT(N == Y.value());

            mY.reset();
            ASSERT(     Y.isNull());

            mY = mX;
            ASSERT(N == Y.value());

            Obj& rmX = mX;

            mY.reset();
            ASSERT(     Y.isNull());

            mY = rmX;
            ASSERT(N == Y.value());
        }

        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType S = "abc";

            Obj mX;  const Obj& X = mX;  mX.makeValue(S);
            Obj mY;  const Obj& Y = mY;
            ASSERT(     Y.isNull());

            mY = X;
            ASSERT(S == Y.value());

            mY.reset();
            ASSERT(     Y.isNull());

            mY = mX;
            ASSERT(S == Y.value());

            Obj& rmX = mX;

            mY.reset();
            ASSERT(     Y.isNull());

            mY = rmX;
            ASSERT(S == Y.value());
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.
        //
        //   Specify a set whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identical objects W and X using tested methods.  Then
        //   copy construct Y from X and use the equality operator to assert
        //   that both X and Y have the same value as W.
        //
        // Testing:
        //   NullableValue(const NullableValue& original);
        //   NullableValue(const NullableValue& original, allocator);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COPY CONSTRUCTOR"
                             "\n========================" << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<int>." << endl;
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];
            Obj mW[NUM_VALUES];

            const ValueType VALUE1 = 123;
            const ValueType VALUE2 = 456;

            mX[1].makeValue(VALUE1);
            mW[1].makeValue(VALUE1);

            mX[2].makeValue(VALUE2);
            mW[2].makeValue(VALUE2);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                Obj mY(X);  const Obj& Y = mY;

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                ASSERTV(X, W, X == W);
                ASSERTV(Y, W, Y == W);
            }
            ASSERT(0 == da.numBlocksTotal());
        }

        if (verbose) cout << "\nUsing bdlb::NullableValue<bsl::string>."
                          << endl;
        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const int NUM_VALUES = 3;

            Obj mX[NUM_VALUES];
            Obj mW[NUM_VALUES];

            const ValueType VALUE1 = "123";
            const ValueType VALUE2 = SUFFICIENTLY_LONG_STRING;

            mX[1].makeValue(VALUE1);
            mW[1].makeValue(VALUE1);

            mX[2].makeValue(VALUE2);
            mW[2].makeValue(VALUE2);

            bslma::TestAllocatorMonitor dam(&da);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                dam.reset();

                const Obj Y(X);

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                ASSERTV(X, W, X == W);
                ASSERTV(Y, W, Y == W);
                if (2 == i) {
                    ASSERT(dam.isInUseUp());
                }
                else {
                    ASSERT(dam.isTotalSame());
                }
            }

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            dam.reset();

            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj& X = mX[i];
                const Obj& W = mW[i];

                bslma::TestAllocatorMonitor oam(&oa);

                const Obj Y(X, &oa);

                if (veryVerbose) {
                    T_ P_(i) P_(W) P_(X) P(Y)
                }

                ASSERTV(X, W, X == W);
                ASSERTV(Y, W, Y == W);
                ASSERT(dam.isTotalSame());  // no temporaries
                if (2 == i) {
                    ASSERT(oam.isInUseUp());
                }
                else {
                    ASSERT(oam.isTotalSame());
                }
            }
        }

        if (verbose) cout << "\nWith non-'const' source object." << endl;

        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType N = 77;

            Obj mX;  const Obj& X = mX;  mX.makeValue(N);

            const Obj U(X);
            ASSERT(N == U.value());

            Obj& rmX = mX;

            const Obj V(rmX);
            ASSERT(N == V.value());
        }

        {
            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            const ValueType S = "abc";

            Obj mX;  const Obj& X = mX;  mX.makeValue(S);

            const Obj U(X);
            ASSERT(S == U.value());

            Obj& rmX = mX;

            const Obj V(rmX);
            ASSERT(S == V.value());
        }

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
        //:18 Either member comparison function or free comparison function
        //:   make it possible to compare nullable object and another object.
        //
        // Plan:
        //: 1 Create several objects having different values of special types
        //:   'FirstMethodComparableType' and 'SecondMethodComparableType'
        //:   which can be compared using member functions.  Then create
        //:   several 'bdlb::NullableValue' objects having the same values or
        //:   null value. Loop through the cross product of the test data.  For
        //:   each couple, check the correctness of the return value of all
        //:   relational operators (==, !=, <, >, <=, >=).
        //:
        //: 2 Create several objects of special types
        //:   'FirstFunctionComparableType' and 'SecondFunctionComparableType'
        //:   which can be compared using free functions.  Then create several
        //:   'bdlb::NullableValue' objects having the same values or null
        //:   value. Loop through the cross product of the test data.  For each
        //:   couple, check the correctness of the return value of all
        //:   relational operators (==, !=, <, >, <=, >=).
        //:
        //: 3 Create several objects of integer and long integer types.  Then
        //:   create several 'bdlb::NullableValue' objects having the same
        //:   values or null value. Loop through the cross product of the test
        //:   data.  For each couple, check the correctness of the return value
        //:   of all relational operators (==, !=, <, >, <=, >=).
        //:
        //: 4 Create several objects of comparable 'bsl::string' and
        //:   'bslstl::StringRef' types.  Then create several
        //:   'bdlb::NullableValue' objects having the same values or null
        //:   value. Loop through the cross product of the test data.  For each
        //:   couple, check the correctness of the return value of all
        //:   relational operators (==, !=, <, >, <=, >=).  (C-1..18)
        //
        // Testing:
        //   bool operator==(const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
        //   bool operator!=(const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
        //   bool operator< (const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
        //   bool operator<=(const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
        //   bool operator> (const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
        //   bool operator>=(const NullableValue<LHS_TYPE>&, <RHS_TYPE>&);
        //   bool operator==(const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
        //   bool operator==(const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
        //   bool operator!=(const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
        //   bool operator!=(const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
        //   bool operator< (const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
        //   bool operator< (const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
        //   bool operator<=(const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
        //   bool operator<=(const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
        //   bool operator> (const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
        //   bool operator> (const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
        //   bool operator>=(const NullableValue<LHS_TYPE>&, const RHS_TYPE&);
        //   bool operator>=(const LHS_TYPE&, const NullableValue<RHS_TYPE>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING EQUALITY AND RELATIONAL OPERATORS"
                          << "\n========================================="
                          << endl;

        if (verbose) cout << "\tTesting member function comparison" << endl;
        {
            typedef FirstMethodComparableType  M_TYPE_1;
            typedef SecondMethodComparableType M_TYPE_2;

            testRelationalOperations<M_TYPE_1, M_TYPE_1>(0, 1);
            testRelationalOperations<M_TYPE_1, M_TYPE_2>(0, 1);
            testRelationalOperations<M_TYPE_2, M_TYPE_2>(0, 1);
        }

        if (verbose) cout << "\tTesting free   function comparison" << endl;
        {
            typedef FirstFunctionComparableType      F_TYPE_1;
            typedef SecondFunctionComparableType     F_TYPE_2;

            // Note that we do not test '<F_TYPE_2, F_TYPE_2, int> ' scenario
            // because 'FirstFunctionComparableType' and
            // 'SecondFunctionComparableType' are totally identical.

            testRelationalOperations<F_TYPE_1, F_TYPE_1>(0, 1);
            testRelationalOperations<F_TYPE_1, F_TYPE_2>(0, 1);
            testRelationalOperations<F_TYPE_2, F_TYPE_2>(0, 1);
        }

        if (verbose) cout << "\tTesting real world examples" << endl;
        {
            // Integer types.

            typedef int                              I_TYPE_1;
            typedef long int                         I_TYPE_2;

            testRelationalOperations<I_TYPE_1, I_TYPE_1>(0, 1);
            testRelationalOperations<I_TYPE_1, I_TYPE_2>(0, 1);
            testRelationalOperations<I_TYPE_2, I_TYPE_2>(0, 1);

            // String types.

            typedef bsl::string                      S_TYPE_1;
            typedef bslstl::StringRef                S_TYPE_2;
            typedef const char *                     S_TYPE_3;

            // Comparison of two 'bdlb::NullableValue<const char *>' objects
            // gives confusing results because pointers are compared instead of
            // c-string values.  So we do not test '<S_TYPE_3, S_TYPE_3'>
            // scenario.

            testRelationalOperations<S_TYPE_1, S_TYPE_1, const char *>("",
                                                                       "0");
            testRelationalOperations<S_TYPE_1, S_TYPE_2, const char *>("",
                                                                       "0");
            testRelationalOperations<S_TYPE_1, S_TYPE_3, const char *>("",
                                                                       "0");
            testRelationalOperations<S_TYPE_2, S_TYPE_2, const char *>("",
                                                                       "0");
            testRelationalOperations<S_TYPE_2, S_TYPE_3, const char *>("",
                                                                       "0");
         }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD AND OUTPUT (<<) OPERATOR
        //   We need to test the 'print' method and the '<<' operator.
        //
        // Concerns:
        //: 1 The print method and output (<<) operator must work.
        //
        // Plan:
        //   Conduct the test using 'int' for 'TYPE'.
        //
        //   For a set of values, check that the 'print' method and output (<<)
        //   operator work as expected.
        //
        // Testing:
        //   ostream& print(ostream& s, int l=0, int spl=4) const;
        //   ostream& operator<<(ostream&, const NullableValue<TYPE>&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRINT METHOD AND OUTPUT (<<) OPERATOR"
                             "\n============================================="
                          << endl;

        typedef int                            ValueType;
        typedef bdlb::NullableValue<ValueType> Obj;

        const ValueType VALUE1          = 123;
        const char      NULL_RESULT[]   = "NULL";
        const char      VALUE1_RESULT[] = "123";

        if (verbose) cout << "\nTesting 'print' Method." << endl;
        {
            {
                Obj mX;  const Obj& X = mX;

                bsl::stringstream ss;
                ASSERT(&ss == &X.print(ss, 0, -1));
                ASSERTV(ss.str(), NULL_RESULT == ss.str());
            }
            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue(VALUE1);
                bsl::stringstream ss;
                ASSERT(&ss == &X.print(ss, 0, -1));
                ASSERTV(ss.str(), VALUE1_RESULT == ss.str());
            }
        }

        if (verbose) cout << "\nTesting Output (<<) Operator." << endl;
        {
            {
                Obj mX;  const Obj& X = mX;

                bsl::stringstream ss;
                ASSERT(&ss == &(ss << X));
                ASSERTV(ss.str(), NULL_RESULT == ss.str());
            }
            {
                Obj mX;  const Obj& X = mX;

                mX.makeValue(VALUE1);
                bsl::stringstream ss;
                ASSERT(&ss == &(ss << X));
                ASSERTV(ss.str(), VALUE1_RESULT == ss.str());
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS AND BASIC ACCESSORS
        //   This test will verify that the primary manipulators are working as
        //   expected.  Also, we test that the basic accessors are working as
        //   expected.
        //
        // Concerns:
        //   * The default constructor must create a null object.
        //   * 'makeValue()' must set the value to the one constructed with the
        //     default constructor.
        //   * The 'makeValue(const TYPE&)' function must set the value
        //     appropriately.
        //
        // Plan:
        //   Conduct the test using 'int' (does not use allocator) and
        //   'bsl::string' (uses allocator) for 'TYPE'.  (Check that
        //   'ValueType' is the right size in each case.)
        //
        //   First, verify the default constructor by testing that the
        //   resulting object is null.
        //
        //   Next, verify that the 'makeValue' function works by making a value
        //   equal to the value passed into 'makeValue'.
        //
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   typedef TYPE ValueType;
        //   NullableValue();
        //   NullableValue(const allocator_type& allocator);
        //   ~NullableValue();
        //   TYPE& makeValue();
        //   TYPE& makeValue(const TYPE& value);
        //   bool isNull() const;
        //   const TYPE& value() const;
        // --------------------------------------------------------------------

        if (verbose) cout <<
                           "\nTESTING PRIMARY MANIPULATORS AND BASIC ACCESSORS"
                           "\n================================================"
                          << endl;

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<int>'." << endl;
        {
            typedef int                            ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            ValueType *addr;

            ASSERT(sizeof(ValueType) == sizeof(Obj::ValueType));

            if (veryVerbose) cout << "\tTesting default constructor." << endl;

            {
                bslma::TestAllocator da("default", veryVeryVeryVerbose);

                bslma::DefaultAllocatorGuard dag(&da);

                Obj mX;  const Obj& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(X.isNull());
                ASSERT(0 == da.numBlocksTotal());
            }

            if (veryVerbose) cout << "\tTesting 'makeValue'." << endl;

            {
                Obj mX;  const Obj& X = mX;

                addr = &mX.makeValue();
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                addr = &mX.makeValue(3);  // set some random value
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());

                addr = &mX.makeValue();   // reset to default
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERTV(X.value(), ValueType() == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                const ValueType VALUE1 = 123;

                addr = &mX.makeValue(VALUE1);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERTV(X.value(), VALUE1 == X.value());
            }

            {
                Obj mX;  const Obj& X = mX;

                const ValueType VALUE1 = 123;
                const ValueType VALUE2 = 456;

                addr = &mX.makeValue(VALUE1);
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());

                addr = &mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERTV(X.value(), VALUE2 == X.value());
            }
        }

        if (verbose) cout << "\nUsing 'bdlb::NullableValue<bsl::string>'."
                          << endl;
        {
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            typedef bsl::string                    ValueType;
            typedef bdlb::NullableValue<ValueType> Obj;

            ValueType *addr;

            ASSERT(sizeof(ValueType) == sizeof(Obj::ValueType));

            if (veryVerbose)
                cout <<
                     "\tTesting default constructor (with supplied allocator)."
                     << endl;

            {
                Obj mX(&oa);  const Obj& X = mX;
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(X.isNull());
                ASSERT(0 == da.numBlocksTotal());
                ASSERT(0 == oa.numBlocksTotal());
            }

            if (veryVerbose) cout << "\tTesting 'makeValue'." << endl;

            {
                Obj mX(&oa);  const Obj& X = mX;

                addr = &mX.makeValue();
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERTV(X.value(), ValueType() == X.value());
                ASSERT(0 == da.numBlocksTotal());
                ASSERT(0 == oa.numBlocksTotal());
            }

            {
                Obj mX(&oa);  const Obj& X = mX;

                addr = &mX.makeValue("3");  // set some random value
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERT(0 == da.numBlocksTotal());
                ASSERT(0 == oa.numBlocksTotal());

                addr = &mX.makeValue();     // reset to default
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERTV(X.value(), ValueType() == X.value());
                ASSERT(0 == da.numBlocksTotal());
                ASSERT(0 == oa.numBlocksTotal());
            }

            {
                Obj mX(&oa);  const Obj& X = mX;

                const ValueType VALUE1 = "123";

                addr = &mX.makeValue(VALUE1);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERTV(X.value(), VALUE1 == X.value());
                ASSERT(0 == da.numBlocksTotal());
                ASSERT(0 == oa.numBlocksTotal());
            }

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);  const Obj& X = mX;

                const ValueType VALUE1 = "123";
                const ValueType VALUE2(SUFFICIENTLY_LONG_STRING, &scratch);

                addr = &mX.makeValue(VALUE1);
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERT(0 == da.numBlocksTotal());
                ASSERT(0 == oa.numBlocksTotal());

                addr = &mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERTV(X.value(), VALUE2 == X.value());
                ASSERT(0 == da.numBlocksTotal());
                ASSERT(0 != oa.numBlocksInUse());
            }
            ASSERT(0 == oa.numBlocksInUse());

            if (veryVerbose) cout << "\tTesting with default allocator."
                                  << endl;

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                bslma::TestAllocatorMonitor dam(&da);

                Obj mX;  const Obj& X = mX;
                ASSERT(dam.isTotalSame());

                const ValueType VALUE1 = "123";
                const ValueType VALUE2(SUFFICIENTLY_LONG_STRING, &scratch);

                addr = &mX.makeValue(VALUE1);
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERT(dam.isTotalSame());

                addr = &mX.makeValue(VALUE2);
                if (veryVeryVerbose) { T_ T_ P(X) }
                ASSERT(!X.isNull());
                ASSERT(addr == &X.value());
                ASSERTV(X.value(), VALUE2 == X.value());
                ASSERT(dam.isInUseUp());
            }
            ASSERT(0 == oa.numBlocksInUse());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // BREATHING TEST 2: USING 'int'
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'makeValue' and 'reset'.
        //     - basic accessors: 'value' and 'isNull'.
        //
        // Plan:
        //   Create four test objects using the default, initializing, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [9, 9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB' and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //   1. Create an object x1 (init. to VA)    { x1:VA                  }
        //   2. Create an object x2 (copy of x1)     { x1:VA x2:VA            }
        //   3. Create an object x3 (default ctor)   { x1:VA x2:VA x3:U       }
        //   4. Create an object x4 (copy of x3)     { x1:VA x2:VA x3:U  x4:U }
        //   5. Set x3 using 'makeValue' (set to VB) { x1:VA x2:VA x3:VB x4:U }
        //   6. Change x1 using 'reset'              { x1:U  x2:VA x3:VB x4:U }
        //   7. Change x1 ('makeValue', set to VC)   { x1:VC x2:VA x3:VB x4:U }
        //   8. Assign x2 = x1                       { x1:VC x2:VC x3:VB x4:U }
        //   9. Assign x2 = x3                       { x1:VC x2:VB x3:VB x4:U }
        //  10. Assign x1 = x1 (aliasing)            { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   BREATHING TEST 2: Using 'int'.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST 2: USING 'int'"
                             "\n=============================" << endl;

        typedef int                            ValueType;
        typedef bdlb::NullableValue<ValueType> Obj;

        // possible values
        const ValueType VA(123);
        const ValueType VB(234);
        const ValueType VC(345);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VA == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VA == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(X3.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t'; P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(X4.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using '=' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.makeValue(VB);
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(!X3.isNull());
        ASSERT(VB == X3.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.reset();
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('makeValue', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.makeValue(VC);
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VC == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VB == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST 1: USING 'bsl::string'
        //   This test exercises basic functionality, but tests nothing.
        //
        // Concerns:
        //   We want to demonstrate a base-line level of correct operation of
        //   the following methods and operators:
        //     - default and copy constructors.
        //     - the assignment operator (including aliasing).
        //     - equality operators: 'operator==' and 'operator!='.
        //     - primary manipulators: 'makeValue' and 'reset'.
        //     - basic accessors: 'value' and 'isSet'.
        //
        // Plan:
        //   Create four test objects using the default, initializing, and copy
        //   constructors.  Exercise the basic value-semantic methods and the
        //   equality operators using the test objects.  Invoke the primary
        //   manipulator [5, 6, 7], copy constructor [2, 4], assignment
        //   operator without [9, 9] and with [10] aliasing.  Use the basic
        //   accessors to verify the expected results.  Display object values
        //   frequently in verbose mode.  Note that 'VA', 'VB' and 'VC' denote
        //   unique, but otherwise arbitrary, object values, while 'U' denotes
        //   the valid, but "unknown", default object value.
        //
        //   1. Create an object x1 (init. to VA)    { x1:VA                  }
        //   2. Create an object x2 (copy of x1)     { x1:VA x2:VA            }
        //   3. Create an object x3 (default ctor)   { x1:VA x2:VA x3:U       }
        //   4. Create an object x4 (copy of x3)     { x1:VA x2:VA x3:U  x4:U }
        //   5. Set x3 using 'makeValue' (set to VB) { x1:VA x2:VA x3:VB x4:U }
        //   6. Change x1 using 'reset'              { x1:U  x2:VA x3:VB x4:U }
        //   7. Change x1 ('makeValue', set to VC)   { x1:VC x2:VA x3:VB x4:U }
        //   8. Assign x2 = x1                       { x1:VC x2:VC x3:VB x4:U }
        //   9. Assign x2 = x3                       { x1:VC x2:VB x3:VB x4:U }
        //  10. Assign x1 = x1 (aliasing)            { x1:VC x2:VB x3:VB x4:U }
        //
        // Testing:
        //   BREATHING TEST 1: Using 'bsl::string'.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST 1: USING 'bsl::string'"
                             "\n====================================="
                          << endl;

        typedef bsl::string                    ValueType;
        typedef bdlb::NullableValue<ValueType> Obj;

        // possible values
        const ValueType VA("The");
        const ValueType VB("Breathing");
        const ValueType VC("Test");

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(VA);  const Obj& X1 = mX1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VA == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: x1 <op> x1."
                              << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy of x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VA == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Create an object x3 (default ctor)."
                             "\t\t{ x1:VA x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(X3.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create an object x4 (copy of x3)."
                             "\t\t{ x1:VA x2:VA x3:U x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { cout << '\t'; P(X4); }

        if (veryVerbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(X4.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x3 using '=' (set to VB)."
                             "\t\t{ x1:VA x2:VA x3:VB x4:U }" << endl;
        mX3.makeValue(VB);
        if (veryVerbose) { cout << '\t'; P(X3); }

        if (veryVerbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(!X3.isNull());
        ASSERT(VB == X3.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Change x1 using 'reset'."
                             "\t\t\t{ x1:U x2:VA x3:VB x4:U }" << endl;
        mX1.reset();
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(X1.isNull());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Change x1 ('makeValue', set to VC)."
                             "\t\t{ x1:VC x2:VA x3:VB x4:U }" << endl;
        mX1.makeValue(VC);
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x1."
                             "\t\t\t\t{ x1:VC x2:VC x3:VB x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VC == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x3."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { cout << '\t'; P(X2); }

        if (veryVerbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(!X2.isNull());
        ASSERT(VB == X2.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x1 = x1."
                             "\t\t\t\t{ x1:VC x2:VB x3:VB x4:U }" << endl;
        mX1 = X1;
        if (veryVerbose) { cout << '\t'; P(X1); }

        if (veryVerbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(!X1.isNull());
        ASSERT(VC == X1.value());

        if (veryVerbose) cout << "\tb. Try equality operators: "
                                 "x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // TESTING 'makeValueInplace'
        //
        // Concerns:
        //: 1 If 'TYPE' takes an allocator argument, then specifying
        //:   an allocator is prevented by compilation error.
        //
        // Plans:
        //: 1 Depending on a preprocessor flag, this test case specifies, or
        //:   not, a disallowed allocator argument to the 'makeValueInplace'
        //:   method.  This allows manual checking for this compile-time
        //:   failure.
        //
        // Testing:
        //   Expected compile-time failure for 'makeValueInplace'.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'makeValueInplace'"
                             "\n==========================" << endl;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);
        bslma::TestAllocator sa("string", veryVeryVeryVerbose);

        typedef bsl::string                    ValueType;
        typedef bdlb::NullableValue<ValueType> ObjType;

        ObjType obj(&oa);  ASSERT(obj.isNull());

#ifdef WANT_COMPILE_FAILURE
        obj.makeValueInplace(&sa);
#else
        obj.makeValueInplace();
#endif
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
