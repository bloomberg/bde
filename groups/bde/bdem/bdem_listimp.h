// bdem_listimp.h                                                     -*-C++-*-
#ifndef INCLUDED_BDEM_LISTIMP
#define INCLUDED_BDEM_LISTIMP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide implementation of a 'bdem' list.
//
//@CLASSES:
//  bdem_ListImp: heterogeneous sequence, managed by 'bdem_List' (q.v.)
//
//@SEE_ALSO: bdem_list
//
//@AUTHOR: Pablo Halpern (phalpern)    (originally by Dan Glaser)
//
//@DESCRIPTION: This component implements a 'bdem' list, a heterogeneous
// sequence of 'bdem' elements.  This component is *used* *by* the 'bdem_list'
// component.  Clients should use 'bdem_list', and should not use this
// component directly.
//
///'bdem' Null States
///------------------
// The concept of null applies to each 'bdem' type.  In addition to the range
// of values in a given 'bdem' type's domain (e.g., '[ INT_MIN .. INT_MAX ]'
// for 'BDEM_INT'), each type has a null value.  When a 'bdem' element is null,
// it has an underlying (unique) designated unset value (or state) as indicated
// in the following table:
//..
//       'bdem' element type                 "unset" value (or state)
//  ------------------------------    --------------------------------------
//  BDEM_CHOICE and
//  BDEM_CHOICE_ARRAY_ITEM            selector() < 0 && 0 == numSelections()
//
//  BDEM_CHOICE_ARRAY                 0 == length()  && 0 == numSelections()
//
//  BDEM_LIST                         0 == length()
//
//  BDEM_ROW                          All the elements in the row are "null"
//
//  BDEM_TABLE                        0 == numRows() && 0 == numColumns()
//
//  scalar (BDEM_INT, etc.)           bdetu_Unset<TYPE>::unsetValue()
//
//  vector (BDEM_INT_ARRAY, etc.)     0 == size()
//..
// To illustrate, consider a 'BDEM_BOOL' element within a 'bdem_List'.  The
// element can be in one of three possible states:
//: o null with underlying value 'bdetu_Unset<bool>::unsetValue()'
//: o non-null with underlying value 'false'
//: o non-null with underlying value 'true'
//
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdem_ListImp', 'myListImp', whose first
// element is of type 'BDEM_STRING':
//..
//  myListImp.rowData().elemRef(0).theModifiableString() = "HELLO";
//  assert(myListImp.rowData().isNonNull(0));
//..
// Making the element null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myListImp.rowData().makeNull(0);
//  assert(myListImp.rowData().isNull(0));
//  assert(myListImp.rowData().elemRef(0).theString().empty());
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATEOPTION
#include <bdem_aggregateoption.h>
#endif

#ifndef INCLUDED_BDEM_ALLOCATORMANAGER
#include <bdem_allocatormanager.h>
#endif

#ifndef INCLUDED_BDEM_DESCRIPTOR
#include <bdem_descriptor.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEM_ROWDATA
#include <bdem_rowdata.h>
#endif

#ifndef INCLUDED_BDEM_ROWLAYOUT
#include <bdem_rowlayout.h>
#endif

#ifndef INCLUDED_BDEIMP_BITWISECOPY
#include <bdeimp_bitwisecopy.h>
#endif

#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#include <bslalg_passthroughtrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                        // ==================
                        // class bdem_ListImp
                        // ==================

class bdem_ListImp {
    // This class implements a value-semantic, heterogeneous, indexable
    // sequence container of scalar, array, and aggregate values, managing a
    // single underlying 'bdem' row object, to which a reference may be readily
    // obtained.  A list can be configured, either by appending each element
    // (types and value) one at a time, or all at once by suppling a
    // 'bsl::vector' of 'bdem_ElemType::Type' objects and then assigning to
    // these elements their respective values, which, in some cases may lead to
    // a better packed, more space-efficient internal representation.  Note
    // however that the runtime cost of assigning array or aggregate values may
    // be prohibitive, which argues for populating such values in place.  This
    // class is *used* *by* the 'bdem_list' class implementation.  Clients
    // should use 'bdem_list', and should not use this class in place.

    // PRIVATE TYPES
    typedef bslalg_PassthroughTrait<bdem_RowData,
                                    bslalg_TypeTraitBitwiseMoveable> MoveTrait;
    // DATA
    // NOTE: Do *NOT* change the order of the data members.  The implementation
    // relies on them being declared in this order.
    bdem_AllocatorManager    d_allocatorManager;
                                            // object storing the allocation
                                            // strategy and custom allocator

    bdem_RowLayout          *d_rowLayout_p; // row layout (owned)

    bdem_RowData            *d_rowData_p;   // holds all items contributing to
                                            // size and alignment (owned)

    // FRIENDS
    friend bool operator==(const bdem_ListImp&, const bdem_ListImp&);

  private:
    // PRIVATE MANIPULATORS
    void init();
    void init(const bdem_ElemType::Type *elementTypes,
              int                        numElements,
              const bdem_Descriptor     *const *attrLookupTbl);
    void init(const bdem_RowData& srcRowData);
        // TBD: Doc

  private:
    // NOT IMPLEMENTED
    bdem_ListImp(const bdem_ListImp&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdem_ListImp,
                                  bslalg_TypeTraitUsesBslmaAllocator,
                                  MoveTrait);

    // TYPES
    class InitialMemory {
        // DATA
        int d_i;

      public:
        // CREATORS
        explicit InitialMemory(int i) : d_i(i) { }
        ~InitialMemory() { }

        // ACCESSORS
        operator int() const { return d_i; }
    };

    // PUBLIC CLASS DATA
    static const bdem_Descriptor d_listAttr;
        // Attributes of the 'bdem_ListImp' class.  Contains pointers to
        // functions for construction, destruction, assignment, equality test,
        // etc.

    // CLASS METHODS
    template <class STREAM>
    static void
    streamInList(void                                  *obj,
                 STREAM&                                stream,
                 int                                    version,
                 const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                 const bdem_Descriptor                 *const *attrLookup);
        // Call the 'bdexStreamInImp' method through a generic interface.
        // A pointer to an instantiation of this function is stored in the
        // streaming function lookup tables for each stream type.

    template <class STREAM>
    static void
    streamOutList(const void                             *obj,
                  STREAM&                                 stream,
                  int                                     version,
                  const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup);
        // Call the 'bdexStreamOutImp' method through a generic interface.  A
        // pointer to an instantiation of this function is stored in the
        // streaming function lookup tables for each stream type.

    // CREATORS
    explicit
    bdem_ListImp(bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_ListImp(bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 const InitialMemory&                      initialMemory,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create a list of length 0 using the specified memory allocation
        // strategy 'allocationStrategy'.  (The meanings of the various
        // 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  Optionally specify an 'initialMemorySize'
        // (in bytes) that will be preallocated in order to satisfy allocation
        // requests without replenishment (i.e., without internal allocation):
        // it has no effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless '0 <= initialMemorySize', if 'initialMemorySize' is
        // specified.

    bdem_ListImp(const bdem_ElemType::Type                *elementTypes,
                 int                                       numElements,
                 const bdem_Descriptor *const             *attrLookupTbl,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_ListImp(const bdem_ElemType::Type                *elementTypes,
                 int                                       numElements,
                 const bdem_Descriptor *const             *attrLookupTbl,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 const InitialMemory&                      initialMemory,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create a list having the the specified 'numElements', whose types
        // are the same as those in the specified 'elementTypes', with each
        // element set to null and having its corresponding unset value (see
        // 'bdetu_unset').  Optionally specify a memory allocation strategy
        // 'allocationStrategy'.  If 'allocationStrategy' is not specified,
        // then 'BDEM_PASS_THROUGH' is used.  (The meanings of the various
        // 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  If 'allocationStrategy' is specified,
        // optionally specify an 'initialMemorySize' (in bytes) that will be
        // preallocated in order to satisfy allocation requests without
        // replenishment (i.e., without internal allocation): it has no effect
        // unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'elementTypes' contains at least 'numElements'
        // types, 'attrLookupTbl' size is at least the maximum integer among
        // those represented by the 'bdem' element types contained by
        // 'columnTypes', and '0 <= initialMemorySize', if 'initialMemorySize'
        // is specified.

    bdem_ListImp(const bdem_RowData&                     original,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_ListImp(const bdem_ListImp&                       original,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_ListImp(const bdem_RowData   &                  original,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 const InitialMemory&                      initialMemory,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_ListImp(const bdem_ListImp&                       original,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 const InitialMemory&                      initialMemory,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create a list having the value of the specified 'original' object
        // (row or list).  Optionally specify a memory allocation strategy
        // 'allocationStrategy'.  If 'allocationStrategy' is not specified,
        // then 'BDEM_PASS_THROUGH' is used.  (The meanings of the various
        // 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  If 'allocationStrategy' is specified,
        // optionally specify an 'initialMemorySize' (in bytes) that will be
        // preallocated in order to satisfy allocation requests without
        // replenishment (i.e., without internal allocation): it has no effect
        // unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Also note that
        // implicit conversion of a 'bdem_Row' to a 'bdem_List' is deliberately
        // suppressed.  The behavior is undefined unless
        // '0 <= initialMemorySize', if 'initialMemorySize' is specified.

    ~bdem_ListImp();
        // Destroy this list object.  If the list object was constructed with
        // any memory allocation strategy mode other than 'BDEM_PASS_THROUGH',
        // then on individually contained elements are not invoked.  The
        // memory used by those elements will be released efficiently (all at
        // once) when the internal (managed) memory allocator is destroyed.

    // MANIPULATORS
    bdem_ListImp& operator=(const bdem_RowData& rhs);
    bdem_ListImp& operator=(const bdem_ListImp& rhs);
        // Assign, to this list, the value of the specified 'rhs' object, and
        // return a reference to this modifiable list.  Note that after the
        // assignment, both objects will have identical sequences of elements
        // (types and values).

    void reserveMemory(int numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // internal allocation).  The behavior is undefined unless
        // '0 <= numBytes'.  Note that this method has no effect unless the
        // internal allocation mode is 'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY'.

    bdem_RowData& rowData();
        // Return a reference to the modifiable heterogeneous row of element
        // values managed by this list.

    void *insertElementRaw(int                    dstIndex,
                           const bdem_Descriptor *elemAttr);
        // Insert an element of type indicated by the specified 'elemAttr' into
        // this list at the specified 'dstIndex', and return the address of the
        // newly-inserted modifiable element.  Each element having an index
        // greater than or equal to 'dstIndex' before the insertion is shifted
        // up by one index position.  The inserted element will be non-null.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.  Note
        // that the entry is not initialized to any value, including the
        // "unset" value.

    void *insertElement(int                    dstIndex,
                        const void            *value,
                        const bdem_Descriptor *elemAttr);
        // Insert an element of the specified 'value' of type indicated by the
        // the specified 'elemAttr' into this list at the specified 'dstIndex',
        // and return the address of the newly-inserted modifiable element.
        // Each element having an index greater than or equal to 'dstIndex'
        // before the insertion is shifted up by one index position.  The
        // inserted element will be non-null.  The behavior is undefined unless
        // the actual type of 'value' matches the type indicated by 'elemAttr'
        // and '0 <= dstIndex <= length()'.

    void insertElement(int                      dstIndex,
                       const bdem_ConstElemRef& srcElement);
        // Insert into this list, at the specified 'dstIndex', the value of the
        // specified by 'srcElement'.  Each element having an index greater
        // than or equal to 'dstIndex' before the insertion is shifted up by
        // one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.

    void insertElements(int                 dstIndex,
                        const bdem_RowData& srcRow,
                        int                 srcIndex,
                        int                 numElements);
        // Insert into this list, as individual elements beginning at the
        // specified 'dstIndex', the specified 'numElements' beginning at the
        // specified 'srcIndex' of 'srcRow'.  The inserted elements will be
        // non-null with each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by 'numElements' index
        // position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()', '0 <= srcIndex', '0 <= numElements',
        // and 'srcIndex + numElements <= srcRow.length()'.

    void *insertNullElement(int dstIndex, const bdem_Descriptor *elemAttr);
        // Insert a null element of type indicated by the specified 'elemAttr'
        // into this list at the specified 'dstIndex', and return the address
        // of the newly-inserted modifiable element.  Each element having an
        // index greater than or equal to 'dstIndex' before the insertion is
        // shifted up by one index position.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value will
        // be the corresponding unset value for its corresponding element type
        // (see 'bdetu_unset').

    void insertNullElements(int                        dstIndex,
                            const bdem_ElemType::Type *elementTypes,
                            int                        numElements,
                            const bdem_Descriptor     *const *attrLookupTbl);
        // Insert, into this list, as individual elements beginning at the
        // specified 'dstIndex', a sequence of null elements of the specified
        // 'length', having the specified 'elementTypes', and additionally
        // described by the specified 'attrLookupTbl'.  The behavior is
        // undefined unless '0 <= dstIndex <= length()', the number of items in
        // 'elementTypes' equals 'numElements', and the number of items in
        // 'attrLookupTbl' equals 'numElements'.  Note that, if accessed, the
        // values will be the corresponding unset values for their
        // corresponding element types (see 'bdetu_unset').

    bsl::string& insertString(int dstIndex, const char *value);
        // Insert into this list, at the specified 'dstIndex ', a non-null
        // element of type  'bdem_ElemType::BDEM_STRING' having the specified
        // 'value' , and return a reference to the newly-inserted modifiable
        // string.  Each element having an index greater than or equal to
        // 'dstIndex' before the insertion is shifted up by one index position.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    bdem_ListImp& insertList(int dstIndex, const bdem_RowData& srcRow);
        // Insert into this list at the specified 'dstIndex' a list consisting
        // of (copies of) the elements of the specified 'srcRow', and return a
        // reference to the newly-inserted modifiable list.  Each element
        // having an index greater than or equal to 'dstIndex' before the
        // insertion is shifted up by one index position.  The behavior is
        // undefined unless '0 <= dstIndex <= length()'.

    void removeElement(int index);
        // Remove the element at the specified 'index' from this list.  The
        // behavior is undefined unless '0 <= index < length()'.

    void removeElements(int startIndex, int numElements);
        // Remove from this list the specified 'numElements' beginning at the
        // specified 'startIndex'.  The behavior is undefined unless
        // '0 <= startIndex', '0 <= numElements', and
        // 'startIndex + numElements <= length()'.

    void removeAll();
        // Remove all of the elements from this list, leaving the list empty.

    void clear();
        // Remove all of the elements from this list, leaving the list empty.
        // Note that this method delegates to removeAll, which is needed for
        // 'bdem_functiontemplates'.

    void replaceElement(int index, const bdem_ConstElemRef& srcElem);
        // Replace the element (type and value) at the specified 'index' in
        // this list with (a copy of) the element specified by 'srcElem'.
        // The behavior is undefined unless '0 <= index < length()'.

    void resetElement(int index, const bdem_Descriptor *elemAttr);
        // Replace the element (type only) at the specified 'index' in this
        // list with a null element of type indicated by the specified
        // 'elemAttr'.  The behavior is undefined unless
        // '0 <= index < length()'.  Note that, if accessed, the value will be
        // the corresponding unset value for its corresponding element type
        // (see 'bdetu_unset').  Also note that, absent aliasing, the behavior
        // of this method is the same as:
        //..
        //  remove(index);
        //  insertNullElement(index, elemType);
        //..

    void reset(const bdem_ElemType::Type *elementTypes,
               int                        numElements,
               const bdem_Descriptor     *const *attrLookupTbl);
        // Remove all types and values from this list and append a sequence of
        // null elements having the specified 'length', each of the type
        // indicated by the appropriate entry in the specified 'elementTypes'
        // array, and additionally described by the specified 'attrLookupTbl'.
        // The behavior is undefined unless 'elementTypes' contains
        // 'numElements' types and the number of items in 'attrLookupTbl'
        // equals 'numElements'.  Note that, if accessed, the values will be
        // the corresponding unset values for its corresponding element types
        // (see 'bdetu_unset').  Also note that the behavior of this method is
        // the same as:
        //..
        //  removeAll();
        //  appendNullElements(srcTypes);
        //..

    void compact();
        // Recalculate the offsets of the sequence of elements contained in
        // this list in order to produce optimal packing.  The type and value
        // at each index is unaffected.  Note that this function potentially
        // alters the offset of every element in the list.  Note that the
        // imprudent use of this method such as repeatedly calling:
        //..
        //  a.insert( ... );
        //  a.compact();
        //..
        // may lead to poor runtime performance.

    void swap(bdem_ListImp& rhs);
        // Swap the value of this list with that of the specified 'other' list
        // in constant time.  The behavior is undefined unless 'other' has the
        // same allocator and allocation hint as this list.

    void swap(int index1, int index2);
        // Efficiently swap the elements (types and values) at the specified
        // 'index1' and 'index2' positions in this list.  The behavior is
        // undefined unless both 'index1' and 'index2' are in the range
        // '[ 0 .. length() - 1 ]'.  Note that this method never throws an
        // exception.

    template <class STREAM>
    STREAM&
    bdexStreamInImp(STREAM&                                stream,
                    int                                    version,
                    const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                    const bdem_Descriptor                 *const attrLookup[]);
        // Read the row definition and element values from the specified input
        // 'stream' and return a reference to the modifiable 'stream'.  The
        // previous value of '*this' is discarded.  The 'version' is mandatory
        // and indicates the expected input stream format.  'strmAttLookup'
        // maps each known element type to a corresponding function for
        // streaming in that type.  'attrLookup' maps each known element type
        // to a descriptor (see bdem_descriptor).
        //
        // If 'version' is not supported, 'stream' is marked invalid and list
        // is left unchanged.  If 'stream' is initially invalid, the list is
        // left unchanged.  If 'stream' becomes invalid during this operation,
        // 'stream' is marked invalid; list is valid, but its value is
        // unspecified.  Decoding will fail (incorrect data will be read
        // and/or the stream will become invalid) unless the types of objects
        // encoded in 'stream' match the sequence of element types in the
        // list's row definition and 'stream's version matches 'version'.
        //
        // Note that the type 'STREAM' must adhere to the protocol specified
        // by 'bdex_InStream' (see the package-group-level documentation for
        // more information on 'bdex' streaming of container types).

    // ACCESSORS
    int length() const;
        // Return the number of elements in this list.

    int size() const;
        // Return the number of elements in this list.  Note that this method
        // is needed for 'bdem_functiontemplates'.

    const bdem_RowData& rowData() const;
        // Return a reference to the non-modifiable heterogeneous row of
        // element values managed by this list.  Note that a
        // 'bdem_RowData   ' object does not permit the number or types of
        // its elements to be modified directly.

    bdem_ElemType::Type elemType(int index) const;
        // Return the type of the element at the specified 'index' in this
        // list.  The behavior is undefined unless '0 <= index < length()'.

    template <class STREAM>
    STREAM&
    bdexStreamOutImp(
                 STREAM&                                 stream,
                 int                                     version,
                 const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const;
        // Write the list's row definition and element values to the specified
        // output 'stream' and return a reference to the modifiable 'stream'.
        // The 'version' is mandatory and specifies the format of the output.
        // The 'version' is *not* written to the stream.  If 'version' is not
        // supported, 'stream' is left unmodified.  Auxiliary information is
        // provided by 'strmAttrLookup', which maps each known element type to
        // a corresponding function for streaming in that type.
        //
        // The type 'STREAM' must adhere to the 'bdex_OutStream' protocol (see
        // the package-group-level documentation for more information on
        // 'bdex' streaming of container types).

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // absolute value of the optionally specified indentation 'level' and
        // return a reference to 'stream'.  If 'level' is specified, optionally
        // specify 'spacesPerLevel', the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
bool operator==(const bdem_ListImp& lhs, const bdem_ListImp& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' lists have the same
    // value, and 'false' otherwise.  Two lists have the same value if they
    // have the same number of elements and corresponding elements at each
    // index position have the same type and value.

bool operator!=(const bdem_ListImp& lhs, const bdem_ListImp& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' lists do not have the
    // same value, and 'false' otherwise.  Two lists do not have the same
    // value if they do not have the same number of elements or there are
    // corresponding elements at some index position that do not have the same
    // type or value.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ------------------
                        // class bdem_ListImp
                        // ------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
int bdem_ListImp::length() const
{
    return d_rowLayout_p->length();
}

                        // -------------------
                        // All Other Functions
                        // -------------------

// CLASS METHODS
template <class STREAM>
inline
void bdem_ListImp::streamInList(
                     void                                  *obj,
                     STREAM&                                stream,
                     int                                    version,
                     const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const bdem_Descriptor                 *const attrLookup[])
{
    BSLS_ASSERT_SAFE(obj);

    static_cast<bdem_ListImp *>(obj)->bdexStreamInImp(stream,
                                                      version,
                                                      strmAttrLookup,
                                                      attrLookup);
}

template <class STREAM>
inline
void bdem_ListImp::streamOutList(
                        const void                             *obj,
                        STREAM&                                 stream,
                        int                                     version,
                        const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup)
{
    BSLS_ASSERT_SAFE(obj);

    static_cast<const bdem_ListImp *>(obj)->bdexStreamOutImp(stream,
                                                             version,
                                                             strmAttrLookup);
}

// MANIPULATORS
inline
bdem_ListImp& bdem_ListImp::operator=(const bdem_ListImp& rhs)
{
    if (this != &rhs) {
        *this = *rhs.d_rowData_p;
    }
    return *this;
}

inline
void bdem_ListImp::reserveMemory(int numBytes)
{
    BSLS_ASSERT_SAFE(0 <= numBytes);

    d_allocatorManager.reserveMemory(numBytes);
}

inline
void bdem_ListImp::removeElement(int index)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    d_rowData_p->removeElement(index);
    d_rowLayout_p->remove(index);
}

inline
void bdem_ListImp::removeElements(int startIndex, int numElements)
{
    BSLS_ASSERT_SAFE(0 <= startIndex);
    BSLS_ASSERT_SAFE(0 <= numElements);
    BSLS_ASSERT_SAFE(startIndex + numElements <= length());

    d_rowData_p->removeElements(startIndex, numElements);
    d_rowLayout_p->remove(startIndex, numElements);
}

inline
void bdem_ListImp::removeAll()
{
    d_rowData_p->removeElements(0, d_rowData_p->length());
    d_rowLayout_p->removeAll();
}

inline
void bdem_ListImp::clear()
{
    removeAll();
}

inline
void bdem_ListImp::resetElement(int index, const bdem_Descriptor *elemAttr)
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    removeElement(index);
    insertNullElement(index, elemAttr);
}

inline
void bdem_ListImp::reset(const bdem_ElemType::Type *elementTypes,
                         int                        numElements,
                         const bdem_Descriptor     *const *attrLookupTbl)
{
    BSLS_ASSERT_SAFE(0 <= numElements);

    removeAll();
    insertNullElements(0, elementTypes, numElements, attrLookupTbl);
}

inline
void bdem_ListImp::compact()
{
    bdem_ListImp tmpList(*this,
                         d_allocatorManager.allocationStrategy(),
                         d_allocatorManager.originalAllocator());
    bdeimp_BitwiseCopy<bdem_ListImp>::swap(this, &tmpList);
}

inline
void bdem_ListImp::swap(int index1, int index2)
{
    BSLS_ASSERT_SAFE(0 <= index1);
    BSLS_ASSERT_SAFE(     index1 < length());
    BSLS_ASSERT_SAFE(0 <= index2);
    BSLS_ASSERT_SAFE(     index2 < length());

    if (index1 != index2) {
        d_rowLayout_p->swap(index1, index2);

        d_rowData_p->swapElements(index1, index2);
    }
}

template <class STREAM>
STREAM& bdem_ListImp::bdexStreamInImp(
                     STREAM&                                stream,
                     int                                    version,
                     const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const bdem_Descriptor                 *const attrLookup[])
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 3:                                                   // FALL THROUGH
      case 2:                                                   // FALL THROUGH
      case 1: {
        bdem_RowLayout layout(d_allocatorManager.internalAllocator());
        layout.bdexStreamIn(stream, 1, attrLookup);
        if (!stream) {
            return stream;                                            // RETURN
        }
        d_rowData_p->reset();

        bdeimp_BitwiseCopy<bdem_RowLayout>::swap(d_rowLayout_p, &layout);

        bdem_RowData rowData(d_rowLayout_p,
                             d_allocatorManager.allocationStrategy(),
                             d_allocatorManager.internalAllocator());

        rowData.bdexStreamInImp(stream,
                                version,
                                strmAttrLookup,
                                attrLookup);

        bdeimp_BitwiseCopy<bdem_RowData>::swap(d_rowData_p, &rowData);
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}

// ACCESSORS
inline
int bdem_ListImp::size() const
{
    return length();
}

inline
bdem_RowData& bdem_ListImp::rowData()
{
    return *d_rowData_p;
}

inline
bdem_ElemType::Type bdem_ListImp::elemType(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return static_cast<bdem_ElemType::Type>(
                             (*d_rowLayout_p)[index].attributes()->d_elemEnum);
}

inline
const bdem_RowData& bdem_ListImp::rowData() const
{
    return *d_rowData_p;
}

template <class STREAM>
STREAM&
bdem_ListImp::bdexStreamOutImp(
                  STREAM&                                 stream,
                  int                                     version,
                  const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 3:                                                   // FALL THROUGH
      case 2:                                                   // FALL THROUGH
      case 1: {
        d_rowLayout_p->bdexStreamOut(stream, 1);
        d_rowData_p->bdexStreamOutImp(stream, version, strmAttrLookup);
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}

inline
bsl::ostream& bdem_ListImp::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    return d_rowData_p->printRow(stream, level, spacesPerLevel);
}

// FREE OPERATORS
inline
bool operator==(const bdem_ListImp& lhs, const bdem_ListImp& rhs)
{
    return *lhs.d_rowData_p == *rhs.d_rowData_p;
}

inline
bool operator!=(const bdem_ListImp& lhs, const bdem_ListImp& rhs)
{
    return !(lhs == rhs);
}

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
