// bdlat_typecategory.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLAT_TYPECATEGORY
#define INCLUDED_BDLAT_TYPECATEGORY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide type category tags and a tag selection meta-function.
//
//@CLASSES:
//  bdlat_TypeCategory::Array          : tag for array types
//  bdlat_TypeCategory::Choice         : tag for choice types
//  bdlat_TypeCategory::CustomizedType : tag for customized types
//  bdlat_TypeCategory::Enumeration    : tag for enumeration types
//  bdlat_TypeCategory::NullableValue  : tag for nullable types
//  bdlat_TypeCategory::Sequence       : tag for sequence types
//  bdlat_TypeCategory::Simple         : tag for simple types
//  bdlat_TypeCategory::Select         : meta-function for selecting tag
//  bdlat_TypeCategoryDeclareDynamic   : meta-function to declare dynamic type
//  bdlat_TypeCategoryFunctions        : overloadable type category functions
//  bdlat_TypeCategoryUtil             : type category utility
//
//@SEE_ALSO: bdlat_arrayfunctions, bdlat_choicefunctions,
//           bdlat_customizedtypefunctions, bdlat_enumfunctions,
//           bdlat_nullablevaluefunctions, bdlat_sequencefunctions
//
// TBD: update this documentation to reflect the new overloaded functions
//
//@DESCRIPTION: The tag selection meta-function ('bdlat_TypeCategory::Select')
// provided in this component can be used to categorize a parameterized 'TYPE'
// into one of the following categories:
//..
//  Category Tag     Assigned To
//  ------------     -----------
//
//  Array            types that expose "array" behavior through the
//                   'bdlat_ArrayFunctions' 'namespace'.
//
//  Choice           types that expose "choice" behavior through the
//                   'bdlat_ChoiceFunctions' 'namespace'.
//
//  CustomizedType   types that expose "customized type" behavior through the
//                   'bdlat_CustomizedTypeFunctions' 'namespace'.
//
//  DynamicType      types that can select a category at runtime
//
//  Enumeration      types that expose "enumeration" behavior through the
//                   'bdlat_EnumFunctions' 'namespace'.
//
//  NullableValue    types that expose "nullable" behavior through the
//                   'bdlat_NullableValueFunctions' 'namespace'.
//
//  Sequence         types that expose "sequence" behavior through the
//                   'bdlat_SequenceFunctions' 'namespace'.
//
//  Simple           all other types.
//..
// A type may only belong to one of these categories at compile time.
//
///Dynamic Types
///-------------
// There are two ways that a type can be categorized as a dynamic type:
//..
//  o by specializing the 'bdlat_TypeCategoryDeclareDynamic' meta-function
//    and providing a 'VALUE' of 1
//  o by implementing more than one of the 'bdlat' category functions (i.e.,
//    more than one of 'bdlat_ArrayFunctions', 'bdlat_ChoiceFunctions',
//    'bdlat_CustomizedTypeFunctions', 'bdlat_EnumFunctions',
//    'bdlat_NullableValueFunctions', or 'bdlat_SequenceFunctions').
//..
// Note that AIX and GCC compilers require that the first way be used.
//
// When a type is categorized as a dynamic type, it *must* overload the
// 'bdlat_TypeCategoryFunctions::select' function to provide the necessary
// runtime logic that determines its runtime category.
//
///Usage
///-----
// This section illustrates intended use of this component.
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
//  void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Array)
//  {
//      stream << "Array";
//  }
//
//  void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Choice)
//  {
//      stream << "Choice";
//  }
//
//  void printCategory(bsl::ostream& stream,
//                     bdlat_TypeCategory::CustomizedType)
//  {
//      stream << "CustomizedType";
//  }
//
//  void printCategory(bsl::ostream& stream, bdlat_TypeCategory::DynamicType)
//  {
//      stream << "DynamicType";
//  }
//
//  void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Enumeration)
//  {
//      stream << "Enumeration";
//  }
//
//  void printCategory(bsl::ostream& stream, bdlat_TypeCategory::NullableValue)
//  {
//      stream << "NullableValue";
//  }
//
//  void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Sequence)
//  {
//      stream << "Sequence";
//  }
//
//  void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Simple)
//  {
//      stream << "Simple";
//  }
//..
// Now we can implement the 'printCategoryAndValue' function in terms of the
// 'printCategory' helper functions:
//..
//  template <typename TYPE>
//  void printCategoryAndValue(bsl::ostream& stream, const TYPE& object)
//  {
//      typedef typename
//      bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;
//
//      printCategory(stream, TypeCategory());
//
//      stream << ": ";
//
//      bdlb::PrintMethods::print(stream, object, 0, -1);
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
//      bdlb::NullableValue<int> nullableInt;
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
// 'bdlat_TypeCategoryDeclareDynamic' meta-function in the 'BloombergLP'
// namespace:
//..
//  namespace BloombergLP {
//
//      template <>
//      struct bdlat_TypeCategoryDeclareDynamic<VectorCharOrString> {
//          enum { VALUE = 1 };
//      };
//
//  }  // close namespace BloombergLP
//
//..
// Next, we define bdlat_typeCategorySelect', and a suite of four function,
// 'bdlat_typeCategory(Manipulate|Access)(Array|Simple)', each overloaded for
// our type, 'VectorCharOrString'.
//..
//  bdlat_TypeCategory::Value
//  bdlat_typeCategorySelect(const VectorCharOrString& object)
//  {
//      if (object.isVectorChar()) {
//          return bdlat_TypeCategory::e_ARRAY_CATEGORY;              // RETURN
//      }
//      else if (object.isString()) {
//          return bdlat_TypeCategory::e_SIMPLE_CATEGORY;             // RETURN
//      }
//
//      assert(0);
//      return static_cast<bdlat_TypeCategory::Value>(-1);
//  }
//
//  template <typename MANIPULATOR>
//  int bdlat_typeCategoryManipulateArray(VectorCharOrString *object,
//                                        MANIPULATOR&        manipulator)
//  {
//      if (object->isVectorChar()) {
//          return manipulator(&object->theVectorChar(),
//                             bdlat_TypeCategory::Array());          // RETURN
//      }
//
//      return manipulator(object, bslmf::Nil());
//  }
//
//  template <typename MANIPULATOR>
//  int bdlat_typeCategoryManipulateSimple(VectorCharOrString *object,
//                                         MANIPULATOR&        manipulator)
//  {
//      if (object->isString()) {
//          return manipulator(&object->theString(),
//                             bdlat_TypeCategory::Simple());         // RETURN
//      }
//
//      return manipulator(object, bslmf::Nil());
//  }
//
//  template <typename ACCESSOR>
//  int bdlat_typeCategoryAccessArray(const VectorCharOrString& object,
//                                    ACCESSOR&                 accessor)
//  {
//      if (object.isVectorChar()) {
//          return accessor(object.theVectorChar(),
//                          bdlat_TypeCategory::Array());             // RETURN
//      }
//
//      return accessor(object, bslmf::Nil());
//  }
//
//  template <typename ACCESSOR>
//  int bdlat_typeCategoryAccessSimple(const VectorCharOrString& object,
//                                     ACCESSOR&                 accessor)
//  {
//      if (object.isString()) {
//          return accessor(object.theString(),
//                          bdlat_TypeCategory::Simple());            // RETURN
//      }
//
//      return accessor(object, bslmf::Nil());
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
//      int operator()(const TYPE& object, bslmf::Nil)
//      {
//          assert(0);  // received invalid object
//          return -1;
//      }
//
//      template <typename TYPE>
//      int operator()(const TYPE& object, bdlat_TypeCategory::Array)
//      {
//          (*d_stream_p) << "Array = ";
//          bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
//          return 0;
//      }
//
//      template <typename TYPE>
//      int operator()(const TYPE& object, bdlat_TypeCategory::Simple)
//      {
//          (*d_stream_p) << "Simple = ";
//          bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
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
//      ret = bdlat_TypeCategoryUtil::accessByCategory(object, accessor);
//      assert("Array = \"Hello\"" == oss.str());
//      oss.str("");
//
//      object.makeString();
//      object.theString() = "World";
//
//      ret = bdlat_TypeCategoryUtil::accessByCategory(object, accessor);
//      assert("Simple = World" == oss.str());
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#include <bdlat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_BDEATOVERRIDES
#include <bdlat_bdeatoverrides.h>
#endif

#ifndef INCLUDED_BDLAT_CHOICEFUNCTIONS
#include <bdlat_choicefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_ENUMFUNCTIONS
#include <bdlat_enumfunctions.h>
#endif

#ifndef INCLUDED_BDLAT_NULLABLEVALUEFUNCTIONS
#include <bdlat_nullablevaluefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_CUSTOMIZEDTYPEFUNCTIONS
#include <bdlat_customizedtypefunctions.h>
#endif

#ifndef INCLUDED_BDLAT_SEQUENCEFUNCTIONS
#include <bdlat_sequencefunctions.h>
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
                         // struct bdlat_TypeCategory
                         // =========================


template <class TYPE>
struct bdlat_TypeCategoryDeclareDynamic {
    // Types that have more than one of the following traits:
    //..
    //  o bdlat_ArrayFunctions::IsArray<TYPE>::VALUE
    //  o bdlat_ChoiceFunctions::IsChoice<TYPE>::VALUE
    //  o bdlat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::VALUE
    //  o bdlat_EnumFunctions::IsEnumeration<TYPE>::VALUE
    //  o bdlat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE
    //  o bdlat_SequenceFunctions::IsSequence<TYPE>::VALUE
    //..
    // are automatically recognized as dynamic types.  However, there may exist
    // types that have *none* or only one of the above traits and still be
    // considered dynamic (e.g., "variants" of simple types etc).  In order for
    // the 'bdlat' framework to treat these types as dynamic types, this
    // 'struct' must be specialized with a 'VALUE' of 1, and the
    // 'bdlat_TypeCategoryFunctions::select' function must be implemented,
    // along with the appropriate 'bdlat_TypeCategoryFunctions::manipulate*'
    // and 'bdlat_TypeCategoryFunctions::access*' functions.  Inside these
    // 'manipulate*' and 'access*' functions, the dynamic object should expose
    // its real type.

    enum { VALUE = 0 };
};

struct bdlat_TypeCategory {
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
        e_DYNAMIC_CATEGORY         = 0,
        e_ARRAY_CATEGORY           = 1,
        e_CHOICE_CATEGORY          = 2,
        e_CUSTOMIZED_TYPE_CATEGORY = 3,
        e_ENUMERATION_CATEGORY     = 4,
        e_NULLABLE_VALUE_CATEGORY  = 5,
        e_SEQUENCE_CATEGORY        = 6,
        e_SIMPLE_CATEGORY          = 7

    };

    // META-FUNCTION
    template <class TYPE>
    struct Select {
        // This meta-function contains a typedef 'Type' that specifies the
        // category tag for the parameterized 'TYPE'.

      private:
        enum {
            e_IS_ARRAY = bdlat_ArrayFunctions::IsArray<TYPE>::VALUE,
            e_IS_CHOICE = bdlat_ChoiceFunctions::IsChoice<TYPE>::VALUE,
            e_IS_CUSTOMIZED_TYPE =
                  bdlat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::VALUE,
            e_IS_ENUMERATION = bdlat_EnumFunctions::IsEnumeration<TYPE>::VALUE,
            e_IS_NULLABLE_VALUE =
                    bdlat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE,
            e_IS_SEQUENCE = bdlat_SequenceFunctions::IsSequence<TYPE>::VALUE,
            e_NUM_CATEGORIES = e_IS_ARRAY
                             + e_IS_CHOICE
                             + e_IS_CUSTOMIZED_TYPE
                             + e_IS_ENUMERATION
                             + e_IS_NULLABLE_VALUE
                             + e_IS_SEQUENCE,
            e_IS_DYNAMIC = e_NUM_CATEGORIES > 1
                        || bdlat_TypeCategoryDeclareDynamic<TYPE>::VALUE
        };

      public:
        enum {
            e_SELECTION = e_IS_DYNAMIC         ? e_DYNAMIC_CATEGORY
                        : e_IS_ARRAY           ? e_ARRAY_CATEGORY
                        : e_IS_CHOICE          ? e_CHOICE_CATEGORY
                        : e_IS_CUSTOMIZED_TYPE ? e_CUSTOMIZED_TYPE_CATEGORY
                        : e_IS_ENUMERATION     ? e_ENUMERATION_CATEGORY
                        : e_IS_NULLABLE_VALUE  ? e_NULLABLE_VALUE_CATEGORY
                        : e_IS_SEQUENCE        ? e_SEQUENCE_CATEGORY
                        : e_SIMPLE_CATEGORY

        };

        typedef typename
        bslmf::Switch<e_SELECTION, DynamicType,
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
                       // struct bdlat_TypeCategoryUtil
                       // =============================

struct bdlat_TypeCategoryUtil {
    // This 'struct' contains utility functions pertaining to type category.

    template <class TYPE, class MANIPULATOR>
    static int manipulateByCategory(TYPE         *object,
                                    MANIPULATOR&  manipulator);
        // Invoke one of the following functions, based on the result of
        // 'bdlat_TypeCategoryFunctions::select':
        //..
        //  o bdlat_TypeCategoryFunctions::manipulateArray
        //  o bdlat_TypeCategoryFunctions::manipulateChoice
        //  o bdlat_TypeCategoryFunctions::manipulateCustomizedType
        //  o bdlat_TypeCategoryFunctions::manipulateEnumeration
        //  o bdlat_TypeCategoryFunctions::manipulateNullableValue
        //  o bdlat_TypeCategoryFunctions::manipulateSequence
        //  o bdlat_TypeCategoryFunctions::manipulateSimple
        //..
        // Return the value that was returned by the invoked function.  The
        // behavior is undefined unless the parameterized 'TYPE' supports the
        // type category that was returned by the call to the
        // 'bdlat_TypeCategoryFunctions::select' function.

    template <class TYPE, class ACCESSOR>
    static int accessByCategory(const TYPE& object,
                                ACCESSOR&   accessor);
        // Invoke one of the following functions, based on the result of
        // 'bdlat_TypeCategoryFunctions::select':
        //..
        //  o bdlat_TypeCategoryFunctions::accessArray
        //  o bdlat_TypeCategoryFunctions::accessChoice
        //  o bdlat_TypeCategoryFunctions::accessCustomizedType
        //  o bdlat_TypeCategoryFunctions::accessEnumeration
        //  o bdlat_TypeCategoryFunctions::accessNullableValue
        //  o bdlat_TypeCategoryFunctions::accessSequence
        //  o bdlat_TypeCategoryFunctions::accessSimple
        //..
        // Return the value that was returned by the invoked function.  The
        // behavior is undefined unless the parameterized 'TYPE' supports the
        // type category that was returned by the call to the
        // 'bdlat_TypeCategoryFunctions::select' function.
};

                   // =====================================
                   // namespace bdlat_TypeCategoryFunctions
                   // =====================================

namespace bdlat_TypeCategoryFunctions {
    // This namespace contains overloadable functions pertaining to type
    // categories.

    // MANIPULATORS
    template <class TYPE, class MANIPULATOR>
    int manipulateArray(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::Array' tag object as the second
        // argument if the first argument can be used with
        // 'bdlat_arrayfunctions', or a 'bslmf::Nil' tag object otherwise.
        // Return the result from the invocation of 'manipulator'.

    template <class TYPE, class MANIPULATOR>
    int manipulateChoice(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::Choice' tag object as the second
        // argument if the first argument can be used with
        // 'bdlat_choicefunctions', or a 'bslmf::Nil' tag object otherwise.
        // Return the result from the invocation of 'manipulator'.

    template <class TYPE, class MANIPULATOR>
    int manipulateCustomizedType(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::CustomizedType' tag object as the
        // second argument if the first argument can be used with
        // 'bdlat_customizedtypefunctions', or a 'bslmf::Nil' tag object
        // otherwise.  Return the result from the invocation of 'manipulator'.

    template <class TYPE, class MANIPULATOR>
    int manipulateEnumeration(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::Enumeration' tag object as the second
        // argument if the first argument can be used with
        // 'bdlat_enumfunctions', or a 'bslmf::Nil' tag object otherwise.
        // Return the result from the invocation of 'manipulator'.

    template <class TYPE, class MANIPULATOR>
    int manipulateNullableValue(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::NullableValue' tag object as the
        // second argument if the first argument can be used with
        // 'bdlat_nullablevaluefunctions', or a 'bslmf::Nil' tag object
        // otherwise.  Return the result from the invocation of 'manipulator'.

    template <class TYPE, class MANIPULATOR>
    int manipulateSequence(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::Sequence' tag object as the second
        // argument if the first argument can be used with
        // 'bdlat_sequencefunctions', or a 'bslmf::Nil' tag object otherwise.
        // Return the result from the invocation of 'manipulator'.

    template <class TYPE, class MANIPULATOR>
    int manipulateSimple(TYPE *object, MANIPULATOR& manipulator);
        // Invoke the specified 'manipulator', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::Simple' tag object as the second
        // argument if the first argument can be used as a simple type, or a
        // 'bslmf::Nil' tag object otherwise.  Return the result from the
        // invocation of 'manipulator'.

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int accessArray(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::Array' tag object as the second
        // argument if the first argument can be used with
        // 'bdlat_arrayfunctions', or a 'bslmf::Nil' tag object otherwise.
        // Return the result from the invocation of 'accessor'.

    template <class TYPE, class ACCESSOR>
    int accessChoice(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::Choice' tag object as the second
        // argument if the first argument can be used with
        // 'bdlat_choicefunctions', or a 'bslmf::Nil' tag object otherwise.
        // Return the result from the invocation of 'accessor'.

    template <class TYPE, class ACCESSOR>
    int accessCustomizedType(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::CustomizedType' tag object as the
        // second argument if the first argument can be used with
        // 'bdlat_customizedtypefunctions', or a 'bslmf::Nil' tag object
        // otherwise.  Return the result from the invocation of 'accessor'.

    template <class TYPE, class ACCESSOR>
    int accessEnumeration(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::Enumeration' tag object as the second
        // argument if the first argument can be used with
        // 'bdlat_enumfunctions', or a 'bslmf::Nil' tag object otherwise.
        // Return the result from the invocation of 'accessor'.

    template <class TYPE, class ACCESSOR>
    int accessNullableValue(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::NullableValue' tag object as the
        // second argument if the first argument can be used with
        // 'bdlat_nullablevaluefunctions', or a 'bslmf::Nil' tag object
        // otherwise.  Return the result from the invocation of 'accessor'.

    template <class TYPE, class ACCESSOR>
    int accessSequence(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::Sequence' tag object as the second
        // argument if the first argument can be used with
        // 'bdlat_sequencefunctions', or a 'bslmf::Nil' tag object otherwise.
        // Return the result from the invocation of 'accessor'.

    template <class TYPE, class ACCESSOR>
    int accessSimple(const TYPE& object, ACCESSOR& accessor);
        // Invoke the specified 'accessor', passing it an appropriate
        // representation of the specified 'object' as the first argument, and
        // either a 'bdlat_TypeCategory::Simple' tag object as the second
        // argument if the first argument can be used as a simple type, or a
        // 'bslmf::Nil' tag object otherwise.  Return the result from the
        // invocation of 'accessor'.

    template <class TYPE>
    bdlat_TypeCategory::Value select(const TYPE& object);
        // Return the *runtime* type category for the specified 'object'.
        // Dynamic types *must* overload this function and implement the
        // appropriate logic for detecting the runtime type category.


    // OVERLOADABLE FUNCTIONS

    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for 'bas_codegen.pl'-generated types).  Do *not* call
    // these functions directly.  Use the functions above instead.

    // MANIPULATORS
    template <class TYPE, class MANIPULATOR>
    int bdlat_typeCategoryManipulateArray(TYPE         *object,
                                          MANIPULATOR&  manipulator);
    template <class TYPE, class MANIPULATOR>
    int bdlat_typeCategoryManipulateChoice(TYPE         *object,
                                           MANIPULATOR&  manipulator);
    template <class TYPE, class MANIPULATOR>
    int bdlat_typeCategoryManipulateCustomizedType(TYPE         *object,
                                                   MANIPULATOR&  manipulator);
    template <class TYPE, class MANIPULATOR>
    int bdlat_typeCategoryManipulateEnumeration(TYPE         *object,
                                                MANIPULATOR&  manipulator);
    template <class TYPE, class MANIPULATOR>
    int bdlat_typeCategoryManipulateNullableValue(TYPE         *object,
                                                  MANIPULATOR&  manipulator);
    template <class TYPE, class MANIPULATOR>
    int bdlat_typeCategoryManipulateSequence(TYPE         *object,
                                             MANIPULATOR&  manipulator);
    template <class TYPE, class MANIPULATOR>
    int bdlat_typeCategoryManipulateSimple(TYPE         *object,
                                           MANIPULATOR&  manipulator);

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int bdlat_typeCategoryAccessArray(const TYPE& object, ACCESSOR& accessor);
    template <class TYPE, class ACCESSOR>
    int bdlat_typeCategoryAccessChoice(const TYPE& object, ACCESSOR& accessor);
    template <class TYPE, class ACCESSOR>
    int bdlat_typeCategoryAccessCustomizedType(const TYPE& object,
                                               ACCESSOR&   accessor);
    template <class TYPE, class ACCESSOR>
    int bdlat_typeCategoryAccessEnumeration(const TYPE& object,
                                            ACCESSOR&   accessor);
    template <class TYPE, class ACCESSOR>
    int bdlat_typeCategoryAccessNullableValue(const TYPE& object,
                                              ACCESSOR&   accessor);
    template <class TYPE, class ACCESSOR>
    int bdlat_typeCategoryAccessSequence(const TYPE& object,
                                         ACCESSOR&   accessor);
    template <class TYPE, class ACCESSOR>
    int bdlat_typeCategoryAccessSimple(const TYPE& object, ACCESSOR& accessor);
    template <class TYPE>
    bdlat_TypeCategory::Value bdlat_typeCategorySelect(const TYPE& object);

}  // close namespace bdlat_TypeCategoryFunctions

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // struct bdlat_TypeCategoryUtil
                       // -----------------------------

template <class TYPE, class MANIPULATOR>
int bdlat_TypeCategoryUtil::manipulateByCategory(TYPE         *object,
                                                 MANIPULATOR&  manipulator)
{
    using namespace bdlat_TypeCategoryFunctions;

    switch (select(*object)) {
      case bdlat_TypeCategory::e_DYNAMIC_CATEGORY: {
        BSLS_ASSERT_SAFE("'select' must not return 'DYNAMIC_CATEGORY'." && 0);
        return -1;                                                    // RETURN
      }
      case bdlat_TypeCategory::e_ARRAY_CATEGORY: {
        return manipulateArray(object, manipulator);                  // RETURN
      }
      case bdlat_TypeCategory::e_CHOICE_CATEGORY: {
        return manipulateChoice(object, manipulator);                 // RETURN
      }
      case bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY: {
        return manipulateCustomizedType(object, manipulator);         // RETURN
      }
      case bdlat_TypeCategory::e_ENUMERATION_CATEGORY: {
        return manipulateEnumeration(object, manipulator);            // RETURN
      }
      case bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY: {
        return manipulateNullableValue(object, manipulator);          // RETURN
      }
      case bdlat_TypeCategory::e_SEQUENCE_CATEGORY: {
        return manipulateSequence(object, manipulator);               // RETURN
      }
      case bdlat_TypeCategory::e_SIMPLE_CATEGORY: {
        return manipulateSimple(object, manipulator);                 // RETURN
      }
    }

    // An assertion here indicates that the 'select' function returned an
    // invalid value.
    BSLS_ASSERT_SAFE(0);
    return -1;
}

template <class TYPE, class ACCESSOR>
int bdlat_TypeCategoryUtil::accessByCategory(const TYPE& object,
                                             ACCESSOR&   accessor)
{
    using namespace bdlat_TypeCategoryFunctions;

    switch (select(object)) {
      case bdlat_TypeCategory::e_DYNAMIC_CATEGORY: {
        BSLS_ASSERT_SAFE("'select' must not return 'DYNAMIC_CATEGORY'." && 0);
        return -1;                                                    // RETURN
      }
      case bdlat_TypeCategory::e_ARRAY_CATEGORY: {
        return accessArray(object, accessor);                         // RETURN
      }
      case bdlat_TypeCategory::e_CHOICE_CATEGORY: {
        return accessChoice(object, accessor);                        // RETURN
      }
      case bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY: {
        return accessCustomizedType(object, accessor);                // RETURN
      }
      case bdlat_TypeCategory::e_ENUMERATION_CATEGORY: {
        return accessEnumeration(object, accessor);                   // RETURN
      }
      case bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY: {
        return accessNullableValue(object, accessor);                 // RETURN
      }
      case bdlat_TypeCategory::e_SEQUENCE_CATEGORY: {
        return accessSequence(object, accessor);                      // RETURN
      }
      case bdlat_TypeCategory::e_SIMPLE_CATEGORY: {
        return accessSimple(object, accessor);                        // RETURN
      }
    }

    // An assertion here indicates that the 'select' function returned an
    // invalid value.
    BSLS_ASSERT_SAFE(0);
    return -1;
}

                   // -------------------------------------
                   // namespace bdlat_TypeCategoryFunctions
                   // -------------------------------------

// MANIPULATORS

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::manipulateArray(TYPE         *object,
                                                 MANIPULATOR&  manipulator)
{
    return bdlat_typeCategoryManipulateArray(object, manipulator);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::manipulateChoice(TYPE         *object,
                                                  MANIPULATOR&  manipulator)
{
    return bdlat_typeCategoryManipulateChoice(object, manipulator);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::manipulateCustomizedType(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    return bdlat_typeCategoryManipulateCustomizedType(object, manipulator);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::manipulateEnumeration(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    return bdlat_typeCategoryManipulateEnumeration(object, manipulator);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::manipulateNullableValue(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    return bdlat_typeCategoryManipulateNullableValue(object, manipulator);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::manipulateSequence(TYPE         *object,
                                                    MANIPULATOR&  manipulator)
{
    return bdlat_typeCategoryManipulateSequence(object, manipulator);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::manipulateSimple(TYPE         *object,
                                                  MANIPULATOR&  manipulator)
{
    return bdlat_typeCategoryManipulateSimple(object, manipulator);
}

// ACCESSORS

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::accessArray(const TYPE& object,
                                             ACCESSOR&   accessor)
{
    return bdlat_typeCategoryAccessArray(object, accessor);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::accessChoice(const TYPE& object,
                                              ACCESSOR&   accessor)
{
    return bdlat_typeCategoryAccessChoice(object, accessor);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::accessCustomizedType(const TYPE& object,
                                                      ACCESSOR&   accessor)
{
    return bdlat_typeCategoryAccessCustomizedType(object, accessor);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::accessEnumeration(const TYPE& object,
                                                   ACCESSOR&   accessor)
{
    return bdlat_typeCategoryAccessEnumeration(object, accessor);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::accessNullableValue(const TYPE& object,
                                                     ACCESSOR&   accessor)
{
    return bdlat_typeCategoryAccessNullableValue(object, accessor);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::accessSequence(const TYPE& object,
                                                ACCESSOR&   accessor)
{
    return bdlat_typeCategoryAccessSequence(object, accessor);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::accessSimple(const TYPE& object,
                                              ACCESSOR&   accessor)
{
    return bdlat_typeCategoryAccessSimple(object, accessor);
}

template <class TYPE>
inline
bdlat_TypeCategory::Value bdlat_TypeCategoryFunctions::select(
                                                            const TYPE& object)
{
    return bdlat_typeCategorySelect(object);
}

       // --------------------------------------------------------------
       // namespace bdlat_TypeCategoryFunctions (OVERLOADABLE FUNCTIONS)
       // --------------------------------------------------------------

// MANIPULATORS

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateArray(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf::If<bdlat_ArrayFunctions::IsArray<TYPE>::VALUE,
             bdlat_TypeCategory::Array, bslmf::Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateChoice(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf::If<bdlat_ChoiceFunctions::IsChoice<TYPE>::VALUE,
             bdlat_TypeCategory::Choice, bslmf::Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateCustomizedType(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf::If<bdlat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::VALUE,
             bdlat_TypeCategory::CustomizedType, bslmf::Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateEnumeration(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf::If<bdlat_EnumFunctions::IsEnumeration<TYPE>::VALUE,
             bdlat_TypeCategory::Enumeration, bslmf::Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateNullableValue(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf::If<bdlat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE,
             bdlat_TypeCategory::NullableValue, bslmf::Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateSequence(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf::If<bdlat_SequenceFunctions::IsSequence<TYPE>::VALUE,
             bdlat_TypeCategory::Sequence, bslmf::Nil>::Type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateSimple(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename
    bslmf::If<(int)bdlat_TypeCategory::Select<TYPE>::e_SELECTION ==
              (int)bdlat_TypeCategory::              e_SIMPLE_CATEGORY,
             bdlat_TypeCategory::Simple, bslmf::Nil>::Type Tag;
    return manipulator(object, Tag());
}

// ACCESSORS

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessArray(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf::If<bdlat_ArrayFunctions::IsArray<TYPE>::VALUE,
             bdlat_TypeCategory::Array, bslmf::Nil>::Type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessChoice(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf::If<bdlat_ChoiceFunctions::IsChoice<TYPE>::VALUE,
             bdlat_TypeCategory::Choice, bslmf::Nil>::Type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessCustomizedType(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf::If<bdlat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::VALUE,
             bdlat_TypeCategory::CustomizedType, bslmf::Nil>::Type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessEnumeration(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf::If<bdlat_EnumFunctions::IsEnumeration<TYPE>::VALUE,
             bdlat_TypeCategory::Enumeration, bslmf::Nil>::Type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessNullableValue(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf::If<bdlat_NullableValueFunctions::IsNullableValue<TYPE>::VALUE,
             bdlat_TypeCategory::NullableValue, bslmf::Nil>::Type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessSequence(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf::If<bdlat_SequenceFunctions::IsSequence<TYPE>::VALUE,
             bdlat_TypeCategory::Sequence, bslmf::Nil>::Type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessSimple(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename
    bslmf::If<(int)bdlat_TypeCategory::Select<TYPE>::e_SELECTION ==
              (int)bdlat_TypeCategory::              e_SIMPLE_CATEGORY,
             bdlat_TypeCategory::Simple, bslmf::Nil>::Type Tag;
    return accessor(object, Tag());
}

template <class TYPE>
inline
bdlat_TypeCategory::Value
bdlat_TypeCategoryFunctions::bdlat_typeCategorySelect(const TYPE&)
{
    enum {
        e_SELECTION = bdlat_TypeCategory::Select<TYPE>::e_SELECTION
    };

    // An assertion below indicates that 'TYPE' is a dynamic type, but does not
    // overload this 'select' function.  Dynamic types *must* overload this
    // 'select' function to implement the logic that determines the *runtime*
    // type category.

    BSLMF_ASSERT(bdlat_TypeCategory::e_DYNAMIC_CATEGORY
             != (bdlat_TypeCategory::Value)e_SELECTION);

    return (bdlat_TypeCategory::Value)e_SELECTION;
}

}  // close enterprise namespace

#endif

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
