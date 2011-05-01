// bcema_sharedptrinplacerep.h                                        -*-C++-*-
#ifndef INCLUDED_BCEMA_SHAREDPTRINPLACEREP
#define INCLUDED_BCEMA_SHAREDPTRINPLACEREP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an in-place implementation of 'bcema_SharedPtrRep'.
//
//@CLASSES:
//  bcema_SharedPtrInplaceRep: in-place 'bcema_SharedPtrRep' implementation
//
//@AUTHOR: Ilougino Rocha (irocha)
//         Herve Bronnimann (hbronnim)
//         Vlad Kliatchko (vkliatch)
//         Raymond Chiu (schiu49)
//
//@SEE_ALSO: bcema_sharedptr, bcema_sharedptr_rep, bcema_sharedptroutofplacerep
//
//@DESCRIPTION: This component provides a concrete implementation of
// 'bcema_SharedPtrRep' for managing objects of the parameterized 'TYPE' that
// are stored in-place in the representation .  Thus, only one memory
// allocation is required to create both the representation and the managed
// object.  When all references to the in-place object are released (using
// 'releaseRef'), the destructor of 'TYPE' is invoked.
//
///Thread-Safety
///-------------
// 'bcema_SharedPtrInplaceRep' is thread-safe provided that 'disposeObject' and
// 'disposeRep' are not called explicitly, meaning that all non-creator
// operations other than 'disposeObject' and 'disposeRep' on a given instance
// can be safely invoked simultaneously from multiple threads ('disposeObject'
// and 'disposeRep' are meant to be invoked only by 'releaseRef' and
// 'releaseWeakRef').  Note that there is no thread safety guarantees for
// operations on the managed object contained in 'bcema_SharedPtrInplaceRep'.
//
///Usage
///-----
// The following example demonstrates how to implement a shared 'bdet_Datetime'
// using 'bcema_SharedPtrInplaceRep':
//..
//  class MySharedDatetimePtr {
//      // This class provide a reference counted smart pointer to support
//      // shared ownership of a 'bdet_Datetime' object.
//
//      bdet_Datetime      *d_ptr_p;  // pointer to the managed object
//      bcema_SharedPtrRep *d_rep_p;  // pointer to the representation object
//
//    private:
//      // NOT IMPLEMENTED
//      MySharedDatetimePtr& operator=(const MySharedDatetimePtr&);
//
//    public:
//      // CREATORS
//      MySharedDatetimePtr();
//          // Create an empty shared datetime.
//
//      MySharedDatetimePtr(bdet_Datetime* ptr, bcema_SharedPtrRep* rep);
//          // Create a shared datetime that adopts ownership of the specified
//          // 'ptr' and the specified 'rep.
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
//      void createInplace(bslma_Allocator *basicAllocator,
//                         int              year,
//                         int              month,
//                         int              day);
//          // Create a new 'bcema_SharedPtrInplaceRep', using the specified
//          // 'basicAllocator' to supply memory, using the specified 'year',
//          // 'month' and 'day' to initialize the 'bdet_Datetime' within the
//          // newly created 'bcema_SharedPtrInplaceRep', and make this
//          // object refer to the newly created 'bdet_Datetime' object.
//
//      bdet_Datetime& operator*() const;
//          // Return a modifiable reference to the shared 'bdet_Datetime'
//          // object.
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
//  MySharedDatetimePtr::MySharedDatetimePtr()
//  : d_ptr_p(0)
//  , d_rep_p(0)
//  {
//  }
//
//  MySharedDatetimePtr::MySharedDatetimePtr(bdet_Datetime      *ptr,
//                                           bcema_SharedPtrRep *rep)
//  : d_ptr_p(ptr)
//  , d_rep_p(rep)
//  {
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
//  void MySharedDatetimePtr::createInplace(bslma_Allocator *basicAllocator,
//                                          int              year,
//                                          int              month,
//                                          int              day)
//  {
//      basicAllocator = bslma_Default::allocator(basicAllocator);
//      bcema_SharedPtrInplaceRep<bdet_Datetime> *rep = new (*basicAllocator)
//                     bcema_SharedPtrInplaceRep<bdet_Datetime>(basicAllocator,
//                                                              year,
//                                                              month,
//                                                              day);
//      MySharedDatetimePtr temp(rep->ptr(), rep);
//      bsl::swap(d_ptr_p, temp.d_ptr_p);
//      bsl::swap(d_rep_p, temp.d_rep_p);
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

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

#define bcema_SharedPtr_InplaceRepImpl bcema_SharedPtrInplaceRep
    // For backward compatibility.  Not for use by clients.

                      // ===============================
                      // class bcema_SharedPtrInplaceRep
                      // ===============================

template <class TYPE>
class bcema_SharedPtrInplaceRep : public bcema_SharedPtrRep {
    // This class provides a concrete implementation of the
    // 'bcema_SharedPtrRep' protocol for "in-place" instances of the
    // parameterized 'TYPE'.  Upon destruction of this object, the destructor
    // of 'TYPE' is invoked.

    // DATA
    bslma_Allocator *d_allocator_p; // memory allocator (held, not owned)

    TYPE             d_instance;    // beginning of the in-place buffer
                                    // note that this must be last in this
                                    // layout to allow for the possibility of
                                    // creating in-place uninitialized buffer,
                                    // where it is possible to access memory
                                    // beyond the 'd_instance' footprint (refer
                                    // to 'bcema_SharePtr::
                                    // createInplaceUninitializedBuffer' for
                                    // sample usage)

  private:
    // NOT IMPLEMENTED
    bcema_SharedPtrInplaceRep(const bcema_SharedPtrInplaceRep&);
    bcema_SharedPtrInplaceRep& operator=(const bcema_SharedPtrInplaceRep&);

    // PRIVATE CREATORS
    ~bcema_SharedPtrInplaceRep();
    // Destroy this representation object and the embedded instance of
    // parameterized 'TYPE'.  Note that this destructor is never called.
    // Instead, 'disposeObject' destroys the in-place object and
    // 'disposeRep' deallocates this representation object (including the
    // shared object's footprint).

  public:
    // CREATORS
    explicit bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator);
        // Create a 'bcema_SharedPtrInplaceRep' object having an "in-place"
        // default-constructed instance of the parameterized 'TYPE'.  Use the
        // specified 'basicAllocator' to supply memory and, upon a call to
        // 'disposeRep', to destroy this representation (and the "in-place"
        // shared object) .

    template <class A1>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator, const A1& a1);
    template <class A1, class A2>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2);
    template <class A1, class A2, class A3>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3);
    template <class A1, class A2, class A3, class A4>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4);
    template <class A1, class A2, class A3, class A4, class A5>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4,
                              const A5&        a5);
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4,
                              const A5&        a5,
                              const A6&        a6);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
             class A7>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4,
                              const A5&        a5,
                              const A6&        a6,
                              const A7&        a7);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4,
                              const A5&        a5,
                              const A6&        a6,
                              const A7&        a7,
                              const A8&        a8);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4,
                              const A5&        a5,
                              const A6&        a6,
                              const A7&        a7,
                              const A8&        a8,
                              const A9&        a9);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4,
                              const A5&        a5,
                              const A6&        a6,
                              const A7&        a7,
                              const A8&        a8,
                              const A9&        a9,
                              const A10&       a10);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4,
                              const A5&        a5,
                              const A6&        a6,
                              const A7&        a7,
                              const A8&        a8,
                              const A9&        a9,
                              const A10&       a10,
                              const A11&       a11);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4,
                              const A5&        a5,
                              const A6&        a6,
                              const A7&        a7,
                              const A8&        a8,
                              const A9&        a9,
                              const A10&       a10,
                              const A11&       a11,
                              const A12&       a12);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4,
                              const A5&        a5,
                              const A6&        a6,
                              const A7&        a7,
                              const A8&        a8,
                              const A9&        a9,
                              const A10&       a10,
                              const A11&       a11,
                              const A12&       a12,
                              const A13&       a13);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13, class A14>
    bcema_SharedPtrInplaceRep(bslma_Allocator *basicAllocator,
                              const A1&        a1,
                              const A2&        a2,
                              const A3&        a3,
                              const A4&        a4,
                              const A5&        a5,
                              const A6&        a6,
                              const A7&        a7,
                              const A8&        a8,
                              const A9&        a9,
                              const A10&       a10,
                              const A11&       a11,
                              const A12&       a12,
                              const A13&       a13,
                              const A14&       a14);
        // Create a 'bcema_SharedPtrInplaceRep' object having an "in-place"
        // instance of the parameterized 'TYPE' using the 'TYPE' constructor
        // that takes the specified arguments, 'a1' up to 'aN', where 'N' (at
        // most 14) is the number of arguments passed to this method.  Use the
        // specified 'basicAllocator' to supply memory and, upon a call to
        // 'disposeRep', to destroy this representation (and the "in-place"
        // shared object).

    // MANIPULATORS
    TYPE *ptr();
        // Return the address of the modifiable (in-place) object referred to
        // by this representation object.

    virtual void disposeRep();
        // Deallocate the memory associated with this representation object
        // (including the shared object's footprint).  This method is
        // automatically invoked by 'releaseRef' and 'releaseWeakRef' when the
        // number of weak references and the number of shared references both
        // reach zero and should not be explicitly invoked otherwise.  The
        // behavior is undefined unless 'disposeObject' has already been called
        // for this representation.  Note that this 'disposeRep' method
        // effectively serves as the representation object's destructor.

    virtual void disposeObject();
        // Destroy the object being referred to by this representation.  This
        // method is automatically invoked by 'releaseRef' when the number of
        // shared references reaches zero and should not be explicitly invoked
        // otherwise.  Note that this function calls the destructor for the
        // shared object, but does not deallocate its footprint.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) address of the modifiable (in-place) object
        // referred to by this representation object.
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                      // -------------------------------
                      // class bcema_SharedPtrInplaceRep
                      // -------------------------------

// CREATORS
template <class TYPE>
inline
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <class TYPE>
template <class A1>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1)
: d_allocator_p(basicAllocator)
, d_instance(a1)
{
}

template <class TYPE>
template <class A1, class A2>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2)
{
}

template <class TYPE>
template <class A1, class A2, class A3>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4,
                                               const A5&        a5)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4,
                                               const A5&        a5,
                                               const A6&        a6)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4,
                                               const A5&        a5,
                                               const A6&        a6,
                                               const A7&        a7)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4,
                                               const A5&        a5,
                                               const A6&        a6,
                                               const A7&        a7,
                                               const A8&        a8)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4,
                                               const A5&        a5,
                                               const A6&        a6,
                                               const A7&        a7,
                                               const A8&        a8,
                                               const A9&        a9)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4,
                                               const A5&        a5,
                                               const A6&        a6,
                                               const A7&        a7,
                                               const A8&        a8,
                                               const A9&        a9,
                                               const A10&       a10)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4,
                                               const A5&        a5,
                                               const A6&        a6,
                                               const A7&        a7,
                                               const A8&        a8,
                                               const A9&        a9,
                                               const A10&       a10,
                                               const A11&       a11)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4,
                                               const A5&        a5,
                                               const A6&        a6,
                                               const A7&        a7,
                                               const A8&        a8,
                                               const A9&        a9,
                                               const A10&       a10,
                                               const A11&       a11,
                                               const A12&       a12)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4,
                                               const A5&        a5,
                                               const A6&        a6,
                                               const A7&        a7,
                                               const A8&        a8,
                                               const A9&        a9,
                                               const A10&       a10,
                                               const A11&       a11,
                                               const A12&       a12,
                                               const A13&       a13)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
bcema_SharedPtrInplaceRep<TYPE>::bcema_SharedPtrInplaceRep(
                                               bslma_Allocator *basicAllocator,
                                               const A1&        a1,
                                               const A2&        a2,
                                               const A3&        a3,
                                               const A4&        a4,
                                               const A5&        a5,
                                               const A6&        a6,
                                               const A7&        a7,
                                               const A8&        a8,
                                               const A9&        a9,
                                               const A10&       a10,
                                               const A11&       a11,
                                               const A12&       a12,
                                               const A13&       a13,
                                               const A14&       a14)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)
{
}

template <class TYPE>
bcema_SharedPtrInplaceRep<TYPE>::~bcema_SharedPtrInplaceRep()
{
    BSLS_ASSERT(0);
}

// MANIPULATORS
template <class TYPE>
inline
void bcema_SharedPtrInplaceRep<TYPE>::disposeRep()
{
    d_allocator_p->deallocate(this);
}

template <class TYPE>
inline
void bcema_SharedPtrInplaceRep<TYPE>::disposeObject()
{
    d_instance.~TYPE();
}

// ACCESSORS
template <class TYPE>
inline
void *bcema_SharedPtrInplaceRep<TYPE>::originalPtr() const
{
    return const_cast<void *>(static_cast<const void *>(&d_instance));
}

template <class TYPE>
inline
TYPE *bcema_SharedPtrInplaceRep<TYPE>::ptr()
{
    return &d_instance;
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
