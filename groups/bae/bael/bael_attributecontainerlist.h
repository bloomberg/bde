// bael_attributecontainerlist.h                  -*-C++-*-
#ifndef INCLUDED_BAEL_ATTRIBUTECONTAINERLIST
#define INCLUDED_BAEL_ATTRIBUTECONTAINERLIST

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a list of attribute container addresses.
//
//@CLASSES:
//          bael_AttributeContainerList: a list of container addresses
//  bael_AttributeContainerListIterator: an iterator over a container list
//
//@SEE_ALSO: bael_attribute, bael_attributecontainer
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component defines a class 'bael_AttributeContainerList'
// that provides a linked list of 'bael_AttributeContainer' object *addresses*.
// Addresses can be prepended (to the front of the list) using the
// 'pushFront()' method.  The 'pushFront()' method returns an iterator that
// can be used later to efficiently remove the added element.  The
// 'bael_AttributeContainerList' also provides a 'hasValue()' operation, that
// returns 'true' if any of the attribute containers in the list contain the
// supplied attribute, and 'false' otherwise.  The
// 'bael_AttributeContainerList' maintains a store of free list-nodes to
// minimize the the amount of  memory allocation required if addresses are
// frequently added and removed from the container.  This component also
// defines a class 'bael_AttributeContainerListIterator'
// (as well as the alias 'bael_AttributeContainerList::iterator) that provides
// an stl-style iterator over the addresses in a 'bael_AttributeContainer'.
//
///Thread Safety
///-------------
// 'bael_AttributeContainerList' is *const* *thread-safe*, meaning that
// accessors may be invoked concurrently from different threads, but it is not
// safe to access or modify a 'bael_AttributeContainerList' in one thread
// while another thread modifies the same object.
//
///Usage
///-----
// In the following example we demonstrate how to create a
// 'bael_AttributeContainerList' object, how to add and remove elements from
// the list, and how to walk the list of attribute container addresses.
//
// We start by creating three attribute sets that we will use to populate our
// attribute container list.  Note that this example uses the 'AttributeSet'
// implementation of the 'bael_AttributeContainer' protocol defined in the
// 'bael_attributecontainer' component documentation.
//..
//  AttributeSet s1, s2, s3;
//  s1.insert(bael_AttributeValue("Set1", 1));
//  s2.insert(bael_AttributeValue("Set2", 2));
//  s3.insert(bael_AttributeValue("Set3", 3));
//..
// We now create a 'bael_AttributeContainerList' and add the three attribute
// container addresses to the list:
//..
//  bael_AttributeContainerList exampleList;
//  bael_AttributeContainerList::iterator s1Iter = exampleList.pushFront(&s1);
//  bael_AttributeContainerList::iterator s2Iter = exampleList.pushFront(&s2);
//  bael_AttributeContainerList::iterator s3Iter = exampleList.pushFront(&s3);
//..
// We can use the 'hasValue()' operation to test which attribute value are
// contained within the list of containers:
//..
//  assert(true == exampleList.hasValue("Set1", 1));
//  assert(true == exampleList.hasValue("Set2", 2));
//  assert(true == exampleList.hasValue("Set3", 3));
//
//  assert(false == exampleList.hasValue("Set1", 2));
//  assert(false == exampleList.hasValue("Set2", 1));
//  assert(false == exampleList.hasValue("Set4", 1));
//..
// We can use the iterators to efficiently remove elements from the list:
//..
//  exampleList.remove(s3Iter);
//..
// Finally we can use either the stream operator or the 'print()' method to
// print the attributes within an attribute container list:
//..
//  bsl::cout << exampleList << bsl::endl;
//..
// The resulting output will be the following:
//..
// [  [  [ Set2 = 2 ]  ]  [  [ Set1 = 1 ]  ] ]
//..
// Note that the output shows the values in 's2' (i.e., '("Set2", 2)') and
// then the values in 's1' (i.e., '("Set1", 1)').

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bslma_Allocator;
class bael_Attribute;
class bael_AttributeContainer;

struct bael_AttributeContainerList_Node;

              // =========================================
              // class bael_AttributeContainerListIterator
              // =========================================

class bael_AttributeContainerListIterator {
    // This class provides a stl-style iterator over a sequence of
    // 'bael_AttributeContainer' object addresses.  The behavior of the
    // 'operator*' method is undefined unless the iterator is at a valid
    // position in the sequence of 'bael_AttributeContainer' object addresses
    // (i.e., not the "end") and the referenced element has not been removed
    // since the iterator was constructed.

    // PRIVATE TYPES
    typedef bael_AttributeContainerList_Node Node;

    // DATA
    Node *d_node_p;  // current iterator location

    // FRIENDS
    friend bool operator==(const bael_AttributeContainerListIterator& lhs,
                           const bael_AttributeContainerListIterator& rhs);
    friend class bael_AttributeContainerList;

  public:

    // CREATORS
    bael_AttributeContainerListIterator();
        // Create an uninitialized iterator.

    bael_AttributeContainerListIterator(
                          const bael_AttributeContainerListIterator& original);
        // Create an iterator having the same value as the specified
        // 'original' one.

    bael_AttributeContainerListIterator(
                                   bael_AttributeContainerList_Node *position);
        // Create an iterator at the specified 'position'.

    // ~bael_AttributeContainerListIterator();
        // Destroy this iterator.  Note that this trivial destructor is
        // generated by the compiler.

    // MANIPULATORS
    bael_AttributeContainerListIterator& operator=(
                               const bael_AttributeContainerListIterator& rhs);
        // Assign this iterator the value of the specified 'rhs' and return a
        // modifiable reference to this iterator.

    bael_AttributeContainerListIterator operator++(int);
        // Advance this iterator to the next attribute container in the list
        // and return the value of this iterator.  The behavior is undefined
        // unless the iterator is at a valid position in the list.

    bael_AttributeContainerListIterator operator++();
        // Advance this iterator to the next attribute container in the list
        // and return the value of the iterator prior to this method call.
        // The behavior is undefined unless the iterator is at a valid
        // position in the list.

    // ACCESSORS
    const bael_AttributeContainer *operator*() const;
        // Return the address of the non-modifiable attribute container at
        // which this iterator is positioned.  The behavior is undefined unless
        // this iterator is at a valid position in the list.

    bool valid() const;
        // Return 'true' if this iterator is at a valid position in the
        // sequence of 'bael_AttributeContainer' addresses and 'false'
        // otherwise.
};

// FREE OPERATORS
inline
bool operator==(const bael_AttributeContainerListIterator& lhs,
                const bael_AttributeContainerListIterator& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // have the same value and 'false' otherwise.  Two iterators have
    // the same value if they refer to the same position in the same list, or
    // if both iterators are at an invalid position in the list (i.e., the
    // "end" of the list, or the default constructed value).

inline
bool operator!=(const bael_AttributeContainerListIterator& lhs,
                const bael_AttributeContainerListIterator& rhs);
    // Return 'true' if the specified 'lhs' and the specified 'rhs' iterators
    // do not have the same value and 'false' otherwise.  Two iterators do not
    // have the same value if they differ in either the list to which they
    // refer or the position in the list object.


                    // =================================
                    // class bael_AttributeContainerList
                    // =================================

class bael_AttributeContainerList {
    // This class provides an in-core value-semantic list of
    // 'bael_AttributeContainer' object addresses.  Attribute container
    // addresses are added to this list using 'pushFront()', which returns an
    // iterator located at the new element.  A
    // 'bael_AttributeContainerList::iterator' object remains valid
    // until the element referred to by the iterator is removed.  Attribute
    // container addresses can be removed using either 'remove()',
    // 'removeAll()', or 'removeAllAndRelease()'.  This list object attempts
    // to minimize the number of memory allocations it requires by placing the
    // memory for elements that have  been released in a free memory store,
    // and re-using the memory when new elements are added.  The 'removeAll()'
    // removes all the elements from the list, but does not release any
    // allocated memory (placing it in the free store).  The
    // 'removeAllAndRelease()' operation removes all elements and releases all
    // allocated memory.  Note that maintaining a free store is important for
    // this component because the expectation is that elements will be both
    // added and removed frequently.

    // PRIVATE TYPES
    typedef bael_AttributeContainerList_Node Node;

    // DATA
    Node            *d_head_p;       // head of the linked list of elements
    Node            *d_free_p;       // head of the free store
    int              d_length;       // length of the list
    bslma_Allocator *d_allocator_p;  // allocator (held, not owned)

  public:
    // PUBLIC TYPES
    typedef bael_AttributeContainerListIterator iterator;
        // An iterator over this list.

    // CREATORS
    bael_AttributeContainerList(bslma_Allocator *basicAllocator = 0);
        // Create an empty container list.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    bael_AttributeContainerList(
                       const bael_AttributeContainerList&  original,
                       bslma_Allocator                    *basicAllocator = 0);
        // Create a container list having the same value as the specified
        // 'original'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~bael_AttributeContainerList();
        // Destroy this object.

    // MANIPULATORS
    bael_AttributeContainerList& operator=(
                                      const bael_AttributeContainerList& rhs);
        // Assign this container list the value of the specified 'rhs' one, and
        // return a reference to this list.

    iterator pushFront(const bael_AttributeContainer *container);
        // Prepend the address of the specified 'container' to this list of
        // attribute container addresses, and return an iterator located at
        // the newly added list element.

    void remove(const iterator& element);
        // Remove the specified 'element' from this list.  The behavior is
        // undefined unless 'element' is a valid iterator on this list and the
        // referenced address has not previously been removed (by either
        // 'remove()', 'removeAll()', or 'removeAllAndRelease()'.

    void removeAll();
        // Remove all the elements from this list.  After this operation
        // returns, 'numContainers()' will be 0.  This operation adds all the
        // allocated memory to an internal free store, and does not deallocate
        // any memory.

    void removeAllAndRelease();
        // Remove all the elements from this list and deallocate any allocated
        // memory.

    // ACCESSORS
    iterator begin() const;
        // Return an iterator positioned at the beginning of the list of
        // 'bael_AttributeContainer' object addresses represented by this
        // object.

    iterator end() const;
        // Return an iterator positioned one past the final
        // 'bael_AttributeContainer' object address in the list of addresses
        // represented by this object.

    int numContainers() const;
        // Return the number of attribute container addresses currently in
        // this list.

    bool hasValue(const bael_Attribute& value) const;
        // Return 'true' if the attribute having specified 'value' exists in
        // any of the attribute containers referred to by this object, and
        // 'false' otherwise.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.
};

// FREE OPERATORS
bool operator==(const bael_AttributeContainerList& lhs,
                const bael_AttributeContainerList& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' lists have the same
    // value, and 'false' otherwise.  Two lists have the same value if they
    // have the same number of attribute container addresses, and the
    // address at each index position have the same value.

inline
bool operator!=(const bael_AttributeContainerList& lhs,
                const bael_AttributeContainerList& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' lists do not have the
    // same value, and 'false' otherwise.  Two lists do not have the same
    // value if have differing numbers of attribute container addresses or any
    // of the addresses at corresponding indices have different values.

inline
bsl::ostream& operator<<(bsl::ostream&                      output,
                         const bael_AttributeContainerList& rhs);
    // Write a formatted description of the specified 'rhs' to the specified
    // 'stream' and return a reference to the modifiable 'stream'.


                 // ======================================
                 // class bael_AttributeContainerList_Node
                 // ======================================

struct bael_AttributeContainerList_Node {
    // This is an implementation type of 'bael_AttributeContainerList' and
    // should not be used by clients of this package.  A
    // 'bael_AttributeContainerList_Node' represents a node in a
    // 'bael_AttributeContainerList' object.

    const bael_AttributeContainer     *d_value_p;  // address value of this
                                                   // element

    bael_AttributeContainerList_Node  *d_next_p;  // next element

    bael_AttributeContainerList_Node **d_prevNextAddr_p;
                                                  // address of previous
                                                  // element's next pointer
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

              // -----------------------------------------
              // class bael_AttributeContainerListIterator
              // -----------------------------------------

// CREATORS
inline
bael_AttributeContainerListIterator::bael_AttributeContainerListIterator()
: d_node_p(0)
{
}

inline
bael_AttributeContainerListIterator::
bael_AttributeContainerListIterator(bael_AttributeContainerList_Node *position)
: d_node_p(position)
{
}

inline
bael_AttributeContainerListIterator::
bael_AttributeContainerListIterator(
                           const bael_AttributeContainerListIterator& original)
: d_node_p(original.d_node_p)
{
}

// MANIPULATORS
inline
bael_AttributeContainerListIterator&
bael_AttributeContainerListIterator::operator=(
                               const bael_AttributeContainerListIterator& rhs)
{
    d_node_p = rhs.d_node_p;
    return *this;
}

inline
bael_AttributeContainerListIterator
bael_AttributeContainerListIterator::operator++(int)
{
    Node *current = d_node_p;
    d_node_p = d_node_p->d_next_p;
    return bael_AttributeContainerListIterator(current);
}

inline
bael_AttributeContainerListIterator
bael_AttributeContainerListIterator::operator++()
{
    d_node_p = d_node_p->d_next_p;
    return bael_AttributeContainerListIterator(d_node_p);
}

// ACCESSORS
inline
const bael_AttributeContainer *
bael_AttributeContainerListIterator::operator*() const
{
    return d_node_p->d_value_p;
}

inline
bool bael_AttributeContainerListIterator::valid() const
{
    return 0 != d_node_p;
}

                    // =================================
                    // class bael_AttributeContainerList
                    // =================================

// CREATORS
inline
bael_AttributeContainerList::bael_AttributeContainerList(
                                               bslma_Allocator *basicAllocator)
: d_head_p(0)
, d_free_p(0)
, d_length(0)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

inline
bael_AttributeContainerList::~bael_AttributeContainerList()
{
    removeAllAndRelease();
}

// ACCESSORS
inline
bael_AttributeContainerList::iterator
bael_AttributeContainerList::begin() const
{
    return iterator(d_head_p);
}

inline
bael_AttributeContainerList::iterator bael_AttributeContainerList::end() const
{
    return iterator(0);
}

inline
int bael_AttributeContainerList::numContainers() const
{
    return d_length;
}

// FREE OPERATORS
inline
bool operator==(const bael_AttributeContainerListIterator& lhs,
                const bael_AttributeContainerListIterator& rhs)
{
    return lhs.d_node_p == rhs.d_node_p;
}

inline
bool operator!=(const bael_AttributeContainerListIterator& lhs,
                const bael_AttributeContainerListIterator& rhs)
{
    return !(lhs == rhs);
}

inline
bool operator!=(const bael_AttributeContainerList& lhs,
                const bael_AttributeContainerList& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&                      output,
                         const bael_AttributeContainerList& rhs)
{
    return rhs.print(output, 0, -1);
}

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
