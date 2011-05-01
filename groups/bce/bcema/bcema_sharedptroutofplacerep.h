// bcema_sharedptroutofplacerep.h                                    -*-C++-*-
#ifndef INCLUDED_BCEMA_SHAREDPTROUTOFPLACEREP
#define INCLUDED_BCEMA_SHAREDPTROUTOFPLACEREP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an out-of-place implementation of 'bcema_SharedPtrRep'.
//
//@CLASSES:
//  bcema_SharedPtrOutofplaceRep: out-of-place shared ptr implementation
//
//@AUTHOR: Ilougino Rocha (irocha)
//         Herve Bronnimann (hbronnim)
//         Vlad Kliatchko (vkliatch)
//         Raymond Chiu (schiu49)
//
//@SEE_ALSO: bcema_sharedptr, bcema_sharedptrrep, bcema_sharedptrinplacerep
//
//@DESCRIPTION: This component provides a concrete implementation of
// 'bcema_SharedPtrRep' for managing objects of the parameterized 'TYPE' that
// are stored outside of the representation.  When all references to the
// out-of-place object are released using 'releaseRef', the deleter of the
// parameterized 'DELETER' type is invoked to delete the shared object.
//
///Thread-Safety
///-------------
// 'bcema_SharedPtrOutofplaceRep' is thread-safe provided that 'disposeObject'
// and 'disposeRep' are not called explicitly, meaning that all non-creator
// operations other than 'disposeObject' and 'disposeRep' on a given instance
// can be safely invoked simultaneously from multiple threads ('disposeObject'
// and 'disposeRep' are meant to be invoked only by 'releaseRef' and
// 'releaseWeakRef').  Note that there is no thread safety guarantees for
// operations on the managed object.
//
///Deleters
///--------
// When the last shared reference to a shared object is released, the object
// is destroyed using the "deleter" provided when the associated shared
// pointer representation was created.  'bcema_SharedPtrOutofplaceRep'
// supports two kinds of "deleter" objects, which vary in how they are invoked.
// A "function-like" deleter is any language entity that can be invoked such
// that the expression 'deleterInstance(objectPtr)' is a valid expression, and
// a "factory" deleter is any language entity that can be invoked such that the
// expression 'deleterInstance.deleteObject(objectPtr)' is a valid expression,
// where 'deleterInstance' is an instance of the "deleter" object, and
// 'objectPtr' is a pointer to the shared object.  In summary:
//..
//  Deleter                     Expression used to destroy 'objectPtr'
//  - - - - - - - -             - - - - - - - - - - - - - - - - - - -
//  "function-like"             deleterInstance(objectPtr);
//  "factory"                   deleterInstance.deleteObject(objectPtr);
//..
// The following are examples of function-like deleters that delete an object
// of 'MyType':
//..
//  void deleteObject(MyType *object);
//      // Delete the specified 'object'.
//
//  void releaseObject(MyType *object);
//      // Release the specified 'object'.
//
//  struct FunctionLikeDeleterObject {
//      // This 'struct' provides an 'operator()' that can be used to delete a
//      // 'MyType' object.
//
//      void operator()(MyType *object);
//          // Destroy the specified 'object'.
//  };
//..
// The following on the other hand is an example of a factory deleter:
//..
//  class MyFactory {
//
//     // . . .
//
//     // MANIPULATORS
//     MyType *createObject(bslma_Allocator *basicAllocator = 0);
//         // Create a 'MyType' object.  Optionally specify a
//         // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//         // 0, the currently installed default allocator is used.
//
//     void deleteObject(MyType *object);
//         // Delete the specified 'object'.
//  };
//..
// Note that 'deleteObject' is provided by all 'bslma' allocators and by any
// object that implements the 'bcema_Deleter' protocol.  Thus, any of these
// objects can be used as a factory deleter.  The purpose of this design is to
// allow 'bslma' allocators and factories to be used seamlessly as deleters.
//
// The selection of which expression is used by 'bcema_SharedPtrOutofplaceRep'
// to destroy a shared object is based on how the deleter is passed to the
// shared pointer object: Deleters that are passed by *address* are assumed to
// be factory deleters, while those that are passed by *value* are assumed to
// be function-like.  Note that if the wrong interface is used for a deleter,
// i.e., if a function-like deleter is passed by pointer, or a factory deleter
// is passed by value, and the expression used to delete the object is invalid,
// a compiler diagnostic will be emitted indicating the error.
//
///Usage
///-----
// The following example demonstrates how to implement a shared 'bdet_Datetime'
// object using 'bcema_SharedPtrOutofplaceRep':
//..
//  class MySharedDatetimePtr {
//      // This class provide a reference counted smart pointer to support
//      // shared ownership of a 'bdet_Datetime' object.
//
//    private:
//      bdet_Datetime      *d_ptr_p;  // pointer to the managed object
//      bcema_SharedPtrRep *d_rep_p;  // pointer to the representation object
//
//    private:
//      // NOT IMPLEMENTED
//      MySharedDatetimePtr& operator=(const MySharedDatetimePtr&);
//
//    public:
//      // CREATORS
//      MySharedDatetimePtr(bdet_Datetime   *ptr,
//                          bslma_Allocator *basicAllocator = 0);
//          // Create a 'MySharedDatetimePtr' object to managed the specified
//          // 'ptr'.  Optionally specify an 'basicAllocator' to allocate and
//          // deallocate the internal representation and to destroy 'ptr' when
//          // all references have been released.  The behavior is undefined
//          // unless 'ptr' was allocated using memory supplied by
//          // 'basicAllocator'.
//
//      MySharedDatetimePtr(const MySharedDatetimePtr& original);
//          // Create a shared datetime that refers to the same object managed
//          // by the specified 'original'
//
//      ~MySharedDatetimePtr();
//          // Destroy this shared datetime and release the reference to the
//          // 'bdet_Datetime' object to which it might be referring.  If this
//          // is the last shared reference, deleted the managed object.
//
//      // MANIPULATORS
//      bdet_Datetime& operator*() const;
//          // Return a modifiable reference to the shared datetime.
//
//      bdet_Datetime *operator->() const;
//          // Return the address of the modifiable 'bdet_Datetime' to which
//          // this object refers.
//
//      bdet_Datetime *ptr() const;
//          // Return the address of the modifiable 'bdet_Datetime' to which
//          // this object refers.
//  };
//..
// Finally, we define the implementation.
//..
//  MySharedDatetimePtr::MySharedDatetimePtr(bdet_Datetime   *ptr,
//                                           bslma_Allocator *basicAllocator)
//  {
//      d_ptr_p = ptr;
//      d_rep_p =
//              bcema_SharedPtrOutofplaceRep<bdet_Datetime, bslma_Allocator*>::
//                      makeOutofplaceRep(ptr, basicAllocator, basicAllocator);
//  }
//
//  MySharedDatetimePtr::MySharedDatetimePtr(
//                                         const MySharedDatetimePtr& original)
//  : d_ptr_p(original.d_ptr_p)
//  , d_rep_p(original.d_rep_p)
//  {
//      if (d_ptr_p) {
//          d_rep_p->acquireRef();
//      } else {
//          d_rep_p = 0;
//      }
//  }
//
//  MySharedDatetimePtr::~MySharedDatetimePtr()
//  {
//      if (d_rep_p) {
//          d_rep_p->releaseRef();
//      }
//  }
//
//  bdet_Datetime& MySharedDatetimePtr::operator*() const {
//      return *d_ptr_p;
//  }
//
//  bdet_Datetime *MySharedDatetimePtr::operator->() const {
//      return d_ptr_p;
//  }
//
//  bdet_Datetime *MySharedDatetimePtr::ptr() const {
//      return d_ptr_p;
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTRREP
#include <bcema_sharedptrrep.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

namespace BloombergLP {

template <class TYPE, class DELETER>
struct bcema_SharedPtrOutofplaceRep_InitGuard;
struct bcema_SharedPtrOutofplaceRep_DeleterHelper;
struct bcema_SharedPtrOutofplaceRep_DeleterType;
template <class DELETER>
class bcema_SharedPtrOutofplaceRep_DeleterDiscriminator;

#define bcema_SharedPtr_RepImpl bcema_SharedPtrOutofplaceRep
    // For backward compatibility.  Not for use by client.

              // ===============================================
              // struct bcema_SharedPtrOutofplaceRep_DeleterType
              // ===============================================

struct bcema_SharedPtrOutofplaceRep_DeleterType {
    // This 'struct' enumerates four kinds of deleters, the first two are
    // factory deleters, and the last two are function-like deleters.

    enum {
        // Enumeration used to discriminate among the different deleters.

        BCEMA_ALLOCATOR_PTR = 0,     // Used to indicate that a deleter is a
                                     // pointer that follows the
                                     // 'bslma_Allocator' protocol.

        BCEMA_FACTORY_PTR = 1,       // Used to indicate that a deleter is a
                                     // pointer to a factory object that
                                     // implements the 'deleteObject'
                                     // protocol.

        BCEMA_FUNCTOR_WITH_ALLOC = 2,// Used to indicate that a deleter is a
                                     // functor that takes an allocator at
                                     // construction.

        BCEMA_FUNCTOR_WITHOUT_ALLOC = 3
                                     // Used to indicate that a deleter is a
                                     // functor that does not take an allocator
                                     // at construction.
    };
};

                     // ==================================
                     // class bcema_SharedPtrOutofplaceRep
                     // ==================================

template <class TYPE, class DELETER>
class bcema_SharedPtrOutofplaceRep : public bcema_SharedPtrRep {
    // This class provides a concrete implementation of the
    // 'bcema_SharedPtrRep' protocol for out-of-place instances of the
    // parameterized 'TYPE'.  Upon destruction of this object, the
    // parameterized 'DELETER' type is invoked on the pointer to the shared
    // object.

    // PRIVATE TYPES
    typedef bcema_SharedPtrOutofplaceRep_DeleterType DeleterType;
        // 'DeleterType' is an alias for the 'struct' that defines the types of
        // deleter used to destroy the shared object.

    typedef typename
      bcema_SharedPtrOutofplaceRep_DeleterDiscriminator<DELETER>::Type Deleter;
        // 'Deleter' is an alias for the type of deleter used to destroy the
        // shared object.

    // DATA
    Deleter          d_deleter;     // deleter for this out-of-place instance
    TYPE            *d_ptr_p;       // pointer to out-of-place instance (held,
                                    // not owned)
    bslma_Allocator *d_allocator_p; // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    bcema_SharedPtrOutofplaceRep(const bcema_SharedPtrOutofplaceRep&);
    bcema_SharedPtrOutofplaceRep& operator=(
                                          const bcema_SharedPtrOutofplaceRep&);

    // PRIVATE CREATORS
    bcema_SharedPtrOutofplaceRep(
                              TYPE            *ptr,
                              const DELETER&   deleter,
                              bslma_Allocator *basicAllocator,
                              bslmf_MetaInt<DeleterType::BCEMA_ALLOCATOR_PTR>);
    bcema_SharedPtrOutofplaceRep(
                                TYPE            *ptr,
                                const DELETER&   deleter,
                                bslma_Allocator *basicAllocator,
                                bslmf_MetaInt<DeleterType::BCEMA_FACTORY_PTR>);
    bcema_SharedPtrOutofplaceRep(
                         TYPE            *ptr,
                         const DELETER&   deleter,
                         bslma_Allocator *basicAllocator,
                         bslmf_MetaInt<DeleterType::BCEMA_FUNCTOR_WITH_ALLOC>);
    bcema_SharedPtrOutofplaceRep(
                      TYPE            *ptr,
                      const DELETER&   deleter,
                      bslma_Allocator *basicAllocator,
                      bslmf_MetaInt<DeleterType::BCEMA_FUNCTOR_WITHOUT_ALLOC>);
        // Create a 'bcema_SharedPtrOutofplaceRep' that manages the lifetime of
        // the specified 'ptr', using the specified 'deleter' to destroy 'ptr',
        // and using the specified 'basicAllocator' to supply memory.  Note
        // that 'basicAllocator' will be used to destroy this representation
        // object, but not necessarily to destroy 'ptr'.  Also note that
        // 'bcema_SharedPtrOutofplaceRep' should be created using
        // 'makeOutofplaceRep', which will call the appropriate private
        // constructor depending on the parameterized 'DELETER' type.

    ~bcema_SharedPtrOutofplaceRep();
        // Destroy this representation object and if the shared object has not
        // been deleted, delete the shared object using the associated deleter.
        // Note that this destructor is never called explicitly.  Instead,
        // 'disposeObject' destroys the shared object object and 'disposeRep'
        // deallocates this representation object.

  public:
    // MANIPULATORS
    static bcema_SharedPtrOutofplaceRep<TYPE, DELETER> *makeOutofplaceRep(
                                          TYPE            *ptr,
                                          const DELETER&   deleter,
                                          bslma_Allocator *basicAllocator = 0);
        // Return the address of a newly created 'bcema_SharedPtrOutofplaceRep'
        // object that manages the lifetime of the specified 'ptr', using the
        // specified 'deleter' to destroy 'ptr'.  Optionally, specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Note that the
        // parameterized 'DELETER' type will be used to deallocate the memory
        // pointed to by 'ptr'.

    virtual void disposeRep();
        // Destroy this representation object and deallocate the associated
        // memory.  This method is automatically invoked by 'releaseRef' and
        // 'releaseWeakRef' when the number of weak references and the number
        // of shared references both reach zero and should not be explicitly
        // invoked otherwise.  The behavior is undefined unless 'disposeObject'
        // has already been called for this representation.  Note that this
        // 'disposeRep' method effectively serves as the representation
        // object's destructor.

    virtual void disposeObject();
        // Destroy the object being referred to by this representation.  This
        // method is automatically invoked by 'releaseRef' when the number of
        // shared references reaches zero and should not be explicitly invoked
        // otherwise.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) address of the modifiable shared object to
        // which this object refers.

    TYPE *ptr() const;
        // Return the address of the modifiable shared object to which this
        // object refers.
};

          // =======================================================
          // class bcema_SharedPtrOutofplaceRep_DeleterDiscriminator
          // =======================================================

template <class DELETER>
class bcema_SharedPtrOutofplaceRep_DeleterDiscriminator {
    // This 'class' provides two meta-functions for determining the enumerated
    // type and the C++ type of a deleter based on whether it is a pointer to a
    // function, a pointer to a factory deleter, or an instance of a
    // function-like deleter.

    // PRIVATE TYPES
    enum {
        // Enumeration that calls meta-functions to describe properties of the
        // deleter.

        BCEMA_USES_ALLOC =
           bslalg_HasTrait<DELETER, bslalg_TypeTraitUsesBslmaAllocator>::VALUE,

        BCEMA_IS_PTR = bslmf_IsPointer<DELETER>::VALUE
                   && !bslmf_IsFunctionPointer<DELETER>::VALUE,

        BCEMA_IS_ALLOC_PTR = bslmf_IsConvertible<DELETER,
                                                 bslma_Allocator *>::VALUE
    };

    typedef bcema_SharedPtrOutofplaceRep_DeleterType DeleterType;
        // 'DeleterType' is an alias for the 'struct' that defines the types of
        // deleter used to destroy the shared object.

  public:
    // TYPES
    enum {
        // This enumeration contains the return value of the meta-function.

        VALUE = BCEMA_USES_ALLOC
                ? DeleterType::BCEMA_FUNCTOR_WITH_ALLOC
                : !BCEMA_IS_PTR
                  ? DeleterType::BCEMA_FUNCTOR_WITHOUT_ALLOC
                  : BCEMA_IS_ALLOC_PTR
                    ? DeleterType::BCEMA_ALLOCATOR_PTR
                    : DeleterType::BCEMA_FACTORY_PTR
    };

    typedef
    typename bslmf_If<BCEMA_IS_ALLOC_PTR,
                      bslma_Allocator *,
                      DELETER>::Type Type;
        // 'Type' represents the type of the deleter used to destroy the shared
        // object.
};

             // =================================================
             // struct bcema_SharedPtrOutofplaceRep_DeleterHelper
             // =================================================

struct bcema_SharedPtrOutofplaceRep_DeleterHelper {
    // This 'struct' provides utility functions to apply a deleter to a shared
    // object referred to by 'bcema_SharedPtrOutofplaceRep'.

    // PUBLIC TYPES
    typedef bcema_SharedPtrOutofplaceRep_DeleterType DeleterType;
        // 'DeleterType' is an alias for the 'struct' that defines the types of
        // deleter used to destroy the shared object.

  private:
    // PRIVATE CLASS  METHODS
    template <class TYPE, class DELETER>
    static void deleteObject(TYPE     *ptr,
                             DELETER&  deleter,
                             bslmf_MetaInt<DeleterType::BCEMA_ALLOCATOR_PTR>);
        // Delete the specified 'ptr' using the specified 'deleter' that
        // implements the 'bslma_Allocator' protocol, which provides a
        // 'deleteObject' function that can be invoked to delete 'ptr'.

    template <class TYPE, class DELETER>
    static void deleteObject(TYPE     *ptr,
                             DELETER&  deleter,
                             bslmf_MetaInt<DeleterType::BCEMA_FACTORY_PTR>);
        // Delete the specified 'ptr' using the specified 'deleter' that
        // provides a 'deleteObject' function that can be invoked to delete
        // 'ptr'.

    template <class TYPE, class DELETER>
    static void deleteObject(
                         TYPE     *ptr,
                         DELETER&  deleter,
                         bslmf_MetaInt<DeleterType::BCEMA_FUNCTOR_WITH_ALLOC>);
        // Delete the specified 'ptr' using the specified 'deleter' that is a
        // functor that takes an allocator at construction and can be invoked
        // to delete 'ptr'.

    template <class TYPE, class DELETER>
    static void deleteObject(
                      TYPE     *ptr,
                      DELETER&  deleter,
                      bslmf_MetaInt<DeleterType::BCEMA_FUNCTOR_WITHOUT_ALLOC>);
        // Delete the specified 'ptr' using the specified 'deleter' that is a
        // functor that does not take an allocator at construction and can be
        // invoked to delete 'ptr'.

  public:
    // CLASS METHODS
    template <class TYPE, class DELETER>
    static void deleteObject(TYPE *ptr, DELETER& deleter);
        // Delete the specified 'ptr' using the specified 'deleter'.
};

               // =============================================
               // struct bcema_SharedPtrOutofplaceRep_InitGuard
               // =============================================

template <class TYPE, class DELETER>
struct bcema_SharedPtrOutofplaceRep_InitGuard {
    // This guard is used for out-of-place shared pointer instantiations.
    // Generally, a guard is created prior to constructing a
    // 'bcema_SharedPtrOutofplaceRep' and released after successful
    // construction.  In the event that an exception is thrown during
    // construction of the representation, the guard will delete the provided
    // pointer using the provided deleter.  Note that the provided deleter is
    // held by reference and must remain valid for the lifetime of the guard.
    // If the guard is not released before it's destruction, a copy of the
    // deleter is instantiated to delete the pointer (in case 'operator()' is
    // non-'const').  Also note that if the deleter throws during
    // copy-construction, the provided pointer will not be destroyed.

    // DATA
    TYPE           *d_ptr_p;    // address of the managed object (held, not
                                // owned)

    const DELETER&  d_deleter;  // deleter used to destroy managed object

  public:
    // CREATORS
    bcema_SharedPtrOutofplaceRep_InitGuard(TYPE *ptr, const DELETER& deleter);
        // Create a guard referring to the specified 'ptr' and using the
        // specified 'deleter' to destroy 'ptr' when the guard is destroyed.

    ~bcema_SharedPtrOutofplaceRep_InitGuard();
        // Destroy this guard and the object (if any) referred to by this
        // guard.

    // MANIPULATORS
    void release();
        // Release from management the object referred to by this guard.
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                     // ----------------------------------
                     // class bcema_SharedPtrOutofplaceRep
                     // ----------------------------------

// CREATORS
template <class TYPE, class DELETER>
bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::bcema_SharedPtrOutofplaceRep(
                               TYPE            *ptr,
                               const DELETER&   deleter,
                               bslma_Allocator *basicAllocator,
                               bslmf_MetaInt<DeleterType::BCEMA_ALLOCATOR_PTR>)
: d_deleter(bslma_Default::allocator(deleter))
, d_ptr_p(ptr)
, d_allocator_p(basicAllocator)
{
}

template <class TYPE, class DELETER>
bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::bcema_SharedPtrOutofplaceRep(
                                 TYPE            *ptr,
                                 const DELETER&   deleter,
                                 bslma_Allocator *basicAllocator,
                                 bslmf_MetaInt<DeleterType::BCEMA_FACTORY_PTR>)
: d_deleter(deleter)
, d_ptr_p(ptr)
, d_allocator_p(basicAllocator)
{
}

template <class TYPE, class DELETER>
bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::bcema_SharedPtrOutofplaceRep(
                          TYPE            *ptr,
                          const DELETER&   deleter,
                          bslma_Allocator *basicAllocator,
                          bslmf_MetaInt<DeleterType::BCEMA_FUNCTOR_WITH_ALLOC>)
: d_deleter(deleter, basicAllocator)
, d_ptr_p(ptr)
, d_allocator_p(basicAllocator)
{
}

template <class TYPE, class DELETER>
bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::bcema_SharedPtrOutofplaceRep(
                       TYPE            *ptr,
                       const DELETER&   deleter,
                       bslma_Allocator *basicAllocator,
                       bslmf_MetaInt<DeleterType::BCEMA_FUNCTOR_WITHOUT_ALLOC>)
: d_deleter(deleter)
, d_ptr_p(ptr)
, d_allocator_p(basicAllocator)
{
}

template <class TYPE, class DELETER>
bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::~bcema_SharedPtrOutofplaceRep()
{
}

// MANIPULATORS
template <class TYPE, class DELETER>
bcema_SharedPtrOutofplaceRep<TYPE, DELETER> *
               bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::makeOutofplaceRep(
                                               TYPE            *ptr,
                                               const DELETER&   deleter,
                                               bslma_Allocator *basicAllocator)
{
    bcema_SharedPtrOutofplaceRep_InitGuard<TYPE, DELETER> guard(ptr, deleter);

    enum { BCEMA_DELETER_TYPE =
           bcema_SharedPtrOutofplaceRep_DeleterDiscriminator<DELETER>::VALUE };

    bcema_SharedPtrOutofplaceRep<TYPE, DELETER> *rep = 0;

    if (ptr) {
        basicAllocator = bslma_Default::allocator(basicAllocator);
        rep = new (*basicAllocator)
                                   bcema_SharedPtrOutofplaceRep<TYPE, DELETER>(
                                          ptr,
                                          deleter,
                                          basicAllocator,
                                          bslmf_MetaInt<BCEMA_DELETER_TYPE>());
    }

    guard.release();

    return rep;
}

template <class TYPE, class DELETER>
inline
void bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::disposeRep()
{
    // If 'd_allocator_p->deleteObject' is used to destroy the
    // 'bcema_SharedPtrOutofplaceRep' object, a virtual function call will be
    // used and a 'dynamic_cast' is required to obtain the address of the most
    // derived object to deallocate it.  Knowing 'bcema_SharedPtrOutofplaceRep'
    // is the most derived class, this unnecessary overhead of a virtual
    // function call can be avoided by explicitly calling the destructor.  This
    // behavior is guaranteed by the standard ([class.virtual] 13: "Explicit
    // qualification with the scope operator (5.1) suppresses the virtual call
    // mechanism.", page 224 of Working Draft 2007-10).

    this->bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::
                                              ~bcema_SharedPtrOutofplaceRep();
    d_allocator_p->deallocate(this);
}

template <class TYPE, class DELETER>
void bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::disposeObject()
{
    bcema_SharedPtrOutofplaceRep_DeleterHelper::deleteObject(d_ptr_p,
                                                             d_deleter);
    d_ptr_p = 0;
}

// ACCESSORS
template <class TYPE, class DELETER>
inline
void *bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::originalPtr() const
{
    return const_cast<void *>(static_cast<const void *>(d_ptr_p));
}

template <class TYPE, class DELETER>
inline
TYPE *bcema_SharedPtrOutofplaceRep<TYPE, DELETER>::ptr() const
{
    return d_ptr_p;
}

             // -------------------------------------------------
             // struct bcema_SharedPtrOutofplaceRep_DeleterHelper
             // -------------------------------------------------

// CLASS METHODS
template <class TYPE, class DELETER>
inline
void bcema_SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                               TYPE     *ptr,
                               DELETER&  deleter,
                               bslmf_MetaInt<DeleterType::BCEMA_ALLOCATOR_PTR>)
{
    deleter->deleteObject(ptr);
}

template <class TYPE, class DELETER>
inline
void bcema_SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                                 TYPE     *ptr,
                                 DELETER&  deleter,
                                 bslmf_MetaInt<DeleterType::BCEMA_FACTORY_PTR>)
{
    deleter->deleteObject(ptr);
}

template <class TYPE, class DELETER>
inline
void bcema_SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                          TYPE     *ptr,
                          DELETER&  deleter,
                          bslmf_MetaInt<DeleterType::BCEMA_FUNCTOR_WITH_ALLOC>)
{
    deleter(ptr);
}

template <class TYPE, class DELETER>
inline
void bcema_SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                       TYPE     *ptr,
                       DELETER&  deleter,
                       bslmf_MetaInt<DeleterType::BCEMA_FUNCTOR_WITHOUT_ALLOC>)
{
    deleter(ptr);
}

template <class TYPE, class DELETER>
inline
void bcema_SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                                                             TYPE     *ptr,
                                                             DELETER&  deleter)
{
    enum { BCEMA_DELETER_TYPE =
           bcema_SharedPtrOutofplaceRep_DeleterDiscriminator<DELETER>::VALUE };

    bcema_SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                                          ptr,
                                          deleter,
                                          bslmf_MetaInt<BCEMA_DELETER_TYPE>());
}

               // ---------------------------------------------
               // struct bcema_SharedPtrOutofplaceRep_InitGuard
               // ---------------------------------------------

// CREATORS
template <class TYPE, class DELETER>
inline
bcema_SharedPtrOutofplaceRep_InitGuard<TYPE, DELETER>
              ::bcema_SharedPtrOutofplaceRep_InitGuard(TYPE           *ptr,
                                                       const DELETER&  deleter)
: d_ptr_p(ptr)
, d_deleter(deleter)
{
}

template <class TYPE, class DELETER>
inline
bcema_SharedPtrOutofplaceRep_InitGuard<TYPE, DELETER>
                                    ::~bcema_SharedPtrOutofplaceRep_InitGuard()
{
    // The definition of this function is intentionally *not* written as:
    //..
    //  if (d_ptr_p) {
    //      DELETER tempDeleter(d_deleter);
    //      bcema_SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
    //                                                            d_ptr_p,
    //                                                            tempDeleter);
    //  }
    //..
    // to work around a CC (Studio 12v4 on Solaris) compilation failure when
    // optimizations are enabled ('opt_exc_mt' and 'opt_exc_mt_64').

    if (!d_ptr_p) {
        return;                                                       // RETURN
    }

    DELETER tempDeleter(d_deleter);
    bcema_SharedPtrOutofplaceRep_DeleterHelper::deleteObject(d_ptr_p,
                                                             tempDeleter);
}

// MANIPULATORS
template <class TYPE, class DELETER>
inline
void bcema_SharedPtrOutofplaceRep_InitGuard<TYPE, DELETER>::release()
{
    d_ptr_p = 0;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
