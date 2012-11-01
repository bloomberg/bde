// bslstl_hashtablebucketiterator.h                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASHTABLEBUCKETITERATOR
#define INCLUDED_BSLSTL_HASHTABLEBUCKETITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL compliant iterator over hash table buckets.
//
//@CLASSES:
// bslstl::HashBucketIterator: iterator to walk a hash-table bucket
//
//@SEE_ALSO: bslstl_unorderedmultimap, bslstl_unorderedmultiset
//
//@AUTHOR: Alisdair Meredith (ameredith1), Stefano Pacifico (spacifico1)
//
//@DESCRIPTION: This component provides a common foundation to implement the
// four 'unordered' containers specified by the C++11 standard.  It supplies
// a hash table, which stores values in buckets determined by the supplied
// hashing policy, and iterators to walk the table returning access to said
// values.
//
// This implementation will be the 'monkey-skin' approach that minimizes the
// size of the nodes, by using the preceding element in the list when storing
// references to nodes, e.g., in iterators or buckets.  The main container is
// a singly-linked list, indexed by a vector of buckets.  The overhead is:
//   o one pointer in each node
//   o one pointer per bucket (no.  buckets determined by max_load_factor)
//   o one additional (empty) sentinel node
// As we do not cache the hashed value, if any hash function throws we will
// either do nothing and allow the exception to propagate, or, if some change
// of state has already been made, clear the whole container to provide the
// basic exception guarantee.  There are similar concerns for the 'equal_to'
// predicate.
//
///Usage
///-----

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_hashtablebucketiterator.h> header can't be included directly \
in BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLINK
#include <bslalg_bidirectionallink.h>
#endif

#ifndef INCLUDED_BSLALG_BIDIRECTIONALNODE
#include <bslalg_bidirectionalnode.h>
#endif

#ifndef INCLUDED_BSLALG_HASHTABLEBUCKET
#include <bslalg_hashtablebucket.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP
{
namespace bslstl
{

                          // =============================
                          // class HashTableBucketIterator
                          // =============================

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
class HashTableBucketIterator
#ifdef BSLS_PLATFORM__OS_SOLARIS
: public native_std::iterator<native_std::forward_iterator_tag, VALUE_TYPE>
// On Solaris just to keep studio12-v4 happy, since algorithms takes only
// iterators inheriting from 'std::iterator'.
#endif
{

    // PRIVATE TYPES
    typedef typename bslmf::RemoveCvq<VALUE_TYPE>::Type NcType;
    typedef HashTableBucketIterator<NcType, DIFFERENCE_TYPE>
                                                                        NcIter;

  public:
    // PUBLIC TYPES
    typedef NcType                            value_type;
    typedef DIFFERENCE_TYPE                   difference_type;
    typedef VALUE_TYPE                       *pointer;
    typedef VALUE_TYPE&                       reference;
    typedef native_std::forward_iterator_tag  iterator_category;
        // Standard iterator defined types [24.4.2].

  private:
    // DATA
    bslalg::BidirectionalLink     *d_node_p;
    const bslalg::HashTableBucket *d_bucket_p;

  private:
    void advance();
        // Advance to the next element.

  public:
    // CREATORS
    HashTableBucketIterator();
        // Create an iterator object with the default value for this type.  All
        // default-constructed 'HashTableBucketIterator' objects represent
        // non-dereferenceable iterators into the same empty range.  They do
        // not have a singular value.

    explicit HashTableBucketIterator(const bslalg::HashTableBucket *bucket);
        // Create an iterator referring to the specified 'bucket', initially
        // pointing to the first node in that 'bucket', or a past-the-end value
        // if the 'bucket' is empty.  Note that this constructor is an
        // implementation detail and is not part of the C++ standard.

    explicit HashTableBucketIterator(bslalg::BidirectionalLink     *node,
                                     const bslalg::HashTableBucket *bucket);
        // Create an iterator referring to the specified 'bucket', initially
        // pointing to the specified 'node' in that bucket.  The behavior is
        // undefined unless 'node' is part of 'bucket', or 'node' is 0.  Note
        // that this constructor is an implementation detail and is not part of
        // the C++ standard.


    HashTableBucketIterator(const NcIter& original);
        // Create an iterator having the same value as the specified
        // 'original'.  Note that this operation is either defined by the
        // constructor taking 'NcIter' (if 'NcType' is the same as
        // 'VALUE_TYPE'), or generated automatically by the compiler.  Also
        // note that this constructor cannot be defined explicitly (without
        // using 'bsls::enableif') to avoid a duplicate declaration when
        // 'NcType' is the same as 'VALUE_TYPE'.

    //~HashTableBucketIterator() = default;

    // MANIPULATORS
    //HashTableBucketIterator& operator=(const HashTableBucketIterator&)
    //                                                               = default;

    HashTableBucketIterator& operator++();
        // Move this iterator to the next element in the hash table bucket and
        // return a reference providing modifiable access to this iterator.
        // The behavior is undefined unless the iterator refers to a valid (not
        // yet erased) node a hash table bucket.  Note that
        // 'HashTableBucketIterator's are invalidated when the underlying hash
        // table is rehashed.

    HashTableBucketIterator operator++(int);
        // Move this iterator to the next element in the hash table bucket and
        // return value of this iterator prior to this call.  The behavior is
        // undefined unless the iterator refers to a valid (not yet erased)
        // node a hash table bucket.  Note that 'HashTableBucketIterator's are
        // invalidated when the underlying hash table is rehashed.

    // ACCESSORS
    reference operator*()  const;
        // Return a reference providing modifiable access to the value (of the
        // parameterized 'VALUE_TYPE') of the element at which this iterator
        // is positioned.  The behavior is undefined unless the iterator refers
        // to a valid (not yet erased) node a hash table.    Note that
        // 'HashTableBucketIterator's are invalidated when the underlying hash
        // table is rehashed.

    pointer operator->() const;
        // Return the address of the value (of the parameterized 'VALUE_TYPE')
        // of the element at which this iterator is positioned.  The behavior
        // is undefined unless the iterator refers to a valid (not yet erased)
        // node a hash table.  Note that 'HashTableBucketIterator's are
        // invalidated when the underlying hash table is rehashed.

    bslalg::BidirectionalLink *node() const;
        // Return the address of the list-node at which this iterator is
        // positioned, or 0 if this iterator is positioned after the end of a
        // bucket.  Note that this method is an implementation detail and is
        // not part of the C++ standard.

    const bslalg::HashTableBucket *bucket() const;
        // Return the address of the list-node at which this iterator is
        // positioned, or 0 if this iterator is positioned after the end of a
        // bucket.  Note that this method is an implementation detail intended
        // for debugging purposes only, and is not part of the C++ standard.
};

// FREE FUNCTIONS AND OPERATORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator==(
              const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
              const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator==(
        const HashTableBucketIterator<      VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator==(
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
        const HashTableBucketIterator<      VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator==(
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // have the same value and 'false' otherwise.  Two iterators have
    // the same value if they refer to the same node in the same hash table, or
    // if both iterators are positioned after the end of a hash table bucket.

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator!=(
              const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
              const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator!=(
        const HashTableBucketIterator<      VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator!=(
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
        const HashTableBucketIterator<      VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator!=(
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // do not have the same value and 'false' otherwise.  Two iterators do not
    // have the same value if they refer to the different nodes in the same
    // hash table, or if either (but not both) of the iterators are positioned
    // after the end of a hash table bucket.


// INLINE FUNCTION DEFINITIONS

//CREATORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
HashTableBucketIterator()
: d_node_p()
, d_bucket_p()
{
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
HashTableBucketIterator(const bslalg::HashTableBucket *bucket)
: d_node_p(bucket ? bucket->first() : 0)
, d_bucket_p(bucket)
{
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
HashTableBucketIterator(bslalg::BidirectionalLink     *node,
                        const bslalg::HashTableBucket *bucket)
: d_node_p(node)
, d_bucket_p(bucket)
{
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
HashTableBucketIterator(const NcIter& original)
: d_node_p(original.node())
, d_bucket_p(original.bucket())
{
}

// MANIPULATORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
void
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
advance()
{
    BSLS_ASSERT_SAFE(this->d_node_p);
    BSLS_ASSERT_SAFE(this->d_bucket_p);

    if (this->d_bucket_p->last() == this->d_node_p) {
        this->d_node_p = 0;
    }
    else {
        this->d_node_p = this->d_node_p->nextLink();
    }
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>&
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
operator++()
{
    BSLS_ASSERT_SAFE(this->d_node_p);
    BSLS_ASSERT_SAFE(this->d_bucket_p);

    this->advance();
    return *this;
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
operator++(int)
{
    BSLS_ASSERT_SAFE(this->d_node_p);
    BSLS_ASSERT_SAFE(this->d_bucket_p);

    HashTableBucketIterator result(*this);
    this->advance();
    return result;
}

// ACCESSORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
typename
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::reference
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
operator*() const
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    return static_cast<bslalg::BidirectionalNode<VALUE_TYPE> *>(
                                                            d_node_p)->value();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
typename
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::pointer
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
operator->() const
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    return bsls::Util::addressOf(
            static_cast<bslalg::BidirectionalNode<VALUE_TYPE> *>(
                                                           d_node_p)->value());
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bslalg::BidirectionalLink *
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::node() const
{
    return this->d_node_p;
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
const bslalg::HashTableBucket *
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE>::bucket() const
{
    return this->d_bucket_p;
}

} // namespace BloombergLP::bslstl

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool bslstl::operator==(
              const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE >& lhs,
              const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE >& rhs)
{
    BSLS_ASSERT_SAFE(lhs.bucket() == rhs.bucket() );

    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool bslstl::operator==(
        const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE >&       lhs,
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE >& rhs)
{
    BSLS_ASSERT_SAFE(lhs.bucket() == rhs.bucket() );

    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool bslstl::operator==(
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE >& lhs,
        const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE >&       rhs)
{
    BSLS_ASSERT_SAFE(lhs.bucket() == rhs.bucket() );

    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool bslstl::operator==(
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE >& lhs,
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE >& rhs)
{
    BSLS_ASSERT_SAFE(lhs.bucket() == rhs.bucket() );

    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool bslstl::operator!=(
              const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE >& lhs,
              const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE >& rhs)
{
    BSLS_ASSERT_SAFE(lhs.bucket() == rhs.bucket() );

    return lhs.node() != rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool bslstl::operator!=(
        const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE >&       lhs,
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE >& rhs)
{
    BSLS_ASSERT_SAFE(lhs.bucket() == rhs.bucket() );

    return lhs.node() != rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool bslstl::operator!=(
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE >& lhs,
        const HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE >&       rhs)
{
    BSLS_ASSERT_SAFE(lhs.bucket() == rhs.bucket() );

    return lhs.node() != rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool bslstl::operator!=(
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE >& lhs,
        const HashTableBucketIterator<const VALUE_TYPE, DIFFERENCE_TYPE >& rhs)
{
    BSLS_ASSERT_SAFE(lhs.bucket() == rhs.bucket() );

    return lhs.node() != rhs.node();
}

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
