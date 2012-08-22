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
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides a common foundation to implement the
// four 'unorderd' containers specified by the C++11 standard.  It supplies
// a hash table, which stores values in buckets determined by the supplied
// hashing policy, and iterators to walk the table returning access to said
// values.
//
// This implementation will be the 'monkey-skin' approach that minimizes the
// size of the nodes, by using the preceding element in the list when storing
// references to nodes, e.g., in iterators or buckets.  The main container is
// a singly-linked list, indexed by a vector of buckets.  The overhead is:
//   o one pointer in each node
//   o one pointer per bucket (no. buckets determined by max_load_factor)
//   o one additional (empty) sentinel node
// As we do not cache the hashed value, if any hash function throws we will
// either do nothing and allow the exception to propogate, or, if some change
// of state has already been made, clear the whole container to provide the
// basic exception guarantee.  There are similar concerns for the 'equal_to'
// predicate.
//-----------------------------------------------------------------------------
//..
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

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLISTNODE
#include <bslalg_bidirectionallistnode.h>
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

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
class HashTableBucketIterator
#ifdef BSLS_PLATFORM__OS_SOLARIS
: public std::iterator<bsl::forward_iterator_tag, VALUE_TYPE>
// On Solaris just to keep studio12-v4 happy, since algorithms takes only
// iterators inheriting from 'std::iterator'.
#endif
{
    
    // PRIVATE TYPES
    typedef typename bslmf::RemoveCvq<VALUE_TYPE>::Type NcType;
    typedef HashTableBucketIterator<NcType, DIFFERENCE_TYPE, LINK_TRANSLATOR>
                                                                        NcIter;

  public:
    // PUBLIC TYPES
    typedef VALUE_TYPE                  value_type;
    typedef DIFFERENCE_TYPE             difference_type;
    typedef value_type                 *pointer;
    typedef value_type&                 reference;
    typedef bsl::forward_iterator_tag   iterator_category;
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
    // Note that in C++11 we would use the 'inheriting constructors facility
    // using TableNodeIterator::TableNodeIterator;

    HashTableBucketIterator();
        // Create in iterator not associated with any hash table.
        // Investigate if this constructor is truly required.

    HashTableBucketIterator(const bslalg::HashTableBucket *bucket);
        // NOT explicit
        // Create a 'TableNodeIterator' object pointing to the element in the
        // node following 'node' in a 'HashTable'.

    //HashTableBucketIterator(const HashTableBucketIterator&) = default;

    //~HashTableBucketIterator() = default;

    // MANIPULATORS
    //HashTableBucketIterator& operator=(const HashTableBucketIterator&)
    //                                                               = default;

    HashTableBucketIterator& operator++();

    HashTableBucketIterator operator++(int);

    // ACCESSORS
    reference operator*()  const;
    pointer   operator->() const;

    bslalg::BidirectionalLink *node() const {
        return d_node_p;
    }
};

// FREE FUNCTIONS AND OPERATORS
template <class VALUE_TYPE1, class VALUE_TYPE2, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
bool operator==(const HashTableBucketIterator<VALUE_TYPE1, DIFFERENCE_TYPE, LINK_TRANSLATOR>& lhs,
                const HashTableBucketIterator<VALUE_TYPE2, DIFFERENCE_TYPE, LINK_TRANSLATOR>& rhs);

template <class VALUE_TYPE1, class VALUE_TYPE2, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
bool operator!=(const HashTableBucketIterator<VALUE_TYPE1, DIFFERENCE_TYPE, LINK_TRANSLATOR>& lhs,
                const HashTableBucketIterator<VALUE_TYPE2, DIFFERENCE_TYPE, LINK_TRANSLATOR>& rhs);

// INLINE FUNCTION DEFINITIONS

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::HashTableBucketIterator()
: d_node_p()
, d_bucket_p()
{}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::
                 HashTableBucketIterator(const bslalg::HashTableBucket *bucket)
: d_node_p(bucket ? bucket->first() : 0)
, d_bucket_p(bucket)
{
}

    // MANIPULATORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
void HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::advance()
{
    BSLS_ASSERT_SAFE(this->d_node_p);
    BSLS_ASSERT_SAFE(this->d_bucket_p);

    if (this->d_bucket_p->last() == this->d_node_p) {
        this->d_node_p = 0;
    }
    else {
        this->d_node_p = this->d_node_p->next();
    }
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>&
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::operator++()
{
    BSLS_ASSERT_SAFE(this->d_node_p);
    BSLS_ASSERT_SAFE(this->d_bucket_p);

    this->advance();
    return *this;
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::operator++(int)
{
    BSLS_ASSERT_SAFE(this->d_node_p);
    BSLS_ASSERT_SAFE(this->d_bucket_p);

    HashTableBucketIterator result(*this);
    this->advance();
    return result;
}

// ACCESSORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
typename HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::reference
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::operator*() const
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    typedef bslalg::BidirectionalListNode<VALUE_TYPE> NodeType;
    return static_cast<NodeType *>(this->d_node_p)->value();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
typename HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::pointer
HashTableBucketIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::operator->() const
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    typedef bslalg::BidirectionalListNode<VALUE_TYPE> NodeType;
    return bsls_Util::addressOf(
                             static_cast<NodeType *>(this->d_node_p)->value());
}

} // namespace BloombergLP::bslstl

template <class VALUE_TYPE1, class VALUE_TYPE2, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bool bslstl::operator==(const HashTableBucketIterator<VALUE_TYPE1, DIFFERENCE_TYPE, LINK_TRANSLATOR>& lhs,
                        const HashTableBucketIterator<VALUE_TYPE2, DIFFERENCE_TYPE, LINK_TRANSLATOR>& rhs)
{
    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE1, class VALUE_TYPE2, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bool bslstl::operator!=(const HashTableBucketIterator<VALUE_TYPE1, DIFFERENCE_TYPE, LINK_TRANSLATOR>& lhs,
                        const HashTableBucketIterator<VALUE_TYPE2, DIFFERENCE_TYPE, LINK_TRANSLATOR>& rhs)
{
    return lhs.node() != rhs.node();
}

} // namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
