// bdlmxxx_rowlayout.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLMXXX_ROWLAYOUT
#define INCLUDED_BDLMXXX_ROWLAYOUT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Specify the layout of 'bdem' elements in a contiguous memory block.
//
//@CLASSES:
//  bdlmxxx::RowLayoutEntry: container of 'bdlmxxx::Descriptor *' and offset attributes
//       bdlmxxx::RowLayout: map of indices to 'bdlmxxx::RowLayoutEntry' objects
//
//@AUTHOR: Dan Glaser, Bill Chapman
//
//@SEE_ALSO: bdlmxxx_aggregate
//
//@DESCRIPTION: This component provides a class, 'bdlmxxx::RowLayout', that
// specifies the layout of a sequence of 'bdem' elements in a contiguous
// memory segment where the types of the elements are specified by
// 'bdlmxxx::ElemType::Type' values.  The layout of each element is represented by
// a 'bdlmxxx::RowLayoutEntry' object, which has two fields: a pointer to a
// 'bdlmxxx::Descriptor' that describes the element, and an 'offset' that
// represents the offset into the contiguous segment where the element is to
// reside.  Of the fields in a 'bdlmxxx::Descriptor', only three are of direct
// relevance to a 'bdlmxxx::RowLayout':
//
//: o The 'bdlmxxx::ElemType::Type' value representing the type of the element.
//: o The size in bytes that the element is to occupy in the memory segment.
//: o The alignment requirement of the element.
//
// (Note that 'bdlmxxx::Descriptor' also contains function pointers for
// manipulating elements of the described type, but they are not used by this
// component.)
//
// The layout is guaranteed at all times to respect the alignment requirements
// of the elements to be contained in the segment, and elements are guaranteed
// not to overlap.  After the 'compact' method is called, the layout is
// guaranteed to be space-minimal, until the next manipulation of the layout.
// The elements are accessed through an index, though the offsets of the
// elements in the layout are not constrained to occur in the same ordering as
// the indices.  Note that 'bdlmxxx::RowLayout' specifies only the layout of
// hypothetical elements in a hypothetical memory segment; neither the elements
// nor the memory segment are manipulated by a 'bdlmxxx::RowLayout' object.
//
///PERFORMANCE
///-----------
// The following characterizes the performance of representative operations
// using big-oh notation, 'O[f(N, M)]', where the names 'N' and 'M' also refer
// to the number of respective elements in each container -- i.e., its
// 'length'.  In this table, 'N' and 'M' represent 'bdlmxxx::RowLayout' objects,
// and their lengths.
//..
//  Operation                Worst Case     Average Case
//  ---------                ----------     ------------
//  DEFAULT CTOR                O[1]
//  COPY CTOR(N)                O[N]
//  N.DTOR()                    O[1]
//  N.OP=(M)                    O[M]                        (assignment)
//
//  N.append(value)             O[N]           A[1]
//  N.insert(index, value)      O[N]
//  N.replace(index, value)     O[1]
//  N.remove(index)             O[N]
//
//  N.compact()                 O[N]
//  N.length()                  O[1]
//..
///Usage
///-----
// The following snippets of code illustrate how to create and use a row
// layout.
//..
//  enum {
//      CHAR  = bdlmxxx::ElemType::BDEM_CHAR,
//      SHORT = bdlmxxx::ElemType::BDEM_SHORT,
//      INT   = bdlmxxx::ElemType::BDEM_INT
//  };
//..
// Create some 'bdlmxxx::Descriptor's for the types we will use.  For the purposes
// of this example, we can leave the fields not used directly by
// 'bdlmxxx::RowLayout' as 0.
//..
//  const bdlmxxx::Descriptor CHAR_DESC = {
//      CHAR,
//      sizeof(char),
//      bsls::AlignmentOf<char>::VALUE,
//      0,0,0,0,0,0,0,0,0
//  };
//
//  const bdlmxxx::Descriptor SHORT_DESC = {
//      SHORT,
//      sizeof(short),
//      bsls::AlignmentOf<short>::VALUE,
//      0,0,0,0,0,0,0,0,0
//  };
//
//  const bdlmxxx::Descriptor INT_DESC = {
//      INT,
//      sizeof(int),
//      bsls::AlignmentOf<int>::VALUE,
//      0,0,0,0,0,0,0,0,0
//  };
//..
// First, create an empty row layout:
//..
//  bdlmxxx::RowLayout m;     assert(0 == m.length());
//..
// Add some elements to our layout by appending to the end:
//..
//  m.append(&CHAR_DESC);
//  assert( 1    == m.length());
//  assert(CHAR  == m[0].attributes()->d_elemEnum);
//  assert( 0    == m[0].offset());
//  assert( 1    == m.totalOffset());
//
//  m.append(&INT_DESC);
//  assert( 2    == m.length());
//  assert(INT   == m[1].attributes()->d_elemEnum);
//  assert( 4    == m[1].offset());
//  assert( 8    == m.totalOffset());
//
//  m.append(&INT_DESC);
//  assert( 3    == m.length());
//  assert(INT   == m[2].attributes()->d_elemEnum);
//  assert( 8    == m[2].offset());
//  assert(12    == m.totalOffset());
//
//  m.append(&SHORT_DESC);
//  assert( 4    == m.length());
//  assert(SHORT == m[3].attributes()->d_elemEnum);
//  assert(12    == m[3].offset());
//  assert(14    == m.totalOffset());
//..
// Note we can insert in the middle.  While according to the indexes, the new
// element is in the middle of the list, according to the offsets it was added
// onto the end.
//..
//  m.insert(2, &SHORT_DESC);
//  assert( 5    == m.length());
//  assert(SHORT == m[2].attributes()->d_elemEnum);
//  assert(14    == m[2].offset());
//  assert(16    == m.totalOffset());
//
//  assert(CHAR  == m[0].attributes()->d_elemEnum);
//  assert(INT   == m[1].attributes()->d_elemEnum);
//  assert(SHORT == m[2].attributes()->d_elemEnum);
//  assert(INT   == m[3].attributes()->d_elemEnum);
//  assert(SHORT == m[4].attributes()->d_elemEnum);
//..
// Let's print out the layout, which will give us "<TYPE> <OFFSET>" pairs
// describing the elements:
//..
//  m.print(cout, 0, -1) << endl;
//..
// And the console output is: "{ CHAR 0 INT 4 SHORT 14 INT 8 SHORT 12 }".
//..
//  m.remove(1);    // Remove element 1, which is of type 'INT'.
//..
// Print out the layout, showing that the first 'INT' is now gone, and that the
// offsets of all the other elements are unchanged:
//..
//  m.print(cout, 0, -1) << endl;
//..
// And the console output is: "{ CHAR 0 SHORT 14 INT 8 SHORT 12 }".
//..
//  assert( 4 == m.length());       // one shorter
//  assert(16 == m.totalOffset());  // unchanged
//..
// So our contiguous memory segment is taking 16 bytes to store
// '1 + 2 + 4 + 2 == 9' bytes of actual data -- not very efficient.  Let's run
// 'compact', which will sort the elements in the segment by non-increasing
// alignment requirements, but the type for each index will be unchanged.
//
// Note that adding elements to a 'bdlmxxx::RowLayout' is not analogous to adding
// types to a 'struct'.  The offset of the beginning of every element has to be
// aligned according to the alignment requirements of that element, but the
// total offset of the 'bdlmxxx::RowLayout', unlike the size of a 'struct', does
// not have to be a multiple of the largest alignment requirement of any
// element in the 'bdlmxxx::RowLayout'.
//..
//  m.compact();
//
//  assert(9 == m.totalOffset());   // now optimal
//  assert(4 == m.length());        // unchanged
//..
// Now print it out so we can see that the types are still occurring in the
// same order, and we can look at what happened to the offsets:
//..
//  m.print(cout, 0, -1) << endl;
//..
// And the console output is: "{ CHAR 8 SHORT 6 INT 0 SHORT 4 }".  Observe that
// 'compact' left the sequence of types unchanged, but the offsets are
// substantially changed, and the ordering of the elements in the segment was
// rearranged.
//
// Note that specifying an index outside of the valid range will result in
// undefined behavior.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_DESCRIPTOR
#include <bdlmxxx_descriptor.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>      // for 'swap'
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>        // for 'memcpy'
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace bdlmxxx {
                        // =========================
                        // class RowLayoutEntry
                        // =========================

class RowLayoutEntry {
    // An object of this class represents one entry in a 'RowLayout'
    // describing the type of an element to be stored at a given index.  It
    // consists of a descriptor pointer and an offset.  The descriptor pointer
    // is used in this component to access the 'ElemType::Type' of the
    // element, the size of the element, and the alignment requirement of the
    // element.  The offset indicates the distance in bytes from the beginning
    // of a hypothetical memory segment to the hypothetical element represented
    // by the entry.  Note that within the 'compact' method of the
    // 'RowLayout' ('friend') class, the 'd_offset' data member is
    // temporarily used to construct a linked list of all entries that share a
    // given alignment requirement.

    // DATA
    const Descriptor *d_elementAttributes_p;  // characterizing properties
                                                   // of element

    int                    d_offset;               // offset (in bytes) of
                                                   // element in contiguous
                                                   // memory block

    // FRIENDS
    friend class RowLayout;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(RowLayoutEntry,
                                 bslalg::TypeTraitBitwiseCopyable);

  private:
    // PRIVATE MANIPULATORS
    int& offset();
        // Return a reference to the modifiable offset associated with this row
        // layout entry.

  public:
    // CREATORS
    RowLayoutEntry(const Descriptor *elementAttributes, int offset);
        // Create a row layout entry with its element descriptor and offset
        // attributes set to the specified 'elementAttributes' and 'offset'
        // values, respectively.  The behavior is undefined unless
        // 'elementAttributes' is non-null and '0 <= offset'.

    RowLayoutEntry(const RowLayoutEntry& original);
        // Create a row layout entry having the value of the specified
        // 'original' row layout entry.

    // There are no manipulators; the data members can be modified directly by
    // friend class 'RowLayout'.

    // ACCESSORS
    const Descriptor *attributes() const;
        // Return the address of the non-modifiable descriptor associated with
        // this row layout entry.

    int offset() const;
        // Return the offset associated with this row layout entry.
};

                        // ====================
                        // class RowLayout
                        // ====================

class RowLayout {
    // This class implements a mechanism for defining an efficient layout for
    // storing 'bdem' elements of varying type in a contiguous memory segment.
    // Each element is represented in the layout by a 'RowLayoutEntry'
    // object, each of which consists of a pointer to a 'Descriptor' that
    // describes the type, size, and alignment requirement, and an offset into
    // the contiguous memory segment where the element is to reside.  The class
    // guarantees that at all times the offsets for the elements will be
    // properly aligned and non-overlapping.  It accomplishes this by obtaining
    // the size and alignment information from the 'Descriptor'.  After
    // calling the 'compact' method, the layout is guaranteed to occupy a
    // minimal amount of space in the segment.
    //
    // This class is not value-semantic.  The capacity is clearly not part of
    // its value, nor are the specific offsets.  This class does not support
    // equality comparison, but it does support some other *value* *semantic*
    // operations, including copy construction, assignment, 'ostream' printing,
    // and 'bdex' serialization.  This mechanism is *exception* *neutral* with
    // no guarantee of rollback: If an exception is thrown during the
    // invocation of a method on a pre-existing object, the object is left
    // in a valid state, but its value is undefined.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) is supported in all cases.

    // DATA
    bsl::vector<RowLayoutEntry>
                           d_entries;      // entries describing the layout

    int                    d_totalOffset;  // total space described by offsets

  public:
    // TYPES
    struct InitialCapacity {
        // Enable uniform use of an optional integral constructor argument to
        // specify the initial internal capacity (in elements).  For example,
        //..
        //  RowLayout m(RowLayout::InitialCapacity(8));
        //..
        // defines an object 'm' with an initial capacity of 8 elements, but
        // with a logical length of 0 elements.

        int d_i;
        explicit InitialCapacity(int i) : d_i(i) { }
        ~InitialCapacity() { }
    };

    // CREATORS
    explicit
    RowLayout(bslma::Allocator       *basicAllocator = 0);
    explicit
    RowLayout(const InitialCapacity&  numElements,
                   bslma::Allocator       *basicAllocator = 0);
        // Create an empty row layout.  Optionally specify 'numElements' of
        // initial capacity, to create a row layout with sufficient initial
        // capacity to accommodate up to the specified 'numElements' values
        // without subsequent reallocation.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= numElements.d_i'.

    RowLayout(const ElemType::Type     elementTypes[],
                   int                           numElements,
                   const Descriptor *const  attributesTable[],
                   bslma::Allocator             *basicAllocator = 0);
        // Create a row layout having the specified 'numElements' of the
        // specified 'elementTypes' using the specified 'attributesTable' to
        // associate the appropriate descriptor for a type.  The offset of each
        // layout element is calculated.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.  The behavior is undefined
        // unless '0 <= numElements' and the length of 'elementTypes' is at
        // least 'numElements'.

    RowLayout(const RowLayout&  srcRowLayout,
                   int                    srcIndex,
                   int                    numElements,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a row layout with each element's values initialized to the
        // specified 'numElements' values of the specified 'srcRowLayout',
        // starting with the entry at the specified 'srcIndex'.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless '0 <= srcIndex',
        // '0 <= numElements', and
        // 'srcIndex + numElements <= srcRowLayout.length()'.

    RowLayout(const RowLayout&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a row layout with each element's type and offset values
        // initialized to the values of the specified 'original' row layout.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~RowLayout();
        // Destroy this row layout.

    // MANIPULATORS
    RowLayout& operator=(const RowLayout& rhs);
        // Assign to this row layout the value of the specified 'rhs' row
        // layout, and return a reference to this modifiable row layout.

    void append(const Descriptor *elementAttributes);
        // Append to this row layout an element described by the specified
        // 'elementAttributes'.  Assign the new element the lowest offset at or
        // after the end of the current last element that satisfies its
        // alignment requirement.  Appending an element will not alter the
        // offset of any other element in the layout; the resulting packing
        // described by the offsets is *not* guaranteed to be minimal.  To
        // achieve minimal packing, call 'compact' after all modifications are
        // complete.  The behavior is undefined unless 'elementAttributes' is
        // non-null.  Note that this function is logically equivalent to the
        // following:
        //..
        //  insert(length(), elementAttributes);
        //..

    void append(const ElemType::Type    elementTypes[],
                int                          numElements,
                const Descriptor *const attributesTable[]);
        // Append to this row layout the specified 'numElements' of the
        // specified 'elementTypes' using the specified 'attributesTable' to
        // associate the appropriate descriptor for a type.  Assign the new
        // elements the minimum offsets at or after the end of the current last
        // element that satisfy the elements's respective alignment
        // requirements.  Appending elements will not alter the offset of any
        // other element in the layout; the resulting packing described by the
        // offsets is *not* guaranteed to be minimal.  To achieve minimal
        // packing, call 'compact' after all modifications are complete.  The
        // behavior is undefined unless '0 <= numElements' and the length
        // of 'elementTypes' is at least 'numElements'.  Note that this
        // function is logically equivalent to the following:
        //..
        //  insert(length(), srcArray);
        //..

    void append(const ElemType::Type    elementTypes[],
                int                          numElements,
                int                          srcIndex,
                const Descriptor *const attributesTable[]);
        // Append to this row layout 'numElements' of the specified
        // 'elementTypes', beginning at the specified 'srcIndex', with the
        // minimal offsets at or after the end of the record that satisfy their
        // respective alignment requirements, using the specified
        // 'attributesTable' to associate the appropriate descriptor for a
        // type.  Appending elements will not alter the offset of any other
        // element in the layout; the resulting packing described by the
        // offsets is *not* guaranteed to be minimal.  To achieve minimal
        // packing, call 'compact' after all modifications are complete.  The
        // behavior is undefined unless '0 <= srcIndex', '0 <= numElements',
        // and the length of 'elementTypes' is at least
        // 'srcIndex + numElements'.  Note that this function is logically
        // equivalent to the following:
        //..
        //  insert(length(), srcArray, srcIndex, numElements);
        //..

    void compact();
        // Recalculate the offsets of the sequence of elements contained in
        // this row layout in order to produce minimal packing.  The type value
        // at each index is unaffected.  Note that this function potentially
        // alters the offset of every element in the layout.  Warning: this is
        // a relatively expensive operation.  Clients are encouraged to avoid
        // calling this method frequently, but rather to call it once at the
        // end of manipulation.

    void insert(int dstIndex, const Descriptor *elementAttributes);
        // Insert into this row layout, at the specified 'dstIndex', an element
        // whose type is described by the specified 'elementAttributes' and
        // assign the element the minimal offset at or after the end of the
        // current last element that satisfies the type's alignment
        // requirement.  All elements with initial indices at or above
        // 'dstIndex' are shifted up by one index position.  Inserting an
        // element will not alter the offset of any other element in the
        // layout; the resulting packing described by the offsets is *not*
        // guaranteed to be minimal.  To achieve minimal packing, call
        // 'compact' after all modifications are complete.  The behavior is
        // undefined unless '0 <= dstIndex <= length()' and 'elementAttributes'
        // is non-null.

    void insert(int                   dstIndex,
                const RowLayout& srcRowLayout,
                int                   srcIndex,
                int                   numElements);
        // Insert into this row layout, at the specified 'dstIndex', the
        // specified 'numElements' starting at the specified 'srcIndex' in the
        // specified 'srcRowLayout'.  Assign the new elements the minimal
        // offset at or after the end of the current last element that
        // satisfies their alignment requirements.  All elements with initial
        // indices at or above 'dstIndex' are shifted up by 'numElements'
        // positions.  Inserting an element will not alter the offset of any
        // other element in the layout; the resulting packing described by the
        // offsets is *not* guaranteed to be minimal.  To achieve minimal
        // packing, call 'compact' after all modifications are complete.  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numElements', and
        // 'srcIndex + numElements <= srcRowLayout.length()'.

    void insert(int                          dstIndex,
                const ElemType::Type    elementTypes[],
                int                          numElements,
                const Descriptor *const attributesTable[]);
        // Insert into this row layout, at the specified 'dstIndex', the
        // specified 'numElements' having the specified 'elementTypes' and
        // using the specified 'attributesTable' to associate the appropriate
        // descriptor for a type.  Assign the new elements the minimal offsets
        // at or after the end of the current last element that meet the
        // respect elements's alignment requirements.  All elements with
        // initial indices at or above 'dstIndex' are shifted up by
        // 'numElement' index positions.  Inserting elements will not alter the
        // offset of any other element in the layout; the resulting packing
        // described by the offsets is *not* guaranteed to be minimal.  To
        // achieve minimal packing, call 'compact' after all modifications are
        // complete.  The behavior is undefined unless
        // '0 <= dstIndex <= length()' and the length of 'elementTypes' is at
        // least 'numElements'.

    void remove(int index);
        // Remove from this row layout the element at the specified 'index'.
        // All elements with initial indices above 'index' are shifted down by
        // one index position.  Removing an element will not alter the offset
        // of any other element in the layout; the resulting packing described
        // by the offsets is *not* guaranteed or even likely to be minimal.  To
        // achieve minimal packing, call 'compact' after all modifications are
        // complete.  The behavior is undefined unless '0 <= index < length()'.

    void remove(int index, int numElements);
        // Remove from this row layout, beginning at the specified 'index', the
        // specified 'numElements'.  All elements with initial indices at or
        // above 'index + numElements' are shifted down by 'numElements' index
        // positions.  Removing elements will not alter the offset of any other
        // element in the layout.  However, the packing described by the
        // offsets is *not* guaranteed or even likely to be minimal.  To
        // achieve minimal packing, call 'compact' after all modifications are
        // complete.  The behavior is undefined unless '0 <= index',
        // '0 <= numElements', and 'index + numElements <= length()'.

    void removeAll();
        // Remove all elements from this row layout.  Note that after calling
        // this method, 'length' will return 0.

    void replace(int                    dstIndex,
                 const Descriptor *elementAttributes);
        // Replace the element at the specified 'dstIndex' in this row layout
        // with one whose type is described by 'elementAttributes' and assign
        // this element the minimal offset at or after the end of the current
        // last element that satisfies the alignment requirements of the new
        // type.  Replacing an element will not alter the offset of any other
        // element in the layout; the resulting packing described by the
        // offsets is *not* guaranteed to be minimal.  To achieve minimal
        // packing, call 'compact' after all modifications are complete.  The
        // behavior is undefined unless '0 <= dstIndex < length()' and
        // 'elementAttributes' is non-null.  Note that this function is
        // logically equivalent to (but more efficient than) the following:
        //..
        //  remove(dstIndex);
        //  insert(dstIndex, item);
        //..

    void replace(int                          dstIndex,
                 const ElemType::Type    elementTypes[],
                 int                          numElements,
                 const Descriptor *const attributesTable[]);
        // Replace the specified 'numElements' beginning at the specified
        // 'dstIndex' in this row layout with elements having the specified
        // 'elementTypes', and using the specified 'attributesTable' to
        // associate the descriptor for a type.  Assign the elements the
        // minimal offsets at or after the end of the current last element.
        // Replacing elements will not alter the offset of any other element in
        // the layout; the resulting packing described by the offsets is *not*
        // guaranteed to be minimal.  To achieve minimal packing, call
        // 'compact' after all modifications are complete.  The behavior is
        // undefined unless '0 <= dstIndex', '0 <= numElements',
        // 'dstIndex + numElements <= length()', and the length of
        // 'elementTypes' is at least 'numElements'.

    void replace(int                                        dstIndex,
                 const bsl::vector<const Descriptor*>& srcArray,
                 int                                        srcIndex,
                 int                                        numElements);
        // Replace the specified 'numElements' beginning at the specified
        // 'dstIndex' in this row layout with element types beginning at the
        // specified 'srcIndex' in the specified 'srcArray'.  Assign the new
        // elements the minimal offsets at or after the end of the current last
        // element that satisfy the alignment requirements of the respective
        // elements.  Replacing elements will not alter the offset of any other
        // element in the layout; the resulting packing described by the
        // offsets is *not* guaranteed to be minimal.  To achieve minimal
        // packing, call 'compact' after all modifications are complete.  The
        // behavior is undefined unless '0 <= dstIndex', '0 <= srcIndex',
        // '0 <= numElements', 'dstIndex + numElements <= length()', and
        // 'srcIndex + numElements <= srcArray.size()'.  Note that this
        // function is logically equivalent to (but more efficient than) the
        // following:
        //..
        //  remove(dstIndex, numElements);
        //  insert(dstIndex, srcArray, srcIndex, numElements);
        //..

    void resetElemTypes(const ElemType::Type    elementTypes[],
                        int                          numElements,
                        const Descriptor *const attributesTable[]);
        // Replace the elements in this row layout with the specified
        // 'numElements' of the specified 'elementTypes' using the specified
        // 'attributesTable' to associate a descriptor for a type.  Assign the
        // new elements the minimal offsets at or after the end of the current
        // last element that meet the alignment requirements of the respective
        // elements.  The 'Descriptor' associated with the 'i'th element,
        // where '0 <= i < numElements', is specified by
        // 'attributesTable[elementTypes[i]]'.  This method *guarantees* that
        // the packing described by the offsets is minimal, so there is no need
        // to call 'compact' following 'resetElemTypes'.  The behavior is
        // undefined unless '0 <= numElements' and the length of 'elementTypes'
        // is at least 'numElements'.  Note that this function is logically
        // equivalent to (but more efficient than) the following:
        //..
        //  removeAll();
        //  append(elementTypes, numElements, attributesTable);
        //  compact();
        //..

    void swap(int index1, int index2);
        // Exchange the values at the specified 'index1' and 'index2' positions
        // in this row layout.  No other values will be affected and the
        // packing described by the offsets is also unaffected.  The behavior
        // is undefined unless '0 <= index1 < length()' and
        // '0 <= index2 < length()'.

    void reserveCapacity(int numElements);
        // Reserve sufficient internal capacity to accommodate at least the
        // specified 'numElements' values without reallocation.  The behavior
        // is undefined unless '0 <= numElements'.  Note that if
        // 'numElements <= length()', this operation has no effect.  Also note
        // that if an exception is thrown during reallocation, the value of the
        // layout is left unchanged.

    void reserveCapacityRaw(int numElements);
        // Reserve *exactly* *enough* capacity to accommodate the specified
        // 'numElements' values without reallocation.  The behavior is
        // undefined unless '0 <= numElements'.  Note that if
        // 'numElements <= length()', this operation has no effect.  Also note
        // that if an exception is thrown during reallocation, the value of the
        // layout is left unchanged.  Warning: Inappropriate use of this method
        // such as repeatedly calling:
        //..
        //  reserveCapacityRaw(a.size() + 1);
        //  a.append(element);
        //..
        // may lead to poor runtime behavior.

    template <class STREAM>
    STREAM&
    bdexStreamIn(STREAM&                      stream,
                 int                          version,
                 const Descriptor *const attrLookup[]);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Auxiliary information is provided by
        // 'attrLookup', which maps each known element type to a descriptor
        // (see bdlmxxx_descriptor).  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    // ACCESSORS
    int insertionOffset(const Descriptor *elementAttributes) const;
        // Return the alignment-appropriate offset at which an element
        // specified by the 'elementAttributes' would be added by an 'append'
        // or 'insert' call.  The behavior is undefined unless
        // 'elementAttributes' is non-null.

    int adjustedTotalOffset(const Descriptor *elementAttributes) const;
        // Return the total size in bytes needed for the memory layout once an
        // element described by the specified 'elementAttributes' is appended
        // at an alignment-appropriate offset.  The behavior is undefined
        // unless 'elementAttributes' is non-null.

    int adjustedTotalOffset(const RowLayout& srcRowLayout,
                            int                   srcIndex,
                            int                   numElements) const;
        // Return the total size in bytes of the contiguous memory layout if
        // the specified 'numElements' of the specified 'srcRowLayout' starting
        // at the specified 'srcIndex' were to be appended in the order
        // they occur in 'srcRowLayout'.  The behavior is undefined unless
        // '0 <= srcIndex', '0 <= numElements', and
        // 'srcIndex + numElements <= srcRowLayout.length()'.

    int adjustedTotalOffset(
                         const ElemType::Type    elementTypes[],
                         int                          numElements,
                         const Descriptor *const attributesTable[]) const;
        // Return the total size in bytes of the contiguous memory layout
        // described by this row layout if the specified 'numElements' of types
        // indicated in the specified array 'elementTypes', with attributes
        // indicated in the specified 'attributesTable', were appended to this
        // object.  The 'Descriptor' associated with the 'i'th element,
        // where '0 <= i < numElements', is specified by
        // 'attributesTable[elementTypes[i]]'.  The behavior is undefined
        // unless '0 <= numElements' and the length of 'elementTypes' is at
        // least 'numElements'.

    bool areElemTypesEqual(const RowLayout& other) const;
        // Return 'true' if the specified 'other' layout has the same number of
        // elements as this row layout, and corresponding elements at each
        // valid index position in the two layouts have the same type; return
        // 'false' otherwise.  Note that element offsets are not compared.

    const RowLayoutEntry& operator[](int index) const;
        // Return the entry at the specified 'index' in this row layout.  The
        // behavior is undefined unless '0 <= index < length()'.

    int totalOffset() const;
        // Return the total size of the packing described by the offsets in
        // this row layout.  Note that calling 'remove' on elements does not
        // affect this value unless 'remove' is followed by a call to
        // 'compact'.

    int length() const;
        // Return the number of elements in this row layout.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this row layout object to the specified output 'stream' at
        // the (absolute value of) the optionally specified indentation 'level'
        // and return a reference to the modifiable 'stream'.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', the number of spaces
        // per indentation level for this object.  If 'level' is negative,
        // suppress indentation of the first line.  If 'stream' is not valid on
        // entry, this operation has no effect.  If 'spacesPerLevel < 0', it
        // will print all the information without any '\n's.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is invalidated.
        // Note that 'version' is not written to 'stream'.  See the 'bdex'
        // package-level documentation for more information on 'bdex' streaming
        // of value-semantic types and containers.  Note that the offsets of
        // the elements are not streamed out.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                      // --------------------------
                      // class RowLayoutEntry
                      // --------------------------

// PRIVATE MANIPULATORS
inline
int& RowLayoutEntry::offset()
{
    return d_offset;
}

// CREATORS
inline
RowLayoutEntry::RowLayoutEntry(
                                      const Descriptor *elementAttributes,
                                      int                    offset)
: d_elementAttributes_p(elementAttributes)
, d_offset(offset)
{
    BSLS_ASSERT_SAFE(elementAttributes);
    BSLS_ASSERT_SAFE(0 <= offset);
}

inline
RowLayoutEntry::RowLayoutEntry(const RowLayoutEntry& original)
: d_elementAttributes_p(original.d_elementAttributes_p)
, d_offset(original.d_offset)
{
}

// ACCESSORS
inline
const Descriptor *RowLayoutEntry::attributes() const
{
    return d_elementAttributes_p;
}

inline
int RowLayoutEntry::offset() const
{
    return d_offset;
}

                        // --------------------
                        // class RowLayout
                        // --------------------

// CREATORS
inline
RowLayout::RowLayout(bslma::Allocator *basicAllocator)
: d_entries(basicAllocator)
, d_totalOffset(0)
{
}

inline
RowLayout::RowLayout(const RowLayout&  original,
                               bslma::Allocator      *basicAllocator)
: d_entries(original.d_entries, basicAllocator)
, d_totalOffset(original.d_totalOffset)
{
}

inline
RowLayout::~RowLayout()
{
}

// MANIPULATORS
inline
void RowLayout::remove(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE((unsigned) index < d_entries.size());

    d_entries.erase(d_entries.begin() + index);
}

inline
void RowLayout::remove(int index, int numElements)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE((unsigned) (index + numElements) <= d_entries.size());

    d_entries.erase(d_entries.begin() + index,
                    d_entries.begin() + index + numElements);
}

inline
void RowLayout::removeAll()
{
    d_entries.clear();
    d_totalOffset = 0;
}

inline
void RowLayout::reserveCapacity(int numElements)
{
    BSLS_ASSERT_SAFE(0 <= numElements);

    d_entries.reserve(numElements);
}

inline
void RowLayout::reserveCapacityRaw(int numElements)
{
    BSLS_ASSERT_SAFE(0 <= numElements);

    d_entries.reserve(numElements);
}

inline
void RowLayout::swap(int index1, int index2)
{
    BSLS_ASSERT_SAFE(0 <= index1);
    BSLS_ASSERT_SAFE((unsigned) index1 < d_entries.size());
    BSLS_ASSERT_SAFE(0 <= index2);
    BSLS_ASSERT_SAFE((unsigned) index2 < d_entries.size());

    bsl::swap(d_entries[index1], d_entries[index2]);
}

template <class STREAM>
STREAM&
RowLayout::bdexStreamIn(STREAM&                      stream,
                             int                          version,
                             const Descriptor *const attrLookup[])
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 1: {
        int len;
        stream.getLength(len);
        if (!stream) {
            return stream;                                            // RETURN
        }

        removeAll();
        reserveCapacity(len);
        for (int index = 0; index < len; ++index) {
            ElemType::Type elemType = ElemType::BDEM_VOID;
            ElemType::bdexStreamIn(stream, elemType, version);
            if (!stream) {
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

// ACCESSORS
inline
int RowLayout::insertionOffset(
                                const Descriptor *elementAttributes) const
{
    BSLS_ASSERT_SAFE(elementAttributes);

    // Return 'd_totalOffset' rounded up to the next multiple of the alignment
    // requirement specified by 'elementAttributes'.

    const int alignMinus1 = elementAttributes->d_alignment - 1;
    return (d_totalOffset + alignMinus1) & ~alignMinus1;
}

inline
int RowLayout::adjustedTotalOffset(
                                const Descriptor *elementAttributes) const
{
    BSLS_ASSERT_SAFE(elementAttributes);

    return insertionOffset(elementAttributes) + elementAttributes->d_size;
}

inline
int RowLayout::length() const
{
    return static_cast<int>(d_entries.size());
}

template <class STREAM>
STREAM& RowLayout::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        const int len = static_cast<int>(d_entries.size());
        stream.putLength(len);
        for (int index = 0; index < len; ++index) {
            ElemType::bdexStreamOut(
                stream,
                (ElemType::Type)d_entries[index].attributes()->d_elemEnum,
                version);
        }
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}

inline
int RowLayout::totalOffset() const
{
    return d_totalOffset;
}

inline
const RowLayoutEntry& RowLayout::operator[](int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE((unsigned) index < d_entries.size());

    return d_entries[index];
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
