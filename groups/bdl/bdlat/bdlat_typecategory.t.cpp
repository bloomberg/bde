// bdlat_typecategory.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlat_typecategory.h>

#include <bslim_testutil.h>

#include <bslmf_issame.h>
#include <bslmf_nil.h>

#include <bsl_string.h>
#include <bsl_vector.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bdlb_nullablevalue.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//                                  TBD doc
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

struct MyArrayType {
    enum { SELECTION = bdlat_TypeCategory::e_ARRAY_CATEGORY };
    typedef bdlat_TypeCategory::Array Category;
};

namespace BloombergLP {
namespace bdlat_ArrayFunctions {
    template <>
    struct IsArray<MyArrayType> : public bsl::true_type {
    };
}  // close namespace bdlat_ArrayFunctions
}  // close enterprise namespace

struct MyChoiceType {
    enum { SELECTION = bdlat_TypeCategory::e_CHOICE_CATEGORY };
    typedef bdlat_TypeCategory::Choice Category;
};

namespace BloombergLP {
namespace bdlat_ChoiceFunctions {
    template <>
    struct IsChoice<MyChoiceType> : public bsl::true_type {
    };
}  // close namespace bdlat_ChoiceFunctions
}  // close enterprise namespace

struct MyCustomizedType {
    enum { SELECTION = bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY };
    typedef bdlat_TypeCategory::CustomizedType Category;
};

namespace BloombergLP {
namespace bdlat_CustomizedTypeFunctions {
    template <>
    struct IsCustomizedType<MyCustomizedType> : public bsl::true_type {
    };
}  // close namespace bdlat_CustomizedTypeFunctions
}  // close enterprise namespace

struct MyEnumerationType {
    enum { SELECTION = bdlat_TypeCategory::e_ENUMERATION_CATEGORY };
    typedef bdlat_TypeCategory::Enumeration Category;
};

namespace BloombergLP {
namespace bdlat_EnumFunctions {
    template <>
    struct IsEnumeration<MyEnumerationType> : public bsl::true_type {
    };
}  // close namespace bdlat_EnumFunctions
}  // close enterprise namespace

struct MyNullableValueType {
    enum { SELECTION = bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY };
    typedef bdlat_TypeCategory::NullableValue Category;
};

namespace BloombergLP {
namespace bdlat_NullableValueFunctions {
    template <>
    struct IsNullableValue<MyNullableValueType> : public bsl::true_type {
    };
}  // close namespace bdlat_NullableValueFunctions
}  // close enterprise namespace

struct MySequenceType {
    enum { SELECTION = bdlat_TypeCategory::e_SEQUENCE_CATEGORY };
    typedef bdlat_TypeCategory::Sequence Category;
};

namespace BloombergLP {
namespace bdlat_SequenceFunctions {
    template <>
    struct IsSequence<MySequenceType> : public bsl::true_type {
    };
}  // close namespace bdlat_SequenceFunctions
}  // close enterprise namespace


struct MyDeclaredDynamicType {
    enum { SELECTION = 0 };
    typedef bdlat_TypeCategory::DynamicType Category;

    bdlat_TypeCategory::Value d_currentCategory;

    void makeChoice()
    {
        d_currentCategory = bdlat_TypeCategory::e_CHOICE_CATEGORY;
    }

    void makeSequence()
    {
        d_currentCategory = bdlat_TypeCategory::e_SEQUENCE_CATEGORY;
    }
};

namespace BloombergLP {
    template <>
    struct bdlat_TypeCategoryDeclareDynamic<MyDeclaredDynamicType>
    : public bsl::true_type {
    };
}  // close enterprise namespace

bdlat_TypeCategory::Value
bdlat_typeCategorySelect(const MyDeclaredDynamicType& object)
{
    (void)object;
    return object.d_currentCategory;
}

template <class MANIPULATOR>
int bdlat_typeCategoryManipulateChoice(MyDeclaredDynamicType *object,
                                       MANIPULATOR&           manipulator)
{
    (void)object;
    return manipulator(object, bdlat_TypeCategory::Choice());
}

template <class MANIPULATOR>
int bdlat_typeCategoryManipulateSequence(MyDeclaredDynamicType *object,
                                         MANIPULATOR&           manipulator)
{
    (void)object;
    return manipulator(object, bdlat_TypeCategory::Sequence());
}

template <class ACCESSOR>
int bdlat_typeCategoryAccessChoice(const MyDeclaredDynamicType& object,
                                   ACCESSOR&                    accessor)
{
    (void)object;
    return accessor(object, bdlat_TypeCategory::Choice());
}

template <class ACCESSOR>
int bdlat_typeCategoryAccessSequence(const MyDeclaredDynamicType& object,
                                     ACCESSOR&                    accessor)
{
    (void)object;
    return accessor(object, bdlat_TypeCategory::Sequence());
}

struct MyAutoDetectDynamicType {
    enum { SELECTION = 0 };
    typedef bdlat_TypeCategory::DynamicType Category;

    bdlat_TypeCategory::Value d_currentCategory;

    void makeSequence()
    {
        d_currentCategory = bdlat_TypeCategory::e_SEQUENCE_CATEGORY;
    }

    void makeNullableValue()
    {
        d_currentCategory = bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY;
    }
};

namespace BloombergLP {

namespace bdlat_SequenceFunctions {
    template <>
    struct IsSequence<MyAutoDetectDynamicType> : public bsl::true_type {
    };
}  // close namespace bdlat_SequenceFunctions

namespace bdlat_NullableValueFunctions {
    template <>
    struct IsNullableValue<MyAutoDetectDynamicType> : public bsl::true_type {
    };
}  // close namespace bdlat_NullableValueFunctions
}  // close enterprise namespace

bdlat_TypeCategory::Value
bdlat_typeCategorySelect(const MyAutoDetectDynamicType& object)
{
    (void)object;
    return object.d_currentCategory;
}


struct MyManipulator {

    template <class TYPE>
    int operator()(TYPE *object, bslmf::Nil)
    {
        (void)object;
        return -1;
    }

    template <class TYPE>
    int operator()(TYPE *object, bdlat_TypeCategory::DynamicType)
    {
        (void)object;
        return 0;
    }

    template <class TYPE>
    int operator()(TYPE *object, bdlat_TypeCategory::Array)
    {
        (void)object;
        return bdlat_TypeCategory::e_ARRAY_CATEGORY;
    }

    template <class TYPE>
    int operator()(TYPE *object, bdlat_TypeCategory::Choice)
    {
        (void)object;
        return bdlat_TypeCategory::e_CHOICE_CATEGORY;
    }

    template <class TYPE>
    int operator()(TYPE *object, bdlat_TypeCategory::CustomizedType)
    {
        (void)object;
        return bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY;
    }

    template <class TYPE>
    int operator()(TYPE *object, bdlat_TypeCategory::Enumeration)
    {
        (void)object;
        return bdlat_TypeCategory::e_ENUMERATION_CATEGORY;
    }

    template <class TYPE>
    int operator()(TYPE *object, bdlat_TypeCategory::NullableValue)
    {
        (void)object;
        return bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY;
    }

    template <class TYPE>
    int operator()(TYPE *object, bdlat_TypeCategory::Sequence)
    {
        (void)object;
        return bdlat_TypeCategory::e_SEQUENCE_CATEGORY;
    }

    template <class TYPE>
    int operator()(TYPE *object, bdlat_TypeCategory::Simple)
    {
        (void)object;
        return bdlat_TypeCategory::e_SIMPLE_CATEGORY;
    }
};

struct MyAccessor {

    template <class TYPE>
    int operator()(const TYPE& object, bslmf::Nil)
    {
        (void)object;
        return -1;
    }

    template <class TYPE>
    int operator()(const TYPE& object, bdlat_TypeCategory::DynamicType)
    {
        (void)object;
        return 0;
    }

    template <class TYPE>
    int operator()(const TYPE& object, bdlat_TypeCategory::Array)
    {
        (void)object;
        return bdlat_TypeCategory::e_ARRAY_CATEGORY;
    }

    template <class TYPE>
    int operator()(const TYPE& object, bdlat_TypeCategory::Choice)
    {
        (void)object;
        return bdlat_TypeCategory::e_CHOICE_CATEGORY;
    }

    template <class TYPE>
    int operator()(const TYPE& object, bdlat_TypeCategory::CustomizedType)
    {
        (void)object;
        return bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY;
    }

    template <class TYPE>
    int operator()(const TYPE& object, bdlat_TypeCategory::Enumeration)
    {
        (void)object;
        return bdlat_TypeCategory::e_ENUMERATION_CATEGORY;
    }

    template <class TYPE>
    int operator()(const TYPE& object, bdlat_TypeCategory::NullableValue)
    {
        (void)object;
        return bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY;
    }

    template <class TYPE>
    int operator()(const TYPE& object, bdlat_TypeCategory::Sequence)
    {
        (void)object;
        return bdlat_TypeCategory::e_SEQUENCE_CATEGORY;
    }

    template <class TYPE>
    int operator()(const TYPE& object, bdlat_TypeCategory::Simple)
    {
        (void)object;
        return bdlat_TypeCategory::e_SIMPLE_CATEGORY;
    }
};

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Compile-Time Dispatch by Category Type
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The 'bdlat_typecategory' framework provides facilities to control actions
// based on the 'bdlat' type category of the objects being used.  Dispatching
// on type category can be achieved both at compile time and at runtime.
// Depending on that context, different facilities, having different
// restrictions/requirements are used.  There are interesting differences when
// dealing with objects in the "dynamic" type category.
//
// This first example explores compile-time dispatch.  Suppose we have an
// object that is compliant with one of the 'bdlat' type categories and that we
// wish to examine it to the extent of determining into which type category it
// falls and what value it has.
//
// First, we declare a 'printCategoryAndValue' function that has a template
// parameter 'TYPE':
//..
    namespace BloombergLP {
    namespace mine {

    template <class TYPE>
    void printCategoryAndValue(bsl::ostream& stream, const TYPE& object);
        // Print the category of the specified 'object' followed by the value
        // of 'object' to the specified output 'stream'.
//..
// Then, to implement this function, we will use a set of helper functions that
// are overloaded based on the category tag.  The first set of helper functions
// address the category aspect of our assigned goal:
//..
    void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Array)
    {
        stream << "Array";
    }

    void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Choice)
    {
        stream << "Choice";
    }

    void printCategory(bsl::ostream& stream,
                       bdlat_TypeCategory::CustomizedType)
    {
        stream << "CustomizedType";
    }

    void printCategory(bsl::ostream& stream, bdlat_TypeCategory::DynamicType)
    {
        stream << "DynamicType";
    }

    void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Enumeration)
    {
        stream << "Enumeration";
    }

    void printCategory(bsl::ostream& stream, bdlat_TypeCategory::NullableValue)
    {
        stream << "NullableValue";
    }

    void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Sequence)
    {
        stream << "Sequence";
    }

    void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Simple)
    {
        stream << "Simple";
    }
//..
// Next, we implement another helper function template to handle the value
// aspect of our goal:
//..
    template <class TYPE, class CATEGORY>
    void printValue(bsl::ostream& stream,
                    const TYPE&   object,
                    CATEGORY      )
    {
        bdlb::PrintMethods::print(stream, object, 0, -1);
    }

    template <class TYPE>
    void printValue(bsl::ostream&                   stream,
                    const TYPE&                     ,
                    bdlat_TypeCategory::DynamicType )
    {
        stream << "Printing dynamic types requires extra work.";
    }
//..
// Notice that a partial specialization was created for objects falling into
// the "dynamic" category.  Determining the value of such objects will be
// explored in {Example 3}.
//
// Now, we can implement the 'printCategoryAndValue' function in terms of the
// 'printCategory' and 'printValue' helper functions:
//..

    template <class TYPE>
    void printCategoryAndValue(bsl::ostream& stream, const TYPE& object)
    {
        typedef typename
        bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;

        printCategory(stream, TypeCategory());

        stream << ": ";

        printValue(stream, object, TypeCategory());
    }

    }  // close package namespace
    }  // close enterprise namespace
//..
// Finally, we can exercise the 'printCategoryAndValue' function on objects
// that fall in different (non-dynamic) type categories.
//..
    using namespace BloombergLP;

    void runUsageExample1()
    {
        bsl::ostringstream oss;

        int intVal = 123;

        mine::printCategoryAndValue(oss, intVal);
        ASSERT("Simple: 123" == oss.str());
        oss.str("");

        bdlb::NullableValue<int> nullableInt;

        mine::printCategoryAndValue(oss, nullableInt);
        ASSERT("NullableValue: NULL" == oss.str());
        oss.str("");

        nullableInt = 321;

        mine::printCategoryAndValue(oss, nullableInt);
        ASSERT("NullableValue: 321" == oss.str());
        oss.str("");

        bsl::vector<int> vec;

        vec.push_back(123);
        vec.push_back(345);
        vec.push_back(987);

        mine::printCategoryAndValue(oss, vec);
        ASSERT("Array: [ 123 345 987 ]" == oss.str());
    }
//..
//
///Example 2: Run-Time Dispatch by 'bdlat_TypeCategoryUtil'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// For run-time dispatching we can use the utility functions provided by
// 'bdlat_TypeCategoryUtil'.  Suppose we wish to examine the type category and
// value of an arbitrary 'bdlat' compatible object, as we did in {Example 1}.
//
// First, we define 'mine::PrintAccessor', a functor that encapsulates the
// action to be taken:
//..
    namespace BloombergLP {
    namespace mine {

    class PrintAccessor {

        bsl::ostream  *d_stream_p;

      public:
        PrintAccessor(bsl::ostream *stream)
        : d_stream_p(stream) { ASSERT(stream); }

        template <class TYPE>
        int operator()(const TYPE& , bslmf::Nil )
        {
            *d_stream_p << "Nil";
            return -1;
        }

        template <class TYPE>
        int operator()(const TYPE& object, bdlat_TypeCategory::Array)
        {
            *d_stream_p << "Array" << ": ";
            bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
            return 0;
        }

        template <class TYPE>
        int operator()(const TYPE& object, bdlat_TypeCategory::Choice)
        {
            *d_stream_p << "Choice" << ": ";
            bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
            return 0;
        }

        template <class TYPE>
        int operator()(const TYPE& object, bdlat_TypeCategory::CustomizedType)
        {
            *d_stream_p << "CustomizedType" << ": ";
            bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
            return 0;
        }

        template <class TYPE>
        int operator()(const TYPE& object, bdlat_TypeCategory::Enumeration)
        {
            *d_stream_p << "Enumeration" << ": ";
            bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
            return 0;
        }

        template <class TYPE>
        int operator()(const TYPE& object, bdlat_TypeCategory::NullableValue)
        {
            *d_stream_p << "NullableValue" << ": ";
            bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
            return 0;
        }

        template <class TYPE>
        int operator()(const TYPE& object, bdlat_TypeCategory::Sequence)
        {
            *d_stream_p << "Sequence" << ": ";
            bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
            return 0;
        }

        template <class TYPE>
        int operator()(const TYPE& object, bdlat_TypeCategory::Simple)
        {
            *d_stream_p << "Simple" << ": ";
            bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
            return 0;
        }
    };

    }  // close package namespace
    }  // close enterprise namespace
//..
// Notice that this overload set for 'operator()' includes an overload for
// 'bslmf::Nil' (as required) but does *not* include an overload for
// 'bdlat_TypeCategory::DynamicType' which is never reported as a runtime type
// category.
//
// Now, we can simply use 'bdlat_TypeCategoryUtil' to determine the type of a
// given object dispatch control to the corresponding overload of the accessor
// functor:
//..
    using namespace BloombergLP;

    void runUsageExample2()
    {
        bsl::ostringstream oss;
        mine::PrintAccessor accessor(&oss);

        int intVal = 123;

        bdlat_TypeCategoryUtil::accessByCategory(intVal, accessor);
        ASSERT("Simple: 123" == oss.str());
        oss.str("");

        bdlb::NullableValue<int> nullableInt;

        bdlat_TypeCategoryUtil::accessByCategory(nullableInt, accessor);
        ASSERT("NullableValue: NULL" == oss.str());
        oss.str("");

        nullableInt = 321;

        bdlat_TypeCategoryUtil::accessByCategory(nullableInt, accessor);
        ASSERT("NullableValue: 321" == oss.str());
        oss.str("");

        bsl::vector<int> vec;

        vec.push_back(123);
        vec.push_back(345);
        vec.push_back(987);

        bdlat_TypeCategoryUtil::accessByCategory(vec, accessor);
        LOOP_ASSERT(oss.str(), "Array: [ 123 345 987 ]" == oss.str());
        oss.str("");
    }
//..
//
///Example 3: Dynamic (Run-Time) Typing and Dispatch
///- - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we introduce a class that is the 'bdlat' "dyanmic" type
// category and show how its behavior is a generalization of what we have seen
// for the "static" 'bdlat' types.
//
// First, we define a class, 'mine::MyDynamicType', that can hold one of two
// value types: either a 'bsl::vector<char>' or a 'bsl::string'.
//..
    namespace BloombergLP {
    namespace mine {

    class MyDynamicType {
        // This class can represent data in two forms: either a 'bsl::string'
        // or as a 'bsl::vector' of 'char' values.

        // PRIVATE DATA MEMBERS
        bsl::vector<char> d_vectorChar;  // Note: Production code should use a
        bsl::string       d_string;      //       union of object buffers.
        int               d_selector;    // 0 = vectorChar, 1 = string

      public:
        // MANIPULATORS
        void makeVectorChar() { d_selector = 0; }
        void makeString()     { d_selector = 1; }

        bsl::vector<char>& theVectorChar()
                               { ASSERT(isVectorChar()); return d_vectorChar; }
        bsl::string& theString()
                               { ASSERT(isString());     return d_string;     }

        // ACCESSORS
        bool isVectorChar() const { return 0 == d_selector; }
        bool isString()     const { return 1 == d_selector; }

        const bsl::vector<char>& theVectorChar() const
                               { ASSERT(isVectorChar()); return d_vectorChar; }
        const bsl::string& theString() const
                               { ASSERT(isString());     return d_string;     }

    };

    }  // close package namespace
    }  // close enterprise namespace
//..
// When acting as a vector this class is a 'bdlat' "array" type and when
// holding a string, the class is a 'bdlat' "simple" type.  Since this type can
// be in two type categories (determined at runtime) this class is deemed a
// "dynamic" class (for calculations at compile time).
//
// Then, to denote that this class is a dynamic type, we specialize the
// 'bdlat_TypeCategoryDeclareDynamic' meta-function in the 'BloombergLP'
// namespace:
//..
    namespace BloombergLP {

        template <>
        struct bdlat_TypeCategoryDeclareDynamic<mine::MyDynamicType>
        : public bsl::true_type {
        };

    }  // close enterprise namespace
//..
// Now, we define bdlat_typeCategorySelect', and a suite of four functions,
// 'bdlat_typeCategory(Manipulate|Access)(Array|Simple)', each overloaded for
// our type, 'MyDynamicType'.
//..
   namespace BloombergLP {
   namespace mine {

    bdlat_TypeCategory::Value
    bdlat_typeCategorySelect(const MyDynamicType& object)
    {
        if (object.isVectorChar()) {
            return bdlat_TypeCategory::e_ARRAY_CATEGORY;              // RETURN
        }
        else if (object.isString()) {
            return bdlat_TypeCategory::e_SIMPLE_CATEGORY;             // RETURN
        }

        ASSERT(!"Reached");

        // Note that this 'return' is never reached and hence the returned
        // value is immaterial.

        return bdlat_TypeCategory::e_SIMPLE_CATEGORY;
    }

    template <class MANIPULATOR>
    int bdlat_typeCategoryManipulateArray(MyDynamicType *object,
                                          MANIPULATOR&        manipulator)
    {
        if (object->isVectorChar()) {
            return manipulator(&object->theVectorChar(),
                               bdlat_TypeCategory::Array());          // RETURN
        }

        return manipulator(object, bslmf::Nil());
    }

    template <class MANIPULATOR>
    int bdlat_typeCategoryManipulateSimple(MyDynamicType *object,
                                           MANIPULATOR&        manipulator)
    {
        if (object->isString()) {
            return manipulator(&object->theString(),
                               bdlat_TypeCategory::Simple());         // RETURN
        }

        return manipulator(object, bslmf::Nil());
    }

    template <class ACCESSOR>
    int bdlat_typeCategoryAccessArray(const MyDynamicType& object,
                                      ACCESSOR&                 accessor)
    {
        if (object.isVectorChar()) {
            return accessor(object.theVectorChar(),
                            bdlat_TypeCategory::Array());             // RETURN
        }

        return accessor(object, bslmf::Nil());
    }

    template <class ACCESSOR>
    int bdlat_typeCategoryAccessSimple(const MyDynamicType& object,
                                       ACCESSOR&                 accessor)
    {
        if (object.isString()) {
            return accessor(object.theString(),
                            bdlat_TypeCategory::Simple());            // RETURN
        }

        return accessor(object, bslmf::Nil());
    }

    }  // close package namespace
    }  // close enterprise namespace
//..
// Notice that the customization points were implemented for just the two type
// categories that 'MyDynamicType' can achieve: "array" and "simple".
//
// Finally, we can see how the facilities we developed in {Example 1} and
// {Example 2} behave when given a "dynamic" type;
//..
    void runUsageExample3()
    {
//..
// We see that the 'Select' meta-function returns the expected value:
//..
        ASSERT(bdlat_TypeCategory::e_DYNAMIC_CATEGORY
            == static_cast<bdlat_TypeCategory::Value>(
               bdlat_TypeCategory::Select<mine::MyDynamicType>::e_SELECTION));
//..
// We create an object of our dynamic type and observe that the specialization
// we created for printing the values (actually, for *not* printing the value
// of) "dynamic" types is invoked:
//..
        bsl::ostringstream  oss;
        mine::MyDynamicType object;

        mine::printCategoryAndValue(oss, object);
        ASSERT("DynamicType: Printing dynamic types requires extra work."
               == oss.str());
        oss.str("");
//..
// We instruct object to behave as a vector and see that the 'bdlat' framework
// treats the object as a member of the "array" category and the
// 'PrintAccessor' we defined in {Example 2} treats 'object' as a member of the
// "array" category:
//..
        object.makeVectorChar();

        ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
            == bdlat_TypeCategoryFunctions::select(object));

        object.theVectorChar().push_back('H');
        object.theVectorChar().push_back('e');
        object.theVectorChar().push_back('l');
        object.theVectorChar().push_back('l');
        object.theVectorChar().push_back('o');

        mine::PrintAccessor accessor(&oss);
        int                 ret;

        ret = bdlat_TypeCategoryUtil::accessByCategory(object, accessor);
        ASSERT(0 == ret);
        LOOP_ASSERT(oss.str(), "Array: \"Hello\"" == oss.str());
        oss.str("");
//..
// Lastly, we instruct 'object' to behave as a string and find that the 'bdlat'
// framework now considers 'object' to be the "simple" category:
//..
        object.makeString();

        ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
            == bdlat_TypeCategoryFunctions::select(object));

        object.theString() = "World";

        ret = bdlat_TypeCategoryUtil::accessByCategory(object, accessor);
        ASSERT(0 == ret);
        ASSERT("Simple: World" == oss.str());
        oss.str("");
    }
//..
// Notice that the output of the accessor matches the state of the object,
// reporting an "array" type when the object 'isVector' and a "simple" type
// when the object 'isString'.

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLES
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING USAGE EXAMPLES"
                          << "\n======================" << endl;

        runUsageExample1();
        runUsageExample2();
        runUsageExample3();

        if (verbose) cout << "\nEnd of test." << endl;
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 1
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING USAGE EXAMPLE 1"
                          << "\n============= " << endl;

        runUsageExample1();

        if (verbose) cout << "\nEnd of test." << endl;
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TEST ENUMERATORS
        //
        // Concerns
        //: 1 When BDE_OMIT_INTERNAL_DEPRECATED is not defined, the BDEAT_...
        //:   enumeration literals should exist and evaluate to their e_...
        //:   equivalents.
        //:
        //: 2 When BDE_OMIT_INTERNAL_DEPRECATED is defined, the BDEAT_...
        //:   enumeration literals should not exist.
        //
        // Plan
        //: 1 When BDE_OMIT_INTERNAL_DEPRECATED is not defined, check that the
        //:   BDEAT_... enumeration literals evaluate to their e_...
        //:   equivalents.  (C-1)
        //:
        //: 2 We cannot check for (C-2), so hope for the best.
        //
        // Testing:
        //   ENUMERATORS
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTEST ENUMERATORS"
                          << "\n================" << endl;

#undef e
#define e(Class, Enumerator)                                                  \
    { L_, #Class "::..._" #Enumerator,                                        \
      Class::e_##Enumerator, Class::BDEAT_##Enumerator }

        static struct {
            int         d_line;         // line number
            const char *d_name;         // printable enumerator name
            int         d_bdeat_value;  // value of BDEAT_... version
            int         d_bdlat_value;  // value of e_... version
        } DATA [] = {
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
          e(bdlat_TypeCategory, DYNAMIC_CATEGORY),
          e(bdlat_TypeCategory, ARRAY_CATEGORY),
          e(bdlat_TypeCategory, CHOICE_CATEGORY),
          e(bdlat_TypeCategory, CUSTOMIZED_TYPE_CATEGORY),
          e(bdlat_TypeCategory, ENUMERATION_CATEGORY),
          e(bdlat_TypeCategory, NULLABLE_VALUE_CATEGORY),
          e(bdlat_TypeCategory, SEQUENCE_CATEGORY),
          e(bdlat_TypeCategory, SIMPLE_CATEGORY),
          e(bdlat_TypeCategory::Select<void*>, SELECTION),
#endif
          { L_, "None", 0, 0 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_line;
            const char *NAME   = DATA[i].d_name;
            const int   EVALUE = DATA[i].d_bdeat_value;
            const int   LVALUE = DATA[i].d_bdlat_value;

            if (veryVerbose) { P_(LINE) P_(NAME) P_(EVALUE) P(LVALUE) }

            ASSERTV(LINE, NAME, EVALUE, LVALUE, EVALUE == LVALUE);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'accessByCategory' FUNCTION
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'accessByCategory' FUNCTION"
                          << "\n============= " << endl;

        {
            typedef MyArrayType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MyChoiceType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_CHOICE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MyCustomizedType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MyEnumerationType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_ENUMERATION_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MyNullableValueType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MySequenceType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef int Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef char Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef float Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef bsl::string Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef bsl::vector<char> Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MyDeclaredDynamicType Type;

            Type          object;
            MyAccessor    accessor;

            object.makeChoice();
            int catchoice = bdlat_TypeCategoryUtil::
                                            accessByCategory(object, accessor);
            ASSERT(bdlat_TypeCategory::e_CHOICE_CATEGORY == catchoice);

            object.makeSequence();
            int catseq = bdlat_TypeCategoryUtil::
                                            accessByCategory(object, accessor);
            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY == catseq);
        }

        {
            typedef MyAutoDetectDynamicType Type;

            Type          object;
            MyAccessor    accessor;

            object.makeSequence();
            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));

            object.makeNullableValue();
            ASSERT(bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        if (verbose) cout << "\nEnd of test." << endl;
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'manipulateByCategory' FUNCTION
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'manipulateByCategory' FUNCTION"
                          << "\n============= " << endl;

        {
            typedef MyArrayType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MyChoiceType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_CHOICE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MyCustomizedType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MyEnumerationType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_ENUMERATION_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MyNullableValueType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MySequenceType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef int Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef char Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef float Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef bsl::string Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef bsl::vector<char> Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MyDeclaredDynamicType Type;

            Type          object;
            MyManipulator manip;

            object.makeChoice();
            int catchoice = bdlat_TypeCategoryUtil::
                                          manipulateByCategory(&object, manip);
            ASSERT(bdlat_TypeCategory::e_CHOICE_CATEGORY == catchoice);

            object.makeSequence();
            int catseq = bdlat_TypeCategoryUtil::
                                          manipulateByCategory(&object, manip);
            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY == catseq);
        }

        {
            typedef MyAutoDetectDynamicType Type;

            Type          object;
            MyManipulator manip;

            object.makeSequence();
            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));

            object.makeNullableValue();
            ASSERT(bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY
                    == bdlat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        if (verbose) cout << "\nEnd of test." << endl;
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'access<Category>' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'acess<Category>' FUNCTIONS"
                          << "\n============= " << endl;

        {
            typedef MyArrayType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef MyChoiceType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(bdlat_TypeCategory::e_CHOICE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef MyCustomizedType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MyEnumerationType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(bdlat_TypeCategory::e_ENUMERATION_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef MyNullableValueType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef MySequenceType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef int Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef char Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef float Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef bsl::string Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef bsl::vector<char> Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        if (verbose) cout << "\nEnd of test." << endl;
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'manipulate<Category>' FUNCTIONS
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'manipulate<Category>' FUNCTIONS"
                          << "\n============= " << endl;

        {
            typedef MyArrayType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MyChoiceType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(bdlat_TypeCategory::e_CHOICE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MyCustomizedType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MyEnumerationType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(bdlat_TypeCategory::e_ENUMERATION_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MyNullableValueType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MySequenceType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef int Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef char Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef float Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef bsl::string Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef bsl::vector<char> Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
                      == bdlat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdlat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        if (verbose) cout << "\nEnd of test." << endl;
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'select' FUNCTION
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'select' FUNCTION"
                          << "\n============= " << endl;

        {
            typedef MyArrayType Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyChoiceType Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_CHOICE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyCustomizedType Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyEnumerationType Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_ENUMERATION_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyNullableValueType Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MySequenceType Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef int Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef char Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef float Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef bsl::string Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_SIMPLE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef bsl::vector<char> Type;

            Type object;

            ASSERT(bdlat_TypeCategory::e_ARRAY_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyDeclaredDynamicType Type;

            Type object;

            object.makeChoice();
            ASSERT(bdlat_TypeCategory::e_CHOICE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));

            object.makeSequence();
            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyAutoDetectDynamicType Type;

            Type object;

            object.makeSequence();
            ASSERT(bdlat_TypeCategory::e_SEQUENCE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));

            object.makeNullableValue();
            ASSERT(bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY
                               == bdlat_TypeCategoryFunctions::select(object));
        }


        if (verbose) cout << "\nEnd of test." << endl;
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n============= " << endl;

        {
            typedef MyArrayType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef MyChoiceType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef MyCustomizedType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef MyEnumerationType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef MyNullableValueType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef MySequenceType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef MyDeclaredDynamicType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef MyAutoDetectDynamicType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef int Type;

            enum { SELECTION = bdlat_TypeCategory::e_SIMPLE_CATEGORY };
            typedef bdlat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef float Type;

            enum { SELECTION = bdlat_TypeCategory::e_SIMPLE_CATEGORY };
            typedef bdlat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef char Type;

            enum { SELECTION = bdlat_TypeCategory::e_SIMPLE_CATEGORY };
            typedef bdlat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef bsl::string Type;

            enum { SELECTION = bdlat_TypeCategory::e_SIMPLE_CATEGORY };
            typedef bdlat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

        {
            typedef bsl::vector<char> Type;

            enum { SELECTION = bdlat_TypeCategory::e_ARRAY_CATEGORY };
            typedef bdlat_TypeCategory::Array Category;

            ASSERT(((int)SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Category,
                              bdlat_TypeCategory::Select<Type>::Type>::value));
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

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
