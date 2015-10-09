// bslma_sharedptrinplacerep.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMA_SHAREDPTRINPLACEREP
#define INCLUDED_BSLMA_SHAREDPTRINPLACEREP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an in-place implementation of 'bslma::SharedPtrRep'.
//
//@CLASSES:
//  bslma::SharedPtrInplaceRep: in-place 'bslma::SharedPtrRep' implementation
//
//@SEE_ALSO: bslma_sharedptr, bslma_sharedptr_rep, bslma_sharedptroutofplacerep
//
//@DESCRIPTION: This component provides a concrete implementation of
// 'bslma::SharedPtrRep' for managing objects of the parameterized 'TYPE' that
// are stored in-place in the representation .  Thus, only one memory
// allocation is required to create both the representation and the managed
// object.  When all references to the in-place object are released (using
// 'releaseRef'), the destructor of 'TYPE' is invoked.
//
///Thread Safety
///-------------
// 'bslma::SharedPtrInplaceRep' is thread-safe provided that 'disposeObject'
// and 'disposeRep' are not called explicitly, meaning that all non-creator
// operations other than 'disposeObject' and 'disposeRep' on a given instance
// can be safely invoked simultaneously from multiple threads ('disposeObject'
// and 'disposeRep' are meant to be invoked only by 'releaseRef' and
// 'releaseWeakRef').  Note that there is no thread safety guarantees for
// operations on the managed object contained in 'bslma::SharedPtrInplaceRep'.
//
///Usage
///-----
// The following example demonstrates how to implement a shared 'bdet_Datetime'
// using 'bslma::SharedPtrInplaceRep':
//..
//  class MySharedDatetimePtr {
//      // This class provide a reference counted smart pointer to support
//      // shared ownership of a 'bdet_Datetime' object.
//
//      bdet_Datetime       *d_ptr_p;  // pointer to the managed object
//      bslma::SharedPtrRep *d_rep_p;  // pointer to the representation object
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
//      MySharedDatetimePtr(bdet_Datetime* ptr, bslma::SharedPtrRep* rep);
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
//      void createInplace(bslma::Allocator *basicAllocator,
//                         int               year,
//                         int               month,
//                         int               day);
//          // Create a new 'bslma::SharedPtrInplaceRep', using the specified
//          // 'basicAllocator' to supply memory, using the specified 'year',
//          // 'month' and 'day' to initialize the 'bdet_Datetime' within the
//          // newly created 'bslma::SharedPtrInplaceRep', and make this
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
//  MySharedDatetimePtr::MySharedDatetimePtr(bdet_Datetime       *ptr,
//                                           bslma::SharedPtrRep *rep)
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
//  void MySharedDatetimePtr::createInplace(bslma::Allocator *basicAllocator,
//                                          int               year,
//                                          int               month,
//                                          int               day)
//  {
//      basicAllocator = bslma::Default::allocator(basicAllocator);
//      bslma::SharedPtrInplaceRep<bdet_Datetime> *rep = new (*basicAllocator)
//                    bslma::SharedPtrInplaceRep<bdet_Datetime>(basicAllocator,
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_SHAREDPTRREP
#include <bslma_sharedptrrep.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
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

                      // =========================
                      // class SharedPtrInplaceRep
                      // =========================

template <class TYPE>
class SharedPtrInplaceRep : public SharedPtrRep {
    // This class provides a concrete implementation of the 'SharedPtrRep'
    // protocol for "in-place" instances of the parameterized 'TYPE'.  Upon
    // destruction of this object, the destructor of 'TYPE' is invoked.

    // DATA
    Allocator *d_allocator_p; // memory allocator (held, not owned)

    TYPE       d_instance;    // Beginning of the in-place buffer.  Note that
                              // this must be last in this layout to allow for
                              // the possibility of creating in-place
                              // uninitialized buffer, where it is possible to
                              // access memory beyond the 'd_instance'
                              // footprint (refer to 'bsl::share_ptr::
                              // createInplaceUninitializedBuffer' for sample
                              // usage)

  private:
    // NOT IMPLEMENTED
    SharedPtrInplaceRep(const SharedPtrInplaceRep&);
    SharedPtrInplaceRep& operator=(const SharedPtrInplaceRep&);

    // PRIVATE CREATORS
    ~SharedPtrInplaceRep();
        // Destroy this representation object and the embedded instance of
        // parameterized 'TYPE'.  Note that this destructor is never called.
        // Instead, 'disposeObject' destroys the in-place object and
        // 'disposeRep' deallocates this representation object (including the
        // shared object's footprint).

  public:
    // CREATORS
#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <class... ARGS>
    explicit SharedPtrInplaceRep(Allocator *basicAllocator,
                                 ARGS&&...  args);
        // Create a 'SharedPtrInplaceRep' object having an "in-place" instance
        // of the parameterized 'TYPE' using the 'TYPE' constructor that takes
        // the specified arguments, 'args...'.  Use the specified
        // 'basicAllocator' to supply memory and, upon a call to 'disposeRep',
        // to destroy this representation (and the "in-place" shared object).
# else
    template <class... ARGS>
    explicit SharedPtrInplaceRep(Allocator      *basicAllocator,
                                 const ARGS&...  args);
        // Create a 'SharedPtrInplaceRep' object having an "in-place" instance
        // of the parameterized 'TYPE' using the 'TYPE' constructor that takes
        // the specified arguments, 'args...'.  Use the specified
        // 'basicAllocator' to supply memory and, upon a call to 'disposeRep',
        // to destroy this representation (and the "in-place" shared object).

# endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
#else
    explicit SharedPtrInplaceRep(Allocator *basicAllocator);
        // Create a 'SharedPtrInplaceRep' object having an "in-place"
        // default-constructed instance of the parameterized 'TYPE'.  Use the
        // specified 'basicAllocator' to supply memory and, upon a call to
        // 'disposeRep', to destroy this representation (and the "in-place"
        // shared object) .

    template <class A1>
    SharedPtrInplaceRep(Allocator *basicAllocator, const A1& a1);
    template <class A1, class A2>
    SharedPtrInplaceRep(Allocator *basicAllocator,
                        const A1&  a1,
                        const A2&  a2);
    template <class A1, class A2, class A3>
    SharedPtrInplaceRep(Allocator *basicAllocator,
                        const A1&  a1,
                        const A2&  a2,
                        const A3&  a3);
    template <class A1, class A2, class A3, class A4>
    SharedPtrInplaceRep(Allocator *basicAllocator,
                        const A1&  a1,
                        const A2&  a2,
                        const A3&  a3,
                        const A4&  a4);
    template <class A1, class A2, class A3, class A4, class A5>
    SharedPtrInplaceRep(Allocator *basicAllocator,
                        const A1&  a1,
                        const A2&  a2,
                        const A3&  a3,
                        const A4&  a4,
                        const A5&  a5);
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    SharedPtrInplaceRep(Allocator *basicAllocator,
                        const A1&  a1,
                        const A2&  a2,
                        const A3&  a3,
                        const A4&  a4,
                        const A5&  a5,
                        const A6&  a6);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7>
    SharedPtrInplaceRep(Allocator *basicAllocator,
                        const A1&  a1,
                        const A2&  a2,
                        const A3&  a3,
                        const A4&  a4,
                        const A5&  a5,
                        const A6&  a6,
                        const A7&  a7);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    SharedPtrInplaceRep(Allocator *basicAllocator,
                        const A1&  a1,
                        const A2&  a2,
                        const A3&  a3,
                        const A4&  a4,
                        const A5&  a5,
                        const A6&  a6,
                        const A7&  a7,
                        const A8&  a8);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    SharedPtrInplaceRep(Allocator *basicAllocator,
                        const A1&  a1,
                        const A2&  a2,
                        const A3&  a3,
                        const A4&  a4,
                        const A5&  a5,
                        const A6&  a6,
                        const A7&  a7,
                        const A8&  a8,
                        const A9&  a9);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10>
    SharedPtrInplaceRep(Allocator  *basicAllocator,
                        const A1&   a1,
                        const A2&   a2,
                        const A3&   a3,
                        const A4&   a4,
                        const A5&   a5,
                        const A6&   a6,
                        const A7&   a7,
                        const A8&   a8,
                        const A9&   a9,
                        const A10&  a10);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11>
    SharedPtrInplaceRep(Allocator  *basicAllocator,
                        const A1&   a1,
                        const A2&   a2,
                        const A3&   a3,
                        const A4&   a4,
                        const A5&   a5,
                        const A6&   a6,
                        const A7&   a7,
                        const A8&   a8,
                        const A9&   a9,
                        const A10&  a10,
                        const A11&  a11);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12>
    SharedPtrInplaceRep(Allocator  *basicAllocator,
                        const A1&   a1,
                        const A2&   a2,
                        const A3&   a3,
                        const A4&   a4,
                        const A5&   a5,
                        const A6&   a6,
                        const A7&   a7,
                        const A8&   a8,
                        const A9&   a9,
                        const A10&  a10,
                        const A11&  a11,
                        const A12&  a12);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13>
    SharedPtrInplaceRep(Allocator  *basicAllocator,
                        const A1&   a1,
                        const A2&   a2,
                        const A3&   a3,
                        const A4&   a4,
                        const A5&   a5,
                        const A6&   a6,
                        const A7&   a7,
                        const A8&   a8,
                        const A9&   a9,
                        const A10&  a10,
                        const A11&  a11,
                        const A12&  a12,
                        const A13&  a13);
    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9, class A10, class A11, class A12,
              class A13, class A14>
    SharedPtrInplaceRep(Allocator  *basicAllocator,
                        const A1&   a1,
                        const A2&   a2,
                        const A3&   a3,
                        const A4&   a4,
                        const A5&   a5,
                        const A6&   a6,
                        const A7&   a7,
                        const A8&   a8,
                        const A9&   a9,
                        const A10&  a10,
                        const A11&  a11,
                        const A12&  a12,
                        const A13&  a13,
                        const A14&  a14);
        // Create a 'SharedPtrInplaceRep' object having an "in-place" instance
        // of the parameterized 'TYPE' using the 'TYPE' constructor that takes
        // the specified arguments, 'a1' up to 'aN', where 'N' (at most 14) is
        // the number of arguments passed to this method.  Use the specified
        // 'basicAllocator' to supply memory and, upon a call to 'disposeRep',
        // to destroy this representation (and the "in-place" shared object).
#endif

    // MANIPULATORS
    virtual void disposeObject();
        // Destroy the object being referred to by this representation.  This
        // method is automatically invoked by 'releaseRef' when the number of
        // shared references reaches zero and should not be explicitly invoked
        // otherwise.  Note that this function calls the destructor for the
        // shared object, but does not deallocate its footprint.

    virtual void disposeRep();
        // Deallocate the memory associated with this representation object
        // (including the shared object's footprint).  This method is
        // automatically invoked by 'releaseRef' and 'releaseWeakRef' when the
        // number of weak references and the number of shared references both
        // reach zero and should not be explicitly invoked otherwise.  The
        // behavior is undefined unless 'disposeObject' has already been called
        // for this representation.  Note that this 'disposeRep' method
        // effectively serves as the representation object's destructor.

    void *getDeleter(const std::type_info& type);
        // Return a null pointer.  Note that the specified 'type' is not used
        // as an in-place representation for a shared pointer can never store a
        // user-supplied deleter (there is no function that might try to create
        // one).

    TYPE *ptr();
        // Return the address of the modifiable (in-place) object referred to
        // by this representation object.

    // ACCESSORS
    virtual void *originalPtr() const;
        // Return the (untyped) address of the modifiable (in-place) object
        // referred to by this representation object.
};

// ============================================================================
//              INLINE FUNCTION AND FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

                      // -------------------------
                      // class SharedPtrInplaceRep
                      // -------------------------

// CREATORS
#if defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
template <class... ARGS>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               ARGS&&...  args)
: d_allocator_p(basicAllocator)
, d_instance(BSLS_COMPILERFEATURES_FORWARD(ARGS,args)...)
{
}
# else
template <class TYPE>
template <class... ARGS>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator      *basicAllocator,
                                               const ARGS&...  args)
: d_allocator_p(basicAllocator)
, d_instance(args...)
{
}
# endif
#else
template <class TYPE>
inline
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator)
: d_allocator_p(basicAllocator)
{
}

template <class TYPE>
template <class A1>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1)
: d_allocator_p(basicAllocator)
, d_instance(a1)
{
}

template <class TYPE>
template <class A1, class A2>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2)
{
}

template <class TYPE>
template <class A1, class A2, class A3>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4,
                                               const A5&  a5)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4,
                                               const A5&  a5,
                                               const A6&  a6)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4,
                                               const A5&  a5,
                                               const A6&  a6,
                                               const A7&  a7)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4,
                                               const A5&  a5,
                                               const A6&  a6,
                                               const A7&  a7,
                                               const A8&  a8)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4,
                                               const A5&  a5,
                                               const A6&  a6,
                                               const A7&  a7,
                                               const A8&  a8,
                                               const A9&  a9)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4,
                                               const A5&  a5,
                                               const A6&  a6,
                                               const A7&  a7,
                                               const A8&  a8,
                                               const A9&  a9,
                                               const A10& a10)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4,
                                               const A5&  a5,
                                               const A6&  a6,
                                               const A7&  a7,
                                               const A8&  a8,
                                               const A9&  a9,
                                               const A10& a10,
                                               const A11& a11)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4,
                                               const A5&  a5,
                                               const A6&  a6,
                                               const A7&  a7,
                                               const A8&  a8,
                                               const A9&  a9,
                                               const A10& a10,
                                               const A11& a11,
                                               const A12& a12)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4,
                                               const A5&  a5,
                                               const A6&  a6,
                                               const A7&  a7,
                                               const A8&  a8,
                                               const A9&  a9,
                                               const A10& a10,
                                               const A11& a11,
                                               const A12& a12,
                                               const A13& a13)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)
{
}

template <class TYPE>
template <class A1, class A2, class A3, class A4, class A5, class A6,
          class A7, class A8, class A9, class A10, class A11, class A12,
          class A13, class A14>
SharedPtrInplaceRep<TYPE>::SharedPtrInplaceRep(Allocator *basicAllocator,
                                               const A1&  a1,
                                               const A2&  a2,
                                               const A3&  a3,
                                               const A4&  a4,
                                               const A5&  a5,
                                               const A6&  a6,
                                               const A7&  a7,
                                               const A8&  a8,
                                               const A9&  a9,
                                               const A10& a10,
                                               const A11& a11,
                                               const A12& a12,
                                               const A13& a13,
                                               const A14& a14)
: d_allocator_p(basicAllocator)
, d_instance(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)
{
}
#endif

template <class TYPE>
SharedPtrInplaceRep<TYPE>::~SharedPtrInplaceRep()
{
    BSLS_ASSERT(0);
}

// MANIPULATORS
template <class TYPE>
inline
void SharedPtrInplaceRep<TYPE>::disposeObject()
{
    d_instance.~TYPE();
}

template <class TYPE>
inline
void SharedPtrInplaceRep<TYPE>::disposeRep()
{
    d_allocator_p->deallocate(this);
}

template <class TYPE>
inline
void *SharedPtrInplaceRep<TYPE>::getDeleter(const std::type_info&)
{
    return 0;
}

template <class TYPE>
inline
TYPE *SharedPtrInplaceRep<TYPE>::ptr()
{
    return &d_instance;
}

// ACCESSORS
template <class TYPE>
inline
void *SharedPtrInplaceRep<TYPE>::originalPtr() const
{
    return const_cast<void *>(static_cast<const void *>(
                                           bsls::Util::addressOf(d_instance)));
}

// ============================================================================
//                              TYPE TRAITS
// ============================================================================

template <class ELEMENT_TYPE>
struct UsesBslmaAllocator<SharedPtrInplaceRep<ELEMENT_TYPE> >
    : bsl::false_type {
    // The class template 'SharedPtrInplaceRep' appears to use allocators, but
    // passes its allocator argument in the first position, rather than in the
    // last position, so is not compatible with BDE APIs that use this trait.
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
