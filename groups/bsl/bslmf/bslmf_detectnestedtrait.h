// bslmf_detectnestedtrait.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#define INCLUDED_BSLMF_DETECTNESTEDTRAIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a facility for defining traits and detecting legacy traits.
//
//@CLASSES:
//   bslmf::DetectNestedTrait: meta-function to test a nested trait
//
//@SEE_ALSO: bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: This component defines a meta-function,
// 'bslmf::DetectNestedTrait', that facilitates the creation of traits that can
// be associated with a type using the nested trait mechanism in
// 'bslmf_declarenestedtrait'.  Such traits are referred to as "nested traits"
// because their association with a type is embedded within the type's
// definition.  'bslmf::DetectNestedTrait' and can also be used to detect
// traits created using older legacy traits definitions mechanisms used at
// Bloomberg.
//
// Please note:
//: 1. The use of 'bslmf::DetectNestedTrait' to detect traits is *deprecated*.
//:    Clients should detect traits using the C++11 idiom (see
//:    {Nested Trait Idiom vs. C++11 Trait Idiom} below)
//:
//: 2. Clients are encouraged to use a C++11 idiom for defining traits.
//:    However, authors of traits who want trait users to be able to take
//:    advantage of 'bslmf_nestedtraitdeclaration' must define traits that
//:    inherit from 'bslmf::DetectNestedTrait' (see
//:    {Writing a User-Defined Trait} below).
//
///Nested Trait Idiom vs. C++11 Trait Idiom
///----------------------------------------
// BDE supports two idioms for defining traits and associating them with types.
// The older idiom uses 'bslmf::DetectNestedTrait' to define traits, and the
// 'BSLMF_NESTED_TRAIT_DECLARATION*' macros to associate traits with types.
// This idiom is called the "nested trait" idiom.
//
// The newer idiom is familiar to users of C++11 traits, and is referred to
// here as the "C++11 trait" idiom.  In the C++11 trait idiom, a trait is a
// template that derives its truth value from 'bsl::true_type' or
// 'bsl::false_type', and is associated with a type by providing a
// specialization of the trait for the associated type.
//
// For example, a minimal C++11 trait, 'abcd::C11Trait', could be defined as:
//..
//  namespace abcd {
//
//  template <class TYPE>
//  struct C11Trait : bsl::false_type {
//  };
//
//  }  // close namespace abcd
//..
// 'abcd::C11Trait' would then be associated with a class, 'xyza::SomeClass',
// by specializing the trait for that class:
//..
//  namespace xyza {
//
//  class SomeClass {
//      // The definition of 'SomeClass' does not affect the trait mechanism.
//
//      // ...
//  };
//
//  }  // close namespace xyza
//
//  namespace abcd {
//
//  template <>
//  struct C11Trait<xyza::SomeClass> : bsl::true_type {
//  };
//
//  }  // close namespace abcd
//..
// Note that the specialization is defined in the same namespace as the
// original trait.
//
// Both idioms detect the association of a trait with a class in the same way:
// by inspecting the trait's 'value' member.
//..
//  assert(true  == abcd::C11Trait<xyza::SomeClass>::value);
//  assert(false == abcd::C11Trait<xyza::Foo>::value);
//  assert(true  == abcd::BarTrait<xyza::Foo>::value);
//  assert(false == abcd::BarTrait<xyza::SomeClass>::value);
//..
// The C++11 trait idiom is the standard idiom for new code in BDE.
//
///Writing a User-Defined Trait
///----------------------------
// On systems that do not require compatibility with the nested trait idiom,
// new traits should be written according to the C++11 trait idiom.
//
// On systems that support the nested trait idiom, any new user-defined trait
// should derive its truth value from 'bslmf::DetectNestedTrait' following the
// Curiously Recurring Template Pattern.  This will allow the trait to be
// detected by directly inspecting the trait's 'value' member, regardless of
// whether the trait is associated with a type through the nested trait idiom
// or through the C++11 trait idiom.
//
// Therefore, the simplest maximally-compatible trait would look like this:
//..
//  template <class TYPE>
//  struct MyTrait : bslmf::DetectNestedTrait<TYPE, MyTrait>::type {};
//..
// A trait having more complex default logic could derive from
// 'bsl::integral_constant' using the 'value' member of
// 'bslmf::DetectNestedTrait', such as:
//..
//  template <class TYPE>
//  struct ComplexTrait : bsl::integral_constant<bool,
//                          bslmf::DetectNestedTrait<TYPE, ComplexTrait>::value
//                          || SomeOtherTrait<TYPE>::value> {
//  };
//..
// These are the only recommended uses of
// 'bslmf::DetectNestedTrait<TYPE, TRAIT>::type' and
// 'bslmf::DetectNestedTrait<TYPE, TRAIT>::value'.
//
///Detecting Legacy Traits
///-----------------------
// If a trait, 'TRAIT', has been associated with a type, 'TYPE', using one of
// the 'BSLMF_NESTED_TRAIT_DECLARATION*' macros then
// 'bslmf::DetectNestedTrait<TYPE, TRAIT>' derives from 'bsl::true_type'.
// Otherwise, 'bslmf::DetectNestedTrait<TYPE, TRAIT>' derives from
// 'bsl::false_type'.
//
// Therefore, if a trait 'abcd::BarTrait' has been associated with a class
// 'xyza::Foo' in the following way:
//..
//  namespace xyza {
//
//  class Foo {
//      // ... various implementation details ...
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(Foo, abcd::BarTrait);
//
//      // ... the rest of the public interface ...
//  };
//
//  }  // close namespace xyza
//..
// then 'bslmf::DetectNestedTrait<TYPE, TRAIT>::value' will evaluate to 'true'
// and 'bslmf::DetectNestedTrait<TYPE, TRAIT>::type' will be 'bsl::true_type'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Defining a Custom Trait
/// - - - - - - - - - - - - - - - - -
// When writing generic infrastructure code, we often need to choose among
// multiple code paths based on the capabilities of the types on which we are
// operating.  If those capabilities are reflected in a type's public
// interface, we may be able to use techniques such as SFINAE to choose the
// appropriate code path.  However, SFINAE cannot detect all of a type's
// capabilities.  In particular, SFINAE cannot detect constructors, memory
// allocation, thread-safety characteristics, and so on.  Functions that depend
// on these capabilities must use another technique to determine the correct
// code path to use for a given type.  We can solve this sort of problem by
// associating types with custom traits that indicate what capabilities are
// provided by a given type.
//
// First, in package 'abcd', define a trait, 'RequiresLockTrait', that
// indicates that a type's methods must not be called unless a known lock it
// first acquired:
//..
//  namespace abcd {
//
//  template <class TYPE>
//  struct RequiresLockTrait :
//                    bslmf::DetectNestedTrait<TYPE, RequiresLockTrait>::type {
//  };
//
//  } // close package namespace
//..
// Notice that 'RequiresLockTrait' derives from
// 'bslmf::DetectNestedTrait<TYPE, RequiresLockTrait>::type' using the
// curiously recurring template pattern.
//
// Then, in package 'xyza', we declare a type, 'DoesNotRequireALockType', that
// can be used without acquiring the lock:
//..
//  namespace xyza {
//
//  class DoesNotRequireLockType {
//      // ...
//
//    public:
//      // CREATORS
//      DoesNotRequireLockType();
//          // ...
//  };
//..
// Next, we declare a type, 'RequiresLockTypeA', that does require the lock.
// We use the 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the type with
// the 'abcd::RequiresLockTrait' trait:
//..
//  class RequiresLockTypeA {
//      // ...
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(RequiresLockTypeA,
//                                     abcd::RequiresLockTrait);
//
//      // CREATORS
//      RequiresLockTypeA();
//          // ...
//
//  };
//..
// Notice that the macro declaration is performed within the scope of the class
// declaration, and must be done with public scope.
//
// Then, we declare a templatized container type, 'Container', that is
// parameterized on some 'ELEMENT' type.  If 'ELEMENT' requires a lock, then a
// 'Container' of 'ELEMENT's will require a lock as well.  This can be
// expressed using the 'BSLMF_NESTED_TRAIT_DECLARATION_IF' macro, by providing
// 'abcd::RequiresLockTrait<ELEMENT>::value' as the condition for associating
// the trait with 'Container'.
//..
//  template <class ELEMENT>
//  struct Container {
//      // ...
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION_IF(Container, abcd::RequiresLockTrait,
//                                    abcd::RequiresLockTrait<ELEMENT>::value);
//
//      // ...
//  };
//..
// Next, we show that traits based on 'bslmf::DetectNestedTrait' can be
// associated with a type using "C++11-style" trait association.  To do this,
// we declare a type, 'RequiresLockTypeB', that also requires the lock, but
// does not used the 'BSLMF_NESTED_TRAIT_DECLARATION' macro:
//..
//  class RequiresLockTypeB {
//      // ...
//
//    public:
//      // CREATORS
//      RequiresLockTypeB();
//          // ...
//
//  };
//
//  } // close package namespace
//..
// Then, we associate 'RequiresLockTypeB' with 'abcd::RequiresLockTrait' by
// directly specializing 'abcd::RequiresLockTrait<xyza::RequiresLockTypeB>'.
// This is the standard way of associating a type with a trait since C++11:
//..
//  namespace abcd {
//
//  template <>
//  struct RequiresLockTrait<xyza::RequiresLockTypeB> : bsl::true_type {
//  };
//
//  } // close namespace abcd
//..
// Now, we can write a function that inspects
// 'abcd::RequiresLockTrait<TYPE>::value' to test whether or not various types
// are associated with 'abcd::RequiresLockTrait':
//..
//  void example1()
//  {
//      assert(false ==
//             abcd::RequiresLockTrait<xyza::DoesNotRequireLockType>::value);
//
//      assert(true  ==
//             abcd::RequiresLockTrait<xyza::RequiresLockTypeA>::value);
//
//      assert(true  ==
//             abcd::RequiresLockTrait<xyza::RequiresLockTypeB>::value);
//
//      assert(false ==
//             abcd::RequiresLockTrait<
//                     xyza::Container<xyza::DoesNotRequireLockType> >::value);
//
//      assert(true  ==
//             abcd::RequiresLockTrait<
//                          xyza::Container<xyza::RequiresLockTypeA> >::value);
//
//      assert(true  ==
//             abcd::RequiresLockTrait<
//                          xyza::Container<xyza::RequiresLockTypeB> >::value);
//
//      // ...
//  }
//..
// Finally, we demonstrate that the trait can be tested at compilation time, by
// writing a function that tests the trait within the context of a compile-time
// 'BSLMF_ASSERT':
//..
//  void example2()
//  {
//      BSLMF_ASSERT(false ==
//             abcd::RequiresLockTrait<xyza::DoesNotRequireLockType>::value);
//
//      BSLMF_ASSERT(true  ==
//             abcd::RequiresLockTrait<xyza::RequiresLockTypeA>::value);
//
//      BSLMF_ASSERT(true  ==
//             abcd::RequiresLockTrait<xyza::RequiresLockTypeB>::value);
//
//      BSLMF_ASSERT(false ==
//             abcd::RequiresLockTrait<
//                     xyza::Container<xyza::DoesNotRequireLockType> >::value);
//
//      BSLMF_ASSERT(true  ==
//             abcd::RequiresLockTrait<
//                          xyza::Container<xyza::RequiresLockTypeA> >::value);
//
//      BSLMF_ASSERT(true  ==
//             abcd::RequiresLockTrait<
//                          xyza::Container<xyza::RequiresLockTypeB> >::value);
//
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLETOANY
#include <bslmf_isconvertibletoany.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)                                            \
 ||(defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130)
// IBM xlC and Sun CC compilers have a hard time handling function types in
// some of the template instantiations required by this component, so we simply
// treat function types as 'void' types.  This gives the same result as neither
// can hold a nested typedef.  Last tested with xlC 12.
# define BSLMF_DETECTNESTEDTRAIT_CANNOT_COMPILE_WITH_FUNCTION_TYPES  1
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)                                            \
 ||(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1700)
# define BSLMF_DETECTNESTEDTRAIT_NO_SUPPORT_FOR_ARRAY_TYPES   1
#endif

namespace BloombergLP {

namespace bslmf {

                        // =======================
                        // class DetectNestedTrait
                        // =======================

template <class TYPE, template <class> class TRAIT>
class DetectNestedTrait_Imp {
    // Implementation of class to detect whether the specified 'TRAIT'
    // parameter is associated with the specified 'TYPE' parameter using the
    // nested type trait mechanism.  The 'k_VALUE' constant will be true iff
    // 'TYPE' is convertible to 'NestedTraitDeclaration<TYPE, TRAIT>'.

  private:
    static char check(NestedTraitDeclaration<TYPE, TRAIT>, int);
        // Declared but not defined.  This overload is selected if called with
        // a type convertible to 'NestedTraitDeclaration<TYPE, TRAIT>'

    static int check(MatchAnyType, ...);
        // Declared but not defined.  This overload is selected if called with
        // a type not convertible to 'NestedTraitDeclaration<TYPE, TRAIT>'

  private:
    // NOT IMPLEMENTED
    DetectNestedTrait_Imp();                             // = delete
    DetectNestedTrait_Imp(const DetectNestedTrait_Imp&); // = delete
    ~DetectNestedTrait_Imp();                            // = delete
        // This trait type can neither be constructed nor destroyed.

    enum {
        k_CONVERTIBLE_TO_NESTED_TRAIT = sizeof(check(TypeRep<TYPE>::rep(), 0))
                                      == sizeof(char),
        k_CONVERTIBLE_TO_ANY_TYPE     = IsConvertibleToAny<TYPE>::value
    };

  public:
    // PUBLIC CONSTANTS

    enum {
        k_VALUE = k_CONVERTIBLE_TO_NESTED_TRAIT && !k_CONVERTIBLE_TO_ANY_TYPE
    };
        // Non-zero if 'TRAIT' is associated with 'TYPE' using the nested type
        // trait mechanism; otherwise zero.

    typedef bsl::integral_constant<bool, k_VALUE> Type;
        // Type representing the result of this metafunction.  Equivalent to
        // 'true_type' if 'TRAIT' is associated with 'TYPE' using the nested
        // type trait mechanism; otherwise 'false_type'.
};

template <template <class> class TRAIT>
struct DetectNestedTrait_Imp<void, TRAIT> {
    // Implementation of 'DetectNestedTrait' for 'void' type.  Short-circuits
    // to 'bsl::false_type' because 'void' can't have any nested traits.

    typedef bsl::false_type Type;
};

#if defined(BSLMF_DETECTNESTEDTRAIT_NO_SUPPORT_FOR_ARRAY_TYPES)
template <class TYPE, template <class> class TRAIT>
struct DetectNestedTrait_Imp<TYPE[], TRAIT> {
    // Implementation of 'DetectNestedTrait' for array-of-unknown-bound type.
    // This specialization is required to work around compiler bugs with the
    // IBM xlC compiler and old Microsoft compilers. which fail to parse this
    // case (array-of-unknown-bound) as a valid partial specialization.  The
    // nested 'Type' is always 'bsl::false_type' as array types cannot have any
    // nested traits.

    typedef bsl::false_type Type;
};
#endif

#if !defined(BSLMF_DETECTNESTEDTRAIT_CANNOT_COMPILE_WITH_FUNCTION_TYPES)
template <class TYPE, template <class> class TRAIT>
struct DetectNestedTrait : DetectNestedTrait_Imp<TYPE, TRAIT>::Type {
    // Metafunction to detect whether the specified 'TRAIT' parameter is
    // associated with the specified 'TYPE' parameter using the nested type
    // trait mechanism.  Inherits from 'true_type' iff 'TYPE' is convertible to
    // 'NestedTraitDeclaration<TYPE, TRAIT>' and from 'false_type' otherwise.
};
#else
template <class TYPE, template <class> class TRAIT>
struct DetectNestedTrait : DetectNestedTrait_Imp<
                       typename bsl::conditional< bsl::is_function<TYPE>::value
                                                , void
                                                , TYPE>::type, TRAIT>::Type {
    // Metafunction to detect whether the specified 'TRAIT' parameter is
    // associated with the specified 'TYPE' parameter using the nested type
    // trait mechanism.  Inherits from 'true_type' iff 'TYPE' is convertible to
    // 'NestedTraitDeclaration<TYPE, TRAIT>' and from 'false_type' otherwise.
    // Note that on compilers that have a difficult time processing function
    // types as template parameters, we treat such instantiations the same as
    // requesting a nested trait of a 'void' type; the result is guaranteed to
    // be the same as neither 'void' nor function types can contain nested
    // types.  We choose to use 'bsl::conditional' inside the 'Imp' template
    // instantiation as this guarantees that there is always a valid template
    // instantiation requested, the alternative of using 'conditonal' to pick
    // the base class as either 'false_type' or the 'Imp' instantiation would
    // still require the 'Imp' instantiation be valid, which is exactly the
    // compiler bug we are trying to work around.
};
#endif


// Additional partial template specializations ensure that cv-qualified types
// have (or do not have) the same traits as the corresponding cv-unqualified
// type.

#if defined(BSLS_REMOVECONST_WORKAROUND_CONST_MULTIDIMENSIONAL_ARRAY)
// XLC has an issue removing const-qualifiers from array types.  The rendundant
// use of 'bsl::remove_const' below is a work-around, which utilizes the
// work-arounds already in place for XLC in the 'bslmf_removeconst' component.
// See 'bslmf_removeconst' for more information.

template <class TYPE, template <class> class TRAIT>
struct DetectNestedTrait<const TYPE, TRAIT>
     : DetectNestedTrait<typename bsl::remove_const<TYPE>::type, TRAIT>::Type {
};

#else

template <class TYPE, template <class> class TRAIT>
struct DetectNestedTrait<const TYPE, TRAIT>
     : DetectNestedTrait<TYPE, TRAIT>::Type {
};

#endif


template <class TYPE, template <class> class TRAIT>
struct DetectNestedTrait<volatile TYPE, TRAIT>
     : DetectNestedTrait<TYPE, TRAIT>::Type {
};

template <class TYPE, template <class> class TRAIT>
struct DetectNestedTrait<const volatile TYPE, TRAIT>
     : DetectNestedTrait<TYPE, TRAIT>::Type {
};

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_DETECTNESTEDTRAIT)

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
