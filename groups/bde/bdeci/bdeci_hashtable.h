// bdeci_hashtable.h                -*-C++-*-
#ifndef INCLUDED_BDECI_HASHTABLE
#define INCLUDED_BDECI_HASHTABLE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container of unique 'T' values.
//
//@DEPRECATED: Use 'bsl::hash_map' instead.
//
//@CLASSES:
//          bdeci_Hashtable: hashtable of 'T' values
//  bdeci_HashtableSlotIter: sequential read-only accessor of members
// bdeci_HashtableSlotManip: sequential selective remover of members
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component implements an efficient hashtable for elements
// of type 'T'.  Iterators and manipulators provide efficient access for
// accessing and filtering (i.e., selectively removing and inserting) members,
// respectively.
//
///Abstract Representation
///-----------------------
// The organization for a hashtable is shown below:
//..
//          +---------+     +---------+
//          |    o----|---->| element |
//          +---------+     +---------+
//          |    0    |     |    0    |
//          +---------+     +---------+
//          |    0    |     Link
//          +---------+
//          |    0    |
//          +---------+     +---------+     +---------+
//          |    o----|---->| element |  +->| element |
//          +---------+     +---------+  |  +---------+
//          |    0    |     | nextPtr-|--+  |    0    |
//          +---------+     +---------+     +---------+
//          |    0    |     Link            Link
//          +---------+
//          d_table_p
//..
// Each element of 'd_table_p' is referred to as a "slot" of the hashtable.
//
// The value of a hashtable is defined by its logical number of slots
// ('numSlots()'), its number of elements ('numElements()'), its elements, and
// the relative positioning of these elements.  Specifically, the order of the
// elements within a slot is part of the value of the hashtable.  Having the
// number of slots and the order within each slot being part of the value may
// not be relevant to all users of this component and hence the 'compact'
// method is provided to reduce the potential spatial inefficiency incurred by
// preserving this (elaborate) value across operations.
//
///PERFORMANCE
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, O[f(N,M)], where the names 'N' and 'M' also refer to
// the number of respective elements in each container (i.e., its
// 'numElements()').  Here the expected case, E[f(N)], is the expected cost of
// the operation assuming "normal" data.
//..
//     Operation           Worst Case          Expected Case
//     ---------           ----------          -------------
//     add                 O[1]
//     addUnique           O[N]                E[1]
//     compact             O[N]
//     removeFirst         O[N]                E[1]
//     removeEvery         O[N]
//     isMember            O[N]                E[1]
//     numOccurrences      O[N]
//     numElements         O[1]
//     numSlots            O[1]
//..
//
///HASHTABLE USAGE
///---------------
// The following snippets of code illustrate how to create and use a hashtable.
// First create an empty 'bdeci_Hashtable' 'ht' and populate it with
// elements:
//..
//      const int E1 = 100;
//      const int E2 = 200;
//      const int E3 = 300;
//      const int E4 = 400;
//      const int E5 = 500;
//
//      bdeci_Hashtable<int, bdeimp_IntHash> ht;
//                                      assert( 0 == ht.numElements());
//
//      ht.add(E1);                     assert( 1 == ht.numElements());
//                                      assert( ht.isMember(E1));
//                                      assert(!ht.isMember(E2));
//                                      assert(!ht.isMember(E3));
//                                      assert(!ht.isMember(E4));
//                                      assert(!ht.isMember(E5));
//
//      ht.add(E2);                     assert( 2 == ht.numElements());
//                                      assert( ht.isMember(E1));
//                                      assert( ht.isMember(E2));
//                                      assert(!ht.isMember(E3));
//                                      assert(!ht.isMember(E4));
//                                      assert(!ht.isMember(E5));
//
//      ht.add(E3);                     assert( 3 == ht.numElements());
//                                      assert( ht.isMember(E1));
//                                      assert( ht.isMember(E2));
//                                      assert( ht.isMember(E3));
//                                      assert(!ht.isMember(E4));
//                                      assert(!ht.isMember(E5));
//..
// Finally, remove the elements from the hashtable 'ht':
//..
//      ht.removeFirst(E1);             assert( 2 == ht.numElements());
//                                      assert(!ht.isMember(E1));
//                                      assert( ht.isMember(E2));
//                                      assert( ht.isMember(E3));
//                                      assert(!ht.isMember(E4));
//                                      assert(!ht.isMember(E5));
//
//      ht.removeFirst(E2);             assert( 1 == ht.numElements());
//                                      assert(!ht.isMember(E1));
//                                      assert(!ht.isMember(E2));
//                                      assert( ht.isMember(E3));
//                                      assert(!ht.isMember(E4));
//                                      assert(!ht.isMember(E5));
//
//      ht.removeFirst(E3);             assert( 0 == ht.numElements());
//                                      assert(!ht.isMember(E1));
//                                      assert(!ht.isMember(E2));
//                                      assert(!ht.isMember(E3));
//                                      assert(!ht.isMember(E4));
//                                      assert(!ht.isMember(E5));
//..
///ITERATOR USAGE
///--------------
// The following shows the standard iterator pattern:
//..
//      for (bdeci_HashtableSlotIter<int, bdeimp_IntHash> it(ht, 0);
//                                                                 it; ++it) {
//          bsl::cout << it() << bsl::endl; // i.e., operate on current element
//      }
//..
///MANIPULATOR USAGE
///-----------------
// The following illustrates the (less-common) manipulator pattern:
//..
//      bdeci_HashtableSlotManip<int, bdeimp_IntHash> manip(&ht, 0);
//      while (manip) {
//          if (E3 >= manip()) {   // condition to keep current element
//              manip.advance();
//          }
//          else {                 // otherwise get rid of current element
//              manip.remove();
//          }
//      }
//..
// and insertion of elements with the 'addRaw' method:
//..
//      ht.reserveCapacity(2);
//      manip.setSlot(ht.hash(E1));
//      manip.addRaw(E1);
//      manip.setSlot(ht.hash(E2));
//      manip.addRaw(E2);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

#ifndef INCLUDED_BDEU_PRINT
#include <bdeu_print.h>
#endif

#ifndef INCLUDED_BDEIMP_DUFFSDEVICE
#include <bdeimp_duffsdevice.h>
#endif

#ifndef INCLUDED_BDECI_HASHTABLEIMPUTIL
#include <bdeci_hashtableimputil.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEALLOCATORPROCTOR
#include <bslma_deallocatorproctor.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_NEW
#include <bsl_new.h>         // placement syntax
#endif


namespace BloombergLP {

template <class T, class HASH>
class bdeci_HashtableSlotIter;

template <class T, class HASH>
class bdeci_HashtableSlotManip;

class bslma_Allocator;

                        // ==========================
                        // class bdeci_Hashtable_Link
                        // ==========================

// This struct should be private to the class bdeci_Hashtable.  Due to compiler
// issues (e.g., IBM AIX xlC) it is not.  Do not use this struct outside of
// this component.

template <class T>
struct bdeci_Hashtable_Link {
    // This struct implements the storage required for a linked-list of
    // elements.
    bslalg_ConstructorProxy<T>  d_memory;
    bdeci_Hashtable_Link       *d_next_p;

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdeci_Hashtable_Link,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    bdeci_Hashtable_Link(const T&                      value,
                         bdeci_Hashtable_Link         *next,
                         bslma_Allocator              *allocator);
    T& value() { return d_memory.object(); }
};

template <class T>
inline
bdeci_Hashtable_Link<T>::
                 bdeci_Hashtable_Link(const T&                      value,
                                      bdeci_Hashtable_Link         *next,
                                      bslma_Allocator              *allocator)
: d_memory(value, allocator)
, d_next_p(next)
{
}

                           // =====================
                           // class bdeci_Hashtable
                           // =====================

template <class T, class HASH>
class bdeci_Hashtable {
    // This class implements an efficient hashtable of 'T' values.
    //
    // More generally, this class supports a complete set of *in-core* *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, and 'ostream' printing (but not 'bdex' serialization).  (A
    // precise operational definition of when two instances have the same value
    // can be found in the description of 'operator==' for the class.)  This
    // container is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // instance, the class is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

    // CONSTANTS
    enum {
        BDECI_CAPACITY_RATIO = 2  // maximum load factor:
                                  // d_numElements / d_numSlots
    };

    // TYPES
    class AutoRemoveAll {
        bdeci_Hashtable *d_origin;
        int d_release;
      public:
        AutoRemoveAll(bdeci_Hashtable *origin)
                                           : d_origin(origin), d_release(0) {}
        ~AutoRemoveAll() { if (!d_release) d_origin->removeAll(); }
        void release() { d_release = 1; }
    };

    bdeci_Hashtable_Link<T> **d_table_p; // hash table
    int                d_sizeIndex;     // physical size index
    int                d_numSlotsIndex; // logical size index
    int                d_numSlots;      // the number of slots in the table
    int                d_numElements;   // the number of elements in the table
    bdema_Pool         d_pool;          // manage free list for links
    bslma_Allocator   *d_allocator_p;   // holds (not owns) memory allocator

    // The values 'd_sizeIndex' and 'd_numSlotsIndex' are references into the
    // lookup table of 'bdeci_HashtableImpUtil::lookup(index)'.  Since it is
    // needed frequently, the value of
    // 'bdeci_HashtableImpUtil::lookup(d_numSlotsIndex)' is maintained in
    // 'd_numSlots'.

    // FRIENDS
    friend class bdeci_HashtableSlotIter<T, HASH>;
    friend class bdeci_HashtableSlotManip<T, HASH>;

  private:
    void deleteElements(bdeci_Hashtable_Link<T> **table,
                        int                       numSlots,
                        bdema_Pool               *pool);
        // Remove all elements from the hashtable indicated by the specified
        // 'table' and the specified 'numSlots'.  Use the specified 'pool' to
        // reclaim memory.  Note that any separate element count (e.g.,
        // 'd_numElements') should be zeroed.

    void ensureLoadFactor(bdeci_Hashtable_Link<T> ***addrTable,
                          int                       *addrSizeIndex,
                          int                       *addrNumSlotsIndex,
                          int                       *addrNumSlots,
                          int                        numElements,
                          bslma_Allocator           *allocator);
        // Ensure the load factor requirement on the table is currently met for
        // the hashtable indicated by the specified 'addrTable',
        // 'addrSizeIndex', 'addrNumSlotsIndex', 'addrNumSlots', and
        // 'numElements' by - if necessary - increasing the size of the table,
        // updating the table values, and using the specified 'allocator' to
        // obtain memory if there is currently insufficient memory allocated to
        // accommodate the resize.  The behavior is undefined unless
        // 0 <= *addrSizeIndex < bdeci_HashtableImpUtil::NUM_SIZES,
        // 0 <= *addrNumSlotsIndex < bdeci_HashtableImpUtil::NUM_SIZES,
        // 0 < *addrNumSlots,
        // *addrNumSlots == bdeci_HashtableImpUtil::lookup(*addrNumSlotsIndex),
        // and 0 <= numElements.  Note that if a resize causes an exception to
        // be thrown, this function has no effect; otherwise the resize is
        // guaranteed to work.

    void resizeEmptyTable(bdeci_Hashtable_Link<T> ***addrTable,
                          int                       *addrSizeIndex,
                          int                       *addrNumSlotsIndex,
                          int                       *addrNumSlots,
                          int                        newNumSlotsIndex,
                          int                        newNumSlots,
                          bslma_Allocator           *allocator);
        // Resize the empty hashtable indicated by the specified 'addrTable',
        // 'addrSizeIndex', 'addrNumSlotsIndex', and 'addrNumSlots' to the
        // logical size indicated by the specified 'newNumSlotsIndex' and
        // 'newNumSlots' by - if necessary - increasing the size of the table,
        // using the specified 'allocator' to obtain memory if there is
        // currently insufficient memory allocated to accommodate the resize.
        // The behavior is undefined unless
        // 0 <= *addrSizeIndex < bdeci_HashtableImpUtil::NUM_SIZES,
        // 0 <= *addrNumSlotsIndex < bdeci_HashtableImpUtil::NUM_SIZES,
        // 0 < *addrNumSlots,
        // *addrNumSlots == bdeci_HashtableImpUtil::lookup(*addrNumSlotsIndex),
        // 0 <= newNumSlotsIndex < bdeci_HashtableImpUtil::NUM_SIZES,
        // 0 < newNumSlots, and
        // newNumSlots == bdeci_HashtableImpUtil::lookup(newNumSlotsIndex).
        // Note that if a resize causes an exception to be thrown, this
        // function has no effect; otherwise the resize is guaranteed to work.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdeci_Hashtable,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // TYPES
    struct InitialCapacity {
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //  bdeci_Hashtable x(bdeci_Hashtable::InitialCapacity(8));
        //..
        // defines an instance, 'x', with an initial capacity of 8 elements,
        // but with a logical length of 0 elements.

        int d_i;
        ~InitialCapacity() { }
        explicit InitialCapacity(int i) : d_i(i) { }
    };

    // CREATORS
    explicit bdeci_Hashtable(bslma_Allocator *basicAllocator = 0);
        // Create an empty hashtable.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    explicit bdeci_Hashtable(const InitialCapacity&  numElements,
                             bslma_Allocator        *basicAllocator = 0);
        // Create an empty hashtable with sufficient initial capacity to
        // accommodate up to the specified 'numElements' values - excluding
        // capacity required by allocations by the Ts - without subsequent
        // reallocation.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // '0 <= numElements'.

    bdeci_Hashtable(const bdeci_Hashtable&  original,
                    bslma_Allocator        *basicAllocator = 0);
        // Create a hashtable initialized to the value of the specified
        // 'original' hashtable.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~bdeci_Hashtable();
        // Destroy this object.

    // MANIPULATORS
    bdeci_Hashtable& operator=(const bdeci_Hashtable& rhs);
        // Assign to this hashtable the value of the specified 'rhs' hashtable.
        // Return a reference to this hashtable.

    T *add(const T& value);
        // Add the specified 'value' to this hashtable, regardless of whether
        // the value is already present.  Return a pointer to the newly created
        // modifiable T.  The behavior of this hashtable is undefined if
        // subsequent modifications to the pointed to modifiable T result in
        // the hash value for the T changing.

    T *addUnique(const T& value);
        // Add the specified 'value' to this hashtable if 'value' is not
        // currently a member of this hashtable.  Return a pointer to the newly
        // created T if 'value' was not already a member of this hashtable,
        // and 0 otherwise.  The behavior of this hashtable is undefined if
        // subsequent modifications to the pointed to modifiable T result in
        // the hash value for the T changing.

    void compact();
        // Compact the hashtable so as to minimize the logical size of the
        // hashtable (the value returned by 'numSlots()').  This operation is
        // O[n] and is intended for optional use.

    T *lookup(const T& value);
        // Return a pointer to the modifiable value corresponding to the
        // specified 'value' if 'value' is a member of this hashtable, and 0
        // otherwise.  The pointer will remain valid as long as this hashtable
        // is not destroyed or modified (e.g., via 'add' or 'removeFirst').
        // The behavior of this hashtable is undefined if subsequent
        // modifications to the pointed to modifiable T result in the hash
        // value for the T changing.

    template <class TYPE>
    T *lookupRaw(const TYPE& value) {
        bdeci_Hashtable_Link<T> *p = d_table_p[HASH::hash(value, d_numSlots)];
        while (p) {
            if (p->value() == value) {
                return &p->value();
            }
            p = p->d_next_p;
        }
        return 0;
    }
        // TBD doc
        // Return a pointer to the modifiable value corresponding to the
        // specified 'value' if 'value' is a member of this hashtable, and 0
        // otherwise.  The pointer will remain valid as long as this hashtable
        // is not destroyed or modified (e.g., via 'add' or 'removeFirst').
        // The behavior of this hashtable is undefined if subsequent
        // modifications to the pointed to modifiable T result in the hash
        // value for the T changing.

    void removeAll();
        // Remove all members of this hashtable and assign this hashtable the
        // value of a hashtable created by the default constructor.

    int removeEvery(const T& value);
        // Remove every instance of the specified 'value' from this hashtable.
        // Return the number of members removed from this hashtable.

    int removeFirst(const T& value);
        // Remove the first occurrence of the specified 'value' from this
        // hashtable.  Return 1 if the 'value' was already a member of this
        // hashtable, and 0 otherwise.

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate up to the
        // specified 'numElements' values - excluding capacity required by
        // allocations by the Ts - without reallocation.  The behavior is
        // undefined unless 0 <= numElements.  Note that if a reallocation
        // occurs all hashtable members are rehashed and the value of the
        // hashtable has changed.

    T *set(const T& value);
        // Add the specified 'value' to this hashtable if 'value' is not
        // currently a member of this hashtable.  If 'value' is a member,
        // replace the 'value'.  Return a pointer to the T .  The behavior
        // of this hashtable is undefined if subsequent modifications to the
        // pointed to modifiable T result in the hash value for the T
        // changing.

    // ACCESSORS
    int hash(const T& value) const;
        // Return the hash value for the specified 'value' given the current
        // state of the hashtable.

    int isMember(const T& value) const;
        // Return 1 if the specified 'value' is an element of this hashtable,
        // and 0 otherwise.

    const T *lookup(const T& value) const;
        // Return a pointer to the non-modifiable value corresponding to the
        // specified 'value' if 'value' is a member of this hashtable, and 0
        // otherwise.  The pointer will remain valid as long as this hashtable
        // is not destroyed or modified (e.g., via 'add' or 'removeFirst').

    template <class TYPE>
    const T *lookupRaw(const TYPE& value) const {
        bdeci_Hashtable_Link<T> *p = d_table_p[HASH::hash(value, d_numSlots)];
        while (p) {
            if (p->value() == value) {
                return &p->value();
            }
            p = p->d_next_p;
        }
        return 0;
    }
        // Return a pointer to the non-modifiable value corresponding to the
        // specified 'value' if 'value' is a member of this hashtable, and 0
        // otherwise.  The pointer will remain valid as long as this hashtable
        // is not destroyed or modified (e.g., via 'add' or 'removeFirst').

    int numOccurrences(const T& value) const;
        // Return the number of occurrences of the specified 'value' in this
        // hashtable.

    int numElements() const;
        // Return the number of elements in this hashtable.

    bsl::ostream& print(bsl::ostream& stream,
                   int      level,
                   int      spacesPerLevel) const;
        // Format the element values in this hashtable to the specified output
        // 'stream' at the (absolute value of the) specified indentation
        // 'level' using the specified 'spacesPerLevel' of indentation.
        // Making 'level' negative suppress indentation for the first line
        // only.  The behavior is undefined unless 0 <= spacesPerLevel.

    int numSlots() const;
        // Return the number of slots in this hashtable.
};

// FREE OPERATORS
template <class T, class HASH>
bool operator==(const bdeci_Hashtable<T, HASH>& lhs,
                const bdeci_Hashtable<T, HASH>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' hash tables have the same
    // value, and 'false' otherwise.  Two hash tables have the same value if
    // they have the same number of slots, the same number of elements, the
    // same elements, and all elements of the hash tables are in the same
    // order.

template <class T, class HASH>
inline
bool operator!=(const bdeci_Hashtable<T, HASH>& lhs,
                const bdeci_Hashtable<T, HASH>& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' hash tables do not have
    // the same value, and 'false' otherwise.  Two hash tables do not have the
    // same value if they differ in number of slots, differ in number of
    // elements, differ in elements, or the elements are not in the same order.

template <class T, class HASH>
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const bdeci_Hashtable<T, HASH>& hashtable);
    // Write the specified 'hashtable' to the specified output 'stream' in some
    // reasonable (single-line) format and return a reference to 'stream'.

                       // =============================
                       // class bdeci_HashtableSlotIter
                       // =============================

template <class T, class HASH>
class bdeci_HashtableSlotIter {
    // Provide read-only, sequential access to the members of a hashtable of
    // 'T' elements.  The iterator becomes invalid if any manipulator
    // method of the associated hashtable is used.  Note that the order in
    // which members are visited is implementation dependent.

    bdeci_Hashtable_Link<T> **d_hashtable_p;  // hashtable
    bdeci_Hashtable_Link<T>  *d_link_p;       // current link

    // NOT IMPLEMENTED
    //bool operator==(const bdeci_HashtableSlotIter&) const;
    //bool operator!=(const bdeci_HashtableSlotIter&) const;

    //template<class OTHER, class HASH2>
    //bool operator==(const bdeci_HashtableSlotIter<OTHER, HASH2>&) const;
    //template<class OTHER, class HASH2>
    //bool operator!=(const bdeci_HashtableSlotIter<OTHER, HASH2>&) const;

  public:
    // CREATORS
    bdeci_HashtableSlotIter(const bdeci_Hashtable<T, HASH>& hashtable,
                            int                             slotIndex);
        // Create an iterator for the specified 'slotIndex' of the specified
        // 'hashtable'.  The behavior is undefined unless
        // 0 <= slotIndex < hashtable.numSlots().

    bdeci_HashtableSlotIter(const bdeci_HashtableSlotIter<T, HASH>& original);
        // TBD doc

    ~bdeci_HashtableSlotIter();
        // Destroy this object.

    // MANIPULATORS
    bdeci_HashtableSlotIter<T, HASH>&
                        operator=(const bdeci_HashtableSlotIter<T, HASH>& rhs);
        // TBD doc

    void operator++();
        // Advance the iterator to refer to the next member of its associated
        // hashtable.  The behavior is undefined unless the current hashtable
        // member is valid.  Note that the order of iteration is implementation
        // dependent.

    void setSlot(int slotIndex);
        // Change the referenced slot of this iterator to the specified
        // 'slotIndex'.  The behavior is undefined unless
        // 0 <= slotIndex < hashtable.numSlots().

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if the current member of the hashtable associated
        // with this iterator is valid, and 0 otherwise.

    const T& operator()() const;
        // Return the current member of the hashtable associated with this
        // iterator.  The behavior is undefined unless the current hashtable
        // member is valid.
};

                 // ==============================
                 // class bdeci_HashtableSlotManip
                 // ==============================

template <class T, class HASH>
class bdeci_HashtableSlotManip {
    // Provide sequential access with selective-remove (filtering) and raw
    // insertion capability to members of a hashtable of 'T' elements.
    // This manipulator becomes invalid if any manipulator method of the
    // associated hashtable or other manipulator is used.  Note that the order
    // in which members are visited is implementation dependent.

    bdeci_Hashtable_Link<T> **d_hashtable_p;    // hashtable
    bdeci_Hashtable_Link<T> **d_addrLink_p;     // current link
    int                      *d_numElements_p;  // length of table
    bdema_Pool               *d_pool_p;         // holds memory allocator
    bslma_Allocator   *d_allocator_p;   // holds (not owns) memory allocator

  private:  // not implemented
    bdeci_HashtableSlotManip(const bdeci_HashtableSlotManip&);
    bdeci_HashtableSlotManip& operator=(const bdeci_HashtableSlotManip&);

    bool operator==(const bdeci_HashtableSlotManip&) const;
    bool operator!=(const bdeci_HashtableSlotManip&) const;

    template<class OTHER, class HASH2>
    bool operator==(const bdeci_HashtableSlotManip<OTHER, HASH2>&) const;
    template<class OTHER, class HASH2>
    bool operator!=(const bdeci_HashtableSlotManip<OTHER, HASH2>&) const;

  public:
    // CREATORS
    bdeci_HashtableSlotManip(bdeci_Hashtable<T, HASH> *hashtable,
                             int                       slotIndex);
        // Create a manipulator for the specified 'slotIndex' of the specified
        // 'hashtable'.  Note that use of a manipulator invalidates iterators
        // and other manipulators operating on the same hashtable.  The
        // behavior is undefined unless 0 <= slotIndex < hashtable.numSlots().

    ~bdeci_HashtableSlotManip();
        // Destroy this object.

    // MANIPULATORS
    T& operator()();
        // Return the current member of the hashtable associated with this
        // manipulator.  The behavior is undefined unless the current hashtable
        // member is valid.

    void addRaw(const T& value);
        // Add the specified 'value' to this hashtable slot.  This method does
        // not maintain the invariants of the associated hashtable's load
        // factor restrictions and this must have been explicitly accounted for
        // with the hashtable's 'reserveCapacity' method (or equivalent) before
        // this manipulator was created.  The 'value' is inserted after the
        // current value and the manipulator is advanced to this newly inserted
        // 'value'.  The behavior of this manipulator and the associated
        // hashtable is undefined unless the slot this manipulator is acting
        // upon is equivalent to the one produced by the hash function for the
        // 'value'.

    void advance();
        // Advance this manipulator to refer to the next member of its
        // associated hashtable.  The behavior is undefined unless the current
        // hashtable member is valid.  Note that the order in which members are
        // visited is implementation dependent.

    void remove();
        // Remove the current member of this manipulator's associated
        // hashtable, in effect advancing to the next member.  The behavior is
        // undefined unless the current hashtable member is valid.

    void setSlot(int slotIndex);
        // Change the referenced slot of this manipulator to the specified
        // 'slotIndex'.  The behavior is undefined unless
        // '0 <= slotIndex < hashtable.numSlots()'.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if the current member of the hashtable associated
        // with this manipulator is valid, and 0 otherwise.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// PRIVATE MANIPULATORS
template <class T, class HASH>
void bdeci_Hashtable<T, HASH>::
                             deleteElements(bdeci_Hashtable_Link<T> **table,
                                            int                       numSlots,
                                            bdema_Pool               *pool)
{
    for (int i = 0; i < numSlots; ++i) {
        bdeci_Hashtable_Link<T> *p = table[i];
        while (p) {
            bdeci_Hashtable_Link<T> *tmp = p;
            p = p->d_next_p;
            tmp->~bdeci_Hashtable_Link<T>();
            pool->deallocate(tmp);
        }
    }
    bdeimp_DuffsDevice<bdeci_Hashtable_Link<T> *>::
                                             initializeRaw(table, 0, numSlots);
}

template <class T, class HASH>
void bdeci_Hashtable<T, HASH>::
                 ensureLoadFactor(bdeci_Hashtable_Link<T> ***addrTable,
                                  int                       *addrSizeIndex,
                                  int                       *addrNumSlotsIndex,
                                  int                       *addrNumSlots,
                                  int                        numElements,
                                  bslma_Allocator           *allocator)
{

    const int numSlots = *addrNumSlots;
    if (numElements > BDECI_CAPACITY_RATIO * numSlots) {
        int newNumSlotsIndex = *addrNumSlotsIndex + 1;
        int newNumSlots = bdeci_HashtableImpUtil::lookup(newNumSlotsIndex);
        while (numElements > BDECI_CAPACITY_RATIO * newNumSlots) {
            ++newNumSlotsIndex;
            newNumSlots = bdeci_HashtableImpUtil::lookup(newNumSlotsIndex);
        }
        bdeci_Hashtable_Link<T> **table = *addrTable;

        if (newNumSlotsIndex > *addrSizeIndex) {

            // Allocate a new table.

            bdeci_Hashtable_Link<T> **newTable =
                           (bdeci_Hashtable_Link<T> **)
                           allocator->allocate(newNumSlots * sizeof *newTable);

            // Zero the new table's slots.

            bdeimp_DuffsDevice<bdeci_Hashtable_Link<T> *>::
                                       initializeRaw(newTable, 0, newNumSlots);

            // Copy the data to the new table.

            for (int i = 0; i < numSlots; ++i) {
                bdeci_Hashtable_Link<T> *p = table[i];
                while (p) {
                    bdeci_Hashtable_Link<T> *& slot =
                                 newTable[HASH::hash(p->value(), newNumSlots)];
                    bdeci_Hashtable_Link<T> *q = p->d_next_p;
                    p->d_next_p = slot;
                    slot = p;
                    p = q;
                }
            }

            // Commit the changes to the table.

            allocator->deallocate(table);
            *addrTable = newTable;
            *addrSizeIndex = newNumSlotsIndex;
        }
        else {

            // Using existing memory.  Need to remove data from the table and
            // then replace it into the table; which implies a two-pass
            // solution.

            // Make a single linked-list of the data and zero table slots.

            bdeci_Hashtable_Link<T> *data = 0;
            for (int i = 0; i < numSlots; ++i) {
                bdeci_Hashtable_Link<T> *p = table[i];
                if (p) {
                    while (p->d_next_p) {
                        p = p->d_next_p;
                    }
                    p->d_next_p = data;
                    data = table[i];
                    table[i] = 0;
                }
            }

            // Zero new table slots.

            bdeimp_DuffsDevice<bdeci_Hashtable_Link<T> *>::
                    initializeRaw(table + numSlots, 0, newNumSlots - numSlots);

            // Replace the data.

            while (data) {
                bdeci_Hashtable_Link<T> *& slot =
                                 table[HASH::hash(data->value(), newNumSlots)];
                bdeci_Hashtable_Link<T> *q = data->d_next_p;
                data->d_next_p = slot;
                slot = data;
                data = q;
            }
        }

        // Commit the changes to the number of slots.

        *addrNumSlotsIndex = newNumSlotsIndex;
        *addrNumSlots = newNumSlots;
    }
}

template <class T, class HASH>
void bdeci_Hashtable<T, HASH>::
            resizeEmptyTable(bdeci_Hashtable_Link<T>      ***addrTable,
                                          int               *addrSizeIndex,
                                          int               *addrNumSlotsIndex,
                                          int               *addrNumSlots,
                                          int                newNumSlotsIndex,
                                          int                newNumSlots,
                                          bslma_Allocator   *allocator)
{
    if (*addrNumSlotsIndex < newNumSlotsIndex) {
        if (*addrSizeIndex < newNumSlotsIndex) {
            bdeci_Hashtable_Link<T> **ht = (bdeci_Hashtable_Link<T> **)
                                 allocator->allocate(newNumSlots * sizeof *ht);
            allocator->deallocate(*addrTable);
            *addrTable = ht;
            *addrSizeIndex = newNumSlotsIndex;
            bdeimp_DuffsDevice<bdeci_Hashtable_Link<T> *>::
                                     initializeRaw(*addrTable, 0, newNumSlots);
        }
        else {
            const int numSlots = *addrNumSlots;
            bdeimp_DuffsDevice<bdeci_Hashtable_Link<T> *>::
                                         initializeRaw(*addrTable + numSlots,
                                                       0,
                                                       newNumSlots - numSlots);
        }
    }
    *addrNumSlotsIndex = newNumSlotsIndex;
    *addrNumSlots = newNumSlots;
}

// CREATORS
template <class T, class HASH>
bdeci_Hashtable<T, HASH>::bdeci_Hashtable(bslma_Allocator *basicAllocator)
: d_sizeIndex(0)
, d_numSlotsIndex(0)
, d_numElements(0)
, d_pool(sizeof **d_table_p, basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_numSlots = bdeci_HashtableImpUtil::lookup(0);  // 0 == d_numSlotsIndex
    d_table_p = (bdeci_Hashtable_Link<T> **)
                       d_allocator_p->allocate(d_numSlots * sizeof *d_table_p);
    bdeimp_DuffsDevice<bdeci_Hashtable_Link<T> *>::
                                       initializeRaw(d_table_p, 0, d_numSlots);
}

template <class T, class HASH>
bdeci_Hashtable<T, HASH>::
                 bdeci_Hashtable(const InitialCapacity&  numElements,
                                 bslma_Allocator        *basicAllocator)
: d_numElements(0)
, d_pool(sizeof **d_table_p, basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_sizeIndex = 0;  // once sized, will be used to set 'd_numSlotsIndex'
    d_numSlots = bdeci_HashtableImpUtil::lookup(d_sizeIndex);
    while (numElements.d_i > BDECI_CAPACITY_RATIO * d_numSlots) {
        ++d_sizeIndex;
        d_numSlots = bdeci_HashtableImpUtil::lookup(d_sizeIndex);
    }
    d_numSlotsIndex = d_sizeIndex;

    d_pool.reserveCapacity(numElements.d_i);
    d_table_p = (bdeci_Hashtable_Link<T> **)
                       d_allocator_p->allocate(d_numSlots * sizeof *d_table_p);
    bdeimp_DuffsDevice<bdeci_Hashtable_Link<T> *>::
                                       initializeRaw(d_table_p, 0, d_numSlots);
}

template <class T, class HASH>
bdeci_Hashtable<T, HASH>::
                       bdeci_Hashtable(const bdeci_Hashtable&  original,
                                       bslma_Allocator        *basicAllocator)
: d_sizeIndex(original.d_numSlotsIndex)
, d_numSlotsIndex(original.d_numSlotsIndex)
, d_numSlots(original.d_numSlots)
, d_numElements(0)
, d_pool(sizeof **d_table_p, basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    d_pool.reserveCapacity(original.d_numElements);
    d_table_p = (bdeci_Hashtable_Link<T> **)
                       d_allocator_p->allocate(d_numSlots * sizeof *d_table_p);
    bslma_DeallocatorProctor<bslma_Allocator> autoTableDeallocator(d_table_p,
                                                                d_allocator_p);
    bdeimp_DuffsDevice<bdeci_Hashtable_Link<T> *>::
                                       initializeRaw(d_table_p, 0, d_numSlots);

    // Copy the data while maintaining order.

    AutoRemoveAll autoRemoveAll(this);
    for (int i = 0; i < d_numSlots; ++i) {
        bdeci_Hashtable_Link<T> *p = original.d_table_p[i];
        bdeci_Hashtable_Link<T> **slot = d_table_p + i;
        while (p) {
            *slot = new(d_pool.allocate())
                                        bdeci_Hashtable_Link<T>(p->value(),
                                                               *slot,
                                                                d_allocator_p);
            ++d_numElements;
            p = p->d_next_p;
            slot = &(*slot)->d_next_p;
        }
    }
    autoRemoveAll.release();
    autoTableDeallocator.release();
}

template <class T, class HASH>
bdeci_Hashtable<T, HASH>::~bdeci_Hashtable()
    // NOTE: destroying the embedded d_pool will release all T data
{
    for (int i = 0; i < d_numSlots; ++i) {
        bdeci_Hashtable_Link<T> *p = d_table_p[i];
        while (p) {
            bdeci_Hashtable_Link<T> *q = p;
            p = p->d_next_p;
            q->~bdeci_Hashtable_Link<T>();
        }
    }
    d_allocator_p->deallocate(d_table_p);
}

// MANIPULATORS
template <class T, class HASH>
bdeci_Hashtable<T, HASH>&
                  bdeci_Hashtable<T, HASH>::
                  operator=(const bdeci_Hashtable<T, HASH>& rhs)
{
    if (this != &rhs) {
        if (d_numElements < rhs.d_numElements) {
            d_pool.reserveCapacity(rhs.d_numElements - d_numElements);
        }
        deleteElements(d_table_p,
                       d_numSlots,
                       &d_pool);
        d_numElements = 0;
        resizeEmptyTable(&d_table_p,
                         &d_sizeIndex,
                         &d_numSlotsIndex,
                         &d_numSlots,
                         rhs.d_numSlotsIndex,
                         rhs.d_numSlots,
                         d_allocator_p);

        // Copy the data while maintaining order.

        AutoRemoveAll autoRemoveAll(this);
        for (int i = 0; i < d_numSlots; ++i) {
            bdeci_Hashtable_Link<T> *p = rhs.d_table_p[i];
            bdeci_Hashtable_Link<T> **slot = d_table_p + i;
            while (p) {
                *slot = new(d_pool.allocate())
                                        bdeci_Hashtable_Link<T>(p->value(),
                                                               *slot,
                                                                d_allocator_p);
                ++d_numElements;
                p = p->d_next_p;
                slot = &(*slot)->d_next_p;
            }
        }
        autoRemoveAll.release();
    }
    return *this;
}

template <class T, class HASH>
T *bdeci_Hashtable<T, HASH>::add(const T& value)
{
    ensureLoadFactor(&d_table_p,
                     &d_sizeIndex,
                     &d_numSlotsIndex,
                     &d_numSlots,
                     d_numElements + 1,
                     d_allocator_p);
    bdeci_Hashtable_Link<T> *& slot = d_table_p[HASH::hash(value, d_numSlots)];
    void *memory = d_pool.allocate();
    bslma_DeallocatorProctor<bdema_Pool> autoMemoryDeallocator(memory,
                                                                    &d_pool);
    slot = new(memory) bdeci_Hashtable_Link<T>(value, slot, d_allocator_p);
    autoMemoryDeallocator.release();
    ++d_numElements;
    return &slot->value();
}

template <class T, class HASH>
T *bdeci_Hashtable<T, HASH>::addUnique(const T& value)
{
    ensureLoadFactor(&d_table_p,
                     &d_sizeIndex,
                     &d_numSlotsIndex,
                     &d_numSlots,
                     d_numElements + 1,
                     d_allocator_p);
    bdeci_Hashtable_Link<T> *& slot = d_table_p[HASH::hash(value, d_numSlots)];
    bdeci_Hashtable_Link<T> *p = slot;
    while (p) {
        if (p->value() == value) {
            return 0;
        }
        p = p->d_next_p;
    }
    void *memory = d_pool.allocate();
    bslma_DeallocatorProctor<bdema_Pool> autoMemoryDeallocator(memory,
                                                                    &d_pool);
    slot = new(memory) bdeci_Hashtable_Link<T>(value, slot, d_allocator_p);
    autoMemoryDeallocator.release();
    ++d_numElements;
    return &slot->value();
}

template <class T, class HASH>
void bdeci_Hashtable<T, HASH>::compact()
{

    // Compute required number of slots.

    int newNumSlotsIndex = 0;
    int newNumSlots = bdeci_HashtableImpUtil::lookup(newNumSlotsIndex);
    while (d_numElements > BDECI_CAPACITY_RATIO * newNumSlots) {
        ++newNumSlotsIndex;
        newNumSlots = bdeci_HashtableImpUtil::lookup(newNumSlotsIndex);
    }


    if (newNumSlotsIndex < d_numSlotsIndex) {

        // Make a single linked-list of the data and zero table slots.

        bdeci_Hashtable_Link<T> *data = 0;
        for (int i = 0; i < d_numSlots; ++i) {
            bdeci_Hashtable_Link<T> *p = d_table_p[i];
            if (p) {
                while (p->d_next_p) {
                    p = p->d_next_p;
                }
                p->d_next_p = data;
                data = d_table_p[i];
                d_table_p[i] = 0;
            }
        }

        // Replace the data.

        while (data) {
            bdeci_Hashtable_Link<T> *& slot =
                             d_table_p[HASH::hash(data->value(), newNumSlots)];
            bdeci_Hashtable_Link<T> *q = data->d_next_p;
            data->d_next_p = slot;
            slot = data;
            data = q;
        }

        // Commit the changes to the number of slots.

        d_numSlotsIndex = newNumSlotsIndex;
        d_numSlots = newNumSlots;
    }
}

template <class T, class HASH>
T *bdeci_Hashtable<T, HASH>::lookup(const T& value)
{
    bdeci_Hashtable_Link<T> *p = d_table_p[HASH::hash(value, d_numSlots)];
    while (p) {
        if (p->value() == value) {
            return &p->value();
        }
        p = p->d_next_p;
    }
    return 0;
}

template <class T, class HASH>
void bdeci_Hashtable<T, HASH>::removeAll()
{
    for (int i = 0; i < d_numSlots; ++i) {
        bdeci_Hashtable_Link<T> *p = d_table_p[i];
        while (p) {
            bdeci_Hashtable_Link<T> *q = p;
            p = p->d_next_p;
            q->~bdeci_Hashtable_Link<T>();
            d_pool.deallocate(q);
        }
    }
    d_numElements = 0;
    d_numSlotsIndex = 0;
    d_numSlots = bdeci_HashtableImpUtil::lookup(0);  // 0 == d_numSlotsIndex
    bdeimp_DuffsDevice<bdeci_Hashtable_Link<T> *>::
                                       initializeRaw(d_table_p, 0, d_numSlots);
}

#if defined(BSLS_PLATFORM__CMP_MSVC)
#pragma warning(push)
// Protect the next two functions against warning C4706, "assignment within a
// conditional expression".  This is the while-loop condition, which tests that
// a non-null pointer is returned to continue each iteration of the loop.  The
// design is intentional, but the only way to silence the warning in is with a
// pragma.  Note that a further Microsoft bug means this pragma must be
// installed outside of any function body, see the link below for more details.
// http://support.microsoft.com/kb/120968
#pragma warning(disable : 4706) // assignment within conditional expression
#endif

template <class T, class HASH>
int bdeci_Hashtable<T, HASH>::removeEvery(const T& value)
{
    const int hash = HASH::hash(value, d_numSlots);
    bdeci_Hashtable_Link<T> **addrLink = d_table_p + hash;
    int count = 0;

    bdeci_Hashtable_Link<T> *p;  // set in while
    while ((p = *addrLink)) {  // "=" o.k.
        if (p->value() == value) {  // remove and advance
            *addrLink = p->d_next_p;
            p->~bdeci_Hashtable_Link<T>();
            d_pool.deallocate(p);
            --d_numElements;
            ++count;
        }
        else {
            addrLink = &p->d_next_p;  // advance
        }
    }
    return count;
}

template <class T, class HASH>
int bdeci_Hashtable<T, HASH>::removeFirst(const T& value)
{
    const int hash = HASH::hash(value, d_numSlots);
    bdeci_Hashtable_Link<T> **addrLink = d_table_p + hash;

    bdeci_Hashtable_Link<T> *p;  // set in while
    while ((p = *addrLink)) {  // "=" o.k.
        if (p->value() == value) {  // remove
            *addrLink = p->d_next_p;
            p->~bdeci_Hashtable_Link<T>();
            d_pool.deallocate(p);
            --d_numElements;
            return 1;
        }
        addrLink = &p->d_next_p;  // advance
    }
    return 0;
}

#if defined(BSLS_PLATFORM__CMP_MSVC)
#pragma warning(push)
// Restore the state of the warning flag.  It is most important that we do not
// accidentally disable the warning for later headers.
#endif

template <class T, class HASH>
void bdeci_Hashtable<T, HASH>::reserveCapacity(int numElements)
{
    if (numElements > d_numElements) {
        d_pool.reserveCapacity(numElements - d_numElements);
        ensureLoadFactor(&d_table_p,
                         &d_sizeIndex,
                         &d_numSlotsIndex,
                         &d_numSlots,
                         numElements,
                         d_allocator_p);
    }
}

template <class T, class HASH>
T *bdeci_Hashtable<T, HASH>::set(const T& value)
{
    ensureLoadFactor(&d_table_p,
                     &d_sizeIndex,
                     &d_numSlotsIndex,
                     &d_numSlots,
                     d_numElements + 1,
                     d_allocator_p);
    bdeci_Hashtable_Link<T> *& slot = d_table_p[HASH::hash(value, d_numSlots)];
    bdeci_Hashtable_Link<T> *p = slot;
    while (p) {
        if (p->value() == value) {
            p->value() = value;
            return &p->value();
        }
        p = p->d_next_p;
    }
    void *memory = d_pool.allocate();
    bslma_DeallocatorProctor<bdema_Pool> autoMemoryDeallocator(memory,
                                                                    &d_pool);
    slot = new(memory) bdeci_Hashtable_Link<T>(value, slot, d_allocator_p);
    autoMemoryDeallocator.release();
    ++d_numElements;
    return &slot->value();
}

// ACCESSORS
template <class T, class HASH>
inline
int bdeci_Hashtable<T, HASH>::hash(const T& value) const
{
    return HASH::hash(value, d_numSlots);
}

template <class T, class HASH>
int bdeci_Hashtable<T, HASH>::isMember(const T& value) const
{
    bdeci_Hashtable_Link<T> *p = d_table_p[HASH::hash(value, d_numSlots)];
    while (p) {
        if (p->value() == value) {
            return 1;
        }
        p = p->d_next_p;
    }
    return 0;
}

template <class T, class HASH>
const T *bdeci_Hashtable<T, HASH>::lookup(const T& value) const
{
    bdeci_Hashtable_Link<T> *p = d_table_p[HASH::hash(value, d_numSlots)];
    while (p) {
        if (p->value() == value) {
            return &p->value();
        }
        p = p->d_next_p;
    }
    return 0;
}

template <class T, class HASH>
inline
int bdeci_Hashtable<T, HASH>::numElements() const
{
    return d_numElements;
}

template <class T, class HASH>
int bdeci_Hashtable<T, HASH>::numOccurrences(const T& value) const
{
    bdeci_Hashtable_Link<T> *p = d_table_p[HASH::hash(value, d_numSlots)];
    int count = 0;
    while (p) {
        if (p->value() == value) {
            ++count;
        }
        p = p->d_next_p;
    }
    return count;
}

template <class T, class HASH>
inline
int bdeci_Hashtable<T, HASH>::numSlots() const
{
    return d_numSlots;
}

template <class T, class HASH>
bsl::ostream& bdeci_Hashtable<T, HASH>::print(bsl::ostream& stream,
                                         int      level,
                                         int      spacesPerLevel) const
{
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[\n";
    if (level < 0) {
        level = -level;
    }
    int levelPlus1 = level + 1;
    for (int i = 0; i < d_numSlots; ++i) {
        bdeci_Hashtable_Link<T> *p = d_table_p[i];
        while (p) {
            bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
            stream << p->value() << '\n';
            p = p->d_next_p;
        }
    }
    return stream << "]\n";
}

// FREE OPERATORS
template <class T, class HASH>
bool operator==(const bdeci_Hashtable<T, HASH>& lhs,
                const bdeci_Hashtable<T, HASH>& rhs)
{
    const int numSlots = lhs.numSlots();
    if (lhs.numElements() != rhs.numElements() ||
        numSlots != rhs.numSlots()) {
        return 0;
    }
    for (int slotIndex = 0; slotIndex < numSlots; ++slotIndex) {
        bdeci_HashtableSlotIter<T, HASH> li(lhs, slotIndex);
        bdeci_HashtableSlotIter<T, HASH> ri(rhs, slotIndex);
        while (li && ri) {
            if (li() != ri()) {
              return 0;
            }
            ++li;
            ++ri;
        }
        if (li != ri) {
            return 0;
        }
    }
    return 1;
}

template <class T, class HASH>
inline
bool operator!=(const bdeci_Hashtable<T, HASH>& lhs,
                const bdeci_Hashtable<T, HASH>& rhs)
{
    return !(lhs == rhs);
}

template <class T, class HASH>
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const bdeci_Hashtable<T, HASH>& hashtable)
{
    stream << "[ ";
    const int numSlots = hashtable.numSlots();
    for (int slotIndex = 0; slotIndex < numSlots; ++slotIndex) {
        bdeci_HashtableSlotIter<T, HASH> iter(hashtable, slotIndex);
        while (iter) {
            stream << iter() << ' ';
            ++iter;
        }
    }
    return stream << ']';
}

                          // -----------------------
                          // bdeci_HashtableSlotIter
                          // -----------------------

template <class T, class HASH>
inline
bdeci_HashtableSlotIter<T, HASH>::
             bdeci_HashtableSlotIter(const bdeci_Hashtable<T, HASH>& hashtable,
                                     int                             slotIndex)
: d_hashtable_p(hashtable.d_table_p)
, d_link_p(hashtable.d_table_p[slotIndex])
{
}

template <class T, class HASH>
inline
bdeci_HashtableSlotIter<T, HASH>::
      bdeci_HashtableSlotIter(const bdeci_HashtableSlotIter<T, HASH>& original)
: d_hashtable_p(original.d_hashtable_p)
, d_link_p(original.d_link_p)
{
}

template <class T, class HASH>
inline
bdeci_HashtableSlotIter<T, HASH>::~bdeci_HashtableSlotIter()
{
}

template <class T, class HASH>
inline
bdeci_HashtableSlotIter<T, HASH>& bdeci_HashtableSlotIter<T, HASH>::
                         operator=(const bdeci_HashtableSlotIter<T, HASH>& rhs)
{
    d_hashtable_p = rhs.d_hashtable_p;
    d_link_p = rhs.d_link_p;
    return *this;
}

template <class T, class HASH>
inline
void bdeci_HashtableSlotIter<T, HASH>::operator++()
{
    d_link_p = d_link_p->d_next_p;
}

template <class T, class HASH>
inline
void bdeci_HashtableSlotIter<T, HASH>::setSlot(int slotIndex)
{
    d_link_p = d_hashtable_p[slotIndex];
}

template <class T, class HASH>
inline
bdeci_HashtableSlotIter<T, HASH>::operator const void *() const
{
    return d_link_p;
}

template <class T, class HASH>
inline
const T& bdeci_HashtableSlotIter<T, HASH>::operator()() const
{
    return d_link_p->value();
}

                          // ------------------------
                          // bdeci_HashtableSlotManip
                          // ------------------------

template <class T, class HASH>
bdeci_HashtableSlotManip<T, HASH>::
                  bdeci_HashtableSlotManip(bdeci_Hashtable<T, HASH> *hashtable,
                                           int                       slotIndex)
: d_hashtable_p(hashtable->d_table_p)
, d_addrLink_p(hashtable->d_table_p + slotIndex)
, d_numElements_p(&hashtable->d_numElements)
, d_pool_p(&hashtable->d_pool)
, d_allocator_p(hashtable->d_allocator_p)
{
}

template <class T, class HASH>
inline
bdeci_HashtableSlotManip<T, HASH>::~bdeci_HashtableSlotManip()
{
}

template <class T, class HASH>
inline
T& bdeci_HashtableSlotManip<T, HASH>::operator()()
{
    return (*d_addrLink_p)->value();
}

template <class T, class HASH>
void bdeci_HashtableSlotManip<T, HASH>::addRaw(const T& value)
{
    *d_addrLink_p = new(d_pool_p->allocate())
                                        bdeci_Hashtable_Link<T>(value,
                                                               *d_addrLink_p,
                                                                d_allocator_p);
    ++(*d_numElements_p);
}

template <class T, class HASH>
inline
void bdeci_HashtableSlotManip<T, HASH>::advance()
{
    d_addrLink_p = &(*d_addrLink_p)->d_next_p;
}

template <class T, class HASH>
void bdeci_HashtableSlotManip<T, HASH>::remove()
{
    // remove link
    bdeci_Hashtable_Link<T> *q = *d_addrLink_p;
    *d_addrLink_p = q->d_next_p;
    q->~bdeci_Hashtable_Link<T>();
    d_pool_p->deallocate(q);
    --(*d_numElements_p);
}

template <class T, class HASH>
inline
void bdeci_HashtableSlotManip<T, HASH>::setSlot(int slotIndex)
{
    d_addrLink_p = d_hashtable_p + slotIndex;
}

template <class T, class HASH>
inline
bdeci_HashtableSlotManip<T, HASH>::operator const void *() const
{
    return *d_addrLink_p;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
