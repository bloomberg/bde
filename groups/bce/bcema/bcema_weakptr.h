// bcema_weakptr.h                                                    -*-C++-*-
#ifndef INCLUDED_BCEMA_WEAKPTR
#define INCLUDED_BCEMA_WEAKPTR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a weak pointer to a reference-counted shared object.
//
//@CLASSES:
//  bcema_WeakPtr: "weak" reference to reference-counted shared object
//
//@AUTHOR: Henry Verschell (hversche), Rohan Bhindwale (rbhindwa)
//
//@SEE_ALSO: bcema_sharedptr, bdema_managedptr, bcec_sharedobjectpool
//
//@DESCRIPTION: This component provides a mechanism, 'bcema_WeakPtr', used to
// create weak references to reference-counted shared ('bcema_SharedPtr')
// objects.  A weak reference provides conditional access to a shared object
// managed by a 'bcema_SharedPtr', but, unlike a shared (or "strong")
// reference, does not affect the shared object's lifetime.  An object having
// even one shared reference to it will not be destroyed, but an object having
// only weak references would have been destroyed when the last shared
// reference was released.
//
// A weak pointer can be constructed from another weak pointer or a
// 'bcema_SharedPtr'.  To access the shared object referenced by a weak pointer
// clients must first obtain a shared pointer to that object using the
// 'acquireSharedPtr' method.  If the shared object has been destroyed (as
// indicated by the 'expired' method), then 'acquireSharedPtr' returns a shared
// pointer in the default constructed (empty) state.
//
///Thread-Safety
///-------------
// This section qualifies the thread-safety of 'bcema_WeakPtr' objects
// themselves rather than the thread-safety of the objects being referenced.
//
// It is safe to access or modify two distinct 'bcema_WeakPtr' objects
// simultaneously, each from a separate thread, even if they reference the same
// shared object.  It is safe to access a single 'bcema_WeakPtr' object
// simultaneously from two or more threads, provided no other thread is
// simultaneously modifying the object.  It is not safe to access or modify a
// 'bcema_WeakPtr' object in one thread while another thread modifies the same
// object.
//
// It is safe to access, modify, copy, or delete a weak pointer in one thread,
// while other threads access or modify other weak pointers that reference the
// same object.  However, there is no guarantee regarding the safety of
// accessing or modifying the object *referenced* by the weak pointer
// simultaneously from multiple threads (that depends on the thread-safety
// guarantees of the shared object itself).
//
///Weak Pointers using "in-place" or Pooled Shared Pointer Representations
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A weak pointer that is not in the empty state shares a common representation
// (used to refer to the shared object) with the shared (or other weak)
// pointer from which it was constructed, and holds this representation until
// it is either destroyed or reset.  This common representation is not
// destroyed and deallocated (although the shared object itself may have been
// destroyed) until all weak references to that common representation have been
// released.
//
// Due to this behavior the memory footprint of shared objects that are
// constructed "in-place" in the shared pointer representation (refer to the
// component-level documentation of 'bcema_sharedptr' for more information on
// shared pointers with "in-place" representations) is not deallocated until
// all weak references to that shared object are released.  Note that a
// shared object is always destroyed when the last shared reference to it is
// released.  Also note that the same behavior is applicable if the shared
// objects were obtained from a class that pools shared pointer representations
// (for example, 'bcec_SharedObjectPool').
//
// For example suppose we have a class with a large memory footprint:
//..
//  class ClassWithLargeFootprint {
//      // This class has a large memory footprint.
//
//      // TYPES
//      enum { BUFFER_SIZE = 1024 };
//          // The size of the buffer owned by this 'class'.
//
//      // DATA
//      char d_buffer[BUFFER_SIZE];
//
//      // ...
//  };
//..
// We then create an "in-place" shared pointer to an object of
// 'ClassWithLargeFootprint' using the 'createInplace' method of
// 'bcema_SharedPtr'.  The 'sp' shared pointer representation of 'sp' will
// create a 'ClassWithLargeFootprint' object "in-place":
//..
//  bcema_SharedPtr<ClassWithLargeFootprint> sp;
//  sp.createInplace();
//..
// Next we construct a weak pointer from this (in-place) shared pointer:
//..
//  bcema_WeakPtr<ClassWithLargeFootprint> wp(sp);
//..
// Now releasing all shared references to the shared object (using the 'reset'
// function) causes the object's destructor to be called, but the
// representation is not destroyed (and the object's footprint is not
// deallocated) until 'wp' releases its weak reference:
//..
//  sp.reset(); // The object's footprint is not deallocated until all weak
//              // references to it are released.
//
//  wp.reset(); // The release of the *last* weak reference results in the
//              // destruction and deallocation of the representation and the
//              // object's footprint.
//..
// If a shared object has a large footprint, and the client anticipates there
// will be weak references to it, then it may be advisable to create an
// out-of-place shared pointer representation, which destroys the shared object
// and deallocates its footprint when the last *shared* reference to it is
// released, regardless of whether there are any outstanding weak references to
// the same representation.
//
///Usage
///-----
// The following examples demonstrate various features and uses of weak
// pointers.
//
///Example 1 - Basic Usage
///- - - - - - - - - - - -
// This example illustrates the basic syntax needed to create and use a
// 'bcema_WeakPtr'.  Suppose that we want to construct a weak pointer that
// refers to an 'int' managed by a shared pointer.  Next we define the shared
// pointer and assign a value to the shared 'int':
//..
//  bcema_SharedPtr<int> intPtr;
//  intPtr.createInplace(bslma_Default::allocator());
//  *intPtr = 10;
//  assert(10 == *intPtr);
//..
// Next we construct a weak pointer to the 'int':
//..
//  bcema_WeakPtr<int> intWeakPtr(intPtr);
//  assert(!intWeakPtr.expired());
//..
// 'bcema_WeakPtr' does not provide direct access to the shared object being
// referenced.  To access and manipulate the 'int' from the weak pointer, we
// have to obtain a shared pointer from it:
//..
//  bcema_SharedPtr<int> intPtr2 = intWeakPtr.acquireSharedPtr();
//  assert(intPtr2);
//  assert(10 == *intPtr2);
//
//  *intPtr2 = 20;
//  assert(20 == *intPtr);
//  assert(20 == *intPtr2);
//..
// We remove the weak reference to the shared 'int' by calling the 'reset'
// method:
//..
//  intWeakPtr.reset();
//  assert(intWeakPtr.expired());
//..
// Note that resetting the weak pointer does not affect the shared pointers
// referencing the 'int' object:
//..
//  assert(20 == *intPtr);
//  assert(20 == *intPtr2);
//..
// Now, we construct another weak pointer referencing the shared 'int':
//..
//  bcema_WeakPtr<int> intWeakPtr2(intPtr);
//  assert(!intWeakPtr2.expired());
//..
// Finally 'reset' all shared references to the 'int', which will cause the
// weak pointer to become "expired"; any subsequent attempt to obtain a shared
// pointer from the weak pointer will return a shared pointer in the default
// constructed (empty) state:
//..
//  intPtr.reset();
//  intPtr2.reset();
//  assert(intWeakPtr2.expired());
//  assert(!intWeakPtr2.acquireSharedPtr());
//..
///Example 2 - Breaking Cyclic Dependencies
///- - - - - - - - - - - - - - - - - - - -
// Weak pointers are frequently used to break cyclic dependencies among objects
// that store references to each other via shared pointers.  Consider, for
// example, a simplified broadcasting system that sends news alerts to users
// based on user-selected keywords.  The user information is stored in the
// 'NewsUser' class and the details of the news alert are stored in the
// 'NewsAlert' class.  The class definitions for 'NewsUser' and 'NewsAlert' are
// provided below (with any code not essential to this example elided):
//..
//  class NewsAlert;
//
//  class NewsUser {
//      // This class stores the user information required for listening to
//      // alerts.
//
//      // DATA
//      bsl::vector<bcema_SharedPtr<NewsAlert> > d_alerts; // list of alerts
//                                                         // for which a user
//                                                         // is registered
//
//    public:
//      // ...
//
//      // MANIPULATORS
//      void addAlert(const bcema_SharedPtr<NewsAlert>& alertPtr)
//          // Add the specified 'alertPtr' to the list of alerts being
//          // monitored by this user.
//      {
//          d_alerts.push_back(alertPtr);
//      }
//
//      // ...
//  };
//..
// Now we define the alert class, 'NewsAlert':
//..
//  class NewsAlert {
//      // This class stores the alert information required for sending
//      // alerts.
//
//      // DATA
//      bsl::vector<bcema_SharedPtr<NewsUser> > d_users;  // list of users
//                                                        // who have
//                                                        // registered for
//                                                        // this alert
//
//    public:
//      // ...
//
//      // MANIPULATORS
//      void addUser(const bcema_SharedPtr<NewsUser>& userPtr)
//          // Add the specified 'userPtr' to the list of users monitoring this
//          // alert.
//      {
//          d_users.push_back(userPtr);
//      }
//
//      // ...
//  };
//..
// Note that the 'NewsUser' and 'NewsAlert' classes, as currently defined could
// easily result in a memory leak under typical use due to a cyclic reference:
//..
//  bslma_TestAllocator ta;
//  {
//      bcema_SharedPtr<NewsUser> userPtr;
//      userPtr.createInplace(&ta);
//
//      bcema_SharedPtr<NewsAlert> alertPtr;
//      alertPtr.createInplace(&ta);
//
//      alertPtr->addUser(userPtr);
//      userPtr->addAlert(alertPtr);
//
//      alertPtr.reset();
//      userPtr.reset();
//  }
//
//  // MEMORY LEAK !!
//..
// Even though we have released 'alertPtr' and 'userPtr', there still exists a
// cyclic reference between the two objects, so none of the objects are
// destroyed.
//
// We can break this cyclic dependency by modifying the alert class,
// 'NewsAlert', to store a weak pointer, instead of a shared pointer, to a
// 'NewsUser' object.  Below is the modified definition for the 'NewsAlert'
// class:
//..
//  class NewsAlert {
//      // This class stores the alert information required for sending
//      // alerts.
//
//..
// Note that the user is stored by a weak pointer instead of by a shared
// pointer:
//..
//      // DATA
//      bsl::vector<bcema_WeakPtr<NewsUser> > d_users; // list of users that
//                                                     // have registered for
//                                                     // this alert
//
//    public:
//      // MANIPULATORS
//      void addUser(const bcema_WeakPtr<NewsUser>& userPtr)
//          // Add the specified 'userPtr' to the list of users monitoring this
//          // alert.
//      {
//          d_users.push_back(userPtr);
//      }
//
//      // ...
//  };
//..
// The use of 'NewsUser' and 'NewsAlert' objects no longer causes a memory
// leak:
//..
//  bslma_TestAllocator ta;
//  {
//      bcema_SharedPtr<NewsAlert> alertPtr;
//      alertPtr.createInplace(&ta);
//
//      bcema_SharedPtr<NewsUser> userPtr;
//      userPtr.createInplace(&ta);
//
//      bcema_WeakPtr<NewsUser> userWeakPtr(userPtr);
//
//      alertPtr->addUser(userWeakPtr);
//      userPtr->addAlert(alertPtr);
//
//      alertPtr.reset();
//      userPtr.reset();
//  }
//
//  // No memory leak now.
//..
// Now both the user and alert objects are correctly destroyed, and the memory
// returned to the allocator after 'alertPtr' and 'userPtr' are reset.
//
///Example 3 - Caching
///- - - - - - - - - -
// Suppose we want to implement a peer-to-peer file-sharing system that allows
// users to search for files that match certain keywords.  A peer-to-peer
// file-sharing system connects remotely to similar file-sharing systems
// (peers) and determines which files stored by those peers best match the
// keywords specified by the user.
//
// Typically, such a system would connect to hundreds of peers to increase the
// probability of finding files that match any keyword.  When a user initiates
// a request with certain keywords the system sends the search request to
// connected peers to find out which one of them contains the files that best
// match the requested keywords.  But it is inefficient to send every file
// search request to all connected peers.  So the system may cache a subset of
// the connected peers, and based on their connection bandwidth, relevancy of
// previous search results, etc., send the search requests only to those cached
// peers.
//
// To show the implementation of such a system we will first define a 'Peer'
// class that stores the information about a peer (with code not relevant to
// the usage example elided):
//..
//  class Peer {
//      // This class stores relevant information for a peer.
//
//      // ...
//  };
//..
// We will now define a 'PeerManager' class that maintains the information
// about all connected 'Peer's.  The 'PeerManager' class creates a 'Peer'
// object when a new connection occurs, manages the lifetime of that object,
// and eventually destroys it when the peer disconnects.  In addition, the
// 'PeerManager' class needs to pass 'Peer' objects to other objects that
// need to communicate with the 'Peer'.  Therefore, 'PeerManager' stores
// a 'bcema_SharedPtr' to each 'Peer' object.  The following would be a simple
// definition for the 'PeerManager' classes:
//..
//  class PeerManager {
//      // This class acts as a manager of peers and adds and removes peers
//      // based on peer requests and disconnections.
//
//      // DATA
//      bsl::map<int, bcema_SharedPtr<Peer> > d_peers; // list of all
//                                                     // connected peers
//
//      // ...
//  };
//..
// We now define a 'PeerCache' class that stores a subset of the peers that are
// the primary recipients of search requests.  The 'PeerCache' stores a 'Peer'
// object via a 'bcema_WeakPtr' (as opposed to via a 'bcema_SharedPtr').  This
// allows the cache to store a reference to a 'Peer' object without affecting
// its lifetime and allowing it to be destroyed asynchronously on disconnection
// by that 'Peer'.  For brevity, the population and flushing of this cache is
// not shown:
//..
//  class PeerCache {
//      // This class caches a subset of all peers that match certain criteria.
//
//      // DATA
//      bsl::list<bcema_WeakPtr<Peer> > d_cachedPeers;  // list of cached peers
//
//    public:
//      // TYPES
//      typedef bsl::list<bcema_WeakPtr<Peer> >::const_iterator PeerConstIter;
//
//      // ...
//
//      // ACCESSORS
//      PeerConstIter begin() const
//          // Return an iterator that refers to the first peer in the list of
//          // peers stored by this peer cache.
//      {
//          return d_cachedPeers.begin();
//      }
//
//      PeerConstIter end() const
//          // Return an iterator that indicates the element one past the
//          // last peer in the list of peers stored by this peer cache.
//      {
//          return d_cachedPeers.end();
//      }
//  };
//..
// We now need to store the information representing a search result that is
// returned to the user and allows the user to decide which search result to
// select for downloading.  A search result would contain a 'Peer' object and
// the filename stored by that 'Peer' that best matches the specified keywords.
// For simplicity we store the 'Peer' information and a 'bsl::string' storing
// the filename as a 'bsl::pair'.  Again, note that because we do not want
// search result objects to control the lifetime of 'Peer' objects we use a
// 'bcema_WeakPtr' to 'Peer':
//..
//  typedef bsl::pair<bcema_WeakPtr<Peer>, bsl::string> SearchResult;
//      // 'SearchResult' is an alias for a search result.
//..
// Finally, we provide a search function that retrieves the search results for
// a set of keywords by querying all peers in the 'PeerCache'.  The
// implementation of the search function is provided below:
//..
//  PeerCache cachedPeers;
//
//  void search(bsl::vector<SearchResult>       *results,
//              const bsl::vector<bsl::string>&  keywords)
//  {
//      for (PeerConstIter iter = cachedPeers.begin();
//           iter != cachedPeers.end();
//           ++iter) {
//..
// In the following, we check if the peer is still connected by acquiring a
// shared pointer to the peer.  If the acquire operation succeeds, then we can
// send the peer a request to return the filename best matching the specified
// keywords:
//..
//          bcema_SharedPtr<Peer> peerSharedPtr = iter->acquireSharedPtr();
//          if (peerSharedPtr) {
//
//              // Search the peer for the file best matching the specified
//              // keywords and, if a file is found, append the returned
//              // 'SearchResult' object to 'result'.
//
//              // ...
//          }
//      }
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>        // 'bsl::swap'
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>       // 'bsl::less'
#endif

namespace BloombergLP {

                        // ===================
                        // class bcema_WeakPtr
                        // ===================

template <class TYPE>
class bcema_WeakPtr {
    // This 'class' provides a mechanism to create weak references to
    // reference-counted shared ('bcema_SharedPtr') objects.  A weak reference
    // provides conditional access to a shared object managed by a
    // 'bcema_SharedPtr', but, unlike a shared (or "strong") reference, does
    // not affect the shared object's lifetime.

    // DATA
    TYPE               *d_ptr_p;  // pointer to the shared object (held, not
                                  // owned)

    bcema_SharedPtrRep *d_rep_p;  // pointer to the representation object
                                  // that manages the shared object (held, not
                                  // owned)

    // FRIENDS
    template <class BCE_OTHER_TYPE> friend class bcema_WeakPtr;
        // This 'friend' declaration provides access to the internal data
        // members while constructing a weak pointer from a weak pointer of a
        // different type.

  public:
    // TYPES
    typedef TYPE ElementType;
        // 'ElementType' is an alias for the 'TYPE' parameter of this class
        // template.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcema_WeakPtr,
                                 bslalg_TypeTraitBitwiseMoveable);

    // CREATORS
    bcema_WeakPtr();
        // Create a weak pointer in the empty state, i.e., a weak pointer
        // having no representation that does not refer to any object.

    bcema_WeakPtr(const bcema_WeakPtr<TYPE>& original);
        // Create a weak pointer that refers to the same object (if any) as
        // the specified 'original' weak pointer, and increment the number of
        // weak references to that object (if any).  Note that if 'original' is
        // in the empty state, this weak pointer will be initialized to the
        // empty state.

    template <typename BCE_OTHER_TYPE>
    bcema_WeakPtr(const bcema_SharedPtr<BCE_OTHER_TYPE>& other);
    template <typename BCE_OTHER_TYPE>
    bcema_WeakPtr(const bcema_WeakPtr<BCE_OTHER_TYPE>& other);
        // Create a weak pointer that refers to the same object (if any) as the
        // specified 'other' (shared or weak) pointer of the parameterized
        // 'BCE_OTHER_TYPE', and increment the number of weak references to
        // that object (if any).  If 'BCE_OTHER_TYPE *' is not implicitly
        // convertible to 'TYPE *', then a compiler diagnostic will be emitted.
        // Note that if 'other' is in the empty state, this weak pointer will
        // be initialized to the empty state.

    ~bcema_WeakPtr();
        // Destroy this weak pointer object.  If this weak pointer refers to
        // a (possibly shared) object, release the weak reference to that
        // object.

    // MANIPULATORS
    bcema_WeakPtr<TYPE>& operator=(const bcema_WeakPtr<TYPE>& rhs);
        // Make this weak pointer refer to the same object (if any) as the
        // specified 'rhs' weak pointer.  Decrement the number of weak
        // references to the object to which this weak pointer currently refers
        // (if any), and increment the number of weak references to the
        // object referred to by 'rhs' (if any).  Return a reference to this
        // modifiable weak pointer.  Note that if 'rhs' is in the empty state,
        // this weak pointer will be set to the empty state.

    template <typename BCE_OTHER_TYPE>
    bcema_WeakPtr<TYPE>& operator=(const bcema_SharedPtr<BCE_OTHER_TYPE>& rhs);
    template <typename BCE_OTHER_TYPE>
    bcema_WeakPtr<TYPE>& operator=(const bcema_WeakPtr<BCE_OTHER_TYPE>& rhs);
        // Make this weak pointer refer to the same object (if any) as the
        // specified 'rhs' (shared or weak) pointer to the parameterized
        // 'BCE_OTHER_TYPE'.  Decrement the number of weak references to the
        // object to which this weak pointer currently refers (if any), and
        // increment the number of weak references to the object referred to by
        // 'rhs' (if any).  Return a reference to this modifiable weak pointer.
        // If 'BCE_OTHER_TYPE *' is not implicitly convertible to 'TYPE *',
        // then a compiler diagnostic will be emitted.  Note that if 'rhs' is
        // in the empty state, this weak pointer will be set to the empty
        // state.

    void swap(bcema_WeakPtr<TYPE>& other);
        // Efficiently exchange the states of this weak pointer and the
        // specified 'other' weak pointer such that each will refer to the
        // object (if any) and representation formerly referred to by the
        // other.

    // ACCESSORS
    bcema_SharedPtr<TYPE> acquireSharedPtr() const;
        // Return a shared pointer to the object referred to by this weak
        // pointer if 'false == expired()', and a shared pointer in the empty
        // state otherwise.

    int numReferences() const;
        // Return a "snapshot" of the current number of shared pointers that
        // share ownership of the object referred to by this weak pointer, or 0
        // if this weak pointer is in the empty state.

    bcema_SharedPtrRep *rep() const;
        // Return the address of the modifiable 'bcema_SharedPtrRep' object
        // held by this weak pointer, or 0 if this weak pointer is in the
        // empty state.

                        // C++0x Compatibility

    // TYPES
    typedef TYPE element_type;
        // 'element_type' is an alias for the 'TYPE' parameter of this class
        // template, and is equivalent to 'ElementType'.

    // MANIPULATORS
    void reset();
        // Reset this weak pointer to the empty state.  If this weak pointer
        // refers to a (possibly shared) object, then decrement the number of
        // weak references to that object.

    // ACCESSORS
    bcema_SharedPtr<TYPE> lock() const;
        // Return a shared pointer to the object referred to by this weak
        // pointer if 'false == expired()', and a shared pointer in the empty
        // state otherwise.  Note that the behavior of this method is the same
        // as that of 'acquireSharedPtr'.

    int use_count() const;
        // Return a "snapshot" of the current number of shared pointers that
        // share ownership of the object referred to by this weak pointer, or 0
        // if this weak pointer is in the empty state.  Note that the behavior
        // of this method is the same as that of 'numReferences'.

    bool expired() const;
        // Return 'true' if this weak pointer is in the empty state or the
        // object that it originally referenced has been destroyed, and 'false'
        // otherwise.

    template <typename BCE_OTHER_TYPE>
    bool owner_before(const bcema_SharedPtr<BCE_OTHER_TYPE>& other) const;
    template <typename BCE_OTHER_TYPE>
    bool owner_before(const bcema_WeakPtr<BCE_OTHER_TYPE>& other) const;
        // Return 'true' if this weak pointer can be ordered before the
        // specified 'other' (shared or weak) pointer, and 'false' otherwise.
};

                        // C++0x Compatibility

// FREE METHODS
template <typename TYPE>
void swap(bcema_WeakPtr<TYPE>& a, bcema_WeakPtr<TYPE>& b);
    // Efficiently exchange the states of the specified 'a' and 'b' weak
    // pointers such that each will refer to the object (if any) and
    // representation formerly referred to by the other.

// ============================================================================
//                 INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // -------------------
                        // class bcema_WeakPtr
                        // -------------------

// CREATORS
template <typename TYPE>
inline
bcema_WeakPtr<TYPE>::bcema_WeakPtr()
: d_ptr_p(0)
, d_rep_p(0)
{
}

template <typename TYPE>
bcema_WeakPtr<TYPE>::bcema_WeakPtr(const bcema_WeakPtr<TYPE>& original)
: d_ptr_p(original.d_ptr_p)
, d_rep_p(original.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->acquireWeakRef();
    }
}

template <typename TYPE>
template <typename BCE_OTHER_TYPE>
bcema_WeakPtr<TYPE>::bcema_WeakPtr(
                                  const bcema_SharedPtr<BCE_OTHER_TYPE>& other)
: d_ptr_p(other.ptr())
, d_rep_p(other.rep())
{
    if (d_rep_p) {
        d_rep_p->acquireWeakRef();
    }
}

template <typename TYPE>
template <typename BCE_OTHER_TYPE>
bcema_WeakPtr<TYPE>::bcema_WeakPtr(const bcema_WeakPtr<BCE_OTHER_TYPE>& other)
: d_ptr_p(other.d_ptr_p)
, d_rep_p(other.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->acquireWeakRef();
    }
}

template <typename TYPE>
inline
bcema_WeakPtr<TYPE>::~bcema_WeakPtr()
{
    if (d_rep_p) {
        d_rep_p->releaseWeakRef();
    }
}

// MANIPULATORS
template <typename TYPE>
bcema_WeakPtr<TYPE>& bcema_WeakPtr<TYPE>::operator=(
                                                const bcema_WeakPtr<TYPE>& rhs)
{
    bcema_WeakPtr<TYPE> tmp(rhs);
    tmp.swap(*this);
    return *this;
}

template <typename TYPE>
template <typename BCE_OTHER_TYPE>
bcema_WeakPtr<TYPE>& bcema_WeakPtr<TYPE>::operator=(
                                    const bcema_SharedPtr<BCE_OTHER_TYPE>& rhs)
{
    bcema_WeakPtr<TYPE> tmp(rhs);
    tmp.swap(*this);
    return *this;
}

template <typename TYPE>
template <typename BCE_OTHER_TYPE>
bcema_WeakPtr<TYPE>& bcema_WeakPtr<TYPE>::operator=(
                                      const bcema_WeakPtr<BCE_OTHER_TYPE>& rhs)
{
    bcema_WeakPtr<TYPE> tmp(rhs);
    tmp.swap(*this);
    return *this;
}

template <typename TYPE>
void bcema_WeakPtr<TYPE>::swap(bcema_WeakPtr<TYPE>& other)
{
    bsl::swap(d_ptr_p, other.d_ptr_p);
    bsl::swap(d_rep_p, other.d_rep_p);
}

// ACCESSORS
template <typename TYPE>
bcema_SharedPtr<TYPE> bcema_WeakPtr<TYPE>::acquireSharedPtr() const
{
    if (d_rep_p && d_rep_p->tryAcquireRef()) {
        return bcema_SharedPtr<TYPE>(d_ptr_p, d_rep_p);
    }
    return bcema_SharedPtr<TYPE>();
}

template <typename TYPE>
inline
int bcema_WeakPtr<TYPE>::numReferences() const
{
    return d_rep_p ? d_rep_p->numReferences() : 0;
}

template <typename TYPE>
inline
bcema_SharedPtrRep *bcema_WeakPtr<TYPE>::rep() const
{
    return d_rep_p;
}

                        // C++0x Compatibility

// MANIPULATORS
template <typename TYPE>
inline
void bcema_WeakPtr<TYPE>::reset()
{
    if (d_rep_p) {
        d_rep_p->releaseWeakRef();
    }

    d_ptr_p = 0;
    d_rep_p = 0;
}

// ACCESSORS
template <typename TYPE>
inline
bcema_SharedPtr<TYPE> bcema_WeakPtr<TYPE>::lock() const
{
    return acquireSharedPtr();
}

template <typename TYPE>
inline
int bcema_WeakPtr<TYPE>::use_count() const
{
    return numReferences();
}

template <typename TYPE>
inline
bool bcema_WeakPtr<TYPE>::expired() const
{
    return !(d_rep_p && d_rep_p->numReferences());
}

template <typename TYPE>
template <typename BCE_OTHER_TYPE>
inline
bool bcema_WeakPtr<TYPE>::owner_before(
                            const bcema_SharedPtr<BCE_OTHER_TYPE>& other) const
{
    return bsl::less<bcema_SharedPtrRep *>()(d_rep_p, other.rep());
}

template <typename TYPE>
template <typename BCE_OTHER_TYPE>
inline
bool bcema_WeakPtr<TYPE>::owner_before(
                              const bcema_WeakPtr<BCE_OTHER_TYPE>& other) const
{
    return bsl::less<bcema_SharedPtrRep *>()(d_rep_p, other.d_rep_p);
}

// FREE METHODS
template <typename TYPE>
inline
void swap(bcema_WeakPtr<TYPE>& a, bcema_WeakPtr<TYPE>& b)
{
    a.swap(b);
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
