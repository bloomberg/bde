// bdlat_arrayutil.t.cpp                                              -*-C++-*-
#include <bdlat_arrayutil.h>

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
#include <bsl_functional.h>

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// This test driver tests each utility operation provided by 'ArrayUtil' in its
// own case.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] static int accessElementByCategory(const TYPE&, ACCESSOR&, int);
// [ 3] static int manipulateElementByCategory(const TYPE&, MANIPULATOR&, int);
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
///Example 1: Accessing an Array Element And Its Category
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we would like to define a function that detects whether an element
// of an array is itself an array, in order to more generally detect nested
// arrays.
//
// First, we need to define an accessor functor per
// {'bdlat_typecategory'|'ACCESSOR' Functors} that will be used to detect
// whether an array element is itself an array:
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
// Then, we can define a utility 'struct', 'MyArrayUtil', that provides a
// function for detecting whether or not an array has an element that is itself
// an array:
//..
    struct MyArrayUtil {

        // CLASS METHODS
        template <class TYPE>
        static int isElementAnArray(bool        *isArray,
                                    const TYPE&  array,
                                    int          index)
            // Load the value 'true' to the object addressed by the specified
            // 'isArray' if the element at the specified 'index' of the
            // specified 'array' has the "array" type category, and load
            // the value 'false' otherwise.  Return 0 on success, and a
            // non-zero value otherwise.  If a non-zero value is returned,
            // the value loaded to 'isArray' is unspecified.  The behavior is
            // undefined unless the specified 'array' has the "array" type
            // category, '0 <= index', and
            // 'index < bdlat_ArrayFunctions::size(array)'.
        {
            BSLS_ASSERT(bdlat_TypeCategoryFunctions::select(array) ==
                        bdlat_TypeCategory::e_ARRAY_CATEGORY);
            BSLS_ASSERT(0 <= index);
            BSLS_ASSERT(static_cast<bsl::size_t>(index) <
                        bdlat_ArrayFunctions::size(array));

            MyArrayDetector detector;
            int rc = bdlat::ArrayUtil::accessElementByCategory(array,
                                                               detector,
                                                               index);
            if (0 != rc) {
                return -1;                                            // RETURN
            }

            *isArray = detector.didVisitArray();
            return 0;
        }
    };
//..
// Finally, we can use this utility to detect whether elements of array types
// are themselves arrays:
//..
    void example()
    {
        bsl::vector<int> vectorA;
        vectorA.push_back(42);

        bool isArray = false;
        int rc = MyArrayUtil::isElementAnArray(&isArray, vectorA, 0);

        ASSERT(0 == rc);
        ASSERT(! isArray);

        bsl::vector<bsl::vector<int> > vectorB;
        vectorB.push_back(bsl::vector<int>());

        rc = MyArrayUtil::isElementAnArray(&isArray, vectorB, 0);

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

                             // ==================
                             // class ArrayElement
                             // ==================

class ArrayElement {
  public:
    // CREATORS
    ArrayElement() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_ArrayFunctions {

template <>
struct IsArray<u::ArrayElement> : bsl::true_type {
};

}  // close namespace bdlat_ArrayFunctions

namespace {
namespace u {

                            // ===================
                            // class ChoiceElement
                            // ===================

class ChoiceElement {
  public:
    // CREATORS
    ChoiceElement() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_ChoiceFunctions {

template <>
struct IsChoice<u::ChoiceElement> : bsl::true_type {
};

}  // close namespace bdlat_ChoiceFunctions

namespace {
namespace u {

                        // ===========================
                        // class CustomizedTypeElement
                        // ===========================

class CustomizedTypeElement {
  public:
    // CREATORS
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_CustomizedTypeFunctions {

template <>
struct IsCustomizedType<u::CustomizedTypeElement> : bsl::true_type {
};

}  // close namespace bdlat_CustomizedTypeFunctions

namespace {
namespace u {

                          // ========================
                          // class DynamicTypeElement
                          // ========================

class DynamicTypeElement {
  public:
    // PUBLIC DATA
    bdlat_TypeCategory::Value d_category;

    // CREATORS
    explicit DynamicTypeElement(bdlat_TypeCategory::Value category)
    : d_category(category)
    {
    }
};

// TRAITS
bdlat_TypeCategory::Value
bdlat_typeCategorySelect(const DynamicTypeElement& object)
{
    return object.d_category;
}

template <class MANIPULATOR>
int bdlat_arrayManipulateElement(DynamicTypeElement *, MANIPULATOR&, int)
{
    return -1;
}

template <class ACCESSOR>
int bdlat_arrayAccessElement(const DynamicTypeElement&, ACCESSOR&, int)
{
    return -1;
}

BSLA_MAYBE_UNUSED bsl::size_t bdlat_arraySize(const DynamicTypeElement&)
{
    return 0;
}

}  // close namespace u
}  // close unnamed namespace

template <>
struct bdlat_TypeCategoryDeclareDynamic<u::DynamicTypeElement>
: bsl::true_type {
};

namespace bdlat_ArrayFunctions {

template <>
struct IsArray<u::DynamicTypeElement> : bsl::true_type {
};

template <>
struct ElementType<u::DynamicTypeElement> {
    typedef int Type;
};

}  // close namespace bdlat_ArrayFunctions

namespace bdlat_ChoiceFunctions {

template <>
struct IsChoice<u::DynamicTypeElement> : bsl::true_type {
};

}  // close namespace bdlat_ChoiceFunctions

namespace bdlat_CustomizedTypeFunctions {

template <>
struct IsCustomizedType<u::DynamicTypeElement> : bsl::true_type {
};

}  // close namespace bdlat_CustomizedTypeFunctions

namespace bdlat_EnumFunctions {

template <>
struct IsEnumeration<u::DynamicTypeElement> : bsl::true_type {
};

}  // close namespace bdlat_EnumFunctions

namespace bdlat_NullableValueFunctions {

template <>
struct IsNullableValue<u::DynamicTypeElement> : bsl::true_type {
};

}  // close namespace bdlat_NullableValueFunctions

namespace bdlat_SequenceFunctions {

template <>
struct IsSequence<u::DynamicTypeElement> : bsl::true_type {
};

}  // close namespace bdlat_SequenceFunctions

namespace {
namespace u {

                          // ========================
                          // class EnumerationElement
                          // ========================

class EnumerationElement {
  public:
    EnumerationElement() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_EnumFunctions {

template <>
struct IsEnumeration<u::EnumerationElement> : bsl::true_type {
};

}  // close namespace bdlat_EnumFunctions

namespace {
namespace u {

                         // ==========================
                         // class NullableValueElement
                         // ==========================

class NullableValueElement {
  public:
    NullableValueElement() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_NullableValueFunctions {

template <>
struct IsNullableValue<u::NullableValueElement> : bsl::true_type {
};

}  // close namespace bdlat_NullableValueFunctions

namespace {
namespace u {

                           // =====================
                           // class SequenceElement
                           // =====================

class SequenceElement {
  public:
    SequenceElement() {}
};

}  // close namespace u
}  // close unnamed namespace

// TRAITS
namespace bdlat_SequenceFunctions {

template <>
struct IsSequence<u::SequenceElement> : bsl::true_type {
};

}  // close namespace bdlat_SequenceFunctions

namespace {
namespace u {

                            // ===================
                            // class SimpleElement
                            // ===================

class SimpleElement {
  public:
    SimpleElement() {}
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

                            // ====================
                            // class GenerateVector
                            // ====================

class GenerateVector {
  public:
    // CREATORS
    GenerateVector() {}

    // ACCESSORS
    template <class ELEMENT_TYPE>
    bsl::vector<ELEMENT_TYPE> gen() const
    {
        bsl::vector<ELEMENT_TYPE> result;
        return result;
    }

    template <class ELEMENT_TYPE>
    bsl::vector<ELEMENT_TYPE> gen(const ELEMENT_TYPE& element) const
    {
        bsl::vector<ELEMENT_TYPE> result;
        result.push_back(element);
        return result;
    }

    template <class ELEMENT_TYPE>
    bsl::vector<ELEMENT_TYPE> gen(const ELEMENT_TYPE& element0,
                                  const ELEMENT_TYPE& element1) const
    {
        bsl::vector<ELEMENT_TYPE> result;
        result.push_back(element0);
        result.push_back(element1);
        return result;
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
    bdlat_TypeCategory::Value category() const
    {
        return d_category;
    }

    const ELEMENT_TYPE *elementPtr() const
    {
        return d_element_p;
    }
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
    TestCategory()
    {
    }

    // ACCESSORS
    template <class TYPE>
    void run(int line,
             const TYPE& object,
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
    template <class ARRAY_TYPE>
    void run(int               line,
             const ARRAY_TYPE& array,
             int               index,
             int               rc,
             const typename bdlat_ArrayFunctions::ElementType<ARRAY_TYPE>::Type
                                       *expectedAddress,
             bdlat_TypeCategory::Value  expectedCategory) const
    {
        typedef typename bdlat_ArrayFunctions::ElementType<ARRAY_TYPE>::Type
            ElementType;

        GetCategoryAccessor<ElementType> accessor(rc);

        const int result = bdlat::ArrayUtil::accessElementByCategory(array,
                                                                     accessor,
                                                                     index);
        LOOP1_ASSERT_EQ(line, result, rc);
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
    template <class ARRAY_TYPE>
    void run(int         line,
             ARRAY_TYPE *array,
             int         index,
             int         rc,
             typename bdlat_ArrayFunctions::ElementType<ARRAY_TYPE>::Type
                                       *expectedAddress,
             bdlat_TypeCategory::Value  expectedCategory) const
    {
        typedef typename bdlat_ArrayFunctions::ElementType<ARRAY_TYPE>::Type
            ElementType;

        GetCategoryManipulator<ElementType> manipulator(rc);

        const int result = bdlat::ArrayUtil::manipulateElementByCategory(
                                                                   array,
                                                                   manipulator,
                                                                   index);
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
        // TESTING 'manipulateElementByCategory'
        //   This case tests the 'manipulateElementByCategory' function.
        //
        // Concerns:
        //: 1 The type category supplied to the manipulator functor corresponds
        //:   is the dynamic type category of the element.
        //:
        //: 2 The element pointer supplied to the manipulator functor
        //:   addresses the array element at the specified index.
        //:
        //: 3 If the manipulator functor returns 0 then 0 is returned,
        //:   otherwise a non-zero value is returned.
        //:
        //: 4 Passing a non-array violates an assertion.
        //:
        //: 5 Passing an index less than 0 violates an assertion.
        //:
        //: 6 Passing an index greater than the size of the array violates an
        //:   assertion.
        //
        // Plan:
        //: 1 For each type category, create a 'bsl::vector' object holding
        //:   a single element with that type category, and verify that a
        //:   manipulator functor is invoked with that category.
        //:
        //: 2 Similarly, verify that a manipulator functor is invoked with
        //:   the correct element address.
        //:
        //: 3 Configure manipulator functors to return a non-zero value, and
        //:   verify that the same value is returned.
        //:
        //: 4 Pass objects with a dynamic, non-array type category and verify
        //:   that an assertion is violated.
        //:
        //: 5 Attempt to access an element at a negative index and verify that
        //:   an assertion is violated.
        //:
        //: 6 Attempt to access an element at an index equal to the size of
        //:   the array and verify that an assertion is violated.
        //
        // Testing
        //   manipulateElementByCategory(const TYPE&, MANIPULATOR&, int);
        // --------------------------------------------------------------------

        const u::TestManipulateByCategory TEST;

        typedef u::ArrayElement          Array;
        typedef u::ChoiceElement         Choice;
        typedef u::CustomizedTypeElement Customized;
        typedef u::DynamicTypeElement    Dynamic;
        typedef u::EnumerationElement    Enumeration;
        typedef u::NullableValueElement  Nullable;
        typedef u::SequenceElement       Sequence;
        typedef u::SimpleElement         Simple;

        typedef bsl::vector<Array>       VArray;
        typedef bsl::vector<Choice>      VChoice;
        typedef bsl::vector<Customized>  VCustomized;
        typedef bsl::vector<Dynamic>     VDynamic;
        typedef bsl::vector<Enumeration> VEnumeration;
        typedef bsl::vector<Nullable>    VNullable;
        typedef bsl::vector<Sequence>    VSequence;
        typedef bsl::vector<Simple>      VSimple;

        typedef bdlat_TypeCategory Cat;

        const Cat::Value ARRAY       = Cat::e_ARRAY_CATEGORY;
        const Cat::Value CHOICE      = Cat::e_CHOICE_CATEGORY;
        const Cat::Value CUSTOMIZED  = Cat::e_CUSTOMIZED_TYPE_CATEGORY;
        const Cat::Value ENUMERATION = Cat::e_ENUMERATION_CATEGORY;
        const Cat::Value NULLABLE    = Cat::e_NULLABLE_VALUE_CATEGORY;
        const Cat::Value SEQUENCE    = Cat::e_SEQUENCE_CATEGORY;
        const Cat::Value SIMPLE      = Cat::e_SIMPLE_CATEGORY;
        const Cat::Value NA          = Cat::e_SIMPLE_CATEGORY;

        const u::GenerateVector V;

        VArray       array1          = V.gen(Array());
        VChoice      choice1         = V.gen(Choice());
        VCustomized  customized1     = V.gen(Customized());
        VDynamic     dynArray1       = V.gen(Dynamic(ARRAY));
        VDynamic     dynChoice1      = V.gen(Dynamic(CHOICE));
        VDynamic     dynCustomized1  = V.gen(Dynamic(CUSTOMIZED));
        VDynamic     dynEnumeration1 = V.gen(Dynamic(ENUMERATION));
        VDynamic     dynNullable1    = V.gen(Dynamic(NULLABLE));
        VDynamic     dynSequence1    = V.gen(Dynamic(SEQUENCE));
        VDynamic     dynSimple1      = V.gen(Dynamic(SIMPLE));
        VEnumeration enumeration1    = V.gen(Enumeration());
        VNullable    nullable1       = V.gen(Nullable());
        VSequence    sequence1       = V.gen(Sequence());
        VSimple      simple1         = V.gen(Simple());

        //                               ELEMENT INDEX
        //                              .-------------
        //                              |    FUNCTOR RETURN CODE
        //                              |   .-------------------
        //      LINE      OBJECT        |  /  EXPECTED ELEM ADDR  CATEGORY
        //       --- ----------------- -- --- ------------------- -----------
        TEST.run(L_, &array1,          0,  0, &array1[0],         ARRAY      );
        TEST.run(L_, &array1,          0, -3, &array1[0],         ARRAY      );
        TEST.run(L_, &choice1,         0,  0, &choice1[0],        CHOICE     );
        TEST.run(L_, &choice1,         0, -3, &choice1[0],        CHOICE     );
        TEST.run(L_, &customized1,     0,  0, &customized1[0],    CUSTOMIZED );
        TEST.run(L_, &customized1,     0, -3, &customized1[0],    CUSTOMIZED );
        TEST.run(L_, &dynArray1,       0,  0, &dynArray1[0],      ARRAY      );
        TEST.run(L_, &dynArray1,       0, -3, &dynArray1[0],      ARRAY      );
        TEST.run(L_, &dynChoice1,      0,  0, &dynChoice1[0],     CHOICE     );
        TEST.run(L_, &dynChoice1,      0, -3, &dynChoice1[0],     CHOICE     );
        TEST.run(L_, &dynCustomized1,  0,  0, &dynCustomized1[0], CUSTOMIZED );
        TEST.run(L_, &dynCustomized1,  0, -3, &dynCustomized1[0], CUSTOMIZED );
        TEST.run(L_, &dynEnumeration1, 0,  0, &dynEnumeration1[0],ENUMERATION);
        TEST.run(L_, &dynEnumeration1, 0, -3, &dynEnumeration1[0],ENUMERATION);
        TEST.run(L_, &dynNullable1,    0,  0, &dynNullable1[0],   NULLABLE   );
        TEST.run(L_, &dynNullable1,    0, -3, &dynNullable1[0],   NULLABLE   );
        TEST.run(L_, &dynSequence1,    0,  0, &dynSequence1[0],   SEQUENCE   );
        TEST.run(L_, &dynSequence1,    0, -3, &dynSequence1[0],   SEQUENCE   );
        TEST.run(L_, &dynSimple1,      0,  0, &dynSimple1[0],     SIMPLE     );
        TEST.run(L_, &dynSimple1,      0, -3, &dynSimple1[0],     SIMPLE     );
        TEST.run(L_, &enumeration1,    0,  0, &enumeration1[0],   ENUMERATION);
        TEST.run(L_, &enumeration1,    0, -3, &enumeration1[0],   ENUMERATION);
        TEST.run(L_, &nullable1,       0,  0, &nullable1[0],      NULLABLE   );
        TEST.run(L_, &nullable1,       0, -3, &nullable1[0],      NULLABLE   );
        TEST.run(L_, &sequence1,       0,  0, &sequence1[0],      SEQUENCE   );
        TEST.run(L_, &sequence1,       0, -3, &sequence1[0],      SEQUENCE   );
        TEST.run(L_, &simple1,         0,  0, &simple1[0],        SIMPLE     );
        TEST.run(L_, &simple1,         0, -3, &simple1[0],        SIMPLE     );

#ifdef BDE_BUILD_TARGET_EXC
        bsls::AssertTestHandlerGuard g;

        const u::AnyNullPtr null;

        Dynamic dynArray(ARRAY);
        Dynamic dynChoice(CHOICE);
        Dynamic dynCustomized(CUSTOMIZED);
        Dynamic dynEnumeration(ENUMERATION);
        Dynamic dynNullable(NULLABLE);
        Dynamic dynSequence(SEQUENCE);
        Dynamic dynSimple(SIMPLE);

        ASSERT_FAIL(TEST.run(L_, &dynArray,              0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynChoice,             0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynCustomized,         0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynEnumeration,        0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynNullable,           0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynSequence,           0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynSimple,             0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &array1,               -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &array1,                1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &choice1,              -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &choice1,               1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &customized1,          -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &customized1,           1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynArray1,            -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynArray1,             1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynChoice1,           -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynChoice1,            1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynCustomized1,       -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynCustomized1,        1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynEnumeration1,      -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynEnumeration1,       1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynNullable1,         -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynNullable1,          1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynSequence1,         -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynSequence1,          1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynSimple1,           -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, &dynSimple1,            1, 0, null, NA));
#endif

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'accessElementByCategory'
        //   This case tests the 'accessElementByCategory' function.
        //
        // Concerns:
        //: 1 The type category supplied to the accessor functor corresponds
        //:   is the dynamic type category of the element.
        //:
        //: 2 The element reference supplied to the accessor functor is a
        //:   reference to the array element at the specified index.
        //:
        //: 3 If the accessor functor returns 0 then 0 is returned, otherwise
        //:   a non-zero value is returned.
        //:
        //: 4 Passing a non-array violates an assertion.
        //:
        //: 5 Passing an index less than 0 violates an assertion.
        //:
        //: 6 Passing an index greater than the size of the array violates an
        //:   assertion.
        //
        // Plan:
        //: 1 For each type category, create a 'bsl::vector' object holding
        //:   a single element with that type category, and verify that an
        //:   accessor functor is invoked with that category.
        //:
        //: 2 Similarly, verify that an accessor functor is invoked with
        //:   the correct element address.
        //:
        //: 3 Configure accessor functors to return a non-zero value, and
        //:   verify that the same value is returned.
        //:
        //: 4 Pass objects with a dynamic, non-array type category and verify
        //:   that an assertion is violated.
        //:
        //: 5 Attempt to access an element at a negative index and verify that
        //:   an assertion is violated.
        //:
        //: 6 Attempt to access an element at an index equal to the size of
        //:   the array and verify that an assertion is violated.
        //
        // Testing
        //   accessElementByCategory(const TYPE&, ACCESSOR&, int);
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
                      << "'accessElementByCategory'" << bsl::endl
                      << "=========================" << bsl::endl;
        }

        const u::TestAccessByCategory TEST;

        typedef u::ArrayElement          Array;
        typedef u::ChoiceElement         Choice;
        typedef u::CustomizedTypeElement Customized;
        typedef u::DynamicTypeElement    Dynamic;
        typedef u::EnumerationElement    Enumeration;
        typedef u::NullableValueElement  Nullable;
        typedef u::SequenceElement       Sequence;
        typedef u::SimpleElement         Simple;

        typedef bsl::vector<Array>       VArray;
        typedef bsl::vector<Choice>      VChoice;
        typedef bsl::vector<Customized>  VCustomized;
        typedef bsl::vector<Dynamic>     VDynamic;
        typedef bsl::vector<Enumeration> VEnumeration;
        typedef bsl::vector<Nullable>    VNullable;
        typedef bsl::vector<Sequence>    VSequence;
        typedef bsl::vector<Simple>      VSimple;

        typedef bdlat_TypeCategory Cat;

        const Cat::Value ARRAY       = Cat::e_ARRAY_CATEGORY;
        const Cat::Value CHOICE      = Cat::e_CHOICE_CATEGORY;
        const Cat::Value CUSTOMIZED  = Cat::e_CUSTOMIZED_TYPE_CATEGORY;
        const Cat::Value ENUMERATION = Cat::e_ENUMERATION_CATEGORY;
        const Cat::Value NULLABLE    = Cat::e_NULLABLE_VALUE_CATEGORY;
        const Cat::Value SEQUENCE    = Cat::e_SEQUENCE_CATEGORY;
        const Cat::Value SIMPLE      = Cat::e_SIMPLE_CATEGORY;
        const Cat::Value NA          = Cat::e_SIMPLE_CATEGORY;

        const u::GenerateVector V;

        VArray       array1          = V.gen(Array());
        VChoice      choice1         = V.gen(Choice());
        VCustomized  customized1     = V.gen(Customized());
        VDynamic     dynArray1       = V.gen(Dynamic(ARRAY));
        VDynamic     dynChoice1      = V.gen(Dynamic(CHOICE));
        VDynamic     dynCustomized1  = V.gen(Dynamic(CUSTOMIZED));
        VDynamic     dynEnumeration1 = V.gen(Dynamic(ENUMERATION));
        VDynamic     dynNullable1    = V.gen(Dynamic(NULLABLE));
        VDynamic     dynSequence1    = V.gen(Dynamic(SEQUENCE));
        VDynamic     dynSimple1      = V.gen(Dynamic(SIMPLE));
        VEnumeration enumeration1    = V.gen(Enumeration());
        VNullable    nullable1       = V.gen(Nullable());
        VSequence    sequence1       = V.gen(Sequence());
        VSimple      simple1         = V.gen(Simple());

        //                               ELEMENT INDEX
        //                              .-------------
        //                              |    FUNCTOR RETURN CODE
        //                              |   .-------------------
        //      LINE      OBJECT        |  /  EXPECTED ELEM ADDR  CATEGORY
        //       --- ----------------- -- --- ------------------- -----------
        TEST.run(L_, array1,           0,  0, &array1[0],         ARRAY      );
        TEST.run(L_, array1,           0, -3, &array1[0],         ARRAY      );
        TEST.run(L_, choice1,          0,  0, &choice1[0],        CHOICE     );
        TEST.run(L_, choice1,          0, -3, &choice1[0],        CHOICE     );
        TEST.run(L_, customized1,      0,  0, &customized1[0],    CUSTOMIZED );
        TEST.run(L_, customized1,      0, -3, &customized1[0],    CUSTOMIZED );
        TEST.run(L_, dynArray1,        0,  0, &dynArray1[0],      ARRAY      );
        TEST.run(L_, dynArray1,        0, -3, &dynArray1[0],      ARRAY      );
        TEST.run(L_, dynChoice1,       0,  0, &dynChoice1[0],     CHOICE     );
        TEST.run(L_, dynChoice1,       0, -3, &dynChoice1[0],     CHOICE     );
        TEST.run(L_, dynCustomized1,   0,  0, &dynCustomized1[0], CUSTOMIZED );
        TEST.run(L_, dynCustomized1,   0, -3, &dynCustomized1[0], CUSTOMIZED );
        TEST.run(L_, dynEnumeration1,  0,  0, &dynEnumeration1[0],ENUMERATION);
        TEST.run(L_, dynEnumeration1,  0, -3, &dynEnumeration1[0],ENUMERATION);
        TEST.run(L_, dynNullable1,     0,  0, &dynNullable1[0],   NULLABLE   );
        TEST.run(L_, dynNullable1,     0, -3, &dynNullable1[0],   NULLABLE   );
        TEST.run(L_, dynSequence1,     0,  0, &dynSequence1[0],   SEQUENCE   );
        TEST.run(L_, dynSequence1,     0, -3, &dynSequence1[0],   SEQUENCE   );
        TEST.run(L_, dynSimple1,       0,  0, &dynSimple1[0],     SIMPLE     );
        TEST.run(L_, dynSimple1,       0, -3, &dynSimple1[0],     SIMPLE     );
        TEST.run(L_, enumeration1 ,    0,  0, &enumeration1[0],   ENUMERATION);
        TEST.run(L_, enumeration1 ,    0, -3, &enumeration1[0],   ENUMERATION);
        TEST.run(L_, nullable1,        0,  0, &nullable1[0],      NULLABLE   );
        TEST.run(L_, nullable1,        0, -3, &nullable1[0],      NULLABLE   );
        TEST.run(L_, sequence1,        0,  0, &sequence1[0],      SEQUENCE   );
        TEST.run(L_, sequence1,        0, -3, &sequence1[0],      SEQUENCE   );
        TEST.run(L_, simple1,          0,  0, &simple1[0],        SIMPLE     );
        TEST.run(L_, simple1,          0, -3, &simple1[0],        SIMPLE     );

#ifdef BDE_BUILD_TARGET_EXC
        bsls::AssertTestHandlerGuard g;

        const u::AnyNullPtr null;

        ASSERT_FAIL(TEST.run(L_, Dynamic(ARRAY),        0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(CHOICE),       0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(CUSTOMIZED),   0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(ENUMERATION),  0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(NULLABLE),     0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(SEQUENCE),     0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, Dynamic(SIMPLE),       0, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, array1,               -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, array1,                1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, choice1,              -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, choice1,               1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, customized1,          -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, customized1,           1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynArray1,            -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynArray1,             1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynChoice1,           -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynChoice1,            1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynCustomized1,       -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynCustomized1,        1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynEnumeration1,      -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynEnumeration1,       1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynNullable1,         -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynNullable1,          1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynSequence1,         -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynSequence1,          1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynSimple1,           -1, 0, null, NA));
        ASSERT_FAIL(TEST.run(L_, dynSimple1,            1, 0, null, NA));
#endif

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING TEST APPARATUS
        //   This case tests the testing apparatus.
        //
        // Concerns:
        //: 1 The 'u::*Element' types have the static and dynamic type category
        //:   of their namesake.
        //:
        //: 2 Objects of 'u::DyanmicTypeElement' type have 'dynamic type'
        //:   static type category, and the dynamic type category matching
        //:   the category enumerator supplied on construction.
        //
        // Plan:
        //: 1 For all 'u::*Element' types, verify their type category matches
        //:   their namesake.
        //:
        //: 2 Instantiate 'u::DynamicTypeElement' objects with each category
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

        typedef u::ArrayElement          Array;
        typedef u::ChoiceElement         Choice;
        typedef u::CustomizedTypeElement Customized;
        typedef u::DynamicTypeElement    Dynamic;
        typedef u::EnumerationElement    Enumeration;
        typedef u::NullableValueElement  Nullable;
        typedef u::SequenceElement       Sequence;
        typedef u::SimpleElement         Simple;

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
