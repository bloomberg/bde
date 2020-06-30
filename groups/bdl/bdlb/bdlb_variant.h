// bdlb_variant.h                                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLB_VARIANT
#define INCLUDED_BDLB_VARIANT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a variant (discriminated 'union'-like) type.
//
//@CLASSES:
//  bdlb::Variant:    variant of up to   20 types
//  bdlb::Variant2:   variant of exactly  2 types
//  bdlb::Variant3:   variant of exactly  3 types
//  bdlb::Variant4:   variant of exactly  4 types
//  bdlb::Variant5:   variant of exactly  5 types
//  bdlb::Variant6:   variant of exactly  6 types
//  bdlb::Variant7:   variant of exactly  7 types
//  bdlb::Variant8:   variant of exactly  8 types
//  bdlb::Variant9:   variant of exactly  9 types
//  bdlb::Variant10:  variant of exactly 10 types
//  bdlb::Variant11:  variant of exactly 11 types
//  bdlb::Variant12:  variant of exactly 12 types
//  bdlb::Variant13:  variant of exactly 13 types
//  bdlb::Variant14:  variant of exactly 14 types
//  bdlb::Variant15:  variant of exactly 15 types
//  bdlb::Variant16:  variant of exactly 16 types
//  bdlb::Variant17:  variant of exactly 17 types
//  bdlb::Variant18:  variant of exactly 18 types
//  bdlb::Variant19:  variant of exactly 19 types
//  bdlb::VariantImp: variant from a type list
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides: (1) a variant class template,
// 'bdlb::Variant', that can store an instance of one of up to some
// (implementation-defined) number of parameter types (currently 20), (2)
// several variant class templates that accommodate a *fixed* number (from 2
// to 19) of types, 'bdlb::Variant2', 'bdlb::Variant3', 'bdlb::Variant4',
// 'bdlb::Variant5', 'bdlb::Variant6', 'bdlb::Variant7', 'bdlb::Variant8',
// 'bdlb::Variant9', 'bdlb::Variant10', 'bdlb::Variant11', 'bdlb::Variant12',
// 'bdlb::Variant13', 'bdlb::Variant14', 'bdlb::Variant15', 'bdlb::Variant16',
// 'bdlb::Variant17', 'bdlb::Variant18', and 'bdlb::Variant19', and (3) a final
// variant class template, 'bdlb::VariantImp', whose supported types are
// specified via a 'bslmf::TypeList'.  A variant (of any of the aforementioned
// classes) can hold any one of the types defined in its signature at any point
// in time.  Clients can retrieve the value and type that a variant currently
// holds, assign a new value to the variant, or apply a visitor to the variant.
// A visitor's action is based on the value and type the variant currently
// holds.  Assigning a value of a new type destroys the object of the old type
// and constructs the new value by copy constructing the supplied value.
//
// When the number ('N') of types that needs to be supported is known, it is
// better to use the 'bdlb::VariantN' templates that use an identical
// implementation, but generate shorter symbols and debugging information due
// to the lack of defaulted template argument types.  Note that
// 'bdlb::VariantN<T1, ..., TN>' and 'bdlb::Variant<T1, ..., TN>' are,
// nevertheless, distinct types.
//
// When the variant types are (directly) supplied as a type list (of type
// 'bslmf::TypeList'), the type 'bdlb::VariantImp<TYPELIST>' can be used in
// place of:
//..
//  bdlb::Variant<typename TYPELIST::Type1, typename TYPELIST::Type2, ...>
//..
//
// Lastly, move constructors (taking an optional allocator) and move-assignment
// operators are also provided.  Note that move semantics are emulated with
// C++03 compilers.
//
///Default Construction
///--------------------
// The 'bdlb::Variant' class, when default constructed, does not hold a value
// and 'isUnset' returns 'true'.  This state is the same as that of a
// 'bdlb::Variant' that is reset by the 'reset' method.
//
///Visitors
///--------
// 'bdlb::Variant' provides an 'apply' method that implements the visitor
// design pattern.  'apply' accepts a visitor (functor) that provides an
// 'operator()' that is invoked with the value that the variant currently
// holds.
//
// Note, that visitor must satisfy the following requirements:
//: o The visitor's 'operator()' must be callable with any of the types that
//:   might be contained in the variant.
//: o For the 'apply' methods (but not 'applyRaw') the visitor's 'operator()'
//:   must be callable with an argument of type 'bslmf::Nil'.
//: o For the 'apply' and 'applyRaw' methods returning non-void type the return
//:   value of all callable overloads of 'operator()' must be convertible to
//:   this type.
//
// The 'apply' method should be preferred over a 'switch' statement based on
// the type index of a variant.  If the order or types contained by the variant
// is changed in the future, every place where the type index is hard-coded
// needs to be updated.  Whereas if 'apply' were used, no change would be
// needed because function overloading will automatically resolve to the proper
// 'operator()' to invoke.
//
// There are several variations of the 'apply' method, varying based on the
// return type and the handling of unset variants.  Firstly,
// the method varies based on whether 'apply' returns a value or not.
// There can either be:
//: o No return value.
//: o A return type specified in the visitor interface.
//: o A return type specified explicitly when invoking 'apply'.
//
// The default is no return value. Even if visitor's 'operator()' returns any
// non-void value, it will not be passed to the user.  If users would like to
// return a value from the visitor's 'operator()', they can specify a public
// alias 'ResultType' to the desired return type in the functor class.  For
// example, if 'operator()' were to return an 'int', the functor class should
// specify:
//..
//  typedef int ResultType;
//..
// If 'ResultType' cannot be determined, users also have the option of
// explicitly specifying the return type when invoking apply:
//..
//  apply<int>(visitor);
//..
// Secondly, the 'apply' method varies based on how the method handles an unset
// variant.  A user can choose to:
//: o Pass a default-constructed 'bslmf::Nil' to the visitor.
//: o Pass a user-specified "default" value to the visitor.
//
// Furthermore, if the user is sure that the variant cannot be unset, the user
// can invoke 'applyRaw', which is slightly more efficient.  However, if the
// variant is, in fact, unset, the behavior of 'applyRaw' is undefined.
//
///BDEX Streamability
///------------------
// BDEX streaming is not implemented for any of the variant classes.
//
///Class Synopsis
///--------------
// Due to the complexity of the implementation, the following synopsis is
// provided to aid users in locating documentation for functions.  Note that
// this is not a complete summary of all available methods; only the key
// methods are shown.  For more information, refer to the function-level
// documentation.
//
///Creators
/// - - - -
//..
//  bdlb::Variant()
//  bdlb::Variant(const TYPE_OR_ALLOCATOR& valueOrAllocator);
//  bdlb::Variant(const TYPE& value, bslma::Allocator *basicAllocator);
//..
// Create a variant.  Users can choose to initialize a variant with a specified
// value, or leave the variant in the unset state (via default construction).
// Users can also supply a 'bslma::Allocator *' for memory allocation.
//
///Manipulators
/// - - - - - -
//..
//  bdlb::Variant& operator=(const TYPE& value);
//..
// Assign a different value of template parameter 'TYPE' to the variant.
//..
//  bdlb::Variant& operator=(const bdlb::Variant& rhs);
//..
// Assign another variant to a variant.
//..
//  void                apply(VISITOR& visitor);
//  VISITOR::ResultType apply(VISITOR& visitor);
//  RET_TYPE            apply(VISITOR& visitor);
//..
// Access a variant's value using a specified visitor functor whereby
// 'bslmf::Nil' is passed to the visitor's 'operator()' if the variant is
// unset.
//..
//  void                apply(VISITOR& visitor, const TYPE& defaultValue);
//  VISITOR::ResultType apply(VISITOR& visitor, const TYPE& defaultValue);
//  RET_TYPE            apply(VISITOR& visitor, const TYPE& defaultValue);
//..
// Access a variant's value using a specified visitor functor whereby a
// user-specified default value is passed to the visitor's 'operator()' if the
// variant is unset.
//..
//  void                applyRaw(VISITOR& visitor);
//  VISITOR::ResultType applyRaw(VISITOR& visitor);
//  RET_TYPE            applyRaw(VISITOR& visitor);
//..
// Access a variant's value using a specified visitor functor whereby the
// behavior is undefined if the variant is unset.
//..
//  template <class TYPE>
//  TYPE& createInPlace();
//  TYPE& createInPlace(const A1& a1);
//  // ...
//  TYPE& createInPlace(const A1& a1, const A2& a2, ..., const A14& a14);
//..
// Create a new value of template parameter 'TYPE' in-place, with up to 14
// constructor arguments.
//..
//  void reset();
//..
// Reset a variant to the unset state.
//..
//  template <class TYPE>
//  TYPE& the();
//..
// Access the value of template parameter 'TYPE' currently held by a variant.
// This method should be invoked using the syntax 'the<TYPE>()', e.g.,
// 'the<int>()'.
//
///Accessors
///- - - - -
//..
//  template <class TYPE>
//  bool is() const;
//..
// Check whether a variant is currently holding a particular type.  This
// method should be invoked using the syntax 'is<TYPE>()', e.g., 'is<int>()'.
//..
//  bool isUnset() const;
//..
// Return 'true' if a variant is currently unset, and 'false' otherwise.
//..
//  bsl::ostream& print(bsl::ostream& stream,
//                      int           level          = 0,
//                      int           spacesPerLevel = 4) const;
//..
// Write a description of a variant to a specified 'stream'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Variant Construction
///- - - - - - - - - - - - - - - -
// The following example illustrates the different ways of constructing a
// 'bdlb::Variant':
//..
//  typedef bdlb::Variant <int, double, bsl::string> List;
//  typedef bdlb::Variant3<int, double, bsl::string> List3;  // equivalent
//..
// The contained types can be retrieved as a 'bslmf::TypeList' (using the
// 'TypeList' nested type), or individually (using 'TypeN', for 'N' varying
// from 1 to the length of the 'TypeList').  In the example below, we use the
// 'List' variant, but this could be substituted with 'List3' with no change
// to the code:
//..
//  assert(3 == List::TypeList::LENGTH);
//  assert(3 == List3::TypeList::LENGTH);
//..
// We can check that the variant defaults to the unset state by using the
// 'is<TYPE>' and 'typeIndex' methods:
//..
//  List x;
//
//  assert(!x.is<int>());
//  assert(!x.is<double>());
//  assert(!x.is<bsl::string>());
//  assert(0 == x.typeIndex());
//..
// Single-argument construction from a type in the 'TypeList' of a variant is
// also supported.  This is more efficient than creating an unset variant and
// assigning a value to it:
//..
//  List3 y(bsl::string("Hello"));
//
//  assert(!y.is<int>());
//  assert(!y.is<double>());
//  assert( y.is<bsl::string>());
//
//  assert("Hello" == y.the<bsl::string>());
//..
// Furthermore, 'createInPlace' is provided to support direct in-place
// construction.  This method allows users to directly construct the target
// type inside the variant, instead of first creating a temporary object, then
// copy constructing the object to initialize the variant:
//..
//  List z;
//  z.createInPlace<bsl::string>("Hello", 5);
//
//  assert(!z.is<int>());
//  assert(!z.is<double>());
//  assert( z.is<bsl::string>());
//
//  assert("Hello" == z.the<bsl::string>());
//..
// Up to 14 constructor arguments are supported for in-place construction of
// an object.  Users can also safely create another object of the same or
// different type in a variant that already holds a value using the
// 'createInPlace' method.  No memory is leaked in all cases and the destructor
// for the currently held object is invoked:
//..
//  z.createInPlace<bsl::string>("Hello", 5);
//  assert(z.is<bsl::string>());
//  assert("Hello" == z.the<bsl::string>());
//
//  z.createInPlace<double>(10.0);
//  assert(z.is<double>());
//  assert(10.0 == z.the<double>());
//
//  z.createInPlace<int>(10);
//  assert(z.is<int>());
//  assert(10 == z.the<int>());
//..
// 'createInPlace' returns a reference providing modifiable access to the
// created object:
//..
//  bsl::string& ref = z.createInPlace<bsl::string>("Goodbye");
//  assert("Goodbye" == z.the<bsl::string>());
//  assert("Goodbye" == ref);
//  assert(&ref == &z.the<bsl::string>());
//
//  ref = "Hello again!";
//  assert("Hello again!" == z.the<bsl::string>());
//..
//
///Example 2: Variant Assignment
///- - - - - - - - - - - - - - -
// A value of a given type can be stored in a variant in three different ways:
//
//: o 'operator='
//: o 'assignTo<TYPE>'
//: o 'assign'
//
// 'operator=' automatically deduces the type that the user is trying to assign
// to the variant.  This should be used most of the time.  The 'assignTo<TYPE>'
// method should be used when conversion to the type that the user is assigning
// to is necessary (see the first two examples below for more details).
// Finally, 'assign' is equivalent to 'operator=' and exists simply for
// backwards compatibility.
//
///'operator='
/// -  -  -  -
// The following example illustrates how to use 'operator=':
//..
//  typedef bdlb::Variant<int, double, bsl::string> List;
//
//  List x;
//
//  List::Type1 v1 = 1;       // 'int'
//  List::Type2 v2 = 2.0;     // 'double'
//  List::Type3 v3("hello");  // 'bsl::string'
//
//  x = v1;
//  assert( x.is<int>());
//  assert(!x.is<double>());
//  assert(!x.is<bsl::string>());
//  assert(v1 == x.the<int>());
//
//  x = v2;
//  assert(!x.is<int>());
//  assert( x.is<double>());
//  assert(!x.is<bsl::string>());
//  assert(v2 == x.the<double>());
//
//  x = v3;
//  assert(!x.is<int>());
//  assert(!x.is<double>());
//  assert( x.is<bsl::string>());
//  assert(v3 == x.the<bsl::string>());
//..
// Note that the type of the object is deduced automatically during assignment,
// as in:
//..
//  x = v1;
//..
// This automatic deduction, however, cannot be extended to conversion
// constructors, such as:
//..
//  x = static_cast<const char *>("Bye");  // ERROR
//..
// The compiler will diagnose that 'const char *' is not a variant type
// specified in the list of parameter types used in the definition of 'List',
// and will trigger a compile-time assertion.  To overcome this problem, see
// the next usage example of 'assignTo<TYPE>'.
//
///'assignTo<TYPE>'
///-  -  -  -  -  -
// In the previous example, 'const char *' was not part of the variant's type
// list, which resulted in a compilation diagnostic.  The use of
// 'assignTo<TYPE>' explicitly informs the compiler of the intended type to
// assign to the variant:
//..
//  x.assignTo<bsl::string>(static_cast<const char *>("Bye"));
//
//  assert(!x.is<int>());
//  assert(!x.is<double>());
//  assert( x.is<bsl::string>());
//
//  assert("Bye" == x.the<bsl::string>());
//..
//
///'assign'
/// -  -  -
// Finally, for backwards compatibility, 'assign' can also be used in place of
// 'operator=' (but not 'assignTo'):
//..
//  x.assign<int>(v1);
//  assert( x.is<int>());
//  assert(!x.is<double>());
//  assert(!x.is<bsl::string>());
//  assert(v1 == x.the<int>());
//
//  x.assign<double>(v2);
//  assert(!x.is<int>());
//  assert( x.is<double>());
//  assert(!x.is<bsl::string>());
//  assert(v2 == x.the<double>());
//
//  x.assign<bsl::string>(v3);
//  assert(!x.is<int>());
//  assert(!x.is<double>());
//  assert( x.is<bsl::string>());
//  assert(v3 == x.the<bsl::string>());
//..
//
///Example 3: Visiting a Variant via 'apply'
///- - - - - - - - - - - - - - - - - - - - -
// As described in {Visitors} (above), there are different ways to invoke the
// 'apply' method.  The first two examples below illustrate the different ways
// to invoke 'apply' (with no return value) to control the behavior of visiting
// an unset variant:
//: o 'bslmf::Nil' is passed to the visitor.
//: o A user-specified default value is passed to the visitor.
//
// A third example illustrates use of 'applyRaw', the behavior of which is
// undefined if the variant is unset.  Two final examples illustrate different
// ways to specify the return value from 'apply':
//: o The return value is specified in the visitor.
//: o The return value is specified with the function call.
//
///'bslmf::Nil' Passed to Visitor
///-  -  -  -  -  -  -  -  -  - -
// A simple visitor that does not require any return value might be one that
// prints the value of the variant to 'stdout':
//..
//  class my_PrintVisitor {
//    public:
//      template <class TYPE>
//      void operator()(const TYPE& value) const
//      {
//          bsl::cout << value << bsl::endl;
//      }
//
//      void operator()(bslmf::Nil value) const
//      {
//          bsl::cout << "null" << bsl::endl;
//      }
//  };
//
//  typedef bdlb::Variant<int, double, bsl::string> List;
//
//  List x[4];
//
//  // Note that 'x[3]' is uninitialized.
//
//  x[0].assign(1);
//  x[1].assign(1.1);
//  x[2].assignTo<bsl::string>(static_cast<const char *>("Hello"));
//
//  my_PrintVisitor printVisitor;
//
//  for (int i = 0; i < 4; ++i) {
//      x[i].apply(printVisitor);
//  }
//..
// The above prints the following on 'stdout':
//..
//  1
//  1.1
//  Hello
//  null
//..
// Note that 'operator()' is overloaded with 'bslmf::Nil'.  A direct match has
// higher precedence than a template parameter match.  When the variant is
// unset (such as 'x[3]'), a 'bslmf::Nil' is passed to the visitor.
//
///User-Specified Default Value Passed to Visitor
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Instead of using 'bslmf::Nil', users can also specify a default value to
// pass to the visitor when the variant is currently unset.  Using the same
// 'my_PrintVisitor' class from the previous example:
//..
//  for (int i = 0; i < 4; ++i) {
//      x[i].apply(printVisitor, "Print this when unset");
//  }
//..
// Now, the above code prints the following on 'stdout':
//..
//  1
//  1.1
//  Hello
//  Print this when unset
//..
// This variation of 'apply' is useful since the user can provide a default
// value to the visitor without incurring the cost of initializing the variant
// itself.
//
///'applyRaw' Undefined If Variant Is Unset
///-  -  -  -  -  -  -  -  -  -  -  -  -  -
// If it is certain that a variant is not unset, then the 'applyRaw' method can
// be used instead of 'apply'.  'applyRaw' is slightly more efficient than
// 'apply', but the behavior of 'applyRaw' is undefined if the variant is
// unset.  In the following application of 'applyRaw', we purposely circumvent
// 'x[3]' from being visited because we know that it is unset:
//..
//  for (int i = 0; i < 3; ++i) {     // NOT 'i < 4' as above.
//      assert(!x[i].isUnset());
//      x[i].applyRaw(printVisitor);  // undefined behavior for 'x[3]'
//  }
//  assert(x[3].isUnset());
//..
//
///Return Value Specified in Visitor
///  -  -  -  -  -  -  -  -  -  -  -
// Users can also specify a return type that 'operator()' will return by
// specifying a 'typedef' with the name 'ResultType' in their functor class.
// This is necessary in order for the 'apply' method to know what type to
// return at compile time:
//..
//  class my_AddVisitor {
//    public:
//      typedef bool ResultType;
//
//      // Note that the return type of 'operator()' is 'ResultType'.
//
//      template <class TYPE>
//      ResultType operator()(TYPE& value) const
//          // Return 'true' when addition is performed successfully, and
//          // 'false' otherwise.
//      {
//          if (bsl::is_convertible<TYPE, double>::value) {
//
//              // Add certain values to the variant.  The details are elided
//              // as it is the return value that is the focus of this example.
//
//              return true;                                          // RETURN
//          }
//          return false;
//      }
//  };
//
//  typedef bdlb::Variant<int, double, bsl::string> List;
//
//  List x[3];
//
//  x[0].assign(1);
//  x[1].assign(1.1);
//  x[2].assignTo<bsl::string>(static_cast<const char *>("Hello"));
//
//  my_AddVisitor addVisitor;
//
//  bool ret[3];
//
//  for (int i = 0; i < 3; ++i) {
//      ret[i] = x[i].apply(addVisitor);
//      if (!ret[i]) {
//          bsl::cout << "Cannot add to types not convertible to 'double'."
//                    << bsl::endl;
//      }
//  }
//  assert(true  == ret[0]);
//  assert(true  == ret[1]);
//  assert(false == ret[2]);
//..
// The above prints the following on 'stdout':
//..
//  Cannot add to types not convertible to 'double'.
//..
// Note that if no 'typedef' is provided (as in the 'my_PrintVisitor' class),
// then the default return value is 'void'.
//
///Return Value Specified with Function Call
/// -  -  -  -  -  -  -  -  -  -  -  -  -  -
// There may be some cases when a visitor interface is not owned by a client
// (hence the client cannot add a 'typedef' to the visitor), or the visitor
// could not determine the return type at design time.  In these scenarios,
// users can explicitly specify the return type when invoking 'apply':
//..
//  class ThirdPartyVisitor {
//    public:
//      template <class TYPE>
//      bsl::string operator()(const TYPE& value) const;
//          // Return the name of the specified 'value' as a 'bsl::string'.
//          // Note that the implementation of this class is deliberately not
//          // shown since this class belongs to a third-party library.
//  };
//
//  typedef bdlb::Variant<int, double, bsl::string> List;
//
//  List x[3];
//
//  x[0].assign(1);
//  x[1].assign(1.1);
//  x[2].assignTo<bsl::string>(static_cast<const char *>("Hello"));
//
//  ThirdPartyVisitor visitor;
//
//  for (int i = 0; i < 3; ++i) {
//
//      // Note that the return type is explicitly specified.
//
//      bsl::string ret = x[i].apply<bsl::string>(visitor);
//      bsl::cout << ret << bsl::endl;
//  }
//..

#include <bdlscm_version.h>

#include <bdlb_printmethods.h>

#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_destructionutil.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_if.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_nil.h>
#include <bslmf_removeconst.h>
#include <bslmf_removereference.h>
#include <bslmf_typelist.h>

#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_versionfunctions.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_objectbuffer.h>

#include <bsl_algorithm.h>
#include <bsl_iosfwd.h>

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#include <bslalg_scalardestructionprimitives.h>
#include <bslalg_scalarprimitives.h>

#include <bsl_typeinfo.h>
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES) \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
#define BDLB_VARIANT_USING_VARIADIC_TEMPLATES
    // Note that this macro definition parallels that of a similar macro
    // defined in 'bslmf_typelist.h'.
#endif

namespace BloombergLP {
namespace bdlb {

struct Variant_CopyAssignVisitor;
struct Variant_CopyConstructVisitor;
struct Variant_DefaultConstructVisitor;
struct Variant_DestructorVisitor;
struct Variant_EqualityTestVisitor;
struct Variant_MoveAssignVisitor;
struct Variant_MoveConstructVisitor;
struct Variant_PrintVisitor;

template <class TYPES>
class VariantImp;

template <class TYPES, class TYPE>
struct Variant_TypeIndex;

                    // ================================
                    // struct Variant_ReturnValueHelper
                    // ================================

// These definitions are provided outside of 'Variant_ReturnValueHelper'
// because of a bug in the IBM xlC compiler.
typedef char                  Variant_ReturnValueHelper_YesType;
typedef struct { char a[2]; } Variant_ReturnValueHelper_NoType;

BSLMF_ASSERT(sizeof(Variant_ReturnValueHelper_YesType)
             != sizeof(Variant_ReturnValueHelper_NoType));

template <class VISITOR>
struct Variant_ReturnValueHelper {
    // This struct is a component-private meta-function.  Do *not* use.  This
    // meta-function checks whether the template parameter type 'VISITOR' has
    // the member 'ResultType' defined using "SFINAE" (Substitution Failure Is
    // Not An Error).

    template <class T>
    static Variant_ReturnValueHelper_YesType match(
               typename bsl::remove_reference<typename T::ResultType>::type *);
    template <class T>
    static Variant_ReturnValueHelper_NoType match(...);
        // Return 'YesType' if 'T::ResultType' exists, and 'NoType' otherwise.
        // Note that if 'T::ResultType' exists, then the first function is a
        // better match than the ellipsis version.

    enum {
        value =
         sizeof(match<VISITOR>(0)) == sizeof(Variant_ReturnValueHelper_YesType)

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE3.0
      , VALUE =
         sizeof(match<VISITOR>(0)) == sizeof(Variant_ReturnValueHelper_YesType)
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE3.0

    };
        // A 'value' of 'true' indicates 'VISITOR::ResultType' exists, and
        // 'false' otherwise.
};

                   // =====================================
                   // class VariantImp_AllocatorBase<TYPES>
                   // =====================================

template <class TYPES>
class VariantImp_AllocatorBase {
    // This class is component-private.  Do not use.  This class contains the
    // 'typedef's and data members of the 'Variant' class, and serves as the
    // base class for the variant when any one of the types held by the variant
    // has the 'bslma::UsesBslmaAllocator' type trait.

  public:
    // TYPES
    typedef TYPES TypeList;
        // 'TypeList' is an alias for the 'bslmf::TypeList' type serving as the
        // template parameter to this variant implementation.

    typedef typename bslmf::TypeListTypeOf< 1, TYPES>::TypeOrDefault Type1;
    typedef typename bslmf::TypeListTypeOf< 2, TYPES>::TypeOrDefault Type2;
    typedef typename bslmf::TypeListTypeOf< 3, TYPES>::TypeOrDefault Type3;
    typedef typename bslmf::TypeListTypeOf< 4, TYPES>::TypeOrDefault Type4;
    typedef typename bslmf::TypeListTypeOf< 5, TYPES>::TypeOrDefault Type5;
    typedef typename bslmf::TypeListTypeOf< 6, TYPES>::TypeOrDefault Type6;
    typedef typename bslmf::TypeListTypeOf< 7, TYPES>::TypeOrDefault Type7;
    typedef typename bslmf::TypeListTypeOf< 8, TYPES>::TypeOrDefault Type8;
    typedef typename bslmf::TypeListTypeOf< 9, TYPES>::TypeOrDefault Type9;
    typedef typename bslmf::TypeListTypeOf<10, TYPES>::TypeOrDefault Type10;
    typedef typename bslmf::TypeListTypeOf<11, TYPES>::TypeOrDefault Type11;
    typedef typename bslmf::TypeListTypeOf<12, TYPES>::TypeOrDefault Type12;
    typedef typename bslmf::TypeListTypeOf<13, TYPES>::TypeOrDefault Type13;
    typedef typename bslmf::TypeListTypeOf<14, TYPES>::TypeOrDefault Type14;
    typedef typename bslmf::TypeListTypeOf<15, TYPES>::TypeOrDefault Type15;
    typedef typename bslmf::TypeListTypeOf<16, TYPES>::TypeOrDefault Type16;
    typedef typename bslmf::TypeListTypeOf<17, TYPES>::TypeOrDefault Type17;
    typedef typename bslmf::TypeListTypeOf<18, TYPES>::TypeOrDefault Type18;
    typedef typename bslmf::TypeListTypeOf<19, TYPES>::TypeOrDefault Type19;
    typedef typename bslmf::TypeListTypeOf<20, TYPES>::TypeOrDefault Type20;
        // 'TypeN' is an alias for the 'N'th type in the 'TypeList' of this
        // variant implementation.  If less than 'N' template arguments were
        // given to the 'Variant' type, then 'TypeN' is 'bslmf::Nil'.

  private:
    union Value {
        bsls::ObjectBuffer<Type1>  d_v1;
        bsls::ObjectBuffer<Type2>  d_v2;
        bsls::ObjectBuffer<Type3>  d_v3;
        bsls::ObjectBuffer<Type4>  d_v4;
        bsls::ObjectBuffer<Type5>  d_v5;
        bsls::ObjectBuffer<Type6>  d_v6;
        bsls::ObjectBuffer<Type7>  d_v7;
        bsls::ObjectBuffer<Type8>  d_v8;
        bsls::ObjectBuffer<Type9>  d_v9;
        bsls::ObjectBuffer<Type10> d_v10;
        bsls::ObjectBuffer<Type11> d_v11;
        bsls::ObjectBuffer<Type12> d_v12;
        bsls::ObjectBuffer<Type13> d_v13;
        bsls::ObjectBuffer<Type14> d_v14;
        bsls::ObjectBuffer<Type15> d_v15;
        bsls::ObjectBuffer<Type16> d_v16;
        bsls::ObjectBuffer<Type17> d_v17;
        bsls::ObjectBuffer<Type18> d_v18;
        bsls::ObjectBuffer<Type19> d_v19;
        bsls::ObjectBuffer<Type20> d_v20;
    };
        // 'Value' is a union of 'bsls::ObjectBuffer' of all types contained by
        // the variant.  'bsls::ObjectBuffer' is used to: 1) wrap non-POD types
        // within the union, and 2) ensure proper alignment of the types.

    // DATA
    Value             d_value;        // value of the object, initialized by
                                      // derived class

    int               d_type;         // current type the variant is holding (0
                                      // if unset)

    bslma::Allocator *d_allocator_p;  // pointer to allocator (held, not owned)

    // FRIENDS
    template <class VARIANT_TYPES>
    friend class VariantImp;

    template <class VARIANT_TYPES>
    friend bool operator==(const VariantImp<VARIANT_TYPES>&,
                           const VariantImp<VARIANT_TYPES>&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(VariantImp_AllocatorBase,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    VariantImp_AllocatorBase(int type, bslma::Allocator *basicAllocator);

    VariantImp_AllocatorBase(int               type,
                             bslma::Allocator *basicAllocator,
                             bsl::true_type);
        // Create a 'VariantImp_AllocatorBase' with the specified 'type'
        // indicating the type of the object that the variant will initially
        // hold, and the specified 'basicAllocator' to supply memory.

    template <class TYPE>
    VariantImp_AllocatorBase(int type, const TYPE&, bsl::false_type);

    // ACCESSORS
    bslma::Allocator *getAllocator() const;
        // Return the allocator used by this object to supply memory.
};

                  // =======================================
                  // class VariantImp_NoAllocatorBase<TYPES>
                  // =======================================

template <class TYPES>
class VariantImp_NoAllocatorBase {
    // This class is component-private.  Do not use.  This class contains the
    // 'typedef's and data members of the 'Variant' class, and serves as the
    // base class for the variant when none of the types held by the variant
    // has the 'bslma::UsesBslmaAllocator' type trait.  The goal is to optimize
    // the size of the variant to avoid holding an unnecessary allocator
    // pointer.

  public:
    // TYPES
    typedef TYPES TypeList;
        // 'TypeList' is an alias for the 'bslmf::TypeList' type serving as the
        // template parameter to this variant implementation.

    typedef typename bslmf::TypeListTypeOf< 1, TYPES>::TypeOrDefault Type1;
    typedef typename bslmf::TypeListTypeOf< 2, TYPES>::TypeOrDefault Type2;
    typedef typename bslmf::TypeListTypeOf< 3, TYPES>::TypeOrDefault Type3;
    typedef typename bslmf::TypeListTypeOf< 4, TYPES>::TypeOrDefault Type4;
    typedef typename bslmf::TypeListTypeOf< 5, TYPES>::TypeOrDefault Type5;
    typedef typename bslmf::TypeListTypeOf< 6, TYPES>::TypeOrDefault Type6;
    typedef typename bslmf::TypeListTypeOf< 7, TYPES>::TypeOrDefault Type7;
    typedef typename bslmf::TypeListTypeOf< 8, TYPES>::TypeOrDefault Type8;
    typedef typename bslmf::TypeListTypeOf< 9, TYPES>::TypeOrDefault Type9;
    typedef typename bslmf::TypeListTypeOf<10, TYPES>::TypeOrDefault Type10;
    typedef typename bslmf::TypeListTypeOf<11, TYPES>::TypeOrDefault Type11;
    typedef typename bslmf::TypeListTypeOf<12, TYPES>::TypeOrDefault Type12;
    typedef typename bslmf::TypeListTypeOf<13, TYPES>::TypeOrDefault Type13;
    typedef typename bslmf::TypeListTypeOf<14, TYPES>::TypeOrDefault Type14;
    typedef typename bslmf::TypeListTypeOf<15, TYPES>::TypeOrDefault Type15;
    typedef typename bslmf::TypeListTypeOf<16, TYPES>::TypeOrDefault Type16;
    typedef typename bslmf::TypeListTypeOf<17, TYPES>::TypeOrDefault Type17;
    typedef typename bslmf::TypeListTypeOf<18, TYPES>::TypeOrDefault Type18;
    typedef typename bslmf::TypeListTypeOf<19, TYPES>::TypeOrDefault Type19;
    typedef typename bslmf::TypeListTypeOf<20, TYPES>::TypeOrDefault Type20;
        // 'TypeN' is an alias for the 'N'th type in the 'TypeList' of this
        // variant implementation.  If less than 'N' template arguments were
        // given to the 'Variant' type, then 'TypeN' is 'bslmf::Nil'.

  private:
    union Value {
        bsls::ObjectBuffer<Type1>  d_v1;
        bsls::ObjectBuffer<Type2>  d_v2;
        bsls::ObjectBuffer<Type3>  d_v3;
        bsls::ObjectBuffer<Type4>  d_v4;
        bsls::ObjectBuffer<Type5>  d_v5;
        bsls::ObjectBuffer<Type6>  d_v6;
        bsls::ObjectBuffer<Type7>  d_v7;
        bsls::ObjectBuffer<Type8>  d_v8;
        bsls::ObjectBuffer<Type9>  d_v9;
        bsls::ObjectBuffer<Type10> d_v10;
        bsls::ObjectBuffer<Type11> d_v11;
        bsls::ObjectBuffer<Type12> d_v12;
        bsls::ObjectBuffer<Type13> d_v13;
        bsls::ObjectBuffer<Type14> d_v14;
        bsls::ObjectBuffer<Type15> d_v15;
        bsls::ObjectBuffer<Type16> d_v16;
        bsls::ObjectBuffer<Type17> d_v17;
        bsls::ObjectBuffer<Type18> d_v18;
        bsls::ObjectBuffer<Type19> d_v19;
        bsls::ObjectBuffer<Type20> d_v20;
    };
        // 'Value' is a union of 'bsls::ObjectBuffer' of all types contained by
        // the variant.  'bsls::ObjectBuffer' is used to: 1) wrap non-POD types
        // within the union, and 2) ensure proper alignment of the types.

    // DATA
    Value d_value;  // value of the object, initialized by derived class
    int   d_type;   // current type the variant is holding (0 if unset)

    // FRIENDS
    template <class VARIANT_TYPES>
    friend class VariantImp;

    template <class VARIANT_TYPES>
    friend bool operator==(const VariantImp<VARIANT_TYPES>&,
                           const VariantImp<VARIANT_TYPES>&);

  public:
    // CREATORS
    VariantImp_NoAllocatorBase(int type, bslma::Allocator *);

    VariantImp_NoAllocatorBase(int type, bslma::Allocator *, bsl::true_type);
        // Create a 'VariantImp_NoAllocatorBase' with the specified 'type'
        // indicating the type of the object that the variant will initially
        // hold.

    template <class TYPE>
    VariantImp_NoAllocatorBase(int type, const TYPE&, bsl::false_type);

    // ACCESSORS
    bslma::Allocator *getAllocator() const;
        // Return 0.  Note that this object does not hold an allocator pointer.
};

                       // ========================
                       // struct VariantImp_Traits
                       // ========================

template <class TYPES>
struct VariantImp_Traits {
    // This struct is component-private.  Do not use.  This meta-function
    // selects 'VariantImp_AllocatorBase' as a base class type if any one of
    // the types held by a variant has the 'bslma::UsesBslmaAllocator' trait,
    // and 'VariantImp_NoAllocatorBase' otherwise.

    // TYPES
    typedef typename bslmf::TypeListTypeOf< 1, TYPES>::TypeOrDefault Type1;
    typedef typename bslmf::TypeListTypeOf< 2, TYPES>::TypeOrDefault Type2;
    typedef typename bslmf::TypeListTypeOf< 3, TYPES>::TypeOrDefault Type3;
    typedef typename bslmf::TypeListTypeOf< 4, TYPES>::TypeOrDefault Type4;
    typedef typename bslmf::TypeListTypeOf< 5, TYPES>::TypeOrDefault Type5;
    typedef typename bslmf::TypeListTypeOf< 6, TYPES>::TypeOrDefault Type6;
    typedef typename bslmf::TypeListTypeOf< 7, TYPES>::TypeOrDefault Type7;
    typedef typename bslmf::TypeListTypeOf< 8, TYPES>::TypeOrDefault Type8;
    typedef typename bslmf::TypeListTypeOf< 9, TYPES>::TypeOrDefault Type9;
    typedef typename bslmf::TypeListTypeOf<10, TYPES>::TypeOrDefault Type10;
    typedef typename bslmf::TypeListTypeOf<11, TYPES>::TypeOrDefault Type11;
    typedef typename bslmf::TypeListTypeOf<12, TYPES>::TypeOrDefault Type12;
    typedef typename bslmf::TypeListTypeOf<13, TYPES>::TypeOrDefault Type13;
    typedef typename bslmf::TypeListTypeOf<14, TYPES>::TypeOrDefault Type14;
    typedef typename bslmf::TypeListTypeOf<15, TYPES>::TypeOrDefault Type15;
    typedef typename bslmf::TypeListTypeOf<16, TYPES>::TypeOrDefault Type16;
    typedef typename bslmf::TypeListTypeOf<17, TYPES>::TypeOrDefault Type17;
    typedef typename bslmf::TypeListTypeOf<18, TYPES>::TypeOrDefault Type18;
    typedef typename bslmf::TypeListTypeOf<19, TYPES>::TypeOrDefault Type19;
    typedef typename bslmf::TypeListTypeOf<20, TYPES>::TypeOrDefault Type20;

    enum {
        k_VARIANT_USES_BSLMA_ALLOCATOR = (
            bslma::UsesBslmaAllocator< Type1>::value
         || bslma::UsesBslmaAllocator< Type2>::value
         || bslma::UsesBslmaAllocator< Type3>::value
         || bslma::UsesBslmaAllocator< Type4>::value
         || bslma::UsesBslmaAllocator< Type5>::value
         || bslma::UsesBslmaAllocator< Type6>::value
         || bslma::UsesBslmaAllocator< Type7>::value
         || bslma::UsesBslmaAllocator< Type8>::value
         || bslma::UsesBslmaAllocator< Type9>::value
         || bslma::UsesBslmaAllocator<Type10>::value
         || bslma::UsesBslmaAllocator<Type11>::value
         || bslma::UsesBslmaAllocator<Type12>::value
         || bslma::UsesBslmaAllocator<Type13>::value
         || bslma::UsesBslmaAllocator<Type14>::value
         || bslma::UsesBslmaAllocator<Type15>::value
         || bslma::UsesBslmaAllocator<Type16>::value
         || bslma::UsesBslmaAllocator<Type17>::value
         || bslma::UsesBslmaAllocator<Type18>::value
         || bslma::UsesBslmaAllocator<Type19>::value
         || bslma::UsesBslmaAllocator<Type20>::value),

        k_VARIANT_IS_BITWISE_COPYABLE = (
            bsl::is_trivially_copyable< Type1>::value
         && bsl::is_trivially_copyable< Type2>::value
         && bsl::is_trivially_copyable< Type3>::value
         && bsl::is_trivially_copyable< Type4>::value
         && bsl::is_trivially_copyable< Type5>::value
         && bsl::is_trivially_copyable< Type6>::value
         && bsl::is_trivially_copyable< Type7>::value
         && bsl::is_trivially_copyable< Type8>::value
         && bsl::is_trivially_copyable< Type9>::value
         && bsl::is_trivially_copyable<Type10>::value
         && bsl::is_trivially_copyable<Type11>::value
         && bsl::is_trivially_copyable<Type12>::value
         && bsl::is_trivially_copyable<Type13>::value
         && bsl::is_trivially_copyable<Type14>::value
         && bsl::is_trivially_copyable<Type15>::value
         && bsl::is_trivially_copyable<Type16>::value
         && bsl::is_trivially_copyable<Type17>::value
         && bsl::is_trivially_copyable<Type18>::value
         && bsl::is_trivially_copyable<Type19>::value
         && bsl::is_trivially_copyable<Type20>::value),

        k_VARIANT_IS_BITWISE_MOVEABLE = (
            bslmf::IsBitwiseMoveable< Type1>::value
         && bslmf::IsBitwiseMoveable< Type2>::value
         && bslmf::IsBitwiseMoveable< Type3>::value
         && bslmf::IsBitwiseMoveable< Type4>::value
         && bslmf::IsBitwiseMoveable< Type5>::value
         && bslmf::IsBitwiseMoveable< Type6>::value
         && bslmf::IsBitwiseMoveable< Type7>::value
         && bslmf::IsBitwiseMoveable< Type8>::value
         && bslmf::IsBitwiseMoveable< Type9>::value
         && bslmf::IsBitwiseMoveable<Type10>::value
         && bslmf::IsBitwiseMoveable<Type11>::value
         && bslmf::IsBitwiseMoveable<Type12>::value
         && bslmf::IsBitwiseMoveable<Type13>::value
         && bslmf::IsBitwiseMoveable<Type14>::value
         && bslmf::IsBitwiseMoveable<Type15>::value
         && bslmf::IsBitwiseMoveable<Type16>::value
         && bslmf::IsBitwiseMoveable<Type17>::value
         && bslmf::IsBitwiseMoveable<Type18>::value
         && bslmf::IsBitwiseMoveable<Type19>::value
         && bslmf::IsBitwiseMoveable<Type20>::value)
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , VARIANT_USES_BSLMA_ALLOCATOR = k_VARIANT_USES_BSLMA_ALLOCATOR
      , VARIANT_IS_BITWISE_COPYABLE  = k_VARIANT_IS_BITWISE_COPYABLE
      , VARIANT_IS_BITWISE_MOVEABLE  = k_VARIANT_IS_BITWISE_MOVEABLE
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    typedef typename bslmf::If<k_VARIANT_USES_BSLMA_ALLOCATOR,
                               VariantImp_AllocatorBase<TYPES>,
                               VariantImp_NoAllocatorBase<TYPES> >::Type
                                                                      BaseType;
        // Determines what the base type is.
};

                       // ===============================
                       // class Variant_ReturnAnyTypeUtil
                       // ===============================

template <class TYPE>
struct Variant_ReturnAnyTypeUtil {
    // This 'struct' provides a function that returns an (invalid) instance of
    // any type.  It is meant to allow clients to express:
    //..
    //  template <RESULT_TYPE>
    //  RESULT_TYPE foo()
    //  {
    //      // ...
    //
    //      // The following 'return' is unreachable, but is required for
    //      // compilation.
    //
    //      return Variant_ReturnAnyTypeUtil::doNotCall();
    //  }
    //..
    // where 'RESULT_TYPE' may be 'void'.  Note that while such a return
    // statement is not required by the C++ standard, the lack of such a
    // return statement causes a warning (or error) with many compilers.

    // CLASS METHODS
    static TYPE doNotCall(TYPE *dummy);
        // Return the specified '*dummy'.

    static TYPE doNotCall();
        // Return a 'TYPE' object.
};

template <>
struct Variant_ReturnAnyTypeUtil<void> {
    // This partial specialization of 'Variant_ReturnAnyTypeUtil' provides
    // functions that do not have return value.

    // CLASS METHODS
    static void doNotCall();
        // Do nothing.
};

template <class TYPE>
struct Variant_ReturnAnyTypeUtil<TYPE&> {
    // This partial specialization of 'Variant_ReturnAnyTypeUtil' provides a
    // function that returns an lvalue reference.

    // CLASS METHODS
    static TYPE& lvalueRef(TYPE *dummy);
        // Return an lvalue reference providing modifiable access to the
        // specified '*dummy'.

    static TYPE& doNotCall();
        // Return a 'TYPE&' object.
};

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class TYPE>
struct Variant_ReturnAnyTypeUtil<TYPE&&>
    // This partial specialization of 'Variant_ReturnAnyTypeUtil' provides a
    // function that returns an rvalue reference.
{
    // CLASS METHODS
    static TYPE&& rvalueRef(TYPE *dummy);
        // Return an rvalue reference providing modifiable access to the
        // specified '*dummy'.

    static TYPE&& doNotCall();
        // Return a 'TYPE&&' object.
};
#endif

                       // ==============================
                       // class Variant_RawVisitorHelper
                       // ==============================

template <class RESULT_TYPE, class VISITOR>
class Variant_RawVisitorHelper {
    // This 'struct' provides a helper for implementing 'Variant::applyRaw'
    // that enables 'applyRaw' to support visitor functors that do not provide
    // an overload for 'operator()(bslmf::Nil)'.  Objects of this type are
    // constructed using a functor of (template parameter) type 'VISITOR',
    // whose 'operator()' returns the (template parameter) type 'RESULT_TYPE'.
    // A 'Variant_RawVisitorHelper' wraps a functor of type 'VISITOR' and
    // provides an implementation of 'operator()(bslmf::Nil)' that performs a
    // 'BSLS_ASSERT_OPT(false)'.  Note that this overload is needed to enable
    // compilation (specifically, to instantiate 'doApply' and 'doApplyR'), but
    // is never invoked by any code path at runtime.

    // DATA
    VISITOR *d_visitor;  // visitor to which this helper delegates

  public:
    // CREATORS
    explicit
    Variant_RawVisitorHelper(VISITOR *visitor);
        // Create a 'RawVisitorHelper' functor that delegates to the specified
        // 'visitor'.

    // MANIPULATORS
    template <class ARGUMENT_TYPE>
    RESULT_TYPE operator()(ARGUMENT_TYPE& argument);
    template <class ARGUMENT_TYPE>
    RESULT_TYPE operator()(const ARGUMENT_TYPE& argument);
        // Invoke the functor supplied at construction with the specified
        // 'argument', and return the result.

    // ACCESSORS
    template <class ARGUMENT_TYPE>
    RESULT_TYPE operator()(ARGUMENT_TYPE& argument) const;
    template <class ARGUMENT_TYPE>
    RESULT_TYPE operator()(const ARGUMENT_TYPE& argument) const;
        // Invoke the functor supplied at construction with the specified
        // 'argument', and return the result.

    RESULT_TYPE operator()(bslmf::Nil) const;
        // Do not call.  The behavior of this method is undefined.
};

                       // =======================
                       // class VariantImp<TYPES>
                       // =======================

template <class TYPES>
class VariantImp : public VariantImp_Traits<TYPES>::BaseType {
    // This class provides the implementation of 'Variant' (except for the
    // creators) given a list of template parameter 'TYPES'.
    //
    // More generally, if each of the types in the list of 'TYPES' is
    // value semantic, then this class also supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, and 'ostream' printing.  A precise operational definition of
    // when two instances have the same value can be found in the description
    // of 'operator==' for the class.  This class is *exception* *neutral* with
    // no guarantee of rollback: if an exception is thrown during the
    // invocation of a method on a pre-existing instance, the object is left in
    // a valid state, but its value is undefined.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) is supported in all cases.
    //
    // If any of the types in the list of 'TYPES' does not support
    // 'operator==', or any of the value-semantic operations mentioned above,
    // then this variant also does not support that operation and attempts to
    // invoke it will trigger a compilation diagnostic.

    // PRIVATE TYPES
    typedef VariantImp_Traits<TYPES>  Traits;
    typedef typename Traits::BaseType Base;
    typedef bslmf::MovableRefUtil     MoveUtil;

    typedef VariantImp<TYPES>         SelfType;
        // 'SelfType' is an alias to this class.

  private:
    // PRIVATE MANIPULATORS
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor, bsl::false_type);
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor, bsl::true_type);
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor);
        // Invoke 'operator()' of the specified 'visitor' on the current value
        // (of template parameter 'TYPE') held by this variant.  'TYPE' must be
        // the same as one of the types that this variant can hold.  The
        // behavior is undefined unless this variant holds a value of template
        // parameter 'TYPE'.  Note that the second argument is for resolving
        // overloading ambiguity and is not used.

    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor, bsl::false_type);
    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor, bsl::true_type);
    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor);
        // Invoke 'operator()' of the specified 'visitor' on the current value
        // (of template parameter 'TYPE') held by this variant, and return the
        // value (of template parameter 'RET_TYPE') returned by the 'visitor'.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  The behavior is undefined unless this variant holds a value
        // of template parameter 'TYPE'.  Note that the second argument is for
        // resolving overloading ambiguity and is not used.

    template <class TYPE, class SOURCE_TYPE>
    void assignImp(const SOURCE_TYPE& value);
        // Assign to this variant the specified 'value' of template parameter
        // 'SOURCE_TYPE' converted to template parameter 'TYPE'.  'TYPE' must
        // be the same as one of the types that this variant can hold and
        // 'SOURCE_TYPE' must be convertible to 'TYPE'.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    void assignImp(TYPE&&                  value);
#else
    void assignImp(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this variant the specified 'value' of template parameter
        // 'TYPE'.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  'TYPE' must be the
        // same as one of the types that this variant can hold.

    template <class TYPE>
    void create(const TYPE& value, bsl::false_type);
        // Construct this variant object to initially hold the specified
        // 'value' of template parameter 'TYPE'.  'TYPE' must be the same as
        // one of the types that this variant can hold.  Note that the second
        // parameter is for resolving overloading ambiguity and is not used.

    void create(bslma::Allocator *, bsl::true_type);
        // Construct this variant object to be initially in the unset state.

    template <class VISITOR_REF>
    void doApply(VISITOR_REF visitor, int type);
        // Apply the specified 'visitor' on the current value held by this
        // variant by invoking 'applyImp' with the appropriate template
        // arguments, determined by the specified 'type'.  The behavior is
        // undefined unless 'type != 0'.

    template <class VISITOR_REF, class RET_TYPE>
    RET_TYPE doApplyR(VISITOR_REF visitor, int type);
        // Apply the specified 'visitor' on the current value held by this
        // variant by invoking 'applyImpR' with the appropriate template
        // arguments, determined by the specified 'type', and return the value
        // (of template parameter 'RET_TYPE') returned by the 'visitor'.  The
        // behavior is undefined unless 'type != 0'.

    // PRIVATE ACCESSORS
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor, bsl::false_type) const;
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor, bsl::true_type) const;
    template <class TYPE, class VISITOR_REF>
    void applyImp(VISITOR_REF visitor) const;
        // Invoke 'operator()' of the specified 'visitor' on the current value
        // (of template parameter 'TYPE') held by this variant.  'TYPE' must be
        // the same as one of the types that this variant can hold.  The
        // behavior is undefined unless this variant holds a value of template
        // parameter 'TYPE'.  Note that the second argument is for resolving
        // overloading ambiguity and is not used.

    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor, bsl::false_type) const;
    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor, bsl::true_type) const;
    template <class TYPE, class VISITOR_REF, class RET_TYPE>
    RET_TYPE applyImpR(VISITOR_REF visitor) const;
        // Invoke 'operator()' of the specified 'visitor' on the current value
        // (of template parameter 'TYPE') held by this variant, and return the
        // value (of template parameter 'RET_TYPE') returned by the 'visitor'.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  The behavior is undefined unless this variant holds a value
        // of template parameter 'TYPE'.  Note that the second argument is for
        // resolving overloading ambiguity and is not used.

    template <class VISITOR_REF>
    void doApply(VISITOR_REF visitor, int type) const;
        // Apply the specified 'visitor' on the current value held by this
        // variant by invoking 'applyImp' with the appropriate template
        // arguments, determined by the specified 'type'.  The behavior is
        // undefined unless 'type != 0'.

    template <class VISITOR_REF, class RET_TYPE>
    RET_TYPE doApplyR(VISITOR_REF visitor, int type) const;
        // Apply the specified 'visitor' on the current value held by this
        // variant by invoking 'applyImpR' with the appropriate template
        // arguments, determined by the specified 'type', and return the value
        // (of template parameter 'RET_TYPE') returned by the 'visitor'.  The
        // behavior is undefined unless 'type != 0'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(VariantImp,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(VariantImp,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(VariantImp,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(VariantImp, HasPrintMethod);

    // CREATORS
    VariantImp();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    VariantImp(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    VariantImp(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    VariantImp(TYPE&&                   value,
               typename bsl::enable_if<
                   !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                   &&
                   !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                           void>::type * = 0);
#else
    VariantImp(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    VariantImp(TYPE&&                   value,
               typename bsl::enable_if<
                   !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
               bslma::Allocator>::type *basicAllocator);
#else
    VariantImp(bslmf::MovableRef<TYPE>  value,
               bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    VariantImp(const VariantImp&  original,
               bslma::Allocator  *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    VariantImp(bslmf::MovableRef<VariantImp> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    VariantImp(bslmf::MovableRef<VariantImp>  original,
               bslma::Allocator              *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    ~VariantImp();
        // Destroy this variant object, invoking the destructor of the type of
        // object contained (if any) on the value of that type.

    // MANIPULATORS
    template <class TYPE>
    VariantImp& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    VariantImp&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    VariantImp& operator=(const VariantImp& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    VariantImp& operator=(bslmf::MovableRef<VariantImp> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    apply(VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to the 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'.  If
        // this variant is unset, a default constructed 'bslmf::Nil' is passed
        // to the 'visitor'.  Note that this method is selected only if the
        // template parameter type 'VISITOR' defines a 'typedef' of
        // 'ResultType' in its public interface.  Also note that this method is
        // defined inline to work around a Windows compiler bug with SFINAE
        // functions.

        if (this->d_type) {
            return doApplyR<VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
                                                                      // RETURN
        }

        bslmf::Nil nil = bslmf::Nil();
        return visitor(nil);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    apply(const VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to the 'visitor' object's
        // 'operator()', and return the value returned by the 'visitor'.  If
        // If this variant is unset, a default constructed 'bslmf::Nil' is
        // passed to the 'visitor'.  Note that this method is selected only if
        // the template parameter type 'VISITOR' defines a 'typedef' of
        // 'ResultType' in its public interface.  Also note that this method is
        // defined inline to work around a Windows compiler bug with SFINAE
        // functions.

        if (this->d_type) {
            return doApplyR<const VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
                                                                      // RETURN
        }

        bslmf::Nil nil = bslmf::Nil();
        return visitor(nil);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    apply(VISITOR& visitor, const TYPE& defaultValue) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value (of template parameter 'TYPE') this variant currently
        // holds to the 'visitor' object's 'operator()', and return the value
        // returned by the 'visitor'.  If this variant is unset, the specified
        // 'defaultValue' of template parameter 'TYPE' is passed to the
        // 'visitor'.  'TYPE' must be the same as one of the types that this
        // variant can hold.  The behavior is undefined unless this variant is
        // unset or holds a value of template parameter 'TYPE'.  Note that this
        // method is selected only if the template parameter type 'VISITOR'
        // defines a 'typedef' of 'ResultType' in its public interface.  Also
        // note that this method is defined inline to work around a Windows
        // compiler bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
                                                                      // RETURN
        }

        return visitor(defaultValue);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    apply(const VISITOR& visitor, const TYPE& defaultValue) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value (of template parameter 'TYPE') this variant currently
        // holds to the 'visitor' object's 'operator()', and return the value
        // returned by the 'visitor'.  If this variant is unset, the specified
        // 'defaultValue' of template parameter 'TYPE' is passed to the
        // 'visitor'.  'TYPE' must be the same as one of the types that this
        // variant can hold.  The behavior is undefined unless this variant is
        // unset or holds a value of template parameter 'TYPE'.  Note that this
        // method is selected only if the template parameter type 'VISITOR'
        // defines a 'typedef' of 'ResultType' in its public interface.  Also
        // note that this method is defined inline to work around a Windows
        // compiler bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<const VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
                                                                      // RETURN
        }

        return visitor(defaultValue);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    apply(VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to the 'visitor' object's
        // 'operator()'.  This method does not return a value.  If this variant
        // is unset, a default constructed 'bslmf::Nil' is passed to the
        // 'visitor'.  Note that this method is selected only if the template
        // parameter type 'VISITOR' does not define a 'typedef' of 'ResultType'
        // in its public interface.  Also note that this method is defined
        // inline to work around a Windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<VISITOR&>(visitor, this->d_type);
            return;                                                   // RETURN
        }

        bslmf::Nil nil = bslmf::Nil();
        visitor(nil);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    apply(const VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to the 'visitor' object's
        // 'operator()'.  This method does not return a value.  If this variant
        // is unset, a default constructed 'bslmf::Nil' is passed to the
        // 'visitor'.  Note that this method is selected only if the template
        // parameter type 'VISITOR' does not define a 'typedef' of 'ResultType'
        // in its public interface.  Also note that this method is defined
        // inline to work around a Windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<const VISITOR&>(visitor, this->d_type);
            return;                                                   // RETURN
        }

        bslmf::Nil nil = bslmf::Nil();
        visitor(nil);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    apply(VISITOR& visitor, const TYPE& defaultValue) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value (of template parameter 'TYPE') this variant currently
        // holds to the 'visitor' object's 'operator()'.  This method does not
        // return a value.  If this variant is unset, the specified
        // 'defaultValue' of template parameter 'TYPE' is passed to the
        // 'visitor'.  'TYPE' must be the same as one of the types that this
        // variant can hold.  The behavior is undefined unless this variant is
        // unset or holds a value of template parameter 'TYPE'.  Note that this
        // method is selected only if the template parameter type 'VISITOR'
        // does not define a 'typedef' of 'ResultType' in its public interface.
        // Also note that this method is defined inline to work around a
        // Windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<VISITOR&>(visitor, this->d_type);
            return;                                                   // RETURN
        }

        visitor(defaultValue);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    apply(const VISITOR& visitor, const TYPE& defaultValue) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value (of template parameter 'TYPE') this variant currently
        // holds to the 'visitor' object's 'operator()'.  This method does not
        // return a value.  If this variant is unset, the specified
        // 'defaultValue' of template parameter 'TYPE' is passed to the
        // 'visitor'.  'TYPE' must be the same as one of the types that this
        // variant can hold.  The behavior is undefined unless this variant is
        // unset or holds a value of template parameter 'TYPE'.  Note that this
        // method is selected only if the template parameter type 'VISITOR'
        // does not define a 'typedef' of 'ResultType' in its public interface.
        // Also note that this method is defined inline to work around a
        // Windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<const VISITOR&>(visitor, this->d_type);
            return;                                                   // RETURN
        }

        visitor(defaultValue);
    }

    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(VISITOR& visitor);
    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(const VISITOR& visitor);
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value (of template parameter 'TYPE') this variant currently
        // holds to the 'visitor' object's 'operator()', and return the value
        // (of template parameter 'RET_TYPE') returned by the 'visitor'.  If
        // this variant is unset, a default constructed 'bslmf::Nil' is passed
        // to the 'visitor'.

    template <class RET_TYPE, class VISITOR, class TYPE>
    RET_TYPE apply(VISITOR& visitor, const TYPE& defaultValue);
    template <class RET_TYPE, class VISITOR, class TYPE>
    RET_TYPE apply(const VISITOR& visitor, const TYPE& defaultValue);
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value (of template parameter 'TYPE') this variant currently
        // holds to the 'visitor' object's 'operator()', and return the value
        // (of template parameter 'RET_TYPE') returned by the 'visitor'.  If
        // this variant is unset, the specified 'defaultValue' of template
        // parameter 'TYPE' is passed to the 'visitor'.  'TYPE' must be the
        // same as one of the types that this variant can hold.  The behavior
        // is undefined unless this variant is unset or holds a value of
        // template parameter 'TYPE'.

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    applyRaw(VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value (of template parameter 'TYPE') this variant currently
        // holds to the 'visitor' object's 'operator()', and return the value
        // returned by the 'visitor'.  The behavior is undefined if this
        // variant is unset.  Note that this method is selected only if the
        // template parameter type 'VISITOR' defines a 'typedef' of
        // 'ResultType' in its public interface.  Also note that this method is
        // defined inline to work around a Windows compiler bug with SFINAE
        // functions.

        typedef Variant_RawVisitorHelper<typename VISITOR::ResultType,
                                         VISITOR> Helper;

        return doApplyR<const Helper&,
                        typename VISITOR::ResultType>(Helper(&visitor),
                                                      this->d_type);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    applyRaw(const VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value (of template parameter 'TYPE') this variant currently
        // holds to the 'visitor' object's 'operator()', and return the value
        // returned by the 'visitor'.  The behavior is undefined if this
        // variant is unset.  Note that this method is selected only if the
        // template parameter type 'VISITOR' defines a 'typedef' of
        // 'ResultType' in its public interface.  Also note that this method is
        // defined inline to work around a Windows compiler bug with SFINAE
        // functions.

        typedef Variant_RawVisitorHelper<typename VISITOR::ResultType,
                                         const VISITOR> Helper;

        return doApplyR<const Helper&,
                        typename VISITOR::ResultType>(Helper(&visitor),
                                                      this->d_type);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    applyRaw(VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value (of template parameter 'TYPE') this variant currently
        // holds to the 'visitor' object's 'operator()'.  This method does not
        // return a value.  The behavior is undefined if this variant is unset.
        // Note that this method is selected only if the template parameter
        // type 'VISITOR' does not define a 'typedef' of 'ResultType' in its
        // public interface.  Also note that this method is defined inline to
        // work around a Windows compiler bug with SFINAE functions.

        typedef Variant_RawVisitorHelper<void, VISITOR> Helper;

        doApply<const Helper&>(Helper(&visitor), this->d_type);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    applyRaw(const VISITOR& visitor) {
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value (of template parameter 'TYPE') this variant currently
        // holds to the 'visitor' object's 'operator()'.  This method does not
        // return a value.  The behavior is undefined if this variant is unset.
        // Note that this method is selected only if the template parameter
        // type 'VISITOR' does not define a 'typedef' of 'ResultType' in its
        // public interface.  Also note that this method is defined inline to
        // work around a Windows compiler bug with SFINAE functions.

        typedef Variant_RawVisitorHelper<void, const VISITOR> Helper;

        doApply<const Helper&>(Helper(&visitor), this->d_type);
    }

    template <class RET_TYPE, class VISITOR>
    RET_TYPE applyRaw(VISITOR& visitor);
    template <class RET_TYPE, class VISITOR>
    RET_TYPE applyRaw(const VISITOR& visitor);
        // Apply the specified 'visitor' to this modifiable variant by passing
        // the value this variant currently holds to the 'visitor' object's
        // 'operator()', and return the value (of template parameter
        // 'RET_TYPE') returned by the 'visitor'.  The behavior is undefined if
        // this variant is unset.

    template <class TYPE>
    VariantImp& assign(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    VariantImp& assign(TYPE&&                  value);
#else
    VariantImp& assign(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.

    template <class TYPE, class SOURCE_TYPE>
    VariantImp& assignTo(const SOURCE_TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'SOURCE_TYPE' converted to template parameter 'TYPE', and return a
        // reference providing modifiable access to this object.  The value
        // currently held by this variant (if any) is destroyed if that value's
        // type is not the same as 'TYPE'.  'TYPE' must be the same as one of
        // the types that this variant can hold and 'SOURCE_TYPE' must be
        // convertible to 'TYPE'.

    template <class TYPE>
    TYPE& createInPlace();
    template <class TYPE, class A1>
    TYPE& createInPlace(const A1& a1);
    template <class TYPE, class A1, class A2>
    TYPE& createInPlace(const A1& a1, const A2& a2);
    template <class TYPE, class A1, class A2, class A3>
    TYPE& createInPlace(const A1& a1, const A2& a2, const A3& a3);
    template <class TYPE, class A1, class A2, class A3, class A4>
    TYPE& createInPlace(const A1& a1, const A2& a2, const A3& a3,
                        const A4& a4);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5>
    TYPE& createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6>
    TYPE& createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7>
    TYPE& createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8>
    TYPE& createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7,
                        const A8& a8);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8, class A9>
    TYPE& createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                        const A9& a9);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8, class A9, class A10>
    TYPE& createInPlace(const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                        const A9& a9, const A10& a10);
    template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                          class A6, class A7, class A8, class A9, class A10,
                          class A11>
    TYPE& createInPlace(const A1&  a1,  const A2&  a2, const A3& a3,
                        const A4&  a4,  const A5&  a5, const A6& a6,
                        const A7&  a7,  const A8&  a8, const A9& a9,
                        const A10& a10, const A11& a11);
    template <class TYPE, class A1,  class A2, class A3, class A4, class A5,
                          class A6,  class A7, class A8, class A9, class A10,
                          class A11, class A12>
    TYPE& createInPlace(const A1&  a1,  const A2&  a2,  const A3&  a3,
                        const A4&  a4,  const A5&  a5,  const A6&  a6,
                        const A7&  a7,  const A8&  a8,  const A9&  a9,
                        const A10& a10, const A11& a11, const A12& a12);
    template <class TYPE, class A1,  class A2,  class A3, class A4, class A5,
                          class A6,  class A7,  class A8, class A9, class A10,
                          class A11, class A12, class A13>
    TYPE& createInPlace(const A1&  a1,  const A2&  a2,  const A3&  a3,
                        const A4&  a4,  const A5&  a5,  const A6&  a6,
                        const A7&  a7,  const A8&  a8,  const A9&  a9,
                        const A10& a10, const A11& a11, const A12& a12,
                        const A13& a13);
    template <class TYPE, class A1,  class A2,  class A3,  class A4, class A5,
                          class A6,  class A7,  class A8,  class A9, class A10,
                          class A11, class A12, class A13, class A14>
    TYPE& createInPlace(const A1&  a1,  const A2&  a2,  const A3&  a3,
                        const A4&  a4,  const A5&  a5,  const A6&  a6,
                        const A7&  a7,  const A8&  a8,  const A9&  a9,
                        const A10& a10, const A11& a11, const A12& a12,
                        const A13& a13, const A14& a14);
        // Create an instance of template parameter 'TYPE' in this variant
        // object with up to 14 parameters using the allocator currently held
        // by this variant to supply memory, and return a reference providing
        // modifiable access to the created instance.  This method first
        // destroys the current value held by this variant (even if 'TYPE' is
        // the same as the type currently held).  'TYPE' must be the same as
        // one of the types that this variant can hold.  Note the order of the
        // template arguments was chosen so that 'TYPE' must always be
        // specified.

    void reset();
        // Destroy the current value held by this variant (if any), and reset
        // this variant to the unset state.

    void swap(VariantImp& other);
        // Swap the value of this object with the value of the specified
        // 'other' object.  This method provides the no-throw guarantee if the
        // 'TYPE' template parameter has a no-throw 'swap' and the two variant
        // objects being swapped have the same type; otherwise this method
        // provides the basic guarantee.

    template <class TYPE>
    TYPE& the();
        // Return a reference providing modifiable access to the value of
        // template parameter 'TYPE' held by this variant object.  'TYPE' must
        // be the same as one of the types that this variant can hold.  The
        // behavior is undefined unless 'is<TYPE>()' returns 'true' and 'TYPE'
        // is not 'void'.  Note that 'TYPE' must be specified explicitly, e.g.,
        // 'myValue.the<int>()'.

    // ACCESSORS
    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    apply(VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to the 'visitor' object's 'operator()',
        // and return the value returned by the 'visitor'.  If this variant is
        // unset, a default constructed 'bslmf::Nil' is passed to the
        // 'visitor'.  Note that this method is selected only if the template
        // parameter type 'VISITOR' defines a 'typedef' of 'ResultType' in its
        // public interface.  Also note that this method is defined inline to
        // work around a Windows compiler bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
                                                                      // RETURN
        }

        bslmf::Nil nil = bslmf::Nil();
        return visitor(nil);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    apply(const VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to the 'visitor' object's 'operator()',
        // and return the value returned by the 'visitor.  If this variant is
        // unset, a default constructed 'bslmf::Nil' is passed to the
        // 'visitor'.  Note that this method is selected only if the template
        // parameter type 'VISITOR' defines a 'typedef' of 'ResultType' in its
        // public interface.  Also note that this method is defined inline to
        // work around a Windows compiler bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<const VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
                                                                      // RETURN
        }

        bslmf::Nil nil = bslmf::Nil();
        return visitor(nil);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    apply(VISITOR& visitor, const TYPE& defaultValue) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // (of template parameter 'TYPE') this variant currently holds to the
        // 'visitor' object's 'operator()', and return the value returned by
        // the 'visitor'.  If this variant is unset, the specified
        // 'defaultValue' of template parameter 'TYPE' is passed to the
        // 'visitor'.  'TYPE' must be the same as one of the types that this
        // variant can hold.  The behavior is undefined unless this variant is
        // unset or holds a value of template parameter 'TYPE'.  Note that this
        // method is selected only if the template parameter type 'VISITOR'
        // defines a 'typedef' of 'ResultType' in its public interface.  Also
        // note that this method is defined inline to work around a Windows
        // compiler bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
                                                                      // RETURN
        }

        return visitor(defaultValue);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    apply(const VISITOR& visitor, const TYPE& defaultValue) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // (of template parameter 'TYPE') this variant currently holds to the
        // 'visitor' object's 'operator()', and return the value returned by
        // the 'visitor'.  If this variant is unset, the specified
        // 'defaultValue' of template parameter 'TYPE' is passed to the
        // 'visitor'.  'TYPE' must be the same as one of the types that this
        // variant can hold.  The behavior is undefined unless this variant is
        // unset or holds a value of template parameter 'TYPE'.  Note that this
        // method is selected only if the template parameter type 'VISITOR'
        // defines a 'typedef' of 'ResultType' in its public interface.  Also
        // note that this method is defined inline to work around a Windows
        // compiler bug with SFINAE functions.

        if (this->d_type) {
            return doApplyR<const VISITOR&,
                          typename VISITOR::ResultType>(visitor, this->d_type);
                                                                      // RETURN
        }

        return visitor(defaultValue);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    apply(VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to the 'visitor' object's 'operator()'.
        // This method does not return a value.  If this variant is unset, a
        // default constructed 'bslmf::Nil' is passed to the 'visitor'.  Note
        // that this method is selected only if the template parameter type
        // 'VISITOR' does not define a 'typedef' of 'ResultType' in its public
        // interface.  Also note that this method is defined inline to work
        // around a Windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<VISITOR&>(visitor, this->d_type);
            return;                                                   // RETURN
        }

        bslmf::Nil nil = bslmf::Nil();
        visitor(nil);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    apply(const VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to the 'visitor' object's 'operator()'.
        // This method does not return a value.  If this variant is unset, a
        // default constructed 'bslmf::Nil' is passed to the 'visitor'.  Note
        // that this method is selected only if the template parameter type
        // 'VISITOR' does not define a 'typedef' of 'ResultType' in its public
        // interface.  Also note that this method is defined inline to work
        // around a Windows compiler bug with SFINAE functions.

        if (this->d_type) {
            doApply<const VISITOR&>(visitor, this->d_type);
            return;                                                   // RETURN
        }

        bslmf::Nil nil = bslmf::Nil();
        visitor(nil);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    apply(VISITOR& visitor, const TYPE& defaultValue) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // (of template parameter 'TYPE') this variant currently holds to the
        // 'visitor' object's 'operator()'.  This method does not return a
        // value.  If this variant is unset, the specified 'defaultValue' of
        // template parameter 'TYPE' is passed to the 'visitor'.  'TYPE' must
        // be the same as one of the types that this variant can hold.  The
        // behavior is undefined unless this variant is unset or holds a value
        // of template parameter 'TYPE'.  Note that this method is selected
        // only if the template parameter type 'VISITOR' does not define a
        // 'typedef' of 'ResultType' in its public interface.  Also note that
        // this method is defined inline to work around a Windows compiler bug
        // with SFINAE functions.

        if (this->d_type) {
            doApply<VISITOR&>(visitor, this->d_type);
            return;                                                   // RETURN
        }

        visitor(defaultValue);
    }

    template <class VISITOR, class TYPE>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    apply(const VISITOR& visitor, const TYPE& defaultValue) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // (of template parameter 'TYPE') this variant currently holds to the
        // 'visitor' object's 'operator()'.  This method does not return a
        // value.  If this variant is unset, the specified 'defaultValue' of
        // template parameter 'TYPE' is passed to the 'visitor'.  'TYPE' must
        // be the same as one of the types that this variant can hold.  The
        // behavior is undefined unless this variant is unset or holds a value
        // of template parameter 'TYPE'.  Note that this method is selected
        // only if the template parameter type 'VISITOR' does not define a
        // 'typedef' of 'ResultType' in its public interface.  Also note that
        // this method is defined inline to work around a Windows compiler bug
        // with SFINAE functions.

        if (this->d_type) {
            doApply<const VISITOR&>(visitor, this->d_type);
            return;                                                   // RETURN
        }

        visitor(defaultValue);
    }

    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(VISITOR& visitor) const;
    template <class RET_TYPE, class VISITOR>
    RET_TYPE apply(const VISITOR& visitor) const;
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to the 'visitor' object's 'operator()',
        // and return the value (of template parameter 'RET_TYPE') returned by
        // the 'visitor'.  If this variant is unset, a default constructed
        // 'bslmf::Nil' is passed to the 'visitor'.

    template <class RET_TYPE, class VISITOR, class TYPE>
    RET_TYPE apply(VISITOR& visitor, const TYPE& defaultValue) const;
    template <class RET_TYPE, class VISITOR, class TYPE>
    RET_TYPE apply(const VISITOR& visitor, const TYPE& defaultValue) const;
        // Apply the specified 'visitor' to this variant by passing the value
        // (of template parameter 'TYPE') this variant currently holds to the
        // 'visitor' object's 'operator()', and return the value (of template
        // parameter 'RET_TYPE') returned by the 'visitor'.  If this variant is
        // unset, the specified 'defaultValue' of template parameter 'TYPE' is
        // passed to the 'visitor'.  'TYPE' must be the same as one of the
        // types that this variant can hold.  The behavior is undefined unless
        // this variant is unset or holds a value of template parameter 'TYPE'.

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    applyRaw(VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to the 'visitor' object's 'operator()',
        // and return the value (of template parameter 'RET_TYPE') returned by
        // the 'visitor'.  The behavior is undefined if this variant is unset.
        // Note that this method is selected only if the template parameter
        // type 'VISITOR' defines a 'typedef' of 'ResultType' in its public
        // interface.  Also note that this method is defined inline to work
        // around a Windows compiler bug with SFINAE functions.

        typedef Variant_RawVisitorHelper<typename VISITOR::ResultType,
                                         VISITOR> Helper;

        return doApplyR<const Helper&,
                        typename VISITOR::ResultType>(Helper(&visitor),
                                                      this->d_type);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 1,
                            typename VISITOR::ResultType>::type
    applyRaw(const VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to the 'visitor' object's 'operator()',
        // and return the value returned by the 'visitor'.  The behavior is
        // undefined if this variant is unset.  Note that this method is
        // selected only if the template parameter type 'VISITOR' defines a
        // 'typedef' of 'ResultType' in its public interface.  Also note that
        // this method is defined inline to work around a Windows compiler bug
        // with SFINAE functions.

        typedef Variant_RawVisitorHelper<typename VISITOR::ResultType,
                                         const VISITOR> Helper;

        return doApplyR<const Helper&,
                        typename VISITOR::ResultType>(Helper(&visitor),
                                                      this->d_type);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    applyRaw(VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to the 'visitor' object's 'operator()'.
        // This method does not return a value.  The behavior is undefined if
        // this variant is unset.  Note that this method is selected only if
        // the template parameter type 'VISITOR' does not define a 'typedef' of
        // 'ResultType' in its public interface.  Also note that this method is
        // defined inline to work around a Windows compiler bug with SFINAE
        // functions.

        typedef Variant_RawVisitorHelper<void, VISITOR> Helper;

        return doApply<const Helper&>(Helper(&visitor), this->d_type);
    }

    template <class VISITOR>
    typename bsl::enable_if<Variant_ReturnValueHelper<VISITOR>::value == 0,
                            void>::type
    applyRaw(const VISITOR& visitor) const {
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to the 'visitor' object's 'operator()'.
        // This method does not return a value.  The behavior is undefined if
        // this variant is unset.  Note that this method is selected only if
        // the template parameter type 'VISITOR' does not define a 'typedef' of
        // 'ResultType' in its public interface.  Also note that this method is
        // defined inline to work around a Windows compiler bug with SFINAE
        // functions.

        typedef Variant_RawVisitorHelper<void, const VISITOR> Helper;

        return doApply<const Helper&>(Helper(&visitor), this->d_type);
    }

    template <class RET_TYPE, class VISITOR>
    RET_TYPE applyRaw(VISITOR& visitor) const;
    template <class RET_TYPE, class VISITOR>
    RET_TYPE applyRaw(const VISITOR& visitor) const;
        // Apply the specified 'visitor' to this variant by passing the value
        // this variant currently holds to the 'visitor' object's 'operator()',
        // and return the value (of template parameter 'RET_TYPE') returned by
        // the 'visitor'.  The behavior is undefined if this variant is unset.

    template <class TYPE>
    bool is() const;
        // Return 'true' if the value held by this variant object is of
        // template parameter 'TYPE', and 'false' otherwise.  'TYPE' must be
        // the same as one of the types that this variant can hold.  Note that
        // 'TYPE' must be specified explicitly, e.g., 'myValue.is<int>()'.

    bool isUnset() const;
        // Return 'true' if this variant is currently unset, and 'false'
        // otherwise.  An unset variant does not hold a value or type.  Note
        // that this method should be preferred over checking the type index of
        // the variant.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level', and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.  Nothing is
        // printed if this variant is unset.  Each type that may be contained
        // by this variant shall be printable with 'bdlb::PrintMethods'
        // (typically meaning that they either declare the
        // 'bdlb::HasPrintMethods' trait or provide the '<<' output streaming
        // operator).  See {'bdlb_printmethods'}.  The compiler will emit an
        // error if 'bdlb::PrintMethods::print' cannot be instantiated for each
        // type that may be contained by this variant.

    template <class TYPE>
    const TYPE& the() const;
        // Return a reference providing non-modifiable access to the value of
        // template parameter 'TYPE' held by this variant object.  'TYPE' must
        // be the same as one of the types that this variant can hold.  The
        // behavior is undefined unless 'is<TYPE>()' returns 'true' and 'TYPE'
        // is not 'void'.  Note that 'TYPE' must be specified explicitly, e.g.,
        // 'myValue.the<int>()'.

    int typeIndex() const;
        // Return the index in the list of 'TYPES' corresponding to the type of
        // the value currently held by this variant object (starting at 1), or
        // 0 if this object is unset.  Note that instead of switching code on
        // the type index, calling 'apply' is the preferred method of
        // manipulating different types stored inside a variant.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    const bsl::type_info& typeInfo() const;
        // Return 'typeid(void)'.
        //
        // DEPRECATED: Do not use.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // DEPRECATED: Do not use.

    int maxSupportedBdexVersion() const;
        // DEPRECATED: Do not use.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // DEPRECATED: Do not use.

#endif  // BDE_OMIT_INTERNAL_DEPRECATED
};

// FREE OPERATORS
template <class TYPES>
bool operator==(const VariantImp<TYPES>& lhs,
                const VariantImp<TYPES>& rhs);
    // Return 'true' if the specified 'lhs' variant object has the same value
    // as the specified 'rhs' variant object, and 'false' otherwise.  Two
    // variant objects have the same value if they are both set and hold
    // objects of the same type and same value, or are both unset.

template <class TYPES>
bool operator!=(const VariantImp<TYPES>& lhs,
                const VariantImp<TYPES>& rhs);
    // Return 'true' if the specified 'lhs' variant object does not have the
    // same value as the specified 'rhs' variant object, and 'false' otherwise.
    // Two variant objects do not have the same value if one is set and the
    // other is unset, or if they are both set but hold objects that differ in
    // type or value.

template <class TYPES>
bsl::ostream& operator<<(bsl::ostream&            stream,
                         const VariantImp<TYPES>& object);
    // Write the specified variant 'object' to the specified output 'stream' in
    // a single-line (human-readable) format, and return a reference to
    // 'stream'.

// FREE FUNCTIONS
template <class TYPES>
void swap(VariantImp<TYPES>& a, VariantImp<TYPES>& b);
    // Swap the values of the specified 'a' and 'b' objects.  This method
    // provides the no-throw guarantee if the 'TYPE' template parameter has a
    // no-throw 'swap' and the two variant objects being swapped has the same
    // type; otherwise this method provides the basic guarantee.

                       // ==================
                       // class Variant<...>
                       // ==================

#if defined(BDLB_VARIANT_USING_VARIADIC_TEMPLATES)
template <class ...TYPES>
class Variant : public VariantImp<typename bslmf::TypeList<TYPES...>::ListType>
                                                                              {
#else
template <class A1  = bslmf::Nil, class A2  = bslmf::Nil,
          class A3  = bslmf::Nil, class A4  = bslmf::Nil,
          class A5  = bslmf::Nil, class A6  = bslmf::Nil,
          class A7  = bslmf::Nil, class A8  = bslmf::Nil,
          class A9  = bslmf::Nil, class A10 = bslmf::Nil,
          class A11 = bslmf::Nil, class A12 = bslmf::Nil,
          class A13 = bslmf::Nil, class A14 = bslmf::Nil,
          class A15 = bslmf::Nil, class A16 = bslmf::Nil,
          class A17 = bslmf::Nil, class A18 = bslmf::Nil,
          class A19 = bslmf::Nil, class A20 = bslmf::Nil>
class Variant : public VariantImp<typename bslmf::TypeList<
                                          A1,  A2,  A3,  A4,  A5,
                                          A6,  A7,  A8,  A9,  A10,
                                          A11, A12, A13, A14, A15,
                                          A16, A17, A18, A19, A20>::ListType> {
#endif
    // This class provides a "variant" type, i.e., a type capable of storing
    // values from a list of template parameter types 'A1' to 'A20'.  Note that
    // if the number 'N' of types is smaller than 20, 'AN+1' up to 'A20'
    // default to 'bslmf::Nil', but it is more economical to use 'VariantN',
    // which accepts exactly 'N' template arguments, as this leads to shorter
    // symbols and debug string information.

    // PRIVATE TYPES
#if defined(BDLB_VARIANT_USING_VARIADIC_TEMPLATES)
    typedef VariantImp<typename bslmf::TypeList<TYPES...>::ListType>  Imp;

    typedef Variant<TYPES...>                                         SelfType;
        // 'SelfType' is an alias to this class.
#else
    typedef VariantImp<typename bslmf::TypeList<A1,  A2,  A3,  A4,  A5,  A6,
                                                A7,  A8,  A9,  A10, A11, A12,
                                                A13, A14, A15, A16, A17, A18,
                                                A19, A20>::ListType>  Imp;

    typedef Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8,  A9,  A10,
                    A11, A12, A13, A14, A15, A16, A17, A18, A19, A20> SelfType;
        // 'SelfType' is an alias to this class.
#endif

    typedef VariantImp_Traits<typename Imp::TypeList>                 Traits;
    typedef bslmf::MovableRefUtil                                     MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant, HasPrintMethod);

    // CREATORS
    Variant();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant(TYPE&&                   value,
            typename bsl::enable_if<
                !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                &&
                !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                        void>::type * = 0);
#else
    Variant(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant(TYPE&&                   value,
            typename bsl::enable_if<
                !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
            bslma::Allocator>::type *basicAllocator);
#else
    Variant(bslmf::MovableRef<TYPE>  value,
            bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant(const Variant& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant(bslmf::MovableRef<Variant> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant(bslmf::MovableRef<Variant>  original,
            bslma::Allocator           *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant& operator=(const Variant& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant& operator=(bslmf::MovableRef<Variant> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ===================
                       // class Variant2<...>
                       // ===================

template <class A1, class A2>
class Variant2 : public VariantImp<typename bslmf::TypeList2<
                                                           A1, A2>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (2) of
    // types.  Its 2 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList2<A1, A2>::ListType> Imp;

    typedef Variant2<A1, A2>                                        SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>               Traits;
    typedef bslmf::MovableRefUtil                                   MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant2,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant2,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant2,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant2, HasPrintMethod);

    // CREATORS
    Variant2();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant2(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant2(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant2(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                 &&
                 !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                         void>::type * = 0);
#else
    Variant2(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant2(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
             bslma::Allocator>::type *basicAllocator);
#else
    Variant2(bslmf::MovableRef<TYPE>  value,
             bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant2(const Variant2& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant2(bslmf::MovableRef<Variant2> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant2(bslmf::MovableRef<Variant2>  original,
             bslma::Allocator            *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant2& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant2&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant2& operator=(const Variant2& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant2& operator=(bslmf::MovableRef<Variant2> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ===================
                       // class Variant3<...>
                       // ===================

template <class A1, class A2, class A3>
class Variant3 : public VariantImp<typename bslmf::TypeList3<
                                                       A1, A2, A3>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (3) of
    // types.  Its 3 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, A3>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList3<A1, A2,
                                                 A3>::ListType> Imp;

    typedef Variant3<A1, A2, A3>                                SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>           Traits;
    typedef bslmf::MovableRefUtil                               MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant3,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant3,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant3,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant3, HasPrintMethod);

    // CREATORS
    Variant3();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant3(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant3(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant3(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                 &&
                 !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                         void>::type * = 0);
#else
    Variant3(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant3(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
             bslma::Allocator>::type *basicAllocator);
#else
    Variant3(bslmf::MovableRef<TYPE>  value,
             bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant3(const Variant3& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant3(bslmf::MovableRef<Variant3> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant3(bslmf::MovableRef<Variant3>  original,
             bslma::Allocator            *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant3& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant3&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant3& operator=(const Variant3& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant3& operator=(bslmf::MovableRef<Variant3> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ===================
                       // class Variant4<...>
                       // ===================

template <class A1, class A2, class A3, class A4>
class Variant4 : public VariantImp<typename bslmf::TypeList4<
                                                   A1, A2, A3, A4>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (4) of
    // types.  Its 4 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A4>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList4<A1, A2, A3,
                                                 A4>::ListType> Imp;

    typedef Variant4<A1, A2, A3, A4>                            SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>           Traits;
    typedef bslmf::MovableRefUtil                               MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant4,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant4,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant4,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant4, HasPrintMethod);

    // CREATORS
    Variant4();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant4(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant4(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant4(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                 &&
                 !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                         void>::type * = 0);
#else
    Variant4(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant4(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
             bslma::Allocator>::type *basicAllocator);
#else
    Variant4(bslmf::MovableRef<TYPE>  value,
             bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant4(const Variant4& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant4(bslmf::MovableRef<Variant4> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant4(bslmf::MovableRef<Variant4>  original,
             bslma::Allocator            *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant4& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant4&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant4& operator=(const Variant4& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant4& operator=(bslmf::MovableRef<Variant4> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ===================
                       // class Variant5<...>
                       // ===================

template <class A1, class A2, class A3, class A4, class A5>
class Variant5 : public VariantImp<typename bslmf::TypeList5<
                                               A1, A2, A3, A4, A5>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (5) of
    // types.  Its 5 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A5>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList5<A1, A2, A3, A4,
                                                 A5>::ListType> Imp;

    typedef Variant5<A1, A2, A3, A4, A5>                        SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>           Traits;
    typedef bslmf::MovableRefUtil                               MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant5,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant5,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant5,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant5, HasPrintMethod);

    // CREATORS
    Variant5();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant5(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant5(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant5(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                 &&
                 !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                         void>::type * = 0);
#else
    Variant5(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant5(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
             bslma::Allocator>::type *basicAllocator);
#else
    Variant5(bslmf::MovableRef<TYPE>  value,
             bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant5(const Variant5& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant5(bslmf::MovableRef<Variant5> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant5(bslmf::MovableRef<Variant5>  original,
             bslma::Allocator            *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant5& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant5&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant5& operator=(const Variant5& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant5& operator=(bslmf::MovableRef<Variant5> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ===================
                       // class Variant6<...>
                       // ===================

template <class A1, class A2, class A3, class A4, class A5, class A6>
class Variant6 : public VariantImp<typename bslmf::TypeList6<
                                           A1, A2, A3, A4, A5, A6>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (6) of
    // types.  Its 6 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A6>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList6<A1, A2, A3, A4, A5,
                                                 A6>::ListType> Imp;

    typedef Variant6<A1, A2, A3, A4, A5, A6>                    SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>           Traits;
    typedef bslmf::MovableRefUtil                               MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant6,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant6,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant6,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant6, HasPrintMethod);

    // CREATORS
    Variant6();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant6(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant6(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant6(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                 &&
                 !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                         void>::type * = 0);
#else
    Variant6(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant6(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
             bslma::Allocator>::type *basicAllocator);
#else
    Variant6(bslmf::MovableRef<TYPE>  value,
             bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant6(const Variant6& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant6(bslmf::MovableRef<Variant6> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant6(bslmf::MovableRef<Variant6>  original,
             bslma::Allocator            *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant6& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant6&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant6& operator=(const Variant6& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant6& operator=(bslmf::MovableRef<Variant6> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ===================
                       // class Variant7<...>
                       // ===================

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
class Variant7 : public VariantImp<typename bslmf::TypeList7<
                                       A1, A2, A3, A4, A5, A6, A7>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (7) of
    // types.  Its 7 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A7>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList7<A1, A2, A3, A4, A5, A6,
                                                 A7>::ListType> Imp;

    typedef Variant7<A1, A2, A3, A4, A5, A6, A7>                SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>           Traits;
    typedef bslmf::MovableRefUtil                               MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant7,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant7,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant7,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant7, HasPrintMethod);

    // CREATORS
    Variant7();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant7(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant7(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant7(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                 &&
                 !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                         void>::type * = 0);
#else
    Variant7(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant7(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
             bslma::Allocator>::type *basicAllocator);
#else
    Variant7(bslmf::MovableRef<TYPE>  value,
             bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant7(const Variant7& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant7(bslmf::MovableRef<Variant7> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant7(bslmf::MovableRef<Variant7>  original,
             bslma::Allocator            *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant7& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant7&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant7& operator=(const Variant7& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant7& operator=(bslmf::MovableRef<Variant7> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ===================
                       // class Variant8<...>
                       // ===================

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
class Variant8 : public VariantImp<typename bslmf::TypeList8<
              A1, A2, A3, A4, A5, A6, A7, A8>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (8) of
    // types.  Its 8 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A8>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList8<A1, A2, A3, A4, A5, A6, A7,
                                                 A8>::ListType> Imp;

    typedef Variant8<A1, A2, A3, A4, A5, A6, A7, A8>            SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>           Traits;
    typedef bslmf::MovableRefUtil                               MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant8,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant8,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant8,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant8, HasPrintMethod);

    // CREATORS
    Variant8();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant8(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant8(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant8(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                 &&
                 !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                         void>::type * = 0);
#else
    Variant8(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant8(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
             bslma::Allocator>::type *basicAllocator);
#else
    Variant8(bslmf::MovableRef<TYPE>  value,
             bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant8(const Variant8& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant8(bslmf::MovableRef<Variant8> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant8(bslmf::MovableRef<Variant8>  original,
             bslma::Allocator            *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant8& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant8&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant8& operator=(const Variant8& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant8& operator=(bslmf::MovableRef<Variant8> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ===================
                       // class Variant9<...>
                       // ===================

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
class Variant9 : public VariantImp<typename bslmf::TypeList9<
              A1, A2, A3, A4, A5, A6, A7, A8, A9>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (9) of
    // types.  Its 9 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A9>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList9<A1, A2, A3, A4, A5, A6, A7,
                                                 A8, A9>::ListType> Imp;

    typedef Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>            SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>               Traits;
    typedef bslmf::MovableRefUtil                                   MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant9,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant9,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant9,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant9, HasPrintMethod);

    // CREATORS
    Variant9();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant9(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant9(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant9(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                 &&
                 !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                         void>::type * = 0);
#else
    Variant9(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant9(TYPE&&                   value,
             typename bsl::enable_if<
                 !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
             bslma::Allocator>::type *basicAllocator);
#else
    Variant9(bslmf::MovableRef<TYPE>  value,
             bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant9(const Variant9& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant9(bslmf::MovableRef<Variant9> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant9(bslmf::MovableRef<Variant9>  original,
             bslma::Allocator            *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant9& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant9&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant9& operator=(const Variant9& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant9& operator=(bslmf::MovableRef<Variant9> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ====================
                       // class Variant10<...>
                       // ====================

template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
class Variant10 : public VariantImp<typename bslmf::TypeList10<
                          A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (10) of
    // types.  Its 10 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A10>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList10<A1, A2, A3, A4, A5, A6, A7,
                                                  A8, A9,
                                                  A10>::ListType> Imp;

    typedef Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>    SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;
    typedef bslmf::MovableRefUtil                                 MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant10,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant10,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant10,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant10, HasPrintMethod);

    // CREATORS
    Variant10();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant10(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant10(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant10(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                  &&
                  !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                          void>::type * = 0);
#else
    Variant10(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant10(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
              bslma::Allocator>::type *basicAllocator);
#else
    Variant10(bslmf::MovableRef<TYPE>  value,
              bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant10(const Variant10& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant10(bslmf::MovableRef<Variant10> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant10(bslmf::MovableRef<Variant10>  original,
              bslma::Allocator             *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant10& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant10&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant10& operator=(const Variant10& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant10& operator=(bslmf::MovableRef<Variant10> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ====================
                       // class Variant11<...>
                       // ====================

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
class Variant11 : public VariantImp<typename bslmf::TypeList11<
                     A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (11) of
    // types.  Its 11 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A11>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList11<A1, A2, A3, A4, A5, A6, A7,
                                                  A8, A9, A10,
                                                  A11>::ListType> Imp;

    typedef Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10,
                      A11>                                        SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;
    typedef bslmf::MovableRefUtil                                 MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant11,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant11,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant11,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant11, HasPrintMethod);

    // CREATORS
    Variant11();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant11(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant11(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant11(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                  &&
                  !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                          void>::type * = 0);
#else
    Variant11(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant11(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
              bslma::Allocator>::type *basicAllocator);
#else
    Variant11(bslmf::MovableRef<TYPE>  value,
              bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant11(const Variant11& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant11(bslmf::MovableRef<Variant11> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant11(bslmf::MovableRef<Variant11>  original,
              bslma::Allocator             *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant11& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant11&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant11& operator=(const Variant11& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant11& operator=(bslmf::MovableRef<Variant11> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ====================
                       // class Variant12<...>
                       // ====================

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
class Variant12 : public VariantImp<typename bslmf::TypeList12<
              A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (12) of
    // types.  Its 12 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A12>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList12<A1, A2, A3, A4,  A5, A6,
                                                  A7, A8, A9, A10, A11,
                                                  A12>::ListType> Imp;

    typedef Variant12<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10,
                      A11, A12>                                   SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;
    typedef bslmf::MovableRefUtil                                 MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant12,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant12,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant12,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant12, HasPrintMethod);

    // CREATORS
    Variant12();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant12(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant12(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant12(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                  &&
                  !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                          void>::type * = 0);
#else
    Variant12(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant12(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
              bslma::Allocator>::type *basicAllocator);
#else
    Variant12(bslmf::MovableRef<TYPE>  value,
              bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant12(const Variant12& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant12(bslmf::MovableRef<Variant12> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant12(bslmf::MovableRef<Variant12>  original,
              bslma::Allocator             *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant12& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant12&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant12& operator=(const Variant12& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant12& operator=(bslmf::MovableRef<Variant12> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ====================
                       // class Variant13<...>
                       // ====================

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
class Variant13 : public VariantImp<typename bslmf::TypeList13<
           A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (13) of
    // types.  Its 13 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A13>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList13<A1, A2, A3, A4,  A5,  A6,
                                                  A7, A8, A9, A10, A11, A12,
                                                  A13>::ListType> Imp;

    typedef Variant13<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9,
                      A10, A11, A12, A13>                         SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;
    typedef bslmf::MovableRefUtil                                 MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant13,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant13,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant13,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant13, HasPrintMethod);

    // CREATORS
    Variant13();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant13(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant13(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant13(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                  &&
                  !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                          void>::type * = 0);
#else
    Variant13(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant13(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
              bslma::Allocator>::type *basicAllocator);
#else
    Variant13(bslmf::MovableRef<TYPE>  value,
              bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant13(const Variant13& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant13(bslmf::MovableRef<Variant13> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant13(bslmf::MovableRef<Variant13>  original,
              bslma::Allocator             *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant13& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant13&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant13& operator=(const Variant13& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant13& operator=(bslmf::MovableRef<Variant13> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ====================
                       // class Variant14<...>
                       // ====================

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
class Variant14 : public VariantImp<typename bslmf::TypeList14<
                                               A1,  A2,  A3,  A4,  A5,
                                               A6,  A7,  A8,  A9,  A10,
                                               A11, A12, A13, A14>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (14) of
    // types.  Its 14 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A14>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList14<A1,  A2,  A3,  A4,  A5,  A6,
                                                  A7,  A8,  A9,  A10, A11, A12,
                                                  A13,
                                                  A14>::ListType> Imp;

    typedef Variant14<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9,
                      A10, A11, A12, A13, A14>                    SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;
    typedef bslmf::MovableRefUtil                                 MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant14,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant14,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant14,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant14, HasPrintMethod);

    // CREATORS
    Variant14();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant14(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant14(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant14(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                  &&
                  !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                          void>::type * = 0);
#else
    Variant14(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant14(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
              bslma::Allocator>::type *basicAllocator);
#else
    Variant14(bslmf::MovableRef<TYPE>  value,
              bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant14(const Variant14& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant14(bslmf::MovableRef<Variant14> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant14(bslmf::MovableRef<Variant14>  original,
              bslma::Allocator             *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant14& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant14&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant14& operator=(const Variant14& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant14& operator=(bslmf::MovableRef<Variant14> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ====================
                       // class Variant15<...>
                       // ====================

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
class Variant15 : public VariantImp<typename bslmf::TypeList15<
                                          A1,  A2,  A3,  A4,  A5,
                                          A6,  A7,  A8,  A9,  A10,
                                          A11, A12, A13, A14, A15>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (15) of
    // types.  Its 15 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A15>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList15<A1,  A2, A3, A4,  A5,  A6,
                                                  A7,  A8, A9, A10, A11, A12,
                                                  A13, A14,
                                                  A15>::ListType> Imp;

    typedef Variant15<A1,  A2,  A3,  A4,  A5,  A6, A7, A8, A9,
                      A10, A11, A12, A13, A14, A15>               SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;
    typedef bslmf::MovableRefUtil                                 MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant15,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant15,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant15,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant15, HasPrintMethod);

    // CREATORS
    Variant15();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant15(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant15(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant15(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                  &&
                  !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                          void>::type * = 0);
#else
    Variant15(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant15(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
              bslma::Allocator>::type *basicAllocator);
#else
    Variant15(bslmf::MovableRef<TYPE>  value,
              bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant15(const Variant15& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant15(bslmf::MovableRef<Variant15> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant15(bslmf::MovableRef<Variant15>  original,
              bslma::Allocator             *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant15& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant15&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant15& operator=(const Variant15& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant15& operator=(bslmf::MovableRef<Variant15> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ====================
                       // class Variant16<...>
                       // ====================

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
class Variant16 : public VariantImp<typename bslmf::TypeList16<
                                                       A1,  A2,  A3,  A4,  A5,
                                                       A6,  A7,  A8,  A9,  A10,
                                                       A11, A12, A13, A14, A15,
                                                       A16>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (16) of
    // types.  Its 16 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A16>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList16<A1,  A2,  A3, A4,  A5,  A6,
                                                  A7,  A8,  A9, A10, A11, A12,
                                                  A13, A14, A15,
                                                  A16>::ListType> Imp;

    typedef Variant16<A1,  A2,  A3,  A4,  A5,  A6,  A7, A8, A9,
                      A10, A11, A12, A13, A14, A15, A16>          SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;
    typedef bslmf::MovableRefUtil                                 MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant16,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant16,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant16,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant16, HasPrintMethod);

    // CREATORS
    Variant16();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant16(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant16(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant16(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                  &&
                  !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                          void>::type * = 0);
#else
    Variant16(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant16(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
              bslma::Allocator>::type *basicAllocator);
#else
    Variant16(bslmf::MovableRef<TYPE>  value,
              bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant16(const Variant16& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant16(bslmf::MovableRef<Variant16> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant16(bslmf::MovableRef<Variant16>  original,
              bslma::Allocator             *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant16& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant16&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant16& operator=(const Variant16& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant16& operator=(bslmf::MovableRef<Variant16> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ====================
                       // class Variant17<...>
                       // ====================

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
class Variant17 : public VariantImp<typename bslmf::TypeList17<
                                                       A1,  A2,  A3,  A4,  A5,
                                                       A6,  A7,  A8,  A9,  A10,
                                                       A11, A12, A13, A14, A15,
                                                       A16, A17>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (17) of
    // types.  Its 17 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A17>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList17<A1,  A2,  A3,  A4,  A5,  A6,
                                                  A7,  A8,  A9,  A10, A11, A12,
                                                  A13, A14, A15, A16,
                                                  A17>::ListType> Imp;

    typedef Variant17<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9,
                      A10, A11, A12, A13, A14, A15, A16, A17>     SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;
    typedef bslmf::MovableRefUtil                                 MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant17,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant17,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant17,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant17, HasPrintMethod);

    // CREATORS
    Variant17();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant17(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant17(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant17(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                  &&
                  !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                          void>::type * = 0);
#else
    Variant17(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant17(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
              bslma::Allocator>::type *basicAllocator);
#else
    Variant17(bslmf::MovableRef<TYPE>  value,
              bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant17(const Variant17& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant17(bslmf::MovableRef<Variant17> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant17(bslmf::MovableRef<Variant17>  original,
              bslma::Allocator             *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant17& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant17&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant17& operator=(const Variant17& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant17& operator=(bslmf::MovableRef<Variant17> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ====================
                       // class Variant18<...>
                       // ====================

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
class Variant18 : public VariantImp<typename bslmf::TypeList18<
                                                    A1,  A2,  A3,  A4,  A5,
                                                    A6,  A7,  A8,  A9,  A10,
                                                    A11, A12, A13, A14, A15,
                                                    A16, A17, A18>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (18) of
    // types.  Its 18 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A18>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList18<A1,  A2,  A3,  A4,  A5,  A6,
                                                  A7,  A8,  A9,  A10, A11, A12,
                                                  A13, A14, A15, A16, A17,
                                                  A18>::ListType>  Imp;

    typedef Variant18<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8,  A9,
                      A10, A11, A12, A13, A14, A15, A16, A17, A18> SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>              Traits;
    typedef bslmf::MovableRefUtil                                  MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant18,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant18,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant18,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant18, HasPrintMethod);

    // CREATORS
    Variant18();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant18(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant18(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant18(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                  &&
                  !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                          void>::type * = 0);
#else
    Variant18(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant18(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
              bslma::Allocator>::type *basicAllocator);
#else
    Variant18(bslmf::MovableRef<TYPE>  value,
              bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant18(const Variant18& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant18(bslmf::MovableRef<Variant18> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant18(bslmf::MovableRef<Variant18>  original,
              bslma::Allocator             *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant18& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant18&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant18& operator=(const Variant18& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant18& operator=(bslmf::MovableRef<Variant18> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

                       // ====================
                       // class Variant19<...>
                       // ====================

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
class Variant19 : public VariantImp<typename bslmf::TypeList19<
                                               A1,  A2,  A3,  A4,  A5,
                                               A6,  A7,  A8,  A9,  A10,
                                               A11, A12, A13, A14, A15,
                                               A16, A17, A18, A19>::ListType> {
    // This class is a "specialization" of 'Variant' for a fixed number (19) of
    // types.  Its 19 template arguments *must* all be specified (none are
    // defaulted to 'bslmf::Nil').  It provides the same functionality as
    // 'Variant<A1, A2, ..., A19>'.

    // PRIVATE TYPES
    typedef VariantImp<typename bslmf::TypeList19<A1,  A2,  A3,  A4,  A5,  A6,
                                                  A7,  A8,  A9,  A10, A11, A12,
                                                  A13, A14, A15, A16, A17, A18,
                                                  A19>::ListType> Imp;

    typedef Variant19<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8,  A9,
                      A10, A11, A12, A13, A14, A15, A16, A17, A18,
                      A19>                                        SelfType;
        // 'SelfType' is an alias to this class.

    typedef VariantImp_Traits<typename Imp::TypeList>             Traits;
    typedef bslmf::MovableRefUtil                                 MoveUtil;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant19,
                                      bslma::UsesBslmaAllocator,
                                      Traits::k_VARIANT_USES_BSLMA_ALLOCATOR);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant19,
                                      bsl::is_trivially_copyable,
                                      Traits::k_VARIANT_IS_BITWISE_COPYABLE);
    BSLMF_NESTED_TRAIT_DECLARATION_IF(Variant19,
                                      bslmf::IsBitwiseMoveable,
                                      Traits::k_VARIANT_IS_BITWISE_MOVEABLE);
    BSLMF_NESTED_TRAIT_DECLARATION(Variant19, HasPrintMethod);

    // CREATORS
    Variant19();
        // Create a variant object in the unset state that uses the currently
        // installed default allocator to supply memory.

    template <class TYPE_OR_ALLOCATOR>
    explicit
    Variant19(const TYPE_OR_ALLOCATOR& valueOrAllocator);
        // Create a variant object with the specified 'valueOrAllocator' that
        // can be either a value of a type that the variant can hold or an
        // allocator to supply memory.  If 'valueOrAllocator' is not a
        // 'bslma::Allocator *', then the variant will hold the value and type
        // of 'valueOrAllocator', and use the currently installed default
        // allocator to supply memory.  Otherwise, the variant will be unset
        // and use 'valueOrAllocator' to supply memory.  'TYPE_OR_ALLOCATOR'
        // must be the same as one of the types that this variant can hold or
        // be convertible to 'bslma::Allocator *'.  Note that this
        // parameterized constructor is defined instead of two constructors
        // (one taking a 'bslma::Allocator *' and the other not) because
        // template parameter arguments are always a better match than
        // derived-to-base conversion (a concrete allocator pointer converted
        // to 'bslma::Allocator *').

    template <class TYPE>
    Variant19(const TYPE& value, bslma::Allocator *basicAllocator);
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' and that uses the specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'TYPE' must be the same as one of the
        // types that this variant can hold.

    template <class TYPE>
    explicit
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant19(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
                  &&
                  !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                          void>::type * = 0);
#else
    Variant19(bslmf::MovableRef<TYPE>  value);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' by moving the contents of 'value' to the
        // newly-created object.  Use the currently installed default allocator
        // to supply memory.  'value' is left in a valid but unspecified state.
        // 'TYPE' must be the same as one of the types that this variant can
        // hold.  Note that in C++11 mode, this method does not participate in
        // overload resolution if it would lead to ambiguity with the move
        // constructor that does not take an allocator (below) or with the
        // constructor taking a 'valueOrAllocator' (above).

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Variant19(TYPE&&                   value,
              typename bsl::enable_if<
                  !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
              bslma::Allocator>::type *basicAllocator);
#else
    Variant19(bslmf::MovableRef<TYPE>  value,
              bslma::Allocator        *basicAllocator);
#endif
        // Create a variant object having the specified 'value' of template
        // parameter 'TYPE' that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'value' are moved to the
        // newly-created object with 'value' left in a valid but unspecified
        // state.  'TYPE' must be the same as one of the types that this
        // variant can hold.  Note that in C++11 mode, this method does not
        // participate in overload resolution if it would lead to ambiguity
        // with the move constructor that takes an allocator (below).

    Variant19(const Variant19& original, bslma::Allocator *basicAllocator = 0);
        // Create a variant object having the type and value of the specified
        // 'original' variant.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Variant19(bslmf::MovableRef<Variant19> original);
        // Create a variant object having the type and value of the specified
        // 'original' object by moving the contents of 'original' to the
        // newly-created object.  The allocator associated with 'original' (if
        // any) is propagated for use in the newly-created object.  'original'
        // is left in a valid but unspecified state.

    Variant19(bslmf::MovableRef<Variant19>  original,
              bslma::Allocator             *basicAllocator);
        // Create a variant object having the type and value of the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The contents of 'original' are moved to the
        // newly-created object with 'original' left in a valid but unspecified
        // state.

    // MANIPULATORS
    template <class TYPE>
    Variant19& operator=(const TYPE& value);
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The value currently held by this variant (if any) is
        // destroyed if that value's type is not the same as 'TYPE'.  'TYPE'
        // must be the same as one of the types that this variant can hold.

    template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    typename bsl::enable_if<
        !bsl::is_same<SelfType,
                      typename bsl::remove_reference<TYPE>::type>::value,
    SelfType>::type&
    operator=(TYPE&&                  value);
#else
    Variant19&
    operator=(bslmf::MovableRef<TYPE> value);
#endif
        // Assign to this object the specified 'value' of template parameter
        // 'TYPE', and return a reference providing modifiable access to this
        // object.  The contents of 'value' are moved to this object with
        // 'value' left in a valid but unspecified state.  The value currently
        // held by this variant (if any) is destroyed if that value's type is
        // not the same as 'TYPE'.  'TYPE' must be the same as one of the types
        // that this variant can hold.  Note that in C++11 mode, this method
        // does not participate in overload resolution if it would lead to
        // ambiguity with the move-assignment operator (below).

    Variant19& operator=(const Variant19& rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.

    Variant19& operator=(bslmf::MovableRef<Variant19> rhs);
        // Assign to this object the type and value currently held by the
        // specified 'rhs' object, and return a reference providing modifiable
        // access to this object.  The value currently held by this variant
        // (if any) is destroyed if that value's type is not the same as the
        // type held by the 'rhs' object.  The contents of 'rhs' are either
        // move-inserted into or move-assigned to this object with 'rhs' left
        // in a valid but unspecified state.
};

// ---- Anything below this line is implementation specific.  Do not use.  ----

               // =====================================
               // struct Variant_TypeIndex<TYPES, TYPE>
               // =====================================

template <class TYPES, class TYPE>
struct Variant_TypeIndex {
    // Component-private meta-function.  Do not use.  This meta-function
    // computes the index of the template parameter 'TYPE' in the template
    // parameter list of 'TYPES'.

    enum {
        value = bsl::is_same<
                      typename bslmf::TypeListTypeOf< 1, TYPES>::TypeOrDefault,
                      TYPE>::value ?  1
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf< 2, TYPES>::TypeOrDefault,
                      TYPE>::value ?  2
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf< 3, TYPES>::TypeOrDefault,
                      TYPE>::value ?  3
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf< 4, TYPES>::TypeOrDefault,
                      TYPE>::value ?  4
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf< 5, TYPES>::TypeOrDefault,
                      TYPE>::value ?  5
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf< 6, TYPES>::TypeOrDefault,
                      TYPE>::value ?  6
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf< 7, TYPES>::TypeOrDefault,
                      TYPE>::value ?  7
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf< 8, TYPES>::TypeOrDefault,
                      TYPE>::value ?  8
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf< 9, TYPES>::TypeOrDefault,
                      TYPE>::value ?  9
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<10, TYPES>::TypeOrDefault,
                      TYPE>::value ? 10
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<11, TYPES>::TypeOrDefault,
                      TYPE>::value ? 11
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<12, TYPES>::TypeOrDefault,
                      TYPE>::value ? 12
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<13, TYPES>::TypeOrDefault,
                      TYPE>::value ? 13
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<14, TYPES>::TypeOrDefault,
                      TYPE>::value ? 14
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<15, TYPES>::TypeOrDefault,
                      TYPE>::value ? 15
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<16, TYPES>::TypeOrDefault,
                      TYPE>::value ? 16
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<17, TYPES>::TypeOrDefault,
                      TYPE>::value ? 17
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<18, TYPES>::TypeOrDefault,
                      TYPE>::value ? 18
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<19, TYPES>::TypeOrDefault,
                      TYPE>::value ? 19
              : bsl::is_same<
                      typename bslmf::TypeListTypeOf<20, TYPES>::TypeOrDefault,
                      TYPE>::value ? 20
              : bsl::is_convertible<TYPE, bslma::Allocator *>::value
                                   ? 21
              : 0
    };

    BSLMF_ASSERT(0 != value);

#if defined(BDLB_VARIANT_USING_VARIADIC_TEMPLATES)
    // See 'testCase17' in the test driver for code snippets that motivate this
    // compile-time assertion (see the "out of bounds" comments).  In C++03,
    // 'TYPES::LENGTH' yields an incorrect value if 'TYPES' has a trailing
    // 'bslmf::Nil' (unless the variant is declared using 'VariantN').  It is
    // arguably dubious to allow 'bslmf::Nil' as one of the types that a
    // variant may hold, but there is such use in production code that needs to
    // be considered if that "feature" is to be suppressed.  This would do it:
    //..
    //  BSLMF_ASSERT((!bsl::is_same<TYPE, bslmf::Nil>::value));
    //..

    // TBD The following breaks compilation of some client code in C++11 mode
    // that is most likely misusing 'bslmf::Nil'.
    // BSLMF_ASSERT(((int)value <= (int)TYPES::LENGTH || 21 == value));
#endif
};

               // ======================================
               // struct Variant_DefaultConstructVisitor
               // ======================================

struct Variant_DefaultConstructVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some template parameter 'TYPE', will create a default
    // instance of 'TYPE'.

    // PUBLIC DATA
    bslma::Allocator *d_allocator_p;

    // CREATORS
    explicit
    Variant_DefaultConstructVisitor(bslma::Allocator *allocator)
    : d_allocator_p(allocator)
    {
    }

    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE& value) const
    {
        bslma::ConstructionUtil::construct(&value, d_allocator_p);
    }
};

                // ===================================
                // struct Variant_CopyConstructVisitor
                // ===================================

struct Variant_CopyConstructVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some template parameter 'TYPE', will copy-insert that
    // instance to create an instance of the same 'TYPE' in an uninitialized
    // buffer specified at construction of this visitor.

    // PUBLIC DATA
    void             *d_buffer_p;
    bslma::Allocator *d_allocator_p;

    // CREATORS
    Variant_CopyConstructVisitor(void *buffer, bslma::Allocator *allocator)
    : d_buffer_p(buffer)
    , d_allocator_p(allocator)
    {
        BSLS_ASSERT_SAFE(d_buffer_p);
    }

    // ACCESSORS
    template <class TYPE>
    void operator()(const TYPE& value) const
    {
        bslma::ConstructionUtil::construct(
                                          reinterpret_cast<TYPE *>(d_buffer_p),
                                          d_allocator_p,
                                          value);
    }
};

                // ===================================
                // struct Variant_MoveConstructVisitor
                // ===================================

struct Variant_MoveConstructVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some template parameter 'TYPE', will move-insert that
    // instance to create an instance of the same 'TYPE' in an uninitialized
    // buffer specified at construction of this visitor.

    // PUBLIC DATA
    void             *d_buffer_p;
    bslma::Allocator *d_allocator_p;

    // CREATORS
    Variant_MoveConstructVisitor(void *buffer, bslma::Allocator *allocator)
    : d_buffer_p(buffer)
    , d_allocator_p(allocator)
    {
        BSLS_ASSERT_SAFE(d_buffer_p);
    }

    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE& value) const
    {
        bslma::ConstructionUtil::construct(
                                          reinterpret_cast<TYPE *>(d_buffer_p),
                                          d_allocator_p,
                                          bslmf::MovableRefUtil::move(value));
    }
};

                  // ================================
                  // struct Variant_DestructorVisitor
                  // ================================

struct Variant_DestructorVisitor {
    // This visitor, when invoked as a function object on an instance of some
    // template parameter 'TYPE', will destroy that instance.

    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE& object) const
    {
        bslma::DestructionUtil::destroy(&object);
    }
};

                    // ================================
                    // struct Variant_CopyAssignVisitor
                    // ================================

struct Variant_CopyAssignVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some template parameter 'TYPE', will copy-assign that
    // instance to the instance of the same 'TYPE' held in a buffer specified
    // at construction of this visitor.

    // PUBLIC DATA
    void *d_buffer_p;

    // CREATORS
    explicit
    Variant_CopyAssignVisitor(void *buffer)
    : d_buffer_p(buffer)
    {
        BSLS_ASSERT_SAFE(d_buffer_p);
    }

    // ACCESSORS
    template <class TYPE>
    void operator()(const TYPE& value)
    {
        *reinterpret_cast<TYPE *>(d_buffer_p) = value;
    }
};

                    // ================================
                    // struct Variant_MoveAssignVisitor
                    // ================================

struct Variant_MoveAssignVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some template parameter 'TYPE', will move-assign that
    // instance to the instance of the same 'TYPE' held in a buffer specified
    // at construction of this visitor.

    // PUBLIC DATA
    void *d_buffer_p;

    // CREATORS
    explicit
    Variant_MoveAssignVisitor(void *buffer)
    : d_buffer_p(buffer)
    {
        BSLS_ASSERT_SAFE(d_buffer_p);
    }

    // ACCESSORS
    template <class TYPE>
    void operator()(TYPE& value)
    {
        *reinterpret_cast<TYPE *>(d_buffer_p) =
                                            bslmf::MovableRefUtil::move(value);
    }
};

                    // ==========================
                    // struct Variant_SwapVisitor
                    // ==========================

struct Variant_SwapVisitor {
    // This visitor swaps the variant object data that it holds with another
    // variant object data of parameterize 'TYPE'.  It requires that the two
    // variant objects being swapped contain data of the same type, and use the
    // same allocator.

    // PUBLIC DATA
    void *d_buffer_p;

    // CREATORS
    explicit
    Variant_SwapVisitor(void *buffer)
    : d_buffer_p(buffer)
    {
        BSLS_ASSERT_SAFE(d_buffer_p);
    }

    // MANIPULATORS
    template <class TYPE>
    void operator()(TYPE& value)
    {
        bslalg::SwapUtil::swap(reinterpret_cast<TYPE *>(d_buffer_p), &value);
    }
};

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

                 // ==================================
                 // struct Variant_BdexStreamInVisitor
                 // ==================================

template <class STREAM>
struct Variant_BdexStreamInVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // initialized instance of some parameterized 'TYPE', will stream in
    // a value of the same 'TYPE' into that instance from a stream specified at
    // construction of this visitor,  using a version also specified at
    // construction of this visitor.

    // PUBLIC DATA
    STREAM& d_stream;   // held, not owned
    int     d_version;  // BDEX version

    // CREATORS
    Variant_BdexStreamInVisitor(STREAM& stream, int version)
    : d_stream(stream)
    , d_version(version)
    {
    }

    // ACCESSORS
    template <class VALUETYPE>
    inline
    void operator()(VALUETYPE& object) const
    {
        bslx::InStreamFunctions::bdexStreamIn(d_stream, object, d_version);
    }

    inline
    void operator()(bslmf::Nil) const
    {
        // no op
    }
};

                // ===================================
                // struct Variant_BdexStreamOutVisitor
                // ===================================

template <class STREAM>
struct Variant_BdexStreamOutVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // initialized instance of some parameterized 'TYPE', will stream out the
    // value of that instance into a stream specified at construction of this
    // visitor, using a version also specified at construction of this
    // visitor.

    // PUBLIC DATA
    STREAM& d_stream;   // held, not owned
    int     d_version;  // BDEX version

    // CREATORS
    Variant_BdexStreamOutVisitor(STREAM& stream, int version)
    : d_stream(stream)
    , d_version(version)
    {
    }

    // ACCESSORS
    template <class VALUETYPE>
    inline
    void operator()(const VALUETYPE& object) const
    {
        bslx::OutStreamFunctions::bdexStreamOut(d_stream, object, d_version);
    }

    inline
    void operator()(bslmf::Nil) const
    {
        // no op
    }
};
#endif

                    // ===========================
                    // struct Variant_PrintVisitor
                    // ===========================

struct Variant_PrintVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some template parameter 'TYPE', writes the value of that
    // instance to a stream specified at construction of this visitor, using
    // spacing information also specified at construction.

    // PUBLIC DATA
    bsl::ostream *d_stream_p;        // held, not owned
    int           d_level;
    int           d_spacesPerLevel;

    // CREATORS
    Variant_PrintVisitor(bsl::ostream *stream,
                         int           level,
                         int           spacesPerLevel)
    : d_stream_p(stream)
    , d_level(level)
    , d_spacesPerLevel(spacesPerLevel)
    {
        BSLS_ASSERT_SAFE(d_stream_p);
    }

    // ACCESSORS
    template <class TYPE>
    void operator()(const TYPE& value) const
    {
        PrintMethods::print(*d_stream_p, value, d_level, d_spacesPerLevel);
    }

    void operator()(bslmf::Nil) const
    {
        // no op
    }
};

                 // ==================================
                 // struct Variant_EqualityTestVisitor
                 // ==================================

struct Variant_EqualityTestVisitor {
    // This visitor, when invoked as a non-modifiable function object on an
    // instance of some template parameter 'TYPE', tests the equality of the
    // value of that instance and of another instance held in a buffer
    // specified at construction of this visitor, and stores the result into
    // its publicly accessible 'd_result' member.

    // PUBLIC DATA
    mutable bool  d_result;
    const void   *d_buffer_p;  // held, not owned

    // CREATORS
    explicit
    Variant_EqualityTestVisitor(const void *buffer)
    : d_result(true)
    , d_buffer_p(buffer)
    {
        BSLS_ASSERT_SAFE(d_buffer_p);
    }

    // ACCESSORS
    template <class TYPE>
    void operator()(const TYPE& value) const
    {
        d_result = *reinterpret_cast<const TYPE *>(d_buffer_p) == value;
    }

    void operator()(bslmf::Nil) const
    {
        d_result = true;
    }
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                   // -------------------------------------
                   // class VariantImp_AllocatorBase<TYPES>
                   // -------------------------------------

// CREATORS
template <class TYPES>
inline
VariantImp_AllocatorBase<TYPES>::
VariantImp_AllocatorBase(int type, bslma::Allocator *basicAllocator)
: d_type(type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TYPES>
inline
VariantImp_AllocatorBase<TYPES>::
VariantImp_AllocatorBase(int, bslma::Allocator *basicAllocator, bsl::true_type)
: d_type(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TYPES>
template <class TYPE>
inline
VariantImp_AllocatorBase<TYPES>::
VariantImp_AllocatorBase(int type, const TYPE&, bsl::false_type)
: d_type(type)
, d_allocator_p(bslma::Default::allocator(0))
{
}

// ACCESSORS
template <class TYPES>
inline
bslma::Allocator *
VariantImp_AllocatorBase<TYPES>::getAllocator() const
{
    return d_allocator_p;
}

                  // ---------------------------------------
                  // class VariantImp_NoAllocatorBase<TYPES>
                  // ---------------------------------------

// CREATORS
template <class TYPES>
inline
VariantImp_NoAllocatorBase<TYPES>::
VariantImp_NoAllocatorBase(int type, bslma::Allocator *)
: d_type(type)
{
}

template <class TYPES>
inline
VariantImp_NoAllocatorBase<TYPES>::
VariantImp_NoAllocatorBase(int, bslma::Allocator *, bsl::true_type)
: d_type(0)
{
}

template <class TYPES>
template <class TYPE>
inline
VariantImp_NoAllocatorBase<TYPES>::
VariantImp_NoAllocatorBase(int type, const TYPE&, bsl::false_type)
: d_type(type)
{
}

// ACCESSORS
template <class TYPES>
inline
bslma::Allocator *
VariantImp_NoAllocatorBase<TYPES>::getAllocator() const
{
    return 0;
}

                       // -------------------------------
                       // class Variant_ReturnAnyTypeUtil
                       // -------------------------------

// CLASS METHODS
template <class TYPE>
inline
TYPE Variant_ReturnAnyTypeUtil<TYPE>::doNotCall(TYPE *dummy)
{
    return *dummy;
}

template <class TYPE>
inline
TYPE Variant_ReturnAnyTypeUtil<TYPE>::doNotCall()
{
    // Note that IBM xlC requires that we explicitly declare a temporary here,
    // rather than cast the null pointer directly as a function argument.

    typedef typename bsl::remove_reference<TYPE>::type UnrefType;

    UnrefType *const ptr = 0;
    return doNotCall(ptr);
}

inline
void Variant_ReturnAnyTypeUtil<void>::doNotCall()
{}

template <class TYPE>
inline
TYPE& Variant_ReturnAnyTypeUtil<TYPE&>::lvalueRef(TYPE *dummy)
{
    return *dummy;
}

template <class TYPE>
inline
TYPE& Variant_ReturnAnyTypeUtil<TYPE&>::doNotCall()
{
    TYPE *const ptr = 0;
    return lvalueRef(ptr);
}

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class TYPE>
inline
TYPE&& Variant_ReturnAnyTypeUtil<TYPE&&>::rvalueRef(TYPE *dummy)
{
    return bslmf::MovableRefUtil::move(*dummy);
}

template <class TYPE>
inline
TYPE&& Variant_ReturnAnyTypeUtil<TYPE&&>::doNotCall()
{
    TYPE *const ptr = 0;
    return rvalueRef(ptr);
}
#endif

                       // ------------------------------
                       // class Variant_RawVisitorHelper
                       // ------------------------------

// CREATORS
template <class RESULT_TYPE, class VISITOR>
inline
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::
Variant_RawVisitorHelper(VISITOR *visitor)
: d_visitor(visitor)
{
    BSLS_ASSERT_SAFE(0 != visitor);
}

// MANIPULATORS
template <class RESULT_TYPE, class VISITOR>
template <class ARGUMENT_TYPE>
inline
RESULT_TYPE
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::operator()(
                                                       ARGUMENT_TYPE& argument)
{
    return static_cast<RESULT_TYPE>((*d_visitor)(argument));
}

template <class RESULT_TYPE, class VISITOR>
template <class ARGUMENT_TYPE>
inline
RESULT_TYPE
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::operator()(
                                                 const ARGUMENT_TYPE& argument)
{
    return static_cast<RESULT_TYPE>((*d_visitor)(argument));
}

// ACCESSORS
template <class RESULT_TYPE, class VISITOR>
template <class ARGUMENT_TYPE>
inline
RESULT_TYPE
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::operator()(
                                                 ARGUMENT_TYPE& argument) const
{
    return static_cast<RESULT_TYPE>((*d_visitor)(argument));
}

template <class RESULT_TYPE, class VISITOR>
template <class ARGUMENT_TYPE>
inline
RESULT_TYPE
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::operator()(
                                           const ARGUMENT_TYPE& argument) const
{
    return static_cast<RESULT_TYPE>((*d_visitor)(argument));
}

template <class RESULT_TYPE, class VISITOR>
RESULT_TYPE
Variant_RawVisitorHelper<RESULT_TYPE, VISITOR>::operator()(bslmf::Nil) const
{
    BSLS_ASSERT_OPT(false);

    return Variant_ReturnAnyTypeUtil<RESULT_TYPE>::doNotCall();
}

                        // -----------------------
                        // class VariantImp<TYPES>
                        // -----------------------

// PRIVATE MANIPULATORS
template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF visitor, bsl::false_type)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    visitor(reinterpret_cast<BufferType *>(&this->d_value)->object());
}

template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF visitor, bsl::true_type)
{
    bslmf::Nil nil = bslmf::Nil();
    visitor(nil);
}

template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF visitor)
{
    typedef typename bsl::is_same<TYPE, bslmf::Nil>::type IsUnset;
    applyImp<TYPE, VISITOR_REF>(visitor, IsUnset());
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF visitor, bsl::false_type)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    return visitor(reinterpret_cast<BufferType *>(&this->d_value)->object());
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF visitor, bsl::true_type)
{
    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF visitor)
{
    typedef typename bsl::is_same<TYPE, bslmf::Nil>::type IsUnset;
    return applyImpR<TYPE, VISITOR_REF, RET_TYPE>(visitor, IsUnset());
}

template <class TYPES>
template <class TYPE, class SOURCE_TYPE>
void VariantImp<TYPES>::assignImp(const SOURCE_TYPE& value)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    reset();
    bslma::ConstructionUtil::construct(
                     reinterpret_cast<BufferType *>(&this->d_value)->address(),
                     this->getAllocator(),
                     value);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;
}

template <class TYPES>
template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
void VariantImp<TYPES>::assignImp(TYPE&&                  value)
#else
void VariantImp<TYPES>::assignImp(bslmf::MovableRef<TYPE> value)
#endif
{
    typedef bsls::ObjectBuffer<typename bsl::remove_reference<TYPE>::type>
                                                                    BufferType;

    reset();

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    bslma::ConstructionUtil::construct(
                     reinterpret_cast<BufferType *>(&this->d_value)->address(),
                     this->getAllocator(),
                     BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    bslma::ConstructionUtil::construct(
                     reinterpret_cast<BufferType *>(&this->d_value)->address(),
                     this->getAllocator(),
                     bslmf::MovableRefUtil::move(lvalue));
#endif

    this->d_type =
          Variant_TypeIndex<TYPES,
                            typename bsl::remove_reference<TYPE>::type>::value;
}

template <class TYPES>
template <class VISITOR_REF>
void VariantImp<TYPES>::doApply(VISITOR_REF visitor, int type)
{
    switch (type) {
      case 0: {
        BSLS_ASSERT(!"'doApply' invoked on an unset variant");
      } break;
      case 1: {
        applyImp<typename Base::Type1, VISITOR_REF>(visitor);
      } break;
      case 2: {
        applyImp<typename Base::Type2, VISITOR_REF>(visitor);
      } break;
      case 3: {
        applyImp<typename Base::Type3, VISITOR_REF>(visitor);
      } break;
      case 4: {
        applyImp<typename Base::Type4, VISITOR_REF>(visitor);
      } break;
      case 5: {
        applyImp<typename Base::Type5, VISITOR_REF>(visitor);
      } break;
      case 6: {
        applyImp<typename Base::Type6, VISITOR_REF>(visitor);
      } break;
      case 7: {
        applyImp<typename Base::Type7, VISITOR_REF>(visitor);
      } break;
      case 8: {
        applyImp<typename Base::Type8, VISITOR_REF>(visitor);
      } break;
      case 9: {
        applyImp<typename Base::Type9, VISITOR_REF>(visitor);
      } break;
      case 10: {
        applyImp<typename Base::Type10, VISITOR_REF>(visitor);
      } break;
      case 11: {
        applyImp<typename Base::Type11, VISITOR_REF>(visitor);
      } break;
      case 12: {
        applyImp<typename Base::Type12, VISITOR_REF>(visitor);
      } break;
      case 13: {
        applyImp<typename Base::Type13, VISITOR_REF>(visitor);
      } break;
      case 14: {
        applyImp<typename Base::Type14, VISITOR_REF>(visitor);
      } break;
      case 15: {
        applyImp<typename Base::Type15, VISITOR_REF>(visitor);
      } break;
      case 16: {
        applyImp<typename Base::Type16, VISITOR_REF>(visitor);
      } break;
      case 17: {
        applyImp<typename Base::Type17, VISITOR_REF>(visitor);
      } break;
      case 18: {
        applyImp<typename Base::Type18, VISITOR_REF>(visitor);
      } break;
      case 19: {
        applyImp<typename Base::Type19, VISITOR_REF>(visitor);
      } break;
      case 20: {
        applyImp<typename Base::Type20, VISITOR_REF>(visitor);
      } break;
      default: {
        BSLS_ASSERT(!"Unreachable by design!");
      } break;
    }
}

template <class TYPES>
template <class TYPE>
inline
void VariantImp<TYPES>::create(const TYPE& value, bsl::false_type)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    bslma::ConstructionUtil::construct(
                     reinterpret_cast<BufferType *>(&this->d_value)->address(),
                     this->getAllocator(),
                     value);
}

template <class TYPES>
inline
void VariantImp<TYPES>::create(bslma::Allocator *, bsl::true_type)
{
}

template <class TYPES>
template <class VISITOR_REF, class RET_TYPE>
RET_TYPE VariantImp<TYPES>::doApplyR(VISITOR_REF visitor, int type)
{
    switch (type) {
      case 0: {
        BSLS_ASSERT(!"'doApplyR' invoked on an unset variant");
      } break;
      case 1: {
        return applyImpR<typename Base::Type1,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 2: {
        return applyImpR<typename Base::Type2,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 3: {
        return applyImpR<typename Base::Type3,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 4: {
        return applyImpR<typename Base::Type4,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 5: {
        return applyImpR<typename Base::Type5,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 6: {
        return applyImpR<typename Base::Type6,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 7: {
        return applyImpR<typename Base::Type7,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 8: {
        return applyImpR<typename Base::Type8,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 9: {
        return applyImpR<typename Base::Type9,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 10: {
        return applyImpR<typename Base::Type10,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 11: {
        return applyImpR<typename Base::Type11,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 12: {
        return applyImpR<typename Base::Type12,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 13: {
        return applyImpR<typename Base::Type13,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 14: {
        return applyImpR<typename Base::Type14,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 15: {
        return applyImpR<typename Base::Type15,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 16: {
        return applyImpR<typename Base::Type16,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 17: {
        return applyImpR<typename Base::Type17,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 18: {
        return applyImpR<typename Base::Type18,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 19: {
        return applyImpR<typename Base::Type19,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 20: {
        return applyImpR<typename Base::Type20,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      default: {
        BSLS_ASSERT(!"Unreachable by design!");
      } break;
    }

    // Unreachable by design; return something to quiet compiler warnings.

    return Variant_ReturnAnyTypeUtil<RET_TYPE>::doNotCall();
}

// PRIVATE ACCESSORS
template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF visitor, bsl::false_type) const
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    visitor(reinterpret_cast<const BufferType *>(&this->d_value)->object());
}

template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF visitor, bsl::true_type) const
{
    bslmf::Nil nil = bslmf::Nil();
    visitor(nil);
}

template <class TYPES>
template <class TYPE, class VISITOR_REF>
inline
void VariantImp<TYPES>::applyImp(VISITOR_REF visitor) const
{
    typedef typename bsl::is_same<TYPE, bslmf::Nil>::type IsUnset;
    applyImp<TYPE, VISITOR_REF>(visitor, IsUnset());
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF     visitor,
                                      bsl::false_type) const
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    return visitor(reinterpret_cast<const BufferType *>(
                                                    &this->d_value)->object());
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF visitor,
                                      bsl::true_type) const
{
    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class TYPE, class VISITOR_REF, class RET_TYPE>
inline
RET_TYPE VariantImp<TYPES>::applyImpR(VISITOR_REF visitor) const
{
    typedef typename bsl::is_same<TYPE, bslmf::Nil>::type IsUnset;
    return applyImpR<TYPE, VISITOR_REF, RET_TYPE>(visitor, IsUnset());
}

template <class TYPES>
template <class VISITOR_REF>
void VariantImp<TYPES>::doApply(VISITOR_REF visitor, int type) const
{
    switch (type) {
      case 0: {
        BSLS_ASSERT(!"'doApply' invoked on an unset variant");
      } break;
      case 1: {
        applyImp<typename Base::Type1, VISITOR_REF>(visitor);
      } break;
      case 2: {
        applyImp<typename Base::Type2, VISITOR_REF>(visitor);
      } break;
      case 3: {
        applyImp<typename Base::Type3, VISITOR_REF>(visitor);
      } break;
      case 4: {
        applyImp<typename Base::Type4, VISITOR_REF>(visitor);
      } break;
      case 5: {
        applyImp<typename Base::Type5, VISITOR_REF>(visitor);
      } break;
      case 6: {
        applyImp<typename Base::Type6, VISITOR_REF>(visitor);
      } break;
      case 7: {
        applyImp<typename Base::Type7, VISITOR_REF>(visitor);
      } break;
      case 8: {
        applyImp<typename Base::Type8, VISITOR_REF>(visitor);
      } break;
      case 9: {
        applyImp<typename Base::Type9, VISITOR_REF>(visitor);
      } break;
      case 10: {
        applyImp<typename Base::Type10, VISITOR_REF>(visitor);
      } break;
      case 11: {
        applyImp<typename Base::Type11, VISITOR_REF>(visitor);
      } break;
      case 12: {
        applyImp<typename Base::Type12, VISITOR_REF>(visitor);
      } break;
      case 13: {
        applyImp<typename Base::Type13, VISITOR_REF>(visitor);
      } break;
      case 14: {
        applyImp<typename Base::Type14, VISITOR_REF>(visitor);
      } break;
      case 15: {
        applyImp<typename Base::Type15, VISITOR_REF>(visitor);
      } break;
      case 16: {
        applyImp<typename Base::Type16, VISITOR_REF>(visitor);
      } break;
      case 17: {
        applyImp<typename Base::Type17, VISITOR_REF>(visitor);
      } break;
      case 18: {
        applyImp<typename Base::Type18, VISITOR_REF>(visitor);
      } break;
      case 19: {
        applyImp<typename Base::Type19, VISITOR_REF>(visitor);
      } break;
      case 20: {
        applyImp<typename Base::Type20, VISITOR_REF>(visitor);
      } break;
      default: {
        BSLS_ASSERT(!"Unreachable by design!");
      } break;
    }
}

template <class TYPES>
template <class VISITOR_REF, class RET_TYPE>
RET_TYPE VariantImp<TYPES>::doApplyR(VISITOR_REF visitor, int type) const
{
    switch (type) {
      case 0: {
        BSLS_ASSERT(!"'doApplyR' invoked on an unset variant");
      } break;
      case 1: {
        return applyImpR<typename Base::Type1,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 2: {
        return applyImpR<typename Base::Type2,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 3: {
        return applyImpR<typename Base::Type3,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 4: {
        return applyImpR<typename Base::Type4,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 5: {
        return applyImpR<typename Base::Type5,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 6: {
        return applyImpR<typename Base::Type6,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 7: {
        return applyImpR<typename Base::Type7,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 8: {
        return applyImpR<typename Base::Type8,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 9: {
        return applyImpR<typename Base::Type9,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 10: {
        return applyImpR<typename Base::Type10,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 11: {
        return applyImpR<typename Base::Type11,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 12: {
        return applyImpR<typename Base::Type12,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 13: {
        return applyImpR<typename Base::Type13,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 14: {
        return applyImpR<typename Base::Type14,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 15: {
        return applyImpR<typename Base::Type15,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 16: {
        return applyImpR<typename Base::Type16,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 17: {
        return applyImpR<typename Base::Type17,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 18: {
        return applyImpR<typename Base::Type18,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 19: {
        return applyImpR<typename Base::Type19,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      case 20: {
        return applyImpR<typename Base::Type20,
                         VISITOR_REF,
                         RET_TYPE>(visitor);                          // RETURN
      } break;
      default: {
        BSLS_ASSERT(!"Unreachable by design!");
      } break;
    }

    // Unreachable by design; return something to quiet compiler warnings.

    return Variant_ReturnAnyTypeUtil<RET_TYPE>::doNotCall();
}

// CREATORS
template <class TYPES>
inline
VariantImp<TYPES>::VariantImp()
: Base(0, 0)
{
}

template <class TYPES>
template <class TYPE_OR_ALLOCATOR>
inline
VariantImp<TYPES>::VariantImp(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Base(Variant_TypeIndex<TYPES, TYPE_OR_ALLOCATOR>::value,
       valueOrAllocator,
       bsl::integral_constant<bool, bsl::is_convertible<TYPE_OR_ALLOCATOR,
                                                 bslma::Allocator *>::value>())
{
    enum {
        k_IS_ALLOCATOR = bsl::is_convertible<TYPE_OR_ALLOCATOR,
                                             bslma::Allocator *>::value
    };

    create(valueOrAllocator, bsl::integral_constant<bool, k_IS_ALLOCATOR>());
}

template <class TYPES>
template <class TYPE>
inline
VariantImp<TYPES>::VariantImp(const TYPE&       value,
                              bslma::Allocator *basicAllocator)
: Base(Variant_TypeIndex<TYPES, TYPE>::value, basicAllocator)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    bslma::ConstructionUtil::construct(
                     reinterpret_cast<BufferType *>(&this->d_value)->address(),
                     this->getAllocator(),
                     value);
}

template <class TYPES>
template <class TYPE>
VariantImp<TYPES>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
VariantImp(TYPE&&                   value,
           typename bsl::enable_if<
               !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
               &&
               !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                       void>::type *)
#else
VariantImp(bslmf::MovableRef<TYPE>  value)
#endif
: Base(Variant_TypeIndex<
                     TYPES,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
       0)
{
    typedef bsls::ObjectBuffer<
                 typename bsl::remove_const<
                 typename bsl::remove_reference<TYPE>::type>::type> BufferType;

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    bslma::ConstructionUtil::construct(
                 reinterpret_cast<BufferType *>(&this->d_value)->address(),
                 this->getAllocator(),
                 BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    bslma::ConstructionUtil::construct(
                 reinterpret_cast<BufferType *>(&this->d_value)->address(),
                 this->getAllocator(),
                 bslmf::MovableRefUtil::move(lvalue));
#endif
}

template <class TYPES>
template <class TYPE>
VariantImp<TYPES>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
VariantImp(TYPE&&                   value,
           typename bsl::enable_if<
               !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
           bslma::Allocator>::type *basicAllocator)
#else
VariantImp(bslmf::MovableRef<TYPE>  value,
           bslma::Allocator        *basicAllocator)
#endif
: Base(Variant_TypeIndex<
                     TYPES,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
       basicAllocator)
{
    typedef bsls::ObjectBuffer<
                 typename bsl::remove_const<
                 typename bsl::remove_reference<TYPE>::type>::type> BufferType;

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    bslma::ConstructionUtil::construct(
                     reinterpret_cast<BufferType *>(&this->d_value)->address(),
                     this->getAllocator(),
                     BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    bslma::ConstructionUtil::construct(
                     reinterpret_cast<BufferType *>(&this->d_value)->address(),
                     this->getAllocator(),
                     bslmf::MovableRefUtil::move(lvalue));
#endif
}

template <class TYPES>
VariantImp<TYPES>::VariantImp(const VariantImp&  original,
                              bslma::Allocator  *basicAllocator)
: Base(original.d_type, basicAllocator)
{
    if (this->d_type) {
        Variant_CopyConstructVisitor copyConstructor(&this->d_value,
                                                     this->getAllocator());
        original.apply(copyConstructor);
    }
}

template <class TYPES>
VariantImp<TYPES>::VariantImp(bslmf::MovableRef<VariantImp> original)
: Base(MoveUtil::access(original).d_type,
       MoveUtil::access(original).getAllocator())
{
    if (this->d_type) {
        Variant_MoveConstructVisitor moveConstructor(&this->d_value,
                                                     this->getAllocator());
        VariantImp& lvalue = original;

        lvalue.apply(moveConstructor);
    }
}

template <class TYPES>
VariantImp<TYPES>::VariantImp(bslmf::MovableRef<VariantImp>  original,
                              bslma::Allocator              *basicAllocator)
: Base(MoveUtil::access(original).d_type, basicAllocator)
{
    if (this->d_type) {
        Variant_MoveConstructVisitor moveConstructor(&this->d_value,
                                                     this->getAllocator());
        VariantImp& lvalue = original;

        lvalue.apply(moveConstructor);
    }
}

template <class TYPES>
inline
VariantImp<TYPES>::~VariantImp()
{
    reset();
}

// MANIPULATORS
template <class TYPES>
template <class TYPE>
inline
VariantImp<TYPES>&
VariantImp<TYPES>::operator=(const TYPE& value)
{
    return assign(value);
}

template <class TYPES>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<VariantImp<TYPES>,
                  typename bsl::remove_reference<TYPE>::type>::value,
VariantImp<TYPES> >::type&
VariantImp<TYPES>::operator=(TYPE&&                  value)
#else
VariantImp<TYPES>&
VariantImp<TYPES>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    return assign(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    return assign(MoveUtil::move(lvalue));
#endif
}

template <class TYPES>
VariantImp<TYPES>&
VariantImp<TYPES>::operator=(const VariantImp& rhs)
{
    if (&rhs != this) {
        if (this->d_type == rhs.d_type) {
            if (this->d_type) {
                Variant_CopyAssignVisitor assigner(&this->d_value);
                rhs.apply(assigner);
            }
        }
        else {
            reset();
            if (rhs.d_type) {
                Variant_CopyConstructVisitor copyConstructor(
                                                         &this->d_value,
                                                         this->getAllocator());
                rhs.apply(copyConstructor);
                this->d_type = rhs.d_type;
            }
        }
    }

    return *this;
}

template <class TYPES>
VariantImp<TYPES>&
VariantImp<TYPES>::operator=(bslmf::MovableRef<VariantImp> rhs)
{
    VariantImp& lvalue = rhs;

    if (&lvalue != this) {
        if (this->d_type == lvalue.d_type) {
            if (this->d_type) {
                Variant_MoveAssignVisitor assigner(&this->d_value);
                lvalue.apply(assigner);
            }
        }
        else {
            reset();
            if (lvalue.d_type) {
                Variant_MoveConstructVisitor moveConstructor(
                                                         &this->d_value,
                                                         this->getAllocator());
                lvalue.apply(moveConstructor);
                this->d_type = lvalue.d_type;
            }
        }
    }

    return *this;
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::apply(VISITOR& visitor)
{
    if (this->d_type) {
        return doApplyR<VISITOR&, RET_TYPE>(visitor, this->d_type);   // RETURN
    }

    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::apply(const VISITOR& visitor)
{
    if (this->d_type) {
        return doApplyR<const VISITOR&, RET_TYPE>(visitor, this->d_type);
                                                                      // RETURN
    }

    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR, class TYPE>
inline
RET_TYPE VariantImp<TYPES>::apply(VISITOR& visitor, const TYPE& defaultValue)
{
    if (this->d_type) {
        return doApplyR<VISITOR&, RET_TYPE>(visitor, this->d_type);   // RETURN
    }

    return visitor(defaultValue);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR, class TYPE>
inline
RET_TYPE VariantImp<TYPES>::apply(const VISITOR& visitor,
                                  const TYPE&    defaultValue)
{
    if (this->d_type) {
        return doApplyR<const VISITOR&, RET_TYPE>(visitor, this->d_type);
                                                                      // RETURN
    }

    return visitor(defaultValue);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::applyRaw(VISITOR& visitor)
{
    typedef Variant_RawVisitorHelper<RET_TYPE, VISITOR> Helper;

    return doApplyR<const Helper&, RET_TYPE>(Helper(&visitor), this->d_type);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::applyRaw(const VISITOR& visitor)
{
    typedef Variant_RawVisitorHelper<RET_TYPE, const VISITOR> Helper;

    return doApplyR<const Helper&, RET_TYPE>(Helper(&visitor), this->d_type);
}

template <class TYPES>
template <class TYPE>
VariantImp<TYPES>& VariantImp<TYPES>::assign(const TYPE& value)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    if (Variant_TypeIndex<TYPES, TYPE>::value == this->d_type) {
        reinterpret_cast<BufferType *>(&this->d_value)->object() = value;
    }
    else {
        assignImp<TYPE, TYPE>(value);
    }

    return *this;
}

template <class TYPES>
template <class TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
VariantImp<TYPES>& VariantImp<TYPES>::assign(TYPE&&                  value)
#else
VariantImp<TYPES>& VariantImp<TYPES>::assign(bslmf::MovableRef<TYPE> value)
#endif
{
    typedef bsls::ObjectBuffer<typename bsl::remove_reference<TYPE>::type>
                                                                    BufferType;

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    if (Variant_TypeIndex<TYPES,
                          typename bsl::remove_reference<TYPE>::type>::value
                                                             == this->d_type) {
        reinterpret_cast<BufferType *>(&this->d_value)->object() =
                                    BSLS_COMPILERFEATURES_FORWARD(TYPE, value);
    }
    else {
        assignImp<TYPE>(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
    }
#else
    TYPE& lvalue = value;

    if (Variant_TypeIndex<TYPES, TYPE>::value == this->d_type) {
        reinterpret_cast<BufferType *>(&this->d_value)->object() =
                                                        MoveUtil::move(lvalue);
    }
    else {
        assignImp<TYPE>(MoveUtil::move(lvalue));
    }
#endif

    return *this;
}

template <class TYPES>
template <class TYPE, class SOURCE_TYPE>
VariantImp<TYPES>& VariantImp<TYPES>::assignTo(const SOURCE_TYPE& value)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;

    if (Variant_TypeIndex<TYPES, TYPE>::value == this->d_type
     && bsl::is_same<TYPE, SOURCE_TYPE>::value) {
        reinterpret_cast<BufferType *>(&this->d_value)->object() = value;
    }
    else {
        assignImp<TYPE, SOURCE_TYPE>(value);
    }

    return *this;
}

template <class TYPES>
template <class TYPE>
inline
TYPE& VariantImp<TYPES>::createInPlace()
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator());
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1>
inline
TYPE& VariantImp<TYPES>::createInPlace(const A1& a1)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1, class A2>
inline
TYPE& VariantImp<TYPES>::createInPlace(const A1& a1, const A2& a2)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                                      const A1& a1, const A2& a2, const A3& a3)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4, a5);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4, a5, a6);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4, a5, a6, a7);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4, a5, a6, a7, a8);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                        const A1& a1, const A2& a2, const A3& a3, const A4& a4,
                        const A5& a5, const A6& a6, const A7& a7, const A8& a8,
                        const A9& a9)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4, a5, a6, a7, a8, a9);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                       const A1& a1, const A2&  a2, const A3& a3, const A4& a4,
                       const A5& a5, const A6&  a6, const A7& a7, const A8& a8,
                       const A9& a9, const A10& a10)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4, a5, a6, a7, a8, a9,
                                                                          a10);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1, class A2, class A3, class A4, class A5,
                      class A6, class A7, class A8, class A9, class A10,
                      class A11>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                     const A1& a1, const A2&  a2,  const A3&  a3, const A4& a4,
                     const A5& a5, const A6&  a6,  const A7&  a7, const A8& a8,
                     const A9& a9, const A10& a10, const A11& a11)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
                                                                          a11);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1,  class A2,  class A3, class A4, class A5,
                      class A6,  class A7,  class A8, class A9, class A10,
                      class A11, class A12>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                    const A1&  a1, const A2&  a2,  const A3&  a3, const A4& a4,
                    const A5&  a5, const A6&  a6,  const A7&  a7, const A8& a8,
                    const A9&  a9, const A10& a10, const A11& a11,
                    const A12& a12)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
                                                                     a11, a12);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1,  class A2,  class A3,  class A4, class A5,
                      class A6,  class A7,  class A8,  class A9, class A10,
                      class A11, class A12, class A13>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                   const A1&  a1,  const A2&  a2,  const A3&  a3, const A4& a4,
                   const A5&  a5,  const A6&  a6,  const A7&  a7, const A8& a8,
                   const A9&  a9,  const A10& a10, const A11& a11,
                   const A12& a12, const A13& a13)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
                                                                a11, a12, a13);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
template <class TYPE, class A1,  class A2,  class A3,  class A4,  class A5,
                      class A6,  class A7,  class A8,  class A9,  class A10,
                      class A11, class A12, class A13, class A14>
inline
TYPE& VariantImp<TYPES>::createInPlace(
                   const A1&  a1,  const A2&  a2,  const A3&  a3, const A4& a4,
                   const A5&  a5,  const A6&  a6,  const A7&  a7, const A8& a8,
                   const A9&  a9,  const A10& a10, const A11& a11,
                   const A12& a12, const A13& a13, const A14& a14)
{
    typedef bsls::ObjectBuffer<TYPE> BufferType;
    BufferType& bufR = *reinterpret_cast<BufferType *>(&this->d_value);

    reset();
    bslma::ConstructionUtil::construct(bufR.address(),
                                       this->getAllocator(),
                                       a1, a2, a3, a4, a5, a6, a7, a8, a9, a10,
                                                           a11, a12, a13, a14);
    this->d_type = Variant_TypeIndex<TYPES, TYPE>::value;

    return bufR.object();
}

template <class TYPES>
void VariantImp<TYPES>::reset()
{
    if (this->d_type) {
        Variant_DestructorVisitor destructor;
        apply(destructor);
        this->d_type = 0;
    }
}

template <class TYPES>
void VariantImp<TYPES>::swap(VariantImp<TYPES>& other)
{
    if (!this->d_type) {
        if (!other.d_type) {
            return;                                                   // RETURN
        }
        *this = other;
        other.reset();
    }
    else if (!other.d_type) {
        other = *this;
        this->reset();
    }
    else {
        if (this->d_type         == other.d_type
         && this->getAllocator() == other.getAllocator()) {
            // Same types and same allocators, so use a visitor that calls
            // 'swap'.

            Variant_SwapVisitor swapper(&this->d_value);
            other.apply(swapper);
        }
        else {
            // Different types and/or allocators, so swap via assignment.  Note
            // that this doesn't fit the usual pattern for calling 'swap'
            // because infinite recursion would result.

            bsl::swap(*this, other);
        }
    }
}

template <class TYPES>
template <class TYPE>
inline
TYPE& VariantImp<TYPES>::the()
{
    BSLMF_ASSERT((0 != Variant_TypeIndex<TYPES, TYPE>::value));
    BSLS_ASSERT_SAFE((this->d_type == Variant_TypeIndex<TYPES, TYPE>::value));

    typedef bsls::ObjectBuffer<TYPE> BufferType;

    return reinterpret_cast<BufferType *>(&this->d_value)->object();
}

// ACCESSORS
template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::apply(VISITOR& visitor) const
{
    if (this->d_type) {
        return doApplyR<VISITOR&, RET_TYPE>(visitor, this->d_type);   // RETURN
    }

    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::apply(const VISITOR& visitor) const
{
    if (this->d_type) {
        return doApplyR<const VISITOR&, RET_TYPE>(visitor, this->d_type);
                                                                      // RETURN
    }

    bslmf::Nil nil = bslmf::Nil();
    return visitor(nil);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR, class TYPE>
inline
RET_TYPE VariantImp<TYPES>::apply(VISITOR&    visitor,
                                  const TYPE& defaultValue) const
{
    if (this->d_type) {
        return doApplyR<VISITOR&, RET_TYPE>(visitor, this->d_type);   // RETURN
    }

    return visitor(defaultValue);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR, class TYPE>
inline
RET_TYPE VariantImp<TYPES>::apply(const VISITOR& visitor,
                                  const TYPE&    defaultValue) const
{
    if (this->d_type) {
        return doApplyR<const VISITOR&, RET_TYPE>(visitor, this->d_type);
                                                                      // RETURN
    }

    return visitor(defaultValue);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::applyRaw(VISITOR& visitor) const
{
    typedef Variant_RawVisitorHelper<RET_TYPE, VISITOR> Helper;

    return doApplyR<const Helper&, RET_TYPE>(Helper(&visitor), this->d_type);
}

template <class TYPES>
template <class RET_TYPE, class VISITOR>
inline
RET_TYPE VariantImp<TYPES>::applyRaw(const VISITOR& visitor) const
{
    typedef Variant_RawVisitorHelper<RET_TYPE, const VISITOR> Helper;

    return doApplyR<const Helper&, RET_TYPE>(Helper(&visitor), this->d_type);
}

template <class TYPES>
template <class TYPE>
inline
bool VariantImp<TYPES>::is() const
{
    return Variant_TypeIndex<TYPES, TYPE>::value == this->d_type;
}

template <class TYPES>
inline
bool VariantImp<TYPES>::isUnset() const
{
    return !this->d_type;
}

template <class TYPES>
bsl::ostream&
VariantImp<TYPES>::print(bsl::ostream& stream,
                         int           level,
                         int           spacesPerLevel) const
{
    if (this->d_type) {
        Variant_PrintVisitor visitor(&stream, level, spacesPerLevel);

        doApply<const Variant_PrintVisitor&>(visitor, this->d_type);
    }
    return stream;
}

template <class TYPES>
template <class TYPE>
inline
const TYPE& VariantImp<TYPES>::the() const
{
    BSLMF_ASSERT((0 != Variant_TypeIndex<TYPES, TYPE>::value));
    BSLS_ASSERT_SAFE((this->d_type == Variant_TypeIndex<TYPES, TYPE>::value));

    typedef bsls::ObjectBuffer<TYPE> BufferType;

    return reinterpret_cast<const BufferType *>(&this->d_value)->object();
}

template <class TYPES>
inline
int VariantImp<TYPES>::typeIndex() const
{
    return this->d_type;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class TYPES>
inline
const bsl::type_info& VariantImp<TYPES>::typeInfo() const
{
    return typeid(void);
}

template <class TYPES>
template <class STREAM>
STREAM& VariantImp<TYPES>::bdexStreamIn(STREAM& stream, int version)
{
    int type;
    bslx::InStreamFunctions::bdexStreamIn(stream, type, 0);

    if (!stream || type < 0 || 20 < type) {
        stream.invalidate();
        return stream;
    }

    if (type != this->d_type) {
        reset();

        if (type) {
            Variant_DefaultConstructVisitor defaultConstructor(
                                                         this->getAllocator());
            doApply(defaultConstructor, type);
        }

        this->d_type = type;
    }

    if (type) {
        Variant_BdexStreamInVisitor<STREAM> streamer(stream, version);
        doApply(streamer, type);
    }
    return stream;
}

template <class TYPES>
inline
int VariantImp<TYPES>::maxSupportedBdexVersion() const
{
    return 1;
}

template <class TYPES>
template <class STREAM>
STREAM& VariantImp<TYPES>::bdexStreamOut(STREAM& stream, int version) const
{
    bslx::OutStreamFunctions::bdexStreamOut(stream, this->d_type, 0);

    if (this->d_type) {
        typedef Variant_BdexStreamOutVisitor<STREAM> Streamer;

        Streamer streamer(stream, version);
        doApply<Streamer&>(streamer, this->d_type);
    }
    return stream;
}

#endif  // BDE_OMIT_INTERNAL_DEPRECATED

}  // close package namespace

// FREE OPERATORS
template <class TYPES>
bool bdlb::operator==(const VariantImp<TYPES>& lhs,
                      const VariantImp<TYPES>& rhs)
{
    if (lhs.typeIndex() != rhs.typeIndex()) {
        return false;                                                 // RETURN
    }

    if (0 == lhs.typeIndex()) {
        return true;                                                  // RETURN
    }

    Variant_EqualityTestVisitor visitor(&rhs.d_value);
    lhs.apply(visitor);

    return visitor.d_result;
}

template <class TYPES>
inline
bool bdlb::operator!=(const VariantImp<TYPES>& lhs,
                      const VariantImp<TYPES>& rhs)
{
    return !(lhs == rhs);
}

template <class TYPES>
inline
bsl::ostream& bdlb::operator<<(bsl::ostream&            stream,
                               const VariantImp<TYPES>& object)
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <class TYPES>
inline
void bdlb::swap(VariantImp<TYPES>& a, VariantImp<TYPES>& b)
{
    a.swap(b);
}

namespace bdlb {

                       // ------------------
                       // class Variant<...>
                       // ------------------

// CREATORS
#if defined(BDLB_VARIANT_USING_VARIADIC_TEMPLATES)
template <class ...TYPES>
inline
Variant<TYPES...>::Variant()
{
}

template <class ...TYPES>
template <class TYPE_OR_ALLOCATOR>
inline
Variant<TYPES...>::Variant(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class ...TYPES>
template <class TYPE>
inline
Variant<TYPES...>::Variant(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class ...TYPES>
template <class TYPE>
inline
Variant<TYPES...>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant(TYPE&&                   value,
        typename bsl::enable_if<
            !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
            &&
            !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                    void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class ...TYPES>
template <class TYPE>
inline
Variant<TYPES...>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant(TYPE&&                   value,
        typename bsl::enable_if<
            !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
        bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant(bslmf::MovableRef<TYPE>  value,
        bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class ...TYPES>
inline
Variant<TYPES...>::Variant(const Variant&    original,
                           bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
    // Up-cast needed since template matching has higher overloading precedence
    // than derived-to-base matching.
{
}

template <class ...TYPES>
inline
Variant<TYPES...>::Variant(bslmf::MovableRef<Variant> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class ...TYPES>
inline
Variant<TYPES...>::Variant(bslmf::MovableRef<Variant>  original,
                           bslma::Allocator           *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class ...TYPES>
template <class TYPE>
inline
Variant<TYPES...>& Variant<TYPES...>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class ...TYPES>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant<TYPES...>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant<TYPES...> >::type&
Variant<TYPES...>::operator=(TYPE&&                  value)
#else
Variant<TYPES...>&
Variant<TYPES...>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class ...TYPES>
inline
Variant<TYPES...>& Variant<TYPES...>::operator=(const Variant& rhs)
{
    // Up-cast needed since template matching has higher overloading precedence
    // than derived-to-base matching.

    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class ...TYPES>
inline
Variant<TYPES...>& Variant<TYPES...>::operator=(bslmf::MovableRef<Variant> rhs)
{
    // Up-cast needed since template matching has higher overloading precedence
    // than derived-to-base matching.

    Variant& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

#else  // BDLB_VARIANT_USING_VARIADIC_TEMPLATES
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::Variant()
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
template <class TYPE_OR_ALLOCATOR>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::Variant(
                                     const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
template <class TYPE>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::Variant(
                                              const TYPE&       value,
                                              bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
template <class TYPE>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant(TYPE&&                   value,
        typename bsl::enable_if<
            !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
            &&
            !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                    void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
template <class TYPE>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant(TYPE&&                   value,
        typename bsl::enable_if<
            !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
        bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant(bslmf::MovableRef<TYPE>  value,
        bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::Variant(
                                              const Variant&    original,
                                              bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
    // Up-cast needed since template matching has higher overloading precedence
    // than derived-to-base matching.
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::Variant(
                                           bslmf::MovableRef<Variant> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::Variant(
                                    bslmf::MovableRef<Variant>  original,
                                    bslma::Allocator           *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
template <class TYPE>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>&
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,
                          A8,  A9,  A10, A11, A12, A13, A14,
                          A15, A16, A17, A18, A19, A20>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
        A16, A17, A18, A19, A20> >::type&
Variant<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
        A16, A17, A18, A19, A20>::operator=(TYPE&&                  value)
#else
Variant<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
        A16, A17, A18, A19, A20>&
Variant<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
        A16, A17, A18, A19, A20>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>&
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::operator=(const Variant& rhs)
{
    // Up-cast needed since template matching has higher overloading precedence
    // than derived-to-base matching.

    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19, class A20>
inline
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>&
Variant<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10, A11, A12,
        A13, A14, A15, A16, A17, A18, A19, A20>::
operator=(bslmf::MovableRef<Variant> rhs)
{
    // Up-cast needed since template matching has higher overloading precedence
    // than derived-to-base matching.

    Variant& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

#endif  // BDLB_VARIANT_USING_VARIADIC_TEMPLATES

#ifdef BDLB_VARIANT_USING_VARIADIC_TEMPLATES
#undef BDLB_VARIANT_USING_VARIADIC_TEMPLATES
#endif

                       // -------------------
                       // class Variant2<...>
                       // -------------------

// CREATORS
template <class A1, class A2>
inline
Variant2<A1, A2>::Variant2()
{
}

template <class A1, class A2>
template <class TYPE_OR_ALLOCATOR>
inline
Variant2<A1, A2>::Variant2(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2>
template <class TYPE>
inline
Variant2<A1, A2>::Variant2(const TYPE&       value,
                           bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2>
template <class TYPE>
inline
Variant2<A1, A2>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant2(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
             &&
             !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                     void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant2(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2>
template <class TYPE>
inline
Variant2<A1, A2>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant2(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
         bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant2(bslmf::MovableRef<TYPE>  value,
         bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2>
inline
Variant2<A1, A2>::Variant2(const Variant2&   original,
                           bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2>
inline
Variant2<A1, A2>::Variant2(bslmf::MovableRef<Variant2> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2>
inline
Variant2<A1, A2>::Variant2(bslmf::MovableRef<Variant2>  original,
                           bslma::Allocator            *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2>
template <class TYPE>
inline
Variant2<A1, A2>&
Variant2<A1, A2>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant2<A1, A2>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant2<A1, A2> >::type&
Variant2<A1, A2>::operator=(TYPE&&                  value)
#else
Variant2<A1, A2>&
Variant2<A1, A2>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2>
inline
Variant2<A1, A2>&
Variant2<A1, A2>::operator=(const Variant2& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2>
inline
Variant2<A1, A2>&
Variant2<A1, A2>::operator=(bslmf::MovableRef<Variant2> rhs)
{
    Variant2& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // -------------------
                       // class Variant3<...>
                       // -------------------

// CREATORS
template <class A1, class A2, class A3>
inline
Variant3<A1, A2, A3>::Variant3()
{
}

template <class A1, class A2, class A3>
template <class TYPE_OR_ALLOCATOR>
inline
Variant3<A1, A2, A3>::
Variant3(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3>
template <class TYPE>
inline
Variant3<A1, A2, A3>::
Variant3(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3>
template <class TYPE>
inline
Variant3<A1, A2, A3>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant3(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
             &&
             !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                     void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant3(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3>
template <class TYPE>
inline
Variant3<A1, A2, A3>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant3(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
        bslma::Allocator>::type  *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant3(bslmf::MovableRef<TYPE>  value,
        bslma::Allocator         *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3>
inline
Variant3<A1, A2, A3>::
Variant3(const Variant3& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3>
inline
Variant3<A1, A2, A3>::
Variant3(bslmf::MovableRef<Variant3> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3>
inline
Variant3<A1, A2, A3>::
Variant3(bslmf::MovableRef<Variant3>  original,
         bslma::Allocator            *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3>
template <class TYPE>
inline
Variant3<A1, A2, A3>&
Variant3<A1, A2, A3>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant3<A1, A2, A3>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant3<A1, A2, A3> >::type&
Variant3<A1, A2, A3>::operator=(TYPE&&                  value)
#else
Variant3<A1, A2, A3>&
Variant3<A1, A2, A3>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3>
inline
Variant3<A1, A2, A3>&
Variant3<A1, A2, A3>::operator=(const Variant3& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3>
inline
Variant3<A1, A2, A3>&
Variant3<A1, A2, A3>::operator=(bslmf::MovableRef<Variant3> rhs)
{
    Variant3& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // -------------------
                       // class Variant4<...>
                       // -------------------

// CREATORS
template <class A1, class A2, class A3, class A4>
inline
Variant4<A1, A2, A3, A4>::Variant4()
{
}

template <class A1, class A2, class A3, class A4>
template <class TYPE_OR_ALLOCATOR>
inline
Variant4<A1, A2, A3, A4>::
Variant4(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3, class A4>
template <class TYPE>
inline
Variant4<A1, A2, A3, A4>::
Variant4(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4>
template <class TYPE>
inline
Variant4<A1, A2, A3, A4>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant4(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
             &&
             !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                     void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant4(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3, class A4>
template <class TYPE>
inline
Variant4<A1, A2, A3, A4>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant4(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
         bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant4(bslmf::MovableRef<TYPE>  value,
         bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3, class A4>
inline
Variant4<A1, A2, A3, A4>::
Variant4(const Variant4& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4>
inline
Variant4<A1, A2, A3, A4>::
Variant4(bslmf::MovableRef<Variant4> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3, class A4>
inline
Variant4<A1, A2, A3, A4>::
Variant4(bslmf::MovableRef<Variant4>  original,
         bslma::Allocator            *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4>
template <class TYPE>
inline
Variant4<A1, A2, A3, A4>&
Variant4<A1, A2, A3, A4>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant4<A1, A2, A3, A4>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant4<A1, A2, A3, A4> >::type&
Variant4<A1, A2, A3, A4>::operator=(TYPE&&                  value)
#else
Variant4<A1, A2, A3, A4>&
Variant4<A1, A2, A3, A4>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3, class A4>
inline
Variant4<A1, A2, A3, A4>&
Variant4<A1, A2, A3, A4>::operator=(const Variant4& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3, class A4>
inline
Variant4<A1, A2, A3, A4>&
Variant4<A1, A2, A3, A4>::operator=(bslmf::MovableRef<Variant4> rhs)
{
    Variant4& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // -------------------
                       // class Variant5<...>
                       // -------------------

// CREATORS
template <class A1, class A2, class A3, class A4, class A5>
inline
Variant5<A1, A2, A3, A4, A5>::Variant5()
{
}

template <class A1, class A2, class A3, class A4, class A5>
template <class TYPE_OR_ALLOCATOR>
inline
Variant5<A1, A2, A3, A4, A5>::
Variant5(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5>
template <class TYPE>
inline
Variant5<A1, A2, A3, A4, A5>::
Variant5(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5>
template <class TYPE>
inline
Variant5<A1, A2, A3, A4, A5>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant5(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
             &&
             !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                     void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant5(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5>
template <class TYPE>
inline
Variant5<A1, A2, A3, A4, A5>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant5(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
         bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant5(bslmf::MovableRef<TYPE>  value,
         bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5>
inline
Variant5<A1, A2, A3, A4, A5>::
Variant5(const Variant5& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5>
inline
Variant5<A1, A2, A3, A4, A5>::
Variant5(bslmf::MovableRef<Variant5> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3, class A4, class A5>
inline
Variant5<A1, A2, A3, A4, A5>::
Variant5(bslmf::MovableRef<Variant5>  original,
         bslma::Allocator            *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4, class A5>
template <class TYPE>
inline
Variant5<A1, A2, A3, A4, A5>&
Variant5<A1, A2, A3, A4, A5>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant5<A1, A2, A3, A4, A5>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant5<A1, A2, A3, A4, A5> >::type&
Variant5<A1, A2, A3, A4, A5>::operator=(TYPE&&                  value)
#else
Variant5<A1, A2, A3, A4, A5>&
Variant5<A1, A2, A3, A4, A5>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3, class A4, class A5>
inline
Variant5<A1, A2, A3, A4, A5>&
Variant5<A1, A2, A3, A4, A5>::operator=(const Variant5& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5>
inline
Variant5<A1, A2, A3, A4, A5>&
Variant5<A1, A2, A3, A4, A5>::operator=(bslmf::MovableRef<Variant5> rhs)
{
    Variant5& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // -------------------
                       // class Variant6<...>
                       // -------------------

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
Variant6<A1, A2, A3, A4, A5, A6>::Variant6()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
template <class TYPE_OR_ALLOCATOR>
inline
Variant6<A1, A2, A3, A4, A5, A6>::
Variant6(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
template <class TYPE>
inline
Variant6<A1, A2, A3, A4, A5, A6>::
Variant6(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
template <class TYPE>
inline
Variant6<A1, A2, A3, A4, A5, A6>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant6(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
             &&
             !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                     void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant6(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
template <class TYPE>
inline
Variant6<A1, A2, A3, A4, A5, A6>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant6(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
         bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant6(bslmf::MovableRef<TYPE>  value,
         bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
Variant6<A1, A2, A3, A4, A5, A6>::
Variant6(const Variant6& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
Variant6<A1, A2, A3, A4, A5, A6>::
Variant6(bslmf::MovableRef<Variant6> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
Variant6<A1, A2, A3, A4, A5, A6>::
Variant6(bslmf::MovableRef<Variant6>  original,
         bslma::Allocator            *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4, class A5, class A6>
template <class TYPE>
inline
Variant6<A1, A2, A3, A4, A5, A6>&
Variant6<A1, A2, A3, A4, A5, A6>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant6<A1, A2, A3, A4, A5, A6>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant6<A1, A2, A3, A4, A5, A6> >::type&
Variant6<A1, A2, A3, A4, A5, A6>::operator=(TYPE&&                  value)
#else
Variant6<A1, A2, A3, A4, A5, A6>&
Variant6<A1, A2, A3, A4, A5, A6>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
Variant6<A1, A2, A3, A4, A5, A6>&
Variant6<A1, A2, A3, A4, A5, A6>::operator=(const Variant6& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6>
inline
Variant6<A1, A2, A3, A4, A5, A6>&
Variant6<A1, A2, A3, A4, A5, A6>::operator=(bslmf::MovableRef<Variant6> rhs)
{
    Variant6& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // -------------------
                       // class Variant7<...>
                       // -------------------

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::Variant7()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
template <class TYPE_OR_ALLOCATOR>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::
Variant7(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
template <class TYPE>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::
Variant7(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
template <class TYPE>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant7(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
             &&
             !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                     void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant7(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
template <class TYPE>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant7(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
         bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant7(bslmf::MovableRef<TYPE>  value,
         bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::
Variant7(const Variant7& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::
Variant7(bslmf::MovableRef<Variant7> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>::
Variant7(bslmf::MovableRef<Variant7>  original,
         bslma::Allocator            *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
template <class TYPE>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>&
Variant7<A1, A2, A3, A4, A5, A6, A7>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant7<A1, A2, A3, A4, A5, A6, A7>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant7<A1, A2, A3, A4, A5, A6, A7> >::type&
Variant7<A1, A2, A3, A4, A5, A6, A7>::operator=(TYPE&&                  value)
#else
Variant7<A1, A2, A3, A4, A5, A6, A7>&
Variant7<A1, A2, A3, A4, A5, A6, A7>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>&
Variant7<A1, A2, A3, A4, A5, A6, A7>::
operator=(const Variant7& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
inline
Variant7<A1, A2, A3, A4, A5, A6, A7>&
Variant7<A1, A2, A3, A4, A5, A6, A7>::
operator=(bslmf::MovableRef<Variant7> rhs)
{
    Variant7& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // -------------------
                       // class Variant8<...>
                       // -------------------

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::Variant8()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
template <class TYPE_OR_ALLOCATOR>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
Variant8(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
template <class TYPE>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
Variant8(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
template <class TYPE>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant8(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
             &&
             !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                     void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant8(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
template <class TYPE>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant8(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
         bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant8(bslmf::MovableRef<TYPE>  value,
         bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
Variant8(const Variant8& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
Variant8(bslmf::MovableRef<Variant8> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
Variant8(bslmf::MovableRef<Variant8>  original,
         bslma::Allocator            *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
template <class TYPE>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>&
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant8<A1, A2, A3, A4, A5, A6, A7, A8>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant8<A1, A2, A3, A4, A5, A6, A7, A8> >::type&
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
operator=(TYPE&&                  value)
#else
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>&
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>&
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
operator=(const Variant8& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
inline
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>&
Variant8<A1, A2, A3, A4, A5, A6, A7, A8>::
operator=(bslmf::MovableRef<Variant8> rhs)
{
    Variant8& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // -------------------
                       // class Variant9<...>
                       // -------------------

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::Variant9()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
template <class TYPE_OR_ALLOCATOR>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
Variant9(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
template <class TYPE>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
Variant9(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
template <class TYPE>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant9(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
             &&
             !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                     void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant9(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
template <class TYPE>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant9(TYPE&&                   value,
         typename bsl::enable_if<
             !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
         bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant9(bslmf::MovableRef<TYPE>  value,
         bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
Variant9(const Variant9& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
Variant9(bslmf::MovableRef<Variant9> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
Variant9(bslmf::MovableRef<Variant9>  original,
         bslma::Allocator            *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
template <class TYPE>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>&
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9> >::type&
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
operator=(TYPE&&                  value)
#else
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>&
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>&
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
operator=(const Variant9& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
inline
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>&
Variant9<A1, A2, A3, A4, A5, A6, A7, A8, A9>::
operator=(bslmf::MovableRef<Variant9> rhs)
{
    Variant9& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // --------------------
                       // class Variant10<...>
                       // --------------------

// CREATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::Variant10()
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
template <class TYPE_OR_ALLOCATOR>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
Variant10(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
template <class TYPE>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
Variant10(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
template <class TYPE>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant10(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
              &&
              !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                      void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant10(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
template <class TYPE>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant10(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
          bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant10(bslmf::MovableRef<TYPE>  value,
          bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
Variant10(const Variant10& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
Variant10(bslmf::MovableRef<Variant10> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
Variant10(bslmf::MovableRef<Variant10>  original,
          bslma::Allocator             *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
template <class TYPE>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>&
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> >::type&
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
operator=(TYPE&&                  value)
#else
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>&
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>&
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
operator=(const Variant10& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
inline
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>&
Variant10<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::
operator=(bslmf::MovableRef<Variant10> rhs)
{
    Variant10& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // --------------------
                       // class Variant11<...>
                       // --------------------

// CREATORS
template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
Variant11()
{
}

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
template <class TYPE_OR_ALLOCATOR>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
Variant11(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
template <class TYPE>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
Variant11(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
template <class TYPE>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant11(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
              &&
              !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                      void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant11(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
template <class TYPE>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant11(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
          bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant11(bslmf::MovableRef<TYPE>  value,
          bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
Variant11(const Variant11& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
Variant11(bslmf::MovableRef<Variant11> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
Variant11(bslmf::MovableRef<Variant11>  original,
          bslma::Allocator             *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
template <class TYPE>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>&
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11> >::type&
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
operator=(TYPE&&                  value)
#else
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>&
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>&
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
operator=(const Variant11& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3, class A4,  class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
inline
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>&
Variant11<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11>::
operator=(bslmf::MovableRef<Variant11> rhs)
{
    Variant11& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // --------------------
                       // class Variant12<...>
                       // --------------------

// CREATORS
template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
Variant12()
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
template <class TYPE_OR_ALLOCATOR>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
Variant12(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
template <class TYPE>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
Variant12(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
template <class TYPE>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant12(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
              &&
              !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                      void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant12(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
template <class TYPE>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant12(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
          bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant12(bslmf::MovableRef<TYPE>  value,
          bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
Variant12(const Variant12& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
Variant12(bslmf::MovableRef<Variant12> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
Variant12(bslmf::MovableRef<Variant12>  original,
          bslma::Allocator             *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
template <class TYPE>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>&
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12> >::type&
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
operator=(TYPE&&                  value)
#else
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>&
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>&
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
operator=(const Variant12& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
inline
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>&
Variant12<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12>::
operator=(bslmf::MovableRef<Variant12> rhs)
{
    Variant12& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // --------------------
                       // class Variant13<...>
                       // --------------------

// CREATORS
template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
Variant13()
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
template <class TYPE_OR_ALLOCATOR>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
Variant13(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
template <class TYPE>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
Variant13(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
template <class TYPE>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant13(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
              &&
              !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                      void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant13(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
template <class TYPE>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant13(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
          bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant13(bslmf::MovableRef<TYPE>  value,
          bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
Variant13(const Variant13& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
Variant13(bslmf::MovableRef<Variant13> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
Variant13(bslmf::MovableRef<Variant13>  original,
          bslma::Allocator             *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
template <class TYPE>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>&
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
                            A13>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13> >::type&
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
operator=(TYPE&&                  value)
#else
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>&
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>&
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
operator=(const Variant13& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1, class A2, class A3, class A4,  class A5,  class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
inline
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>&
Variant13<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13>::
operator=(bslmf::MovableRef<Variant13> rhs)
{
    Variant13& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // --------------------
                       // class Variant14<...>
                       // --------------------

// CREATORS
template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
Variant14()
{
}

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
template <class TYPE_OR_ALLOCATOR>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
Variant14(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
template <class TYPE>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
Variant14(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
template <class TYPE>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant14(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
              &&
              !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                      void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant14(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
template <class TYPE>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant14(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
          bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant14(bslmf::MovableRef<TYPE>  value,
          bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
Variant14(const Variant14& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
Variant14(bslmf::MovableRef<Variant14> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
Variant14(bslmf::MovableRef<Variant14>  original,
          bslma::Allocator             *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
template <class TYPE>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>&
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant14<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12,
                            A13, A14>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14> >::type&
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
operator=(TYPE&&                  value)
#else
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>&
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>&
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
operator=(const Variant14& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1,  class A2, class A3, class A4,  class A5,  class A6,
          class A7,  class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
inline
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>&
Variant14<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14>::
operator=(bslmf::MovableRef<Variant14> rhs)
{
    Variant14& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // --------------------
                       // class Variant15<...>
                       // --------------------

// CREATORS
template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
Variant15()
{
}

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
template <class TYPE_OR_ALLOCATOR>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
Variant15(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
template <class TYPE>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
Variant15(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
template <class TYPE>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant15(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
              &&
              !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                      void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant15(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
template <class TYPE>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant15(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
          bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant15(bslmf::MovableRef<TYPE>  value,
          bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
Variant15(const Variant15& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
Variant15(bslmf::MovableRef<Variant15> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
Variant15(bslmf::MovableRef<Variant15>  original,
          bslma::Allocator             *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
template <class TYPE>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>&
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant15<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11,
                            A12, A13, A14, A15>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15> >::type&
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
operator=(TYPE&&                  value)
#else
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>&
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>&
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
operator=(const Variant15& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1,  class A2,  class A3, class A4,  class A5,  class A6,
          class A7,  class A8,  class A9, class A10, class A11, class A12,
          class A13, class A14, class A15>
inline
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>&
Variant15<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15>::
operator=(bslmf::MovableRef<Variant15> rhs)
{
    Variant15& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // --------------------
                       // class Variant16<...>
                       // --------------------

// CREATORS
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
Variant16()
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
template <class TYPE_OR_ALLOCATOR>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
Variant16(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
template <class TYPE>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
Variant16(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
template <class TYPE>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant16(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
              &&
              !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                      void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant16(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
template <class TYPE>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant16(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
          bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant16(bslmf::MovableRef<TYPE>  value,
          bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
Variant16(const Variant16& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
Variant16(bslmf::MovableRef<Variant16> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
Variant16(bslmf::MovableRef<Variant16>  original,
          bslma::Allocator             *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
template <class TYPE>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>&
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant16<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11,
                            A12, A13, A14, A15, A16>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16> >::type&
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16>::operator=(TYPE&&                  value)
#else
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16>&
Variant16<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>&
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
operator=(const Variant16& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16>
inline
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>&
Variant16<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16>::
operator=(bslmf::MovableRef<Variant16> rhs)
{
    Variant16& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // --------------------
                       // class Variant17<...>
                       // --------------------

// CREATORS
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
Variant17()
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
template <class TYPE_OR_ALLOCATOR>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
Variant17(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
template <class TYPE>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
Variant17(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
template <class TYPE>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant17(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
              &&
              !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                      void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant17(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
template <class TYPE>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant17(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
          bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant17(bslmf::MovableRef<TYPE>  value,
          bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
Variant17(const Variant17& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
Variant17(bslmf::MovableRef<Variant17> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
Variant17(bslmf::MovableRef<Variant17>  original,
          bslma::Allocator             *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
template <class TYPE>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>&
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant17<A1,  A2,  A3,  A4,  A5,  A6, A7, A8, A9, A10, A11,
                            A12, A13, A14, A15, A16, A17>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant17<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17> >::type&
Variant17<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17>::operator=(TYPE&&                  value)
#else
Variant17<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17>&
Variant17<A1,  A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>&
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
operator=(const Variant17& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17>
inline
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>&
Variant17<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17>::
operator=(bslmf::MovableRef<Variant17> rhs)
{
    Variant17& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // --------------------
                       // class Variant18<...>
                       // --------------------

// CREATORS
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
Variant18()
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
template <class TYPE_OR_ALLOCATOR>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
Variant18(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
template <class TYPE>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
Variant18(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
template <class TYPE>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant18(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
              &&
              !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                      void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant18(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
template <class TYPE>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant18(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
          bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant18(bslmf::MovableRef<TYPE>  value,
          bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
Variant18(const Variant18& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
Variant18(bslmf::MovableRef<Variant18> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
Variant18(bslmf::MovableRef<Variant18>  original,
          bslma::Allocator             *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
template <class TYPE>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>&
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant18<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8, A9, A10,
                            A11, A12, A13, A14, A15, A16, A17, A18>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant18<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17, A18> >::type&
Variant18<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17, A18>::operator=(TYPE&&                  value)
#else
Variant18<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17, A18>&
Variant18<A1,  A2,  A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17, A18>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>&
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
operator=(const Variant18& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18>
inline
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>&
Variant18<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18>::
operator=(bslmf::MovableRef<Variant18> rhs)
{
    Variant18& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

                       // --------------------
                       // class Variant19<...>
                       // --------------------

// CREATORS
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
Variant19()
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
template <class TYPE_OR_ALLOCATOR>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
Variant19(const TYPE_OR_ALLOCATOR& valueOrAllocator)
: Imp(valueOrAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
template <class TYPE>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
Variant19(const TYPE& value, bslma::Allocator *basicAllocator)
: Imp(value, basicAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
template <class TYPE>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant19(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                      SelfType,
                      typename bsl::remove_const<
                      typename bsl::remove_reference<TYPE>::type>::type>::value
              &&
              !bsl::is_convertible<TYPE, bslma::Allocator *>::value,
                      void>::type *)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value))
#else
Variant19(bslmf::MovableRef<TYPE>  value)
: Imp(MoveUtil::move(MoveUtil::access(value)))
#endif
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
template <class TYPE>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
Variant19(TYPE&&                   value,
          typename bsl::enable_if<
              !bsl::is_same<
                     SelfType,
                     typename bsl::remove_const<
                     typename bsl::remove_reference<TYPE>::type>::type>::value,
          bslma::Allocator>::type *basicAllocator)
: Imp(BSLS_COMPILERFEATURES_FORWARD(TYPE, value), basicAllocator)
#else
Variant19(bslmf::MovableRef<TYPE>  value,
          bslma::Allocator        *basicAllocator)
: Imp(MoveUtil::move(MoveUtil::access(value)), basicAllocator)
#endif
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
Variant19(const Variant19& original, bslma::Allocator *basicAllocator)
: Imp(static_cast<const Imp&>(original), basicAllocator)
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
Variant19(bslmf::MovableRef<Variant19> original)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))))
{
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
Variant19(bslmf::MovableRef<Variant19>  original,
          bslma::Allocator             *basicAllocator)
: Imp(MoveUtil::move(static_cast<Imp&>(MoveUtil::access(original))),
      basicAllocator)
{
}

// MANIPULATORS
template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
template <class TYPE>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>&
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
operator=(const TYPE& value)
{
    Imp::operator=(value);
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
template <class TYPE>
inline
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
typename bsl::enable_if<
    !bsl::is_same<Variant19<A1,  A2,  A3,  A4,  A5,  A6,  A7,  A8,  A9, A10,
                            A11, A12, A13, A14, A15, A16, A17, A18, A19>,
                  typename bsl::remove_reference<TYPE>::type>::value,
Variant19<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17, A18, A19> >::type&
Variant19<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17, A18, A19>::operator=(TYPE&&                  value)
#else
Variant19<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17, A18, A19>&
Variant19<A1,  A2,  A3,  A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15,
          A16, A17, A18, A19>::operator=(bslmf::MovableRef<TYPE> value)
#endif
{
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    Imp::operator=(BSLS_COMPILERFEATURES_FORWARD(TYPE, value));
#else
    TYPE& lvalue = value;
    Imp::operator=(MoveUtil::move(lvalue));
#endif

    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>&
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
operator=(const Variant19& rhs)
{
    Imp::operator=(static_cast<const Imp&>(rhs));
    return *this;
}

template <class A1,  class A2,  class A3,  class A4,  class A5,  class A6,
          class A7,  class A8,  class A9,  class A10, class A11, class A12,
          class A13, class A14, class A15, class A16, class A17, class A18,
          class A19>
inline
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>&
Variant19<A1,  A2,  A3,  A4,  A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,
          A15, A16, A17, A18, A19>::
operator=(bslmf::MovableRef<Variant19> rhs)
{
    Variant19& lvalue = rhs;

    Imp::operator=(MoveUtil::move(static_cast<Imp&>(lvalue)));
    return *this;
}

}  // close package namespace
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
