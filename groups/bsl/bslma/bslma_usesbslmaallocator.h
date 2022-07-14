// bslma_usesbslmaallocator.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#define INCLUDED_BSLMA_USESBSLMAALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a metafunction to indicate the use of 'bslma' allocators.
//
//@CLASSES:
//  bslma::UsesBslmaAllocator: trait detection metafunction for 'bslma' use
//
//@SEE_ALSO: bslalg_typetraitusesblsmaallocator
//
//@DESCRIPTION: This component defines a metafunction,
// 'bslma::UsesBslmaAllocator', that may be used to associate a type with the
// uses-'bslma'-allocator trait (i.e., declare that a type uses a 'bslma'
// allocator), and also to detect whether a type has been associated with that
// trait (i.e., to test whether a type uses a 'bslma' allocator, and follows
// the 'bslma' allocator model).
//
// 'bslma::UsesBslmaAllocator<TYPE>' derives from 'bsl::true_type' if 'TYPE' is
// a complete object type and one or more of the following are true:
//
//: o 'bslma::UsesBslmaAllocator<TYPE>' is explicitly specialized to derive
//:   from 'bsl::true_type'.
//: o 'TYPE' has a 'BSLMF_NESTED_TRAIT_DECLARATION(TYPE,
//:   bslma::UsesBslmaAllocator);' declaration.
//: o There exists a nested 'TYPE::allocator_type' that is convertible from
//:   'bslma::Allocator *'.  Note that inheriting from a class that defines
//:   'allocator_type' will implicitly make 'UsesBslmaAllocator' true for the
//:   derived class.  To suppress this behavior, create a nested typedef
//:   'allocator_type' in the derived class that aliases 'void'.
//: o 'bslma::Allocator *' is implicitly convertible to 'TYPE' but other,
//:   arbitrary, pointers are NOT implicitly convertible to 'TYPE'.  Note that
//:   this use is DEPRECATED and that such an implicit conversion will be
//:   discouraged (ideally via a compiler warning).
//
// Otherwise, 'bslma::UsesBslmaAllocator<TYPE>' derives from 'bsl::false_type'.
// Note that top-level cv qualifiers are ignored when applying the above tests.
// If 'TYPE' is a reference type, then 'bslma::UsesBslmaAllocator<TYPE>' always
// derives from 'bsl::false_type', even if the reference type is AA.
//
///Properties of Types Declaring the 'UsesBslmaAllocator' Trait
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Types that declare the 'UsesBslmaAllocator' trait must meet some minimal
// requirements in order for that type to be usable with code that tests for
// the 'UsesBslmaAllocator' trait (e.g., 'bsl' containers).  In addition, types
// that use allocators must have certain properties with respect to memory
// allocation, which are not enforced by the compiler (such a type is described
// as following the 'bslma' allocator model).
//
///Compiler-Enforced Requirements of Types Declaring 'UsesBslmaAllocator'
///-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Types declaring the 'UsesBslmaAllocator' trait must provide a constructor
// variant that accepts a 'bslma::Allocator *' as the last argument (typically
// this is an optional argument).  If such a type provides a copy constructor,
// it must similarly provide a variant that takes a (optional)
// 'bslma::Allocator *' as the last argument.  If such a type provides a move
// constructor, it must similarly provide a variant that takes a
// 'bslma::Allocator *' as the last argument.
//
// Template types (such as 'bsl' containers), where the template parameter
// 'TYPE' represents some element type encapsulated by the class template,
// often use the 'UsesBslmaAllocator' trait to test if 'TYPE' uses 'bslma'
// allocators and, if so, to create 'TYPE' objects using the constructor
// variant taking an allocator.  In this context, compilation will fail if a
// type declares the 'UsesBslmaAllocator' trait, but does not provide the
// expected constructor variant accepting a 'bslma::Allocator *' as the last
// argument.
//
///Expected Properties of Types Declaring the 'UsesBslmaAllocator' Trait
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
// Types declaring the 'UsesBslmaAllocator' trait are expected to have certain
// properties with respect to memory allocation.  These properties are not
// enforced by the compiler, but are necessary to ensure consistent and
// comprehensible allocation behavior.
//
//: o The allocator supplied at construction of an object will be used for
//:   non-transient memory allocation during the object's lifetime.  This
//:   particularly includes allocations performed by sub-objects that
//:   themselves support the 'bslma' allocator model (i.e., the type will
//:   provide the supplied allocator to any data members that themselves accept
//:   an allocator).
//:
//: o If the type defines a move constructor *with* an allocator parameter:
//:   1 If another move constructor *without* an allocator parameter that is
//:     'noexcept' exists, then if the allocators match, the behavior of those
//:     two move constructors will be identical.
//:   2 If the type defines a copy constructor, then the behavior of this
//:     move constructor when allocators don't match will be the same as the
//:     behavior of the copy constructor.
//:
//: o The allocator used by an object is not changed after construction (e.g.,
//:   the assignment operator does not change the allocator used by an object).
//:
//: o Transient memory allocations -- i.e., allocations performed within the
//:   scope of a function where the resulting memory is deallocated before
//:   that function returns -- are generally *not* performed using the object's
//:   allocator.  Although clients may choose whichever allocator suits the
//:   specific context, most often transient memory allocations are performed
//:   using the currently installed default allocator.  For example, a
//:   temporary 'bsl::string' that is destroyed within the scope of a method
//:   need not be explicitly supplied an allocator, since it is a transient
//:   allocation, and 'bsl::string' will use the default allocator by default.
//:
//: o The allocator used by an object is not part of an object's value (e.g.,
//:   it is not tested by the equality-comparison operator 'operator==').
//:
//: o If an allocator is not supplied at construction, then the currently
//:   installed default allocator will typically be used (see 'bslma_default').
//:
//: o If a move constructor is called with no allocator argument, the allocator
//:   used by the new object will be the same as the allocator used by the
//:   source object.
//:
//: o Singleton objects, when necessary, allocate memory from the global
//:   allocator (see 'bslma_default').
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Associating the 'bslma' Allocator Trait with a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to declare two types that make use of a 'bslma' allocator,
// and need to associate the 'UsesBslmaAllocator' trait with those types (so
// that they behave correctly when inserted into a 'bsl' container, for
// example).  In this example we will demonstrate two different mechanisms by
// which a trait may be associated with a type.
//
// First, we define a type 'UsesAllocatorType1' and use the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro to associate the type with the
// 'UsesBslmaAllocator' trait:
//..
//  namespace xyz {
//
//  class UsesAllocatorType1 {
//      // ...
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(UsesAllocatorType1,
//                                     bslma::UsesBslmaAllocator);
//      // CREATORS
//      explicit UsesAllocatorType1(bslma::Allocator *basicAllocator = 0);
//          // ...
//
//      UsesAllocatorType1(const UsesAllocatorType1&  original,
//                         bslma::Allocator          *basicAllocator = 0);
//          // ...
//  };
//..
// Note that the macro declaration must appear within the scope of the class
// declaration and must have 'public' access.
//
// Next, we define a type 'UsesAllocatorType2' and define a specialization of
// the 'UsesBslmaAllocator' trait for 'UsesAllocatorType2' that associates the
// 'UsesBslmaAllocator' trait with the type (sometimes referred to as a
// "C++11-style" trait declaration, because it is more in keeping with the
// style of trait declarations found in the C++11 Standard).  Note that the
// specialization must be performed in the 'BloombergLP::bslma' namespace:
//..
//  class UsesAllocatorType2 {
//      // ...
//
//    public:
//      // CREATORS
//      explicit UsesAllocatorType2(bslma::Allocator *basicAllocator = 0);
//          // ...
//
//      UsesAllocatorType2(const UsesAllocatorType2&  original,
//                         bslma::Allocator          *basicAllocator = 0);
//          // ...
//  };
//
//  }  // close namespace xyz
//
//  // TRAITS
//  namespace BloombergLP {
//  namespace bslma {
//
//  template <>
//  struct UsesBslmaAllocator<xyz::UsesAllocatorType2> : bsl::true_type
//  {};
//
//  }  // close package namespace
//  }  // close enterprise namespace
//..
// Next, we define a type 'BslAAClass', which provides a bsl-AA interface
// based on 'bsl::allocator':
//..
//  namespace xyz {
//
//  class BslAAClass {
//      // ...
//
//    public:
//      // TYPES
//      typedef bsl::allocator<char> allocator_type;
//
//      // CREATORS
//      explicit BslAAClass(const allocator_type& alloc = allocator_type());
//
//      BslAAClass(const BslAAClass&     other,
//                 const allocator_type& alloc = allocator_type());
//  };
//..
// Finally, we define a type 'NonAAClass', which is derived from 'BslAAClass'
// but is not AA itself.  Because it inherits 'allocator_type', this class
// explicitly says it is *not* AA by overiding 'allocator_type' with 'void':
//..
//  class NonAAClass : public BslAAClass {
//      // ...
//
//    public:
//      // TYPES
//      typedef void allocator_type;  // Not an AA class
//
//      // CREATORS
//      NonAAClass();
//
//      NonAAClass(const NonAAClass& other);
//  };
//  }  // close namespace xyz
//..
//
///Example 2: Testing Whether a Type Uses a 'bslma' Allocator
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to test whether each of a set of different types uses a
// 'bslma' allocator.
//
// First, we define a class that is not AA, for control purposes:
//..
//  namespace xyz {
//
//  class DoesNotUseAnAllocatorType { };
//
//  }  // close namespace xyz
//..
// Now, we use the 'UsesBslmaAllocator' template to test whether this type, and
// any of the other types in Example 1 use bslma allocators:
//..
//  int main()
//  {
//      using namespace xyz;
//
//      assert(! bslma::UsesBslmaAllocator<DoesNotUseAnAllocatorType>::value);
//      assert(bslma::UsesBslmaAllocator<UsesAllocatorType1>::value);
//      assert(bslma::UsesBslmaAllocator<UsesAllocatorType2>::value);
//      assert(bslma::UsesBslmaAllocator<BslAAClass>::value);
//      assert(!bslma::UsesBslmaAllocator<NonAAClass>::value);
//  }
//..
// Finally, we demonstrate that the trait can be tested at compilation time by
// testing the trait within the context of a compile-time 'BSLMF_ASSERT':
//..
//  BSLMF_ASSERT(
//          !bslma::UsesBslmaAllocator<xyz::DoesNotUseAnAllocatorType>::value);
//  BSLMF_ASSERT(bslma::UsesBslmaAllocator<xyz::UsesAllocatorType1>::value);
//  BSLMF_ASSERT(bslma::UsesBslmaAllocator<xyz::UsesAllocatorType2>::value);
//  BSLMF_ASSERT(bslma::UsesBslmaAllocator<xyz::BslAAClass>::value);
//  BSLMF_ASSERT(!bslma::UsesBslmaAllocator<xyz::NonAAClass>::value);
//..

#include <bslscm_version.h>

#include <bslma_allocator.h>

#include <bslmf_detectnestedtrait.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isconvertible.h>
#include <bslmf_usesallocator.h>

#include <bsls_annotation.h>
#include <bsls_compilerfeatures.h>

#ifndef BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
# define BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE 1
    // If defined as 1, this macro indicates that
    // 'bslma::UsesBslmaAllocator<TYPE>::value' is automatically true when
    // 'TYPE::allocator_type' exists and is convertible from 'bsl::allocator'.
#elif ! BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
    // If 'BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE' is 0, undef it.
# undef BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
#endif

namespace BloombergLP {
namespace bslma {

// FORWARD DECLARATION
template <class TYPE>
struct UsesBslmaAllocator_Imp;

                        // ========================
                        // class UsesBslmaAllocator
                        // ========================

template <class TYPE>
struct UsesBslmaAllocator
    : bsl::integral_constant<bool, UsesBslmaAllocator_Imp<TYPE>::value>
{
    // This metafunction is derived from 'true_type' if 'TYPE' adheres to the
    // 'bslma' allocator usage idiom and 'false_type' otherwise.  Note that
    // this trait must be explicitly associated with a type in order for this
    // metafunction to return true; simply having a constructor that implicitly
    // converts 'bslma::Allocator *' to 'TYPE' is deprecated as a means of
    // indicating that a type follows the 'bslma' allocator usage idiom.
};

template <class TYPE>
struct UsesBslmaAllocator<TYPE *> : bsl::false_type
{
    // Partial specialization that prevents any pointer from being considered
    // an allocator-aware (AA) type, including pointers that are convertible to
    // 'bslma::Allocator *'.
};

template <class TYPE>
struct UsesBslmaAllocator<TYPE&> : bsl::false_type
{
    // Partial specialization that prevents any lvalue reference from being
    // considered an allocator-aware (AA) type.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TYPE>
struct UsesBslmaAllocator<TYPE&&> : bsl::false_type
{
    // Partial specialization that prevents any rvalue reference from being
    // considered an allocator-aware (AA) type.
};
#endif

template <class TYPE>
struct UsesBslmaAllocator<const TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Partial specialization that associates the same trait with 'const TYPE'
    // as with unqualified 'TYPE'.
};

template <class TYPE>
struct UsesBslmaAllocator<volatile TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Partial specialization that associates the same trait with 'volatile
    // TYPE' as with unqualified 'TYPE'.
};

template <class TYPE>
struct UsesBslmaAllocator<const volatile TYPE> : UsesBslmaAllocator<TYPE>::type
{
    // Partial specialization that associates the same trait with 'const
    // volatile TYPE' as with unqualified 'TYPE'.
};

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

                    // =====================================
                    // class UsesBslmaAllocator_DeprecatedIf
                    // =====================================

template <bool>
struct UsesBslmaAllocator_DeprecatedIf {
    // For compilers that support a deprecation annotation, this empty 'struct'
    // has a static 'check' function that yields a deprecation warning when
    // called (including within an unevaluated context) if this class is
    // instantiated with a 'true' argument, but not if instantiated with a
    // 'false' argument.

    // CLASS METHODS
    static char check(const char *message);
        // Ignore the specified 'message' and return a 'char'.  Declared but
        // not defined; for metaprogramming only.
};

template <>
struct UsesBslmaAllocator_DeprecatedIf<true> {
    // Using this specialization of 'UsesBslmaAllocator_DeprecatedIf' yields a
    // deprecation warning on compilers that support a deprecation annotation.

    // CLASS METHODS
    BSLS_ANNOTATION_DEPRECATED static char check(const char *message);
        // Ignore the specified 'message' and return a 'char'.  Declared but
        // not defined; for metaprogramming only.  Note that, on platforms that
        // support a deprecation attribute, use of this function, even in an
        // unevaluated context, will yield a deprecation warning.
};

#define BSLMA_USESBSLMAALLOCATOR_CAT(X, Y)  \
    BSLMA_USESBSLMAALLOCATOR_CAT_IMP1(X, Y)
#define BSLMA_USESBSLMAALLOCATOR_CAT_IMP1(X, Y) \
    BSLMA_USESBSLMAALLOCATOR_CAT_IMP2(X, Y)
#define BSLMA_USESBSLMAALLOCATOR_CAT_IMP2(X, Y) X##Y
    // Component-local macros for concatenating tokens.

#define BSLMA_USESBSLMAALLOCATOR_DEPRECATE_IF(COND, MSG) enum {           \
    BSLMA_USESBSLMAALLOCATOR_CAT(e_bslma_UsesBslmaAllocator_DeprecateIf_, \
                                 __LINE__)                                \
    = sizeof(UsesBslmaAllocator_DeprecatedIf<!!(COND)>::check(MSG)) }
    // On platforms that support a deprecation attribute, produce a deprecation
    // warning if the specified 'COND' is true, otherwise do nothing.  The
    // specified 'MSG' is ignored, but must be a string literal and should
    // describe why the condition (if true) is undesirable.

                        // ==============================
                        // class UsesBslmaAllocator_Sniff
                        // ==============================

template <class TYPE, bool BYPASS>
struct UsesBslmaAllocator_Sniff {
    // This class has a constant 'value' that is 'true' if the specified
    // 'TYPE' is implicitly convertible from 'bslma::Allocator *'.  It thus
    // "sniffs" whether 'UsesBslmaAllocator<TYPE>::value' should be 'true'.
    // If the specified 'BYPASS' parameter is 'true', the conversion check is
    // skipped and 'value' is hard-coded to 'true'.  The bypass is needed
    // because the xlC compiler will sometimes instantiate the copy
    // constructor for 'TYPE', leading to compilation errors if 'TYPE' is not
    // copy constructible.  This automatic sniffing is deprecated; having a
    // type implicitly convertible from an allocator pointer is discouraged.

  private:
    // PRIVATE TYPES
    struct UniqueType {
        // A class convertible from 'UniqueType *' can be deduced to be
        // convertible from EVERY pointer type, either because it has a
        // non-explicit constructor taking a single argument of type 'void *'
        // or because it has a non-explicit constructor template taking a
        // single template argument that matches any pointer.
    };

    enum {
        // If a pointer to 'Allocator' is convertible to 'T', then 'T' has a
        // non-explicit constructor taking an allocator.
        k_BSLMA_POINTER_CTOR = bsl::is_convertible<Allocator *, TYPE>::value,

        // If a pointer to 'UniqueType' is convertible to 'T', it can only mean
        // that ANY POINTER is convertible to 'T'.
        k_ANY_POINTER_CTOR = bsl::is_convertible<UniqueType *, TYPE>::value,

        // We can "sniff" out a class that uses 'bslma::Allocator *' if it is
        // convertible from 'bslma::Allocator *', but that conversion is
        // *specific* to 'Allocator' pointers, not to arbitrary pointers.
        // Note that "Sniffing" the trait is this way is deprecated.
        k_VALUE = k_BSLMA_POINTER_CTOR && !k_ANY_POINTER_CTOR
    };

    BSLMA_USESBSLMAALLOCATOR_DEPRECATE_IF(k_VALUE,
        "Declaring an allocator-aware type by supplying an implicit "
        "conversion constructor from 'Allocator*' is deprecated.");

  public:
    // TYPES
    enum { value = k_VALUE };
};

template <class TYPE>
struct UsesBslmaAllocator_Sniff<TYPE, true> {
    // Specialization of 'UsesBslmaAllocator_Sniff' for which 'BYPASS'
    // is 'true'.

    enum { value = true };
};


                        // ============================
                        // class UsesBslmaAllocator_Imp
                        // ============================

template <class TYPE>
struct UsesBslmaAllocator_Imp {
    // Implementation of 'UsesBslmaAllocator'.  This class has a constant,
    // 'value', that is 'true' if ANY of the following is true:
    //
    //: o 'TYPE' has a 'BSLMF_NESTED_TRAIT_DECLARATION(TYPE,
    //:   bslma::UsesBslmaAllocator);' declaration.
    //: o 'TYPE' has a nested 'allocator_type' and 'bslma::Allocator *' is
    //:   implicitly convertible to 'TYPE::allocator_type'.
    //: o 'bslma::Allocator *' is implicitly convertible to 'TYPE' but other,
    //:   arbitrary pointers are NOT implicitly convertible to 'TYPE'.
    //
    // Otherwise, 'value' is 'false'.

  private:
    // PRIVATE TYPES
    enum {
        k_NESTED_TRAIT = bslmf::DetectNestedTrait<TYPE,
                                                  UsesBslmaAllocator>::value,

        // Check if 'T' has a nested type, 'allocator_type', and
        // 'bslma::Allocator *' is convertible to 'T::allocator_type'.
        k_COMPATIBLE_ALLOC_TYPE = bsl::uses_allocator<TYPE,Allocator *>::value,

#ifdef BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
        // If either of the previous two constants is 'true', it is not
        // necessary to "sniff" further for this trait.
        k_BYPASS_SNIFFING = k_NESTED_TRAIT || k_COMPATIBLE_ALLOC_TYPE
#else
        // If nested trait is detected, it is not necessary to "sniff" further
        // for this trait.
        k_BYPASS_SNIFFING = k_NESTED_TRAIT
#endif // BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
    };

#ifndef BSLMA_USESBSLMAALLOCATOR_AUTODETECT_ALLOCATOR_TYPE
    BSLMA_USESBSLMAALLOCATOR_DEPRECATE_IF(
        k_COMPATIBLE_ALLOC_TYPE && ! k_NESTED_TRAIT,
        "Class declaring 'allocator_type' but not declaring trait "
        "'bslma::UsesBslmaAllocator' is not considered allocator-aware.");
#endif

  public:
    // TYPES
    enum { value = UsesBslmaAllocator_Sniff<TYPE, k_BYPASS_SNIFFING>::value };
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
