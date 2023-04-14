// bslmf_nestedtraitdeclaration.h                                     -*-C++-*-
#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#define INCLUDED_BSLMF_NESTEDTRAITDECLARATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a nested declaration to associate a class with a trait.
//
//@MACROS:
//  BSLMF_NESTED_TRAIT_DECLARATION: macro that associates a trait with a class
//  BSLMF_NESTED_TRAIT_DECLARATION_IF: conditional macro to associates a trait
//
//@SEE_ALSO: bslmf_detectnestedtrait
//
//@DESCRIPTION: This component defines a pair of macros,
// 'BSLMF_NESTED_TRAIT_DECLARATION' and 'BSLMF_NESTED_TRAIT_DECLARATION_IF',
// that can be used in association with the facilities provided by
// 'bslmf_detectnestedtrait' to declare that a given class has a given trait.
//
// Traits provide a mechanism for convenient compile-time discovery of
// information about a class, which is useful in particular for providing
// efficient specializations of generalized containers and algorithms without
// having to rely on knowledge of specific target classes.
//
// The primary public interface of this component consists of two macros that
// provide a facility for declaring that a given class has a given trait.
// These macros embed the association between the type and the trait inside the
// class definition itself, hence the term "nested trait declaration".
//
// Note that the term "nested" is not meant to imply that this facility
// declares a nested type within the class namespace, only that the trait
// declaration appears as one of the public declarations that make up the class
// definition.  For example, we could declare that a class, 'xyza::Foo', has
// the trait 'abcd::BarTrait' in the following way:
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
// Two flavors of macro are provided: one for declaring unconditionally that a
// class has a trait, and another for declaring that the class has a trait if
// and only if a given compile-time expression evaluates to 'true'.
//
///Relationship with "C++11-style" Type Traits
///-------------------------------------------
// Traits declared using this component are not automatically compatible
// mechanisms designed to detect "C++11-style" traits.  For a full discussion
// of the relationship between nested traits and "C++11-style" traits, as well
// as best practices for defining, associating, and detecting traits, see the
// component documentation for 'bslmf_detectnestedtrait'.
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Testing a Type for a Custom Trait
/// - - - - - - - - - - - - - - - - - - - - - -
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
// First, assume that a compatible trait, 'abcd::RequiresLockTrait', has been
// defined that indicates that a type's methods must not be called unless a
// known lock is first acquired:
//..
//  namespace abcd { template <class t_TYPE> struct RequiresLockTrait; }
//..
// The implementation of 'abcd::RequiresLockTrait' is not shown.
//
// Then, in package 'xyza', we declare a type, 'DoesNotRequireLockType', that
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
// Next, we declare a type, 'RequiresLockType', that does require the lock.  We
// use the 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the type with
// the 'abcd::RequiresLockTrait' trait:
//..
//  class RequiresLockType {
//      // ...
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(RequiresLockType,
//                                     abcd::RequiresLockTrait);
//
//      // CREATORS
//      RequiresLockType();
//          // ...
//
//  };
//..
// Notice that the macro declaration is performed within the scope of the class
// declaration, and must be done with public scope.
//
// Now, we declare a templatized container type, 'Container', that is
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
//
//  } // close package namespace
//..
// Finally, code interacting with 'xyza::DoesNotRequireLockType',
// 'xyza::RequiresLockType' or 'xyza::Container' objects will be able to choose
// the appropriate code path by checking for the 'abcd::RequiresLockTrait'
// trait.  See 'bslmf_detectnestedtrait' for an example of how generic code
// would use such a trait.

#include <bslscm_version.h>

namespace BloombergLP {

namespace bslmf {

                        // ============================
                        // class NestedTraitDeclaration
                        // ============================

template <class t_TYPE, template <class t_T> class t_TRAIT, bool t_COND = true>
class NestedTraitDeclaration {
    // Class 't_TYPE' will be convertible to
    // 'NestedTraitDeclaration<t_TYPE,t_TRAIT,true>' if 't_TRAIT' is associated
    // with 't_TYPE' using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro.  Nested
    // trait detection depends on 't_COND' being true.  If 't_COND' is false,
    // the nested trait detection will not see the conversion it is looking for
    // and will not associate 't_TRAIT' with 't_TYPE'.  This feature is used by
    // 'BSLMF_NESTED_TRAIT_DECLARATION_IF' to turn a trait on or off depending
    // on a compile-time condition (usually another trait).

  public:
    // PUBLIC TYPES
    typedef NestedTraitDeclaration Type;

    // CREATORS
    //! NestedTraitDeclaration();
    //! NestedTraitDeclaration(const  NestedTraitDeclaration&);
    //! NestedTraitDeclaration& operator=(const  NestedTraitDeclaration&);
    //! ~NestedTraitDeclaration();
};

                    // ====================================
                    // macro BSLMF_NESTED_TRAIT_DECLARATION
                    // ====================================

#define BSLMF_NESTED_TRAIT_DECLARATION(t_TYPE, t_TRAIT)                       \
    operator BloombergLP::bslmf::NestedTraitDeclaration<t_TYPE, t_TRAIT>()    \
        const                                                                 \
    {                                                                         \
        return BloombergLP::bslmf::NestedTraitDeclaration<t_TYPE, t_TRAIT>(); \
    }

#ifdef __CDT_PARSER__
// Work around an Eclise CDT bug where it fails to parse the conditional trait
// declaration.  See internal DRQS 47839133.
#define BSLMF_NESTED_TRAIT_DECLARATION_IF(t_TYPE, t_TRAIT, t_COND)
#else
#define BSLMF_NESTED_TRAIT_DECLARATION_IF(t_TYPE, t_TRAIT, t_COND)            \
    operator BloombergLP::bslmf::                                             \
        NestedTraitDeclaration<t_TYPE, t_TRAIT, t_COND>() const               \
    {                                                                         \
        return BloombergLP::bslmf::                                           \
            NestedTraitDeclaration<t_TYPE, t_TRAIT, t_COND>();                \
    }
#endif

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_NESTEDTRAITDECLARATION)

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
