// bslma_deleterhelper.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMA_DELETERHELPER
#define INCLUDED_BSLMA_DELETERHELPER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide namespace for functions used to delete objects.
//
//@CLASSES:
//  bslma::DeleterHelper: non-primitive pure procedures for deleting objects
//
//@SEE_ALSO: bslma_rawdeleterguard, bslmf_ispolymporphic
//
//@DESCRIPTION: This component provides non-primitive procedures used to delete
// objects of parameterized 'TYPE' by first calling the destructor of the
// object, and then freeing the memory footprint of the object using a
// parameterized 'ALLOCATOR' (allocator or pool) provided as a second argument.
// The "raw" method ('deleteObjectRaw') should be used only if we are sure that
// the supplied object is !not! of a type that is a secondary base class --
// i.e., the object's address is (numerically) the same as when it was
// originally dispensed by 'ALLOCATOR'.  The non-"raw" 'deleteObject' has no
// such restriction.  Note that this component will fail to compile when
// instantiated for a class that gives a false-positive for the type trait
// 'bsl::is_polymorphic'.  See the 'bslmf_ispolymporphic' component for more
// details.
//
///Usage
///-----
// The following 'my_RawDeleterGuard' class defines a guard that
// unconditionally deletes a managed object upon destruction.  Via the
// 'deleteObjectRaw' method supplied by this component, the guard's destructor
// first destroys the managed object, then deallocates the footprint of the
// object.  The declaration of 'my_RawDeleterGuard' follows:
//..
//  template <class TYPE, class ALLOCATOR>
//  class my_RawDeleterGuard {
//      // This class implements a guard that unconditionally deletes a managed
//      // object upon destruction by first invoking the object's destructor,
//      // and then invoking the 'deallocate' method of an allocator (or pool)
//      // of parameterized 'ALLOCATOR' type supplied at construction.
//
//      // DATA
//      TYPE      *d_object_p;     // managed object
//      ALLOCATOR *d_allocator_p;  // allocator or pool (held, not owned)
//
//      // NOT IMPLEMENTED
//      my_RawDeleterGuard(const my_RawDeleterGuard&);
//      my_RawDeleterGuard& operator=(const my_RawDeleterGuard&);
//
//    public:
//      // CREATORS
//      my_RawDeleterGuard(TYPE *object, ALLOCATOR *allocator);
//          // Create a raw deleter guard that unconditionally manages the
//          // specified 'object', and that uses the specified 'allocator' to
//          // delete 'object' upon the destruction of this guard.  The
//          // behavior is undefined unless 'object' and 'allocator' are
//          // non-zero, and 'allocator' supplied the memory for 'object'.
//          // Note that 'allocator' must remain valid throughout the lifetime
//          // of this guard.
//
//      ~my_RawDeleterGuard();
//          // Destroy this raw deleter guard and delete the object it manages
//          // by first invoking the destructor of the (managed) object, and
//          // then invoking the 'deallocate' method of the allocator (or pool)
//          // that was supplied with the object at construction.
//  };
//..
// The 'deleteObjectRaw' method is used in the destructor as follows:
//..
//  template <class TYPE, class ALLOCATOR>
//  inline
//  my_RawDeleterGuard<TYPE, ALLOCATOR>::~my_RawDeleterGuard()
//  {
//      bslma::DeleterHelper::deleteObjectRaw(d_object_p, d_allocator_p);
//  }
//..
// Note that we have denoted our guard to be a "raw" guard in keeping with this
// use of 'deleteObjectRaw' (as opposed to 'deleteObject').

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOLYMORPHIC
#include <bslmf_ispolymorphic.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

namespace bslma {

             // =================================
             // local struct DeleterHelper_Helper
             // =================================

template <int IS_POLYMORPHIC>
struct DeleterHelper_Helper {
    template <class TYPE>
    static void *caster(const TYPE *object)
    {
        return static_cast<void *>(const_cast<TYPE *>(object));
    }
};

template <>
struct DeleterHelper_Helper<1> {
    template <class TYPE>
    static void *caster(const TYPE *object)
    {
        return dynamic_cast<void *>(const_cast<TYPE *>(object));
    }
};

                           // ====================
                           // struct DeleterHelper
                           // ====================

struct DeleterHelper {
    // This struct provides a namespace for helper functions used for deleting
    // objects in various pools and allocators.

    // CLASS METHODS
    template <class TYPE, class ALLOCATOR>
    static void deleteObject(const TYPE *object, ALLOCATOR *allocator);
        // Destroy the specified 'object' based on its dynamic type and then
        // use the specified 'allocator' to deallocate its memory footprint.
        // Do nothing if 'object' is a null pointer.  The behavior is undefined
        // unless 'allocator' is non-null, and 'object', when cast
        // appropriately to 'void *', was allocated using 'allocator' and has
        // not already been deallocated.  Note that
        // 'dynamic_cast<void *>(object)' is applied if 'TYPE' is polymorphic,
        // and 'static_cast<void *>(object)' is applied otherwise.

    template <class TYPE, class ALLOCATOR>
    static void deleteObjectRaw(const TYPE *object, ALLOCATOR *allocator);
        // Destroy the specified 'object' and then use the specified
        // 'allocator' to deallocate its memory footprint.  Do nothing if
        // 'object' is a null pointer.  The behavior is undefined unless
        // 'allocator' is non-null, 'object' is !not! a secondary base class
        // pointer (i.e., the address is (numerically) the same as when it was
        // originally dispensed by 'allocator'), and 'object' was allocated
        // using 'allocator' and has not already been deallocated.
};

// ============================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                           // --------------------
                           // struct DeleterHelper
                           // --------------------

// CLASS METHODS
template <class TYPE, class ALLOCATOR>
inline
void DeleterHelper::deleteObject(const TYPE *object,
                                 ALLOCATOR  *allocator)
{
    BSLS_ASSERT_SAFE(allocator);

    if (0 != object) {
        void *address = DeleterHelper_Helper<
                            bsl::is_polymorphic<TYPE>::value>::caster(object);
        BSLS_ASSERT_OPT(address);

#ifndef BSLS_PLATFORM_CMP_SUN
        object->~TYPE();
#else
        const_cast<TYPE *>(object)->~TYPE();
#endif

        allocator->deallocate(address);
    }
}

template <class TYPE, class ALLOCATOR>
inline
void DeleterHelper::deleteObjectRaw(const TYPE *object,
                                    ALLOCATOR  *allocator)
{
    BSLS_ASSERT_SAFE(allocator);

    if (0 != object) {
        void *address = const_cast<TYPE *>(object);

#ifndef BSLS_PLATFORM_CMP_SUN
        object->~TYPE();
#else
        const_cast<TYPE *>(object)->~TYPE();
#endif

        allocator->deallocate(address);
    }
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
