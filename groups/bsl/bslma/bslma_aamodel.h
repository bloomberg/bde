// bslma_aamodel.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMA_AAMODEL
#define INCLUDED_BSLMA_AAMODEL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide metafunctions to detect a type's allocator-awareness model.
//
//@CLASSES:
//  AAModelNone:   Tag 'struct' representing no AA model
//  AAModelLegacy: Tag for the *legacy-AA* model ('bslma::Allocator*')
//  AAModelBsl:    Tag for the *bsl-AA* model ('bsl::allocator')
//  AAModelPmr:    Tag for the *pmr-AA* model ('polymorphic_allocator')
//  AAModelStl:    Tag for the *stl-AA* model (non-bsl, non-pmr STL allocator)
//  AAModelIsSupported: Metafunction to detect if type 'T' supports model 'M'
//  AAModel: Metafunction to detect the AA model implemented by a type 'T'
//
//@SEE_ALSO: bslma_allocator, bslma_stdallocator
//
//@DESCRIPTION: This component provides five tag 'struct's ('AAModelNone',
// 'AAModelLegacy', 'AAModelBsl', 'AAModelPmr', and 'AAModelStl') and a pair of
// metafunction classes ('AAModelIsSupported<TYPE, MODEL>' and 'AAModel<TYPE>')
// that allow generic allocator-handling facilities -- such as those that
// construct allocator-aware (a.k.a., AA) objects, extract the allocator from
// AA objects, and manage propagation of allocators from a container to its
// contained elements -- to determine, at compile time, the AA model of the
// type(s) they work with.
//
///Supported Allocator-Aware (AA) models
///-------------------------------------
// The AA model supported by a type 'T' is determined primarily by the type of
// allocator used by 'T' to allocate memory:
//
//: o If 'T' does not use an externally supplied allocator, then it is not AA.
//: o If 'T' uses the address of a 'bslma::Allocator', then it is *legacy-AA*.
//:   Most older AA classes developed at Bloomberg are *legacy-AA*.  The value
//:   of 'bslma::UsesBslmaAllocator<T>::value' must be true for a type to be
//:   detected as supporting the *legacy-AA* model.
//: o If 'T' uses an instantiation of 'bsl::allocator', then it is *bsl-AA*.
//:   Most newer AA classes developed at Bloomberg are *bsl-AA*.  'T' is
//:   automatically detectable as supporting the *bsl-AA* model if there exists
//:   a nested type 'T::allocator_type' that is convertible from
//:   'bsl::allocator<char>'.
//: o (Future) If 'T' uses an instantiation of 'bsl::polymorphic_allocator',
//:   then it is *pmr-AA*.  Note that 'bsl::polymorphic_allocator' is currently
//:   non-existant, but is expected sometime in 2022.  When it becomes
//:   available, 'bsl::polymorphic_allocator' will implement the C++17
//:   'std::pmr::polymorphic_allocator' interface and will, in fact, be just an
//:   alias for 'std::pmr::polymorphic_allocator' when the latter is available
//:   in the platform library.  'T' will be automatically detectable as
//:   supporting the *pmr-AA* model if there exists a nested type
//:   'T::allocator_type' that is convertible from
//:   'bsl::polymorphic_allocator<char>'.
//: o If 'T' uses an STL-style allocator other than 'bsl::allocator' or
//:   'bsl::polymorphic_allocator', then it is *stl-AA*.  For the purposes of
//:   this component, any class having an 'allocator_type' member that is not
//:   convertible from 'bsl::allocator' or 'bsl::polymorphic_allocator' is
//:   assumed to be fall into this category.  For example, 'bslstl::vector' is
//:   *stl-AA* if it is instantiated with a third-party STL-compliant
//:   allocator.
//
// Note that a single type can support more than one AA model.  For example,
// any type that is *bsl-AA* can be constructed with a 'bsl::Allocator' pointer
// and, thus, can be said to support the *legacy-AA* model as well.  Similarly,
// all AA types are assumed to be constructible without specifying an allocator
// (typically using a default-initialized allocator value) and, thus, can be
// used as non-AA types.
//
///Allocator-Aware (AA) model tags
///-------------------------------
// Each of the above models has a corresponding *model tag* 'struct' in this
// component.  A *model tag* can be thought of as an enumerator except that,
// rather than being a simple integral value within an 'enum', each one is a
// 'struct' that has no non-static data members, is derived from an
// instantiation of 'bsl::integral_constant', and has a 'type' member 'typedef'
// alias for itself.  By representing each model as a separate *type* rather
// than a simple *value*, these tag types are more versatile when used for
// metaprogramming.  The table below lists the 'value' constant for each
// *model* *tag* struct:
//..
//  +---------------+-------+
//  | model tag     | value |
//  +---------------+-------+
//  | AAModelNone   |   0   |
//  | AAModelLegacy |   1   |
//  | AAModelBsl    |   2   |
//  | AAModelPmr    |   3   |
//  | AAModelStl    |   4   |
//  +---------------+-------+
//..
//
///AA-model metafunctions
///----------------------
// The metafunctions 'AAModel<T>' and 'AAModelIsSupported<T,M>' are used for
// compile-time dispatch of generic code for different allocator models.  This
// subsection describes the criteria by which these metafunctions determine the
// AA model for a type 'T'.  The information here is not critical for
// understanding the use of this component, so readers are welcome to skip
// forward to the usage examples, below.
//
// With the exception of *legacy-AA*, AA types generally adhere to the basic
// interface pattern shown here:
//..
//  class AAType {
//      // ... (private members)
//    public:
//      // TYPES
//      typedef some-type allocator_type;
//
//      // CREATORS
//      // ... (constructors that optionally take an 'allocator_type' argument)
//      ~AAType();
//
//      // MANIPULATORS
//      // ...
//
//      // ACCESSORS
//      // ...
//      allocator_type get_allocator() const;
//  };
//..
// The metafunctions in this component test for the existence of the
// 'allocator_type' member.  If 'T::allocator_type' exists, then 'T' is assumed
// to support the *stl-AA* model.  If, additionally,
// 'bsl::polymorphic_allocator<char>' and/or 'bsl::allocator<char>' are
// convertible to 'T::allocator_type' then 'T' supports the *pmr-AA* and/or
// *bsl-AA* models, respectively.  The metafunctions do not require that AA
// constructors exist nor that the 'get_allocator' member function exists.
//
// The interface for a *legacy-AA* type is somewhat different:
//..
//  class LegacyAAType {
//      // ... (private members)
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(LegacyAAType,
//                                     bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      // ... (constructors that optionally take an 'bslma::Allocator*' arg)
//      ~LegacyAAType();
//
//      // MANIPULATORS
//      // ...
//
//      // ACCESSORS
//      // ...
//      bslma::Allocator *allocator() const;
//  };
//..
// If 'bslma::UsesBslmaAllocator<T>::value' is 'true', then the metafunctions
// assume that 'T' supports the *legacy-AA* model.  Again, the presence of
// appropriate allocators or the 'allocator' accessor are not required.
//
///Usage
///-----
//
///Example 1: Conditionally Passing an Allocator to a Constructor
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the use of 'AAModelIsSupported' to choose an
// appropriate overload for AA constructors.  Consider a *bsl-AA* class,
// 'Wrapper', that wraps an object of template-parameter type, 'TYPE'.  First,
// we define the data members:
//..
//  template <class TYPE>
//  class Wrapper {
//      // Wrap an object of type 'TYPE'.
//
//      // DATA
//      bsl::allocator<char> d_allocator;
//      TYPE                 d_object;
//
//    public:
//      // TYPES
//      typedef bsl::allocator<char> allocator_type;
//..
// Next, we define the constructors.  The constructors for 'Wrapper' would all
// take an optional 'allocator_type' argument, but the 'd_object' member might
// or might not be constructed with an allocator argument.  To handle the
// allocator correctly, therefore, we choose to have two versions of each
// constructor: one that is invoked if 'TYPE' is AA and one that is invoked if
// it is not.  Since both constructors have the same argument list, we must
// make them templates and distinguish them using SFINAE so that only one
// instantiation is valid, i.e., by using 'enable_if' along with the
// 'AAModelIsSupported':
//..
//      // CREATORS
//      template <class ALLOC>
//      explicit
//      Wrapper(const ALLOC& a,
//              typename bsl::enable_if<
//                 bsl::is_convertible<ALLOC,allocator_type>::value
//              && bslma::AAModelIsSupported<TYPE,bslma::AAModelLegacy>::value,
//                  int
//              >::type = 0)
//          // Construct a 'Wrapper' using the specified 'a' allocator, passing
//          // the allocator to the wrapped object.  This constructor will not
//          // participate in overload resolution unless 'TYPE' supports the
//          // legacy allocator-awareness model (*legacy-AA*).
//          : d_allocator(a), d_object(d_allocator.mechanism()) { }
//
//      template <class ALLOC>
//      explicit
//      Wrapper(const ALLOC& a,
//              typename bsl::enable_if<
//                 bsl::is_convertible<ALLOC,allocator_type>::value
//             && !bslma::AAModelIsSupported<TYPE,bslma::AAModelLegacy>::value,
//                 int
//              >::type = 0)
//          // Construct a 'Wrapper' using the specified 'a' allocator,
//          // constructing the wrapped object without an explicit allocator.
//          // This constructor will not participate in overload resolution if
//          // 'TYPE' supports the legacy allocator-awareness model
//          // (*legacy-AA*).
//          : d_allocator(a), d_object() { }
//..
// Support for *bsl-AA* implies support for *legacy-AA*, so the example above
// needs to test for only the latter model; the first constructor overload is
// selected if 'TYPE' implements either AA model.  Similarly
// 'd_allocator.mechanism()' yields a common denominator type,
// 'bslma::Allocator *' that can be passed to the constructor for 'd_object',
// regardless of its preferred AA model.  The second overload is selected for
// types that do *not* support the *legacy-AA* (or *bsl-AA*) model.  Note that
// this example, though functional, does not handle all cases; e.g., it does
// not handle types whose allocator constructor parameter is preceded by
// 'bsl::allocator_arg_t'.  See higher-level components such as
// 'bslma_contructionutil' for a more comprehensive treatment of AA constructor
// variations.
//
// Next, we finish up our class by creating accessors to get the allocator and
// wrapped object:
//..
//      // ACCESSORS
//      const allocator_type get_allocator() const { return d_allocator; }
//          // Return the allocator used to construct this object.
//
//      const TYPE& value() const { return d_object; }
//  };
//..
// Now, to see the effect of these constructors, we'll use a simple AA class,
// 'SampleAAType' that does nothing more than hold the allocator:
//..
//  class SampleAAType {
//      // Sample AA class that adheres to the bsl-AA interface.
//
//      // DATA
//      bsl::allocator<char> d_allocator;
//
//    public:
//      // TYPES
//      typedef bsl::allocator<char> allocator_type;
//
//      // CREATORS
//      explicit SampleAAType(const allocator_type& alloc = allocator_type())
//          : d_allocator(alloc) { }
//      SampleAAType(const SampleAAType&) { }
//
//      // MANIPULATORS
//      SampleAAType& operator=(const SampleAAType&) { return *this; }
//
//      // ACCESSORS
//      allocator_type get_allocator() const { return d_allocator; }
//  };
//..
// Finally, in our main program, create an allocator and pass it to a couple of
// 'Wrapper' objects, one instantiated with 'int' and the other instantiated
// with our 'SampleAAType'.  We verify that both were constructed
// appropriately, with the allocator being used by the 'SampleAAType' object,
// as desired:
//..
//  void main()
//  {
//      bslma::TestAllocator alloc;
//
//      Wrapper<int> w1(&alloc);
//      assert(&alloc == w1.get_allocator());
//      assert(0 == w1.value());
//
//      Wrapper<SampleAAType> w2(&alloc);
//      assert(&alloc == w2.get_allocator());
//      assert(&alloc == w2.value().get_allocator());
//  }
//..
// Note that, even though 'SampleAAType' conforms to the *bsl-AA* interface, it
// is also supports the *legacy-AA* model because 'bslma::Allocator *' is
// convertible to 'bsl::allocator'.
//
///Example 2: Choose an Implementation Based on Allocator-Aware (AA) model
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the use of 'AAModel' to dispatch among several
// implementations based on the AA model preferred by a parameter type.  We
// would like a uniform way to get the allocator used by an object.  We'll
// define a utility class, 'Util', containing a static member function
// template, 'getAllocator(const TYPE& obj)' returning a 'bsl::allocator<char>'
// as follows:
//
//: o If 'TYPE' is *bsl-AA*, return 'obj.get_allocator()'.
//: o If 'TYPE' is *legacy-AA*, return 'bsl::allocator<char>(obj.allocator())'.
//: o If 'TYPE' is not AA, return 'bsl::allocator<char>()'.
//: o If 'TYPE' is AA but not one of the above, compilation will fail.
//
// We'll use 'AAModel<TYPE>' to dispatch to one of three implementations of
// 'getAllocator'.
//
// First, we declare the 'Util' class and three private overloaded
// implemention functions, each taking an argument of a different AA model
// tag:
//..
//  class Util {
//      // Namespace for functions that operate on AA types.
//
//      template <class TYPE>
//      static bsl::allocator<char> getAllocatorImp(const TYPE& obj,
//                                                  bslma::AAModelBsl)
//          { return obj.get_allocator(); }
//      template <class TYPE>
//      static bsl::allocator<char> getAllocatorImp(const TYPE& obj,
//                                                  bslma::AAModelLegacy)
//          { return obj.allocator(); }
//      template <class TYPE>
//      static bsl::allocator<char> getAllocatorImp(const TYPE&,
//                                                  bslma::AAModelNone)
//          { return bsl::allocator<char>(); }
//..
// Next, we dispatch to one of the implementation functions using
// 'AAModel<TYPE>' to yield a tag that indicates the AA model used by 'TYPE'.
//..
//
//    public:
//      // CLASS METHODS
//      template <class TYPE>
//      static bsl::allocator<char> getAllocator(const TYPE& obj)
//          { return getAllocatorImp(obj, bslma::AAModel<TYPE>()); }
//  };
//..
// Now, to check all of the possibilities, we create a minimal AA type sporting
// the *legacy-AA* interface:
//..
//  class SampleLegacyAAType {
//      // Sample AA class that adheres to the bsl-AA interface.
//
//      // DATA
//      bslma::Allocator *d_allocator_p;
//
//  public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(SampleLegacyAAType,
//                                     bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      explicit SampleLegacyAAType(bslma::Allocator *a =0)
//          : d_allocator_p(a) { }
//
//      // ACCESSORS
//      bslma::Allocator *allocator() const { return d_allocator_p; }
//  };
//..
// Finally, we create objects of 'SampleAAType' and 'SampleLegacyAAType' using
// different allocators as well as an object of type 'float' (which, of course
// is not AA), and verify that 'Util::getAllocator' returns the correct
// allocator for each.
//..
//  void main()
//  {
//      bslma::TestAllocator ta1, ta2;
//
//      SampleAAType       obj1(&ta1);
//      SampleLegacyAAType obj2(&ta2);
//      float              obj3 = 0.0;
//
//      assert(Util::getAllocator(obj1) == &ta1);
//      assert(Util::getAllocator(obj2) == &ta2);
//      assert(Util::getAllocator(obj3) == bslma::Default::defaultAllocator());
//  }
//..


#include <bslscm_version.h>

#include <bslma_allocator.h>
// #include <bslma_polymorphicallocator.h>  // Future
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_conditional.h>
#include <bslmf_integralconstant.h>
#include <bslmf_tag.h>
#include <bslmf_usesallocator.h>
#include <bslmf_voidtype.h>

namespace BloombergLP {
namespace bslma {

// FORWARD DECLARATIONS
template <class TYPE, class = void> struct AAModel_HasAllocatorType;
template <class TYPE,
          class MODEL1,
          class MODEL2 = void,
          class MODEL3 = void,
          class MODEL4 = void,
          class MODEL5 = void> struct AAModel_Imp;

                        // ==================
                        // struct AAModelNone
                        // ==================

struct AAModelNone : bsl::integral_constant<int, 0> {
    // Model tag for non-AA types.

    // TYPES
    typedef AAModelNone type;
};

                        // ====================
                        // struct AAModelLegacy
                        // ====================

struct AAModelLegacy : bsl::integral_constant<int, 1> {
    // Model tag for legacy-AA types.

    // TYPES
    typedef AAModelLegacy type;
};

                        // =================
                        // struct AAModelBsl
                        // =================

struct AAModelBsl : bsl::integral_constant<int, 2> {
    // Model tag for bsl-AA types.

    // TYPES
    typedef AAModelBsl type;
};

                        // =================
                        // struct AAModelPmr
                        // =================

struct AAModelPmr : bsl::integral_constant<int, 3> {
    // Model tag for pmr-AA types.  Note that this 'struct' is a place holder
    // for future support of 'polymorphic_allocator'.

    // TYPES
    typedef AAModelPmr type;
};

                        // =================
                        // struct AAModelStl
                        // =================

struct AAModelStl : bsl::integral_constant<int, 4> {
    // Model tag for stl-AA types.

    // TYPES
    typedef AAModelStl type;
};

                        // =======================
                        // struct template AAModel
                        // =======================

template <class TYPE>
struct AAModel : AAModel_Imp<TYPE,
                             AAModelPmr,
                             AAModelBsl,
                             AAModelLegacy,
                             AAModelStl,
                             AAModelNone>::type {
    // Metafunction that yields (is derived from) a model tag type that
    // indicates the AA model preferred for the specified type -- one of
    // 'AAModelNone', 'AAModelLegacy', 'AAModelBsl', 'AAModelPmr', or
    // 'AAModelStl'.  If 'TYPE' supports more than one of AA models, then the
    // result is the model tag with the highest 'value' except that the result
    // is 'AAModelStl' if and only if 'TYPE::allocator_type' exists and is not
    // convertible from one of the polymorphic allocator types
    // ('bsl::allocator' or 'pmr::polymorphic_allocator').
};

                    // =================================
                    // struct template AAModeIsSupported
                    // =================================

template <class TYPE, class MODEL>
struct AAModelIsSupported : bsl::false_type {
    // Predicate metafunction that determines if the specified 'TYPE' supports
    // the specified 'MODEL' AA model.  It yields (i.e., is derived from)
    // 'bsl:true_type' if type 'TYPE' supports AA model 'MODEL'; otherwise it
    // yields 'false_type'.  'MODEL' Must be one of 'AAModelNone',
    // 'AAModelLegacy', 'AAModelBsl', 'AAModelPmr', or 'AAModelStl'.  This
    // metafunction can yield 'true_type' for more than one type 'MODEL' type,
    // e.g., it will yield 'true_type' if 'TYPE' is *bsl-AA*, and 'MODEL' is
    // either 'AAModelBsl' or 'AAModelLegacy', since 'TYPE' can be constructed
    // with either allocator type.
};

template <class TYPE>
struct AAModelIsSupported<TYPE, AAModelNone> : bsl::true_type {
    // Specialization of 'AAModelIsSupported' for 'MODEL == AAModelNone',
    // always yielding 'true_type'.
};

template <class TYPE>
struct AAModelIsSupported<TYPE, AAModelLegacy>
    : bsl::integral_constant<bool,
                             UsesBslmaAllocator<TYPE>::value               ||
                             AAModelIsSupported<TYPE, AAModelBsl>::value>
{
    // Specialization of 'AAModelIsSupported' for 'MODEL == AAModelStl',
    // yielding 'true_type' if 'TYPE' can use 'bslma::Allocator *' as its
    // memory resource.  Specifically, it is true if 'UsesBslmaAllocator<TYPE>'
    // is true or if 'TYPE::allocator_type' exists and is convertible from
    // 'bslma::Allocator *'.
};

template <class TYPE>
struct AAModelIsSupported<TYPE, AAModelBsl>
    : bsl::integral_constant<bool,
                     bsl::uses_allocator<TYPE, bsl::allocator<char> >::value ||
                     AAModelIsSupported<TYPE, AAModelPmr>::value> {
    // Specialization of 'AAModelIsSupported' for 'MODEL == AAModelBsl',
    // yielding 'true_type' if 'TYPE::allocator_type' exists and is convertible
    // from 'bsl::allocator<char>'.
};

template <class TYPE>
struct AAModelIsSupported<TYPE, AAModelPmr> : bsl::false_type
{
    // Specialization of 'AAModelIsSupported' for 'MODEL == AAModelBsl',
    // yielding 'true_type' if 'TYPE::allocator_type' exists and is convertible
    // from 'bsl::polymorphic_allocator<char>'.  Note that this metafunction is
    // not yet implemented and always yields 'false_type'.
};

template <class TYPE>
struct AAModelIsSupported<TYPE, AAModelStl>
    : bsl::integral_constant<bool,
                             AAModel_HasAllocatorType<TYPE>::value        ||
                             AAModelIsSupported<TYPE, AAModelBsl>::value>
{
    // Specialization of 'AAModelIsSupported' for 'MODEL == AAModelStl',
    // yielding 'true_type' if 'TYPE::allocator_type' exists; otherwise,
    // yielding 'false_type' unless 'AAModelIsSupported<TYPE, AAModelBsl>' is
    // true.  The latter condition exists so that *bsl-AA* always implies
    // *stl-AA* even if 'AAModelIsSupported<TYPE, AAModelBsl>' is explicitly
    // specialized to true for a type that does not have an 'allocator_type'
    // member.
};

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

                // ========================================
                // struct template AAModel_HasAllocatorType
                // ========================================

template <class TYPE, class>
struct AAModel_HasAllocatorType : bsl::false_type {
    // Metafunction yields 'true_type' if 'TYPE::allocator_type' exists; else
    // yields 'false_type'.
};

template <class TYPE>
struct AAModel_HasAllocatorType<TYPE,
                              BSLMF_VOIDTYPE(typename TYPE::allocator_type)>
    : bsl::true_type {
    // Specialization for 'TYPE's that *do* have an 'allocator_type' member.
};

                        // ===========================
                        // struct template AAModel_Imp
                        // ===========================

template <class TYPE,
          class MODEL1,
          class MODEL2,
          class MODEL3,
          class MODEL4,
          class MODEL5>
struct AAModel_Imp {
    // Implementation of 'bslma::AAModel' metafunction.  Tries each of the
    // specified 'MODEL1' to 'MODEL5' template arguments and sets the member
    // 'typedef', 'type', to the first model that is supported by the
    // specified 'TYPE'.  All arguments except the first are defaulted in the
    // forward declaration.  Note that 'type' will not be defined if 'TYPE'
    // doesn't support at least one of the passed-in models.

    // Implementation note 1: this metafunction could have been implemented
    // slightly more compactly using variadic templates, but it seemed
    // unnecessary to involve the C++03 variadic emulation for a fixed-length
    // pack of 5 parameters.
    //
    // Implementation note 2: To avoid infinite compile-time recursion, do not
    // instantiate the 'type' member of 'AAModel_Imp' within the
    // 'bsl::conditional', below.  If the condition is 'true', the first
    // '::type' after the 'conditional' selects the 'AAModel_Imp'
    // specialization whereas the second '::type' yields the recursive
    // application of the metafunction; there is no recursion of the condition
    // is 'false'.  This idiom is the compile-time equivalent of having a
    // conditional expression that returns a pointer-to-function that, only
    // when invoked, could yield a recursive call.

    // TYPES
    typedef typename bsl::conditional<
        AAModelIsSupported<TYPE, MODEL1>::value,
        MODEL1,
        AAModel_Imp<TYPE, MODEL2, MODEL3, MODEL4, MODEL5>
        >::type::type type;
        // One of 'MODEL1' to 'MODEL5', indicating the model supported by
        // 'TYPE'.  If multiple models are supported, then the first supported
        // one in the template argument list is used.
};

template <class TYPE>
struct AAModel_Imp<TYPE, void, void, void, void, void> {
    // Partial specialization of 'AAModel_Imp' for when none of the supplied
    // models is supported (should never happen).  The nested 'type' alias does
    // not exist in this specialization.
};

}  // close package namespace
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMA_AAMODEL)

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
