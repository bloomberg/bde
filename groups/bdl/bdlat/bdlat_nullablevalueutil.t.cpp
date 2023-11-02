// bdlat_nullablevalueutil.t.cpp                                      -*-C++-*-
#include <bdlat_nullablevalueutil.h>

#include <bsla_maybeunused.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_iostream.h>

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test driver tests each utility operation provided by
// 'NullableValueUtil' in its own case.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] static int accessValueByCategory(const TYPE&, ACCESSOR&);
// [ 3] static int manipulateValueByCategory(const TYPE&, MANIPULATOR&);
// ----------------------------------------------------------------------------
// [ 1] TEST MACHINERY
// [ 4] USAGE EXAMPLE
// ============================================================================

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {
using namespace BloombergLP;

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        bsl::cout << "Error " __FILE__ "(" << line << "): " << message
                  << "    (failed)" << bsl::endl;

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

#define ASSERT_EQ(X,Y) ASSERTV(X, Y, X == Y)
#define ASSERT_NE(X,Y) ASSERTV(X, Y, X != Y)
#define ASSERT_LT(X,Y) ASSERTV(X, Y, X <  Y)
#define ASSERT_LE(X,Y) ASSERTV(X, Y, X <= Y)
#define ASSERT_GT(X,Y) ASSERTV(X, Y, X >  Y)
#define ASSERT_GE(X,Y) ASSERTV(X, Y, X >= Y)

#define LOOP1_ASSERT_EQ(L,X,Y) ASSERTV(L, X, Y, X == Y)
#define LOOP1_ASSERT_NE(L,X,Y) ASSERTV(L, X, Y, X != Y)
#define LOOP1_ASSERT_LT(L,X,Y) ASSERTV(L, X, Y, X <  Y)
#define LOOP1_ASSERT_LE(L,X,Y) ASSERTV(L, X, Y, X <= Y)
#define LOOP1_ASSERT_GT(L,X,Y) ASSERTV(L, X, Y, X >  Y)
#define LOOP1_ASSERT_GE(L,X,Y) ASSERTV(L, X, Y, X >= Y)

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)

// ============================================================================
//                   NAMESPACE-SCOPE ENTITIES FOR TESTING
// ----------------------------------------------------------------------------

namespace BloombergLP {
namespace {
namespace usage {

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Accessing the Held Value And Its Category
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to define a function that detects whether the
// value held by a nullable value is an array.
//
// First, we need to define an accessor functor per
// {'bdlat_typecategory'|'ACCESSOR' Functors} that will be used to detect
// whether the held value is an array:
//..
    class MyArrayDetector {
        // DATA
        bool d_didVisitArray;

      public:
        // CREATORS
        MyArrayDetector()
        : d_didVisitArray(false)
        {
        }

        // MANIPULATORS
        template <class TYPE>
        int operator()(const TYPE&, bdlat_TypeCategory::Array)
        {
            d_didVisitArray = true;
            return 0;
        }

        template <class TYPE, class OTHER_CATEGORY>
        int operator()(const TYPE&, OTHER_CATEGORY)
        {
            d_didVisitArray = false;
            return 0;
        }

        // ACCESSORS
        bool didVisitArray()
        {
            return d_didVisitArray;
        }
    };
//..
// Then, we can define a utility 'struct', 'MyNullableValueUtil', that provides
// a function for detecting whether or not the held value of a nullable value
// is an array:
//..
    struct MyNullableValueUtil {

        // CLASS METHODS
        template <class TYPE>
        static int isValueAnArray(bool *isArray, const TYPE& object)
            // Load the value 'true' to the specified 'isArray' if the value
            // stored in the specified 'object' has the "array" type category,
            // and load the value 'false' otherwise.  Return 0 on success,
            // and a non-zero value otherwise.  If a non-zero value is
            // returned, the value loaded to 'isArray' is unspecified.  The
            // behavior is undefined if 'object' contains a null value.
        {
            BSLS_ASSERT(bdlat_TypeCategoryFunctions::select(object) ==
                        bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY);
            BSLS_ASSERT(!bdlat_NullableValueFunctions::isNull(object));

            MyArrayDetector detector;
            int rc = bdlat::NullableValueUtil::accessValueByCategory(object,
                                                                     detector);
            if (0 != rc) {
                return -1;                                            // RETURN
            }

            *isArray = detector.didVisitArray();
            return 0;
        }
    };
//..
// Finally, we can use this utility to detect whether nullable values are
// arrays:
//..
    void example()
    {
        bdlb::NullableValue<int> valueA(42);

        bool isArray = false;
        int rc = MyNullableValueUtil::isValueAnArray(&isArray, valueA);

        ASSERT(0 == rc);
        ASSERT(! isArray);

        bdlb::NullableValue<bsl::vector<int> > valueB;
        valueB.makeValue(bsl::vector<int>());

        rc = MyNullableValueUtil::isValueAnArray(&isArray, valueB);

        ASSERT(0 == rc);
        ASSERT(isArray);
    }
//..

}  // close namespace usage
}  // close unnamed namespace
}  // close enterprise namespace

namespace BloombergLP {
namespace {
namespace u {

                              // ===============
                              // class TestArray
                              // ===============

class TestArray {
  public:
    // CREATORS
    TestArray() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_ArrayFunctions {

template <>
struct IsArray<u::TestArray> : bsl::true_type {
};

}  // close namespace bdlat_ArrayFunctions

namespace {
namespace u {

                              // ================
                              // class TestChoice
                              // ================

class TestChoice {
  public:
    // CREATORS
    TestChoice() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_ChoiceFunctions {

template <>
struct IsChoice<u::TestChoice> : bsl::true_type {
};

}  // close namespace bdlat_ChoiceFunctions

namespace {
namespace u {

                          // ========================
                          // class TestCustomizedType
                          // ========================

class TestCustomizedType {
  public:
    // CREATORS
    TestCustomizedType() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_CustomizedTypeFunctions {

template <>
struct IsCustomizedType<u::TestCustomizedType> : bsl::true_type {
};

}  // close namespace bdlat_CustomizedTypeFunctions

namespace {
namespace u {

                           // =====================
                           // class TestDynamicType
                           // =====================

class TestDynamicType {
  public:
    // PUBLIC DATA
    bdlat_TypeCategory::Value d_category;

    // CREATORS
    explicit TestDynamicType(bdlat_TypeCategory::Value category)
    : d_category(category)
    {
    }
};

// TRAITS
bdlat_TypeCategory::Value bdlat_typeCategorySelect(
                                                 const TestDynamicType& object)
{
    return object.d_category;
}

BSLA_MAYBE_UNUSED bool bdlat_nullableValueIsNull(const u::TestDynamicType&)
{
    return true;
}

template <class ACCESSOR>
int bdlat_nullableValueAccessValue(const u::TestDynamicType&, ACCESSOR&)
{
    return -1;
}

template <class MANIPULATOR>
int bdlat_nullableValueManipulateValue(u::TestDynamicType *, MANIPULATOR&)
{
    return -1;
}

}  // close namespace u
}  // close unnamed namespace

template <>
struct bdlat_TypeCategoryDeclareDynamic<u::TestDynamicType>
: bsl::true_type {
};

namespace bdlat_ArrayFunctions {

template <>
struct IsArray<u::TestDynamicType> : bsl::true_type {
};

}  // close namespace bdlat_ArrayFunctions

namespace bdlat_ChoiceFunctions {

template <>
struct IsChoice<u::TestDynamicType> : bsl::true_type {
};

}  // close namespace bdlat_ChoiceFunctions

namespace bdlat_CustomizedTypeFunctions {

template <>
struct IsCustomizedType<u::TestDynamicType> : bsl::true_type {
};

}  // close namespace bdlat_CustomizedTypeFunctions

namespace bdlat_EnumFunctions {

template <>
struct IsEnumeration<u::TestDynamicType> : bsl::true_type {
};

}  // close namespace bdlat_EnumFunctions

namespace bdlat_NullableValueFunctions {

template <>
struct IsNullableValue<u::TestDynamicType> : bsl::true_type {
};

template <>
struct ValueType<u::TestDynamicType> {
    typedef int Type;
};

}  // close namespace bdlat_NullableValueFunctions

namespace bdlat_SequenceFunctions {

template <>
struct IsSequence<u::TestDynamicType> : bsl::true_type {
};

}  // close namespace bdlat_SequenceFunctions

namespace {
namespace u {

                           // =====================
                           // class TestEnumeration
                           // =====================

class TestEnumeration {
  public:
    TestEnumeration() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_EnumFunctions {

template <>
struct IsEnumeration<u::TestEnumeration> : bsl::true_type {
};

}  // close namespace bdlat_EnumFunctions

namespace {
namespace u {

                          // =======================
                          // class TestNullableValue
                          // =======================

class TestNullableValue {
  public:
    TestNullableValue() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_NullableValueFunctions {

template <>
struct IsNullableValue<u::TestNullableValue> : bsl::true_type {
};

}  // close namespace bdlat_NullableValueFunctions

namespace {
namespace u {

                             // ==================
                             // class TestSequence
                             // ==================

class TestSequence {
  public:
    TestSequence() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_SequenceFunctions {

template <>
struct IsSequence<u::TestSequence> : bsl::true_type {
};

}  // close namespace bdlat_SequenceFunctions

namespace {
namespace u {

                              // ================
                              // class TestSimple
                              // ================

class TestSimple {
  public:
    TestSimple() {}
};

                              // ===============
                              // struct TestUtil
                              // ===============

struct TestUtil {
  private:
    // PRIVATE CLASS METHODS
    static bdlat_TypeCategory::Value getCategoryValueImp(
                                               bdlat_TypeCategory::DynamicType)
    {
        return bdlat_TypeCategory::e_DYNAMIC_CATEGORY;
    }

    static bdlat_TypeCategory::Value getCategoryValueImp(
                                                     bdlat_TypeCategory::Array)
    {
        return bdlat_TypeCategory::e_ARRAY_CATEGORY;
    }

    static bdlat_TypeCategory::Value getCategoryValueImp(
                                                    bdlat_TypeCategory::Choice)
    {
        return bdlat_TypeCategory::e_CHOICE_CATEGORY;
    }

    static bdlat_TypeCategory::Value getCategoryValueImp(
                                            bdlat_TypeCategory::CustomizedType)
    {
        return bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY;
    }

    static bdlat_TypeCategory::Value getCategoryValueImp(
                                               bdlat_TypeCategory::Enumeration)
    {
        return bdlat_TypeCategory::e_ENUMERATION_CATEGORY;
    }

    static bdlat_TypeCategory::Value getCategoryValueImp(
                                             bdlat_TypeCategory::NullableValue)
    {
        return bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY;
    }

    static bdlat_TypeCategory::Value getCategoryValueImp(
                                                  bdlat_TypeCategory::Sequence)
    {
        return bdlat_TypeCategory::e_SEQUENCE_CATEGORY;
    }

    static bdlat_TypeCategory::Value getCategoryValueImp(
                                                    bdlat_TypeCategory::Simple)
    {
        return bdlat_TypeCategory::e_SIMPLE_CATEGORY;
    }

    static bdlat_TypeCategory::Value getCategoryValueImp(bslmf::Nil)
    {
        return bdlat_TypeCategory::e_SIMPLE_CATEGORY;
    }

  public:
    // CLASS METHODS
    template <class TYPE_CATEGORY>
    static bdlat_TypeCategory::Value getCategoryValue(TYPE_CATEGORY category)
    {
        return getCategoryValueImp(category);
    }
};

                              // ================
                              // class AnyNullPtr
                              // ================

class AnyNullPtr {
  public:
    // CREATORS
    AnyNullPtr()
    {
    }

    // ACCESSORS
    template <class TYPE>
    operator TYPE *() const
    {
        return 0;
    }
};

                             // =================
                             // class GetValuePtr
                             // =================

class GetValuePtr {
  public:
    // CREATORS
    GetValuePtr() {}

    // ACCESSORS
    template <class VALUE_TYPE>
    VALUE_TYPE *operator()(
                          bdlb::NullableValue<VALUE_TYPE> *nullableValue) const
    {
        BSLS_ASSERT(!nullableValue->isNull());
        return &nullableValue->value();
    }

    template <class VALUE_TYPE>
    const VALUE_TYPE *operator()(
                    const bdlb::NullableValue<VALUE_TYPE>& nullableValue) const
    {
        BSLS_ASSERT(!nullableValue.isNull());
        return &nullableValue.value();
    }
};

                         // =========================
                         // class GetCategoryAccessor
                         // =========================

template <class ELEMENT_TYPE>
class GetCategoryAccessor {
    // DATA
    int                        d_rc;
    bdlat_TypeCategory::Value  d_category;
    const ELEMENT_TYPE        *d_element_p;

  public:
    // CREATORS
    explicit GetCategoryAccessor(int rc = 0)
    : d_rc(rc)
    , d_category()
    , d_element_p()
    {
    }

    // MANIPULATORS
    template <class TYPE_CATEGORY>
    int operator()(const ELEMENT_TYPE& element, TYPE_CATEGORY category)
    {
        d_category  = TestUtil::getCategoryValue(category);
        d_element_p = &element;
        return d_rc;
    }

    template <class OTHER_ELEMENT_TYPE, class TYPE_CATEGORY>
    int operator()(const OTHER_ELEMENT_TYPE&, TYPE_CATEGORY)
    {
        return -1;
    }

    // ACCESSORS
    bdlat_TypeCategory::Value category() const { return d_category; }

    const ELEMENT_TYPE *elementPtr() const { return d_element_p; }
};

                        // ============================
                        // class GetCategoryManipulator
                        // ============================

template <class ELEMENT_TYPE>
class GetCategoryManipulator {
    // DATA
    int                        d_rc;
    bdlat_TypeCategory::Value  d_category;
    ELEMENT_TYPE              *d_element_p;

  public:
    // CREATORS
    explicit GetCategoryManipulator(int rc = 0)
    : d_rc(rc)
    , d_category()
    , d_element_p()
    {
    }

    // MANIPULATORS
    template <class TYPE_CATEGORY>
    int operator()(ELEMENT_TYPE *element, TYPE_CATEGORY category)
    {
        d_category  = TestUtil::getCategoryValue(category);
        d_element_p = element;
        return d_rc;
    }

    template <class OTHER_ELEMENT_TYPE, class TYPE_CATEGORY>
    int operator()(OTHER_ELEMENT_TYPE *, TYPE_CATEGORY)
    {
        return -1;
    }

    // ACCESSORS
    bdlat_TypeCategory::Value category() const { return d_category; }

    ELEMENT_TYPE *elementPtr() const { return d_element_p; }
};

                             // ==================
                             // class TestCategory
                             // ==================

class TestCategory {
  public:
    // CREATORS
    TestCategory() {}

    // ACCESSORS
    template <class TYPE>
    void run(int                       line,
             const TYPE&               object,
             bdlat_TypeCategory::Value staticCategory,
             bdlat_TypeCategory::Value dynamicCategory) const
    {
        LOOP1_ASSERT_EQ(line,
                        static_cast<bdlat_TypeCategory::Value>(
                            bdlat_TypeCategory::Select<TYPE>::e_SELECTION),
                        staticCategory);
        LOOP1_ASSERT_EQ(line,
                        bdlat_TypeCategoryFunctions::select(object),
                        dynamicCategory);
    }
};

                         // ==========================
                         // class TestAccessByCategory
                         // ==========================

class TestAccessByCategory {
  public:
    // CREATORS
    TestAccessByCategory() {}

    // ACCESSORS
    template <class TYPE>
    void run(int         line,
             const TYPE& object,
             int         rc,
             const typename bdlat_NullableValueFunctions::ValueType<TYPE>::Type
                                       *expectedAddress,
             bdlat_TypeCategory::Value  expectedCategory) const
    {
        typedef typename bdlat_NullableValueFunctions::ValueType<TYPE>::Type
            ValueType;

        GetCategoryAccessor<ValueType> accessor(rc);

        const int result =
             bdlat::NullableValueUtil::accessValueByCategory(object, accessor);
        LOOP1_ASSERT_EQ(line, rc, result);
        if (0 != result) {
            return;                                                   // RETURN
        }

        LOOP1_ASSERT_EQ(line, accessor.category(), expectedCategory);
        LOOP1_ASSERT_EQ(line, accessor.elementPtr(), expectedAddress);
    }
};

                       // ==============================
                       // class TestManipulateByCategory
                       // ==============================

class TestManipulateByCategory {
  public:
    // CREATORS
    TestManipulateByCategory() {}

    // ACCESSORS
    template <class TYPE>
    void run(int   line,
             TYPE *object,
             int   rc,
             typename bdlat_NullableValueFunctions::ValueType<TYPE>::Type
                                       *expectedAddress,
             bdlat_TypeCategory::Value  expectedCategory) const
    {
        typedef typename bdlat_NullableValueFunctions::ValueType<TYPE>::Type
            ValueType;

        GetCategoryManipulator<ValueType> manipulator(rc);

        const int result = bdlat::NullableValueUtil::manipulateValueByCategory(
                                                                  object,
                                                                  manipulator);
        LOOP1_ASSERT_EQ(line, rc, result);
        if (0 != result) {
            return;                                                   // RETURN
        }

        LOOP1_ASSERT_EQ(line, manipulator.category(), expectedCategory);
        LOOP1_ASSERT_EQ(line, manipulator.elementPtr(), expectedAddress);
    }
};

}  // close namespace u
}  // close unnamed namespace
}  // close enterprise namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test = argc > 1 ? atoi(argv[1]) : 0;

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

    switch (test) { case 0:
      case 4: {
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

        using namespace usage;

        example();

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'manipulateValueByCategory'
        //   This case tests the 'manipulateValueByCategory' function.
        //
        // Concerns:
        //: 1 The type category supplied to the manipulator functor corresponds
        //:   is the dynamic type category of the element.
        //:
        //: 2 The element pointer supplied to the manipulator functor addresses
        //:   the held value.
        //:
        //: 3 If the manipulator functor returns 0 then 0 is returned,
        //:   otherwise a non-zero value is returned.
        //:
        //: 4 Passing a non-nullable-value violates an assertion.
        //:
        //: 5 Passing a null value violates an assertion.
        //
        // Plan:
        //: 1 For each type category, create a 'bdlb::NullableValue' object
        //:   holding an object with that type category, and verify that a
        //:   manipultor functor is invoked with that category.
        //:
        //: 2 Similarly, verify that a manipulator functor is invoked with
        //:   the correct address.
        //:
        //: 3 Configure manipulator functors to return a non-zero value, and
        //:   verify that the same value is returned.
        //:
        //: 4 Pass objects with a dynamic, non-nullable-value type category and
        //:   verify that an assertion is violated.
        //:
        //: 5 Attempt to manipulate a null value and verify that an assertion
        //:   is violated.
        //
        // Testing
        //   manipulateValueByCategory(const TYPE&, MANIPULATOR&, int);
        // --------------------------------------------------------------------

        const u::TestManipulateByCategory TEST;

        typedef u::TestArray          Array;
        typedef u::TestChoice         Choice;
        typedef u::TestCustomizedType Customized;
        typedef u::TestDynamicType    Dynamic;
        typedef u::TestEnumeration    Enumeration;
        typedef u::TestNullableValue  Nullable;
        typedef u::TestSequence       Sequence;
        typedef u::TestSimple         Simple;

        typedef bdlb::NullableValue<Array>       NArray;
        typedef bdlb::NullableValue<Choice>      NChoice;
        typedef bdlb::NullableValue<Customized>  NCustomized;
        typedef bdlb::NullableValue<Dynamic>     NDynamic;
        typedef bdlb::NullableValue<Enumeration> NEnumeration;
        typedef bdlb::NullableValue<Nullable>    NNullable;
        typedef bdlb::NullableValue<Sequence>    NSequence;
        typedef bdlb::NullableValue<Simple>      NSimple;

        typedef bdlat_TypeCategory Cat;

        const Cat::Value ARRAY       = Cat::e_ARRAY_CATEGORY;
        const Cat::Value CHOICE      = Cat::e_CHOICE_CATEGORY;
        const Cat::Value CUSTOMIZED  = Cat::e_CUSTOMIZED_TYPE_CATEGORY;
        const Cat::Value ENUMERATION = Cat::e_ENUMERATION_CATEGORY;
        const Cat::Value NULLABLE    = Cat::e_NULLABLE_VALUE_CATEGORY;
        const Cat::Value SEQUENCE    = Cat::e_SEQUENCE_CATEGORY;
        const Cat::Value SIMPLE      = Cat::e_SIMPLE_CATEGORY;
        const Cat::Value NA          = Cat::e_SIMPLE_CATEGORY;

        const Array       array;
        const Choice      choice;
        const Customized  customized;
        const Dynamic     dynArray(ARRAY);
        const Dynamic     dynChoice(CHOICE);
        const Dynamic     dynCustomized(CUSTOMIZED);
        const Dynamic     dynEnumeration(ENUMERATION);
        const Dynamic     dynNullable(NULLABLE);
        const Dynamic     dynSequence(SEQUENCE);
        const Dynamic     dynSimple(SIMPLE);
        const Enumeration enumeration;
        const Nullable    nullable;
        const Sequence    sequence;
        const Simple      simple;

        NArray       array0;
        NArray       array1(array);
        NChoice      choice0;
        NChoice      choice1(choice);
        NCustomized  customized0;
        NCustomized  customized1(customized);
        NDynamic     dyn0;
        NDynamic     dynArray1(dynArray);
        NDynamic     dynChoice1(dynChoice);
        NDynamic     dynCustomized1(dynCustomized);
        NDynamic     dynEnumeration1(dynEnumeration);
        NDynamic     dynNullable1(dynNullable);
        NDynamic     dynSequence1(dynSequence);
        NDynamic     dynSimple1(dynSimple);
        NEnumeration enumeration0;
        NEnumeration enumeration1(enumeration);
        NNullable    nullable0;
        NNullable    nullable1(nullable);
        NSequence    sequence0;
        NSequence    sequence1(sequence);
        NSimple      simple0;
        NSimple      simple1(simple);

        const u::GetValuePtr V;
        const u::AnyNullPtr  null;

        //                                FUNCTOR RETURN CODE
        //                               .-------------------
        //      LINE      OBJECT        /  EXPECTED ELEM ADDR  CATEGORY
        //       --- ----------------- --- ------------------- -----------
        TEST.run(L_, &array1,           0, V(&array1),         ARRAY);
        TEST.run(L_, &array1,          -3, null,               NA);
        TEST.run(L_, &choice1,          0, V(&choice1),        CHOICE);
        TEST.run(L_, &choice1,         -3, null,               NA);
        TEST.run(L_, &customized1,      0, V(&customized1),    CUSTOMIZED);
        TEST.run(L_, &customized1,     -3, null,               NA);
        TEST.run(L_, &dynArray1,        0, V(&dynArray1),      ARRAY);
        TEST.run(L_, &dynArray1,       -3, null,               NA);
        TEST.run(L_, &dynChoice1,       0, V(&dynChoice1),     CHOICE);
        TEST.run(L_, &dynChoice1,      -3, null,               NA);
        TEST.run(L_, &dynCustomized1,   0, V(&dynCustomized1), CUSTOMIZED);
        TEST.run(L_, &dynCustomized1,  -3, null,               NA);
        TEST.run(L_, &dynEnumeration1,  0, V(&dynEnumeration1),ENUMERATION);
        TEST.run(L_, &dynEnumeration1, -3, null,               NA);
        TEST.run(L_, &dynNullable1,     0, V(&dynNullable1),   NULLABLE);
        TEST.run(L_, &dynNullable1,    -3, null,               NA);
        TEST.run(L_, &dynSequence1,     0, V(&dynSequence1),   SEQUENCE);
        TEST.run(L_, &dynSequence1,    -3, null,               NA);
        TEST.run(L_, &dynSimple1,       0, V(&dynSimple1),     SIMPLE);
        TEST.run(L_, &dynSimple1,      -3, null,               NA);
        TEST.run(L_, &enumeration1,     0, V(&enumeration1),   ENUMERATION);
        TEST.run(L_, &enumeration1,    -3, null,               NA);
        TEST.run(L_, &nullable1,        0, V(&nullable1),      NULLABLE);
        TEST.run(L_, &nullable1,       -3, null,               NA);
        TEST.run(L_, &sequence1,        0, V(&sequence1),      SEQUENCE);
        TEST.run(L_, &sequence1,       -3, null,               NA);
        TEST.run(L_, &simple1,          0, V(&simple1),        SIMPLE);
        TEST.run(L_, &simple1,         -3, null,               NA);

#ifdef BDE_BUILD_TARGET_EXC
        {
            bsls::AssertTestHandlerGuard g;

            Dynamic dynArray(ARRAY);
            Dynamic dynChoice(CHOICE);
            Dynamic dynCustomized(CUSTOMIZED);
            Dynamic dynEnumeration(ENUMERATION);
            Dynamic dynNullable(NULLABLE);
            Dynamic dynSequence(SEQUENCE);
            Dynamic dynSimple(SIMPLE);

            ASSERT_FAIL(TEST.run(L_, &dynArray,             0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &dynChoice,            0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &dynCustomized,        0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &dynEnumeration,       0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &dynNullable,          0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &dynSequence,          0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &dynSimple,            0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &array0,               0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &choice0,              0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &customized0,          0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &enumeration0,         0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &nullable0,            0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &sequence0,            0, null, NA));
            ASSERT_FAIL(TEST.run(L_, &simple0,              0, null, NA));
        }
#endif

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'accessValueByCategory'
        //   This case tests the 'accessValueByCategory' function.
        //
        // Concerns:
        //: 1 The type category supplied to the accessor functor corresponds
        //:   is the dynamic type category of the element.
        //:
        //: 2 The element reference supplied to the accessor functor is a
        //:   reference to the held value.
        //:
        //: 3 If the accessor functor returns 0 then 0 is returned, otherwise
        //:   a non-zero value is returned.
        //:
        //: 4 Passing a non-nullable-value violates an assertion.
        //:
        //: 5 Passing a null value violates an assertion.
        //
        // Plan:
        //: 1 For each type category, create a 'bdlb::NullableValue' object
        //:   holding an object with that type category, and verify that an
        //:   accessor functor is invoked with that category.
        //:
        //: 2 Similarly, verify that an accessor functor is invoked with
        //:   the correct address.
        //:
        //: 3 Configure accessor functors to return a non-zero value, and
        //:   verify that the same value is returned.
        //:
        //: 4 Pass objects with a dynamic, non-nullable-value type category and
        //:   verify that an assertion is violated.
        //:
        //: 5 Attempt to access a null value and verify that an assertion is
        //:   violated.
        //
        // Testing
        //   accessValueByCategory(const TYPE&, ACCESSOR&, int);
        // --------------------------------------------------------------------

        const u::TestAccessByCategory TEST;

        typedef u::TestArray          Array;
        typedef u::TestChoice         Choice;
        typedef u::TestCustomizedType Customized;
        typedef u::TestDynamicType    Dynamic;
        typedef u::TestEnumeration    Enumeration;
        typedef u::TestNullableValue  Nullable;
        typedef u::TestSequence       Sequence;
        typedef u::TestSimple         Simple;

        typedef bdlb::NullableValue<Array>       NArray;
        typedef bdlb::NullableValue<Choice>      NChoice;
        typedef bdlb::NullableValue<Customized>  NCustomized;
        typedef bdlb::NullableValue<Dynamic>     NDynamic;
        typedef bdlb::NullableValue<Enumeration> NEnumeration;
        typedef bdlb::NullableValue<Nullable>    NNullable;
        typedef bdlb::NullableValue<Sequence>    NSequence;
        typedef bdlb::NullableValue<Simple>      NSimple;

        typedef bdlat_TypeCategory Cat;

        const Cat::Value ARRAY       = Cat::e_ARRAY_CATEGORY;
        const Cat::Value CHOICE      = Cat::e_CHOICE_CATEGORY;
        const Cat::Value CUSTOMIZED  = Cat::e_CUSTOMIZED_TYPE_CATEGORY;
        const Cat::Value ENUMERATION = Cat::e_ENUMERATION_CATEGORY;
        const Cat::Value NULLABLE    = Cat::e_NULLABLE_VALUE_CATEGORY;
        const Cat::Value SEQUENCE    = Cat::e_SEQUENCE_CATEGORY;
        const Cat::Value SIMPLE      = Cat::e_SIMPLE_CATEGORY;
        const Cat::Value NA          = Cat::e_SIMPLE_CATEGORY;

        const Array       array;
        const Choice      choice;
        const Customized  customized;
        const Dynamic     dynArray(ARRAY);
        const Dynamic     dynChoice(CHOICE);
        const Dynamic     dynCustomized(CUSTOMIZED);
        const Dynamic     dynEnumeration(ENUMERATION);
        const Dynamic     dynNullable(NULLABLE);
        const Dynamic     dynSequence(SEQUENCE);
        const Dynamic     dynSimple(SIMPLE);
        const Enumeration enumeration;
        const Nullable    nullable;
        const Sequence    sequence;
        const Simple      simple;

        const NArray       array0;
        const NArray       array1(array);
        const NChoice      choice0;
        const NChoice      choice1(choice);
        const NCustomized  customized0;
        const NCustomized  customized1(customized);
        const NDynamic     dyn0;
        const NDynamic     dynArray1(dynArray);
        const NDynamic     dynChoice1(dynChoice);
        const NDynamic     dynCustomized1(dynCustomized);
        const NDynamic     dynEnumeration1(dynEnumeration);
        const NDynamic     dynNullable1(dynNullable);
        const NDynamic     dynSequence1(dynSequence);
        const NDynamic     dynSimple1(dynSimple);
        const NEnumeration enumeration0;
        const NEnumeration enumeration1(enumeration);
        const NNullable    nullable0;
        const NNullable    nullable1(nullable);
        const NSequence    sequence0;
        const NSequence    sequence1(sequence);
        const NSimple      simple0;
        const NSimple      simple1(simple);

        const u::GetValuePtr V;
        const u::AnyNullPtr  null;

        //                                FUNCTOR RETURN CODE
        //                               .-------------------
        //      LINE      OBJECT        /  EXPECTED ELEM ADDR  CATEGORY
        //       --- ----------------- --- ------------------- -----------
        TEST.run(L_, array1,           0, V(array1),          ARRAY);
        TEST.run(L_, array1,          -3, null,               NA);
        TEST.run(L_, choice1,          0, V(choice1),         CHOICE);
        TEST.run(L_, choice1,         -3, null,               NA);
        TEST.run(L_, customized1,      0, V(customized1),     CUSTOMIZED);
        TEST.run(L_, customized1,     -3, null,               NA);
        TEST.run(L_, dynArray1,        0, V(dynArray1),       ARRAY);
        TEST.run(L_, dynArray1,       -3, null,               NA);
        TEST.run(L_, dynChoice1,       0, V(dynChoice1),      CHOICE);
        TEST.run(L_, dynChoice1,      -3, null,               NA);
        TEST.run(L_, dynCustomized1,   0, V(dynCustomized1),  CUSTOMIZED);
        TEST.run(L_, dynCustomized1,  -3, null,               NA);
        TEST.run(L_, dynEnumeration1,  0, V(dynEnumeration1), ENUMERATION);
        TEST.run(L_, dynEnumeration1, -3, null,               NA);
        TEST.run(L_, dynNullable1,     0, V(dynNullable1),    NULLABLE);
        TEST.run(L_, dynNullable1,    -3, null,               NA);
        TEST.run(L_, dynSequence1,     0, V(dynSequence1),    SEQUENCE);
        TEST.run(L_, dynSequence1,    -3, null,               NA);
        TEST.run(L_, dynSimple1,       0, V(dynSimple1),      SIMPLE);
        TEST.run(L_, dynSimple1,      -3, null,               NA);
        TEST.run(L_, enumeration1,     0, V(enumeration1),    ENUMERATION);
        TEST.run(L_, enumeration1,    -3, null,               NA);
        TEST.run(L_, nullable1,        0, V(nullable1),       NULLABLE);
        TEST.run(L_, nullable1,       -3, null,               NA);
        TEST.run(L_, sequence1,        0, V(sequence1),       SEQUENCE);
        TEST.run(L_, sequence1,       -3, null,               NA);
        TEST.run(L_, simple1,          0, V(simple1),         SIMPLE);
        TEST.run(L_, simple1,         -3, null,               NA);

#ifdef BDE_BUILD_TARGET_EXC
        bsls::AssertTestHandlerGuard g;

        ASSERT_FAIL(TEST.run(L_, Dynamic(ARRAY),       0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(CHOICE),      0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(CUSTOMIZED),  0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(ENUMERATION), 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(NULLABLE),    0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(SEQUENCE),    0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(SIMPLE),      0, null, NA));
        ASSERT_FAIL(TEST.run(L_, array0,               0, null, NA));
        ASSERT_FAIL(TEST.run(L_, choice0,              0, null, NA));
        ASSERT_FAIL(TEST.run(L_, customized0,          0, null, NA));
        ASSERT_FAIL(TEST.run(L_, enumeration0,         0, null, NA));
        ASSERT_FAIL(TEST.run(L_, nullable0,            0, null, NA));
        ASSERT_FAIL(TEST.run(L_, sequence0,            0, null, NA));
        ASSERT_FAIL(TEST.run(L_, simple0,              0, null, NA));
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //   This case tests the testing apparatus.
        //
        // Concerns:
        //: 1 The 'u::Test*' types have the static and dynamic type category
        //:   of their namesake.
        //:
        //: 2 Objects of 'u::TestDyanmicType' type have 'dynamic type'
        //:   static type category, and the dynamic type category matching
        //:   the category enumerator supplied on construction.
        //
        // Plan:
        //: 1 For all 'u::Test*' types, verify their type category matches
        //:   their namesake.
        //:
        //: 2 Instantiate 'u::TestDynamicType' objects with each category
        //:   enumerator and verify that the type categories of the objects
        //:   match the enumerator with which they were constructed.
        //
        // Testing:
        //   TEST APPARATUS
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
                      << "TEST APPARATUS" << bsl::endl
                      << "==============" << bsl::endl;
        }

        const u::TestCategory TEST;

        typedef bdlat_TypeCategory Cat;

        const Cat::Value DYNAMIC     = Cat::e_DYNAMIC_CATEGORY;
        const Cat::Value ARRAY       = Cat::e_ARRAY_CATEGORY;
        const Cat::Value CHOICE      = Cat::e_CHOICE_CATEGORY;
        const Cat::Value CUSTOMIZED  = Cat::e_CUSTOMIZED_TYPE_CATEGORY;
        const Cat::Value ENUMERATION = Cat::e_ENUMERATION_CATEGORY;
        const Cat::Value NULLABLE    = Cat::e_NULLABLE_VALUE_CATEGORY;
        const Cat::Value SEQUENCE    = Cat::e_SEQUENCE_CATEGORY;
        const Cat::Value SIMPLE      = Cat::e_SIMPLE_CATEGORY;

        typedef u::TestArray          Array;
        typedef u::TestChoice         Choice;
        typedef u::TestCustomizedType Customized;
        typedef u::TestDynamicType    Dynamic;
        typedef u::TestEnumeration    Enumeration;
        typedef u::TestNullableValue  Nullable;
        typedef u::TestSequence       Sequence;
        typedef u::TestSimple         Simple;

        //     LINE  OBJECT                STATIC CAT   DYNAMIC CAT
        //       --- --------------------- ------------ ----------
        TEST.run(L_, Dynamic(ARRAY),       DYNAMIC,     ARRAY      );
        TEST.run(L_, Dynamic(CHOICE),      DYNAMIC,     CHOICE     );
        TEST.run(L_, Dynamic(CUSTOMIZED),  DYNAMIC,     CUSTOMIZED );
        TEST.run(L_, Dynamic(ENUMERATION), DYNAMIC,     ENUMERATION);
        TEST.run(L_, Dynamic(NULLABLE),    DYNAMIC,     NULLABLE   );
        TEST.run(L_, Dynamic(SEQUENCE),    DYNAMIC,     SEQUENCE   );
        TEST.run(L_, Dynamic(SIMPLE),      DYNAMIC,     SIMPLE     );
        TEST.run(L_, Array(),              ARRAY,       ARRAY      );
        TEST.run(L_, Choice(),             CHOICE,      CHOICE     );
        TEST.run(L_, Customized(),         CUSTOMIZED,  CUSTOMIZED );
        TEST.run(L_, Enumeration(),        ENUMERATION, ENUMERATION);
        TEST.run(L_, Nullable(),           NULLABLE,    NULLABLE   );
        TEST.run(L_, Sequence(),           SEQUENCE,    SEQUENCE   );
        TEST.run(L_, Simple(),             SIMPLE,      SIMPLE     );
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
