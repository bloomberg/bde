// bslma_destructorproctor.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMA_DESTRUCTORPROCTOR
#define INCLUDED_BSLMA_DESTRUCTORPROCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor to conditionally manage an object.
//
//@CLASSES:
//  bslma::DestructorProctor: proctor to conditionally manage an object
//
//@SEE_ALSO: bslma_destructorguard, bslma_autodestructor
//
//@DESCRIPTION: This component provides a proctor class template,
// 'bslma::DestructorProctor', to conditionally manage an (otherwise-unmanaged)
// object of parameterized 'TYPE' supplied at construction.  If not explicitly
// released, the managed object is destroyed automatically when the proctor
// object goes out of scope by calling the object's destructor.  Note that
// after a proctor object releases its managed object, the same proctor can be
// reused to conditionally manage another object by invoking the 'reset'
// method.
//
///Usage
///-----
// The 'bslma::DestructorProctor' is normally used to manage objects that are
// constructed sequentially in a block of memory provided.  This is often the
// case when memory management and primitive helpers are implemented in
// different components.  An example would be the construction of a pair object
// within another container with the help of a scalar primitive helper (see
// 'bslma_constructionutil').  After the first object is constructed in the
// provided memory, it should be protected in case the constructor of the
// second object throws.  The following example illustrates a typical use of
// the 'bslma::DestructorProctor'.
//
// First, suppose we have a pair class similar to 'std::pair':
//..
//  // MyPair.h
//  // ...
//
//  template <class TYPE1, class TYPE2>
//  class MyPair {
//      // This class provides a pair container to pair two different objects,
//      // one of parameterized 'TYPE1', and the other of parameterized
//      // 'TYPE2'.
//
//    public:
//      // PUBLIC TYPES
//      typedef TYPE1 firstType;
//      typedef TYPE2 secondType;
//
//      // PUBLIC DATA
//      TYPE1            first;          // first object
//      TYPE2            second;         // second object
//
//      // Declare trait 'my_PairTrait'.
//      // ...
//
//    public:
//      // CREATORS
//      // ...
//
//      MyPair(const TYPE1&      iFirst,
//             const TYPE2&      iSecond,
//             bslma::Allocator *basic_Allocator = 0)
//          // Create a 'MyPair' object that holds a copy of the specified
//          // 'iFirst' and 'iSecond'.  Optionally specify 'basicAllocator' to
//          // supply memory.  If 'basicAllocator' is zero,  the global default
//          // allocator will be used to supply memory.
//      : first(iFirst)
//      , second(iSecond)
//      , d_allocator_p(bslma::Default::allocator(basic_Allocator))
//      {
//      }
//
//      // ...
//
//  };
//..
// Note that parts of the implementation, including the 'my_PairTrait'
// declaration, are elided.  The 'my_PairTrait' will be used by the primitive
// helper to customize implementations for objects that are pairs.
//
// We now implement the primitive helper:
//..
//  // MyPrimitives.h
//  // ...
//
//  struct MyPrimitives {
//      // This 'struct' provides a namespace for primitive functions used to
//      // construct, destroy, insert, append and remove objects.
//
//    private:
//      // PRIVATE TYPES
//      enum { PAIR_TRAIT = 1, NIL_TRAIT = 0 };
//
//    public:
//      // CLASS METHODS
//
//      template <class TYPE>
//      static void copyConstruct(TYPE             *address,
//                                const TYPE&       original,
//                                bslma::Allocator *basicAllocator);
//          // Copy construct the specified 'original' into the specified
//          // 'address' using the specified 'basicAllocator' (if the
//          // copy constructor of 'TYPE' takes an allocator).
//
//      template <class TYPE>
//      static void copyConstruct(TYPE             *address,
//                                const TYPE&       original,
//                                bslma::Allocator *basicAllocator,
//                                bsl::integral_constant<bool, PAIR_TRAIT> *);
//          // Copy construct the specified 'original' into the specified
//          // 'address' using the specified 'basicAllocator' (if the
//          // copy constructor of 'TYPE' takes an allocator).  Note that
//          // the last parameter is used only for overload resolution.
//
//      template <class TYPE>
//      static void copyConstruct(TYPE             *address,
//                                const TYPE&       original,
//                                bslma::Allocator *basicAllocator,
//                                bsl::integral_constant<bool, NIL_TRAIT> *);
//          // Copy construct the specified 'original' into the specified
//          // 'address' using the specified 'basicAllocator' (if the
//          // copy constructor of 'TYPE' takes an allocator).  Note that
//          // the last parameter is used only for overload resolution.
//  };
//
//  template <class TYPE>
//  inline
//  void MyPrimitives::copyConstruct(TYPE             *address,
//                                   const TYPE&       original,
//                                   bslma::Allocator *basicAllocator)
//  {
//      copyConstruct(address,
//                    original,
//                    basicAllocator,
//                    (typename my_HasPairTrait<TYPE>::type *)0);
//  }
//..
// The implementation of 'copyConstruct' constructs the pair object in two
// steps because of the use of allocators.  We cannot simply pass the allocator
// to the copy constructor of the pair object (since 'std::pair' does not take
// an allocator).  Therefore, we copy construct 'first' and 'second' directly
// in the pair object.
//..
//  template <class TYPE>
//  inline
//  void MyPrimitives::copyConstruct(TYPE             *address,
//                                   const TYPE&       original,
//                                   bslma::Allocator *basicAllocator,
//                                   bsl::integral_constant<int, PAIR_TRAIT> *)
//  {
//      copyConstruct(&address->first, original.first, basicAllocator);
//
//      //**************************************************
//      // Note the use of the destructor proctor (below). *
//      //**************************************************
//
//      bslma::DestructorProctor<typename TYPE::firstType> proctor(
//                                                            &address->first);
//
//      copyConstruct(&address->second, original.second, basicAllocator);
//
//      //********************************************************
//      // Note that the destructor proctor is released (below). *
//      //********************************************************
//
//      proctor.release();
//  }
//
//  template <class TYPE>
//  inline
//  void MyPrimitives::copyConstruct(TYPE             *address,
//                                   const TYPE&       original,
//                                   bslma::Allocator *basicAllocator,
//                                   bsl::integral_constant<int, NIL_TRAIT> *)
//  {
//      new(address)TYPE(original, basicAllocator);
//  }
//..
// Note that the implementation of 'my_HasPairTrait' is not shown.  It is used
// to detect whether 'TYPE' has 'my_PairTrait' or not (see 'bslalg_typetraits',
// 'bslalg_typetraitpair').
//
// In the above implementation, if the copy construction of the second object
// in the pair throws, all memory (and any other resources) acquired as a
// result of copying the (not-yet-managed) object would be leaked.  Using the
// 'bslma::DestructorProctor' prevents the leaks by invoking the destructor of
// the proctored object automatically should the proctor go out of scope before
// the 'release' method of the proctor is called (such as when the function
// exits prematurely due to an exception).
//
// Note that the 'copyConstruct' method assumes the copy constructor of
// 'TYPE::firstType' and 'TYPE::secondType' takes an allocator as a second
// argument.  In production code, a constructor proxy that checks the traits of
// 'TYPE::firstType' and 'TYPE::secondType' (to determine whether they uses
// 'bslma::Allocator') should be used (see 'bslalg_constructorproxy').

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DESTRUCTIONUTIL
#include <bslma_destructionutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // =======================
                        // class DestructorProctor
                        // =======================

template <class TYPE>
class DestructorProctor {
    // This class implements a proctor that, unless its 'release' method has
    // previously been invoked, automatically destroys a managed object upon
    // destruction by invoking the (managed) object's destructor.

    // DATA
    TYPE *d_object_p;  // managed object

    // NOT IMPLEMENTED
    DestructorProctor(const DestructorProctor&);
    DestructorProctor& operator=(const DestructorProctor&);

  public:
    // CREATORS
    explicit DestructorProctor(TYPE *object);
        // Create a destructor proctor that conditionally manages the specified
        // 'object' (if non-zero) by invoking the destructor of the object
        // managed by this proctor (if not released -- see 'release') upon
        // destruction.

    ~DestructorProctor();
        // Destroy this destructor proctor, and destroy the object it manages
        // (if any) by invoking the destructor of the (managed) object.  If no
        // object is currently being managed, this method has no effect.

    // MANIPULATORS
    void release();
        // Release from management the object currently managed by this
        // proctor.  If no object is currently being managed, this method has
        // no effect.

    void reset(TYPE *object);
        // Set the specified 'object' as the object to be managed by this
        // proctor.  The behavior is undefined unless 'object' is non-zero.
        // Note that this method releases any previously-managed object from
        // management (without destroying it), and so may be invoked with or
        // without having called 'release' when reusing this object.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // -----------------------
                        // class DestructorProctor
                        // -----------------------

// CREATORS
template <class TYPE>
inline
DestructorProctor<TYPE>::DestructorProctor(TYPE *object)
: d_object_p(object)
{
}

template <class TYPE>
inline
DestructorProctor<TYPE>::~DestructorProctor()
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 != d_object_p)) {
        DestructionUtil::destroy(d_object_p);
    }
}

// MANIPULATORS
template <class TYPE>
inline
void DestructorProctor<TYPE>::release()
{
    d_object_p = 0;
}

template <class TYPE>
inline
void DestructorProctor<TYPE>::reset(TYPE *object)
{
    BSLS_ASSERT_SAFE(object);

    d_object_p = object;
}

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
