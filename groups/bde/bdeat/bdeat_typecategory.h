// bdeat_typecategory.h                  -*-C++-*-
#ifndef INCLUDED_BDEAT_TYPECATEGORY
#define INCLUDED_BDEAT_TYPECATEGORY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide type category tags and a tag selection meta-function.
//
//@CLASSES:
//  bdeat_TypeCategory::Array          : tag for array types
//  bdeat_TypeCategory::Choice         : tag for choice types
//  bdeat_TypeCategory::CustomizedType : tag for customized types
//  bdeat_TypeCategory::Enumeration    : tag for enumeration types
//  bdeat_TypeCategory::NullableValue  : tag for nullable types
//  bdeat_TypeCategory::Sequence       : tag for sequence types
//  bdeat_TypeCategory::Simple         : tag for simple types
//  bdeat_TypeCategory::Select         : meta-function for selecting tag
//  bdeat_TypeCategoryDeclareDynamic   : meta-function to declare dynamic type
//  bdeat_TypeCategoryFunctions        : overloadable type category functions
//  bdeat_TypeCategoryUtil             : type category utility
//
//@SEE_ALSO: bdeat_arrayfunctions, bdeat_choicefunctions,
//           bdeat_customizedtypefunctions, bdeat_enumfunctions,
//           bdeat_nullablevaluefunctions, bdeat_sequencefunctions
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
// TBD: update this documentation to reflect the new overloaded functions
//
//@DESCRIPTION: The tag selection meta-function ('bdeat_TypeCategory::Select')
// provided in this component can be used to categorize a parameterized 'TYPE'
// into one of the following categories:
//..
//  Category Tag     Assigned To
//  ------------     -----------
//
//  Array            types that expose "array" behavior through the
//                   'bdeat_ArrayFunctions' 'namespace'.
//
//  Choice           types that expose "choice" behavior through the
//                   'bdeat_ChoiceFunctions' 'namespace'.
//
//  CustomizedType   types that expose "customized type" behavior through the
//                   'bdeat_CustomizedTypeFunctions' 'namespace'.
//
//  DynamicType      types that can select a category at runtime
//
//  Enumeration      types that expose "enumeration" behavior through the
//                   'bdeat_EnumFunctions' 'namespace'.
//
//  NullableValue    types that expose "nullable" behavior through the
//                   'bdeat_NullableValueFunctions' 'namespace'.
//
//  Sequence         types that expose "sequence" behavior through the
//                   'bdeat_SequenceFunctions' 'namespace'.
//
//  Simple           all other types.
//..
// A type may only belong to one of these categories at compile time.
//
///Dynamic Types
///-------------
// There are two ways that a type can be categorized as a dynamic type:
//..
//  o by specializing the 'bdeat_TypeCategoryDeclareDynamic' meta-function
//    and providing a 'VALUE' of 1
//  o by implementing more than one of the 'bdeat' category functions (i.e.,
//    more than one of 'bdeat_ArrayFunctions', 'bdeat_ChoiceFunctions',
//    'bdeat_CustomizedTypeFunctions', 'bdeat_EnumFunctions',
//    'bdeat_NullableValueFunctions', or 'bdeat_SequenceFunctions').
//..
// Note that AIX and GCC compilers require that the first way be used.
//
// When a type is categorized as a dynamic type, it *must* overload the
// 'bdeat_TypeCategoryFunctions::select' function to provide the necessary
// runtime logic that determines its runtime category.
//
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
//  template <typename TYPE>
//  void printCategoryAndValue(bsl::ostream& stream, const TYPE& object);
//      // Print the category of the specified 'object' followed by the value
//      // of 'object' to the specified output 'stream'.
//..
// In order to implement this function, we will use a set of helper functions
// that are overloaded based on the category tag:
//..
//  void printCategory(bsl::ostream& stream, bdeat_TypeCategory::Array)
//  {
//      stream << "Array";
//  }
//
//  void printCategory(bsl::ostream& stream, bdeat_TypeCategory::Choice)
//  {
//      stream << "Choice";
//  }
//
//  void printCategory(bsl::ostream& stream,
//                     bdeat_TypeCategory::CustomizedType)
//  {
//      stream << "CustomizedType";
//  }
//
//  void printCategory(bsl::ostream& stream, bdeat_TypeCategory::DynamicType)
//  {
//      stream << "DynamicType";
//  }
//
//  void printCategory(bsl::ostream& stream, bdeat_TypeCategory::Enumeration)
//  {
//      stream << "Enumeration";
//  }
//
//  void printCategory(bsl::ostream& stream, bdeat_TypeCategory::NullableValue)
//  {
//      stream << "NullableValue";
//  }
//
//  void printCategory(bsl::ostream& stream, bdeat_TypeCategory::Sequence)
//  {
//      stream << "Sequence";
//  }
//
//  void printCategory(bsl::ostream& stream, bdeat_TypeCategory::Simple)
//  {
//      stream << "Simple";
//  }
//
//..
// Now we can implement the 'printCategoryAndValue' function in terms of the
// 'printCategory' helper functions:
//..
//  template <typename TYPE>
//  void printCategoryAndValue(bsl::ostream& stream, const TYPE& object)
//  {
//      typedef typename
//      bdeat_TypeCategory::Select<TYPE>::Type TypeCategory;
//
//      printCategory(stream, TypeCategory());
//
//      stream << ": ";
//
//      bdeu_PrintMethods::print(stream, object, 0, -1);
//  }
//..
// The following function demonstrates the output from this function:
//..
//  void runUsageExample1()
//  {
//      bsl::ostringstream oss;
//
//
//      int intVal = 123;
//
//      printCategoryAndValue(oss, intVal);
//      assert("Simple: 123" == oss.str());
//      oss.str("");
//
//      bdeut_NullableValue<int> nullableInt;
//
//      printCategoryAndValue(oss, nullableInt);
//      assert("NullableValue: NULL" == oss.str());
//      oss.str("");
//
//      nullableInt = 321;
//
//      printCategoryAndValue(oss, nullableInt);
//      assert("NullableValue: 321" == oss.str());
//      oss.str("");
//
//      bsl::vector<int> vec;
//
//      vec.push_back(123);
//      vec.push_back(345);
//      vec.push_back(987);
//
//      printCategoryAndValue(oss, vec);
//      assert("Array: [ 123 345 987 ]" == oss.str());
//  }
//..
//
///Example 2: Dynamic (Run-Time) Typing
/// - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate the usage of dynamic types.
// Suppose we have a type that can, at runtime, be either a 'bsl::vector<char>'
// or a 'bsl::string':
//..
//  class VectorCharOrString {
//
//      // PRIVATE DATA MEMBERS
//      bsl::vector<char> d_vectorChar;  // Note: Production code should use a
//      bsl::string       d_string;      //       union of object buffers.
//      int               d_selector;    // 0 = vectorChar, 1 = string
//
//    public:
//      // MANIPULATORS
//      void makeVectorChar() { d_selector = 0; }
//      void makeString()     { d_selector = 1; }
//
//      bsl::vector<char>& theVectorChar()
//                             { assert(isVectorChar()); return d_vectorChar; }
//      bsl::string& theString()
//                             { assert(isString()); return d_string; }
//
//      // ACCESSORS
//      bool isVectorChar() const { return 0 == d_selector; }
//      bool isString() const     { return 1 == d_selector; }
//
//      const bsl::vector<char>& theVectorChar() const
//                             { assert(isVectorChar()); return d_vectorChar; }
//      const bsl::string& theString() const
//                             { assert(isString()); return d_string; }
//
//  };
//..
// To make this type dynamic, we will specialize the
// 'bdeat_TypeCategoryDeclareDynamic' meta-function in the 'BloombergLP'
// namespace:
//..
//  namespace BloombergLP {
//
//      template <>
//      struct bdeat_TypeCategoryDeclareDynamic<VectorCharOrString> {
//          enum { VALUE = 1 };
//      };
//
//  }  // close namespace BloombergLP
//
//..
// Next, we define bdeat_typeCategorySelect', and a suite of four function,
// 'bdeat_typeCategory(Manipulate|Access)(Array|Simple)', each overloaded for
// our type, 'VectorCharOrString'.
//..
//  bdeat_TypeCategory::Value
//  bdeat_typeCategorySelect(const VectorCharOrString& object)
//  {
//      if (object.isVectorChar()) {
//          return bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY;          // RETURN
//      }
//      else if (object.isString()) {
//          return bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;         // RETURN
//      }
//
//      assert(0);
//      return static_cast<bdeat_TypeCategory::Value>(-1);
//  }
//
//  template <typename MANIPULATOR>
//  int bdeat_typeCategoryManipulateArray(VectorCharOrString *object,
//                                        MANIPULATOR&        manipulator)
//  {
//      if (object->isVectorChar()) {
//          return manipulator(&object->theVectorChar(),
//                             bdeat_TypeCategory::Array());          // RETURN
//      }
//
//      return manipulator(object, bslmf_Nil());
//  }
//
//  template <typename MANIPULATOR>
//  int bdeat_typeCategoryManipulateSimple(VectorCharOrString *object,
//                                         MANIPULATOR&        manipulator)
//  {
//      if (object->isString()) {
//          return manipulator(&object->theString(),
//                             bdeat_TypeCategory::Simple());         // RETURN
//      }
//
//      return manipulator(object, bslmf_Nil());
//  }
//
//  template <typename ACCESSOR>
//  int bdeat_typeCategoryAccessArray(const VectorCharOrString& object,
//                                    ACCESSOR&                 accessor)
//  {
//      if (object.isVectorChar()) {
//          return accessor(object.theVectorChar(),
//                          bdeat_TypeCategory::Array());             // RETURN
//      }
//
//      return accessor(object, bslmf_Nil());
//  }
//
//  template <typename ACCESSOR>
//  int bdeat_typeCategoryAccessSimple(const VectorCharOrString& object,
//                                     ACCESSOR&                 accessor)
//  {
//      if (object.isString()) {
//          return accessor(object.theString(),
//                          bdeat_TypeCategory::Simple());            // RETURN
//      }
//
//      return accessor(object, bslmf_Nil());
//  }
//
//..
// Now we will create an accessor that dumps the contents of the visited object
// into an associated stream:
//..
//  struct DumpObject {
//      bsl::ostream *d_stream_p;
//
//      template <typename TYPE>
//      int operator()(const TYPE& object, bslmf_Nil)
//      {
//          assert(0);  // received invalid object
//          return -1;
//      }
//
//      template <typename TYPE>
//      int operator()(const TYPE& object, bdeat_TypeCategory::Array)
//      {
//          (*d_stream_p) << "Array = ";
//          bdeu_PrintMethods::print(*d_stream_p, object, 0, -1);
//          return 0;
//      }
//
//      template <typename TYPE>
//      int operator()(const TYPE& object, bdeat_TypeCategory::Simple)
//      {
//          (*d_stream_p) << "Simple = ";
//          bdeu_PrintMethods::print(*d_stream_p, object, 0, -1);
//          return 0;
//      }
//  };
//
//..
// Now we will use the 'accessByCategory' utility function to invoke the
// accessor and pick the correct method to invoke based on the runtime state of
// the 'VectorCharOrString' object:
//..
//  void runUsageExample2()
//  {
//      bsl::ostringstream oss;
//      DumpObject         accessor = { &oss };
//
//      VectorCharOrString object;
//      int                ret;
//
//      object.makeVectorChar();
//      object.theVectorChar().push_back('H');
//      object.theVectorChar().push_back('e');
//      object.theVectorChar().push_back('l');
//      object.theVectorChar().push_back('l');
//      object.theVectorChar().push_back('o');
//
//      ret = bdeat_TypeCategoryUtil::accessByCategory(object, accessor);
//      assert("Array = \"Hello\"" == oss.str());
//      oss.str("");
//
//      object.makeString();
//      object.theString() = "World";
//
//      ret = bdeat_TypeCategoryUtil::accessByCategory(object, accessor);
//      assert("Simple = World" == oss.str());
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CHOICEFUNCTIONS
#include <bdeat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_ENUMFUNCTIONS
#include <bdeat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDEAT_NULLABLEVALUEFUNCTIONS
#include <bdeat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdeat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDEAT_SEQUENCEFUNCTIONS
#include <bdeat_sequencefunctions.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_SWITCH
#include <bslmf_switch.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif


namespace BloombergLP {


                         // =========================
                         // struct bdeat_TypeCategory
                         // =========================


template <typename TYPE>
struct bdeat_TypeCategoryDeclareDynamic {
    // Types that have more than one of the following traits:
    //..
    //  o bdeat_ArrayFunctions::IsArray<TYPE>::VALUE
    //  o bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE
    //  o bdeat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::VALUE
    //  o bdeat_EnumFunctions::IsEnumeration<TYPE>::VALUE
    //  o bdeat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE
    //  o bdeat_SequenceFunctions::IsSequence<TYPE>::VALUE
    //..
    // are automatically recognized as dynamic types.  However, there may exist
    // types that have *none* or only one of the above traits and still be
    // considered dynamic (e.g., "variants" of simple types etc).  In order for
    // the 'bdeat' framework to treat these types as dynamic types, this
    // 'struct' must be specialized with a 'VALUE' of 1, and the
    // 'bdeat_TypeCategoryFunctions::select' function must be implemented,
    // along with the appropriate 'bdeat_TypeCategoryFunctions::manipulate*'
    // and 'bdeat_TypeCategoryFunctions::access*' functions.  Inside these
    // 'manipulate*' and 'access*' functions, the dynamic object should expose
    // its real type.

    enum { VALUE = 0 };
};

struct bdeat_TypeCategory {
    // This struct provides the different category tags and also a
    // meta-function for selecting the tags based on the parameterized 'TYPE'.

    // TAGS
    struct DynamicType    { };  // tag for dynamic types
    struct Array          { };  // tag for arrays
    struct Choice         { };  // tag for choices
    struct CustomizedType { };  // tag for customized types
    struct Enumeration    { };  // tag for enumerations
    struct NullableValue  { };  // tag for nullable values
    struct Sequence       { };  // tag for sequences
    struct Simple         { };  // tag for all other types

    // ENUMERATIONS
    enum Value {
        BDEAT_DYNAMIC_CATEGORY         = 0,
        BDEAT_ARRAY_CATEGORY           = 1,
        BDEAT_CHOICE_CATEGORY          = 2,
        BDEAT_CUSTOMIZED_TYPE_CATEGORY = 3,
        BDEAT_ENUMERATION_CATEGORY     = 4,
        BDEAT_NULLABLE_VALUE_CATEGORY  = 5,
        BDEAT_SEQUENCE_CATEGORY        = 6,
        BDEAT_SIMPLE_CATEGORY          = 7

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , DYNAMIC_CATEGORY         = BDEAT_DYNAMIC_CATEGORY
      , ARRAY_CATEGORY           = BDEAT_ARRAY_CATEGORY
      , CHOICE_CATEGORY          = BDEAT_CHOICE_CATEGORY
      , CUSTOMIZED_TYPE_CATEGORY = BDEAT_CUSTOMIZED_TYPE_CATEGORY
      , ENUMERATION_CATEGORY     = BDEAT_ENUMERATION_CATEGORY
      , NULLABLE_VALUE_CATEGORY  = BDEAT_NULLABLE_VALUE_CATEGORY
      , SEQUENCE_CATEGORY        = BDEAT_SEQUENCE_CATEGORY
      , SIMPLE_CATEGORY          = BDEAT_SIMPLE_CATEGORY
#endif
    };

    // META-FUNCTION
    template <typename TYPE>
    struct Select {
        // This meta-function contains a typedef 'Type' that specifies the
        // category tag for the parameterized 'TYPE'.

      private:
        enum {
            BDEAT_IS_ARRAY = bdeat_ArrayFunctions::IsArray<TYPE>::VALUE,
            BDEAT_IS_CHOICE = bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE,
            BDEAT_IS_CUSTOMIZED_TYPE
                = bdeat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::VALUE,
            BDEAT_IS_ENUMERATION
                             = bdeat_EnumFunctions::IsEnumeration<TYPE>::VALUE,
            BDEAT_IS_NULLABLE_VALUE
                  = bdeat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE,
            BDEAT_IS_SEQUENCE
                            = bdeat_SequenceFunctions::IsSequence<TYPE>::VALUE,
            BDEAT_NUM_CATEGORIES = BDEAT_IS_ARRAY
                                 + BDEAT_IS_CHOICE
                                 + BDEAT_IS_CUSTOMIZED_TYPE
                                 + BDEAT_IS_ENUMERATION
                                 + BDEAT_IS_NULLABLE_VALUE
                                 + BDEAT_IS_SEQUENCE,
            BDEAT_IS_DYNAMIC = BDEAT_NUM_CATEGORIES > 1
                               || bdeat_TypeCategoryDeclareDynamic<TYPE>::VALUE
        };

      public:
        enum {
            BDEAT_SELECTION =
                      BDEAT_IS_DYNAMIC         ? BDEAT_DYNAMIC_CATEGORY
                    : BDEAT_IS_ARRAY           ? BDEAT_ARRAY_CATEGORY
                    : BDEAT_IS_CHOICE          ? BDEAT_CHOICE_CATEGORY
                    : BDEAT_IS_CUSTOMIZED_TYPE ? BDEAT_CUSTOMIZED_TYPE_CATEGORY
                    : BDEAT_IS_ENUMERATION     ? BDEAT_ENUMERATION_CATEGORY
                    : BDEAT_IS_NULLABLE_VALUE  ? BDEAT_NULLABLE_VALUE_CATEGORY
                    : BDEAT_IS_SEQUENCE        ? BDEAT_SEQUENCE_CATEGORY
                    : BDEAT_SIMPLE_CATEGORY

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
          , SELECTION = BDEAT_SELECTION
#endif

        };

        typedef typename
        bslmf_Switch<BDEAT_SELECTION, DynamicType,
                                      Array,
                                      Choice,
                                      CustomizedType,
                                      Enumeration,
                                      NullableValue,
                                      Sequence,
                                      Simple>::Type Type;
    };
};

                       // =============================
                       // struct bdeat_TypeCategoryUtil
                       // =============================

struct bdeat_TypeCategoryUtil {
    // This 'struct' contains utility functions pertaining to type category.

    template <typename TYPE, typename MANIPULATOR>
    static int manipulateByCategory(TYPE         *object,
                                    MANIPULATOR&  manipulator);
        // Invoke one of the following functions, based on the result of
        // 'bdeat_TypeCategoryFunctions::select':
        //..
        //  o bdeat_TypeCategoryFunctions::manipulateArray
        //  o bdeat_TypeCategoryFunctions::manipulateChoice
        //  o bdeat_TypeCategoryFunctions::manipulateCustomizedType
        //  o bdeat_TypeCategoryFunctions::manipulateEnumeration
        //  o bdeat_TypeCategoryFunctions::manipulateNullableValue
        //  o bdeat_TypeCategoryFunctions::manipulateSequence
        //  o bdeat_TypeCategoryFunctions::manipulateSimple
        //..
        // Return the value that was returned by the invoked function.  The
        // behavior is undefined unless the parameterized 'TYPE' supports the
        // type category that was returned by the call to the
        // 'bdeat_TypeCategoryFunctions::select' function.

    template <typename TYPE, typename ACCESSOR>
    static int accessByCategory(const TYPE& object,
                                ACCESSOR&   accessor);
        // Invoke one of the following functions, based on the result of
        // 'bdeat_TypeCategoryFunctions::select':
        //..
        //  o bdeat_TypeCategoryFunctions::accessArray
        //  o bdeat_TypeCategoryFunctions::accessChoice
        //  o bdeat_TypeCategoryFunctions::accessCustomizedType
        //  o bdeat_TypeCategoryFunctions::accessEnumeration
        //  o bdeat_TypeCategoryFunctions::accessNullableValue
        //  o bdeat_TypeCategoryFunctions::accessSequence
        //  o bdeat_TypeCategoryFunctions::accessSimple
        //..
        // Return the value that was returned by the invoked function.  The
        // behavior is undefined unless the parameterized 'TYPE' supports the
        // type category that was returned by the call to the
        // 'bdeat_TypeCategoryFunctions::select' function.
};

                   // =====================================
                   // namespace bdeat_TypeCategoryFunctions
                   // =====================================

namespace bdeat_TypeCategoryFunctions {
    // This namespace contains overloadable functions pertaining to type
    // categories.

    // MANIPULATORS
    template <typename TYPE, typename MANIPULATOR>
    int manipulateArray(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::Array' tag object as the second argument if the
        // first argument can be used with 'bdeat_arrayfunctions', or a
        // 'bslmf_Nil' tag object otherwise.  Return the result from the
        // invocation of 'manipulator'.

    template <typename TYPE, typename MANIPULATOR>
    int manipulateChoice(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::Choice' tag object as the second argument if
        // the first argument can be used with 'bdeat_choicefunctions', or a
        // 'bslmf_Nil' tag object otherwise.  Return the result from the
        // invocation of 'manipulator'.

    template <typename TYPE, typename MANIPULATOR>
    int manipulateCustomizedType(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::CustomizedType' tag object as the second
        // argument if the first argument can be used with
        // 'bdeat_customizedtypefunctions', or a 'bslmf_Nil' tag object
        // otherwise.  Return the result from the invocation of 'manipulator'.

    template <typename TYPE, typename MANIPULATOR>
    int manipulateEnumeration(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::Enumeration' tag object as the second argument
        // if the first argument can be used with 'bdeat_enumfunctions', or a
        // 'bslmf_Nil' tag object otherwise.  Return the result from the
        // invocation of 'manipulator'.

    template <typename TYPE, typename MANIPULATOR>
    int manipulateNullableValue(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::NullableValue' tag object as the second
        // argument if the first argument can be used with
        // 'bdeat_nullablevaluefunctions', or a 'bslmf_Nil' tag object
        // otherwise.  Return the result from the invocation of 'manipulator'.

    template <typename TYPE, typename MANIPULATOR>
    int manipulateSequence(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::Sequence' tag object as the second argument if
        // the first argument can be used with 'bdeat_sequencefunctions', or a
        // 'bslmf_Nil' tag object otherwise.  Return the result from the
        // invocation of 'manipulator'.

    template <typename TYPE, typename MANIPULATOR>
    int manipulateSimple(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::Simple' tag object as the second argument if
        // the first argument can be used as a simple type, or a 'bslmf_Nil'
        // tag object otherwise.  Return the result from the invocation of
        // 'manipulator'.

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int accessArray(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::Array' tag object as the second argument if the
        // first argument can be used with 'bdeat_arrayfunctions', or a
        // 'bslmf_Nil' tag object otherwise.  Return the result from the
        // invocation of 'accessor'.

    template <typename TYPE, typename ACCESSOR>
    int accessChoice(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::Choice' tag object as the second argument if
        // the first argument can be used with 'bdeat_choicefunctions', or a
        // 'bslmf_Nil' tag object otherwise.  Return the result from the
        // invocation of 'accessor'.

    template <typename TYPE, typename ACCESSOR>
    int accessCustomizedType(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::CustomizedType' tag object as the second
        // argument if the first argument can be used with
        // 'bdeat_customizedtypefunctions', or a 'bslmf_Nil' tag object
        // otherwise.  Return the result from the invocation of 'accessor'.

    template <typename TYPE, typename ACCESSOR>
    int accessEnumeration(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::Enumeration' tag object as the second argument
        // if the first argument can be used with 'bdeat_enumfunctions', or a
        // 'bslmf_Nil' tag object otherwise.  Return the result from the
        // invocation of 'accessor'.

    template <typename TYPE, typename ACCESSOR>
    int accessNullableValue(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::NullableValue' tag object as the second
        // argument if the first argument can be used with
        // 'bdeat_nullablevaluefunctions', or a 'bslmf_Nil' tag object
        // otherwise.  Return the result from the invocation of 'accessor'.

    template <typename TYPE, typename ACCESSOR>
    int accessSequence(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::Sequence' tag object as the second argument if
        // the first argument can be used with 'bdeat_sequencefunctions', or a
        // 'bslmf_Nil' tag object otherwise.  Return the result from the
        // invocation of 'accessor'.

    template <typename TYPE, typename ACCESSOR>
    int accessSimple(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of 'object' as the first argument, and either a
        // 'bdeat_TypeCategory::Simple' tag object as the second argument if
        // the first argument can be used as a simple type, or a 'bslmf_Nil'
        // tag object otherwise.  Return the result from the invocation of
        // 'accessor'.

    template <typename TYPE>
    bdeat_TypeCategory::Value select(const TYPE& object);
        // Return the *runtime* type category for the specified 'object'.
        // Dynamic types *must* overload this function and implement the
        // appropriate logic for detecting the runtime type category.


    // OVERLOADABLE FUNCTIONS
    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.

    // MANIPULATORS
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_typeCategoryManipulateArray(TYPE         *object,
                                          MANIPULATOR&  manipulator);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_typeCategoryManipulateChoice(TYPE         *object,
                                           MANIPULATOR&  manipulator);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_typeCategoryManipulateCustomizedType(TYPE         *object,
                                                   MANIPULATOR&  manipulator);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_typeCategoryManipulateEnumeration(TYPE         *object,
                                                MANIPULATOR&  manipulator);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_typeCategoryManipulateNullableValue(TYPE         *object,
                                                  MANIPULATOR&  manipulator);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_typeCategoryManipulateSequence(TYPE         *object,
                                             MANIPULATOR&  manipulator);
    template <typename TYPE, typename MANIPULATOR>
    int bdeat_typeCategoryManipulateSimple(TYPE         *object,
                                           MANIPULATOR&  manipulator);

    // ACCESSORS
    template <typename TYPE, typename ACCESSOR>
    int bdeat_typeCategoryAccessArray(const TYPE& object, ACCESSOR& accessor);
    template <typename TYPE, typename ACCESSOR>
    int bdeat_typeCategoryAccessChoice(const TYPE& object, ACCESSOR& accessor);
    template <typename TYPE, typename ACCESSOR>
    int bdeat_typeCategoryAccessCustomizedType(const TYPE& object,
                                               ACCESSOR&   accessor);
    template <typename TYPE, typename ACCESSOR>
    int bdeat_typeCategoryAccessEnumeration(const TYPE& object,
                                            ACCESSOR&   accessor);
    template <typename TYPE, typename ACCESSOR>
    int bdeat_typeCategoryAccessNullableValue(const TYPE& object,
                                              ACCESSOR&   accessor);
    template <typename TYPE, typename ACCESSOR>
    int bdeat_typeCategoryAccessSequence(const TYPE& object,
                                         ACCESSOR&   accessor);
    template <typename TYPE, typename ACCESSOR>
    int bdeat_typeCategoryAccessSimple(const TYPE& object, ACCESSOR& accessor);
    template <typename TYPE>
    bdeat_TypeCategory::Value bdeat_typeCategorySelect(const TYPE& object);

}  // close namespace bdeat_TypeCategoryFunctions

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // struct bdeat_TypeCategoryUtil
                       // -----------------------------

template <typename TYPE, typename MANIPULATOR>
int bdeat_TypeCategoryUtil::manipulateByCategory(TYPE         *object,
                                                 MANIPULATOR&  manipulator)
{
    using namespace bdeat_TypeCategoryFunctions;

    switch (select(*object)) {
      case bdeat_TypeCategory::BDEAT_DYNAMIC_CATEGORY: {
        BSLS_ASSERT_SAFE("'select' must not return 'DYNAMIC_CATEGORY'." && 0);
        return -1;
      }
      case bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY: {
        return manipulateArray(object, manipulator);
      }
      case bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY: {
        return manipulateChoice(object, manipulator);
      }
      case bdeat_TypeCategory::BDEAT_CUSTOMIZED_TYPE_CATEGORY: {
        return manipulateCustomizedType(object, manipulator);
      }
      case bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY: {
        return manipulateEnumeration(object, manipulator);
      }
      case bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY: {
        return manipulateNullableValue(object, manipulator);
      }
      case bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY: {
        return manipulateSequence(object, manipulator);
      }
      case bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY: {
        return manipulateSimple(object, manipulator);
      }
    }

    // An assertion here indicates that the 'select' function returned an
    // invalid value.
    BSLS_ASSERT_SAFE(0);
    return -1;
}

template <typename TYPE, typename ACCESSOR>
int bdeat_TypeCategoryUtil::accessByCategory(const TYPE& object,
                                             ACCESSOR&   accessor)
{
    using namespace bdeat_TypeCategoryFunctions;

    switch (select(object)) {
      case bdeat_TypeCategory::BDEAT_DYNAMIC_CATEGORY: {
        BSLS_ASSERT_SAFE("'select' must not return 'DYNAMIC_CATEGORY'." && 0);
        return -1;
      }
      case bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY: {
        return accessArray(object, accessor);
      }
      case bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY: {
        return accessChoice(object, accessor);
      }
      case bdeat_TypeCategory::BDEAT_CUSTOMIZED_TYPE_CATEGORY: {
        return accessCustomizedType(object, accessor);
      }
      case bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY: {
        return accessEnumeration(object, accessor);
      }
      case bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY: {
        return accessNullableValue(object, accessor);
      }
      case bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY: {
        return accessSequence(object, accessor);
      }
      case bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY: {
        return accessSimple(object, accessor);
      }
    }

    // An assertion here indicates that the 'select' function returned an
    // invalid value.
    BSLS_ASSERT_SAFE(0);
    return -1;
}

                   // -------------------------------------
                   // namespace bdeat_TypeCategoryFunctions
                   // -------------------------------------

// MANIPULATORS

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::manipulateArray(TYPE         *object,
                                                 MANIPULATOR&  manipulator)
{
    return bdeat_typeCategoryManipulateArray(object, manipulator);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::manipulateChoice(TYPE         *object,
                                                  MANIPULATOR&  manipulator)
{
    return bdeat_typeCategoryManipulateChoice(object, manipulator);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::manipulateCustomizedType(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    return bdeat_typeCategoryManipulateCustomizedType(object, manipulator);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::manipulateEnumeration(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    return bdeat_typeCategoryManipulateEnumeration(object, manipulator);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::manipulateNullableValue(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    return bdeat_typeCategoryManipulateNullableValue(object, manipulator);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::manipulateSequence(TYPE         *object,
                                                    MANIPULATOR&  manipulator)
{
    return bdeat_typeCategoryManipulateSequence(object, manipulator);
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::manipulateSimple(TYPE         *object,
                                                  MANIPULATOR&  manipulator)
{
    return bdeat_typeCategoryManipulateSimple(object, manipulator);
}

// ACCESSORS

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::accessArray(const TYPE& object,
                                             ACCESSOR&   accessor)
{
    return bdeat_typeCategoryAccessArray(object, accessor);
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::accessChoice(const TYPE& object,
                                              ACCESSOR&   accessor)
{
    return bdeat_typeCategoryAccessChoice(object, accessor);
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::accessCustomizedType(const TYPE& object,
                                                      ACCESSOR&   accessor)
{
    return bdeat_typeCategoryAccessCustomizedType(object, accessor);
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::accessEnumeration(const TYPE& object,
                                                   ACCESSOR&   accessor)
{
    return bdeat_typeCategoryAccessEnumeration(object, accessor);
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::accessNullableValue(const TYPE& object,
                                                     ACCESSOR&   accessor)
{
    return bdeat_typeCategoryAccessNullableValue(object, accessor);
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::accessSequence(const TYPE& object,
                                                ACCESSOR&   accessor)
{
    return bdeat_typeCategoryAccessSequence(object, accessor);
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::accessSimple(const TYPE& object,
                                              ACCESSOR&   accessor)
{
    return bdeat_typeCategoryAccessSimple(object, accessor);
}

template <typename TYPE>
inline
bdeat_TypeCategory::Value bdeat_TypeCategoryFunctions::select(
                                                            const TYPE& object)
{
    return bdeat_typeCategorySelect(object);
}

       // --------------------------------------------------------------
       // namespace bdeat_TypeCategoryFunctions (OVERLOADABLE FUNCTIONS)
       // --------------------------------------------------------------

// MANIPULATORS

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryManipulateArray(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf_If<bdeat_ArrayFunctions::IsArray<TYPE>::VALUE,
             bdeat_TypeCategory::Array, bslmf_Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryManipulateChoice(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf_If<bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE,
             bdeat_TypeCategory::Choice, bslmf_Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryManipulateCustomizedType(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf_If<bdeat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::VALUE,
             bdeat_TypeCategory::CustomizedType, bslmf_Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryManipulateEnumeration(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf_If<bdeat_EnumFunctions::IsEnumeration<TYPE>::VALUE,
             bdeat_TypeCategory::Enumeration, bslmf_Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryManipulateNullableValue(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf_If<bdeat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE,
             bdeat_TypeCategory::NullableValue, bslmf_Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryManipulateSequence(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf_If<bdeat_SequenceFunctions::IsSequence<TYPE>::VALUE,
             bdeat_TypeCategory::Sequence, bslmf_Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <typename TYPE, typename MANIPULATOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryManipulateSimple(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf_If<(int)bdeat_TypeCategory::Select<TYPE>::BDEAT_SELECTION ==
                                (int)bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY,
             bdeat_TypeCategory::Simple, bslmf_Nil>::Type Tag;
    return manipulator(object, Tag());
}

// ACCESSORS

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryAccessArray(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf_If<bdeat_ArrayFunctions::IsArray<TYPE>::VALUE,
             bdeat_TypeCategory::Array, bslmf_Nil>::Type Tag;
    return accessor(object, Tag());
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryAccessChoice(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf_If<bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE,
             bdeat_TypeCategory::Choice, bslmf_Nil>::Type Tag;
    return accessor(object, Tag());
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryAccessCustomizedType(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf_If<bdeat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::VALUE,
             bdeat_TypeCategory::CustomizedType, bslmf_Nil>::Type Tag;
    return accessor(object, Tag());
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryAccessEnumeration(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf_If<bdeat_EnumFunctions::IsEnumeration<TYPE>::VALUE,
             bdeat_TypeCategory::Enumeration, bslmf_Nil>::Type Tag;
    return accessor(object, Tag());
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryAccessNullableValue(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf_If<bdeat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE,
             bdeat_TypeCategory::NullableValue, bslmf_Nil>::Type Tag;
    return accessor(object, Tag());
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryAccessSequence(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf_If<bdeat_SequenceFunctions::IsSequence<TYPE>::VALUE,
             bdeat_TypeCategory::Sequence, bslmf_Nil>::Type Tag;
    return accessor(object, Tag());
}

template <typename TYPE, typename ACCESSOR>
inline
int bdeat_TypeCategoryFunctions::bdeat_typeCategoryAccessSimple(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf_If<(int)bdeat_TypeCategory::Select<TYPE>::BDEAT_SELECTION ==
                                (int)bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY,
             bdeat_TypeCategory::Simple, bslmf_Nil>::Type Tag;
    return accessor(object, Tag());
}

template <typename TYPE>
inline
bdeat_TypeCategory::Value
bdeat_TypeCategoryFunctions::bdeat_typeCategorySelect(const TYPE& object)
{
    enum {
        BDEAT_SELECTION = bdeat_TypeCategory::Select<TYPE>::BDEAT_SELECTION
    };

    // An assertion below indicates that 'TYPE' is a dynamic type, but does not
    // overload this 'select' function.  Dynamic types *must* overload this
    // 'select' function to implement the logic that determines the *runtime*
    // type category.

    BSLMF_ASSERT(bdeat_TypeCategory::BDEAT_DYNAMIC_CATEGORY !=
                                                              BDEAT_SELECTION);
    (void)object;
    return (bdeat_TypeCategory::Value)BDEAT_SELECTION;
}

}  // close namespace BloombergLP



#endif  // INCLUDED_BDEAT_TYPECATEGORY

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
