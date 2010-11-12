// bdem_idxoffsetmap.h           -*-C++-*-
#ifndef INCLUDED_BDEM_IDXOFFSETMAP
#define INCLUDED_BDEM_IDXOFFSETMAP

//@PURPOSE: Map indices to element types and memory-layout positions.
//
//@CLASSES:
//   bdem_IdxOffsetMap:  map of indices to 'bdem_Descriptor *' + offset structs
//
//@AUTHOR: Dan Glaser (dglaser)
//
//@SEE ALSO: bdem_aggregate
//
//@DESCRIPTION: This component maps an integer index to a pair (Type
// Descriptor *, Offset), where Type Descriptor is a structure storing the
// address of nine canonical functions (e.g., copy constructor) for some type,
// and Offset is an integer identifying where the item positioned at that
// index would be placed in a contiguous-memory arrangement of map items.
//
// The particular layout identified by this component employs a minimally-sized
// packing of the items in memory where possible, respecting alignment
// considerations appropriate for the platform.
//
///PERFORMANCE
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, O[f(N,M)], where the names 'N' and 'M' also refer to
// the number of respective elements in each container (i.e., its 'length()').
// Here the amortized cost is A[f(N)] when there exists a y such that for all
// x > y, the execution time of the worst case of all sequences of x operations
// never exceeds O[x*f(N)].
//..
//     Operation                Worst Case          Average Case
//     ---------                ----------          ------------
//     DEFAULT CTOR             O[1]
//     COPY CTOR(N)             O[N]
//     N.DTOR()                 O[1]
//     N.OP=(M)                 O[M]
//     OP==(N,M)                O[min(N,M)]
//
//     N.append(value)          O[N]                A[1]
//     N.insert(index, value)   O[N]
//     N.replace(index, value)  O[1]
//     N.remove(index)          O[N]
//
//     N.compact()              O[N]
//     N.length()               O[1]
//..
//
///USAGE
///-----
// The following snippets of code illustrate how to create and use a map.  (The
// asserted values are correct for a 32 bit machine -- the values will differ
// slightly on a 64 bit machine.)
//
// First create an empty 'bdem_IdxOffsetMap' 'm'.
//..
//      bdem_IdxOffsetMap m;          assert(0 == m.length());
//..
// Next add some elements to the map, using the 'append' method, and print it.
//..
//      m.append(&bdem_Properties::d_doubleAttr);
//      assert( 1 == m.length());
//      assert( 8 == m.totalOffset());
//
//      m.append(&bdem_Properties::d_int64Attr);
//      assert( 2 == m.length());
//      assert(16 == m.totalOffset());
//
//      m.append(&bdem_Properties::d_doubleArrayAttr);
//      assert( 3 == m.length());
//      assert(32 == m.totalOffset());
//
//      m.append(&bdem_Properties::d_intArrayAttr);
//      assert( 4 == m.length());
//      assert(48 == m.totalOffset());
//
//      cout << m << endl;
//..
// This produces the following output on 'stdout' (all on one line):
//..
//      Types: [ DOUBLE INT64 DOUBLE_ARRAY INT64_ARRAY ]
//      Offsets: [ 0 8 16 32 ] TotalOffset: 48
//..
// Now remove the 2nd element (i.e., the element indexed by 1) and print
// the map again.
//..
//      m.remove(1);
//      assert( 3 == m.length());
//      assert(48 == m.totalOffset());
//
//      cout << m << endl;
//..
// This time the output is as follows (all on one line):
//..
//      Types: [ DOUBLE DOUBLE_ARRAY INT64_ARRAY ]
//      Offsets: [ 0 16 32 ] TotalOffset: 48
//..
// Finally, recalculate the offsets and print the map one last time.
//..
//      m.compact();
//      assert( 3 == m.length());
//      assert(40 == m.totalOffset());
//
//      cout << m << endl;
//..
// The output is now (all on one line):
//..
//      Types: [ DOUBLE DOUBLE_ARRAY INT64_ARRAY ]
//      Offsets: [ 0 24 8 ] TotalOffset: 40
//..
// Note that specifying an index outside of the valid range will result in
// undefined behavior.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_DESCRIPTOR
#include <bdem_descriptor.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEMF_ISBITWISE
#include <bdemf_isbitwise.h>
#endif

#ifndef INCLUDED_BDEMA_ALLOCATOR
#include <bdema_allocator.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>        // for 'swap'
#define INCLUDED_ALGORITHM
#endif

#ifndef INCLUDED_CSTRING
#include <cstring>          // for 'memcpy'
#define INCLUDED_CSTRING
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_VECTOR
#include <vector>
#define INCLUDED_VECTOR
#endif

namespace BloombergLP {

struct bdem_IdxOffsetMapEntry {
    const bdem_Descriptor *d_elemAttr;
    int                    d_offset;
};

// NON-MEMBER TRAITS
template <>
struct bdemf_IsBitwiseCopyable<bdem_IdxOffsetMapEntry> { enum {VALUE = 1}; };
template <>
struct bdemf_IsBitwiseMoveable<bdem_IdxOffsetMapEntry> { enum {VALUE = 1}; };

                        // =======================
                        // class bdem_IdxOffsetMap
                        // =======================

class bdem_IdxOffsetMap {
    // This class implements an efficient map from an integer index to a pair
    // (bdem_Descriptor *, integer).  The integral offsets are calculated on
    // construction, or can be explicitly recalculated by 'compact'.  The
    // physical capacity of this map may grow, but never shrinks.  Capacity
    // may be reserved initially via a constructor, or at any time thereafter
    // by using the 'reserveCapacity' method; otherwise, capacity will be
    // increased automatically as needed.  Note that capacity is not a logical
    // attribute of this object, and is not a part of its value.
    //
    // More generally, this container class supports a complete set of *value*
    // *semantic* operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  (A precise
    // operational definition of when two instances have the same value can be
    // found in the description of 'operator==' for the class.)  This container
    // is *exception* *neutral* with no guarantee of rollback: if an exception
    // is thrown during the invocation of a method on a pre-existing instance,
    // the container is left in a valid state, but its value is undefined.  In
    // no event is memory leaked.  Finally, *aliasing* (e.g., using all or part
    // of an object as both source and destination) is supported in all cases.

  private:
    std::vector<bdem_IdxOffsetMapEntry>
                            d_map;          // this map's values (keys
                                            // implicit -- array indices)
    int                     d_totalOffset;  // total space described by offsets

  public:
    // TYPES
    struct InitialCapacity {
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //    bdem_IdxOffsetMap m(bdem_IdxOffsetMap::InitialCapacity(8));
        //..
        // defines an instance 'm' with an initial capacity of 8 elements, but
        // with a logical length of 0 elements.

        int d_i;
        InitialCapacity(int i) : d_i(i) { }
        ~InitialCapacity() { }
    };

    // CREATORS
    explicit
    bdem_IdxOffsetMap(bdema_Allocator        *basicAllocator = 0);
    explicit
    bdem_IdxOffsetMap(const InitialCapacity&  numElements,
                      bdema_Allocator        *basicAllocator = 0);
        // Create an index offset map.  By default, the map is empty.
        // Optionally specify an initial size, 'numElements', to create an
        // index offset map with sufficient initial capacity to accommodate
        // up to the specified 'numElements' values without subsequent
        // reallocation.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  The behavior is undefined unless
        // 0 <= numElements.

    bdem_IdxOffsetMap(const bdem_ElemType::Type     elementTypes[],
                      int                           numElements,
                      const bdem_Descriptor *const  attrLookupTbl[],
                      bdema_Allocator              *basicAllocator = 0);
        // Create an index offset map whose element type values are those
        // contained in the specified 'types' array.  The offset of each map
        // element is calculated.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    bdem_IdxOffsetMap(const bdem_IdxOffsetMap&  original,
                      bdema_Allocator          *basicAllocator = 0);
        // Create an index offset map with each element's type and offset
        // values initialized to the values of the specified 'original' map.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bdem_IdxOffsetMap(const bdem_IdxOffsetMap&  original,
                      int                       srcIndex,
                      int                       numElements,
                      bdema_Allocator          *basicAllocator = 0);
        // Create an index-offset map with each element's values initialized
        // to the specified 'numElements' values of the specified 'original'
        // map, starting with the entry at the specified 'srcIndex'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~bdem_IdxOffsetMap();
        // Destroy this object.

    // MANIPULATORS
    void append(const bdem_Descriptor *itemType);
        // Append to the end of this map an element whose type value is the
        // specified 'item' and whose offset is calculated.  Appending an
        // element will not alter the offset of any other element in the map;
        // the resulting packing described by the offsets is *not* guaranteed
        // to be minimal.  To achieve minimal packing, call 'compact' after
        // all modifications are complete.  Note that this function is
        // logically equivalent to the following:
        //..
        //    insert(length(), item);
        //..

    void append(const bdem_ElemType::Type     elementTypes[],
                int                           numElements,
                const bdem_Descriptor *const  attrLookupTbl[]);
        // Append to the end of this map elements whose type values are those
        // of the specified 'srcArray' and whose offsets are calculated.
        // Appending elements will not alter the offset of any other element
        // in the map; the resulting packing described by the offsets is
        // *not* guaranteed to be minimal.  To achieve minimal packing, call
        // 'compact' after all modifications are complete.  Note that this
        // function is logically equivalent to the following:
        //..
        //    insert(length(), srcArray);
        //..

    void append(const bdem_ElemType::Type     elementTypes[],
                int                           numElements,
                int                           srcIndex,
                const bdem_Descriptor *const  attrLookupTbl[]);
        // Append to the end of this map 'numElements' elements whose type
        // values are those of the specified 'srcArray', beginning at the
        // specified 'srcIndex', and whose offsets are calculated.  Appending
        // elements will not alter the offset of any other element in the map;
        // the resulting packing described by the offsets is *not* guaranteed
        // to be minimal.  To achieve minimal packing, call 'compact' after
        // all modifications are complete.  The behavior is undefined
        // unless 0 <= srcIndex, 0 <= numElements, and
        // srcIndex + numElements <= srcArray.size().  Note that this
        // function is logically equivalent to the following:
        //..
        //    insert(length(), srcArray, srcIndex, numElements);
        //..

    void compact();
        // Recalculate the offsets of the sequence of elements contained in
        // this map in order to produce minimal packing.  The type value at
        // each index is unaffected.  Note that this function potentially
        // alters the offset of every element in the map.  Warning:
        // Inappropriate use of this method such as repeatedly calling
        //..
        //    a.insert(index, item);
        //    a.compact();
        //..
        // may lead to poor runtime behavior.

    void insert(int dstIndex, const bdem_Descriptor *elemAttr);
        // Insert into this map at the specified 'dstIndex' an element whose
        // type value is the specified 'item' and whose offset is calculated.
        // All elements with initial indices at or above 'dstIndex' are shifted
        // up by one index position.  Inserting an element will not alter the
        // offset of any other element in the map; the resulting packing
        // described by the offsets is *not* guaranteed to be minimal.  To
        // achieve minimal packing, call 'compact' after all modifications
        // are complete.  The behavior is undefined unless
        // 0 <= dstIndex <= length().

    void insert(int                           dstIndex,
                const bdem_ElemType::Type     elementTypes[],
                int                           numElements,
                const bdem_Descriptor *const  attrLookupTbl[]);
        // Insert into this map, at the specified 'dstIndex, elements whose
        // type values are those of the specified 'srcArray' and whose offsets
        // are calculated.  All elements with initial indices at or above
        // 'dstIndex' are shifted up by 'srcArray.size()' index positions.
        // Inserting elements will not alter the offset of any other element
        // in the map; the resulting packing described by the offsets is *not*
        // guaranteed to be minimal.  To achieve minimal packing, call
        // 'compact' after all modifications are complete.  The behavior is
        // undefined unless 0 <= dstIndex <= length().

    void remove(int index);
        // Remove from this map the element at the specified 'index'.  All
        // elements with initial indices above 'index' are shifted down by one
        // index position.  Removing an element will not alter the offset of
        // any other element in the map; the resulting packing described by the
        // offsets is *not* guaranteed to be minimal.  To achieve minimal
        // packing, call 'compact' after all modifications are complete.  The
        // behavior is undefined unless 0 <= index < length().

    void remove(int index, int numElements);
        // Remove from this map, beginning at the specified 'index', the
        // specified 'numElements' elements.  All elements with initial indices
        // at or above 'index' + 'numElements' are shifted down by
        // 'numElements' index positions.  Removing elements will not alter the
        // offset of any other element in the map.  However, the packing
        // described by the offsets is *not* guaranteed to be minimal.  To
        // achieve minimal packing, call 'compact' after all modifications are
        // complete.  The behavior is undefined unless 0 <= index,
        // 0 <= numElements, and index + numElements <= length().

    void removeAll();
        // Remove all elements from this map.  Note that 'length()' is now 0.

    void replace(int                    dstIndex,
                 const bdem_Descriptor *elemAttr);
        // Replace the element at the specified 'dstIndex' in this map with one
        // whose type value is the specified 'item' and whose offset is
        // calculated.  Replacing an element will not alter the offset of any
        // other element in the map; the resulting packing described by the
        // offsets is *not* guaranteed to be minimal.  To achieve minimal
        // packing, call 'compact' after all modifications are complete.
        // The behavior is undefined unless 0 <= dstIndex < length().  Note
        // that this function is logically equivalent to (but more efficient
        // than) the following:
        //..
        //    remove(dstIndex);
        //    insert(dstIndex, item);
        //..

    void replace(int                           dstIndex,
                 const bdem_ElemType::Type     elementTypes[],
                 int                           numElements,
                 const bdem_Descriptor *const  attrLookupTbl[]);
        // Replace the specified 'numElements' elements beginning at the
        // specified 'dstIndex' in this map with elements whose type values
        // are those of the specified 'elemtTypes', with element attribute
        // structure as in the specified 'attrLookupTbl'.  Replacing elements
        // will not alter the offset of any other element in the map; the
        // resulting packing described by the offsets is *not* guaranteed to
        // be minimal.  To achieve minimal packing, call 'compact' after all
        // modifications are complete.  The behavior is undefined unless
        // 0 <= 'dstIndex', 0 <= 'numElements', and
        // 'dstIndex' + numElements <= length().

    void replace(int                                        dstIndex,
                 const std::vector<const bdem_Descriptor*>& srcArray,
                 int                                        srcIndex,
                 int                                        numElements);
        // Replace the specified 'numElements' elements beginning at the
        // specified 'dstIndex' in this map with elements whose type values
        // are those of the specified 'srcArray', beginning at the specified
        // 'srcIndex', and whose offsets are calculated.  Replacing elements
        // will not alter the offset of any other element in the map; the
        // resulting packing described by the offsets is *not* guaranteed to
        // be minimal.  To achieve minimal packing, call 'compact' after all
        // modifications are complete.  The behavior is undefined unless
        // 0 <= 'dstIndex', 0 <= 'srcIndex', 0 <= 'numElements',
        // 'dstIndex' + 'numElements' <= length(), and
        // 'srcIndex' + 'numElements' <= srcArray.size().  Note that this
        // function is logically equivalent to (but more efficient than)
        //..
        //    remove(dstIndex, numElements);
        //    insert(dstIndex, srcArray, srcIndex, numElements);
        //..
        // except that problems resulting from aliasing are handled correctly.

    void resetElemTypes(const bdem_ElemType::Type     elementTypes[],
                        int                           numElements,
                        const bdem_Descriptor *const  attrLookupTbl[]);
        // Replace all the elements in the map with elements whose type values
        // are those of the specified 'srcArray' and whose offsets are
        // calculated.  'resetElemTypes' *guarantees* that the packing
        // described by the offsets is minimal.  There is no need to call
        // 'compact' following 'resetElemTypes'.  Note that this function is
        // logically equivalent to (but more efficient than):
        //..
        //    removeAll();
        //    append(srcArray);
        //    compact();
        //..

    void swap(int index1, int index2);
        // Exchange the values at the specified indices, 'index1' and 'index2'.
        // No other values will be affected and the packing described by the
        // offsets is also unaffected.  The behavior is undefined
        // unless 0 <= index1 < length() and 0 <= index2 < length().

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate at least the
        // specified 'numElements' values without reallocation.  The behavior
        // is undefined unless 0 <= numElements.  Note that if
        // numElements <= length(), this operation has no affect.  Note that
        // if an exception is thrown during reallocation, the value of the
        // map is left unchanged.

    void reserveCapacityRaw(int numElements);
        // Reserve *exactly* *enough* capacity to accommodate the specified
        // 'numElements' values without reallocation.  The behavior is
        // undefined unless 0 <= numElements.  Note that if
        // numElements <= length(), this operation has no affect.  Note that
        // if an exception is thrown during reallocation, the value of the
        // map is left unchanged.  Warning: Inappropriate use of this method
        // such as repeatedly calling
        //..
        //    reserveCapacityRaw(a.size() + 1);
        //    a.append(element);
        //..
        // may lead to poor runtime behavior.

    void updateTotalOffset();
        // Recalculate the total size of the packing described by the offsets
        // of this map.  When offsets are removed, the total offset is not
        // updated.  Note that offsets are not updated.

    template <class STREAM>
    STREAM&
    bdexStreamIn(STREAM&                      stream,
                 int                          version,
                 const bdem_Descriptor *const attrLookup[]);
        // Assign to this map the value read from the specified input 'stream'
        // If 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, this map is valid
        // but the number, types, and respective values of its elements are
        // unspecified.  Auxiliary information is provided by 'attrLookup',
        // which maps each known element type to a descriptor (see
        // bdem_descriptor).

    template <class STREAM>
    STREAM&
    streamIn(STREAM&                      stream,
             int                          version,
             const bdem_Descriptor *const attrLookup[]);
        // Assign to this map the value read from the specified input 'stream'
        // If 'stream' is initially invalid, this operation has no effect.  If
        // 'stream' becomes invalid during this operation, this map is valid
        // but the number, types, and respective values of its elements are
        // unspecified.  Auxiliary information is provided by 'attrLookup',
        // which maps each known element type to a descriptor (see
        // bdem_descriptor).
        //
        // DEPRECATED: Replaced by 'bdexStreamIn(stream, version, attrLookup)'

    // ACCESSORS
    void adjustedTotalOffset(int *total,
                             const bdem_Descriptor *itemType) const;
        // Determine the total size in bytes needed for an alignment-
        // optimized, contiguous memory layout of the items in this map
        // followed by an item of the specified 'itemType', and store it into
        // the specified 'total'.

    void adjustedTotalOffset(int                      *total,
                             const bdem_IdxOffsetMap&  original,
                             int                       srcIndex,
                             int                       numElements) const;
        // Determine the total size in bytes needed for an alignment-
        // optimized, contiguous memory layout of the items in this map
        // followed by the specified 'numElements' items in the specified
        // 'original' map beginning at the specified 'srcIndex', and store it
        // into the specified 'total'.

    void adjustedTotalOffset(
                        int                          *total,
                        const bdem_ElemType::Type     elementTypes[],
                        int                           numElements,
                        const bdem_Descriptor *const  attributesTable[]) const;
        // Determine the total size in bytes needed for an alignment-
        // optimized, contiguous memory layout of the items in this map
        // followed by the specified 'numElements' items of the types specified
        // by 'elementTypes' with attribute structure specified by
        // 'attributeTable', and store it into the specified 'total'.

    void insertionOffset(int                   *offset,
                         const bdem_Descriptor *elemAttr) const;
        // Determine the offset within an alignment-optimized, contiguous
        // memory layout at which an item of the specified 'elemAttr' type
        // would be placed if inserted anywhere in this map, and store it at
        // the specified 'offset'.

    int areElemTypesEqual(const bdem_IdxOffsetMap& other) const;
        // Return 1 if the element at every valid index in the specified
        // 'other' map is of the same type as the corresponding element in
        // this map (and vice versa), and otherwise 0.

    const bdem_IdxOffsetMapEntry& operator[](int index) const;
        // Return the entry at the specified 'index' in this map.

    int totalOffset() const;
        // Return the total size of the packing described by the offsets.

    int length() const;
        // Return the number of elements in this map.

    std::ostream& print(std::ostream& stream,
                        int           level,
                        int           spacesPerLevel) const;
        // Format the types and offsets of this map to the specified output
        // 'stream' at the (absolute value of the) specified indentation
        // 'level' using the specified 'spacesPerLevel' of indentation.  Making
        // 'level' negative suppresses indentation for the first line only.
        //  The behavior is undefined unless 0 <= spacesPerLevel.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the sequence of types of items in this map to the specified
        // output 'stream'.

    template <class STREAM>
    STREAM& streamOutRaw(STREAM& stream, int version) const;
        // Write the sequence of types of items in this map to the specified
        // output 'stream'.
        //
        // DEPRECATED: replaced by 'bdexStreamOut(stream, version)'
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                             // --------
                             // CREATORS
                             // --------

inline
bdem_IdxOffsetMap::bdem_IdxOffsetMap(bdema_Allocator *basicAllocator)
: d_map(basicAllocator)
, d_totalOffset(0)
{
}

inline
bdem_IdxOffsetMap::bdem_IdxOffsetMap(const InitialCapacity&  numElements,
                                     bdema_Allocator        *basicAllocator)
: d_map(basicAllocator)
, d_totalOffset(0)
{
    d_map.reserve(numElements.d_i);
}

inline
bdem_IdxOffsetMap::
               bdem_IdxOffsetMap(const bdem_ElemType::Type     elementTypes[],
                                 int                           numElements,
                                 const bdem_Descriptor *const  attrLookupTbl[],
                                 bdema_Allocator              *basicAllocator)
: d_map(basicAllocator)
, d_totalOffset(0)
{
    d_map.reserve(numElements);
    resetElemTypes(elementTypes, numElements, attrLookupTbl);
                                   // d_offsets, d_totalOffset set by compact()
}

inline
bdem_IdxOffsetMap::bdem_IdxOffsetMap(const bdem_IdxOffsetMap&  original,
                                     bdema_Allocator          *basicAllocator)
: d_map(original.d_map, basicAllocator)
, d_totalOffset(original.d_totalOffset)
{
}

inline
bdem_IdxOffsetMap::bdem_IdxOffsetMap(const bdem_IdxOffsetMap&  original,
                                     int                       srcIndex,
                                     int                       numElements,
                                     bdema_Allocator          *basicAllocator)
: d_map(basicAllocator)
, d_totalOffset(original.d_totalOffset)
{
    d_map.reserve(numElements);
    d_map.insert(d_map.begin(),
                 original.d_map.begin() + srcIndex,
                 original.d_map.begin() + srcIndex + numElements);
}

inline
bdem_IdxOffsetMap::~bdem_IdxOffsetMap()
{
}

                             // --------------------------------------------
                             // "Level 0" functions -- used by other inlines
                             // --------------------------------------------

inline
void bdem_IdxOffsetMap::insertionOffset(int                   *offset,
                                        const bdem_Descriptor *elemAttr) const
{
    int alignMinus1 = elemAttr->d_alignment - 1;
    *offset = (d_totalOffset + alignMinus1) & ~(alignMinus1);
}

                             // ------------
                             // MANIPULATORS
                             // ------------


inline
void bdem_IdxOffsetMap::append(const bdem_Descriptor *elemAttr)
{
    bdem_IdxOffsetMapEntry e;
    int offset;
    insertionOffset(&offset, elemAttr);  // calculate offset for this elem
    e.d_elemAttr = elemAttr;
    e.d_offset = offset;
    d_map.push_back(e);

    d_totalOffset = offset + elemAttr->d_size;
}

inline
void bdem_IdxOffsetMap::insert(int dstIndex, const bdem_Descriptor *elemAttr)
{
    bdem_IdxOffsetMapEntry e;
    int offset;
    insertionOffset(&offset, elemAttr);  // calculate offset for this elem
    e.d_elemAttr = elemAttr;
    e.d_offset = offset;
    d_map.insert(d_map.begin() + dstIndex, e);

    d_totalOffset = offset + elemAttr->d_size;
}

inline
void bdem_IdxOffsetMap::remove(int index)
{
    d_map.erase(d_map.begin() + index);
}

inline
void bdem_IdxOffsetMap::remove(int index, int numElements)
{
    d_map.erase(d_map.begin() + index, d_map.begin() + index + numElements);
}

inline
void bdem_IdxOffsetMap::removeAll()
{
    d_map.clear();
    d_totalOffset = 0;
}

inline
void bdem_IdxOffsetMap::replace(int dstIndex, const bdem_Descriptor *elemAttr)
{
    int offset;
    insertionOffset(&offset, elemAttr);  // calculate offset for this elem
    d_map[dstIndex].d_elemAttr = elemAttr;
    d_map[dstIndex].d_offset = offset;
    d_totalOffset = offset + elemAttr->d_size;
}

inline
void bdem_IdxOffsetMap::reserveCapacity(int numElements)
{
    d_map.reserve(numElements);
}

inline
void bdem_IdxOffsetMap::reserveCapacityRaw(int numElements)
{
    d_map.reserve(numElements);
}

inline
void bdem_IdxOffsetMap::swap(int index1, int index2)
{
    std::swap(d_map[index1], d_map[index2]);
}

template <class STREAM>
STREAM&
bdem_IdxOffsetMap::bdexStreamIn(STREAM&                      stream,
                                int                          version,
                                const bdem_Descriptor *const attrLookup[])
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 1: {
          int len;
          stream.getLength(len);
          if (!stream) {
              return stream;                                  // RETURN
          }

          removeAll();
          reserveCapacity(len);
          for(int index = 0; index < len; index++) {
              bdem_ElemType::Type elemType;
              bdem_ElemType::bdexStreamIn(stream, elemType, version);
              if (! stream) {
                  break;
              }
              append(attrLookup[elemType]);
          }

      } break;
      default: {
          stream.invalidate();
      }
    }

    return stream;
}

template <class STREAM>
inline
STREAM& bdem_IdxOffsetMap::streamIn(STREAM&                      stream,
                                    int                          version,
                                    const bdem_Descriptor *const attrLookup[])
{
    return bdexStreamIn(stream, version, attrLookup);
}
                             // ---------
                             // ACCESSORS
                             // ---------

inline
void bdem_IdxOffsetMap::adjustedTotalOffset(
                                         int                   *total,
                                         const bdem_Descriptor *elemAttr) const
{
    insertionOffset(total, elemAttr);  // calculate offset for this elem
    *total += elemAttr->d_size;
}

inline
int bdem_IdxOffsetMap::length() const
{
    return d_map.size();
}

template <class STREAM>
inline
STREAM& bdem_IdxOffsetMap::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        int len = d_map.size();
        stream.putLength(len);
        for(int index = 0; index < len; index++) {
            bdem_ElemType::bdexStreamOut(
                stream,
                (bdem_ElemType::Type) d_map[index].d_elemAttr->d_elemEnum,
                version);
        }
      } break;
    }

    return stream;
}

template <class STREAM>
inline
STREAM& bdem_IdxOffsetMap::streamOutRaw(STREAM& stream, int version) const
{
    return bdexStreamOut(stream, version);
}

inline
int bdem_IdxOffsetMap::totalOffset() const
{
    return d_totalOffset;
}

inline
const bdem_IdxOffsetMapEntry& bdem_IdxOffsetMap::operator[](int index) const
{
    return d_map[index];
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
