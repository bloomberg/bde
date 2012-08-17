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
//@AUTHOR: Alisdair Meredith (ameredith1)
//
//@DESCRIPTION: This component provides an STL-compliant foward iterator over
// the elements of a hash table 'bslstl::TreeNode' objects.  The requirements of a
// STL forward iterator are outlined in the C++11 standard in section
// [24.2.6] under the tag [forward.iterators].  A 'HashTableIterator' object
// is parameterized on 'VALUE_TYPE', 'NODE_TYPE', and 'DIFFERENCE_TYPE'.
// The parameterized 'VALUE_TYPE' indicates the type of the value to
// which this iterator provides a references, and may be const-qualified for
// constant iterators.  The parameterized 'NODE_TYPE' indicates the type of the
// node objects in this tree.  Note that 'NODE_TYPE' is not necessarily
// 'BidirectionalListNode<VALUE_TYPE>' as 'VALUE_TYPE' may be const-qualified.
// Finally, the parameterized 'DIFFERENCE_TYPE' determines the, standard required,
// 'difference_type' for the iterator.  'NODE_TYPE' must derives from
// 'bslalg::RbTreeNode', and contains a 'value' method that returns a reference
// providing modifiable access to a type that is convertible to the
// parameterized 'VALUE_TYPE' (e.g., a 'bslstl::BidirectionalListNode' object).
////-----------------------------------------------------------------------------
//..
//
///Usage
///-----

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstl_hashtableiterator.h> header can't be included directly in
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

#ifndef INCLUDED_BSLALG_BIDIRECTIONALLISTNODE
#include <bslalg_bidirectionallistnode.h>
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

#ifndef INCLUDED_CSTDDEF
#include <cstddef>  // 'ptrdiff_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP
{
namespace bslstl
{

                          // =======================
                          // class HashTableIterator
                          // =======================

template <typename VALUE_TYPE>
class HashTableIterator
#ifdef BSLS_PLATFORM__OS_SOLARIS
: public std::iterator<bsl::forward_iterator_tag, VALUE_TYPE>
// On Solaris just to keep studio12-v4 happy, since algorithms takes only
// iterators inheriting from 'std::iterator'.
#endif
{
    // This class provides an STL-conforming forward iterator over a list of
    // 'bslalg::BidirectionalListNode' objects (see section 24.2.6
    // [bidirectional.iterators] of the C++11 standard).  A
    // 'HashTableIterator' provides access to values of the parameterized
    // 'VALUE_TYPE', over a binary tree composed of nodes of the
    // parameterized 'BSLSTL_NODE' (which must derive from
    // 'bslalg::RbTreeNode').  The parameterized 'DIFFERENCE_TYPE' determines
    // the standard required 'difference_type' of the iterator, without
    // requiring access to the allocator-traits for the node.  The behavior of
    // the 'operator*' method is undefined unless the iterator is at a valid
    // position in the tree (i.e., not the 'end') and the referenced element
    // has not been removed since the iterator was constructed.  'NODE_TYPE'
    // must derives from 'bslalg::RbTreeNode', and contains a 'value' method
    // that returns a reference providing modifiable access to a type that is
    // convertible to the parameterized 'VALUE_TYPE' (e.g., a
    // 'bslalg::BidirectionalListNode' object).

    // PRIVATE TYPES
    typedef typename bslmf::RemoveCvq<VALUE_TYPE>::Type NcType;
    typedef HashTableIterator<NcType>                   NcIter;

  public:
    // PUBLIC TYPES
    typedef VALUE_TYPE                  value_type;
    typedef std::ptrdiff_t              difference_type;
    typedef value_type                 *pointer;
    typedef value_type&                 reference;
    typedef bsl::forward_iterator_tag   iterator_category;
        // Standard iterator defined types [24.4.2].

  private:
    // DATA
    bslalg::BidirectionalLink *d_node_p;

  private:
    void advance();
        // Advance to the next element.

  public:
    // CREATORS
    HashTableIterator();
        // Create an iterator object with the default value for this type.  All
        // default-constructed 'HashTableIterator' objects represent
        // non-dereferenceable iterators into the same empty range.  They do
        // not have a singular value.

    explicit HashTableIterator(bslalg::BidirectionalLink *node);
        // Create an iterator referring to the specified 'node'.  The behavior
        // is undefined unless 'node' is of the parameterized 'NODE', which
        // is derived from 'bslalg::BidirectionalLink'.  Note that this
        // constructor is an implementation detail and is not part of the C++
        // standard.

    HashTableIterator(const NcIter& original);
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
        // Copy the value of the specified 'rhs' to this iterator.  Return a
        // reference to this modifiable object.  Note that this method's
        // definition is compiler generated.

    HashTableIterator& operator++();
        // Move this iterator to the next element in the tree and return a
        // reference providing modifiable access to this iterator.  The
        // behavior is undefined unless the iterator refers to an element in
        // the tree.

    HashTableIterator operator++(int);


    // ACCESSORS
    reference operator*() const;
        // Return a reference providing modifiable access to the value (of the
        // parameterized 'BSLSTL_VALUE') of the element at which this iterator
        // is positioned.  The behavior is undefined unless this iterator is at
        // a valid position in the tree.

    pointer operator->() const;
        // Return the address of the value (of the parameterized
        // 'BSLSTL_VALUE') of the element at which this iterator is positioned.
        // The behavior is undefined unless this iterator is at a valid
        // position in the tree.


    bslalg::BidirectionalLink *node() const;
        // Return the address of the list-node at which this iterator is
        // positioned, or 0 if this iterator is positioned after the end of a
        // list.  Note that this method is an implementation detail and is not
        // part of the C++ standard.
};

// FREE FUNCTIONS AND OPERATORS
template <typename VALUE_TYPE>
bool operator==(const HashTableIterator<VALUE_TYPE>& lhs,
                const HashTableIterator<VALUE_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // have the same value and 'false' otherwise.  Two iterators have
    // the same value if they refer to the same position in the same tree, or
    // if both iterators are at an invalid position in the tree (i.e., the
    // 'end' of the tree, or the default constructed value).

template <typename VALUE_TYPE>
bool operator!=(const HashTableIterator<VALUE_TYPE>& lhs,
                const HashTableIterator<VALUE_TYPE>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // do not have the same value and 'false' otherwise.  Two iterators do not
    // have the same value if they differ in either the tree to which they
    // refer or the position in that tree.

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // -----------------------
                     // class HashTableIterator
                     // -----------------------

// CREATORS
template <typename VALUE_TYPE>
inline
HashTableIterator<VALUE_TYPE>::HashTableIterator()
: d_node_p()
{
}

template <typename VALUE_TYPE>
inline
HashTableIterator<VALUE_TYPE>::HashTableIterator(
                                               bslalg::BidirectionalLink *node)
: d_node_p(node)
{
}

template <class VALUE_TYPE>
inline
HashTableIterator<VALUE_TYPE>::HashTableIterator(const NcIter& original)
: d_node_p(original.d_node_p)
{
}

template <typename VALUE_TYPE>
inline
void HashTableIterator<VALUE_TYPE>::advance() {
    BSLS_ASSERT_SAFE(d_node_p);

    this->d_node_p = this->d_node_p->next();
}

template <typename VALUE_TYPE>
inline
HashTableIterator<VALUE_TYPE>& HashTableIterator<VALUE_TYPE>::operator++()
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    this->advance();
    return *this;
}

template <typename VALUE_TYPE>
inline
HashTableIterator<VALUE_TYPE> HashTableIterator<VALUE_TYPE>::operator++(int)
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    HashTableIterator result(*this);
    this->advance();
    return result;
}

// ACCESSORS
template <typename VALUE_TYPE>
inline
typename HashTableIterator<VALUE_TYPE>::reference
HashTableIterator<VALUE_TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    typedef bslalg::BidirectionalListNode<VALUE_TYPE> NodeType;
    return static_cast<NodeType *>(this->d_node_p)->value();
}

template <typename VALUE_TYPE>
inline
typename HashTableIterator<VALUE_TYPE>::pointer
HashTableIterator<VALUE_TYPE>::operator->() const
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    typedef bslalg::BidirectionalListNode<VALUE_TYPE> NodeType;
    return bsls_Util::addressOf(
                             static_cast<NodeType *>(this->d_node_p)->value());
}

template <typename VALUE_TYPE>
inline
bslalg::BidirectionalLink *HashTableIterator<VALUE_TYPE>::node() const
{
    return d_node_p;
}

} // namespace BloombergLP::bslstl

template <typename VALUE_TYPE>
inline
bool bslstl::operator==(const HashTableIterator<VALUE_TYPE>& lhs,
                        const HashTableIterator<VALUE_TYPE>& rhs)
{
    return lhs.node() == rhs.node();
}

template <typename VALUE_TYPE>
inline
bool bslstl::operator!=(const HashTableIterator<VALUE_TYPE>& lhs,
                        const HashTableIterator<VALUE_TYPE>& rhs)
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
