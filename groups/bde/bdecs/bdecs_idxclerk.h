// bdecs_idxclerk.h                                                   -*-C++-*-
#ifndef INCLUDED_BDECS_IDXCLERK
#define INCLUDED_BDECS_IDXCLERK

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a manager of reusable, non-negative integer indices.
//
//@CLASSES:
//   bdecs_IdxClerkIter: sequential accessor to decommissioned indices
//       bdecs_IdxClerk: manager of reusable, non-negative integer indices
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements an efficient, value-semantic manager
// class for reusable, non-negative integer indices.  Each new instance of a
// 'bdecs_IdxClerk' will issue consecutive integers on request, beginning with
// { 0, 1, 2 ... }.  Indices that are no longer needed may be returned for
// reuse.  Existing decommissioned indices are reissued before any new ones are
// created.  Value-semantic operations such as copy construction and
// assignment, equality comparison, and streaming are also provided.  Finally,
// a 'bdecs_IdxClerkIter' is provided to enable sequential, read-only access to
// the currently decommissioned indices.  Note that the order of iteration is
// not defined.
//
///PERFORMANCE
///-----------
// The following characterizes the performance of representative operations
// using "big-oh" notation, O[f(N,M)], where the names 'N' and 'M' also refer
// to the number of respective elements in the sequence of decommissioned
// indices.
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
//     N.isInUse(index)              O[N]
//..
///Usage
///-----
// A 'bdecs_IdxClerk' is commonly used in conjunction with an array to enable
// machine-address-independent referencing.  Rather than dynamically allocating
// an object and holding its address, the object is stored in the array at the
// next position dispensed by its associated 'bdecs_IdxClerk', and that index
// becomes an identifier (Id) for the new object.  Instead of destroying an
// unneeded object, its Id is merely returned to the clerk.
//
// Care must be taken to ensure that objects "created" at reused indices (i.e.,
// indices below the current length of the array) *replace* (the value of) an
// existing object in the array while objects created at new indices (i.e.,
// indices at the current length) are *appended* to the array.
//
// For example, suppose we have a security class object.  To add and remove
// security values from a security array/clerk pair, you might write the
// following two functions:
//..
//  int addSecurity(bsl::vector<Security> *securityArray,
//                  bdecs_IdxClerk        *securityClerk,
//                  const Security&        newSecurity)
//      // Add a copy of the specified 'newSecurity' to the specified
//      // 'securityArray' at the index dispensed by the specified
//      // 'securityClerk'.  Also update the 'securityClerk', and return the id
//      // (in 'securityArray') for the newly added security.
//  {
//      BSLS_ASSERT(securityArray);
//      BSLS_ASSERT(securityClerk);
//
//      int id = securityClerk->getIndex();
//
//      if (id < securityArray->size()) {
//          (*securityArray)[id] = newSecurity;
//      }
//      else {
//          securityArray->push_back(newSecurity);
//      }
//
//      return id;
//  }
//
//  void removeSecurity(bsl::vector<Security> *securityArray,
//                      bdecs_IdxClerk        *securityClerk,
//                      int                    securityId)
//      // Remove the security object identified by the specified 'securityId'
//      // from the specified 'securityArray', and update the specified
//      // 'securityClerk' (making 'securityId' available for reuse).  The
//      // behavior is undefined unless 'securityId' refers to an active
//      // security in 'securityArray' dispensed by 'securityClerk'.
//  {
//      BSLS_ASSERT(securityArray);
//      BSLS_ASSERT(securityClerk);
//
//      BSLS_ASSERT(0                             <= securityId);
//      BSLS_ASSERT(securityClerk->nextNewIndex() >  securityId);
//      BSLS_ASSERT(securityArray->size()         >  securityId);
//
//      // Note that the 'isInUse' function (below) runs in linear time.
//
//      BSLS_ASSERT_SAFE(securityClerk->isInUse(securityId));
//
//      (*securityArray)[securityId] = Security();  // optional
//      securityClerk->putIndex(securityId);
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEX_INSTREAMFUNCTIONS
#include <bdex_instreamfunctions.h>
#endif

#ifndef INCLUDED_BDEX_OUTSTREAMFUNCTIONS
#include <bdex_outstreamfunctions.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ========================
                        // class bdecs_IdxClerkIter
                        // ========================

class bdecs_IdxClerkIter {
    // This class defines an in-core value-semantic iterator providing
    // sequential read-only access to the decommissioned indices of a
    // 'bdecs_IdxClerk'.  The order of iteration is implementation dependent.

    // DATA
    bsl::reverse_iterator<const int *> d_index_p;  // pointer to current
                                                   // decommissioned index

    // FRIENDS
    friend bool operator==(const bdecs_IdxClerkIter& lhs,
                           const bdecs_IdxClerkIter& rhs);
    friend bool operator!=(const bdecs_IdxClerkIter& lhs,
                           const bdecs_IdxClerkIter& rhs);
  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdecs_IdxClerkIter,
                                 bslalg_TypeTraitBitwiseCopyable);
    // CREATORS
    bdecs_IdxClerkIter();
        // Create an unbound iterator.

    bdecs_IdxClerkIter(const int *index);
        // Create an iterator referring to the specified integer 'index'.

    bdecs_IdxClerkIter(const bdecs_IdxClerkIter& original);
        // Create an iterator having the same value as the specified 'original'
        // iterator.

  //~bdecs_IdxClerkIter();
        // Destroy this index clerk iterator.  Note that this method is
        // generated by the compiler.

    // MANIPULATORS
    bdecs_IdxClerkIter& operator=(const bdecs_IdxClerkIter& rhs);
        // Create an iterator having the same value as the specified 'rhs'
        // iterator.

    bdecs_IdxClerkIter& operator++();
        // Increment this iterator to refer to the next index in the
        // corresponding sequence of decommissioned indices.  Return a
        // reference to this modifiable iterator.  The behavior is undefined
        // unless the current index is within the range '[ begin() .. end() )'.

    bdecs_IdxClerkIter& operator--();
        // Decrement this iterator to refer to the previous index in the
        // corresponding sequence of decommissioned indices.  Return a
        // reference to this modifiable iterator.  The behavior is undefined
        // unless the current index is within the range '( begin() .. end() ]'.

    // ACCESSORS
    int operator*() const;
        // Return the value of the integer to which this iterator currently
        // refers.  The behavior is undefined unless the iterator is within the
        // range '[ begin() .. end() )'.
};

bool operator==(const bdecs_IdxClerkIter& lhs, const bdecs_IdxClerkIter& rhs);
    // Return 'true' if 'lhs' and 'rhs' have the same value and 'false'
    // otherwise.  Two iterators have the same value if they refer to the same
    // element of the same container or if they both have the end iterator
    // value for the same container.  The behavior is undefined unless 'lhs'
    // and 'rhs' refer to the same container and are non-singular (i.e., are
    // not default-constructed or copies of singular iterators).

bool operator!=(const bdecs_IdxClerkIter& lhs, const bdecs_IdxClerkIter& rhs);
    // Return 'true' if 'lhs' and 'rhs' do not have the same value and 'false'
    // otherwise.  Two iterators do not have the same value if they do not
    // refer to the same element of the same container or if one has the end
    // iterator value of a container and the other refers to an element (not
    // the end) of the same container.  The behavior is undefined unless 'lhs'
    // and 'rhs' refer to the same container and are non-singular (i.e., are
    // not default-constructed or copies of singular iterators).

                        // ====================
                        // class bdecs_IdxClerk
                        // ====================

class bdecs_IdxClerk {
    // This class defines an efficient, value-semantic manager type for
    // reusable, non-negative integer indices.  The class invariants are that
    // the all decommissioned indices must be non-negative, less than the next
    // new index, and unique.

    // DATA
    bsl::vector<int> d_unusedStack;   // stack of decommissioned indices
    int              d_nextNewIndex;  // next unused index to be created

    // FRIENDS
    friend bool operator==(const bdecs_IdxClerk&, const bdecs_IdxClerk&);
    friend bool operator!=(const bdecs_IdxClerk&, const bdecs_IdxClerk&);

    // PRIVATE CLASS METHODS
    static bool areInvariantsPreserved(const bsl::vector<int>& unusedStack,
                                       int                     nextNewIndex);
        // Return 'true' if the class invariants of this object are preserved
        // and 'false' otherwise.  The class invariants are that all
        // decommissioned indices are non-negative, less than 'nextNewIndex()',
        // and unique.  Note that the run time of this function is proportional
        // to 'numDecommissionedIndices()', but it requires temporary space
        // that is proportional to 'nextNewIndex()'.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdecs_IdxClerk,
                                 bslalg_TypeTraitUsesBslmaAllocator);
    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    explicit bdecs_IdxClerk(bslma_Allocator *basicAllocator = 0);
        // Create a new index clerk that dispenses consecutive non-negative
        // integers beginning with { 0, 1, 2, ... }; however, indices returned
        // via 'putIndex' will be reissued before any new ones are created.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bdecs_IdxClerk(const bdecs_IdxClerk&  original,
                   bslma_Allocator       *basicAllocator = 0);
        // Create a new index clerk having the value of the specified
        // 'original' index clerk.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bdecs_IdxClerk();
        // Destroy this index clerk.

    // MANIPULATORS
    // !bdecs_IdxClerk& operator=(const bdecs_IdxClerk& rhs);
        // Assign to this index clerk the value of the specified 'rhs' index
        // clerk, and return a reference to this modifiable index clerk.  Note
        // that this method's definition is compiler generated.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is unchanged.  If an
        // invalid object is read in, the stream is marked invalid and the
        // value is unchanged.  If 'version' is not supported, 'stream' is
        // marked invalid and this object is unaltered.  Note that no version
        // is read from 'stream'.  See the 'bdex' package-level documentation
        // for more information on 'bdex' streaming of value-semantic types and
        // containers.

    int getIndex();
        // Return the next available unused integer index.  Existing
        // decommissioned indices are reissued before new ones are created.

    void putIndex(int index);
        // Return the specified 'index' to this index clerk, which indicates
        // that 'index' is no longer in use and may be reissued.  The behavior
        // is undefined if the specified 'index' has never been generated by
        // this clerk or is currently decommissioned.

    void removeAll();
        // Remove all of the indices from this index clerk.  Note that the
        // following post conditions apply:
        //..
        //  assert(0 == numCommissionedIndices());
        //  assert(0 == numDecommissionedIndices());
        //  assert(0 == nextNewIndex());
        //..

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    bdecs_IdxClerkIter begin() const;
        // Return a 'bdecs_IdxClerkIter' referring to the first index returned
        // to this 'bdecs_IdxClerk' that is currently unused, or 'end()' if
        // there are currently no decommissioned indices.

    bdecs_IdxClerkIter end() const;
        // Return a 'bdecs_IdxClerkIter' referring to an invalid index,
        // indicating the end of the sequence of decommissioned index.

    bool isInUse(int index) const;
        // Return 'true' if the specified 'index' is currently in use, and
        // 'false' otherwise.  The behavior is undefined unless '0 <= index'
        // and 'index < nextNewIndex()'.  Note that this method runs in time
        // proportional to the number of decommissioned indices.

    int numCommissionedIndices() const;
        // Return the number of indices currently in use.

    int numDecommissionedIndices() const;
        // Return the number of indices that are currently decommissioned.

    int nextNewIndex() const;
        // Return the smallest (non-negative) index that has not been issued by
        // this index clerk.  Note that this function offers the client a
        // "peek" at the next "new" index, but has no effect on the value of
        // this index clerk.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this index clerk to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level' and
        // return a reference to 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
inline
bool operator==(const bdecs_IdxClerk& lhs, const bdecs_IdxClerk& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' index clerks have the
    // same value, and 'false' otherwise.  Two 'bdecs_IdxClerk' objects have
    // the same value if they have the same 'nextNewIndex()' and would always
    // generate the same sequence of integer indices.

inline
bool operator!=(const bdecs_IdxClerk& lhs, const bdecs_IdxClerk& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' index clerks do not have
    // the same value, and 'false' otherwise.  Two 'bdecs_IdxClerk' objects do
    // not have the same value if they do not have the same 'nextNewIndex()',
    // or might generate different sequences of integer indices.

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdecs_IdxClerk& rhs);
    // Write the specified 'rhs' index clerk to the specified output 'stream'
    // in some single-line (human-readable) format, and return a reference to
    // the modifiable 'stream'.

// ============================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                        // ------------------------
                        // class bdecs_IdxClerkIter
                        // ------------------------

// CREATORS
inline
bdecs_IdxClerkIter::bdecs_IdxClerkIter()
: d_index_p(0)
{
}

inline
bdecs_IdxClerkIter::bdecs_IdxClerkIter(const int *index)
: d_index_p(index)
{
}

inline
bdecs_IdxClerkIter::bdecs_IdxClerkIter(const bdecs_IdxClerkIter& original)
: d_index_p(original.d_index_p)
{
}

// MANIPULATORS
inline
bdecs_IdxClerkIter&
bdecs_IdxClerkIter::operator=(const bdecs_IdxClerkIter& rhs)
{
    d_index_p = rhs.d_index_p;
    return *this;
}

inline
bdecs_IdxClerkIter& bdecs_IdxClerkIter::operator++()
{
    BSLS_ASSERT_SAFE(0 != d_index_p.base());

    ++d_index_p;
    return *this;
}

inline
bdecs_IdxClerkIter& bdecs_IdxClerkIter::operator--()
{
    BSLS_ASSERT_SAFE(0 != d_index_p.base());

    --d_index_p;
    return *this;
}

// ACCESSORS
inline
int bdecs_IdxClerkIter::operator*() const
{
    BSLS_ASSERT_SAFE(0 != d_index_p.base());

    return *d_index_p;
}

// FREE OPERATORS
inline
bool operator==(const bdecs_IdxClerkIter& lhs, const bdecs_IdxClerkIter& rhs)
{
    return lhs.d_index_p == rhs.d_index_p;
}

inline
bool operator!=(const bdecs_IdxClerkIter& lhs, const bdecs_IdxClerkIter& rhs)
{
    return lhs.d_index_p != rhs.d_index_p;
}

                        // --------------------
                        // class bdecs_IdxClerk
                        // --------------------

// CREATORS
inline
bdecs_IdxClerk::bdecs_IdxClerk(bslma_Allocator *basicAllocator)
: d_unusedStack(basicAllocator)
, d_nextNewIndex(0)
{
}

inline
bdecs_IdxClerk::bdecs_IdxClerk(const bdecs_IdxClerk&  original,
                               bslma_Allocator       *basicAllocator)
: d_unusedStack(original.d_unusedStack, basicAllocator)
, d_nextNewIndex(original.d_nextNewIndex)
{
}

inline
bdecs_IdxClerk::~bdecs_IdxClerk()
{
    BSLS_ASSERT_SAFE(areInvariantsPreserved(d_unusedStack, d_nextNewIndex));
}

// MANIPULATORS
inline
int bdecs_IdxClerk::getIndex()
{
    if (d_unusedStack.empty()) {
        return d_nextNewIndex++;
    }
    else {
        int index = d_unusedStack.back();
        d_unusedStack.pop_back();
        return index;
    }
}

inline
void bdecs_IdxClerk::putIndex(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < d_nextNewIndex);
    BSLS_ASSERT_SAFE(isInUse(index));

    d_unusedStack.push_back(index);
}

inline
void bdecs_IdxClerk::removeAll()
{
    d_unusedStack.clear();
    d_nextNewIndex = 0;
}

// Note: Order changed from declaration to make use of inlined 'removeAll'.

template <class STREAM>
STREAM& bdecs_IdxClerk::bdexStreamIn(STREAM& stream, int version)
{
    switch (version) {
      case 1: {
        int nextNewIndex;
        stream.getInt32(nextNewIndex);

        if (!stream || nextNewIndex < 0) {
            stream.invalidate();
            return stream;                                           // RETURN
        }

        bsl::vector<int> unusedStack;
        bdex_InStreamFunctions::streamIn(stream, unusedStack, version);

        // Stream can be invalidated after streaming in 'd_unusedStack'.

        if (!stream || !areInvariantsPreserved(unusedStack, nextNewIndex)) {
            stream.invalidate();
            return stream;                                            // RETURN
        }

        d_unusedStack  = unusedStack;
        d_nextNewIndex = nextNewIndex;
      } break;
      default: {
        stream.invalidate();
      } break;
    }
    return stream;
}

// ACCESSORS
template <class STREAM>
inline
STREAM& bdecs_IdxClerk::bdexStreamOut(STREAM& stream, int version) const
{
    stream.putInt32(d_nextNewIndex);
    bdex_OutStreamFunctions::streamOut(stream, d_unusedStack, version);
    return stream;
}

inline
int bdecs_IdxClerk::numCommissionedIndices() const
{
    return d_nextNewIndex - static_cast<int>(d_unusedStack.size());
}

inline
bdecs_IdxClerkIter bdecs_IdxClerk::begin() const
{
    return bdecs_IdxClerkIter(d_unusedStack.begin() + d_unusedStack.size());
}

inline
bdecs_IdxClerkIter bdecs_IdxClerk::end() const
{
    return bdecs_IdxClerkIter(d_unusedStack.begin());
}

inline
int bdecs_IdxClerk::numDecommissionedIndices() const
{
    return static_cast<int>(d_unusedStack.size());
}

inline
int bdecs_IdxClerk::nextNewIndex() const
{
    return d_nextNewIndex;
}

inline
int bdecs_IdxClerk::maxSupportedBdexVersion()
{
    return 1;
}

// FREE OPERATORS
inline
bool operator==(const bdecs_IdxClerk& lhs, const bdecs_IdxClerk& rhs)
{
    return lhs.d_nextNewIndex == rhs.d_nextNewIndex
        && lhs.d_unusedStack  == rhs.d_unusedStack;
}

inline
bool operator!=(const bdecs_IdxClerk& lhs, const bdecs_IdxClerk& rhs)
{
    return lhs.d_nextNewIndex != rhs.d_nextNewIndex
        || lhs.d_unusedStack  != rhs.d_unusedStack;
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdecs_IdxClerk& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
