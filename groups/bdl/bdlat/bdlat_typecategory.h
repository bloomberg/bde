// bdlat_typecategory.h                                               -*-C++-*-
#ifndef INCLUDED_BDLAT_TYPECATEGORY
#define INCLUDED_BDLAT_TYPECATEGORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide type category tags and a tag selection meta-function.
//
//@CLASSES:
//  bdlat_TypeCategory::Array          : tag for array types
//  bdlat_TypeCategory::Choice         : tag for choice types
//  bdlat_TypeCategory::CustomizedType : tag for customized types
//  bdlat_TypeCategory::DynamicType    : tag for customized types
//  bdlat_TypeCategory::Enumeration    : tag for enumeration types
//  bdlat_TypeCategory::NullableValue  : tag for nullable types
//  bdlat_TypeCategory::Sequence       : tag for sequence types
//  bdlat_TypeCategory::Simple         : tag for simple types
//  bdlat_TypeCategory::Select         : meta-function for selecting tag
//  bdlat_TypeCategoryDeclareDynamic   : trait to declare dynamic type
//  bdlat_TypeCategoryFunctions        : overloadable type category functions
//  bdlat_TypeCategoryUtil             : type category utility
//
//@SEE_ALSO: bdlat_arrayfunctions, bdlat_choicefunctions,
//           bdlat_customizedtypefunctions, bdlat_enumfunctions,
//           bdlat_nullablevaluefunctions, bdlat_sequencefunctions
//
//@DESCRIPTION: This component provides an assortment of related facilities
// that fulfill different roles when writing applications that deal with
// classes that comply with the various `bdlat` class categories.  This
// facilities consist of:
// * Tags and associated enumerators
// * Meta-functions and traits
// * Utility Functions
// * Customization Points
//
///Category Tags and Enumerators
///-----------------------------
// This component defines tag types (e.g., `bdlat_TypeCategory::Array`,
// `bdlat_TypeCategory::Choice`) and enumerators (e.g.,
// `bdlat_TypeCategory::e_ARRAY_CATEGORY`
// `bdlat_TypeCategory::e_CHOICE_CATEGORY`) that can be used to describe how a
// user-defined type participates in the `bdlat` type framework and a
// meta-function, `bdlat_TypeCategory::Select`, that can be used to access that
// information for a parameterized `TYPE`.
//
// The categories and their associated tags are:
// ```
// Category Tag     Assigned To
// ------------     -----------
// Array            types that expose "array" behavior through the
//                  `bdlat_ArrayFunctions` `namespace`.
//
// Choice           types that expose "choice" behavior through the
//                  `bdlat_ChoiceFunctions` `namespace`.
//
// CustomizedType   types that expose "customized type" behavior through the
//                  `bdlat_CustomizedTypeFunctions` `namespace`.
//
// DynamicType      types that can select a category at runtime
//
// Enumeration      types that expose "enumeration" behavior through the
//                  `bdlat_EnumFunctions` `namespace`.
//
// NullableValue    types that expose "nullable" behavior through the
//                  `bdlat_NullableValueFunctions` `namespace`.
//
// Sequence         types that expose "sequence" behavior through the
//                  `bdlat_SequenceFunctions` `namespace`.
//
// Simple           all other types (i.e., scalars)
// ```
// Types that belong to the `DynamicType` category also belong to one or more
// other categories.  Otherwise, a type may only belong to one category.
//
///Meta-function: `bdlat::TypeCategory::Select`
///--------------------------------------------
// This component defines a meta-function, `bdlat_TypeCategory::Select`, used
// to detect at compile time the type category of a template parameter `TYPE`,
// which must be a `bdlat`-compatible type.  This meta-function defines two
// (nested) names:
//
// * `Type`: The category tag type for the template parameter `TYPE`.
// * `e_SELECTION`: The enumerator for the category of the template parameter
//   `TYPE`.
//
// This meta-function assumes that the template parameter `TYPE` has set the
// type trait corresponding to its `bdlat` type category.  For example, if
// `TYPE` is a `bdlat` "array" type then:
// ```
// true == bdlat_ArrayFunctions::IsArray<TYPE>::value;
// ```
// and if `TYPE` is a `bdlat` "choice" type then:
// ```
// true == bdlat_ChoiceFunctions::IsChoice<TYPE>::value;
// ```
// Additionally, `TYPE` must also meet the other requirements (e.g., defined
// functions, defined types) of its `bdlat` type category.
//
// There are two special cases:
//
// * If none of these traits have been set for `TYPE`, it is assumed to be in
//   the `Simple` (scalar) category.
// * If more than one of these traits have been set of `TYPE`, `TYPE` is
//   classified as `Dynamic`.
//   - The recommended practice is *also* set the
//     `bdlat_TypeCategoryDeclareDynamic` trait.
//
///Utility: `bdlat_TypeCategoryUtil`
///---------------------------------
// This component defines a utility `struct`, `bdlat_TypeCategoryUtil`, that
// provides two function templates: `manipulateByCategory` and
// `accessByCategory` that invoke a `MANIPULATOR` or `ACCESSOR` functor,
// respectively.  Both of these functor types provide an overload for each of
// the `bdlat` type categories (see [](#`MANIPULATOR` Functors) and
// [](#`ACCESSOR` Functors)).  These utility functions determine the `bdlat`
// type category (see `bdlat_TypeCategoryFunctions::select`) of a given object
// and then invoke the corresponding overload of the given functor.
//
///`MANIPULATOR` Functors
/// - - - - - - - - - - -
// A `MANIPULATOR` functor must be invocable by one or more of these seven
// overloads:
// ```
// /// Modify the specified `object` that matches the specified `category`.
// /// Return 0 on success and a non-zero value otherwise.  On failure,
// /// `object` is left in a valid but unspecified state.  The behavior is
// /// undefined unless the type category of `object` matches `category`.
// int MANIPULATOR(TYPE *object, bdlat_TypeCategory::Array          category);
// int MANIPULATOR(TYPE *object, bdlat_TypeCategory::Choice         category);
// int MANIPULATOR(TYPE *object, bdlat_TypeCategory::CustomizedType category);
// int MANIPULATOR(TYPE *object, bdlat_TypeCategory::Enumeration    category);
// int MANIPULATOR(TYPE *object, bdlat_TypeCategory::NullableValue  category);
// int MANIPULATOR(TYPE *object, bdlat_TypeCategory::Sequence       category);
// int MANIPULATOR(TYPE *object, bdlat_TypeCategory::Simple         category);
// ```
// Notice that, potentially, one can have an overload for each of the "static"
// `bdlat` type categories (i.e., all except for the "dynamic" category).
//
// Additionally, the functor *must* provide an overload for `bslmf::Nil`
// (required for compilation):
// ```
// /// Modify the specified `object`.  Return 0 on success and a non-zero
// /// value otherwise.  On failure, `object` is left in a valid but
// /// unspecified state.  The behavior is undefined if `TYPE` is `bdlat`
// /// compatible (i.e., belongs in one or more of the seven `bdlat`
// /// categories).  Note that `bdlat` compatible types are expected to be
// /// dispatched to one of the seven overloads declared above.
// int MANIPULATOR(TYPE *object, bslmf::Nil);
// ```
// Typically, the "Nil" overload returns an error code (or even aborts).
//
///`ACCESSOR` Functors
///- - - - - - - - - -
// A `ACCESSOR` functor must invocable by one or more of these seven overloads:
// ```
// /// Access the specified `object` that matches the specified `cat`
// /// (category).  Return 0 on success and a non-zero value otherwise.
// /// The behavior is undefined unless the type category of `object`
// /// matches `cat` (category).  Note that, in typical use, invocation of
// /// this functor object changes its state based on the state of `object`.
// int ACCESSOR(const TYPE& object, bdlat_TypeCategory::Array          cat);
// int ACCESSOR(const TYPE& object, bdlat_TypeCategory::Choice         cat);
// int ACCESSOR(const TYPE& object, bdlat_TypeCategory::CustomizedType cat);
// int ACCESSOR(const TYPE& object, bdlat_TypeCategory::Enumeration    cat);
// int ACCESSOR(const TYPE& object, bdlat_TypeCategory::NullableValue  cat);
// int ACCESSOR(const TYPE& object, bdlat_TypeCategory::Sequence       cat);
// int ACCESSOR(const TYPE& object, bdlat_TypeCategory::Simple         cat);
// ```
// Notice that, potentially, one can have an overload for each of the "static"
// `bdlat` type categories (i.e., all except for the "dyanamic" category).
//
// Additionally, the functor *must* provide an overload for `bslmf::Nil`
// (required for compilation):
// ```
// /// Access the specified `object`.  Return 0 on success and a non-zero
// /// value otherwise.  The behavior is undefined if `TYPE` is `bdlat`
// /// compatible (i.e., belongs in one or more of the seven `bdlat`
// /// categories).  Note that `bdlat` compatible types are expected to be
// /// dispatched to one of the seven overloads declared above.
// int ACCESSOR(const TYPE& object, bslmf::Nil);
// ```
// Typically, the "Nil" overload returns an error code (or even aborts).
//
///Namespace: `bdlat_TypeCategoryFunctions`
///---------------------------------------
// The namespace `bdlat_TypeCategoryFunctions` defines a suite of function
// templates for working with types that conform to one or more of the `bdlat`
// type categories.  For each of the seven type categories there are a pair of
// function templates: one to "manipulate" and one to "access" objects of
// (template parameter) `TYPE`.  For example, for `bdlat` "array" types there
// are function templates: `bdlat_TypeCategoryFunctions::manipulateArray` and
// `bdlat_TypeCategoryFunctions::accessArray`.
//
// Additionally, there is a function template,
// `bdlat_TypeCategoryFunctions::select`, that reports at runtime the type
// category of a specified object.
//
// These function templates provide a uniform interface for operations on a
// wide variety of types.  The details of what occurs in each "manipulate" or
// "access" action depends on a user-provided functor object.  See
// [](#`MANIPULATOR` Functors) and [](#`ACCESSOR` Functors) for the
// requirements on those functors.
//
// For a user-defined type to operate in the type category framework, that type
// must define in the namespace in which it is defined, all required overloads
// of the customization points used by these templates.  See
// [](#Customization Points).
//
///Customization Points
/// - - - - - - - - - -
// For a type to work with the "type category" framework, one must create, in
// the namespace where the type is defined, overloads for the "manipulator" and
// "accessor" function templates (described below) for the `bdlat` type
// category of the type.  For example, if the type being plugged into the
// framework is a `bdlat` "array" type, then overloads for
// `bdlat_typeCategoryManipulateArray` and `bdlat_typeCategoryAccessArray` are
// required.  If the type being plugged into the infrastructure belongs to more
// than one `bdlat` type category, the "manipulate" and "accessor" function
// templates for each of those type categories is required.
//
// Every type plugged into the framework must implement an overload of
// `bdlat_typeCategorySelect` function.
//
// Note that the placeholder `YOUR_TYPE` is not a template argument and should
// be replaced with the name of the type being plugged into the framework.
// ```
// // MANIPULATORS
//
// /// Return the result of invoking the specified `manipulator` with the
// /// specified `object` (1st argument) and a `bdlat_TypeCategory::Array`
// /// tag object (2nd argument).  If `object` is not a `bdlat` "array"
// /// type, pass a `bslmf::Nil` tag object as the second argument.  See
// /// [](#`MANIPULATOR` Functors) for the requirements on `manipulator`.
// template <class MANIPULATOR>
// int bdlat_typeCategoryManipulateArray(YOUR_TYPE    *object,
//                                       MANIPULATOR&  manipulator);
//
// /// Return the result of invoking the specified `manipulator` with the
// /// specified `object` (1st argument) and a `bdlat_TypeCategory::Choice`
// /// tag object (2nd argument).  If `object` is not a `bdlat` "choice"
// /// type, pass a `bslmf::Nil` tag object as the second argument.  See
// /// [](#`MANIPULATOR` Functors) for the requirements on `manipulator`.
// template <class MANIPULATOR>
// int bdlat_typeCategoryManipulateChoice(YOUR_TYPE    *object,
//                                        MANIPULATOR&  manipulator);
//
// /// Return the result of invoking the specified `manipulator` with the
// /// specified `object` (1st argument) and a
// /// `bdlat_TypeCategory::CustomizedType` tag object (2nd argument).  If
// /// `object` is not a `bdlat` "customized type" type, pass a `bslmf::Nil`
// /// tag object as the second argument.  See [](#`MANIPULATOR` Functors)
// /// for the requirements on `manipulator`.
// template <class MANIPULATOR>
// int bdlat_typeCategoryManipulateCustomizedType(YOUR_TYPE    *object,
//                                                MANIPULATOR&  manipulator);
//
// /// Return the result of invoking the specified `manipulator` with the
// /// specified `object` (1st argument) and a
// /// `bdlat_TypeCategory::Enumeration` tag object (2nd argument).  If
// /// `object` is not a `bdlat` "enumeration" type, pass a `bslmf::Nil`
// /// tag object as the second argument.  See [](#`MANIPULATOR` Functors) for
// /// the requirements on `manipulator`.
// template <class MANIPULATOR>
// int bdlat_typeCategoryManipulateEnumeration(YOUR_TYPE    *object,
//                                             MANIPULATOR&  manipulator);
//
// /// Return the result of invoking the specified `manipulator` with the
// /// specified `object` (1st argument) and a
// /// `bdlat_TypeCategory::NullableValue` tag object (2nd argument).  If
// /// `object` is not a `bdlat` "nullable value" type, pass a `bslmf::Nil`
// /// tag object as the second argument.  See [](#`MANIPULATOR` Functors) for
// /// the requirements on `manipulator`.
// template <class MANIPULATOR>
// int bdlat_typeCategoryManipulateNullableValue(YOUR_TYPE    *object,
//                                               MANIPULATOR&  manipulator);
//
// /// Return the result of invoking the specified `manipulator` with the
// /// specified `object` (1st argument) and a
// /// `bdlat_TypeCategory::Sequence` tag object (2nd argument).  If
// /// `object` is not a `bdlat` "sequence" type, pass a `bslmf::Nil` tag
// /// object as the second argument.  See [](#`MANIPULATOR` Functors) for the
// /// requirements on `manipulator`.
// template <class MANIPULATOR>
// int bdlat_typeCategoryManipulateSequence(YOUR_TYPE    *object,
//                                          MANIPULATOR&  manipulator);
//
// /// Return the result of invoking the specified `manipulator` with the
// /// specified `object` (1st argument) and a `bdlat_TypeCategory::Simple`
// /// tag object (2nd argument).  If `object` is not a `bdlat` "simple"
// /// type, pass a `bslmf::Nil` tag object as the second argument.  See
// /// [](#`MANIPULATOR` Functors) for the requirements on `manipulator`.
// template <class MANIPULATOR>
// int bdlat_typeCategoryManipulateSimple(YOUR_TYPE    *object,
//                                        MANIPULATOR&  manipulator);
//
// // ACCESSORS
//
// /// Return the result of invoking the specified `accessor` with the
// /// specified `object` (1st argument) and a `bdlat_TypeCategory::Array`
// /// tag object (2nd argument).  If `object` is not a `bdlat` "array"
// /// type, pass a `bslmf::Nil` tag object as the second argument.  See
// /// [](#`ACCESSOR` Functors) for the requirements on `accessor`.
// template <class ACCESSOR>
// int bdlat_typeCategoryAccessArray(const YOUR_TYPE& object,
//                                   ACCESSOR&        accessor);
//
// /// Return the result of invoking the specified `accessor` with the
// /// specified `object` (1st argument) and a `bdlat_TypeCategory::Choice`
// /// tag object (2nd argument).  If `object` is not a `bdlat` "choice"
// /// type, pass a `bslmf::Nil` tag object as the second argument.  See
// /// [](#`ACCESSOR` Functors) for the requirements on `accessor`.
// template <class ACCESSOR>
// int bdlat_typeCategoryAccessChoice(const YOUR_TYPE& object,
//                                    ACCESSOR&        accessor);
//
// /// Return the result of invoking the specified `accessor` with the
// /// specified `object` (1st argument) and a
// /// `bdlat_TypeCategory::CustomizedType` tag object (2nd argument).  If
// /// `object` is not a `bdlat` "customized type" type, pass a `bslmf::Nil`
// /// tag object as the second argument.  See [](#`ACCESSOR` Functors) for
// /// the requirements on `accessor`.
// template <class ACCESSOR>
// int bdlat_typeCategoryAccessCustomizedType(const YOUR_TYPE& object,
//                                            ACCESSOR&        accessor);
//
// /// Return the result of invoking the specified `accessor` with the
// /// specified `object` (1st argument) and a
// /// `bdlat_TypeCategory::Enumeration` tag object (2nd argument).  If
// /// `object` is not a `bdlat` "enumeration type" type, pass a `bslmf::Nil`
// /// tag object as the second argument.  See [](#`ACCESSOR` Functors) for
// /// the requirements on `accessor`.
// template <class ACCESSOR>
// int bdlat_typeCategoryAccessEnumeration(const YOUR_TYPE& object,
//                                         ACCESSOR&        accessor);
//
// /// Return the result of invoking the specified `accessor` with the
// /// specified `object` (1st argument) and a
// /// `bdlat_TypeCategory::NullableValue` tag object (2nd argument).  If
// /// `object` is not a `bdlat` "nullable value" type, pass a `bslmf::Nil`
// /// tag object as the second argument.  See [](#`ACCESSOR` Functors) for
// /// the requirements on `accessor`.
// template <class ACCESSOR>
// int bdlat_typeCategoryAccessNullableValue(const YOUR_TYPE& object,
//                                           ACCESSOR&        accessor);
//
// /// Return the result of invoking the specified `accessor` with the
// /// specified `object` (1st argument) and a
// /// `bdlat_TypeCategory::Sequence` tag object (2nd argument).  If
// /// `object` is not a `bdlat` "sequence" type, pass a `bslmf::Nil` tag
// /// object as the second argument.  See [](#`ACCESSOR` Functors) for the
// /// requirements on `accessor`.
// template <class ACCESSOR>
// int bdlat_typeCategoryAccessSequence(const YOUR_TYPE& object,
//                                      ACCESSOR&        accessor);
//
// /// Return the result of invoking the specified `accessor` with the
// /// specified `object` (1st argument) and a `bdlat_TypeCategory::Simple`
// /// tag object (2nd argument).  If `object` is not a `bdlat` "simple"
// /// type, pass a `bslmf::Nil` tag object as the second argument.  See
// /// [](#`ACCESSOR` Functors) for the requirements on `accessor`.
// template <class ACCESSOR>
// int bdlat_typeCategoryAccessSimple(const YOUR_TYPE& object,
//                                    ACCESSOR&        accessor);
//
// /// Return the *runtime* type category for the specified `object`.  The
// /// behavior is undefined if `bdlat_TypeCategory::e_DYNAMIC_CATEGORY` is
// /// returned.  Note that the compile time analog to this function
// /// (template) is `bdlat_TypeCategory::Select`, a meta-function that
// /// defines a type corresponding to the category tag class of `YOUR_TYPE`.
// bdlat_TypeCategory::Value bdlat_typeCategorySelect(const YOUR_TYPE& object);
// ```
//
///Dynamic Types
///-------------
// "Dynamic" types allow the type category of an object to be determined at
// runtime.  Of course, such types must meet all of the requirements of each of
// the `bdlat` type categories to which they belong.
//
// There are two ways that a type can be recognized as a dynamic type:
//
// * specializing the `bdlat_TypeCategoryDeclareDynamic` trait and providing a
//   `value` of `true`, and
// * implementing all of the requirements (i.e., setting of required traits)
//   of each of the `bdlat` categories the type can assume.
//
// If a type `T` is recognized as a "dynamic" `bdlat` type then
// `bdlat_TypeCategory::Select<T>::Type` is an alias for
// `bdlat_TypeCategory::DynamicType`
//
// When a type is categorized as a dynamic type, it *must* overload the
// `bdlat_TypeCategoryFunctions::select` function to provide the necessary
// runtime logic that determines its current runtime category.  See
// [](#Customization Points).
//
// A type of the `bdlat` "dynamic" category is illustrated in
// [](#Example 3: Dynamic Typing and Dispatch).
//
///Trait: `bdlat_TypeCategoryDeclareDynamic`
///- - - - - - - - - - - - - - - - - - - - -
// Although one is not required to specialize the
// `bdlat_TypeCategoryDeclareDynamic` trait to designate a user-defined class
// as a "dynamic" type, doing so is the recommended practice.
//
// Note that AIX and GCC compilers *require* that trait be specialized; setting
// multiple `bdlat` category traits does not work on those platforms.
//
// Also note that setting that trait is the only way to create a "dynamic" type
// that implements just one of the `bdlat` type categories -- an unusual
// configuration, but one that is allowed by the `bdlat` type category
// framework.
//
// Finally note that the `bdlat_TypeCategoryDeclareDynamic` trait is the *only*
// trait in this component that users are allowed to specialize.  The behavior
// is undefined if any of the other meta-functions are specialized.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Compile-Time Dispatch by Category Type
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The `bdlat_typecategory` framework provides facilities to control actions
// based on the `bdlat` type category of the objects being used.  Dispatching
// on type category can be achieved both at compile time and at runtime.
// Depending on that context, different facilities, having different
// restrictions/requirements are used.  There are interesting differences when
// dealing with objects in the "dynamic" type category.
//
// This first example explores compile-time dispatch.  Suppose we have an
// object that is compliant with one of the `bdlat` type categories and that we
// wish to examine it to the extent of determining into which type category it
// falls and what value it has.
//
// First, we declare a `printCategoryAndValue` function that has a template
// parameter `TYPE`:
// ```
// namespace BloombergLP {
// namespace mine {
//
// /// Print the category of the specified `object` followed by the value
// /// of `object` to the specified output `stream`.
// template <class TYPE>
// void printCategoryAndValue(bsl::ostream& stream, const TYPE& object);
// ```
// Then, to implement this function, we will use a set of helper functions that
// are overloaded based on the category tag.  The first set of helper functions
// address the category aspect of our assigned goal:
// ```
// void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Array)
// {
//     stream << "Array";
// }
//
// void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Choice)
// {
//     stream << "Choice";
// }
//
// void printCategory(bsl::ostream& stream,
//                    bdlat_TypeCategory::CustomizedType)
// {
//     stream << "CustomizedType";
// }
//
// void printCategory(bsl::ostream& stream, bdlat_TypeCategory::DynamicType)
// {
//     stream << "DynamicType";
// }
//
// void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Enumeration)
// {
//     stream << "Enumeration";
// }
//
// void printCategory(bsl::ostream& stream, bdlat_TypeCategory::NullableValue)
// {
//     stream << "NullableValue";
// }
//
// void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Sequence)
// {
//     stream << "Sequence";
// }
//
// void printCategory(bsl::ostream& stream, bdlat_TypeCategory::Simple)
// {
//     stream << "Simple";
// }
// ```
// Next, we implement another helper function template to handle the value
// aspect of our goal:
// ```
// template <class TYPE, class CATEGORY>
// void printValue(bsl::ostream& stream,
//                 const TYPE&   object,
//                 CATEGORY      )
// {
//     bdlb::PrintMethods::print(stream, object, 0, -1);
// }
//
// template <class TYPE>
// void printValue(bsl::ostream&                   stream,
//                 const TYPE&                     ,
//                 bdlat_TypeCategory::DynamicType )
// {
//     stream << "Printing dynamic types requires extra work.";
// }
// ```
// Notice that a partial specialization was created for objects falling into
// the "dynamic" category.  Determining the value of such objects will be
// explored in [](#Example 3: Dynamic Typing and Dispatch).
//
// Now, we can implement the `printCategoryAndValue` function in terms of the
// `printCategory` and `printValue` helper functions:
// ```
// template <class TYPE>
// void printCategoryAndValue(bsl::ostream& stream, const TYPE& object)
// {
//     typedef typename
//     bdlat_TypeCategory::Select<TYPE>::Type TypeCategory;
//
//     printCategory(stream, TypeCategory());
//
//     stream << ": ";
//
//     printValue(stream, object, TypeCategory());
// }
//
// }  // close package namespace
// }  // close enterprise namespace
// ```
// Finally, we can exercise the `printCategoryAndValue` function on objects
// that fall in different (non-dynamic) type categories.
// ```
// void runUsageExample1()
// {
//     bsl::ostringstream oss;
//
//     int intVal = 123;
//
//     mine::printCategoryAndValue(oss, intVal);
//     assert("Simple: 123" == oss.str());
//     oss.str("");
//
//     bdlb::NullableValue<int> nullableInt;
//
//     mine::printCategoryAndValue(oss, nullableInt);
//     assert("NullableValue: NULL" == oss.str());
//     oss.str("");
//
//     nullableInt = 321;
//
//     mine::printCategoryAndValue(oss, nullableInt);
//     assert("NullableValue: 321" == oss.str());
//     oss.str("");
//
//     bsl::vector<int> vec;
//
//     vec.push_back(123);
//     vec.push_back(345);
//     vec.push_back(987);
//
//     mine::printCategoryAndValue(oss, vec);
//     assert("Array: [ 123 345 987 ]" == oss.str());
// }
// ```
//
///Example 2: Run-Time Dispatch by `bdlat_TypeCategoryUtil`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// For run-time dispatching we can use the utility functions provided by
// `bdlat_TypeCategoryUtil`.  Suppose we wish to examine the type category and
// value of an arbitrary `bdlat` compatible object, as we did in
// [](#Example 1: Compile-Time Dispatch by Category Type).
//
// First, we define `mine::PrintAccessor`, a functor that encapsulates the
// action to be taken:
// ```
// namespace BloombergLP {
// namespace mine {
//
// class PrintAccessor {
//
//     bsl::ostream  *d_stream_p;
//
//   public:
//     PrintAccessor(bsl::ostream *stream)
//     : d_stream_p(stream) { assert(stream); }
//
//     template <class TYPE>
//     int operator()(const TYPE& , bslmf::Nil )
//     {
//         *d_stream_p << "Nil";
//         return -1;
//     }
//
//     template <class TYPE>
//     int operator()(const TYPE& object, bdlat_TypeCategory::Array)
//     {
//         *d_stream_p << "Array" << ": ";
//         bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
//         return 0;
//     }
//
//     template <class TYPE>
//     int operator()(const TYPE& object, bdlat_TypeCategory::Choice)
//     {
//         *d_stream_p << "Choice" << ": ";
//         bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
//         return 0;
//     }
//
//     template <class TYPE>
//     int operator()(const TYPE& object, bdlat_TypeCategory::CustomizedType)
//     {
//         *d_stream_p << "CustomizedType" << ": ";
//         bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
//         return 0;
//     }
//
//     template <class TYPE>
//     int operator()(const TYPE& object, bdlat_TypeCategory::Enumeration)
//     {
//         *d_stream_p << "Enumeration" << ": ";
//         bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
//         return 0;
//     }
//
//     template <class TYPE>
//     int operator()(const TYPE& object, bdlat_TypeCategory::NullableValue)
//     {
//         *d_stream_p << "NullableValue" << ": ";
//         bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
//         return 0;
//     }
//
//     template <class TYPE>
//     int operator()(const TYPE& object, bdlat_TypeCategory::Sequence)
//     {
//         *d_stream_p << "Sequence" << ": ";
//         bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
//         return 0;
//     }
//
//     template <class TYPE>
//     int operator()(const TYPE& object, bdlat_TypeCategory::Simple)
//     {
//         *d_stream_p << "Simple" << ": ";
//         bdlb::PrintMethods::print(*d_stream_p, object, 0, -1);
//         return 0;
//     }
// };
//
// }  // close package namespace
// }  // close enterprise namespace
// ```
// Notice that this overload set for `operator()` includes an overload for
// `bslmf::Nil` (as required) but does *not* include an overload for
// `bdlat_TypeCategory::DynamicType` which is never reported as a runtime type
// category.
//
// Now, we can simply use `bdlat_TypeCategoryUtil` to determine the type of a
// given object dispatch control to the corresponding overload of the accessor
// functor:
// ```
// void runUsageExample2()
// {
//     bsl::ostringstream oss;
//     mine::PrintAccessor accessor(&oss);
//
//     int intVal = 123;
//
//     bdlat_TypeCategoryUtil::accessByCategory(intVal, accessor);
//     assert("Simple: 123" == oss.str());
//     oss.str("");
//
//     bdlb::NullableValue<int> nullableInt;
//
//     bdlat_TypeCategoryUtil::accessByCategory(nullableInt, accessor);
//     assert("NullableValue: NULL" == oss.str());
//     oss.str("");
//
//     nullableInt = 321;
//
//     bdlat_TypeCategoryUtil::accessByCategory(nullableInt, accessor);
//     assert("NullableValue: 321" == oss.str());
//     oss.str("");
//
//     bsl::vector<int> vec;
//
//     vec.push_back(123);
//     vec.push_back(345);
//     vec.push_back(987);
//
//     bdlat_TypeCategoryUtil::accessByCategory(vec, accessor);
//     LOOP_ASSERT(oss.str(), "Array: [ 123 345 987 ]" == oss.str());
//     oss.str("");
// }
// ```
//
///Example 3: Dynamic (Run-Time) Typing and Dispatch
///- - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we introduce a class that is the `bdlat` "dynamic" type
// category and show how its behavior is a generalization of what we have seen
// for the "static" `bdlat` types.
//
// First, we define a class, `mine::MyDynamicType`, that can hold one of two
// value types: either a `bsl::vector<char>` or a `bsl::string`.
// ```
// namespace BloombergLP {
// namespace mine {
//
// /// This class can represent data in two forms: either a `bsl::string`
// /// or as a `bsl::vector` of `char` values.
// class MyDynamicType {
//
//     // PRIVATE DATA MEMBERS
//     bsl::vector<char> d_vectorChar;  // Note: Production code should use a
//     bsl::string       d_string;      //       union of object buffers.
//     int               d_selector;    // 0 = vectorChar, 1 = string
//
//   public:
//     // MANIPULATORS
//     void makeVectorChar() { d_selector = 0; }
//     void makeString()     { d_selector = 1; }
//
//     bsl::vector<char>& theVectorChar()
//                            { assert(isVectorChar()); return d_vectorChar; }
//     bsl::string& theString()
//                            { assert(isString());     return d_string;     }
//
//     // ACCESSORS
//     bool isVectorChar() const { return 0 == d_selector; }
//     bool isString()     const { return 1 == d_selector; }
//
//     const bsl::vector<char>& theVectorChar() const
//                            { assert(isVectorChar()); return d_vectorChar; }
//     const bsl::string& theString() const
//                            { assert(isString());     return d_string;     }
//
// };
//
// }  // close package namespace
// }  // close enterprise namespace
// ```
// When acting as a vector this class is a `bdlat` "array" type and when
// holding a string, the class is a `bdlat` "simple" type.  Since this type can
// be in two type categories (determined at runtime) this class is deemed a
// "dynamic" class (for calculations at compile time).
//
// Then, to denote that this class is a dynamic type, we specialize the
// `bdlat_TypeCategoryDeclareDynamic` meta-function in the `BloombergLP`
// namespace:
// ```
// namespace BloombergLP {
//
//     template <>
//     struct bdlat_TypeCategoryDeclareDynamic<mine::MyDynamicType>
//     : bsl::true_type {
//     };
//
// }  // close enterprise namespace
// ```
// Now, we define bdlat_typeCategorySelect', and a suite of four functions,
// `bdlat_typeCategory(Manipulate|Access)(Array|Simple)`, each overloaded for
// our type, `MyDynamicType`.
// ```
// namespace BloombergLP {
// namespace mine {
//
//  bdlat_TypeCategory::Value
//  bdlat_typeCategorySelect(const MyDynamicType& object)
//  {
//      if (object.isVectorChar()) {
//          return bdlat_TypeCategory::e_ARRAY_CATEGORY;              // RETURN
//      }
//      else if (object.isString()) {
//          return bdlat_TypeCategory::e_SIMPLE_CATEGORY;             // RETURN
//      }
//
//      assert(0 == "Reached");
//
//      // Note that this `return` is never reached and hence the returned
//      // value is immaterial.
//
//      return bdlat_TypeCategory::e_SIMPLE_CATEGORY;
//  }
//
//  template <class MANIPULATOR>
//  int bdlat_typeCategoryManipulateArray(MyDynamicType *object,
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
//  template <class MANIPULATOR>
//  int bdlat_typeCategoryManipulateSimple(MyDynamicType *object,
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
//  template <class ACCESSOR>
//  int bdlat_typeCategoryAccessArray(const MyDynamicType& object,
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
//  template <class ACCESSOR>
//  int bdlat_typeCategoryAccessSimple(const MyDynamicType& object,
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
//  }  // close package namespace
//  }  // close enterprise namespace
// ```
// Notice that the customization points were implemented for just the two type
// categories that `MyDynamicType` can achieve: "array" and "simple".
//
// Finally, we can see how the facilities we developed in
// [](#Example 1: Compile-Time Dispatch by Category Type) and
// [](#Example 2: Run-Time Dispatch by bdlat_TypeCategoryUtil) behave when
// given a "dynamic" type;
// ```
// void runUsageExample3()
// {
// ```
// We see that the `Select` meta-function returns the expected value:
// ```
//     assert(bdlat_TypeCategory::e_DYNAMIC_CATEGORY
//         == static_cast<bdlat_TypeCategory::Value>(
//            bdlat_TypeCategory::Select<mine::MyDynamicType>::e_SELECTION));
// ```
// We create an object of our dynamic type and observe that the specialization
// we created for printing the values (actually, for *not* printing the value
// of) "dynamic" types is invoked:
// ```
//     bsl::ostringstream  oss;
//     mine::MyDynamicType object;
//
//     mine::printCategoryAndValue(oss, object);
//     assert("DynamicType: Printing dynamic types requires extra work."
//            == oss.str());
//     oss.str("");
// ```
// We instruct object to behave as a vector and see that the `bdlat` framework
// treats the object as a member of the "array" category and the
// `PrintAccessor` we defined in
// [](#Example 2: Run-Time Dispatch by bdlat_TypeCategoryUtil) treats `object`
// as a member of the "array" category:
// ```
//     object.makeVectorChar();
//
//     assert(bdlat_TypeCategory::e_ARRAY_CATEGORY
//         == bdlat_TypeCategoryFunctions::select(object));
//
//     object.theVectorChar().push_back('H');
//     object.theVectorChar().push_back('e');
//     object.theVectorChar().push_back('l');
//     object.theVectorChar().push_back('l');
//     object.theVectorChar().push_back('o');
//
//     mine::PrintAccessor accessor(&oss);
//     int                 ret;
//
//     ret = bdlat_TypeCategoryUtil::accessByCategory(object, accessor);
//     assert(0 == ret);
//     LOOP_ASSERT(oss.str(), "Array: \"Hello\"" == oss.str());
//     oss.str("");
// ```
// Lastly, we instruct `object` to behave as a string and find that the `bdlat`
// framework now considers `object` to be the "simple" category:
// ```
//     object.makeString();
//
//     assert(bdlat_TypeCategory::e_SIMPLE_CATEGORY
//         == bdlat_TypeCategoryFunctions::select(object));
//
//     object.theString() = "World";
//
//     ret = bdlat_TypeCategoryUtil::accessByCategory(object, accessor);
//     assert(0 == ret);
//     assert("Simple: World" == oss.str());
//     oss.str("");
// }
// ```
// Notice that the output of the accessor matches the state of the object,
// reporting an "array" type when the object `isVector` and a "simple" type
// when the object `isString`.

#include <bdlscm_version.h>

#include <bdlat_arrayfunctions.h>
#include <bdlat_bdeatoverrides.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_nullablevaluefunctions.h>
#include <bdlat_customizedtypefunctions.h>
#include <bdlat_sequencefunctions.h>

#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_issame.h>
#include <bslmf_switch.h>

#include <bsls_assert.h>

namespace BloombergLP {

                         // =======================================
                         // struct bdlat_TypeCategoryDeclareDynamic
                         // =======================================

/// Types that have more than one of the following traits:
///
/// * `bdlat_ArrayFunctions::IsArray<TYPE>::value`
/// * `bdlat_ChoiceFunctions::IsChoice<TYPE>::value`
/// * `bdlat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::value`
/// * `bdlat_EnumFunctions::IsEnumeration<TYPE>::value`
/// * `bdlat_NullableValueFunctions::IsNullableValue<TYPE>::value`
/// * `bdlat_SequenceFunctions::IsSequence<TYPE>::value`
///
/// are automatically recognized as dynamic types.  However, there may exist
/// types that have *none* or only one of the above traits and still be
/// considered dynamic (e.g., "variants" of simple types etc).  In order for
/// the `bdlat` framework to treat these types as dynamic types, this
/// `struct` must be specialized with a `value` of `true` or inheritance from
/// bsl::true_type, and the `bdlat_TypeCategoryFunctions::select` function
/// must be implemented, along with the appropriate
/// `bdlat_TypeCategoryFunctions::manipulate*` and
/// `bdlat_TypeCategoryFunctions::access*` functions.  Inside these
/// `manipulate*` and `access*` functions, the dynamic object should expose
/// its real type.
template <class TYPE>
struct bdlat_TypeCategoryDeclareDynamic : public bsl::false_type {
};

                         // =========================
                         // struct bdlat_TypeCategory
                         // =========================

/// This struct provides the different category tags and also a
/// meta-function for selecting the tags based on the parameterized `TYPE`.
struct bdlat_TypeCategory {

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , DYNAMIC_CATEGORY               = e_DYNAMIC_CATEGORY
      , ARRAY_CATEGORY                 = e_ARRAY_CATEGORY
      , CHOICE_CATEGORY                = e_CHOICE_CATEGORY
      , CUSTOMIZED_TYPE_CATEGORY       = e_CUSTOMIZED_TYPE_CATEGORY
      , ENUMERATION_CATEGORY           = e_ENUMERATION_CATEGORY
      , NULLABLE_VALUE_CATEGORY        = e_NULLABLE_VALUE_CATEGORY
      , SEQUENCE_CATEGORY              = e_SEQUENCE_CATEGORY
      , SIMPLE_CATEGORY                = e_SIMPLE_CATEGORY

      , BDEAT_DYNAMIC_CATEGORY         = e_DYNAMIC_CATEGORY
      , BDEAT_ARRAY_CATEGORY           = e_ARRAY_CATEGORY
      , BDEAT_CHOICE_CATEGORY          = e_CHOICE_CATEGORY
      , BDEAT_CUSTOMIZED_TYPE_CATEGORY = e_CUSTOMIZED_TYPE_CATEGORY
      , BDEAT_ENUMERATION_CATEGORY     = e_ENUMERATION_CATEGORY
      , BDEAT_NULLABLE_VALUE_CATEGORY  = e_NULLABLE_VALUE_CATEGORY
      , BDEAT_SEQUENCE_CATEGORY        = e_SEQUENCE_CATEGORY
      , BDEAT_SIMPLE_CATEGORY          = e_SIMPLE_CATEGORY
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // META-FUNCTION

    /// This meta-function contains a typedef `Type` that specifies the
    /// category tag for the parameterized `TYPE`.  Note that the runtime
    /// analog to this meta-function is
    /// `bdlat_TypeCategoryFunctions::select` that returns the enumerator
    /// corresponding to the `bdlat` category.
    template <class TYPE>
    struct Select {

      private:
        static const int k_IS_ARRAY =
                           bdlat_ArrayFunctions::IsArray<TYPE>::value;
        static const int k_IS_CHOICE =
                         bdlat_ChoiceFunctions::IsChoice<TYPE>::value;
        static const int k_IS_CUSTOMIZED_TYPE =
         bdlat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::value;
        static const int k_IS_ENUMERATION =
                      bdlat_EnumFunctions::IsEnumeration<TYPE>::value;
        static const int k_IS_NULLABLE_VALUE =
           bdlat_NullableValueFunctions::IsNullableValue<TYPE>::value;
        static const int k_IS_SEQUENCE =
                     bdlat_SequenceFunctions::IsSequence<TYPE>::value;
        static const int k_NUM_CATEGORIES = k_IS_ARRAY
                                          + k_IS_CHOICE
                                          + k_IS_CUSTOMIZED_TYPE
                                          + k_IS_ENUMERATION
                                          + k_IS_NULLABLE_VALUE
                                          + k_IS_SEQUENCE;
        static const int k_IS_DYNAMIC =
                        k_NUM_CATEGORIES > 1
                     || bdlat_TypeCategoryDeclareDynamic<TYPE>::value;

      public:
        enum {
            e_SELECTION = k_IS_DYNAMIC         ? e_DYNAMIC_CATEGORY
                        : k_IS_ARRAY           ? e_ARRAY_CATEGORY
                        : k_IS_CHOICE          ? e_CHOICE_CATEGORY
                        : k_IS_CUSTOMIZED_TYPE ? e_CUSTOMIZED_TYPE_CATEGORY
                        : k_IS_ENUMERATION     ? e_ENUMERATION_CATEGORY
                        : k_IS_NULLABLE_VALUE  ? e_NULLABLE_VALUE_CATEGORY
                        : k_IS_SEQUENCE        ? e_SEQUENCE_CATEGORY
                        : /* else */             e_SIMPLE_CATEGORY

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
          , SELECTION       = e_SELECTION
          , BDEAT_SELECTION = e_SELECTION
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
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

/// This `struct` contains utility functions that operate on `bdlat`
/// compatible objects, implicitly dispatching to the action appropriate for
/// the `bdlat` type category of the object.
struct bdlat_TypeCategoryUtil {

    /// Invoke one of the following functions, based on the result of
    /// `bdlat_TypeCategoryFunctions::select(object)`:
    ///
    /// * `bdlat_TypeCategoryFunctions::manipulateArray`
    /// * `bdlat_TypeCategoryFunctions::manipulateChoice`
    /// * `bdlat_TypeCategoryFunctions::manipulateCustomizedType`
    /// * `bdlat_TypeCategoryFunctions::manipulateEnumeration`
    /// * `bdlat_TypeCategoryFunctions::manipulateNullableValue`
    /// * `bdlat_TypeCategoryFunctions::manipulateSequence`
    /// * `bdlat_TypeCategoryFunctions::manipulateSimple`
    ///
    /// where each function is invoked with the specified `object` and
    /// `manipulator`.  See [](#`MANIPULATOR` Functors) for the requirements
    /// on `manipulator`.  Return the value returned by the invoked function.
    /// The behavior is undefined unless the parameterized `TYPE` supports the
    /// type category returned by the call to the
    /// `bdlat_TypeCategoryFunctions::select` function.
    template <class TYPE, class MANIPULATOR>
    static int manipulateByCategory(TYPE         *object,
                                    MANIPULATOR&  manipulator);

    /// Invoke one of the following functions, based on the result of
    /// `bdlat_TypeCategoryFunctions::select`:
    ///
    /// * `bdlat_TypeCategoryFunctions::accessArray`
    /// * `bdlat_TypeCategoryFunctions::accessChoice`
    /// * `bdlat_TypeCategoryFunctions::accessCustomizedType`
    /// * `bdlat_TypeCategoryFunctions::accessEnumeration`
    /// * `bdlat_TypeCategoryFunctions::accessNullableValue`
    /// * `bdlat_TypeCategoryFunctions::accessSequence`
    /// * `bdlat_TypeCategoryFunctions::accessSimple`
    ///
    /// where each function is invoked with the specified `object` and
    /// `accessor`.  See [](#`ACCESSOR` Functors) for the requirements on
    /// `accessor`.  Return the value returned by the invoked function.  The
    /// behavior is undefined unless the parameterized `TYPE` supports the
    /// type category returned by the call to the
    /// `bdlat_TypeCategoryFunctions::select` function.
    template <class TYPE, class ACCESSOR>
    static int accessByCategory(const TYPE& object,
                                ACCESSOR&   accessor);
};

                   // =====================================
                   // namespace bdlat_TypeCategoryFunctions
                   // =====================================

/// This namespace contains function templates pertaining to type categories.
/// For each of the seven `bdlat` type categories there are two function
/// templates: "manipulate*Type*" and "access*Type".  Additionally, the
/// `select` function template returns the type (an enumerator) for a given
/// object.
namespace bdlat_TypeCategoryFunctions {

    // MANIPULATORS

    /// Return the result of invoking the specified `manipulator` with the
    /// specified `object` (1st argument) and a `bdlat_TypeCategory::Array`
    /// tag object (2nd argument).  If `object` is not a `bdlat` "array"
    /// type, pass a `bslmf::Nil` tag object as the second argument.  See
    /// [](#`MANIPULATOR` Functors) for the requirements on `manipulator`.
    template <class TYPE, class MANIPULATOR>
    int manipulateArray(TYPE *object, MANIPULATOR& manipulator);

    /// Return the result of invoking the specified `manipulator` with the
    /// specified `object` (1st argument) and a `bdlat_TypeCategory::Choice`
    /// tag object (2nd argument).  If `object` is not a `bdlat` "choice"
    /// type, pass a `bslmf::Nil` tag object as the second argument.  See
    /// [](#`MANIPULATOR` Functors) for the requirements on `manipulator`.
    template <class TYPE, class MANIPULATOR>
    int manipulateChoice(TYPE *object, MANIPULATOR& manipulator);

    /// Return the result of invoking the specified `manipulator` with the
    /// specified `object` (1st argument) and a
    /// `bdlat_TypeCategory::CustomizedType` tag object (2nd argument).  If
    /// `object` is not a `bdlat` "customized type" type, pass a
    /// `bslmf::Nil` tag object as the second argument.  See [](#`MANIPULATOR`
    /// Functors) for the requirements on `manipulator`.
    template <class TYPE, class MANIPULATOR>
    int manipulateCustomizedType(TYPE *object, MANIPULATOR& manipulator);

    /// Return the result of invoking the specified `manipulator` with the
    /// specified `object` (1st argument) and a
    /// `bdlat_TypeCategory::Enumeration` tag object (2nd argument).  If
    /// `object` is not a `bdlat` "enumeration" type, pass a `bslmf::Nil`
    /// tag object as the second argument.  See [](#`MANIPULATOR` Functors) for
    /// the requirements on `manipulator`.
    template <class TYPE, class MANIPULATOR>
    int manipulateEnumeration(TYPE *object, MANIPULATOR& manipulator);

    /// Return the result of invoking the specified `manipulator` with the
    /// specified `object` (1st argument) and a
    /// `bdlat_TypeCategory::NullableValue` tag object (2nd argument).  If
    /// `object` is not a `bdlat` "nullable value" type, pass a `bslmf::Nil`
    /// tag object as the second argument.  See [](#`MANIPULATOR` Functors) for
    /// the requirements on `manipulator`.
    template <class TYPE, class MANIPULATOR>
    int manipulateNullableValue(TYPE *object, MANIPULATOR& manipulator);

    /// Return the result of invoking the specified `manipulator` with the
    /// specified `object` (1st argument) and a
    /// `bdlat_TypeCategory::Sequence` tag object (2nd argument).  If
    /// `object` is not a `bdlat` "sequence" type, pass a `bslmf::Nil` tag
    /// object as the second argument.  See [](#`MANIPULATOR` Functors) for the
    /// requirements on `manipulator`.
    template <class TYPE, class MANIPULATOR>
    int manipulateSequence(TYPE *object, MANIPULATOR& manipulator);

    /// Return the result of invoking the specified `manipulator` with the
    /// specified `object` (1st argument) and a `bdlat_TypeCategory::Simple`
    /// tag object (2nd argument).  If `object` is not a `bdlat` "simple"
    /// type, pass a `bslmf::Nil` tag object as the second argument.  See
    /// [](#`MANIPULATOR` Functors) for the requirements on `manipulator`.
    template <class TYPE, class MANIPULATOR>
    int manipulateSimple(TYPE *object, MANIPULATOR& manipulator);

    // ACCESSORS

    /// Return the result of invoking the specified `accessor` with the
    /// specified `object` (1st argument) and a `bdlat_TypeCategory::Array`
    /// tag object (2nd argument).  If `object` is not a `bdlat` "array"
    /// type, pass a `bslmf::Nil` tag object as the second argument.  See
    /// [](#`ACCESSOR` Functors) for the requirements on `accessor`.
    template <class TYPE, class ACCESSOR>
    int accessArray(const TYPE& object, ACCESSOR& accessor);

    /// Return the result of invoking the specified `accessor` with the
    /// specified `object` (1st argument) and a `bdlat_TypeCategory::Choice`
    /// tag object (2nd argument).  If `object` is not a `bdlat` "choice"
    /// type, pass a `bslmf::Nil` tag object as the second argument.  See
    /// [](#`ACCESSOR` Functors) for the requirements on `accessor`.
    template <class TYPE, class ACCESSOR>
    int accessChoice(const TYPE& object, ACCESSOR& accessor);

    /// Return the result of invoking the specified `accessor` with the
    /// specified `object` (1st argument) and a
    /// `bdlat_TypeCategory::CustomizedType` tag object (2nd argument).  If
    /// `object` is not a `bdlat` "customized type" type, pass a
    /// `bslmf::Nil` tag object as the second argument.  See [](#`ACCESSOR`
    /// Functors) for the requirements on `accessor`.
    template <class TYPE, class ACCESSOR>
    int accessCustomizedType(const TYPE& object, ACCESSOR& accessor);

    /// Return the result of invoking the specified `accessor` with the
    /// specified `object` (1st argument) and a
    /// `bdlat_TypeCategory::Enumeration` tag object (2nd argument).  If
    /// `object` is not a `bdlat` "enumeration type" type, pass a
    /// `bslmf::Nil` tag object as the second argument.  See [](#`ACCESSOR`
    /// Functors) for the requirements on `accessor`.
    template <class TYPE, class ACCESSOR>
    int accessEnumeration(const TYPE& object, ACCESSOR& accessor);

    /// Return the result of invoking the specified `accessor` with the
    /// specified `object` (1st argument) and a
    /// `bdlat_TypeCategory::NullableValue` tag object (2nd argument).  If
    /// `object` is not a `bdlat` "nullable value" type, pass a `bslmf::Nil`
    /// tag object as the second argument.  See [](#`ACCESSOR` Functors) for
    /// the requirements on `accessor`.
    template <class TYPE, class ACCESSOR>
    int accessNullableValue(const TYPE& object, ACCESSOR& accessor);

    /// Return the result of invoking the specified `accessor` with the
    /// specified `object` (1st argument) and a
    /// `bdlat_TypeCategory::Sequence` tag object (2nd argument).  If
    /// `object` is not a `bdlat` "sequence" type, pass a `bslmf::Nil` tag
    /// object as the second argument.  See [](#`ACCESSOR` Functors) for the
    /// requirements on `accessor`.
    template <class TYPE, class ACCESSOR>
    int accessSequence(const TYPE& object, ACCESSOR& accessor);

    /// Return the result of invoking the specified `accessor` with the
    /// specified `object` (1st argument) and a `bdlat_TypeCategory::Simple`
    /// tag object (2nd argument).  If `object` is not a `bdlat` "simple"
    /// type, pass a `bslmf::Nil` tag object as the second argument.  See
    /// [](#`ACCESSOR` Functors) for the requirements on `accessor`.
    template <class TYPE, class ACCESSOR>
    int accessSimple(const TYPE& object, ACCESSOR& accessor);

    /// Return the *runtime* type category for the specified `object`.  The
    /// value returned is never `bdlat_TypeCategory::e_DYNAMIC_CATEGORY` (as
    /// "dynamic" is a compile-time distinction).  Note that the compile
    /// time analog to this function (template) is
    /// `bdlat_TypeCategory::Select`, a meta-function that defines a type
    /// corresponding to the category tag class of `TYPE`.
    template <class TYPE>
    bdlat_TypeCategory::Value select(const TYPE& object);

    // OVERLOADABLE FUNCTIONS

    // The following functions should be overloaded for other types (in their
    // respective namespaces).  The following functions are the default
    // implementations (for `bas_codegen.pl`-generated types).  Do *not* call
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
    typedef typename bsl::conditional<
        bdlat_ArrayFunctions::IsArray<TYPE>::value,
        bdlat_TypeCategory::Array,
        bslmf::Nil>::type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateChoice(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename bsl::conditional<
        bdlat_ChoiceFunctions::IsChoice<TYPE>::value,
        bdlat_TypeCategory::Choice,
        bslmf::Nil>::type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateCustomizedType(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename bsl::conditional<
        bdlat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::value,
        bdlat_TypeCategory::CustomizedType,
        bslmf::Nil>::type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateEnumeration(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename bsl::conditional<
        bdlat_EnumFunctions::IsEnumeration<TYPE>::value,
        bdlat_TypeCategory::Enumeration,
        bslmf::Nil>::type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateNullableValue(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename bsl::conditional<
        bdlat_NullableValueFunctions::IsNullableValue<TYPE>::value,
        bdlat_TypeCategory::NullableValue,
        bslmf::Nil>::type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateSequence(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename bsl::conditional<
        bdlat_SequenceFunctions::IsSequence<TYPE>::value,
        bdlat_TypeCategory::Sequence,
        bslmf::Nil>::type Tag;
    return manipulator(object, Tag());
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryManipulateSimple(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    typedef typename bsl::conditional<
                          (int)bdlat_TypeCategory::Select<TYPE>::e_SELECTION ==
                                    (int)bdlat_TypeCategory::e_SIMPLE_CATEGORY,
                          bdlat_TypeCategory::Simple,
                          bslmf::Nil>::type Tag;
    return manipulator(object, Tag());
}

// ACCESSORS

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessArray(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename bsl::conditional<
        bdlat_ArrayFunctions::IsArray<TYPE>::value,
        bdlat_TypeCategory::Array,
        bslmf::Nil>::type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessChoice(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename bsl::conditional<
        bdlat_ChoiceFunctions::IsChoice<TYPE>::value,
        bdlat_TypeCategory::Choice,
        bslmf::Nil>::type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessCustomizedType(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename bsl::conditional<
        bdlat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::value,
        bdlat_TypeCategory::CustomizedType,
        bslmf::Nil>::type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessEnumeration(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename bsl::conditional<
        bdlat_EnumFunctions::IsEnumeration<TYPE>::value,
        bdlat_TypeCategory::Enumeration,
        bslmf::Nil>::type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessNullableValue(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename bsl::conditional<
        bdlat_NullableValueFunctions::IsNullableValue<TYPE>::value,
        bdlat_TypeCategory::NullableValue,
        bslmf::Nil>::type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessSequence(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename bsl::conditional<
        bdlat_SequenceFunctions::IsSequence<TYPE>::value,
        bdlat_TypeCategory::Sequence,
        bslmf::Nil>::type Tag;
    return accessor(object, Tag());
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_TypeCategoryFunctions::bdlat_typeCategoryAccessSimple(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    typedef typename bsl::conditional<
                          (int)bdlat_TypeCategory::Select<TYPE>::e_SELECTION ==
                                    (int)bdlat_TypeCategory::e_SIMPLE_CATEGORY,
                          bdlat_TypeCategory::Simple,
                          bslmf::Nil>::type Tag;
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
