// bcecs_idxclerk.h   -*-C++-*-
#ifndef INCLUDED_BCECS_IDXCLERK
#define INCLUDED_BCECS_IDXCLERK

//@PURPOSE: Provide a manager of reusable, non-negative integer indices.
//
//@CLASSES:
//       bcecs_IdxClerk: manager of reusable, non-negative integer indices
//   bcecs_IdxClerkIter: sequential accessor to decommissioned indices
//
//@AUTHOR: David Rubin (drubin6)
//
//@DESCRIPTION: This component implements an efficient, thread-safe manager of
// reusable, non-negative integer indices.  Each new instance of a
// 'bcecs_IdxClerk' will issue consecutive integers on request, beginning with
// { 0, 1, 2, ... N }.  Indices that are no longer needed may be returned for
// reuse.  Existing decommissioned indices are reissued before any new ones are
// created.  A 'bcecs_IdxClerkIter' is provided to enable sequential read-only
// access to the currently decommissioned indices.
//
///Exception Safety
///----------------
// The 'bcecs_IdxClerk' is exception safe with respect to its manipulators and
// accessors.  In particular, the methods 'getIndex' and 'putIndex' are
// exception safe.  This exception safety is due to the fact that the index
// clerk maintains a fixed-size stack of decommissioned indices, and thus does
// not allocate (or deallocate) any memory during its operation.
//
///Performance
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, O[f(N,M)], where the names 'N' and 'M' also refer to
// the number of respective elements in each container (i.e., its 'length()').
//..
//     Operation                     Worst Case
//     ---------                     ----------
//     DEFAULT CTOR                  O[1]
//     COPY CTOR(N)                  O[N]
//     N.DTOR()                      O[1]
//     N.OP=(M)                      O[M]
//     OP==(N,M)                     O[min(N,M)]
//
//     N.getIndex()                  O[1]
//     N.putIndex(index)             O[1]
//     N.removeAll()                 O[1]
//     N.numCommissionedIndices()    O[1]
//     N.numDecommissionedIndices()  O[1]
//     N.nextNewIndex()              O[1]
//..
///Usage Examples
///--------------
// A 'bcecs_IdxClerk' is commonly used in conjunction with an array to enable
// machine-address-independent referencing.  Rather than dynamically allocating
// an object and holding its address, the object is stored in the array at the
// next position dispensed by its associated 'bdecs_IdxClerk', and that index
// becomes an identifier (Id) for the new object.  Instead of destroying an
// unneeded object, its Id is merely returned to the clerk.
//
// Care must be taken to ensure that objects created at reused indices (i.e.,
// indices below the current length of the array) *replace* an existing object
// in the array while objects at new indices (i.e., indices at the current
// length) are *appended* to the array.
//
// For example, to add and remove Security values from a Security array/clerk
// pair, you might write the following two functions:
//..
// int addSecurity(std::vectory<Security> *securityArray,
//                 bcecs_IdxClerk         *securityClerk,
//                 const Security&         newSecurity)
//      // Add a copy of the specified newSecurity to the specified
//      // securityArray, update the specified securityClerk, and
//      // return the Id of the newly added security.
// {
//      int id = securityClerk->getIndex();
//
//      if (id < securityArray->size()) {
//          securityArray[id] = newSecurity;
//      }
//      else {
//          securityArray->push_back(newSecurity);
//      }
//
//      return id;
// }
//
// void removeSecurity(std::vectory<Security> *securityArray,
//                     bdecs_IdxClerk         *securityClerk,
//                     int                     securityId)
//      // "Free" the security object in the specified securityArray
//      // identified by the specified securityId, and update the specified
//      // securityClerk (making securityId available for reuse).
// {
//      (*securityArray)[securityId] = Security(); // (Optional)
//
//      securityClerk->putIndex(securityId);
// }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_READERWRITERLOCK
#include <bcemt_readerwriterlock.h>
#endif

#ifndef INCLUDED_BDECS_IDXCLERK
#include <bdecs_idxclerk.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

namespace BloombergLP {

class bdema_Allocator;

                        // ====================
                        // class bcecs_IdxClerk
                        // ====================

class bcecs_IdxClerk {
    // This class implements an efficient manager of reusable, non- negative
    // integer indices.

    bdecs_IdxClerk                 d_clerk;     // index clerk implementation
    int                            d_maxIndex;  // 0 if no maximum is specified
    mutable bcemt_ReaderWriterLock d_lock;

    friend int operator==(const bcecs_IdxClerk&, const bcecs_IdxClerk&);

  public:
    // CREATORS
    bcecs_IdxClerk(int maxIndex, bdema_Allocator *basicAllocator = 0);
        // Create a new index clerk that dispenses consecutive non-negative
        // integers from the set { 0, 1, 2, ... 'maxIndex' }; however, indices
        // returned via 'putIndex' will be reissued before any new ones are
        // created.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless '0 < maxIndex'.

    bcecs_IdxClerk(const bcecs_IdxClerk&  original,
                   bdema_Allocator       *basicAllocator = 0);
        // Create an independent copy of the specified 'original' index clerk.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~bcecs_IdxClerk();
        // Destroy this object.

    // MANIPULATORS
    bcecs_IdxClerk& operator=(const bcecs_IdxClerk& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference to this modifiable object.

    int getIndex(void);
        // Return the next, non-negative, available unused integer index, or a
        // negative value if no such index is available.  Existing
        // decommissioned indices are reissued before new ones are created.

    void putIndex(int index);
        // Indicate to this clerk that the specified 'index' is no longer
        // in use and may be reissued.  The behavior is undefined if the
        // specified 'index' has never been generated by this clerk or is
        // currently decommissioned.

    void removeAll();
        // Remove all the indices from the clerk.  Note that this operation
        // logically results in the following assertions.
        //..
        //      ASSERT(0 == numCommissionedIndices());
        //      ASSERT(0 == numDecommissionedIndices());
        //      ASSERT(0 == nextNewIndex());
        //..

    // ACCESSORS
    int maxIndex() const;
        // Return the maximum index that can be issued by this clerk, or 0 if
        // the set of indices is unbounded.

    int numCommissionedIndices() const;
        // Return the number of indices currently being used.

    int numDecommissionedIndices() const;
        // Return the number of indices that have been decommissioned.

    int nextNewIndex() const;
        // Return the smallest (non-negative) index not issued by this clerk.
};

// FREE OPERATORS
inline
int operator==(const bcecs_IdxClerk& lhs, const bcecs_IdxClerk& rhs);
    // Return 1 if the two index clerks are logically equivalent (i.e.,
    // the two clerks currently return the same value for nextNewIndex()
    // and would always generate the same sequence of integer indices) and
    // 0 otherwise.

inline
int operator!=(const bcecs_IdxClerk& lhs, const bcecs_IdxClerk& rhs);
    // Return 1 if the two index clerks are not logically equivalent (i.e.,
    // the two clerks currently return different values for nexNewIndex()
    // or may fail to generate the same sequence of integer indices) and 0
    // otherwise.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------
                        // class bcecs_IdxClerk
                        // --------------------

// CREATORS
inline
bcecs_IdxClerk::bcecs_IdxClerk(int maxIndex, bdema_Allocator *basicAllocator)
: d_clerk(basicAllocator)
, d_maxIndex(maxIndex)
{
    BDE_ASSERT_H(0 < maxIndex);

    std::vector<int> indices;
    indices.reserve(maxIndex);

    while(0 < maxIndex) {
        indices.push_back(d_clerk.getIndex());
        --maxIndex;
    }

    for (std::vector<int>::reverse_iterator it = indices.rbegin();
         it != indices.rend(); ++it)
    {
        d_clerk.putIndex(*it);
    }
}

inline
bcecs_IdxClerk::bcecs_IdxClerk(
        const bcecs_IdxClerk&  original,
        bdema_Allocator       *basicAllocator)
: d_clerk(basicAllocator)
, d_maxIndex(original.d_maxIndex)
{
    original.d_lock.lockRead();
    d_clerk = original.d_clerk;
    original.d_lock.unlock();
}

// MANIPULATORS
inline
bcecs_IdxClerk& bcecs_IdxClerk::operator=(const bcecs_IdxClerk& rhs)
{
    if (this != &rhs) {
        d_lock.lockWrite();
        rhs.d_lock.lockRead();

        d_clerk = rhs.d_clerk;
        d_maxIndex = rhs.d_maxIndex;

        rhs.d_lock.unlock();
        d_lock.unlock();
    }
    return *this;
}

inline
int bcecs_IdxClerk::getIndex()
{
    d_lock.lockWrite();
    int index = d_clerk.getIndex();
    d_lock.unlock();

    return (d_maxIndex && d_maxIndex < index) ? -1 : index;
}

inline
void bcecs_IdxClerk::putIndex(int index)
{
    d_lock.lockWrite();
    d_clerk.putIndex(index);
    d_lock.unlock();
}

inline
void bcecs_IdxClerk::removeAll()
{
    d_lock.lockWrite();
    d_clerk.removeAll();
    d_lock.unlock();
}

// ACCESSORS
inline
int bcecs_IdxClerk::maxIndex() const
{
    return d_maxIndex;
}

inline
int bcecs_IdxClerk::numCommissionedIndices() const
{
    d_lock.lockRead();
    int rc = d_clerk.numCommissionedIndices();
    d_lock.unlock();

    return rc;
}

inline
int bcecs_IdxClerk::numDecommissionedIndices() const
{
    d_lock.lockRead();
    int rc = d_clerk.numDecommissionedIndices();
    d_lock.unlock();

    return rc;
}

inline
int bcecs_IdxClerk::nextNewIndex() const
{
    d_lock.lockRead();
    int rc = d_clerk.nextNewIndex();
    d_lock.unlock();

    return rc;
}

// FREE OPERATORS
inline
int operator==(const bcecs_IdxClerk& lhs, const bcecs_IdxClerk& rhs)
{
    lhs.d_lock.lockRead();
    rhs.d_lock.lockRead();

    int rc = lhs.d_clerk == rhs.d_clerk
          && lhs.d_maxIndex == rhs.d_maxIndex;

    rhs.d_lock.unlock();
    lhs.d_lock.unlock();

    return rc;
}

inline
int operator!=(const bcecs_IdxClerk& lhs, const bcecs_IdxClerk& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP
#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
