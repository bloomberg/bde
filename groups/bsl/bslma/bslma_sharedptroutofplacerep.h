// bslma_sharedptroutofplacerep.h                                     -*-C++-*-
#ifndef INCLUDED_BSLMA_SHAREDPTROUTOFPLACEREP
#define INCLUDED_BSLMA_SHAREDPTROUTOFPLACEREP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an out-of-place implementation of 'bslma::SharedPtrRep'.
//
//@CLASSES:
//  bslma::SharedPtrOutofplaceRep: out-of-place shared ptr implementation
//
//@SEE_ALSO: bslma_sharedptr, bslma_sharedptrrep, bslma_sharedptrinplacerep
//
//@DESCRIPTION: This component provides a concrete implementation of
// 'bslma::SharedPtrRep' for managing objects of the parameterized 'TYPE' that
// are stored outside of the representation.  When all references to the
// out-of-place object are released using 'releaseRef', the deleter of the
// parameterized 'DELETER' type is invoked to delete the shared object.
//
///Thread Safety
///-------------
// 'bslma::SharedPtrOutofplaceRep' is thread-safe provided that 'disposeObject'
// and 'disposeRep' are not called explicitly, meaning that all non-creator
// operations other than 'disposeObject' and 'disposeRep' on a given instance
// can be safely invoked simultaneously from multiple threads ('disposeObject'
// and 'disposeRep' are meant to be invoked only by 'releaseRef' and
// 'releaseWeakRef').  Note that there is no thread safety guarantees for
// operations on the managed object.
//
///Deleters
///--------
// When the last shared reference to a shared object is released, the object is
// destroyed using the "deleter" provided when the associated shared pointer
// representation was created.  'bslma::SharedPtrOutofplaceRep' supports two
// kinds of "deleter" objects, which vary in how they are invoked.  A
// "function-like" deleter is any language entity that can be invoked such that
// the expression 'deleterInstance(objectPtr)' is a valid expression, and a
// "factory" deleter is any language entity that can be invoked such that the
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
//     MyType *createObject(bslma::Allocator *basicAllocator = 0);
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
// The selection of which expression is used by 'bslma::SharedPtrOutofplaceRep'
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
// object using 'bslma::SharedPtrOutofplaceRep':
//..
//  class MySharedDatetimePtr {
//      // This class provide a reference counted smart pointer to support
//      // shared ownership of a 'bdet_Datetime' object.
//
//    private:
//      bdet_Datetime      *d_ptr_p;  // pointer to the managed object
//      bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object
//
//    private:
//      // NOT IMPLEMENTED
//      MySharedDatetimePtr& operator=(const MySharedDatetimePtr&);
//
//    public:
//      // CREATORS
//      MySharedDatetimePtr(bdet_Datetime    *ptr,
//                          bslma::Allocator *basicAllocator = 0);
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
//  MySharedDatetimePtr::MySharedDatetimePtr(bdet_Datetime    *ptr,
//                                           bslma::Allocator *basicAllocator)
//  {
//      d_ptr_p = ptr;
//      d_rep_p =
//           bslma::SharedPtrOutofplaceRep<bdet_Datetime, bslma::Allocator *>::
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMA_SHAREDPTRREP
#include <bslma_sharedptrrep.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
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

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_TYPEINFO
#include <typeinfo>
#define INCLUDED_TYPEINFO
#endif

namespace BloombergLP {
namespace bslma {

template <class TYPE, class DELETER>
struct SharedPtrOutofplaceRep_InitProctor;
struct SharedPtrOutofplaceRep_DeleterHelper;
struct SharedPtrOutofplaceRep_DeleterType;
template <class DELETER>
class SharedPtrOutofplaceRep_DeleterDiscriminator;

              // =========================================
              // struct SharedPtrOutofplaceRep_DeleterType
              // =========================================

struct SharedPtrOutofplaceRep_DeleterType {
    // This 'struct' enumerates four kinds of deleters, the first two are
    // factory deleters, and the last two are function-like deleters.

    enum {
        // Enumeration used to discriminate among the different deleters.

        BSLMA_ALLOCATOR_PTR = 0,     // Used to indicate that a deleter is a
                                     // pointer that follows the 'Allocator'
                                     // protocol.

        BSLMA_FACTORY_PTR = 1,       // Used to indicate that a deleter is a
                                     // pointer to a factory object that
                                     // implements the 'deleteObject' protocol.

        BSLMA_FUNCTOR_WITH_ALLOC = 2,// Used to indicate that a deleter is a
                                     // functor that takes an allocator at
                                     // construction.

        BSLMA_FUNCTOR_WITHOUT_ALLOC = 3
                                     // Used to indicate that a deleter is a
                                     // functor that does not take an allocator
                                     // at construction.
    };
};

                     // ============================
                     // class SharedPtrOutofplaceRep
                     // ============================

template <class TYPE, class DELETER>
class SharedPtrOutofplaceRep : public SharedPtrRep {
    // This class provides a concrete implementation of the 'SharedPtrRep'
    // protocol for out-of-place instances of the parameterized 'TYPE'.  Upon
    // destruction of this object, the parameterized 'DELETER' type is invoked
    // on the pointer to the shared object.

    // PRIVATE TYPES
    typedef SharedPtrOutofplaceRep_DeleterType DeleterType;
        // 'DeleterType' is an alias for the 'struct' that defines the types of
        // deleter used to destroy the shared object.

    typedef typename
      SharedPtrOutofplaceRep_DeleterDiscriminator<DELETER>::Type Deleter;
        // 'Deleter' is an alias for the type of deleter used to destroy the
        // shared object.

    // DATA
    Deleter           d_deleter;     // deleter for this out-of-place instance
    TYPE             *d_ptr_p;       // pointer to out-of-place instance (held,
                                     // not owned)
    Allocator *d_allocator_p; // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    SharedPtrOutofplaceRep(const SharedPtrOutofplaceRep&);
    SharedPtrOutofplaceRep& operator=(const SharedPtrOutofplaceRep&);

    // PRIVATE CREATORS
    SharedPtrOutofplaceRep(TYPE           *ptr,
                           const DELETER&  deleter,
                           Allocator      *basicAllocator,
                           bslmf::MetaInt<DeleterType::BSLMA_ALLOCATOR_PTR>);
    SharedPtrOutofplaceRep(TYPE           *ptr,
                           const DELETER&  deleter,
                           Allocator      *basicAllocator,
                           bslmf::MetaInt<DeleterType::BSLMA_FACTORY_PTR>);
    SharedPtrOutofplaceRep(TYPE           *ptr,
                           const DELETER&  deleter,
                           Allocator      *basicAllocator,
                        bslmf::MetaInt<DeleterType::BSLMA_FUNCTOR_WITH_ALLOC>);
    SharedPtrOutofplaceRep(TYPE           *ptr,
                           const DELETER&  deleter,
                           Allocator      *basicAllocator,
                     bslmf::MetaInt<DeleterType::BSLMA_FUNCTOR_WITHOUT_ALLOC>);
        // Create a 'SharedPtrOutofplaceRep' that manages the lifetime of the
        // specified 'ptr', using the specified 'deleter' to destroy 'ptr', and
        // using the specified 'basicAllocator' to supply memory.  Note that
        // 'basicAllocator' will be used to destroy this representation object,
        // but not necessarily to destroy 'ptr'.  Also note that
        // 'SharedPtrOutofplaceRep' should be created using
        // 'makeOutofplaceRep', which will call the appropriate private
        // constructor depending on the parameterized 'DELETER' type.

    ~SharedPtrOutofplaceRep();
        // Destroy this representation object and if the shared object has not
        // been deleted, delete the shared object using the associated deleter.
        // Note that this destructor is never called explicitly.  Instead,
        // 'disposeObject' destroys the shared object object and 'disposeRep'
        // deallocates this representation object.

  public:
    // CLASS METHODS
    static SharedPtrOutofplaceRep<TYPE, DELETER> *makeOutofplaceRep(
                                           TYPE           *ptr,
                                           const DELETER&  deleter,
                                           Allocator      *basicAllocator = 0);
        // Return the address of a newly created 'SharedPtrOutofplaceRep'
        // object that manages the lifetime of the specified 'ptr', using the
        // specified 'deleter' to destroy 'ptr'.  Optionally, specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Note that the
        // parameterized 'DELETER' type will be used to deallocate the memory
        // pointed to by 'ptr'.

    // MANIPULATORS
    virtual void disposeObject();
        // Destroy the object being referred to by this representation.  This
        // method is automatically invoked by 'releaseRef' when the number of
        // shared references reaches zero and should not be explicitly invoked
        // otherwise.

    virtual void disposeRep();
        // Destroy this representation object and deallocate the associated
        // memory.  This method is automatically invoked by 'releaseRef' and
        // 'releaseWeakRef' when the number of weak references and the number
        // of shared references both reach zero and should not be explicitly
        // invoked otherwise.  The behavior is undefined unless 'disposeObject'
        // has already been called for this representation.  Note that this
        // 'disposeRep' method effectively serves as the representation
        // object's destructor.

    virtual void *getDeleter(const std::type_info& type);
        // Return a pointer to the deleter stored by the derived representation
        // (if any) if the deleter has the same type as that described by the
        // specified 'type', and a null pointer otherwise.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) address of the modifiable shared object to
        // which this object refers.

    TYPE *ptr() const;
        // Return the address of the modifiable shared object to which this
        // object refers.
};

          // =================================================
          // class SharedPtrOutofplaceRep_DeleterDiscriminator
          // =================================================

template <class DELETER, bool IS_ALLOC_PTR>
class SharedPtrOutofplaceRep_DeleterDiscriminator_Imp {
    // This 'class' provides the implementation of the
    // 'SharedPtrOutofplaceRep_DeleterDiscriminator' for the 'DELETER' template
    // parameter type which is not 'Allocator *'.

    // PRIVATE TYPES
    enum {
        // Enumeration that calls meta-functions to describe properties of the
        // deleter.

        BSLMA_USES_ALLOC = UsesBslmaAllocator<DELETER>::value,

        BSLMA_IS_OBJ_PTR = bsl::is_pointer<DELETER>::value
                       && !bslmf::IsFunctionPointer<DELETER>::value
    };

    typedef SharedPtrOutofplaceRep_DeleterType DeleterType;
        // 'DeleterType' is an alias for the 'struct' that defines the types of
        // deleter used to destroy the shared object.

  public:
    // TYPES
    enum {
        // This enumeration contains the return value of the meta-function.
        VALUE = BSLMA_USES_ALLOC
                ? DeleterType::BSLMA_FUNCTOR_WITH_ALLOC
                : !BSLMA_IS_OBJ_PTR
                  ? DeleterType::BSLMA_FUNCTOR_WITHOUT_ALLOC
                  : DeleterType::BSLMA_FACTORY_PTR
    };

    typedef typename bsl::conditional<bsl::is_function<DELETER>::value,
                                      DELETER *,
                                      DELETER>::type Type;
        // 'Type' represents the type of the deleter used to destroy the shared
        // object.
};

template <class DELETER>
class SharedPtrOutofplaceRep_DeleterDiscriminator_Imp<DELETER, true> {
    // This 'class' provides the implementation of the
    // 'SharedPtrOutofplaceRep_DeleterDiscriminator' for the 'DELETER' template
    // parameter type which is 'Allocator *'.

    // PRIVATE TYPES
    typedef SharedPtrOutofplaceRep_DeleterType DeleterType;
        // 'DeleterType' is an alias for the 'struct' that defines the types of
        // deleter used to destroy the shared object.

  public:
    // TYPES
    enum {
        // This enumeration contains the return value of the meta-function.
        VALUE = DeleterType::BSLMA_ALLOCATOR_PTR
    };

    typedef Allocator *Type;
        // 'Type' represents the type of the deleter used to destroy the shared
        // object.
};

template <class DELETER>
class SharedPtrOutofplaceRep_DeleterDiscriminator {
    // This 'class' provides two meta-functions for determining the enumerated
    // type and the C++ type of a deleter based on whether it is a pointer to a
    // function, a pointer to a factory deleter, or an instance of a
    // function-like deleter.

    // PRIVATE TYPES
    typedef SharedPtrOutofplaceRep_DeleterDiscriminator_Imp<
                              DELETER,
                              bsl::is_convertible<DELETER, Allocator *>::value>
        ImpType;

  public:
    // TYPES
    enum {
        // This enumeration contains the return value of the meta-function.
        VALUE = ImpType::VALUE
    };

    typedef typename ImpType::Type Type;
        // 'Type' represents the type of the deleter used to destroy the shared
        // object.
};

             // ===========================================
             // struct SharedPtrOutofplaceRep_DeleterHelper
             // ===========================================

struct SharedPtrOutofplaceRep_DeleterHelper {
    // This 'struct' provides utility functions to apply a deleter to a shared
    // object referred to by 'SharedPtrOutofplaceRep'.

    // PUBLIC TYPES
    typedef SharedPtrOutofplaceRep_DeleterType DeleterType;
        // 'DeleterType' is an alias for the 'struct' that defines the types of
        // deleter used to destroy the shared object.

  private:
    // PRIVATE CLASS METHODS
    template <class TYPE, class DELETER>
    static void deleteObject(TYPE     *ptr,
                             DELETER&  deleter,
                             bslmf::MetaInt<DeleterType::BSLMA_ALLOCATOR_PTR>);
        // Delete the specified 'ptr' using the specified 'deleter' that
        // implements the 'Allocator' protocol, which provides a 'deleteObject'
        // function that can be invoked to delete 'ptr'.

    template <class TYPE, class DELETER>
    static void deleteObject(TYPE     *ptr,
                             DELETER&  deleter,
                             bslmf::MetaInt<DeleterType::BSLMA_FACTORY_PTR>);
        // Delete the specified 'ptr' using the specified 'deleter' that
        // provides a 'deleteObject' function that can be invoked to delete
        // 'ptr'.

    template <class TYPE, class DELETER>
    static void deleteObject(
                        TYPE     *ptr,
                        DELETER&  deleter,
                        bslmf::MetaInt<DeleterType::BSLMA_FUNCTOR_WITH_ALLOC>);
        // Delete the specified 'ptr' using the specified 'deleter' that is a
        // functor that takes an allocator at construction and can be invoked
        // to delete 'ptr'.

    template <class TYPE, class DELETER>
    static void deleteObject(
                     TYPE     *ptr,
                     DELETER&  deleter,
                     bslmf::MetaInt<DeleterType::BSLMA_FUNCTOR_WITHOUT_ALLOC>);
        // Delete the specified 'ptr' using the specified 'deleter' that is a
        // functor that does not take an allocator at construction and can be
        // invoked to delete 'ptr'.

  public:
    // CLASS METHODS
    template <class TYPE, class DELETER>
    static void deleteObject(TYPE *ptr, DELETER& deleter);
        // Delete the specified 'ptr' using the specified 'deleter'.
};

               // =========================================
               // struct SharedPtrOutofplaceRep_InitProctor
               // =========================================

template <class TYPE, class DELETER>
struct SharedPtrOutofplaceRep_InitProctor {
    // This proctor is used for out-of-place shared pointer instantiations.
    // Generally, a proctor is created prior to constructing a
    // 'SharedPtrOutofplaceRep' and released after successful construction.  In
    // the event that an exception is thrown during construction of the
    // representation, the proctor will delete the provided pointer using the
    // provided deleter.  Note that the provided deleter is held by reference
    // and must remain valid for the lifetime of the proctor.  If the proctor
    // is not released before it's destruction, a copy of the deleter is
    // instantiated to delete the pointer (in case 'operator()' is
    // non-'const').  Also note that if the deleter throws during
    // copy construction, the provided pointer will not be destroyed.

    // DATA
    TYPE           *d_ptr_p;    // address of the managed object (held, not
                                // owned)

    const DELETER&  d_deleter;  // deleter used to destroy managed object

  public:
    // CREATORS
    SharedPtrOutofplaceRep_InitProctor(TYPE *ptr, const DELETER& deleter);
        // Create a proctor referring to the specified 'ptr' and using the
        // specified 'deleter' to destroy 'ptr' when the proctor is destroyed.

    ~SharedPtrOutofplaceRep_InitProctor();
        // Destroy this proctor and the object (if any) referred to by this
        // proctor.

    // MANIPULATORS
    void release();
        // Release from management the object referred to by this proctor.
};

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

                     // ----------------------------
                     // class SharedPtrOutofplaceRep
                     // ----------------------------

// CLASS FUNCTIONS
template <class TYPE, class DELETER>
SharedPtrOutofplaceRep<TYPE, DELETER> *
               SharedPtrOutofplaceRep<TYPE, DELETER>::makeOutofplaceRep(
                                                TYPE           *ptr,
                                                const DELETER&  deleter,
                                                Allocator      *basicAllocator)
{
    SharedPtrOutofplaceRep_InitProctor<TYPE, DELETER> proctor(ptr, deleter);

    enum { BSLMA_DELETER_TYPE =
                 SharedPtrOutofplaceRep_DeleterDiscriminator<DELETER>::VALUE };

    SharedPtrOutofplaceRep<TYPE, DELETER> *rep = 0;

    basicAllocator = Default::allocator(basicAllocator);
    rep = new (*basicAllocator) SharedPtrOutofplaceRep(
                                         ptr,
                                         deleter,
                                         basicAllocator,
                                         bslmf::MetaInt<BSLMA_DELETER_TYPE>());

    proctor.release();

    return rep;
}

// CREATORS
template <class TYPE, class DELETER>
SharedPtrOutofplaceRep<TYPE, DELETER>::SharedPtrOutofplaceRep(
                              TYPE           *ptr,
                              const DELETER&  deleter,
                              Allocator      *basicAllocator,
                              bslmf::MetaInt<DeleterType::BSLMA_ALLOCATOR_PTR>)
: d_deleter(Default::allocator(deleter))
, d_ptr_p(ptr)
, d_allocator_p(basicAllocator)
{
}

template <class TYPE, class DELETER>
SharedPtrOutofplaceRep<TYPE, DELETER>::SharedPtrOutofplaceRep(
                                TYPE           *ptr,
                                const DELETER&  deleter,
                                Allocator      *basicAllocator,
                                bslmf::MetaInt<DeleterType::BSLMA_FACTORY_PTR>)
: d_deleter(deleter)
, d_ptr_p(ptr)
, d_allocator_p(basicAllocator)
{
}

template <class TYPE, class DELETER>
SharedPtrOutofplaceRep<TYPE, DELETER>::SharedPtrOutofplaceRep(
                         TYPE           *ptr,
                         const DELETER&  deleter,
                         Allocator      *basicAllocator,
                         bslmf::MetaInt<DeleterType::BSLMA_FUNCTOR_WITH_ALLOC>)
: d_deleter(deleter, basicAllocator)
, d_ptr_p(ptr)
, d_allocator_p(basicAllocator)
{
}

template <class TYPE, class DELETER>
SharedPtrOutofplaceRep<TYPE, DELETER>::SharedPtrOutofplaceRep(
                      TYPE           *ptr,
                      const DELETER&  deleter,
                      Allocator      *basicAllocator,
                      bslmf::MetaInt<DeleterType::BSLMA_FUNCTOR_WITHOUT_ALLOC>)
: d_deleter(deleter)
, d_ptr_p(ptr)
, d_allocator_p(basicAllocator)
{
}

template <class TYPE, class DELETER>
SharedPtrOutofplaceRep<TYPE, DELETER>::~SharedPtrOutofplaceRep()
{
}

// MANIPULATORS
template <class TYPE, class DELETER>
void SharedPtrOutofplaceRep<TYPE, DELETER>::disposeObject()
{
    SharedPtrOutofplaceRep_DeleterHelper::deleteObject(d_ptr_p, d_deleter);
    d_ptr_p = 0;
}

template <class TYPE, class DELETER>
inline
void SharedPtrOutofplaceRep<TYPE, DELETER>::disposeRep()
{
    // If 'd_allocator_p->deleteObject' is used to destroy the
    // 'SharedPtrOutofplaceRep' object, a virtual function call will be used
    // and a 'dynamic_cast' is required to obtain the address of the most
    // derived object to deallocate it.  Knowing 'SharedPtrOutofplaceRep' is
    // the most derived class, this unnecessary overhead of a virtual function
    // call can be avoided by explicitly calling the destructor.  This behavior
    // is guaranteed by the standard ([class.virtual] 13: "Explicit
    // qualification with the scope operator (5.1) suppresses the virtual call
    // mechanism.", page 224 of Working Draft 2007-10).

    this->SharedPtrOutofplaceRep<TYPE, DELETER>::~SharedPtrOutofplaceRep();
    d_allocator_p->deallocate(this);
}

template <class TYPE, class DELETER>
inline
void *
SharedPtrOutofplaceRep<TYPE, DELETER>::getDeleter(const std::type_info& type)
{
    return (typeid(d_deleter) == type)
         ? bsls::Util::addressOf(d_deleter)
         : 0;
}

// ACCESSORS
template <class TYPE, class DELETER>
inline
void *SharedPtrOutofplaceRep<TYPE, DELETER>::originalPtr() const
{
    return const_cast<void *>(static_cast<const void *>(d_ptr_p));
}

template <class TYPE, class DELETER>
inline
TYPE *SharedPtrOutofplaceRep<TYPE, DELETER>::ptr() const
{
    return d_ptr_p;
}

             // -------------------------------------------
             // struct SharedPtrOutofplaceRep_DeleterHelper
             // -------------------------------------------

// CLASS METHODS
template <class TYPE, class DELETER>
inline
void SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                              TYPE     *ptr,
                              DELETER&  deleter,
                              bslmf::MetaInt<DeleterType::BSLMA_ALLOCATOR_PTR>)
{
    deleter->deleteObject(ptr);
}

template <class TYPE, class DELETER>
inline
void SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                                TYPE     *ptr,
                                DELETER&  deleter,
                                bslmf::MetaInt<DeleterType::BSLMA_FACTORY_PTR>)
{
    deleter->deleteObject(ptr);
}

template <class TYPE, class DELETER>
inline
void SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                         TYPE     *ptr,
                         DELETER&  deleter,
                         bslmf::MetaInt<DeleterType::BSLMA_FUNCTOR_WITH_ALLOC>)
{
    deleter(ptr);
}

template <class TYPE, class DELETER>
inline
void SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                      TYPE     *ptr,
                      DELETER&  deleter,
                      bslmf::MetaInt<DeleterType::BSLMA_FUNCTOR_WITHOUT_ALLOC>)
{
    deleter(ptr);
}

template <class TYPE, class DELETER>
inline
void SharedPtrOutofplaceRep_DeleterHelper::deleteObject(TYPE     *ptr,
                                                        DELETER&  deleter)
{
    enum { BSLMA_DELETER_TYPE =
                 SharedPtrOutofplaceRep_DeleterDiscriminator<DELETER>::VALUE };

    SharedPtrOutofplaceRep_DeleterHelper::deleteObject(
                                         ptr,
                                         deleter,
                                         bslmf::MetaInt<BSLMA_DELETER_TYPE>());
}

               // ---------------------------------------
               // struct SharedPtrOutofplaceRep_InitProctor
               // ---------------------------------------

// CREATORS
template <class TYPE, class DELETER>
inline
SharedPtrOutofplaceRep_InitProctor<TYPE, DELETER>::
SharedPtrOutofplaceRep_InitProctor(TYPE *ptr, const DELETER&  deleter)
: d_ptr_p(ptr)
, d_deleter(deleter)
{
}

template <class TYPE, class DELETER>
inline
SharedPtrOutofplaceRep_InitProctor<TYPE, DELETER>::
~SharedPtrOutofplaceRep_InitProctor()
{
    // The definition of this function is intentionally *not* written as:
    //..
    //  if (d_ptr_p) {
    //      DELETER tempDeleter(d_deleter);
    //      SharedPtrOutofplaceRep_DeleterHelper::deleteObject(d_ptr_p,
    //                                                         tempDeleter);
    //  }
    //..
    // to work around a CC (Studio 12v4 on Solaris) compilation failure when
    // optimizations are enabled ('opt_exc_mt' and 'opt_exc_mt_64').

    if (!d_ptr_p) {
        return;                                                       // RETURN
    }

    typename bsl::conditional<bsl::is_function<DELETER>::value,
                              DELETER *,
                              DELETER>::type tempDeleter(d_deleter);
    SharedPtrOutofplaceRep_DeleterHelper::deleteObject(d_ptr_p, tempDeleter);
}

// MANIPULATORS
template <class TYPE, class DELETER>
inline
void SharedPtrOutofplaceRep_InitProctor<TYPE, DELETER>::release()
{
    d_ptr_p = 0;
}

// ============================================================================
//                              TYPE TRAITS
// ============================================================================

template <class TYPE, class DELETER>
struct UsesBslmaAllocator<SharedPtrOutofplaceRep_InitProctor<TYPE, DELETER> >
    : bsl::false_type
{
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
