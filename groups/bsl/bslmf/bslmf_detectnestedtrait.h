// bslmf_detectnestedtrait.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#define INCLUDED_BSLMF_DETECTNESTEDTRAIT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to test a type for an associated trait.
//
//@CLASSES:
//   bslmf::DetectNestedTrait: meta-function to test a nested trait
//
//@SEE_ALSO: bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: This component defines a meta-function,
// 'bslmf::DetectNestedTrait', that detects whether or not a given trait has
// been associated with a particular type by means of the macros provided in
// 'bslmf_nestedtraitdeclaration'.  Such traits are referred to as "nested
// traits" because their association with a type is embedded within the type's
// definition.
//
///Detecting Nested Traits
///-----------------------
// If a nested trait, 'TRAIT', is associated with a type, 'TYPE', then
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
//  }  // close namespace 'xyza'
//..
// then 'bslmf::DetectNestedTrait<TYPE, TRAIT>::value' will evaluate to 'true'
// and 'bslmf::DetectNestedTrait<TYPE, TRAIT>::type' will be 'bsl::true_type'.
//
// Additionally, if a trait derives its truth value from
// 'bslmf::DetectNestedTrait', and then that trait is associated with a type as
// a nested trait, we will be able to directly inspect the trait.
//
///Detecting Nested Traits Without 'bslmf::DetectNestedTrait'
///----------------------------------------------------------
// A nested trait can be detected by inspecting
// 'bslmf::DetectNestedTrait<TYPE, TRAIT>::value', but not by inspecting the
// trait directly.  I.e., in the example shown above,
// 'abcd::BarTrait<Foo>::value' will not by default evaluate to 'true'.
// However, if a nested trait derives its truth value from
// 'bslmf::DetectNestedTrait', then it **can** be detected by inspecting the
// trait directly.  I.e., if 'abcd::BarTrait' were defined as follows,
//..
//  namespace abcd {
//
//  template <class TYPE>
//  struct BarTrait : bslmf::DetectNestedTrait<TYPE, BarTrait>::type {
//  };
//
//  }  // close namespace 'abcd'
//..
// and associated with 'Foo' as a nested trait, then
// 'abcd::BarTrait<Foo>::value' would evaluate to 'true'.
//
///Compatibility with "C++11-style" Traits
///---------------------------------------
// By "C++11-style" traits, we mean traits that do not derive from
// 'bslmf::DetectNestedTrait', and are not associated with a type by means of
// the macros provided by 'bslmf_nestedtypetrait'.
//
// Nested traits are "C++11-style" traits are not by default compatible.  The
// following chart shows which combinations of nested and "C++11-style" idioms
// for defining, associating, and detecting traits will result in successful
// detection of a trait:
//..
//            \      Trait Structure        /
//  Association\                           /Detection
//  Method      \   C++11      :DNT<T,TR> / Method
//               |-----------------------|
//  C++11        |    OK     |    OK     |  TR<T>::value
//               |-----------------------|
//  C++11        | XXXXXXXXX | XXXXXXXXX |  DNT<T,TR>::value
//               |-----------------------|
//  Nested       | XXXXXXXXX |    OK     |  TR<T>::value
//               |-----------------------|
//  Nested       |    OK     |    OK     |  DNT<T,TR>::value
//               |-----------------------|
//..
//
///Best Practices
///--------------
// Since the standard method for detecting a trait is to inspect the trait
// directly, a new user-defined trait that might be used as a nested trait
// should derive its truth value from 'bslmf::DetectNestedTrait' following the
// Curiously Recurring Template Pattern.  If all user-defined traits derive
// their true values from 'bslmf::DetectNestedTrait', and all clients test
// traits by directly inspecting the trait's 'value' member, then all clients
// will be able to detect all traits properly, regardless of what mechanism is
// used to associate a particular type with a particular trait.
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
// These are the only compatible uses of
// 'bslmf::DetectNestedTrait<TYPE, TRAIT>::type' and
// 'bslmf::DetectNestedTrait<TYPE, TRAIT>::value' in a system that also uses
// C++11 trait idioms.
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

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLETOANY
#include <bslmf_isconvertibletoany.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

namespace BloombergLP {

namespace bslmf {

                        // =======================
                        // class DetectNestedTrait
                        // =======================

template <class TYPE, template <class T> class TRAIT>
class DetectNestedTrait_Imp {
    // Implementation of class to detect whether the specified 'TRAIT'
    // parameter is associated with the specified 'TYPE' parameter using the
    // nested type trait mechanism.  The 'VALUE' constant will be true iff
    // 'TYPE' is convertible to 'NestedTraitDeclaration<TYPE, TRAIT>'.

  private:
    static char check(NestedTraitDeclaration<TYPE, TRAIT>, int);
        // Declared but not defined.  This overload is selected if called with
        // a type convertible to 'NestedTraitDeclaration<TYPE, TRAIT>'

    static int check(MatchAnyType, ...);
        // Declared but not defined.  This overload is selected if called with
        // a type not convertible to 'NestedTraitDeclaration<TYPE, TRAIT>'

    // Not constructible
    DetectNestedTrait_Imp();
    DetectNestedTrait_Imp(const DetectNestedTrait_Imp&);
    ~DetectNestedTrait_Imp();

    enum {
        CONVERTIBLE_TO_NESTED_TRAIT = sizeof(check(TypeRep<TYPE>::rep(), 0))
                                      == sizeof(char),
        CONVERTIBLE_TO_ANY_TYPE     = IsConvertibleToAny<TYPE>::value
    };

  public:
    // PUBLIC CONSTANTS

    enum { VALUE = CONVERTIBLE_TO_NESTED_TRAIT && !CONVERTIBLE_TO_ANY_TYPE };
        // Non-zero if 'TRAIT' is associated with 'TYPE' using the nested type
        // trait mechanism; otherwise zero.

    typedef bsl::integral_constant<bool, VALUE> Type;
        // Type representing the result of this metafunction.  Equivalent to
        // 'true_type' if 'TRAIT' is associated with 'TYPE' using the nested
        // type trait mechanism; otherwise 'false_type'.
};

template <template <class T> class TRAIT>
struct DetectNestedTrait_Imp<void, TRAIT> {
    // Implementation of 'DetectNestedTrait' for 'void' type.  Short-circuits
    // to 'bsl::false_type' because 'void' can't have any nested traits.

    typedef bsl::false_type Type;
};

template <class TYPE, template <class T> class TRAIT>
struct DetectNestedTrait : DetectNestedTrait_Imp<TYPE, TRAIT>::Type {
    // Metafunction to detect whether the specified 'TRAIT' parameter is
    // associated with the specified 'TYPE' parameter using the nested type
    // trait mechanism.  Inherits from 'true_type' iff 'TYPE' is convertible to
    // 'NestedTraitDeclaration<TYPE, TRAIT>' and from 'false_type' otherwise.
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
