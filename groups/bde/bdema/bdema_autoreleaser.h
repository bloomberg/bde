// bdema_autoreleaser.h      -*-C++-*-
#ifndef INCLUDED_BDEMA_AUTORELEASER
#define INCLUDED_BDEMA_AUTORELEASER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Release memory from a managed allocator or a pool at destruction.
//
//@CLASSES:
//  bdema_AutoReleaser: proctor to release memory to a managed allocator/pool
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@SEE_ALSO: bslma_deallocatorproctor, bslma_managedallocator
//
//@DESCRIPTION: This component provides a proctor object to manage memory
// allocated from a managed allocator or a pool.  The proctor's destructor
// invokes the 'release' method of its managed allocator or pool unless the
// proctor's 'release' method has been called.  Note that after a proctor
// releases management of its managed allocator or pool, the proctor can be
// reused by invoking its 'reset' method with another allocator or pool object
// (of the same type 'ALLOCATOR').
//
///REQUIREMENT
///-----------
// The object of the templatized type 'ALLOCATOR' must provide a method having
// the following signature:
//..
//  void release();
//..
///Usage
///-----
// The 'bdema_AutoReleaser' proctor is often used to preserve exception
// neutrality for containers that allocate their elements using a managed
// allocator or a pool.  For operations that may potentially throw an
// exception, a proctor can be used to (temporarily) manage the container's
// allocator or pool and its associated memory.  If an exception is thrown, the
// proctor's destructor invokes the 'release' method of its held allocator or
// pool, deallocating memory for all of the container's elements, thereby
// preventing a memory leak and restoring the container to the empty state.
// The following 'my_FastStrArray' object allocates memory for its elements
// using its 'my_StrPool' (not shown here) member.  In the 'operator=' method,
// a 'bdema_AutoReleaser' proctor is used to manage the array's string memory
// pool while allocating memory for the new elements.  If an exception is
// thrown, the proctor's destructor releases memory for all elements allocated
// through the pool.
//..
//  // my_fastcstrarray.h
//
//  class my_FastCstrArray {
//      // This class implements an array of C string elements.  Each C string
//      // is allocated using the 'my_StrPool' member for fast memory
//      // allocation and deallocation.
//
//      // DATA
//      char       **d_array_p;  // dynamically allocated array
//      int          d_size;     // physical capacity of this array
//      int          d_length;   // logical length of this array
//      my_StrPool   d_strPool;  // memory manager to supply memory
//
//    private:
//      void increaseSize();
//
//    public:
//      my_FastCstrArray();
//      ~my_FastCstrArray();
//
//      void append(const char *item);
//      my_FastCstrArray& operator=(const my_FastCstrArray& original);
//      const char *operator[](int ind) const { return d_array_p[ind];}
//      int length() const { return d_length; }
//  };
//
//  // ...
//
//  // my_fastcstrarray.cpp
//
//  // ...
//
//  my_FastCstrArray&
//  my_FastCstrArray::operator=(const my_FastCstrArray& original)
//  {
//      if (&original != this) {
//          d_strPool.release();
//          d_length = 0;
//          if (original.d_length > d_size) {
//              char **tmp = d_array_p;
//              d_array_p =
//              (char **) operator new(original.d_length * sizeof *d_array_p);
//              d_size = original.d_length;
//              operator delete(tmp);
//          }
//          bdema_AutoReleaser<my_StrPool> autoReleaser(&d_strPool);
//          for (int i = 0; i < original.d_length; ++i) {
//              int size = strlen(original.d_array_p[i]) + 1;
//              d_array_p[i] = (char *) d_strPool.allocate(size);
//              memcpy(d_array_p[i], original.d_array_p[i], size);
//          }
//          d_length = original.d_length;
//          autoReleaser.release();
//      }
//      return *this;
//  }
//
//  // ...
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

                        // ========================
                        // class bdema_AutoReleaser
                        // ========================

template <class ALLOCATOR>
class bdema_AutoReleaser {
    // This class implements a proctor that invokes the 'release' method of its
    // managed allocator or pool at destruction unless the proctor's 'release'
    // method is invoked.

    // DATA
    ALLOCATOR *d_allocator_p;  // holds (but doesn't own) the allocator or pool

    // NOT IMPLEMENTED
    bdema_AutoReleaser(const bdema_AutoReleaser<ALLOCATOR>&);
    bdema_AutoReleaser<ALLOCATOR>& operator=(const bdema_AutoReleaser&);

  public:
    // CREATORS
    bdema_AutoReleaser(ALLOCATOR *originalAllocator);
        // Create a proctor object to manage the specified 'originalAllocator'.
        // Unless the 'release' method of this proctor is invoked, the
        // 'release' method of 'originalAllocator' is automatically invoked
        // upon destruction of this proctor.

    ~bdema_AutoReleaser();
        // Destroy this proctor object and, unless the 'release' method has
        // been invoked on this object with no subsequent call to
        // 'reset', invoke the 'release' method of the held managed
        // allocator or pool.

    // MANIPULATORS
    void release();
        // Release from management the allocator or pool currently managed by
        // this proctor.  If no allocator or pool is currently being managed,
        // this method has no effect.

    void reset(ALLOCATOR *newAllocator);
        // Set the specified 'originalAllocator' as the allocator or pool to be
        // managed by this proctor.  Note that this method releases from
        // management any previously-held allocator or pool, and so may be
        // called with or without having called 'release' when reusing this
        // object.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class bdema_AutoReleaser
                        // ------------------------

// CREATORS
template <class ALLOCATOR>
inline
bdema_AutoReleaser<ALLOCATOR>::bdema_AutoReleaser(ALLOCATOR *originalAllocator)
: d_allocator_p(originalAllocator)
{
}

template <class ALLOCATOR>
inline
bdema_AutoReleaser<ALLOCATOR>::~bdema_AutoReleaser()
{
    if (d_allocator_p) {
        d_allocator_p->release();
    }
}

// MANIPULATORS
template <class ALLOCATOR>
inline
void bdema_AutoReleaser<ALLOCATOR>::release()
{
    d_allocator_p = 0;
}

template <class ALLOCATOR>
inline
void bdema_AutoReleaser<ALLOCATOR>::reset(ALLOCATOR *newAllocator)
{
    d_allocator_p = newAllocator;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
