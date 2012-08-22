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
// the elements of a hash table.  The requirements of a STL forward iterator
// are outlined in the C++11 standard in section [24.2.5] under the tag
// [forward.iterators].  A 'HashTableIterator' object is parameterized on
// 'VALUE_TYPE', 'LINK_TRANSLATOR', and 'DIFFERENCE_TYPE'.  The parameterized
// 'VALUE_TYPE' indicates the type of the value to which this iterator provides
// as references, and may be const-qualified for constant iterators.  The
// parameterized 'LINK_TRANSLATOR' shall provide a static 'extractValue' member
// function returning a reference values stored through
// 'bslstl::BidirectionalLink *' pointers to nodes in the hash table data
// structure.  Note that 'VALUE_TYPE' may have additional const-qualification
// over the type of reference returned by the 'LINK_TRANSLATOR', in order to
// support constant iterators.  Finally, the parameterized 'DIFFERENCE_TYPE'
// determines the (standard mandated) 'difference_type' for the iterator.
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----

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

                          // =======================
                          // class HashTableIterator
                          // =======================

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
class HashTableIterator
#ifdef BSLS_PLATFORM__OS_SOLARIS
: public std::iterator<bsl::forward_iterator_tag, VALUE_TYPE>
// On Solaris just to keep studio12-v4 happy, since algorithms takes only
// iterators inheriting from 'std::iterator'.
#endif
{
    // This class provides an in-core value semantic type that is an
    // STL-conforming forward iterator over a list of
    // 'bslalg::BidirectionalLink' objects (see section 24.2.5
    // [forward.iterators] of the C++11 standard).  A 'HashTableIterator'
    // provides access to values of the parameterized 'VALUE_TYPE', 
    // stored in a hash table composed of 'bslalg::BidirectionalLink' nodes
    // additional storing objects of the parameterized 'VALUE_TYPE' which are
    // made accessible through the 'extractValue' static member function of the
    // parameterized 'LINK_TRANSLATOR' class.  The parameterized
    // 'DIFFERENCE_TYPE' determines the standard mandated 'difference_type' of
    // the iterator, without requiring access to the allocator-traits for the
    // node.  The parameterized 'LINK_TRANSLATOR' must provide a static method,
    // 'extractValue' that takes a single 'bslalg::BidirectionalLink *'
    // argument, and that returns a reference providing modifiable access to a
    // type that is convertible to the parameterized 'VALUE_TYPE'.

    // PRIVATE TYPES
    typedef typename bslmf::RemoveCvq<VALUE_TYPE>::Type NcType;
    typedef HashTableIterator<NcType, DIFFERENCE_TYPE, LINK_TRANSLATOR> NcIter;

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
        // Move this iterator to the next element in the hash table and return
        // a reference providing modifiable access to this iterator.  The
        // behavior is undefined unless the iterator refers to a valid (not yet
        // erased) node a hash table.

    HashTableIterator operator++(int);
        // Move this iterator to the next element in the hash table and return
        // value of this iterator prior to this call.  The behavior is
        // undefined unless the iterator refers to a valid (not yet erased)
        // node a hash table.

    // ACCESSORS
    reference operator*() const;
        // Return a reference providing modifiable access to the value (of the
        // parameterized 'VALUE_TYPE') of the element at which this iterator
        // is positioned.  The behavior is undefined unless the iterator refers
        // to a valid (not yet erased) node a hash table.

    pointer operator->() const;
        // Return the address of the value (of the parameterized 'VALUE_TYPE')
        // of the element at which this iterator is positioned.  The behavior
        // is undefined unless the iterator refers to a valid (not yet erased)
        // node a hash table.

    bslalg::BidirectionalLink *node() const;
        // Return the address of the list-node at which this iterator is
        // positioned, or 0 if this iterator is positioned after the end of a
        // list.  Note that this method is an implementation detail and is not
        // part of the C++ standard.
};

// FREE FUNCTIONS AND OPERATORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
bool operator==(const HashTableIterator<VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& lhs,
                const HashTableIterator<VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
bool operator==(const HashTableIterator<VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& lhs,
                const HashTableIterator<const VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
bool operator==(const HashTableIterator<const VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& lhs,
                const HashTableIterator<VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
bool operator==(const HashTableIterator<const VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& lhs,
                const HashTableIterator<const VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // have the same value and 'false' otherwise.  Two iterators have
    // the same value if they refer to the same node in the same hash table, or
    // if both iterators are positioned after the end of a hash table.

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
bool operator!=(const HashTableIterator<VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& lhs,
                const HashTableIterator<VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
bool operator!=(const HashTableIterator<VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& lhs,
                const HashTableIterator<const VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
bool operator!=(const HashTableIterator<const VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& lhs,
                const HashTableIterator<VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& rhs);
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
bool operator!=(const HashTableIterator<const VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& lhs,
                const HashTableIterator<const VALUE_TYPE,
                                        DIFFERENCE_TYPE,
                                        LINK_TRANSLATOR>& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // do not have the same value and 'false' otherwise.  Two iterators do not
    // have the same value if they refer to the different nodes in the same
    // hash table, or if either (but not both) of the iterators are positioned
    // after the end of a hash table.

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                     // -----------------------
                     // class HashTableIterator
                     // -----------------------

// CREATORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::
HashTableIterator()
: d_node_p()
{
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::
HashTableIterator(bslalg::BidirectionalLink *node)
: d_node_p(node)
{
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::
HashTableIterator(const NcIter& original)
: d_node_p(original.node())
{
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
void HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::advance()
{
    BSLS_ASSERT_SAFE(d_node_p);

    this->d_node_p = this->d_node_p->next();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>&
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::operator++()
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    this->advance();
    return *this;
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::
operator++(int)
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    HashTableIterator result(*this);
    this->advance();
    return result;
}

// ACCESSORS
template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
typename
     HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::reference
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::
operator*() const
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    return LINK_TRANSLATOR::extractValue(this->d_node_p);
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
typename
       HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::pointer
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::
operator->() const
{
    BSLS_ASSERT_SAFE(this->d_node_p);

    return bsls_Util::addressOf(LINK_TRANSLATOR::extractValue(this->d_node_p));
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bslalg::BidirectionalLink *
HashTableIterator<VALUE_TYPE, DIFFERENCE_TYPE, LINK_TRANSLATOR>::node() const
{
    return d_node_p;
}

} // namespace BloombergLP::bslstl

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bool bslstl::operator==(const HashTableIterator<VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& lhs,
                        const HashTableIterator<VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& rhs)
{
    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bool bslstl::operator==(const HashTableIterator<VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& lhs,
                        const HashTableIterator<const VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& rhs)
{
    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bool bslstl::operator==(const HashTableIterator<const VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& lhs,
                        const HashTableIterator<VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& rhs)
{
    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bool bslstl::operator==(const HashTableIterator<const VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& lhs,
                        const HashTableIterator<const VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& rhs)
{
    return lhs.node() == rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bool bslstl::operator!=(const HashTableIterator<VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& lhs,
                        const HashTableIterator<VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& rhs)
{
    return lhs.node() != rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bool bslstl::operator!=(const HashTableIterator<VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& lhs,
                        const HashTableIterator<const VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& rhs)
{
    return lhs.node() != rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bool bslstl::operator!=(const HashTableIterator<const VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& lhs,
                        const HashTableIterator<VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& rhs)
{
    return lhs.node() != rhs.node();
}

template <class VALUE_TYPE, class DIFFERENCE_TYPE, class LINK_TRANSLATOR>
inline
bool bslstl::operator!=(const HashTableIterator<const VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& lhs,
                        const HashTableIterator<const VALUE_TYPE,
                                                DIFFERENCE_TYPE,
                                                LINK_TRANSLATOR>& rhs)
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
