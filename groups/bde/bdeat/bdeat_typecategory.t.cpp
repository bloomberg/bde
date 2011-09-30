// bdeat_typecategory.t.cpp                  -*-C++-*-

#include <bdeat_typecategory.h>

#include <bslmf_issame.h>

#include <bsl_string.h>
#include <bsl_vector.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::atoi;
using bsl::flush;
using bsl::endl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// TBD doc
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

namespace {

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

struct MyArrayType {
    enum { SELECTION = bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY };
    typedef bdeat_TypeCategory::Array Category;
};

namespace BloombergLP {
namespace bdeat_ArrayFunctions {
    template <>
    struct IsArray<MyArrayType> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_ArrayFunctions
}  // close namespace BloombergLP

struct MyChoiceType {
    enum { SELECTION = bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY };
    typedef bdeat_TypeCategory::Choice Category;
};

namespace BloombergLP {
namespace bdeat_ChoiceFunctions {
    template <>
    struct IsChoice<MyChoiceType> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_ChoiceFunctions
}  // close namespace BloombergLP

struct MyCustomizedType {
    enum { SELECTION = bdeat_TypeCategory::BDEAT_CUSTOMIZED_TYPE_CATEGORY };
    typedef bdeat_TypeCategory::CustomizedType Category;
};

namespace BloombergLP {
namespace bdeat_CustomizedTypeFunctions {
    template <>
    struct IsCustomizedType<MyCustomizedType> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_CustomizedTypeFunctions
}  // close namespace BloombergLP

struct MyEnumerationType {
    enum { SELECTION = bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY };
    typedef bdeat_TypeCategory::Enumeration Category;
};

namespace BloombergLP {
namespace bdeat_EnumFunctions {
    template <>
    struct IsEnumeration<MyEnumerationType> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_EnumFunctions
}  // close namespace BloombergLP

struct MyNullableValueType {
    enum { SELECTION = bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY };
    typedef bdeat_TypeCategory::NullableValue Category;
};

namespace BloombergLP {
namespace bdeat_NullableValueFunctions {
    template <>
    struct IsNullableValue<MyNullableValueType> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_NullableValueFunctions
}  // close namespace BloombergLP

struct MySequenceType {
    enum { SELECTION = bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY };
    typedef bdeat_TypeCategory::Sequence Category;
};

namespace BloombergLP {
namespace bdeat_SequenceFunctions {
    template <>
    struct IsSequence<MySequenceType> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_SequenceFunctions
}  // close namespace BloombergLP


struct MyDeclaredDynamicType {
    enum { SELECTION = 0 };
    typedef bdeat_TypeCategory::DynamicType Category;

    bdeat_TypeCategory::Value d_currentCategory;

    void makeChoice()
    {
        d_currentCategory = bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY;
    }

    void makeSequence()
    {
        d_currentCategory = bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY;
    }
};

namespace BloombergLP {
    template <>
    struct bdeat_TypeCategoryDeclareDynamic<MyDeclaredDynamicType> {
        enum { VALUE = 1 };
    };
}  // close namespace BloombergLP

bdeat_TypeCategory::Value
bdeat_typeCategorySelect(const MyDeclaredDynamicType& object)
{
    (void)object;
    return object.d_currentCategory;
}

template <typename MANIPULATOR>
int bdeat_typeCategoryManipulateChoice(MyDeclaredDynamicType *object,
                                       MANIPULATOR&           manipulator)
{
    (void)object;
    return manipulator(object, bdeat_TypeCategory::Choice());
}

template <typename MANIPULATOR>
int bdeat_typeCategoryManipulateSequence(MyDeclaredDynamicType *object,
                                         MANIPULATOR&           manipulator)
{
    (void)object;
    return manipulator(object, bdeat_TypeCategory::Sequence());
}

template <typename ACCESSOR>
int bdeat_typeCategoryAccessChoice(const MyDeclaredDynamicType& object,
                                   ACCESSOR&                    accessor)
{
    (void)object;
    return accessor(object, bdeat_TypeCategory::Choice());
}

template <typename ACCESSOR>
int bdeat_typeCategoryAccessSequence(const MyDeclaredDynamicType& object,
                                     ACCESSOR&                    accessor)
{
    (void)object;
    return accessor(object, bdeat_TypeCategory::Sequence());
}

struct MyAutoDetectDynamicType {
    enum { SELECTION = 0 };
    typedef bdeat_TypeCategory::DynamicType Category;

    bdeat_TypeCategory::Value d_currentCategory;

    void makeSequence()
    {
        d_currentCategory = bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY;
    }

    void makeNullableValue()
    {
        d_currentCategory = bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY;
    }
};

namespace BloombergLP {

namespace bdeat_SequenceFunctions {
    template <>
    struct IsSequence<MyAutoDetectDynamicType> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_SequenceFunctions

namespace bdeat_NullableValueFunctions {
    template <>
    struct IsNullableValue<MyAutoDetectDynamicType> {
        enum { VALUE = 1 };
    };
}  // close namespace bdeat_NullableValueFunctions
}  // close namespace BloombergLP

bdeat_TypeCategory::Value
bdeat_typeCategorySelect(const MyAutoDetectDynamicType& object)
{
    (void)object;
    return object.d_currentCategory;
}


struct MyManipulator {

    template <typename TYPE>
    int operator()(TYPE *object, bslmf_Nil)
    {
        (void)object;
        return -1;
    }

    template <typename TYPE>
    int operator()(TYPE *object, bdeat_TypeCategory::DynamicType)
    {
        (void)object;
        return 0;
    }

    template <typename TYPE>
    int operator()(TYPE *object, bdeat_TypeCategory::Array)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY;
    }

    template <typename TYPE>
    int operator()(TYPE *object, bdeat_TypeCategory::Choice)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY;
    }

    template <typename TYPE>
    int operator()(TYPE *object, bdeat_TypeCategory::CustomizedType)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_CUSTOMIZED_TYPE_CATEGORY;
    }

    template <typename TYPE>
    int operator()(TYPE *object, bdeat_TypeCategory::Enumeration)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY;
    }

    template <typename TYPE>
    int operator()(TYPE *object, bdeat_TypeCategory::NullableValue)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY;
    }

    template <typename TYPE>
    int operator()(TYPE *object, bdeat_TypeCategory::Sequence)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY;
    }

    template <typename TYPE>
    int operator()(TYPE *object, bdeat_TypeCategory::Simple)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    }
};

struct MyAccessor {

    template <typename TYPE>
    int operator()(const TYPE& object, bslmf_Nil)
    {
        (void)object;
        return -1;
    }

    template <typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::DynamicType)
    {
        (void)object;
        return 0;
    }

    template <typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::Array)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY;
    }

    template <typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::Choice)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY;
    }

    template <typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::CustomizedType)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_CUSTOMIZED_TYPE_CATEGORY;
    }

    template <typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::Enumeration)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY;
    }

    template <typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::NullableValue)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY;
    }

    template <typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::Sequence)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY;
    }

    template <typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::Simple)
    {
        (void)object;
        return bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    }
};

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
    template <typename TYPE>
    void printCategoryAndValue(bsl::ostream& stream, const TYPE& object);
        // Print the category of the specified 'object' followed by the value
        // of 'object' to the specified output 'stream'.
//..
// In order to implement this function, we will use a set of helper functions
// that are overloaded based on the category tag:
//..
    void printCategory(bsl::ostream& stream, bdeat_TypeCategory::Array)
    {
        stream << "Array";
    }

    void printCategory(bsl::ostream& stream, bdeat_TypeCategory::Choice)
    {
        stream << "Choice";
    }

    void printCategory(bsl::ostream& stream,
                       bdeat_TypeCategory::CustomizedType)
    {
        stream << "CustomizedType";
    }

    void printCategory(bsl::ostream& stream, bdeat_TypeCategory::DynamicType)
    {
        stream << "DynamicType";
    }

    void printCategory(bsl::ostream& stream, bdeat_TypeCategory::Enumeration)
    {
        stream << "Enumeration";
    }

    void printCategory(bsl::ostream& stream, bdeat_TypeCategory::NullableValue)
    {
        stream << "NullableValue";
    }

    void printCategory(bsl::ostream& stream, bdeat_TypeCategory::Sequence)
    {
        stream << "Sequence";
    }

    void printCategory(bsl::ostream& stream, bdeat_TypeCategory::Simple)
    {
        stream << "Simple";
    }

//..
// Now we can implement the 'printCategoryAndValue' function in terms of the
// 'printCategory' helper functions:
//..
    template <typename TYPE>
    void printCategoryAndValue(bsl::ostream& stream, const TYPE& object)
    {
        typedef typename
        bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;

        printCategory(stream, TypeCategory());

        stream << ": ";

        bdeu_PrintMethods::print(stream, object, 0, -1);
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

        bdeut_NullableValue<int> nullableInt;

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
// 'bdeat_TypeCategoryDeclareDynamic' meta-function in the 'BloombergLP'
// namespace:
//..
    namespace BloombergLP {

        template <>
        struct bdeat_TypeCategoryDeclareDynamic<VectorCharOrString> {
            enum { VALUE = 1 };
        };

    }  // close namespace BloombergLP

//..
// Next, we define bdeat_typeCategorySelect', and a suite of four function,
// 'bdeat_typeCategory(Manipulate|Access)(Array|Simple)', each overloaded for
// our type, 'VectorCharOrString'.
//..
    bdeat_TypeCategory::Value
    bdeat_typeCategorySelect(const VectorCharOrString& object)
    {
        if (object.isVectorChar()) {
            return bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY;          // RETURN
        }
        else if (object.isString()) {
            return bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;         // RETURN
        }

        ASSERT(0);
        return static_cast<bdeat_TypeCategory::Value>(-1);
    }

    template <typename MANIPULATOR>
    int bdeat_typeCategoryManipulateArray(VectorCharOrString *object,
                                          MANIPULATOR&        manipulator)
    {
        if (object->isVectorChar()) {
            return manipulator(&object->theVectorChar(),
                               bdeat_TypeCategory::Array());          // RETURN
        }

        return manipulator(object, bslmf_Nil());
    }

    template <typename MANIPULATOR>
    int bdeat_typeCategoryManipulateSimple(VectorCharOrString *object,
                                           MANIPULATOR&        manipulator)
    {
        if (object->isString()) {
            return manipulator(&object->theString(),
                               bdeat_TypeCategory::Simple());         // RETURN
        }

        return manipulator(object, bslmf_Nil());
    }

    template <typename ACCESSOR>
    int bdeat_typeCategoryAccessArray(const VectorCharOrString& object,
                                      ACCESSOR&                 accessor)
    {
        if (object.isVectorChar()) {
            return accessor(object.theVectorChar(),
                            bdeat_TypeCategory::Array());             // RETURN
        }

        return accessor(object, bslmf_Nil());
    }

    template <typename ACCESSOR>
    int bdeat_typeCategoryAccessSimple(const VectorCharOrString& object,
                                       ACCESSOR&                 accessor)
    {
        if (object.isString()) {
            return accessor(object.theString(),
                            bdeat_TypeCategory::Simple());            // RETURN
        }

        return accessor(object, bslmf_Nil());
    }

//..
// Now we will create an accessor that dumps the contents of the visited object
// into an associated stream:
//..
    struct DumpObject {
        bsl::ostream *d_stream_p;

        template <typename TYPE>
        int operator()(const TYPE& object, bslmf_Nil)
        {
            ASSERT(0);  // received invalid object
            (void)object;
            return -1;
        }

        template <typename TYPE>
        int operator()(const TYPE& object, bdeat_TypeCategory::Array)
        {
            (*d_stream_p) << "Array = ";
            bdeu_PrintMethods::print(*d_stream_p, object, 0, -1);
            return 0;
        }

        template <typename TYPE>
        int operator()(const TYPE& object, bdeat_TypeCategory::Simple)
        {
            (*d_stream_p) << "Simple = ";
            bdeu_PrintMethods::print(*d_stream_p, object, 0, -1);
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

        ret = bdeat_TypeCategoryUtil::accessByCategory(object, accessor);
        ASSERT("Array = \"Hello\"" == oss.str());
        oss.str("");

        object.makeString();
        object.theString() = "World";

        ret = bdeat_TypeCategoryUtil::accessByCategory(object, accessor);
        ASSERT("Simple = World" == oss.str());
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

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

            ASSERT(bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MyChoiceType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MyCustomizedType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_CUSTOMIZED_TYPE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MyEnumerationType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MyNullableValueType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MySequenceType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef int Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef char Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef float Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef bsl::string Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef bsl::vector<char> Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));
        }

        {
            typedef MyDeclaredDynamicType Type;

            Type          object;
            MyAccessor    accessor;

            object.makeChoice();
            int catchoice = bdeat_TypeCategoryUtil::
                                            accessByCategory(object, accessor);
            ASSERT(bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY == catchoice);

            object.makeSequence();
            int catseq = bdeat_TypeCategoryUtil::
                                            accessByCategory(object, accessor);
            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY == catseq);
        }

        {
            typedef MyAutoDetectDynamicType Type;

            Type          object;
            MyAccessor    accessor;

            object.makeSequence();
            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         accessByCategory(object, accessor));

            object.makeNullableValue();
            ASSERT(bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY
                    == bdeat_TypeCategoryUtil::
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

            ASSERT(bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MyChoiceType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MyCustomizedType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_CUSTOMIZED_TYPE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MyEnumerationType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MyNullableValueType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MySequenceType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef int Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef char Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef float Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef bsl::string Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef bsl::vector<char> Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));
        }

        {
            typedef MyDeclaredDynamicType Type;

            Type          object;
            MyManipulator manip;

            object.makeChoice();
            int catchoice = bdeat_TypeCategoryUtil::
                                          manipulateByCategory(&object, manip);
            ASSERT(bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY == catchoice);

            object.makeSequence();
            int catseq = bdeat_TypeCategoryUtil::
                                          manipulateByCategory(&object, manip);
            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY == catseq);
        }

        {
            typedef MyAutoDetectDynamicType Type;

            Type          object;
            MyManipulator manip;

            object.makeSequence();
            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY
                    == bdeat_TypeCategoryUtil::
                                         manipulateByCategory(&object, manip));

            object.makeNullableValue();
            ASSERT(bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY
                    == bdeat_TypeCategoryUtil::
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

            ASSERT(bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef MyChoiceType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef MyCustomizedType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(bdeat_TypeCategory::BDEAT_CUSTOMIZED_TYPE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MyEnumerationType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef MyNullableValueType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef MySequenceType Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef int Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef char Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef float Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef bsl::string Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   accessSimple(object, accessor));
        }

        {
            typedef bsl::vector<char> Type;

            Type          object;
            MyAccessor    accessor;

            ASSERT(bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   accessArray(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessChoice(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessCustomizedType(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessEnumeration(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessNullableValue(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   accessSequence(object, accessor));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
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

            ASSERT(bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MyChoiceType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MyCustomizedType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(bdeat_TypeCategory::BDEAT_CUSTOMIZED_TYPE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MyEnumerationType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MyNullableValueType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef MySequenceType Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef int Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef char Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef float Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef bsl::string Type;

            Type          object;
            MyManipulator manip;

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateSimple(&object, manip));
        }

        {
            typedef bsl::vector<char> Type;

            Type          object;
            MyManipulator manip;

            ASSERT(bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY
                      == bdeat_TypeCategoryFunctions::
                                   manipulateArray(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateChoice(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateCustomizedType(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateEnumeration(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateNullableValue(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
                                   manipulateSequence(&object, manip));

            ASSERT(-1 == bdeat_TypeCategoryFunctions::
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

            ASSERT(bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyChoiceType Type;

            Type object;

            ASSERT(bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyCustomizedType Type;

            Type object;

            ASSERT(bdeat_TypeCategory::BDEAT_CUSTOMIZED_TYPE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyEnumerationType Type;

            Type object;

            ASSERT(bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyNullableValueType Type;

            Type object;

            ASSERT(bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MySequenceType Type;

            Type object;

            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef int Type;

            Type object;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef char Type;

            Type object;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef float Type;

            Type object;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef bsl::string Type;

            Type object;

            ASSERT(bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef bsl::vector<char> Type;

            Type object;

            ASSERT(bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyDeclaredDynamicType Type;

            Type object;

            object.makeChoice();
            ASSERT(bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));

            object.makeSequence();
            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
        }

        {
            typedef MyAutoDetectDynamicType Type;

            Type object;

            object.makeSequence();
            ASSERT(bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));

            object.makeNullableValue();
            ASSERT(bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY
                               == bdeat_TypeCategoryFunctions::select(object));
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
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Type::Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyChoiceType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Type::Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyCustomizedType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Type::Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyEnumerationType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Type::Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyNullableValueType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Type::Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MySequenceType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Type::Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyDeclaredDynamicType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Type::Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef MyAutoDetectDynamicType Type;

            ASSERT(((int)Type::SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Type::Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef int Type;

            enum { SELECTION = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY };
            typedef bdeat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef float Type;

            enum { SELECTION = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY };
            typedef bdeat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef char Type;

            enum { SELECTION = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY };
            typedef bdeat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef bsl::string Type;

            enum { SELECTION = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY };
            typedef bdeat_TypeCategory::Simple Category;

            ASSERT(((int)SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
        }

        {
            typedef bsl::vector<char> Type;

            enum { SELECTION = bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY };
            typedef bdeat_TypeCategory::Array Category;

            ASSERT(((int)SELECTION
                   == (int)bdeat_TypeCategory::Select<Type>::BDEAT_SELECTION));
            ASSERT((bslmf_IsSame<
                              Category,
                              bdeat_TypeCategory::Select<Type>::Type>::VALUE));
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
