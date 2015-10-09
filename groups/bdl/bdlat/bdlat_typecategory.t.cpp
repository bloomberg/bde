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
    struct IsArray<MyArrayType> {
        enum { VALUE = 1 };
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
    struct IsChoice<MyChoiceType> {
        enum { VALUE = 1 };
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
    struct IsCustomizedType<MyCustomizedType> {
        enum { VALUE = 1 };
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
    struct IsEnumeration<MyEnumerationType> {
        enum { VALUE = 1 };
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
    struct IsNullableValue<MyNullableValueType> {
        enum { VALUE = 1 };
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
    struct IsSequence<MySequenceType> {
        enum { VALUE = 1 };
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
    struct bdlat_TypeCategoryDeclareDynamic<MyDeclaredDynamicType> {
        enum { VALUE = 1 };
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
    struct IsSequence<MyAutoDetectDynamicType> {
        enum { VALUE = 1 };
    };
}  // close namespace bdlat_SequenceFunctions

namespace bdlat_NullableValueFunctions {
    template <>
    struct IsNullableValue<MyAutoDetectDynamicType> {
        enum { VALUE = 1 };
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
///Example 1: Function Compile-Time Parameterized by 'TYPE'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the usage of this component.  We
// will create a 'printCategoryAndValue' function that is parameterized by
// 'TYPE':
//..
    template <class TYPE>
    void printCategoryAndValue(bsl::ostream& stream, const TYPE& object);
        // Print the category of the specified 'object' followed by the value
        // of 'object' to the specified output 'stream'.
//..
// In order to implement this function, we will use a set of helper functions
// that are overloaded based on the category tag:
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
// Now we can implement the 'printCategoryAndValue' function in terms of the
// 'printCategory' helper functions:
//..
    template <class TYPE>
    void printCategoryAndValue(bsl::ostream& stream, const TYPE& object)
    {
        typedef typename
        bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;

        printCategory(stream, TypeCategory());

        stream << ": ";

        bdlb::PrintMethods::print(stream, object, 0, -1);
    }
//..
// The following function demonstrates the output from this function:
//..
    void runUsageExample1()
    {
        bsl::ostringstream oss;


        int intVal = 123;

        printCategoryAndValue(oss, intVal);
        ASSERT("Simple: 123" == oss.str());
        oss.str("");

        bdlb::NullableValue<int> nullableInt;

        printCategoryAndValue(oss, nullableInt);
        ASSERT("NullableValue: NULL" == oss.str());
        oss.str("");

        nullableInt = 321;

        printCategoryAndValue(oss, nullableInt);
        ASSERT("NullableValue: 321" == oss.str());
        oss.str("");

        bsl::vector<int> vec;

        vec.push_back(123);
        vec.push_back(345);
        vec.push_back(987);

        printCategoryAndValue(oss, vec);
        ASSERT("Array: [ 123 345 987 ]" == oss.str());
    }
//..
//
///Example 2: Dynamic (Run-Time) Typing
/// - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the usage of dynamic types.
// Suppose we have a type that can, at runtime, be either a 'bsl::vector<char>'
// or a 'bsl::string':
//..
    class VectorCharOrString {

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
                               { ASSERT(isString()); return d_string; }

        // ACCESSORS
        bool isVectorChar() const { return 0 == d_selector; }
        bool isString() const     { return 1 == d_selector; }

        const bsl::vector<char>& theVectorChar() const
                               { ASSERT(isVectorChar()); return d_vectorChar; }
        const bsl::string& theString() const
                               { ASSERT(isString()); return d_string; }

    };
//..
// To make this type dynamic, we will specialize the
// 'bdlat_TypeCategoryDeclareDynamic' meta-function in the 'BloombergLP'
// namespace:
//..
    namespace BloombergLP {

        template <>
        struct bdlat_TypeCategoryDeclareDynamic<VectorCharOrString> {
            enum { VALUE = 1 };
        };

    }  // close enterprise namespace

//..
// Next, we define bdlat_typeCategorySelect', and a suite of four function,
// 'bdlat_typeCategory(Manipulate|Access)(Array|Simple)', each overloaded for
// our type, 'VectorCharOrString'.
//..
    bdlat_TypeCategory::Value
    bdlat_typeCategorySelect(const VectorCharOrString& object)
    {
        if (object.isVectorChar()) {
            return bdlat_TypeCategory::e_ARRAY_CATEGORY;          // RETURN
        }
        else if (object.isString()) {
            return bdlat_TypeCategory::e_SIMPLE_CATEGORY;         // RETURN
        }

        ASSERT(0);
        return static_cast<bdlat_TypeCategory::Value>(-1);
    }

    template <class MANIPULATOR>
    int bdlat_typeCategoryManipulateArray(VectorCharOrString *object,
                                          MANIPULATOR&        manipulator)
    {
        if (object->isVectorChar()) {
            return manipulator(&object->theVectorChar(),
                               bdlat_TypeCategory::Array());          // RETURN
        }

        return manipulator(object, bslmf::Nil());
    }

    template <class MANIPULATOR>
    int bdlat_typeCategoryManipulateSimple(VectorCharOrString *object,
                                           MANIPULATOR&        manipulator)
    {
        if (object->isString()) {
            return manipulator(&object->theString(),
                               bdlat_TypeCategory::Simple());         // RETURN
        }

        return manipulator(object, bslmf::Nil());
    }

    template <class ACCESSOR>
    int bdlat_typeCategoryAccessArray(const VectorCharOrString& object,
                                      ACCESSOR&                 accessor)
    {
        if (object.isVectorChar()) {
            return accessor(object.theVectorChar(),
                            bdlat_TypeCategory::Array());             // RETURN
        }

        return accessor(object, bslmf::Nil());
    }

    template <class ACCESSOR>
    int bdlat_typeCategoryAccessSimple(const VectorCharOrString& object,
                                       ACCESSOR&                 accessor)
    {
        if (object.isString()) {
            return accessor(object.theString(),
                            bdlat_TypeCategory::Simple());            // RETURN
        }

        return accessor(object, bslmf::Nil());
    }

//..
// Now we will create an accessor that dumps the contents of the visited object
// into an associated stream:
//..
    struct DumpObject {
        bsl::ostream *d_stream_p;

        template <class TYPE>
        int operator()(const TYPE& object, bslmf::Nil)
        {
            ASSERT(0);  // received invalid object
            (void)object;
            return -1;
        }

        template <class TYPE>
        int operator()(const TYPE& object, bdlat_TypeCategory::Array)
        {
            (*d_stream_p) << "Array = ";
            bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
            return 0;
        }

        template <class TYPE>
        int operator()(const TYPE& object, bdlat_TypeCategory::Simple)
        {
            (*d_stream_p) << "Simple = ";
            bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
            return 0;
        }
    };

//..
// Now we will use the 'accessByCategory' utility function to invoke the
// accessor and pick the correct method to invoke based on the runtime state of
// the 'VectorCharOrString' object:
//..
    void runUsageExample2()
    {
        bsl::ostringstream oss;
        DumpObject         accessor = { &oss };

        VectorCharOrString object;
        int                ret;

        object.makeVectorChar();
        object.theVectorChar().push_back('H');
        object.theVectorChar().push_back('e');
        object.theVectorChar().push_back('l');
        object.theVectorChar().push_back('l');
        object.theVectorChar().push_back('o');

        ret = bdlat_TypeCategoryUtil::accessByCategory(object, accessor);
        ASSERT("Array = \"Hello\"" == oss.str());
        oss.str("");

        object.makeString();
        object.theString() = "World";

        ret = bdlat_TypeCategoryUtil::accessByCategory(object, accessor);
        ASSERT("Simple = World" == oss.str());
    }
//..

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
//  int veryVerbose = argc > 3;
//  int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE 2
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING USAGE EXAMPLE 2"
                          << "\n============= " << endl;

        runUsageExample2();

        if (verbose) cout << "\nEnd of test." << endl;
      } break;
      case 7: {
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
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyChoiceType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyCustomizedType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyEnumerationType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyNullableValueType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MySequenceType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyDeclaredDynamicType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyAutoDetectDynamicType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Type::Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef int Type;

            enum { SELECTION = bdlat_TypeCategory::e_SIMPLE_CATEGORY };
            typedef bdlat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef float Type;

            enum { SELECTION = bdlat_TypeCategory::e_SIMPLE_CATEGORY };
            typedef bdlat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef char Type;

            enum { SELECTION = bdlat_TypeCategory::e_SIMPLE_CATEGORY };
            typedef bdlat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef bsl::string Type;

            enum { SELECTION = bdlat_TypeCategory::e_SIMPLE_CATEGORY };
            typedef bdlat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef bsl::vector<char> Type;

            enum { SELECTION = bdlat_TypeCategory::e_ARRAY_CATEGORY };
            typedef bdlat_TypeCategory::Array Category;

            ASSERT(((int)SELECTION
                   == (int)bdlat_TypeCategory::Select<Type>::e_SELECTION));
            ASSERT((bslmf::IsSame<
                              Category,
                              bdlat_TypeCategory::Select<Type>::Type>::VALUE));
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
