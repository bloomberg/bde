// bcema_sharedptrrep.h                                               -*-C++-*-
#ifndef INCLUDED_BCEMA_SHAREDPTRREP
#define INCLUDED_BCEMA_SHAREDPTRREP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an abstract class for a shared object manager.
//
//@CLASSES:
//  bcema_SharedPtrRep : shared pointer representation abstract class
//
//@AUTHOR: Ilougino Rocha (irocha)
//         Herve Bronnimann (hbronnim)
//         Vlad Kliatchko (vkliatch)
//         Raymond Chiu (schiu49)
//
//@SEE_ALSO: bcema_sharedptr, bcema_weakptr, bcema_sharedptrrep_inplace,
//           bcema_sharedptrrep_outofplace
//
//@DESCRIPTION: This component provides a partially implemented abstract class
// for managing the lifetime of a shared object.  'bcema_SharedPtrRep' provides
// a count of both shared and weak references to a shared object (described in
// more detail in the next section).  In addition, 'bcema_SharedPtrRep'
// provides protocol methods that allows a concrete implementation to specify
// what action should be taken when these reference counts reach zero.
//
///Shared and Weak References
///--------------------------
// There are two types of references to shared objects:
//
// 1) A shared reference allows users to share the ownership of an object and
// control its lifetime.  A shared object is destroyed only when the last
// shared reference to it is released.  The function 'acquireRef' should be
// called when a new shared reference is created and 'releaseRef' should be
// called when a share reference is removed.
//
// 2) A weak reference provides users conditional access to an object without
// sharing its ownership (or affecting its lifetime).  A shared object can be
// destroyed even if there are weak references to it.  The function
// 'acquireWeakRef' should be called when a new weak reference is created and
// 'releaseWeakRef' should be called when a weak reference is removed.
//
///Thread-Safety
///-------------
// 'bcema_SharedPtrRep' is thread-safe provided that 'disposeObject' and
// 'disposeRep' are not called explicitly, meaning that all non-creator
// operations other than 'disposeObject' and 'disposeRep' on a given instance
// can be safely invoked simultaneously from multiple threads
// ('disposeObject' and 'disposeRep' are meant to be implemented by types
// inheriting 'bcema_SharedPtrRep', and invoked only by 'releaseRef' and
// 'releaseWeakRef').  Note that there is no thread safety guarantees for
// operations on the managed object.
//
///'disposeObject' and 'disposeRep'
///--------------------------------
// 'disposeObject' is meant to act as the destructor of the managed object and
// each derived class must override this method to perform an action that
// releases the shared object from management, such as deleting the managed
// object.  'disposeObject' will be called when the last shared reference to
// the object has been released using 'releaseRef'.
//
// 'disposeRep' is meant to act as the destructor of 'bcema_SharedPtrRep'.  The
// destructor of 'bcema_SharedPtrRep' is declared as private and cannot be
// called.  The derived class must override 'disposeRep' to perform an action
// such as deallocating the memory of the instance of the derived class.
// 'disposeRep' will be called when both the last shared reference and the last
// weak reference to the object has been released using 'releaseRef' or
// 'releaseWeakRef'.
//
///Usage
///-----
// The following example demonstrates how to implement a shared 'bdet_Datetime'
// using 'bcema_SharedPtrRep'.  In this example, the implementation will store
// an object of 'bdet_Datetime' in-place.  First, we define an implementation
// of 'bcema_SharedPtrRep':
//..
//  class MySharedDatetimeRepImpl : public bcema_SharedPtrRep {
//      // Implementation of 'bcema_SharedPtrRep' for an in-place
//      // 'bdet_Datetime' object.
//
//      // DATA
//      bslma_Allocator *d_allocator_p; // memory allocator (held, not owned)
//      bdet_Datetime    d_instance;    // in-place object
//
//    private:
//      // NOT IMPLEMENTED
//      MySharedDatetimeRepImpl(const MySharedDatetimeRepImpl&);
//      MySharedDatetimeRepImpl& operator=(const MySharedDatetimeRepImpl&);
//
//    public:
//      // CREATORS
//      MySharedDatetimeRepImpl(bslma_Allocator *basicAllocator,
//                              int              year,
//                              int              month,
//                              int              day);
//          // Create a shared representation of a 'bdet_Datetime' object
//          // having the specified 'year', 'month' and 'day' using the
//          // specified 'basicAllocator' to allocate memory.
//
//      // MANIPULATORS
//      virtual void disposeRep();
//          // Dispose of this 'MySharedDatetimeRepImpl' object.
//
//      virtual void disposeObject();
//          // Dispose of the managed 'bdet_Datetime' object.
//
//      // ACCESSORS
//      bdet_Datetime *ptr();
//          // Return the address of the modifiable managed 'bdet_Datetime'
//          // object.
//
//      virtual void *originalPtr() const;
//          // Return the address of the modifiable managed 'bdet_Datetime'
//          // object.
//  };
//..
// Next, we define the implementation:
//..
//  // CREATORS
//  MySharedDatetimeRepImpl::MySharedDatetimeRepImpl(
//                                             bslma_Allocator *basicAllocator,
//                                             int              year,
//                                             int              month,
//                                             int              day)
//  : d_allocator_p(basicAllocator)
//  , d_instance(year, month, day)
//  {
//  }
//
//  void MySharedDatetimeRepImpl::disposeRep()
//  {
//      d_allocator_p->deallocate(this);
//  }
//
//  void MySharedDatetimeRepImpl::disposeObject()
//  {
//      d_instance.~bdet_Datetime();
//  }
//
//  bdet_Datetime *MySharedDatetimeRepImpl::ptr()
//  {
//      return &d_instance;
//  }
//
//  void *MySharedDatetimeRepImpl::originalPtr() const {
//      return const_cast<void*>(static_cast<const void *>(&d_instance));
//  }
//..
// Next, we implement a shared 'bdet_Datetime' class.
//..
//  class MySharedDatetimePtr {
//      // This class provides a reference counted managed pointer to support
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
//          // 'ptr' and the specified 'rep'.
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
//          // Create a new 'MySharedDatetimeRepImpl', using the specified
//          // 'basicAllocator' to supply memory, using the specified 'year',
//          // 'month' and 'day' to initialize the 'bdet_Datetime' within the
//          // newly created 'MySharedDatetimeRepImpl', and make this
//          // object refer to the newly created 'bdet_Datetime' object.
//
//      // ACCESSORS
//      bdet_Datetime& operator*() const;
//          // Return a reference to the modifiable 'bdet_Datetime' to which
//          // this object refers.
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
// Finally, we implement 'MySharedDatetimePtr':
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
//      MySharedDatetimeRepImpl *rep = new (*basicAllocator)
//                                      MySharedDatetimeRepImpl(basicAllocator,
//                                                              year,
//                                                              month,
//                                                              day);
//      MySharedDatetimePtr temp(rep->ptr(), rep);
//      bsl::swap(d_ptr_p, temp.d_ptr_p);
//      bsl::swap(d_rep_p, temp.d_rep_p);
//  }
//
//  bdet_Datetime& MySharedDatetimePtr::operator*() const
//  {
//      return *d_ptr_p;
//  }
//
//  bdet_Datetime *MySharedDatetimePtr::operator->() const
//  {
//      return d_ptr_p;
//  }
//
//  bdet_Datetime *MySharedDatetimePtr::ptr() const
//  {
//      return d_ptr_p;
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
                        // ========================
                        // class bcema_SharedPtrRep
                        // ========================

class bcema_SharedPtrRep {
    // This class provides a partially implemented shared pointer
    // representation ("letter") protocol.  The class provides two counters
    // for storing the number of shared and weak references, and functions to
    // increment and decrement these counters.  In addition, this class
    // provides protocol methods that allow concrete implementations to specify
    // what action should be taken when these counts reach zero.  The function
    // 'disposeRep' is responsible for destroying this object, it is called
    // when the reference count to this object reaches zero.  Thus, the
    // destructor of this object is declared as protected and should never be
    // invoked.

    // DATA
    bces_AtomicInt d_adjustedSharedCount;
                                  // Counter storing a value that allows us to
                                  // calculate the number of shared references.
                                  // The numerical value of
                                  // 'd_adjustedSharedCount' is:
                                  // 2 * number of shared references, plus 1 if
                                  // any weak references were *ever* created.

    bces_AtomicInt d_adjustedWeakCount;
                                  // Counter storing a value that allows us to
                                  // calculate the number of weak references.
                                  // The numerical value of
                                  // 'd_adjustedWeakCount' is:
                                  // 2 * number of weak references, plus 1 if
                                  // there are any *outstanding* shared
                                  // references.

  protected:
    // PROTECTED CREATORS
    virtual ~bcema_SharedPtrRep();
        // Destroy this representation object.

  public:
    // CLASS METHODS
    static void managedPtrDeleter(void *, bcema_SharedPtrRep *rep);
        // Delete the shared object referred to by this representation
        // if all the shared references to the shared object are released.
        // Note that the first argument is ignored.  Also note that this
        // function serves as the managed ptr deleter when converting a
        // 'bcema_SharedPtr' to a 'bdema_ManagedPtr'.

    // CREATORS
    bcema_SharedPtrRep();
        // Create a 'bcema_SharedPtrRep' object having one shared reference and
        // no weak references.

    // MANIPULATORS
    void acquireRef();
        // Atomically acquire a shared reference to the shared object referred
        // to by this representation.  The behavior is undefined unless
        // '0 < numReferences()'.

    void incrementRefs(int incrementAmount = 1);
        // Atomically increment the number of shared references to the shared
        // object referred to by this representation by the specified
        // 'incrementAmount'.  The behavior is undefined unless
        // '0 < incrementAmount' and '0 < numReferences()'.
        //
        // DEPRECATED: Use 'acquireRef' instead.

    void acquireWeakRef();
        // Atomically acquire a weak reference to the shared object referred to
        // by this representation.  The behavior is undefined unless
        // '0 < numWeakReferences() || 0 < numReferences()'.

    void releaseRef();
        // Atomically release a shared reference to the shared object referred
        // to by this representation, disposing of the shared object if all the
        // shared references to that object are released, and disposing of this
        // representation if all (shared and weak) references to that object
        // are released.  The behavior is undefined unless
        // '0 < numReferences()'.

    void releaseWeakRef();
        // Atomically release a weak reference to the shared object referred
        // to by this representation, disposing of this representation if all
        // (shared and weak) references to the shared object are released.  The
        // behavior is undefined unless '0 < numWeakReferences()'.

    bool tryAcquireRef();
        // Atomically acquire a shared reference to the shared object referred
        // to by this representation, if the number of shared references is
        // greater than 0, and do nothing otherwise.  Return 'true' if the
        // acquire succeeds, and 'false' otherwise.  The behavior is undefined
        // unless '0 < numWeakReferences() || 0 < numReferences()'.

    void resetCountsRaw(int numSharedReferences, int numWeakReferences);
        // Reset the number of shared references and the number of weak
        // references stored by this representation to the specified
        // 'numSharedReferences' and 'numWeakReferences' respectively.  This
        // function is *not* thread-safe and users must ensure that they
        // serialize access to the 'bcema_SharedPtrRep' object when calling
        // this function.  Note that this function updates the counts, but does
        // not dispose of the representation or the object irrespective of the
        // values of 'numSharedReferences' and 'numWeakReferences'.

    virtual void disposeRep() = 0;
        // Dispose of this representation object.  This method is automatically
        // invoked by 'releaseRef' and 'releaseWeakRef' when the number of weak
        // references and the number of shared references both reach zero and
        // should not be explicitly invoked otherwise.  The behavior is
        // undefined unless 'disposeObject' has already been called for this
        // representation.  Note that this virtual 'disposeRep' method
        // effectively serves as the representation object's destructor.  Also
        // note that derived classes must override this method to perform
        // appropriate action such as deleting this representation, or
        // returning it to an object pool.

    virtual void disposeObject() = 0;
        // Dispose of the shared object referred to by this representation.
        // This method is automatically invoked by 'releaseRef' when the number
        // of shared references reaches zero and should not be explicitly
        // invoked otherwise.  Note that this virtual 'disposeObject' method
        // effectively serves as the shared object's destructor.  Also note
        // that derived classes must override this method to perform the
        // appropriate action such as deleting the shared object.

    // ACCESSORS
    virtual void *originalPtr() const = 0;
        // Return the (untyped) address of the modifiable shared object
        // referred to by this representation.

    int numReferences() const;
        // Return a "snapshot" of the current number of shared references to
        // the shared object referred to by this representation object.

    int numWeakReferences() const;
        // Return a "snapshot" of the current number of weak references to
        // the shared object referred to by this representation object.

    bool hasUniqueOwner() const;
        // Return 'true' if there is only one shared reference and no weak
        // references to the object referred to by this representation, and
        // 'false' otherwise.
};

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ------------------------
                        // class bcema_SharedPtrRep
                        // ------------------------

// PROTECTED CREATORS
inline
bcema_SharedPtrRep::~bcema_SharedPtrRep()
{
}

// CLASS METHODS
inline
void bcema_SharedPtrRep::managedPtrDeleter(void *, bcema_SharedPtrRep *rep)
{
    rep->releaseRef();
}

// CREATORS
inline
bcema_SharedPtrRep::bcema_SharedPtrRep()
: d_adjustedSharedCount(2)                      // minimum consistency: relaxed
, d_adjustedWeakCount(1)                        // minimum consistency: relaxed
{
}

// MANIPULATORS
inline
void bcema_SharedPtrRep::acquireRef()
{
    BSLS_ASSERT_SAFE(0 < numReferences());

    d_adjustedSharedCount.relaxedAdd(2);        // minimum consistency: relaxed
}

inline
void bcema_SharedPtrRep::incrementRefs(int incrementAmount)
{
    BSLS_ASSERT_SAFE(0 < incrementAmount);
    BSLS_ASSERT_SAFE(0 < numReferences());

    d_adjustedSharedCount.relaxedAdd(incrementAmount * 2);
                                                // minimum consistency: relaxed
}

inline
void bcema_SharedPtrRep::releaseWeakRef()
{
    BSLS_ASSERT_SAFE(0 < numWeakReferences());

    const int weakCount = d_adjustedWeakCount.add(-2);
                                        // release consistency: acquire/release
    if (0 == weakCount) {
        disposeRep();
    }
}

// ACCESSORS
inline
int bcema_SharedPtrRep::numReferences() const
{
    const int sharedCount = d_adjustedSharedCount.relaxedLoad();
                                                // minimum consistency: relaxed
    return sharedCount / 2;
}

inline
int bcema_SharedPtrRep::numWeakReferences() const
{
    const int weakCount = d_adjustedWeakCount.relaxedLoad();
                                                // minimum consistency: relaxed
    return weakCount / 2;
}

inline
bool bcema_SharedPtrRep::hasUniqueOwner() const
{
    const int sharedCount = d_adjustedSharedCount;
                                                // release consistency: acquire
    return 2 == sharedCount
        || (3 == sharedCount
         && 1 == d_adjustedWeakCount);          // release consistency: acquire
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
