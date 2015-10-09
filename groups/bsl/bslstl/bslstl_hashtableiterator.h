// bslstl_hashtableiterator.h                                         -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASHTABLEITERATOR
#define INCLUDED_BSLSTL_HASHTABLEITERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an STL compliant iterator for hash tables.
//
//@CLASSES:
//  bslstl::HashTableIterator: an STL compliant forward iterator
//
//@SEE_ALSO: bslalg_bidirectionallink, bslstl_unorderedmap, bslstl_unorderedset
//
//@DESCRIPTION: This component provides a standard-conforming forward iterator,
// 'bslstl::HashTableIterator', over a list of elements (of type
// 'bslalg::BidirectionalLink') in a hashtable.  The requirements of a forward
// iterator are outlined in the C++11 standard in section [24.2.5] under the
// tag [forward.iterators].  The 'bslstl::HashTableIterator' class template has
// two template parameters: 'VALUE_TYPE', and 'DIFFERENCE_TYPE'.  'VALUE_TYPE'
// indicates the type of the value to which this iterator provides references,
// and may be const-qualified for constant iterators.  'DIFFERENCE_TYPE'
// determines the (standard mandated) 'difference_type' for the iterator, and
// will typically be supplied by the allocator used by the hash-table being
// iterated over.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Iterating a Hash Table Using 'HashTableIterator'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we create a simple hashtable and then use a
// 'HashTableIterator' to iterate through its elements.
//
// First, we define a typedef, 'Node', prepresenting a bidirectional node
// holding an integer value:
//..
//  typedef bslalg::BidirectionalNode<int> Node;
//..
// Then, we construct a test allocator, and we use it to allocate an array of
// 'Node' objects, each holding a unique integer value:
//..
//  bslma::TestAllocator scratch;
//
//  const int NUM_NODES = 5;
//  const int NUM_BUCKETS = 3;
//
//  Node *nodes[NUM_NODES];
//  for (int i = 0; i < NUM_NODES; ++i) {
//      nodes[i] = static_cast<Node *>(scratch.allocate(sizeof(Node)));
//      nodes[i]->value() = i;
//  }
//..
// Next, we create an array of 'HashTableBuckets' objects, and we use the array
// to construct an empty hash table characterized by a 'HashTableAnchor'
// object:
//..
//  bslalg::HashTableBucket buckets[NUM_BUCKETS];
//  for (int i = 0; i < NUM_BUCKETS; ++i) {
//      buckets[i].reset();
//  }
//  bslalg::HashTableAnchor hashTable(buckets, NUM_BUCKETS, 0);
//..
// Then, we insert each node in the array of nodes into the hash table using
// 'bslalg::HashTableImpUtil', supplying the integer value held by each node as
// its hash value:
//..
//  for (int i = 0; i < NUM_NODES; ++i) {
//      bslalg::HashTableImpUtil::insertAtFrontOfBucket(&hashTable,
//                                                      nodes[i],
//                                                      nodes[i]->value());
//  }
//..
// Next, we define a 'typedef' that is an alias an instance of
// 'HashTableIterator' that can traverse hash tables holding integer values.
//..
//  typedef bslstl::HashTableIterator<int, ptrdiff_t> Iter;
//..
// Now, we create two iterators: one pointing to the start of the bidirectional
// linked list held by the hash table, and the other representing the end
// sentinel.  We use them to navigate and print the elements of the hash table:
//..
//  Iter iter(hashTable.listRootAddress());
//  Iter end;
//  for (;iter != end; ++iter) {
//      printf("%d\n", *iter);
//  }
//..
// Then, we observe the following output:
//..
// 2
// 4
// 1
// 3
// 0
//..
// Finally, we deallocate the memory used by the hash table:
//..
//  for (int i = 0; i < NUM_NODES; ++i) {
//      scratch.deallocate(nodes[i]);
//  }
//..

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_hashtableiterator.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
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

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP
{
namespace bslstl
{

                          // =======================
                          // class HashTableIterator
                          // =======================

#ifdef BSLS_PLATFORM_OS_SOLARIS
// On Solaris just to keep studio12-v4 happy, since algorithms take only
// iterators inheriting from 'std::iterator'.

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
class HashTableIterator
: public native_std::iterator<native_std::forward_iterator_tag, VALUE_TYPE> {
#else
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
class HashTableIterator {
#endif
    // This class template implements an in-core value-semantic type that is an
    // standard-conforming forward iterator (see section 24.2.5
    // [forward.iterators] of the C++11 standard) over a list of
    // 'bslalg::BidirectionalLink' objects.  A 'HashTableIterator' object
    // provides access to values of the (template parameter) 'VALUE_TYPE',
    // stored in a hash table composed of 'bslalg::BidirectionalLink' nodes.
    // The (template parameter) 'DIFFERENCE_TYPE' determines the standard
    // mandated 'difference_type' of the iterator, without requiring access to
    // the allocator-traits for the node.

    // PRIVATE TYPES
    typedef typename bslmf::RemoveCvq<VALUE_TYPE>::Type NcType;
    typedef HashTableIterator<NcType, DIFFERENCE_TYPE>  NcIter;

  public:
    // PUBLIC TYPES
    typedef NcType                      value_type;
    typedef DIFFERENCE_TYPE             difference_type;
    typedef VALUE_TYPE                 *pointer;
    typedef VALUE_TYPE&                 reference;
    typedef bsl::forward_iterator_tag   iterator_category;
        // Standard iterator defined types [24.4.2].

  private:
    // DATA
    bslalg::BidirectionalLink *d_node_p;  // pointer to the node referred to by
                                          // this iterator

  private:
    // PRIVATE MANIPULATORS
    void advance();
        // Move this iterator to refer to the next node in the list.

  public:
    // CREATORS
    HashTableIterator();
        // Create a default-constructed iterator referring to an empty list of
        // nodes.  All default-constructed iterators are non-dereferenceable
        // and refer to the same empty list.

    explicit HashTableIterator(bslalg::BidirectionalLink *node);
        // Create an iterator referring to the specified 'node'.  The behavior
        // is undefined unless 'node' is of the type
        // 'bslalg::BidirectionalNode<VALUE_TYPE>', which is derived from
        // 'bslalg::BidirectionalLink'.  Note that this constructor is an
        // implementation detail and is not part of the C++ standard.

    HashTableIterator(const NcIter& original);                      // IMPLICIT
        // Create an iterator at the same position as the specified 'original'
        // iterator.  Note that this constructor enables converting from
        // modifiable to 'const' iterator types.

    //! TreeIterator(const TreeIterator& original) = default;
        // Create an iterator having the same value as the specified
        // 'original'.  Note that this operation is either defined by the
        // constructor taking 'NcIter' (if 'NcType' is the same as
        // 'VALUE_TYPE'), or generated automatically by the compiler.  Also
        // note that this constructor cannot be defined explicitly (without
        // using 'bsls::enableif') to avoid a duplicate declaration when
        // 'NcType' is the same as 'VALUE_TYPE'.

    //! ~HashTableIterator();
        // Destroy this object.

    // MANIPULATORS
    //! HashTableIterator& operator=(const HashTableIterator& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // a return a reference providing modifiable access to this object.

    HashTableIterator& operator++();
        // Move this iterator to the next node in the list and return a
        // reference providing modifiable access to this iterator.  The
        // behavior is undefined unless the iterator refers to a valid (not yet
        // erased) node in the list.

    // ACCESSORS
    reference operator*() const;
        // Return a reference providing modifiable access to the value (of the
        // template parameter 'VALUE_TYPE') of the node at which this iterator
        // is positioned.  The behavior is undefined unless the iterator refers
        // to a valid (not yet erased) node the list.

    pointer operator->() const;
        // Return the address of the value (of the template parameter
        // 'VALUE_TYPE') of the element at which this iterator is positioned.
        // The behavior is undefined unless the iterator refers to a valid (not
        // yet erased) node the list.

    bslalg::BidirectionalLink *node() const;
        // Return the address of the node at which this iterator is positioned
        // in the list, or 0 if this iterator is positioned after the end of a
        // list.  Note that this method is an implementation detail and is not
        // part of the C++ standard.
};

// FREE OPERATORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator==(const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
                const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator==(
              const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>&       lhs,
              const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator==(
              const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
              const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>&       rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator==(
              const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
              const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // have the same value and 'false' otherwise.  Two iterators have the same
    // value if they refer to the same node in the same list, or if both
    // iterators are at the past-the-end position of the tree.

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator!=(const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
                const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator!=(
              const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>&       lhs,
              const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator!=(
        const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
        const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>&       rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
bool operator!=(
              const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
              const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // do not have the same value and 'false' otherwise.  Two iterators do not
    // have the same value if they refer to the different nodes in the same
    // list, or if either (but not both) of the iterators are at the
    // past-the-end position of the tree.


template <class VALUE_TYPE, class DIFFERENCE_TYPE>
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>
operator++(HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>& iter, int);
    // Move the specified 'iter' to the next node in the list and return
    // value of 'iter' prior to this call.  The behavior is undefined
    // unless 'iter' refers to a valid (not yet erased) node a the list.


// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // -----------------------
                     // class HashTableIterator
                     // -----------------------

// CREATORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
HashTableIterator()
: d_node_p()
{
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
HashTableIterator(bslalg::BidirectionalLink *node)
: d_node_p(node)
{
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>::
HashTableIterator(const NcIter& original)
: d_node_p(original.node())
{
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
void HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>::advance()
{
    BSLS_ASSERT_SAFE(d_node_p);

    this->d_node_p = this->d_node_p->nextLink();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>&
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>::operator++()
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    this->advance();
    return *this;
}

// ACCESSORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
typename HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>::reference
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    return static_cast<bslalg::BidirectionalNode<VALUE_TYPE> *>(
                                                            d_node_p)->value();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
typename HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>::pointer
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>::operator->() const
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    return bsls::Util::addressOf(
            static_cast<bslalg::BidirectionalNode<VALUE_TYPE> *>(
                                                           d_node_p)->value());
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bslalg::BidirectionalLink *
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>::node() const
{
    return d_node_p;
}

// FREE OPERATORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool operator==(const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
                const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>& rhs)
{
    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool operator==(
               const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>&       lhs,
               const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs)
{
    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool operator==(
               const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
               const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>&       rhs)
{
    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool operator==(
               const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
               const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs)
{
    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool operator!=(const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
                const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>& rhs)
{
    return lhs.node() != rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool operator!=(
               const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>&       lhs,
               const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs)
{
    return lhs.node() != rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool operator!=(
               const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
               const HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>&       rhs)
{
    return lhs.node() != rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
bool operator!=(
               const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& lhs,
               const HashTableIterator<const VALUE_TYPE, DIFFERENCE_TYPE>& rhs)
{
    return lhs.node() != rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE>
inline
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>
operator++(HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE>& iter, int)
{
    BSLS_ASSERT_SAFE(iter.node());

    HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE> temp(iter);
    ++iter;
    return temp;
}

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
