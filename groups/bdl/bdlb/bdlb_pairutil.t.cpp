// bdlb_pairutil.t.cpp                                                -*-C++-*-
#include <bdlb_pairutil.h>

#include <bslim_testutil.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
#include <bsls_nameof.h>
#endif

#include <bsltf_templatetestfacility.h>

#include <bsl_cstdlib.h>
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
#include <bsl_deque.h>
#include <bsl_forward_list.h>
#endif
#include <bsl_iostream.h>
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
#include <bsl_list.h>
#endif
#include <bsl_map.h>
#include <bsl_string.h>
#include <bsl_type_traits.h>
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
#include <bsl_unordered_map.h>
#include <bsl_vector.h>
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// This component is a utility that provides tools for working with
// `bsl::pair`, in particular `tie` function template, which constructs such
// pair and `stdPairRefAdaptor`, which creates `std::pair` from a `bsl` one,
// storing references to the values of the original.  The primary concern is to
// ensure that these functions satisfy their contracts and return objects of
// the expected types and having the expected values.
//
// Another function of the utility, `adaptForRanges`, does not work directly
// with pairs, but uses the `stdPairRefAdaptor` that allows you to get a
// tuple-like `std::pair`, to create a lazy view. We want to make sure that the
// result of this function can be embedded into chains of range adaptors.
//
// Note that as long as `tie` always returns the correct type, there are no
// allocator issues involved, because reference types are not allocator-aware.
//-----------------------------------------------------------------------------
// CLASS DATA
// [ 3] bdlb::PairUtil::stdPairRefAdaptor;

// CLASS METHODS
// [ 2] template<T1, T2> bsl::pair<T1&, T2&> tie(T1&, T2&);
// [ 4] bsl::ranges::view auto adaptForRanges(t_CONTAINER&& c);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLES

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace {
bool     verbose = false;
bool veryVerbose = false;
}  // close unnamed namespace

//=============================================================================
//                           CASE 2 RELATED ENTITIES
//-----------------------------------------------------------------------------
namespace bdlb_pairutil_test_case_2 {

typedef int    Scalar;
typedef int    Array[2];
typedef int    Function(int);

class Class {
  public:
    Class() {}

  private:
    Class(const Class&);
    Class& operator=(const Class&);
};

union Union {
    int    d_int;
    double d_double;
};

// Alias some types that will be used as macro arguments.  (We cannot wrap
// type-ids in parentheses as we can expressions.)
typedef pair<char, short> PairCharShort;
typedef pair<int,  long>  PairIntLong;

#define BDLB_PAIRUTIL_TEST_CASE_2_TYPE_LIST \
    Scalar,        const Scalar,            \
    Array,         const Array,             \
    Class,         const Class,             \
    Union,         const Union,             \
    Function,                               \
    PairCharShort, const PairCharShort,     \
    PairIntLong,   const PairIntLong

int const0(int)
{
    return 0;
}

int id(int x)
{
    return x;
}

/// Return an lvalue of type `t_TYPE` that is distinct from the one returned
/// by `getSecond<t_TYPE>`.  This template is specialized for various types
/// for which the default definition might not do the right thing.
template <class t_TYPE>
t_TYPE& getFirst()
{
    static t_TYPE x;
    return x;
}

/// Return an lvalue of type `t_TYPE` that is distinct from the one returned
/// by `getFirst<t_TYPE>`.  This template is specialized for various types
/// for which the default definition might not do the right thing.
template <class t_TYPE>
t_TYPE& getSecond()
{
    static t_TYPE y;
    return y;
}

template <>
Function& getFirst()
{
    return const0;
}

template <>
Function& getSecond()
{
    return id;
}

/// Statically assert that the template parameters `t_TYPE1` and `t_TYPE2`
/// are the same, which ensures that the argument types are almost the same,
/// other than that they might differ in const-qualification.  Note that in
/// C++03, we have no way to distinguish whether the original arguments were
/// lvalues or rvalues, but surely we won't accidentally change the return
/// type of of `bdlb::PairUtil::tie` to a reference (or a `const`
/// non-reference) (I hope).
template <class t_TYPE1, class t_TYPE2>
void assertSameType(const t_TYPE1&, const t_TYPE2&)
{
    BSLMF_ASSERT((bsl::is_same<t_TYPE1, t_TYPE2>::value));
}

/// Assert that the specified `first` and `second` pointers are unequal.
/// Note that this overload is chosen only if the arguments point to the
/// same type (up to cv-qualification).
template <class t_TYPE>
void assertDistinctIfSameUnqualType(const t_TYPE *first, const t_TYPE *second)
{
    ASSERT(first != second);
}

void assertDistinctIfSameUnqualType(...) {}

template <class t_FIRST>
struct TestWithFirst {
    template <class t_SECOND>
    struct TestWithSecond {
        static void run() {
            t_FIRST&  first =
                         getFirst<typename bsl::remove_const<t_FIRST>::type>();
            t_SECOND& second =
                       getSecond<typename bsl::remove_const<t_SECOND>::type>();

            assertDistinctIfSameUnqualType(&first, &second);

            pair<t_FIRST&, t_SECOND&> expected(first, second);
            pair<t_FIRST&, t_SECOND&> result = bdlb::PairUtil::tie(first,
                                                                   second);
            assertSameType(expected, bdlb::PairUtil::tie(first, second));
            ASSERT(&first  == &result.first);
            ASSERT(&second == &result.second);
        }
    };

    static void run()
    {
        BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                          TestWithSecond,
                                          run,
                                          BDLB_PAIRUTIL_TEST_CASE_2_TYPE_LIST);
    }
};

/// Run test case 2 with all ordered pairs of scalar, array, class, union,
/// and function types, as well as two specializations of `bsl::pair` (used
/// in order to ensure that `tie` does not call an unintended constructor,
/// since `bsl::pair` has many constructors.)
void run()
{
    BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE(
                                          TestWithFirst,
                                          run,
                                          BDLB_PAIRUTIL_TEST_CASE_2_TYPE_LIST);
}
#undef BDLB_PAIRUTIL_TEST_CASE_2_TYPE_LIST
}  // close namespace bdlb_pairutil_test_case_2

// ============================================================================
//                           CLASSES FOR TESTING
// ----------------------------------------------------------------------------

namespace {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

                              // =============
                              // struct Record
                              // =============

/// This struct is simple value-semantic type used for testing perposes.
struct Record {
    // DATA
    int         d_identifier = 0;   // id
    bsl::string d_name       = "";  // name

    // ACCESSORS
    /// Return this object's identifier.
    int identifier() const
    {
        return d_identifier;
    }

    /// Return this object's name attribute.
    const bsl::string& name() const
    {
        return d_name;
    }
};

/// Convert objects of the (template parameter) `t_PAIR` and `const t_PAIR`
/// types using `stdPairRefAdaptor` and verify that the resulting objects are
/// of types (template parameters) `t_EXPECTED_REF` and `t_EXPECTED_CONST_REF`
/// respectively.
template <class t_PAIR, class t_EXPECTED_REF, class t_EXPECTED_CONST_REF>
void testAdaptorForOneType(int line)
{
    using Util  = bdlb::PairUtil;
    using Key   = bsl::remove_reference_t<typename t_PAIR::first_type>;
    using Value = bsl::remove_reference_t<typename t_PAIR::second_type>;

    Key   key(1);
    Value value(2);

    // lvalue
    {
        t_PAIR lvaluePair(key, value);
        auto   result = Util::stdPairRefAdaptor(lvaluePair);

        using ResultType = decltype(result);

        ASSERTV(line, bsls::NameOf<ResultType>().name(),
                bsls::NameOf<t_EXPECTED_REF>().name(),
                (bsl::is_same_v<t_EXPECTED_REF, ResultType>));
        ASSERTV(&lvaluePair.first  == &result.first);
        ASSERTV(&lvaluePair.second == &result.second);
    }

    // const lvalue
    {
        const t_PAIR constLvaluePair(key, value);
        auto         result = Util::stdPairRefAdaptor(constLvaluePair);

        using ResultType = decltype(result);

        ASSERTV(line, bsls::NameOf<ResultType>().name(),
                bsls::NameOf<t_EXPECTED_CONST_REF>().name(),
                (bsl::is_same_v<t_EXPECTED_CONST_REF, ResultType>));
        ASSERTV(&constLvaluePair.first  == &result.first);
        ASSERTV(&constLvaluePair.second == &result.second);
    }
}

/// Test `stdPairRefAdaptor` functionality.
template <class t_KEY, class t_VALUE>
void testAdaptor()
{
    // value types
    using BslPairKV     = bsl::pair<       t_KEY ,       t_VALUE  >;
    using BslPairCKV    = bsl::pair< const t_KEY ,       t_VALUE  >;
    using BslPairKCV    = bsl::pair<       t_KEY , const t_VALUE  >;
    using BslPairCKCV   = bsl::pair< const t_KEY , const t_VALUE  >;

    // cross products
    using BslPairKVR    = bsl::pair<       t_KEY ,       t_VALUE& >;
    using BslPairCKRV   = bsl::pair< const t_KEY&,       t_VALUE  >;
    using BslPairKCVR   = bsl::pair<       t_KEY , const t_VALUE& >;
    using BslPairCKRCV  = bsl::pair< const t_KEY&, const t_VALUE  >;

    // reference types
    using BslPairKRVR   = bsl::pair<       t_KEY&,       t_VALUE& >;
    using BslPairCKRVR  = bsl::pair< const t_KEY&,       t_VALUE& >;
    using BslPairKRCVR  = bsl::pair<       t_KEY&, const t_VALUE& >;
    using BslPairCKRCVR = bsl::pair< const t_KEY&, const t_VALUE& >;

    // expected result types
    using StdPairKRVR   = std::pair<       t_KEY&,       t_VALUE& >;
    using StdPairCKRVR  = std::pair< const t_KEY&,       t_VALUE& >;
    using StdPairKRCVR  = std::pair<       t_KEY&, const t_VALUE& >;
    using StdPairCKRCVR = std::pair< const t_KEY&, const t_VALUE& >;

    // tests
    //                     INPUT TYPE     EXP FOR REFS     EXP FOR CONST REFS
    //                     ------------   --------------   --------------------
    testAdaptorForOneType< BslPairKV   ,  StdPairKRVR  ,   StdPairCKRCVR >(L_);
    testAdaptorForOneType< BslPairCKV  ,  StdPairCKRVR ,   StdPairCKRCVR >(L_);
    testAdaptorForOneType< BslPairKCV  ,  StdPairKRCVR ,   StdPairCKRCVR >(L_);
    testAdaptorForOneType< BslPairCKCV ,  StdPairCKRCVR,   StdPairCKRCVR >(L_);

    testAdaptorForOneType< BslPairKVR  ,  StdPairKRVR  ,   StdPairCKRVR  >(L_);
    testAdaptorForOneType< BslPairCKRV ,  StdPairCKRVR ,   StdPairCKRCVR >(L_);
    testAdaptorForOneType< BslPairKCVR ,  StdPairKRCVR ,   StdPairCKRCVR >(L_);
    testAdaptorForOneType< BslPairCKRCV,  StdPairCKRCVR,   StdPairCKRCVR >(L_);

    testAdaptorForOneType< BslPairKRVR  , StdPairKRVR  ,   StdPairKRVR   >(L_);
    testAdaptorForOneType< BslPairCKRVR , StdPairCKRVR ,   StdPairCKRVR  >(L_);
    testAdaptorForOneType< BslPairKRCVR , StdPairKRCVR ,   StdPairKRCVR  >(L_);
    testAdaptorForOneType< BslPairCKRCVR, StdPairCKRCVR,   StdPairCKRCVR >(L_);

}

/// Test the ability to create chains of range adaptors using the pipeline
// operator for the `bsl` (template parameter) `t_CONTAINER`.
template <class t_CONTAINER>
void testAdaptForRanges()
{
    if (verbose) bsl::cout << bsl::endl
                           << "\tCONTAINER: "
                           << (bsls::NameOf<t_CONTAINER>().name())
                           << bsl::endl;

    using Util  = bdlb::PairUtil;
    using Pair  = t_CONTAINER::value_type;
    using Key   = bsl::remove_cv<typename Pair::first_type>::type;
    using Value = bsl::remove_cv<typename Pair::second_type>::type;

    const Key   MIN_KEY   = 0;     // minimum key value used to fill containers
    const Value MIN_VALUE = 1000;  // minimum value used to fill containers

    const size_t NUM_ELEMENTS = 10;
    const Key    MAX_KEY      = MIN_KEY   + NUM_ELEMENTS - 1;
    const Value  MAX_VALUE    = MIN_VALUE + NUM_ELEMENTS - 1;

    // Filling container
    bsl::vector<Pair> pairs;
    for (size_t i = 0; i < NUM_ELEMENTS; ++i) {
        pairs.emplace_back(
                        static_cast<Key  >(MIN_KEY   + static_cast<Key  >(i)),
                        static_cast<Value>(MIN_VALUE + static_cast<Value>(i)));
    }

    t_CONTAINER       container(pairs.begin(), pairs.end());
    const t_CONTAINER copy(container);

    // Testing result type

    auto adaptationResult = Util::adaptForRanges(container);
    using ResultType = decltype(adaptationResult);

    // Checking that result type is a view
    ASSERT(bsl::ranges::view<ResultType>);

    // Checking value type of the result type
    using ExpectedValueType =
                      decltype(Util::stdPairRefAdaptor(std::declval<Pair&>()));
    using ActualValueType   = bsl::ranges::range_value_t<ResultType>;

    ASSERTV(bsls::NameOf<ExpectedValueType>().name(),
            bsls::NameOf<ActualValueType>().name(),
            (bsl::is_same_v<ExpectedValueType, ActualValueType>));

    // Checking range category
    using ExpectedIterConcept = typename bsl::iterator_traits<
        typename t_CONTAINER::iterator>::iterator_category;

    using ActualIterConcept =
                typename bsl::ranges::iterator_t<ResultType>::iterator_concept;

    ASSERTV(
          bsls::NameOf<t_CONTAINER>().name(),
          bsls::NameOf<ExpectedIterConcept>().name(),
          bsls::NameOf<ActualIterConcept>().name(),
          (bsl::is_same_v<ExpectedIterConcept, ActualIterConcept>));

    // Filter
    const auto isEven = [](Value value) -> bool
    {
        return 0 == value % 2;
    };

    // Transformer
    const auto doubleValue = [](Value value)
    {
        return value * 2;
    };

    // ------------------------------------------------------------------------
    // `bsl::views::values` test
    // ------------------------------------------------------------------------

    size_t counter = 0;
    for (auto value : container
                    | bsl::views::transform(Util::stdPairRefAdaptor)
                    | bsl::views::values
                    | bsl::views::filter(isEven)) {
        ASSERTV(value, MIN_VALUE <= value );
        ASSERTV(value, MAX_VALUE >= value );
        ASSERTV(value, isEven(value));
        ++counter;
    }
    size_t expectedCounterValue = isEven(NUM_ELEMENTS) ? NUM_ELEMENTS / 2
                                                       : NUM_ELEMENTS / 2 + 1;
    ASSERTV(counter, NUM_ELEMENTS, expectedCounterValue,
            expectedCounterValue == counter);

    counter = 0;

    for (auto value : Util::adaptForRanges(container)
                    | bsl::views::values
                    | bsl::views::filter(isEven)) {
        ASSERTV(value, MIN_VALUE <= value );
        ASSERTV(value, MAX_VALUE >= value );
        ASSERTV(value, isEven(value));
        ++counter;
    }

    ASSERTV(counter, NUM_ELEMENTS, expectedCounterValue,
            expectedCounterValue == counter);

    counter = 0;

    // ------------------------------------------------------------------------

    for (auto value : container
                    | bsl::views::transform(Util::stdPairRefAdaptor)
                    | bsl::views::values
                    | bsl::views::transform(doubleValue)) {
        ASSERTV(value, MIN_VALUE * 2 <= value );
        ASSERTV(value, MAX_VALUE * 2 >= value );
        ASSERTV(value, isEven(value));
        ++counter;
    }
    ASSERTV(counter, NUM_ELEMENTS, NUM_ELEMENTS == counter);

    counter = 0;

    for (auto value : Util::adaptForRanges(container)
                    | bsl::views::values
                    | bsl::views::transform(doubleValue)) {
        ASSERTV(value, MIN_VALUE * 2 <= value );
        ASSERTV(value, MAX_VALUE * 2 >= value );
        ASSERTV(value, isEven(value));
        ++counter;
    }
    ASSERTV(counter, NUM_ELEMENTS, NUM_ELEMENTS == counter);

    counter = 0;

    // ------------------------------------------------------------------------

    for (auto value : container
                    | bsl::views::transform(Util::stdPairRefAdaptor)
                    | bsl::views::values
                    | bsl::views::filter(isEven)
                    | bsl::views::transform(doubleValue)
                    | bsl::views::take(2)) {
        ASSERTV(value, MIN_VALUE * 2 <= value );
        ASSERTV(value, MAX_VALUE * 2 >= value );
        ASSERTV(value, isEven(value));
        ++counter;
    }
    ASSERTV(counter, 2 == counter);

    counter = 0;

    for (auto value : Util::adaptForRanges(container)
                    | bsl::views::values
                    | bsl::views::filter(isEven)
                    | bsl::views::transform(doubleValue)
                    | bsl::views::take(2)) {
        ASSERTV(value, MIN_VALUE * 2 <= value );
        ASSERTV(value, MAX_VALUE * 2 >= value );
        ASSERTV(value, isEven(value));
        ++counter;
    }
    ASSERTV(counter, 2 == counter);

    counter = 0;

    // ------------------------------------------------------------------------
    // `bsl::views::keys` test
    // ------------------------------------------------------------------------

    for (auto key : container | bsl::views::transform(Util::stdPairRefAdaptor)
                              | bsl::views::keys
                              | bsl::views::filter(isEven)) {
        ASSERTV(key, MIN_KEY <= key );
        ASSERTV(key, MAX_KEY >= key );
        ASSERTV(key, isEven(key));
        ++counter;
    }
    ASSERTV(counter, NUM_ELEMENTS, expectedCounterValue,
            expectedCounterValue == counter);

    counter = 0;

    for (auto key : Util::adaptForRanges(container)
                  | bsl::views::keys
                  | bsl::views::filter(isEven)) {
        ASSERTV(key, MIN_KEY <= key );
        ASSERTV(key, MAX_KEY >= key );
        ASSERTV(key, isEven(key));
        ++counter;
    }
    ASSERTV(counter, NUM_ELEMENTS, expectedCounterValue,
            expectedCounterValue == counter);

    counter = 0;

    // ------------------------------------------------------------------------

    for (auto key : container | bsl::views::transform(Util::stdPairRefAdaptor)
                              | bsl::views::keys
                              | bsl::views::transform(doubleValue)) {
        ASSERTV(key, MIN_KEY * 2 <= key );
        ASSERTV(key, MAX_KEY * 2 >= key );
        ASSERTV(key, isEven(key));
        ++counter;
    }
    ASSERTV(counter, NUM_ELEMENTS, NUM_ELEMENTS == counter);

    counter = 0;

    for (auto key : Util::adaptForRanges(container)
                  | bsl::views::keys
                  | bsl::views::transform(doubleValue)) {
        ASSERTV(key, MIN_KEY * 2 <= key );
        ASSERTV(key, MAX_KEY * 2 >= key );
        ASSERTV(key, isEven(key));
        ++counter;
    }
    ASSERTV(counter, NUM_ELEMENTS, NUM_ELEMENTS == counter);

    counter = 0;

    // ------------------------------------------------------------------------

    for (auto key : container | bsl::views::transform(Util::stdPairRefAdaptor)
                              | bsl::views::keys
                              | bsl::views::filter(isEven)
                              | bsl::views::transform(doubleValue)
                              | bsl::views::drop(2)) {
        ASSERTV(key, MIN_KEY * 2 <= key );
        ASSERTV(key, MAX_KEY * 2 >= key );
        ASSERTV(key, isEven(key));
        ++counter;
    }
    expectedCounterValue = isEven(NUM_ELEMENTS) ? NUM_ELEMENTS / 2 - 2
                                                : NUM_ELEMENTS / 2 - 1;
    ASSERTV(counter, NUM_ELEMENTS, expectedCounterValue,
            expectedCounterValue == counter);

    counter = 0;

    for (auto key : Util::adaptForRanges(container)
                  | bsl::views::keys
                  | bsl::views::filter(isEven)
                  | bsl::views::transform(doubleValue)
                  | bsl::views::drop(2)) {
        ASSERTV(key, MIN_KEY * 2 <= key );
        ASSERTV(key, MAX_KEY * 2 >= key );
        ASSERTV(key, isEven(key));
        ++counter;
    }
    ASSERTV(counter, NUM_ELEMENTS, expectedCounterValue,
            expectedCounterValue == counter);

    // ------------------------------------------------------------------------

    ASSERTV(copy == container);

    // ------------------------------------------------------------------------
    // Testing ability to modify container using adaptors
    // ------------------------------------------------------------------------

    bsl::ranges::fill(container
                               | bsl::views::transform(Util::stdPairRefAdaptor)
                               | bsl::views::values
                               | bsl::views::filter(isEven),
                      0);

    for (auto pair : container) {
        if (pair.first % 2 != 0) {
            ASSERTV(pair.first, pair.second, 0 != pair.second);
        }
        else {
            ASSERTV(pair.first, pair.second, 0 == pair.second);
        }
    }

    bsl::ranges::fill(Util::adaptForRanges(container)
                                                  | bsl::views::values
                                                  | bsl::views::filter(isEven),
                      1);

    for (auto pair : container) {
        if (pair.first % 2 != 0) {
            ASSERTV(pair.first, pair.second, 1 != pair.second);
        }
        else {
            ASSERTV(pair.first, pair.second, 1 == pair.second);
        }
    }
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES

}  // close unnamed namespace

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

namespace {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage of `tie` Function
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we need to implement a function that takes a `bsl::map` and stores
// into out-parameters the key and value corresponding to the first entry in
// the map.  Using `bsl::map`s container interface, we can obtain a reference
// to a `bsl::pair` of the key and value.  We can then use
// `bdlb::PairUtil::tie` to assign from both the key and value in a single
// expression:

/// Load into the specified `key` and the specified `value` the key and
/// value for the first entry in the specified `map` and return `true`, or
/// else fail by storing 0 and an empty string and return `false` when `map`
/// is empty.
bool getFirst(int                              *key,
              bsl::string                      *value,
              const bsl::map<int, bsl::string>& map)
{
    if (map.empty()) {
        *key = 0;
        value->clear();
        return false;                                                 // RETURN
    }
    bdlb::PairUtil::tie(*key, *value) = *map.begin();
    return true;
}

/// Run the usage example defined in the component header.
void usageExample1()
{
    bsl::map<int, bsl::string> map;
    map[30782530] = "bbi10";

    int         uuid;
    bsl::string username;

    bool result = getFirst(&uuid, &username, map);
    ASSERT(result);
    ASSERT(30782530 == uuid);
    ASSERT("bbi10"  == username);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
void usageExample2() {
///Example 2: Adapting `bsl` Container For Ranges
/// - - - - - - - - - - - - - - - - - - - - - - -
// Let's assume that we have a `bsl::map` storing employee indexes and their
// names, and we want to get a list of employee names:
// ```
    bsl::map<int, bsl::string_view> employees{{1, "John Dow"},
                                              {2, "Jane Dow"},
                                              {3, "James Dow"}};
// ```
// However, if we were to try and access the names using `bsl::views::values`
// we would see a compilation error:
//
// auto names = employees | bsl::views::values;  // does not compile
// auto namesIt = names.begin();
// ASSERT("John Dow" == *namesIt);
//
// This fails to because `bsl::pair`, unlike the `std::pair`, does not model
// the `tuple-like` concept, which is a requirement of the
// `bsl::views::values`.  This problem can be resolved using the
// `bdlb::PairUtil::adaptForRanges` function on the container:
// ```
    auto names = bdlb::PairUtil::adaptForRanges(employees)
                                                          | bsl::views::values;
    auto namesIt = names.begin();
    ASSERT("John Dow" == *namesIt);
// ```
// And of course this function allows you to create chains of adaptors using a
// pipeline operator:
// ```
    const auto startsWithJa = [](bsl::string_view name) -> bool
    {
        return name.starts_with("Ja");
    };

    auto jaNames = bdlb::PairUtil::adaptForRanges(employees)
                                            | bsl::views::values
                                            | bsl::views::filter(startsWithJa);
    ASSERT(bsl::ranges::equal(jaNames,
                              bsl::vector<bsl::string_view>{"Jane Dow",
                                                            "James Dow"}));
// ```
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
}  // close unnamed namespace

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int    test = argc > 1 ? atoi(argv[1]) : 0;
        verbose = argc > 2;
    veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << '\n';

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLES
        //   This will test the usage examples provided in the component header
        //   file.
        //
        // Concerns:
        // 1. The usage examples provided in the component header file must
        //    compile, link, and run on all platforms as shown.
        //
        // Plan:
        // 1. Copy the usage examples from the component header, change
        //    `assert` to `ASSERT`, and run the code.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING USAGE EXAMPLES" "\n"
                          << "======================" "\n";
        usageExample1();
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        usageExample2();
#endif
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `adaptForRanges`
        //
        // Concerns:
        // 1. The `adaptForRanges` function returns a transform view of the
        //    passed container which can be integrated into a chain of other
        //    range adaptors.
        //
        // Plan:
        // 1. Specify a set of `bsl` containers for testing.  Create and
        //    populate an object of each type.  Use `adaptForRanges` function
        //    to construct a chain of range adaptors and verify the results.
        //    (C-1)
        //
        // Testing:
        //   bsl::ranges::view auto adaptForRanges(t_CONTAINER&& c);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `adaptForRanges`\n"
                          << "========================\n";

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        using UInt  = unsigned int;
        using UChar = unsigned char;
        using LLI   = long long int;

        if (verbose)
            cout << "\tTesting the construction of a chain of adaptors"
                 << endl;
        {
            testAdaptForRanges<bsl::list        <bsl::pair<int,   int > > >();
            testAdaptForRanges<bsl::list        <bsl::pair<char,  UInt> > >();
            testAdaptForRanges<bsl::list        <bsl::pair<UChar, LLI > > >();

            testAdaptForRanges<bsl::deque       <bsl::pair<int,   int > > >();
            testAdaptForRanges<bsl::deque       <bsl::pair<char,  UInt> > >();
            testAdaptForRanges<bsl::deque       <bsl::pair<UChar, LLI > > >();

            testAdaptForRanges<bsl::forward_list<bsl::pair<int,   int > > >();
            testAdaptForRanges<bsl::forward_list<bsl::pair<char,  UInt> > >();
            testAdaptForRanges<bsl::forward_list<bsl::pair<UChar, LLI > > >();

            testAdaptForRanges<bsl::map                   <int,   int >   >();
            testAdaptForRanges<bsl::map                   <char,  UInt>   >();
            testAdaptForRanges<bsl::map                   <UChar, LLI >   >();

            testAdaptForRanges<bsl::multimap              <int,   int >   >();
            testAdaptForRanges<bsl::multimap              <char,  UInt>   >();
            testAdaptForRanges<bsl::multimap              <UChar, LLI >   >();

            testAdaptForRanges<bsl::unordered_map         <int,   int >   >();
            testAdaptForRanges<bsl::unordered_map         <char,  UInt>   >();
            testAdaptForRanges<bsl::unordered_map         <UChar, LLI >   >();

            testAdaptForRanges<bsl::unordered_multimap    <int,   int >   >();
            testAdaptForRanges<bsl::unordered_multimap    <char,  UInt>   >();
            testAdaptForRanges<bsl::unordered_multimap    <UChar, LLI >   >();

            testAdaptForRanges<bsl::vector      <bsl::pair<int,   int > > >();
            testAdaptForRanges<bsl::vector      <bsl::pair<char,  UInt> > >();
            testAdaptForRanges<bsl::vector      <bsl::pair<UChar, LLI > > >();
        }
#else
        if (verbose) cout << "\t`bsl::ranges` are not supported" << endl;
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ADAPTOR FOR PAIRS
        //
        // Concerns:
        // 1. The function call operator of `stdPairRefAdaptor` returns
        //    `std::pair` containing references to the fields of the passed
        //    `bsl::pair`.
        //
        // 2. For all mixed-type combinations, each element in the result pair
        //    is a reference to the corresponding element of the input pair,
        //    with const-ness and reference type preserved:
        //    * value           yields a mutable reference
        //    * const value     yields a const reference
        //    * reference       yields the same kind of reference
        //    * const reference yields a const reference
        //
        // Plan:
        // 1. Using table-based approach specify a set of `bsl::pair` types to
        //    be passed to the function call operator of the
        //    `stdPairRefAdaptor`.  Invoke it for an object of every type and
        //    verify the type of the returned object and its value.  (C-1..2)
        //
        // Testing:
        //   bdlb::PairUtil::stdPairRefAdaptor;
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING ADAPTOR FOR PAIRS\n"
                          << "=========================\n";

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        using UInt  = unsigned int;
        using UChar = unsigned char;
        using LLI   = long long int;

        testAdaptor<int, UInt>();
        testAdaptor<UInt, UChar>();
        testAdaptor<UChar, LLI>();
#else
        if (verbose) cout << "\t`bsl::ranges` are not supported" << endl;
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `tie`
        //
        // Concerns:
        // 1. The `tie` function can accept lvalues of scalar, array, class,
        //    union, and function types.
        //
        // 2. The `tie` function can accept both const and non-const lvalues.
        //
        // 3. The return type of the `tie` function is always a `bsl::pair`
        //    whose first type is an lvalue reference to the first argument's
        //    type and whose second type is an lvalue reference to the second
        //    argument's type.
        //
        // 4. The addresses of the first and second elements of the returned
        //    pair from `tie` (which are the addresses of their referents) are
        //    equal to the addresses of the arguments.
        //
        // 5. The `tie` function always calls the correct constructor of
        //    `bsl::pair` so as to ensure that the `first` (resp. `second`)
        //    member of the result has the same address as the first (resp.
        //    second) argument.
        //
        // Plan:
        // 1. For all possible ordered pairs from a particular set of scalar,
        //    array, class, union, and function type, and const-qualified
        //    versions thereof, create one lvalue of each type.  For each such
        //    combination, call the `tie` function and verify that the result
        //    has the expected type.  Then, verify that the `first` member's
        //    address is the same as the address of the first lvalue, and the
        //    `second` member's address is the same as the address of the
        //    second lvalue.  (C-1..4)
        //
        // 2. Repeat P-1 using some combinations of two `bsl::pair`
        //    specializations.  (C-5)
        //
        // Testing:
        //   template<T1, T2> bsl::pair<T1&, T2&> tie(T1&, T2&);
        // --------------------------------------------------------------------

        if (verbose) cout
               << "TESTING `tie`\n"
               << "=============\n";

        bdlb_pairutil_test_case_2::run();
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The component is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Verify that `bdlb::PairUtil::tie` yields the correct result for a
        //    simple assignment and a simple comparison.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                             "\n==============\n";

        // Test assignment through `tie`.
        const char            *a_str = "a";
        pair<int, const char*> p(1, a_str);

        int         first  = 0;
        const char *second = 0;

        bdlb::PairUtil::tie(first, second) = p;

        ASSERT(1     == first);
        ASSERT(a_str == second);

        // Test comparison of two objects returned by `tie`.
        const int x1 = 2;
        const int y1 = 5;
        const int x2 = 3;
        const int y2 = 4;

        ASSERT(bdlb::PairUtil::tie(x1, y1) < bdlb::PairUtil::tie(x2, y2));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_RANGES
        bsl::map<int, Record> intToRecordBslMap = { {1, {11, "111"}},
                                                    {2, {22, "222"}},
                                                    {3, {33, "333"}}
                                                  };
        int                   counter = 0;

        // filter
        const auto isValidRecord = [](const auto& record)
        {
            return (22 == record.d_identifier);
        };

        // Doesn't compile with bare 'bsl::map'.
        // auto bslFilteredView = intToRecordBslMap
        //                                 | bsl::views::values
        //                                 | bsl::views::filter(isValidRecord);

        auto bslPairToStlPair =
            [](const auto& pair) -> std::pair<const int&, const Record&> {
                return { pair.first, pair.second };
            };

        auto bslFilteredByLambdaView = intToRecordBslMap
                                      | bsl::views::transform(bslPairToStlPair)
                                      | bsl::views::values
                                      | bsl::views::filter(isValidRecord);

        ASSERT(!bslFilteredByLambdaView.empty());
        for (const auto& record : bslFilteredByLambdaView) {
            ASSERTV(record.identifier(),   22    == record.identifier());
            ASSERTV(record.name().c_str(), "222" == record.name());
            ++counter;
        }

        ASSERTV(counter, 1 == counter);
        counter = 0;

        using Util = bdlb::PairUtil;

        auto bslFilteredByAdaptorView = intToRecordBslMap
                               | bsl::views::transform(Util::stdPairRefAdaptor)
                               | bsl::views::values
                               | bsl::views::filter(isValidRecord);

        ASSERT(!bslFilteredByAdaptorView.empty());
        for (const auto& record : bslFilteredByAdaptorView) {
            ASSERTV(record.identifier(),   22    == record.identifier());
            ASSERTV(record.name().c_str(), "222" == record.name());
            ++counter;
        }

        ASSERTV(counter, 1 == counter);
        counter = 0;

        auto bslFilteredByFunctionView =
                                      Util::adaptForRanges(intToRecordBslMap)
                                          | bsl::views::values
                                          |  bsl::views::filter(isValidRecord);

        ASSERT(!bslFilteredByFunctionView.empty());
        for (const auto& record : bslFilteredByFunctionView) {
            ASSERTV(record.identifier(),   22    == record.identifier());
            ASSERTV(record.name().c_str(), "222" == record.name());
            ++counter;
        }

        ASSERTV(counter, 1 == counter);
        counter = 0;

        // filter
        const auto isEven = [](int value) -> bool
        {
            return 0 == value % 2;
        };

        bsl::map< int, int> intToIntBslMap = { {1, 11}, {2, 22}, {3, 33} };

        for (auto value : intToIntBslMap | bsl::views::transform(
                    [](const auto& pair) -> std::pair<const int&, const int&> {
                        return { pair.first, pair.second };
                    })
                                         | bsl::views::values
                                         | bsl::views::filter(isEven)) {
            ASSERTV(value, 22 == value);
            ++counter;
        }

        ASSERTV(counter, 1 == counter);
        counter = 0;
#endif

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." "\n";
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." "\n";
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
