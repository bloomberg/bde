// bslalg_typetraits.h                                                -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITS
#define INCLUDED_BSLALG_TYPETRAITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide facilities for associating types with compile-time traits.
//
//@CLASSES:
//  bslalg::TypeTraitNil: nil type trait (no traits)
//  bslalg::TypeTraitBitwiseMoveable: bitwise-moveable trait
//  bslalg::TypeTraitBitwiseCopyable: bitwise-copyable trait
//  bslalg::TypeTraitBitwiseEqualityComparable: bitwise-eq.-comparable trait
//  bslalg::TypeTraitHasStlIterators: has STL-like iterators
//  bslalg::TypeTraitHasTrivialDefaultConstructor: has trivial default ctor
//  bslalg::TypeTraitPair: for 'std::pair'-like classes
//  bslalg::TypeTraitUsesBslmaAllocator: uses 'bslma' allocators
//  bslalg::TypeTraitsGroupPod: POD trait
//  bslalg::TypeTraitsGroupStlSequence: for STL sequence containers
//  bslalg::TypeTraitsGroupStlOrdered: for STL ordered containers
//  bslalg::TypeTraitsGroupStlUnordered: for STL unordered containers
//  bslalg_TypeTraits: default trait computation
//
//@SEE_ALSO: bslmf_typetraits, bslalg_constructorproxy, bslalg_scalarprimitives
//
//@AUTHOR: Pablo Halpern (phalpern), Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a set of type traits, and a set of
// macros used to assign traits to user-defined class.  Traits are used to
// enable certain optimizations or discriminations at compile-time.  For
// instance, a class having the trait 'bslalg::TypeTraitBitwiseMoveable' may
// allow resizing an array of objects by simply calling 'std::memcpy' instead
// of invoking a copy-constructor on every objects.  The usage example shows
// how to use the 'bslalg::TypeTraitUsesBslmaAllocator' to propagate allocators
// to nested objects that may require them.
//
// This component should be used in conjunction with other components from the
// package 'bslalg'.  See the package-level documentation for an overview.  The
// most useful classes and macros defined in this component are:
//..
//  Entity                                    Description
//  ------                                    -----------
//  bslalg::TypeTraitNil                      The nil trait.  Types with no
//                                            other traits have the nil trait.
//
//  bslalg_TypeTraits<T>                      Traits for type 'T'.  The general
//                                            definition computes the nested
//                                            and implied traits.  A user can
//                                            specialize it to a combination of
//                                            one or more of the following:
//
//  bslalg::TypeTraitBitwiseMoveable         (See the corresponding class-level
//  bslalg::TypeTraitBitwiseCopyable                            documentation.)
//  bslalg::TypeTraitBitwiseEqualityComparable
//  bslalg::TypeTraitHasStlIterators
//  bslalg::TypeTraitHasTrivialDefaultConstructor
//  bslalg::TypeTraitPair
//  bslalg::TypeTraitUsesBslmaAllocator
//
//  BSLALG_DECLARE_NESTED_TRAITS(TYPE, TRAIT) A macro to attach a given 'TRAIT'
//                                            to a given type 'T' in a nested
//                                            fashion.  This macro must be used
//                                            within the 'public' section of a
//                                            class body.
//
//  bslalg::HasTrait<TYPE, TRAIT>             This meta-function computes
//                                            whether the parameterized 'TYPE'
//                                            possesses the parameterized
//                                            'TRAIT'.
//
//  bslalg::PassthroughTrait<TYPE, TRAIT>     This macro selects the
//                                            parameterized 'TRAIT' if the
//                                            parameterized 'TYPE' possesses
//                                            it, and a distinct and otherwise
//                                            unused trait otherwise.
//
//  bslalg::SelectTrait<T, TRAIT1, ...>       This meta-function selects the
//                                            first trait possessed by the
//                                            parameterized 'TYPE' from the
//                                            ordered list 'TRAIT1', ....
//..
//
///Macro List
///----------
// The macros defined in this component are:
//..
//   BSLALG_DECLARE_NESTED_TRAITS: declares a trait in a nested fashion
//  BSLALG_DECLARE_NESTED_TRAITS2: declares two traits in a nested fashion
//  BSLALG_DECLARE_NESTED_TRAITS3: declares three traits in a nested fashion
//  BSLALG_DECLARE_NESTED_TRAITS4: declares four traits in a nested fashion
//  BSLALG_DECLARE_NESTED_TRAITS5: declares five traits in a nested fashion
//           BSLALG_IMPLIES_TRAIT: computes a trait by introspection
//     BSLALG_CHECK_IMPLIED_TRAIT: detects a trait by introspection
//..
//
///Usage
///-----
// In this usage example, we show how to enable the 'bslma' allocator model for
// generic containers, by implementing simplified versions of the
// 'bslalg_constructorproxy' and 'bslalg_scalarprimitives' components.  The
// interested reader should refer to the documentation of those components.
//
///A generic container
///- - - - - - - - - -
// Suppose we want to implement a generic container of a parameterized 'TYPE',
// which may or may not follow the 'bslma' allocator model.  If it does, our
// container should pass an extra 'bslma::Allocator*' argument to copy
// construct a value; but if it does not, then passing this extra argument is
// going to generate a compile-time error.  It thus appears we need two
// implementations of our container.  This can be done more succinctly by
// encapsulating into the constructor some utilities which will, through a
// single interface, determine whether 'TYPE' has the trait
// 'bslalg::TypeTraitUsesBslmaAllocator' and copy-construct it accordingly.
//
// The container contains a single data member of the parameterized 'TYPE'.
// Since we are going to initialize this data member manually, we do not want
// it to be automatically constructed by the compiler.  For this reason, we
// encapsulate it in a 'bsls::ObjectBuffer'.
//..
//  // my_genericcontainer.hpp          -*-C++-*-
//
//  template <class TYPE>
//  class MyGenericContainer {
//      // This generic container type contains a single object, always
//      // initialized, which can be replaced and accessed.  This container
//      // always takes an allocator argument and thus follows the
//      // 'bslalg::TypeTraitUsesBslmaAllocator' protocol.
//
//      // PRIVATE DATA MEMBERS
//      bsls::ObjectBuffer<TYPE> d_object;
//..
// Since the container offers a uniform interface that always takes an extra
// allocator argument, regardless of whether 'TYPE' does or not, we can declare
// it to have the 'bslalg::TypeTraitUsesBslmaAllocator' trait:
//..
//  public:
//    // TRAITS
//    BSLALG_DECLARE_NESTED_TRAITS(MyGenericContainer,
//                                 bslalg::TypeTraitUsesBslmaAllocator);
//..
// For simplicity, we let the container contain only a single element, and
// require that an element always be initialized.
//..
//  // CREATORS
//  MyGenericContainer(const TYPE& object, bslma::Allocator *allocator = 0);
//      // Create an container containing the specified 'object', using the
//      // optionally specified 'allocator' to allocate memory.  If
//      // 'allocator' is 0, the currently installed allocator is used.
//
//  MyGenericContainer(const MyGenericContainer&  container,
//                     bslma::Allocator          *allocator = 0);
//      // Create an container containing the same object as the specified
//      // 'container', using the optionally specified 'allocator' to
//      // allocate memory.  If 'allocator' is 0, the currently installed
//      // allocator is used.
//
//  ~MyGenericContainer();
//      // Destroy this container.
//..
// We can also allow the container to change the object it contains, by
// granting modifiable as well as non-modifiable access to this object:
//..
//      // MANIPULATORS
//      TYPE& object();
//
//      // ACCESSORS
//      const TYPE& object() const;
//  };
//..
///Using the type traits
///- - - - - - - - - - -
// The challenge in the implementation lays in using the traits of the
// contained 'TYPE' to determine whether to pass the allocator argument to its
// copy constructor.  We rely here on a property of templates that templates
// are not compiled (and thus will not generate compilation errors) until they
// are instantiated.  Hence, we can use two function templates, and let the
// overloading resolution (based on the nested traits) decide which to
// instantiate.  The generic way to create an object, passing through all
// arguments (value and allocator) is as follows.  For brevity and to avoid
// breaking the flow of this example, we have embedded the function definition
// into the class.
//..
//  // my_genericcontainer.cpp          -*-C++-*-
//
//  struct my_GenericContainerUtil {
//      // This 'struct' provides a namespace for utilities implementing the
//      // allocator pass-through mechanism in a generic container.
//
//      template <class TYPE>
//      static void copyConstruct(TYPE             *location,
//                                const TYPE&       value,
//                                bslma::Allocator *allocator,
//                                bslalg::TypeTraitUsesBslmaAllocator)
//          // Create a copy of the specified 'value' at the specified
//          // 'location', using the specified 'allocator' to allocate memory.
//      {
//          new (location) TYPE(value, allocator);
//      }
//..
// For types that don't use an allocator, we offer the following overload which
// will be selected if the type trait of 'TYPE' cannot be converted to
// 'bslalg::TypeTraitUsesBslmaAllocator'.  In that case, note that the type
// traits always inherits from 'bslalg::TypeTraitNil'.
//..
//  template <class TYPE>
//  static void copyConstruct(TYPE             *location,
//                            const TYPE&       value,
//                            bslma::Allocator *allocator,
//                            bslalg::TypeTraitNil)
//      // Create a copy of the specified 'value' at the specified
//      // 'location'.  Note that the specified 'allocator' is ignored.
//  {
//      new (location) TYPE(value);
//  }
//..
// And finally, this function will instantiate the type trait and pass it to
// the appropriately (compiler-)chosen overload:
//..
//      template <class TYPE>
//      static void copyConstruct(TYPE             *location,
//                                const TYPE&       value,
//                                bslma::Allocator *allocator)
//          // Create a copy of the specified 'value' at the specified
//          // 'location', optionally using the specified 'allocator' to supply
//          // memory if the parameterized 'TYPE' possesses the
//          // 'bslalg::TypeTraitUsesBslmaAllocator'.
//      {
//          copyConstruct(location, value, allocator,
//                        bslalg_TypeTraits<TYPE>());
//      }
//
//  };
//..
///Generic container implementation
///- - - - - - - - - - - - - - - -
// With these utilities, we can now implement 'MyGenericContainer'.
//..
//  // CREATORS
//  template <typename TYPE>
//  MyGenericContainer<TYPE>::MyGenericContainer(const TYPE&       object,
//                                               bslma::Allocator *allocator)
//  {
//      my_GenericContainerUtil::copyConstruct(&d_object.object(),
//                                             object,
//                                             allocator);
//  }
//
//  template <typename TYPE>
//  MyGenericContainer<TYPE>::MyGenericContainer(
//                                        const MyGenericContainer&  container,
//                                        bslma::Allocator          *allocator)
//  {
//      my_GenericContainerUtil::copyConstruct(&d_object.object(),
//                                             container.object(),
//                                             allocator);
//  }
//..
// Note that all this machinery only affects the constructors, and not the
// destructor which only invokes the destructor of 'd_object'.
//..
//  template <typename TYPE>
//  MyGenericContainer<TYPE>::~MyGenericContainer()
//  {
//      (&d_object.object())->~TYPE();
//  }
//..
// To finish, the accessors and manipulators are trivially implemented.
//..
//  // MANIPULATORS
//  template <typename TYPE>
//  TYPE& MyGenericContainer<TYPE>::object()
//  {
//      return d_object.object();
//  }
//
//  // ACCESSORS
//  template <typename TYPE>
//  const TYPE& MyGenericContainer<TYPE>::object() const
//  {
//      return d_object.object();
//  }
//..
///Usage verification
///- - - - - - - - -
// We can check that our container actually forwards the correct allocator to
// its contained objects with a very simple test apparatus, consisting of two
// classes which have exactly the same signature and implementation except that
// one has the 'bslalg::TypeTraitUsesBslmaAllocator' trait and the other has
// not:
//..
//  bslma::Allocator *allocSlot;
//
//  struct MyTestTypeWithBslmaAllocatorTraits {
//      // Class with declared traits.  Calling copy constructor without an
//      // allocator will compile, but will not set 'allocSlot'.
//
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(MyTestTypeWithBslmaAllocatorTraits,
//                           BloombergLP::bslalg::TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      MyTestTypeWithBslmaAllocatorTraits() {}
//
//      MyTestTypeWithBslmaAllocatorTraits(
//                       const MyTestTypeWithBslmaAllocatorTraits&,
//                       bslma::Allocator                           *allocator)
//      {
//          allocSlot = allocator;
//      }
//  };
//
//  struct MyTestTypeWithNoBslmaAllocatorTraits {
//      // Class with no declared traits.  Calling copy constructor without
//      // an allocator will not set the 'allocSlot', but passing it by mistake
//      // will set it.
//
//      // CREATORS
//      MyTestTypeWithNoBslmaAllocatorTraits() {}
//
//      MyTestTypeWithNoBslmaAllocatorTraits(
//                    const MyTestTypeWithNoBslmaAllocatorTraits &,
//                    bslma::Allocator                              *allocator)
//      {
//          allocSlot = allocator;
//      }
//  };
//..
// Our verification program simply instantiate several 'MyGenericContainer'
// templates with the two test types above, and checks that the allocator slot
// is as expected:
//..
//  int main()
//  {
//      bslma::TestAllocator ta0;
//      bslma::TestAllocator ta1;
//..
// With 'MyTestTypeWithNoBslmaAllocatorTraits', the slot should never be set.
//..
//  MyTestTypeWithNoBslmaAllocatorTraits x;
//
//  allocSlot = &ta0;
//  MyGenericContainer<MyTestTypeWithNoBslmaAllocatorTraits> x0(x);
//  assert(&ta0 == allocSlot);
//
//  allocSlot = &ta0;
//  MyGenericContainer<MyTestTypeWithNoBslmaAllocatorTraits> x1(x, &ta1);
//  assert(&ta0 == allocSlot);
//..
// With 'MyTestTypeWithBslmaAllocatorTraits', the slot should be set to the
// allocator argument, or to 0 if not specified:
//..
//      MyTestTypeWithBslmaAllocatorTraits y;
//
//      allocSlot = &ta0;
//      MyGenericContainer<MyTestTypeWithBslmaAllocatorTraits> y0(y);
//      assert(0 == allocSlot);
//
//      allocSlot = &ta0;
//      MyGenericContainer<MyTestTypeWithBslmaAllocatorTraits> y1(y, &ta1);
//      assert(&ta1 == allocSlot);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE
#include <bslalg_typetraitbitwiseequalitycomparable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS
#include <bslalg_typetraithaspointersemantics.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR
#include <bslalg_typetraithastrivialdefaultconstructor.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITNIL
#include <bslalg_typetraitnil.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITPAIR
#include <bslalg_typetraitpair.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPPOD
#include <bslalg_typetraitsgrouppod.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#include <bslmf_ispointertomember.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

namespace BloombergLP {

// FORWARD DECLARATIONS

namespace bslalg {

struct TypeTraits_NestedYes;
template <class TYPE> struct TypeTraits_AutoDetectIndex;
template <class TYPE> struct TypeTraits_NestedProbe;
template <int PRIORITY> struct TypeTraits_OverloadPriority;
template <int INDEX, class TYPE> struct TypeTraits_AutoDetect;

template <typename TRAIT1, typename TRAIT2>
struct TypeTraits_MakeGroup2;
template <typename TRAIT1, typename TRAIT2, typename TRAIT3>
struct TypeTraits_MakeGroup3;
template <typename TRAIT1, typename TRAIT2, typename TRAIT3, typename TRAIT4>
struct TypeTraits_MakeGroup4;
template <typename TRAIT1, typename TRAIT2, typename TRAIT3, typename TRAIT4,
          typename TRAIT5>
struct TypeTraits_MakeGroup5;

}  // close package namespace

namespace bslalg_TypeTraits_ImpliedTraits {
}  // close namespace bslalg_TypeTraits_Impliedtraits

                          // =======================
                          // class bslalg_TypeTraits
                          // =======================

template<typename T>
struct bslalg_TypeTraits : bslalg::TypeTraits_AutoDetect<
                       bslalg::TypeTraits_AutoDetectIndex<T>::VALUE, T>::Type {
    // This 'struct' provides a mechanism to automatically detect traits for
    // fundamental types, pointers, enums, and types which have declared their
    // traits using the 'BSLALG_DECLARE_NESTED_TRAITS' macro.  Traits can be
    // assigned to other types either by defining a specialization of this
    // template or by using the 'BSLALG_DECLARE_NESTED_TRAITS' macro.  For all
    // other types, including all reference types, the default trait is
    // 'bslalg::TypeTraitNil'.  This 'struct' serves as the default traits
    // class for any type which does not have an explicit specialization of
    // 'BloombergLP::bslalg_TypeTraits'.
};

                    // ====================================
                    // macros BSLALG_DECLARE_NESTED_TRAITS*
                    // ====================================

#define BSLALG_DECLARE_NESTED_TRAITS(T, TRAITS)                   \
    typedef TRAITS NestedTypeTraits;                              \
    BloombergLP::bslalg::TypeTraits_NestedYes&                    \
        operator,(BloombergLP::bslalg::TypeTraits_NestedProbe<T>)
    // Declare the traits of the class as a nested type ('NestedTypeTraits').
    // The 'T' argument should be the name of the class being defined and the
    // 'TRAITS' argument should be the name of its corresponding traits class.
    // Note that this macro should be expanded only within the 'public' section
    // of a class.  Also note that the 'TRAITS' class may be a user-defined
    // class that inherits from several primitive traits and/or traits groups.
    // This macro is deliberately constructed so that the nested traits are not
    // inherited.
    //
    // CAVEAT: This macro defines a comma (,) member operator.  If 'T' inherits
    // from a class with a public comma operator, then it will be necessary to
    // declare 'using Base::operator,;' in order to make the base class
    // operator visible in the derived class.

#define BSLALG_DECLARE_NESTED_TRAITS2(T, TRAIT1, TRAIT2)                      \
    typedef BloombergLP::bslalg::TypeTraits_MakeGroup2<TRAIT1, TRAIT2>        \
                                                            NestedTypeTraits; \
    BloombergLP::bslalg::TypeTraits_NestedYes&                                \
        operator,(BloombergLP::bslalg::TypeTraits_NestedProbe<T>)
    // Like 'BSLALG_DECLARE_NESTED_TRAITS', but for two traits.

#define BSLALG_DECLARE_NESTED_TRAITS3(T, TRAIT1, TRAIT2, TRAIT3)              \
    typedef BloombergLP::bslalg::TypeTraits_MakeGroup3<                       \
                                                      TRAIT1, TRAIT2, TRAIT3> \
                                                            NestedTypeTraits; \
    BloombergLP::bslalg::TypeTraits_NestedYes&                                \
        operator,(BloombergLP::bslalg::TypeTraits_NestedProbe<T>)
    // Like 'BSLALG_DECLARE_NESTED_TRAITS', but for three traits.

#define BSLALG_DECLARE_NESTED_TRAITS4(T, TRAIT1, TRAIT2, TRAIT3, TRAIT4)      \
    typedef Bloomberg::bslalg::TypeTraits_MakeGroup4<                         \
                                              TRAIT1, TRAIT2, TRAIT3, TRAIT4> \
                                                            NestedTypeTraits; \
    BloombergLP::bslalg::TypeTraits_NestedYes&                                \
        operator,(BloombergLP::bslalg::TypeTraits_NestedProbe<T>)
    // Like 'BSLALG_DECLARE_NESTED_TRAITS', but for four traits.

#define BSLALG_DECLARE_NESTED_TRAITS5(T, TRAIT1,TRAIT2,TRAIT3,TRAIT4,TRAIT5)  \
    typedef Bloomberg::bslalg::TypeTraits_MakeGroup5<                         \
                                      TRAIT1, TRAIT2, TRAIT3, TRAIT4, TRAIT5> \
                                                            NestedTypeTraits; \
    BloombergLP::bslalg::TypeTraits_NestedYes&                                \
        operator,(BloombergLP::bslalg::TypeTraits_NestedProbe<T>)

namespace bslalg {

    // Like 'BSLALG_DECLARE_NESTED_TRAITS', but for five traits.

// ---- Anything below this line is implementation specific.  Do not use.  ----

                        //=============================
                        // classes TypeTraits_MakeGroup
                        //=============================

template <typename TRAIT1, typename TRAIT2>
struct TypeTraits_MakeGroup2 : TRAIT1, TRAIT2 {
    // Combine two traits into a group.  This is needed to work-around an IBM
    // bug whereby a nested 'struct' is seen as private even if it is public.
};

template <typename TRAIT1, typename TRAIT2, typename TRAIT3>
struct TypeTraits_MakeGroup3 : TRAIT1, TRAIT2, TRAIT3 {
    // Combine three traits into a group.  This is needed to work-around an IBM
    // bug whereby a nested 'struct' is seen as private even if it is public.
};

template <typename TRAIT1, typename TRAIT2, typename TRAIT3, typename TRAIT4>
struct TypeTraits_MakeGroup4 : TRAIT1, TRAIT2, TRAIT3, TRAIT4 {
    // Combine four traits into a group.  This is needed to work-around an IBM
    // bug whereby a nested 'struct' is seen as private even if it is public.
};

template <typename TRAIT1, typename TRAIT2, typename TRAIT3, typename TRAIT4,
          typename TRAIT5>
struct TypeTraits_MakeGroup5 : TRAIT1, TRAIT2, TRAIT3, TRAIT4, TRAIT5 {
    // Combine five traits into a group.  This is needed to work-around an IBM
    // bug whereby a nested 'struct' is seen as private even if it is public.
};

                        //=============================
                        // class TypeTraits_NestedProbe
                        //=============================

template <typename T>
struct TypeTraits_NestedProbe {
    // Private type used to probe for nested type traits.
};

                         //===========================
                         // class TypeTraits_NestedYes
                         //===========================

struct TypeTraits_NestedYes {
    // Private type returned by a successful probe for nested traits.

    static bslmf::MetaInt<1> match(TypeTraits_NestedYes&);
    template <typename T> static bslmf::MetaInt<0> match(const T&);
    template <typename T> static bslmf::MetaInt<0> match(const volatile T&);
        // Return 'bslmf::MetaInt<1>' if called on an argument of type
        // 'TypeTraits_NestedYes' and 'bslmf::MetaInt<0>' otherwise.
};

                         //============================
                         // class TypeTraits_NestedTest
                         //============================

template <typename TYPE>
class TypeTraits_NestedTest {
    // This private meta-function tests for nested traits, and sets its 'VALUE'
    // member to non-zero if there is a nested traits declaration within the
    // parameterized 'TYPE'.

    typedef typename bslmf::RemoveCvq<TYPE>::Type NoCvqType;
        // The probe must be instantiated with no cv-qualifier or it won't be
        // an exact match.
  public:
    // PUBLIC TYPES
    enum {
        // This anonymous enumerated type declares a single member, 'VALUE',
        // which is true if 'TYPE' has nested traits and false otherwise.

        VALUE = BSLMF_METAINT_TO_BOOL(
            TypeTraits_NestedYes::match(
                (BloombergLP::bslmf::TypeRep<NoCvqType>::rep(),
                 BloombergLP::bslalg::TypeTraits_NestedProbe<NoCvqType>()) ))
    };

    typedef bslmf::MetaInt<VALUE> Type;
};

                     // ==========================
                     // struct TypeTraits_NotTrait
                     // ==========================

template <typename TRAIT> struct TypeTraits_NotTrait {
    // Private class: Given a trait, this template produces a unique type which
    // is NOT the trait type and is not a trait at all.
};

                     // ============================
                     // struct TypeTraits_AutoDetect
                     // ============================

template<int U, typename T> struct TypeTraits_AutoDetect {
    // This meta-function is used to assign traits for the templatized type 'T'
    // using the templatized index 'U'.  The index is determined by the
    // meta-function 'TypeTraits_AutoDetectIndex' for 4 different types:
    //..
    // Index|  Types                          |     Traits
    // =====+=================================+================================
    //  0   |references                       | no implied traits
    //  1   |fundamental type (int, pointers, | POD traits, equality comparable
    //      |                  enum... etc.)  |
    //  2   |nested traits                    | traits declared within class
    //  3   |convertible to 'bslma::Allocator'| usesBslmaAllocator trait
    //  4   |bslmf::Nil                       | bitwise copyable trait
    //..
    //
    // This default implementation of the auto-trait detection mechanism does
    // not assign any trait to its nested 'Type'.

    typedef TypeTraitNil   Type;
};

// PARTIAL SPECIALIZATIONS
template<typename T> struct TypeTraits_AutoDetect<1,T> {
    // This implementation of the auto-detected traits for builtin POD types
    // (fundamental, pointer, enum) assigns the 'TypeTraitsGroupPod' and
    // 'TypeTraitBitwiseEqualityComparable' traits to its nested 'Type'.  It
    // matches the auto-index 1.

    struct Type :
        TypeTraitsGroupPod,
        TypeTraitBitwiseEqualityComparable {};
};

template<typename T> struct TypeTraits_AutoDetect<2, T> {
    // This implementation of the auto-detected traits detects and assigns to
    // its nested 'Type' the nested type 'T::TypeTraits', if any.  It matches
    // the auto-index 2.

    typedef typename T::NestedTypeTraits Type;
};

template<typename T> struct TypeTraits_AutoDetect<3,T> {
    // Auto-detected traits for types constructible from 'bslma::Allocator*'.
    // It matches the auto-index 3.

  private:
    // IMPLEMENTATION NOTE: This meta-function is only evaluated if all other
    // attempts to sniff the traits for 'T' have failed.  Thus, problematic
    // calls to 'bslmf::IsConvertible' are minimized.

    typedef struct UniqueType {
        // A class convertible from this type must have a templated constructor
        // which makes it convertible from EVERY type.
    } *UniquePtrType;

    enum {
        // If a pointer to 'UniqueType' is convertible to 'T', it can only mean
        // that ANY POINTER is convertible to 'T'.

        ANY_POINTER = bslmf::IsConvertible<UniqueType*, T>::VALUE
    };

    enum {
        // Test for conversion from 'bslma::Allocator*'.  If 'ANY_POINTER' is
        // true, then 'bslma::Allocator*' is just like any other pointer and
        // its convertibility to 'T' does not imply that 'T' uses bslma
        // allocators.

        USES_BSLMA = !ANY_POINTER &&
                              bslmf::IsConvertible<bslma::Allocator*, T>::VALUE
    };

  public:
    typedef typename bslmf::If<USES_BSLMA,
                              TypeTraitUsesBslmaAllocator,
                              TypeTraitNil>::Type Type;
};

template <typename T> struct TypeTraits_AutoDetect<4, T> {
    // Auto-detect and assign bitwise copyable traits for 'bslmf::Nil'.  It
    // matches auto-index 4.
    struct Type : TypeTraitBitwiseCopyable
    {
    };
};

                  // ================================
                  // class TypeTraits_AutoDetectIndex
                  // ================================

template <typename T>
struct TypeTraits_AutoDetectIndex {
    // Compute the index of auto-detected traits.  This is used to map an index
    // to 5 different types:
    //..
    //  0. no traits (including references)
    //  1. fundamental types (int, pointers, enum... etc.)
    //  2. nested traits
    //  3. convertible to 'bslma::Allocator'
    //  4. bslmf::Nil
    //..
    // This index value maps to the traits defined in 'TypeTraits_AutoDetect'

  public:
    enum { VALUE = (bslmf::IsFundamental<T>::VALUE      ? 1 :
                    bslmf::IsPointer<T>::VALUE          ? 1 :
                    bslmf::IsPointerToMember<T>::VALUE  ? 1 :
                    bslmf::IsEnum<T>::VALUE             ? 1 :
                    TypeTraits_NestedTest<T>::VALUE     ? 2 :
                    3)
    };
};

// PARTIAL SPECIALIZATIONS
template <typename T> struct TypeTraits_AutoDetectIndex<T&> {
    // This partial specialization of 'TypeTraits_AutoDetectIndex' matches
    // reference types, and assigns an auto-index 0 (nil traits).

    enum { VALUE = 0 };
};

template<> struct TypeTraits_AutoDetectIndex<bslmf::Nil> {
    // This partial specialization of 'TypeTraits_AutoDetectIndex' matches
    // 'bslmf::Nil' type, and assigns an auto-index 4 (bitwise copyable
    // traits).  The use of auto detect index is necessary since 'bslmf::Nil'
    // is defined in the 'bslmf' package, which is below 'bslalg'.

    enum { VALUE = 4 };
};

}  // close package namespace

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#define bdealg_TypeTraits bslalg_TypeTraits
    // This alias is defined for backward compatibility.

#define BDEALG_DECLARE_NESTED_TRAITS(T, TRAITS)                               \
    BSLALG_DECLARE_NESTED_TRAITS(T, TRAITS)
    // This alias is defined for backward compatibility.

#define BDEALG_DECLARE_NESTED_TRAITS2(T, TRAIT1, TRAIT2)                      \
    BSLALG_DECLARE_NESTED_TRAITS2(T, TRAIT1, TRAIT2)
    // This alias is defined for backward compatibility.

#define BDEALG_DECLARE_NESTED_TRAITS3(T, TRAIT1, TRAIT2, TRAIT3)              \
    BSLALG_DECLARE_NESTED_TRAITS3(T, TRAIT1, TRAIT2, TRAIT3)
    // This alias is defined for backward compatibility.

#define BDEALG_DECLARE_NESTED_TRAITS4(T, TRAIT1, TRAIT2, TRAIT3, TRAIT4)      \
    BSLALG_DECLARE_NESTED_TRAITS4(T, TRAIT1, TRAIT2, TRAIT3, TRAIT4)
    // This alias is defined for backward compatibility.

#define BDEALG_DECLARE_NESTED_TRAITS5(T, TRAIT1,TRAIT2,TRAIT3,TRAIT4,TRAIT5)  \
    BSLALG_DECLARE_NESTED_TRAITS5(T, TRAIT1,TRAIT2,TRAIT3,TRAIT4,TRAIT5)
    // This alias is defined for backward compatibility.

#ifdef bdealg_HasTrait
#undef bdealg_HasTrait
#endif
#define bdealg_HasTrait bslalg::HasTrait
    // This alias is defined for backward compatibility.

#ifdef bdealg_PassthroughTrait
#undef bdealg_PassthroughTrait
#endif
#define bdealg_PassthroughTrait bslalg::PassthroughTrait
    // This alias is defined for backward compatibility.

#ifdef bdealg_Passthrough_BdemaAllocator
#undef bdealg_Passthrough_BdemaAllocator
#endif
#define bdealg_Passthrough_BdemaAllocator bslalg::Passthrough_BslmaAllocator
    // This alias is defined for backward compatibility.

#ifdef bdealg_PassthroughTraitBdemaAllocator
#undef bdealg_PassthroughTraitBdemaAllocator
#endif
#define bdealg_PassthroughTraitBdemaAllocator bslalg::Passthrough_BslmaAllocator
    // This alias is defined for backward compatibility.

#ifdef bdealg_TypeTraitsGroupPod
#undef bdealg_TypeTraitsGroupPod
#endif
#define bdealg_TypeTraitsGroupPod bslalg::TypeTraitsGroupPod
    // This alias is defined for backward compatibility.

#ifdef bdealg_TypeTraitsGroupStlSequence
#undef bdealg_TypeTraitsGroupStlSequence
#endif
#define bdealg_TypeTraitsGroupStlSequence bslalg::TypeTraitsGroupStlSequence
    // This alias is defined for backward compatibility.

#ifdef bdealg_TypeTraitsGroupStlOrdered
#undef bdealg_TypeTraitsGroupStlOrdered
#endif
#define bdealg_TypeTraitsGroupStlOrdered bslalg::TypeTraitsGroupStlOrdered
    // This alias is defined for backward compatibility.

#ifdef bdealg_TypeTraitsGroupStlHashed
#undef bdealg_TypeTraitsGroupStlHashed
#endif
#define bdealg_TypeTraitsGroupStlHashed bslalg::TypeTraitsGroupStlUnordered
    // This alias is defined for backward compatibility.

#ifdef bdealg_TypeTraitsGroupStlUnordered
#undef bdealg_TypeTraitsGroupStlUnordered
#endif
#define bdealg_TypeTraitsGroupStlUnordered bslalg::TypeTraitsGroupStlUnordered
    // This alias is defined for backward compatibility.

#ifdef bdealg_SelectTrait
#undef bdealg_SelectTrait
#endif
#define bdealg_SelectTrait bslalg::SelectTrait
    // This alias is defined for backward compatibility.

#endif

#ifdef bslalg_TypeTraits_NotTrait
#undef bslalg_TypeTraits_NotTrait
#endif
#define bslalg_TypeTraits_NotTrait bslalg::TypeTraits_NotTrait
    // This alias is defined for backward compatibility.

}  // close enterprise namespace

#endif // INCLUDED_BSLALG_TYPETRAITS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
