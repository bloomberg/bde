// bslalg_autoarraydestructor.h                                       -*-C++-*-
#ifndef INCLUDED_BSLALG_AUTOARRAYDESTRUCTOR
#define INCLUDED_BSLALG_AUTOARRAYDESTRUCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor for destroying arrays.
//
//@CLASSES:
//  bslalg::AutoArrayDestructor: exception-neutrality proctor for arrays
//
//@SEE_ALSO: bslma_autodestructor
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a proctor object to manage a contiguous
// (in-place) sequence of otherwise-unmanaged instances of a user-defined type.
// If not explicitly released, all objects managed by the proctor object are
// automatically destroyed by the proctor's destructor, using the
// 'bslalg_arraydestructionprimitives'.
//
// In most instances, 'bslma::AutoDestructor' can also be used, but this
// component is more useful in cases where it is simpler to think in terms of
// two pointers at the ends of the array being managed, rather than an origin
// and offset.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Managing an Array Under Construction
///- - - - - - - - - - - - - - - - - - - - - - - -
// In most instances, the use of a 'bslalg::AutoArrayDestructor' could be
// handled by a 'bslma::AutoDeallocator', but sometimes it is conceptually
// clearer to frame the problem in terms of a pair of pointers rather than a
// pointer and an offset.
//
// Suppose we have a class, 'UsageType' that allocates a block of memory upon
// construction, and whose constructor takes a char.  Suppose we want to create
// an array of elements of such objects in an exception-safe manner.
//
// First, we create the type 'UsageType':
//..
//                               // ===============
//                               // class UsageType
//                               // ===============
//..
//  class UsageType {
//      // This test type contains a 'char' in some allocated storage.  It has
//      // no traits other than using a 'bslma' allocator.
//
//      char             *d_data_p;         // managed single char
//      bslma::Allocator *d_allocator_p;    // allocator (held, not owned)
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(UsageType,
//                                   bslalg::TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      explicit UsageType(char c, bslma::Allocator *basicAllocator = 0)
//      : d_data_p(0)
//      , d_allocator_p(bslma::Default::allocator(basicAllocator))
//      {
//          d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
//          *d_data_p = c;
//      }
//
//      ~UsageType()
//      {
//          *d_data_p = '_';
//          d_allocator_p->deallocate(d_data_p);
//          d_data_p = 0;
//      }
//
//      // ACCESSORS
//      char datum() const
//      {
//          return *d_data_p;
//      }
//  };
//..
// Then, in 'main', we create a 'TestAllocator' to supply memory (and to verify
// that no memory is leaked):
//..
//  bslma::TestAllocator ta;
//..
// Next, we create the pointer for our array:
//..
//  UsageType *array;
//..
// Then, we declare a string of chars we will use to initialize the 'UsageType'
// objects in our array.
//..
//  const char *DATA = "Hello";
//  const int   DATA_LEN = std::strlen(DATA);
//..
// Next, we verify that even right after exceptions have been thrown and
// caught, no memory is outstanding:
//..
//  assert(0 == ta.numBlocksInUse());
//..
// Then, we allocate our array and create a guard to free it if a subsequent
// allocation throws an exception:
//..
//  array = (UsageType *) ta.allocate(DATA_LEN * sizeof(UsageType));
//  bslma::DeallocatorProctor<bslma::Allocator> arrayProctor(array, &ta);
//..
// Next, we establish an 'AutoArrayDestructor' on 'array' to destroy any valid
// elements in 'array' if an exception is thrown:
//..
//  bslalg::AutoArrayDestructor<UsageType> arrayElementProctor(array, array);
//..
// Note that we pass 'arrayElementProctor' pointers to the beginning and end
// of the range to be guarded (we start with an empty range since no elements
// have been constructed yet).
//
// Then, we iterate through the valid chars in 'DATA' and use them to construct
// the elements of the array:
//..
//  UsageType *resultElement = array;
//  for (const char *nextChar = DATA; *nextChar; ++nextChar) {
//..
// Next, construct the next element of 'array':
//..
//      new (resultElement++) UsageType(*nextChar, &ta);
//..
// Now, move the end of 'arrayElementProctor' to cover the most recently
// constructed element:
//..
//      arrayElementProctor.moveEnd(1);
//  }
//..
// At this point, we have successfully created our array.
//
// Then, release the guards so they won't destroy our work when they go out of
// scope:
//..
//  arrayProctor.release();
//  arrayElementProctor.release();
//..
// Next, exit the exception testing block:
//
// Then, verify that the array we have created is as expected:
//..
//  assert('H' == array[0].datum());
//  assert('e' == array[1].datum());
//  assert('l' == array[2].datum());
//  assert('l' == array[3].datum());
//  assert('o' == array[4].datum());
//..
// Finally, destroy & free our work and verify that no memory is leaked:
//..
//  for (int i = 0; i < DATA_LEN; ++i) {
//      array[i].~UsageType();
//  }
//  ta.deallocate(array);
//
//  assert(0 == ta.numBlocksInUse());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES
#include <bslalg_arraydestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // size_t
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslalg {

                        // =========================
                        // class AutoArrayDestructor
                        // =========================

template <class OBJECT_TYPE>
class AutoArrayDestructor {
    // This 'class' provides a specialized proctor object that, upon
    // destruction and unless the 'release' method has been called, destroys
    // the elements in a segment of an array of parameterized type
    // 'OBJECT_TYPE'.  The elements destroyed are delimited by the "guarded"
    // range '[ begin(), end() )'.

    // DATA
    OBJECT_TYPE *d_begin_p;  // address of first element in guarded range

    OBJECT_TYPE *d_end_p;    // first address beyond last element in guarded
                             // range

  private:
    // NOT IMPLEMENTED
    AutoArrayDestructor(const AutoArrayDestructor&);
    AutoArrayDestructor& operator=(const AutoArrayDestructor&);

  public:
    // TYPES
    typedef std::ptrdiff_t difference_type;

    // CREATORS
    AutoArrayDestructor(OBJECT_TYPE *begin, OBJECT_TYPE *end);
        // Create an array exception guard object for the sequence of elements
        // of the parameterized 'OBJECT_TYPE' delimited by the range specified
        // by '[ begin, end )'.  The behavior is undefined unless
        // 'begin <= end' and each element in the range '[ begin, end )' has
        // been initialized.

    ~AutoArrayDestructor();
        // Call the destructor on each of the elements of the parameterized
        // 'OBJECT_TYPE' delimited by the range '[ begin(), end() )' and
        // destroy this array exception guard.

    // MANIPULATORS
    OBJECT_TYPE *moveBegin(difference_type offset = -1);
        // Move the begin pointer by the specified 'offset', and return the new
        // begin pointer.

    OBJECT_TYPE *moveEnd(difference_type offset = 1);
        // Move the end pointer by the specified 'offset', and return the new
        // end pointer.

    void release();
        // Set the range of elements guarded by this object to be empty.  Note
        // that 'begin() == end()' following this operation, but the specific
        // value is unspecified.
};

// ===========================================================================
//                          INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // -------------------------
                      // class AutoArrayDestructor
                      // -------------------------

// CREATORS
template <class OBJECT_TYPE>
inline
AutoArrayDestructor<OBJECT_TYPE>::AutoArrayDestructor(OBJECT_TYPE *begin,
                                                      OBJECT_TYPE *end)
: d_begin_p(begin)
, d_end_p(end)
{
    BSLS_ASSERT_SAFE(!begin == !end);
    BSLS_ASSERT_SAFE(begin <= end);
}

template <class OBJECT_TYPE>
inline
AutoArrayDestructor<OBJECT_TYPE>::~AutoArrayDestructor()
{
    BSLS_ASSERT_SAFE(!d_begin_p == !d_end_p);
    BSLS_ASSERT_SAFE(d_begin_p <= d_end_p);

    ArrayDestructionPrimitives::destroy(d_begin_p, d_end_p);
}

// MANIPULATORS
template <class OBJECT_TYPE>
inline
OBJECT_TYPE *AutoArrayDestructor<OBJECT_TYPE>::moveBegin(
                                                        difference_type offset)
{
    BSLS_ASSERT_SAFE(d_begin_p || 0 == offset);
    BSLS_ASSERT_SAFE(d_end_p - d_begin_p >= offset);

    d_begin_p += offset;
    return d_begin_p;
}

template <class OBJECT_TYPE>
inline
OBJECT_TYPE *AutoArrayDestructor<OBJECT_TYPE>::moveEnd(difference_type offset)
{
    BSLS_ASSERT_SAFE(d_end_p || 0 == offset);
    BSLS_ASSERT_SAFE(d_end_p - d_begin_p >= -offset);

    d_end_p += offset;
    return d_end_p;
}

template <class OBJECT_TYPE>
inline
void AutoArrayDestructor<OBJECT_TYPE>::release()
{
    d_begin_p = d_end_p;
}

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslalg_AutoArrayDestructor
#undef bslalg_AutoArrayDestructor
#endif
#define bslalg_AutoArrayDestructor bslalg::AutoArrayDestructor
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
