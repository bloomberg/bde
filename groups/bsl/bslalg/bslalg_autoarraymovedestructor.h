// bslalg_autoarraymovedestructor.h                                   -*-C++-*-
#ifndef INCLUDED_BSLALG_AUTOARRAYMOVEDESTRUCTOR
#define INCLUDED_BSLALG_AUTOARRAYMOVEDESTRUCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor for destroying arrays.
//
//@CLASSES:
//  bslalg::AutoArrayMoveDestructor: exception-neutrality guard for arrays
//
//@SEE_ALSO: bslma_autodestructor, bslalg_autoarraydestructor
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a proctor object to manage a contiguous
// (in-place) sequence of otherwise-unmanaged instances of a user-defined type.
// If not explicitly released, all objects managed by the proctor object are
// automatically destroyed by the proctor's destructor or moved back to their
// original area, using the 'bslalg_arraydestructionprimitives' and
// 'std::memmove'.  This component is intended to be used only with bit-wise
// moveable types, and for a very special purpose as shown in the usage
// example.
//
// Overview of the operation of 'AutoArrayMoveDestructor':
// ------------------------------------------------------
// Supposee we want to double the length of an array by inserting copies of a
// specified 'value' at the beginning.  We are to assume there is ample
// uninitialized memory after the end of the initial array.
//
// Legend:
//..
//    'ABCDE'   -- (uppercase) initial array elements.
//    'v'       -- copy of specified 'value' being inserted.
//    '.'       -- (period) uninitialized memory.
//    '^(,)'    -- area guarded by 'AutoArrayMoveDestructor', where:
//                 '^' -- position of 'guard.destination()'
//                 '(' -- position of 'guard.begin()'
//                 ',' -- (comma) position of 'guard.middle()'
//                 ')' -- position of 'guard.end()'
//
// The copy constructor for the type being inserted may throw, so we need to
// have a guard object which can allow us to make some guarantee about the
// state of things after the guard is destroyed.  What we want to guarantee is
// that there are as many valid objects at the start of the array as before
// with no other valid objects in existence.
//..
//  1: 'ABCDE.....'      -- initial memory.
//  2: '.....ABCDE'      -- memory after first 'std::memcpy'.
//  3: '^.....(,ABCDE)'  -- memory immediately after 'guard' is set
//  4: 'vv^...(AB,CDE)'  -- memory after 2 copies of 'value' have been
//                          created, and 'guard.advance()' has been called
//                          twice.
//  5: 'vvvvv^(ABCDE,)'  -- memory after insertion is completed
//  6: 'vvvvvABCDE'      -- memory after guard was destroyed (at which point
//                          'guard.middle() == guard.end()' so destructor did
//                          nothing.
//..
// Now suppose we threw after step 4, destroying 'guard'.
//..
//  4: 'vv^...(AB,CDE)' -- same as step '4' above, before destructor starts
//  5: 'vv^CDE(AB,...)' -- memory after 'guard's destructor moves 'CDE' back to
//                          their position before we began
//  6: 'vv^CDE(..,...)' -- memory after 'guard's destructor destroys 'A' and
//                           'B'
//  7: 'vvCDE.....'     -- memory after 'guard's destructor completes
//..
// We now have 5 valid elements in the beginning of the range, as it was when
// we started, making the situation predictable for our next d'tor.
//
// This was a very simple case, but using this guard in conjunction with
// 'bslalg::AutoArrayDestructor', we can implment the more general cases of
// inserting arbitrary numbers of elements at the beginning of an array.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Doubling the Length of an Array
/// - - - - - - - - - - - - - - - - - - - - -
// First, we create the class 'TestType', which is bitwise-movable and
// allocates memory upon construction:
//..
//                                 // ==============
//                                 // class TestType
//                                 // ==============
//
//  class TestType {
//      // This test type contains a 'char' in some allocated storage.  It
//      // counts the number of default and copy constructions, assignments,
//      // and destructions.  It has no traits other than using a 'bslma'
//      // allocator.  It could have the bit-wise moveable traits but we defer
//      // that trait to the 'MoveableTestType'.
//
//      char             *d_data_p;
//      bslma::Allocator *d_allocator_p;
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS2(TestType,
//                                    bslalg::TypeTraitUsesBslmaAllocator,
//                                    bslalg::TypeTraitBitwiseMoveable);
//
//      // CREATORS
//      explicit TestType(bslma::Allocator *basicAllocator = 0)
//      : d_data_p(0)
//      , d_allocator_p(bslma::Default::allocator(basicAllocator))
//      {
//          ++numDefaultCtorCalls;
//          d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
//          *d_data_p = '?';
//      }
//
//      explicit TestType(char c, bslma::Allocator *basicAllocator = 0)
//      : d_data_p(0)
//      , d_allocator_p(bslma::Default::allocator(basicAllocator))
//      {
//          ++numCharCtorCalls;
//          d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
//          *d_data_p = c;
//      }
//
//      TestType(const TestType&   original,
//               bslma::Allocator *basicAllocator = 0)
//      : d_data_p(0)
//      , d_allocator_p(bslma::Default::allocator(basicAllocator))
//      {
//          ++numCopyCtorCalls;
//          if (&original != this) {
//              d_data_p  = (char *)d_allocator_p->allocate(sizeof(char));
//              *d_data_p = *original.d_data_p;
//          }
//      }
//
//      ~TestType()
//      {
//          ++numDestructorCalls;
//          *d_data_p = '_';
//          d_allocator_p->deallocate(d_data_p);
//          d_data_p = 0;
//      }
//
//      // MANIPULATORS
//      TestType& operator=(const TestType& rhs)
//      {
//          ++numAssignmentCalls;
//          if (&rhs != this) {
//              char *newData = (char *)d_allocator_p->allocate(sizeof(char));
//              *d_data_p = '_';
//              d_allocator_p->deallocate(d_data_p);
//              d_data_p  = newData;
//              *d_data_p = *rhs.d_data_p;
//          }
//          return *this;
//      }
//
//      void setDatum(char c) { *d_data_p = c; }
//
//      // ACCESSORS
//      char datum() const { return *d_data_p; }
//  };
//
//  bool operator==(const TestType& lhs, const TestType& rhs)
//  {
//      assert(isalpha(lhs.datum()));
//      assert(isalpha(rhs.datum()));
//
//      return lhs.datum() == rhs.datum();
//  }
//..
// Then, we define the function 'insertItems' which uses
// 'AutoArrayMoveDestructor' to ensure that if an exception is thrown (e.g.,
// when allocating memory), the array will be left in a state where it has the
// same number of elements, in the same location, as when the function begin
// (though not necessarily the same value).
//..
//  void insertItems(TestType         *start,
//                   TestType         *divider,
//                   const TestType    value,
//                   bslma::Allocator *allocator)
//      // The memory in the specified range '[ start, divider )' contains
//      // valid elements, and the range of valid elements is to be doubled by
//      // inserting 'divider - start' copies of the specified 'value' at
//      // 'start', shifting the existing valid values back in memory.  Assume
//      // that following the pointer 'divider' is sufficient uninitialized
//      // memory, and that the type 'TestType' is bitwise-movable
//      // ('AutoArrayMoveDestructor' will only work bitwise-movable types).
//  {
//      TestType *finish = divider + (divider - start);
//
//      assert((bslalg::HasTrait<TestType,
//                               bslalg::TypeTraitUsesBslmaAllocator>::VALUE));
//      assert((bslalg::HasTrait<TestType,
//                               bslalg::TypeTraitBitwiseMoveable   >::VALUE));
//
//      // The range '[ start, divider )' contains valid elements.  The range
//      // '[ divider, finish )' is of equal length and contains uninitialized
//      // memory.  We want to insert 'divider - start' copies of the specified
//      // 'value' at the front half of the range '[ start, finish )', moving
//      // the exising elements back to make room for them.  Note that the copy
//      // c'tor of 'TestType' allocates memory and may throw, so we have to
//      // leave the array in a somewhat predicatable state if we do throw.
//      // What the bslalg::AutoArrayMoveDestructor will do is guarantee that,
//      // if it is destroyed before the insertion is complete, the range
//      // '[ start, divider )' will contain valid elements, and that no other
//      // valid elements will exist.
//      //
//      // Note that the existing elements, which are bitwise-moveable, may be
//      // *moved* about the container without the possibility of throwing an
//      // exception, but the newly inserted elements must be copy-constructed
//      // (requiring memory allocation).
//      //
//      // First, move the valid elements from '[ start, divider )' to
//      // '[ divider, finish )'.  This can be done without risk of a throw
//      // occurring.
//
//      std::memcpy(divider, start, (divider - start) * sizeof(TestType));
//
//      bslalg::AutoArrayMoveDestructor<TestType> guard(start,
//                                                      divider,
//                                                      divider,
//                                                      finish);
//
//      while (guard.middle() < guard.end()) {
//          // Test some invariants:
//
//          assert(guard.destination() >= start);
//          assert(guard.destination() <  divider);
//          assert(guard.begin()       == divider);
//          assert(guard.middle()      >= divider);
//          assert(guard.middle()      <  finish);
//          assert(guard.end()         == finish);
//
//          // Call the copy c'tor, which may throw.
//
//          new (guard.destination()) TestType(value, allocator);
//
//          // 'guard.advance()' increments 'guard.destination()' and
//          // 'guard.middle()' by one.
//
//          guard.advance();
//      }
//
//      // 'guard.middle() == guard.end()' -- which means that, when 'guard' is
//      // destroyed, its destructor will do nothing.
//
//      assert(guard.middle()      == guard.end());
//      assert(guard.destination() == guard.begin());
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES
#include <bslalg_arraydestructionprimitives.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>        // std::size_t
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>        // memset, memcpy, memmove
#define INCLUDED_CSTRING
#endif

namespace BloombergLP {

namespace bslalg {

                    // =============================
                    // class AutoArrayMoveDestructor
                    // =============================

template <class OBJECT_TYPE>
class AutoArrayMoveDestructor {
    // This 'class' provides a specialized proctor object that, upon
    // destruction and unless the 'release' method has been called, bit-wise
    // moves the elements in a segment of an array of parameterized
    // 'OBJECT_TYPE' back to some destination, and destroys some other elements
    // in an adjacent segment of the same array.  The elements destroyed are
    // delimited by the range '[ begin(), middle() )' and those moved to
    // 'destination()' and in the range '[ middle(), end() )'.  Note that, once
    // constructed, 'begin()' and 'end()' remain fixed.  As the guard advances,
    // 'middle()' and 'destination()' move, reflecting the successful transfer
    // of data between the moving range and the destination.

    // DATA
    OBJECT_TYPE *d_dst_p;    // destination of the bit-wise move

    OBJECT_TYPE *d_begin_p;  // address of first element in guarded range

    OBJECT_TYPE *d_middle_p; // address of first moved element in guarded range
                             // which is also first address beyond last element
                             // destroyed in same guarded range

    OBJECT_TYPE *d_end_p;    // first address beyond last (moved) element in
                             // guarded range

    // CLASS INVARIANT
    BSLMF_ASSERT(bslmf::IsBitwiseMoveable<OBJECT_TYPE>::value);

  private:
    // NOT IMPLEMENTED
    AutoArrayMoveDestructor(const AutoArrayMoveDestructor&);
    AutoArrayMoveDestructor& operator=(const AutoArrayMoveDestructor&);

  public:
    // CREATORS
    AutoArrayMoveDestructor(OBJECT_TYPE *destination,
                            OBJECT_TYPE *begin,
                            OBJECT_TYPE *middle,
                            OBJECT_TYPE *end);
        // Create a proctor for the sequence of elements of the parameterized
        // 'OBJECT_TYPE' in the specified range '[ begin, end )' which, upon
        // destruction, moves the range '[ begin, middle )' to the specified
        // 'destination' and destroys the '[ middle, end )' range.  The
        // behavior is undefined unless 'begin <= middle <= end', either
        // 'destination < begin' or 'end <= destination', and each element in
        // the range '[ begin, end )' has been initialized.

    ~AutoArrayMoveDestructor();
        // Bit-wise move the range '[ middle(), end() )' to the 'destination()'
        // address and destroy '[ begin(), middle() )'.

    // MANIPULATORS
    void advance();
        // Increment both middle and destination pointers by one position.  The
        // behavior is undefined if this operation result in 'destination()'
        // entering the '[ begin(), end() )' range.

    // ACCESSORS
    OBJECT_TYPE *begin() const;
        // Return the address at the beginning of the guarded range.

    OBJECT_TYPE *destination() const;
        // Return the destination address, to which the second portion of the
        // guarded range, delimited by '[ middle(), end() )', will be moved
        // upon destruction, or 0 if this guard has been released.

    OBJECT_TYPE *end() const;
        // Return the address at the end of the guarded range.

    OBJECT_TYPE *middle() const;
        // Return the address at the middle of the guarded range.
};

// ===========================================================================
//                         INLINE FUNCTION DEFINITIONS
// ===========================================================================

                    // -----------------------------
                    // class AutoArrayMoveDestructor
                    // -----------------------------

// CREATORS
template <class OBJECT_TYPE>
inline
AutoArrayMoveDestructor<OBJECT_TYPE>::AutoArrayMoveDestructor(
                                                      OBJECT_TYPE *destination,
                                                      OBJECT_TYPE *begin,
                                                      OBJECT_TYPE *middle,
                                                      OBJECT_TYPE *end)
: d_dst_p(destination)
, d_begin_p(begin)
, d_middle_p(middle)
, d_end_p(end)
{
    BSLS_ASSERT_SAFE(!begin  == !middle);  // neither or both are null
    BSLS_ASSERT_SAFE(!middle == !end);     // neither or both are null
    BSLS_ASSERT_SAFE(destination || begin == middle);
    BSLS_ASSERT_SAFE(begin  <= middle);
    BSLS_ASSERT_SAFE(middle <= end);

    typedef const char CChar;
    std::size_t numBytes = (CChar *) d_end_p - (CChar *) d_middle_p;
    BSLS_ASSERT_SAFE((CChar *) destination + numBytes <= (CChar *) begin ||
                                       (CChar *) end <= (CChar *) destination);
}

template <class OBJECT_TYPE>
AutoArrayMoveDestructor<OBJECT_TYPE>::~AutoArrayMoveDestructor()
{
    BSLS_ASSERT_SAFE(!d_begin_p  == !d_middle_p);  // neither or both are null
    BSLS_ASSERT_SAFE(!d_middle_p == !d_end_p);     // neither or both are null
    BSLS_ASSERT_SAFE(d_dst_p || d_begin_p == d_middle_p);
    BSLS_ASSERT_SAFE(d_begin_p  <= d_middle_p);
    BSLS_ASSERT_SAFE(d_middle_p <= d_end_p);
    BSLS_ASSERT_SAFE(d_dst_p    <  d_begin_p
                  || d_end_p    <= d_dst_p
                  || d_middle_p == d_end_p);

    if (d_middle_p != d_end_p) {
        std::size_t numBytes = (char *)d_end_p - (char *)d_middle_p;
        std::memcpy(d_dst_p, d_middle_p, numBytes);
        ArrayDestructionPrimitives::destroy(d_begin_p, d_middle_p);
    }
}

// MANIPULATORS
template <class OBJECT_TYPE>
inline
void AutoArrayMoveDestructor<OBJECT_TYPE>::advance()
{
    BSLS_ASSERT_SAFE(d_middle_p < d_end_p);

    ++d_middle_p;
    ++d_dst_p;

    BSLS_ASSERT_SAFE(d_dst_p > d_end_p || d_dst_p < d_begin_p ||
                              (d_middle_p == d_end_p && d_dst_p == d_begin_p));
}

// ACCESSORS
template <class OBJECT_TYPE>
inline
OBJECT_TYPE *AutoArrayMoveDestructor<OBJECT_TYPE>::begin() const
{
    return d_begin_p;
}

template <class OBJECT_TYPE>
inline
OBJECT_TYPE *AutoArrayMoveDestructor<OBJECT_TYPE>::middle() const
{
    return d_middle_p;
}

template <class OBJECT_TYPE>
inline
OBJECT_TYPE *AutoArrayMoveDestructor<OBJECT_TYPE>::end() const
{
    return d_end_p;
}

template <class OBJECT_TYPE>
inline
OBJECT_TYPE *AutoArrayMoveDestructor<OBJECT_TYPE>::destination() const
{
    return d_dst_p;
}

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslalg_AutoArrayMoveDestructor
#undef bslalg_AutoArrayMoveDestructor
#endif
#define bslalg_AutoArrayMoveDestructor bslalg::AutoArrayMoveDestructor
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
