// bdlmxxx_rowdata.h                                                     -*-C++-*-
#ifndef INCLUDED_BDLMXXX_ROWDATA
#define INCLUDED_BDLMXXX_ROWDATA

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a container of data for a 'bdem' row.
//
//@CLASSES:
//  bdlmxxx::RowData: container for 'bdlmxxx::Row' data
//
//@SEE_ALSO: bdlmxxx_row, bdlmxxx_rowlayout, bdlmxxx_listimp, bdlmxxx_tableimp
//
//@AUTHOR: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a class, 'bdlmxxx::RowData', that manages
// the creation, alteration, and destruction of all data defining a 'bdlmxxx::Row'.
// A row is a heterogeneous, indexable sequence of scalars, arrays, and/or
// other aggregate values.  Clients creating a row will typically need to
// create a row layout first (i.e., a 'bdlmxxx::RowLayout' object), which
// prescribes the types of the elements to be contained by the row.  Using the
// row layout, clients can then create 'bdlmxxx::RowData' objects that contain the
// data corresponding to that row layout.
//
// Note that this component is meant for *internal* *use* *only*.  It should
// *not* be used by client code outside of the 'bdem' package.  In particular,
// the implementation of 'bdlmxxx::RowData' is highly collaborative with that of
// 'bdlmxxx::ListImp' and 'bdlmxxx::TableImp' (especially the former).  See the
// function-level documentation of the "insert" and "remove" manipulators for
// details.
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
//  scalar (BDEM_INT, etc.)           bdltuxxx::Unset<TYPE>::unsetValue()
//
//  vector (BDEM_INT_ARRAY, etc.)     0 == size()
//..
// To illustrate, consider a 'BDEM_BOOL' element within a 'bdlmxxx::RowData'.
// The element can be in one of three possible states:
//..
//  * null with underlying value 'bdltuxxx::Unset<bool>::unsetValue()'
//  * non-null with underlying value 'false'
//  * non-null with underlying value 'true'
//..
// The underlying value of a null 'bdem' object is a class invariant.  If an
// object is null, that object *also* has the unset value corresponding to its
// type.
//
// For example, suppose we have a 'bdlmxxx::RowData', 'myRow', whose first
// element is of type 'BDEM_STRING':
//..
//  myRow.elemRef(0).theModifiableString(0) = "HELLO";
//  assert(myRow.elemRef(0).isNonNull());
//..
// Making the element null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myRow.elemRef(0).makeNull();
//  assert(myRow.elemRef(0).isNull());
//  assert(myRow.elemRef(0).asString().empty());
//..
///'bdlmxxx::RowData' Nullability
///- - - - - - - - - - - - -
// Though a row can contain null elements (each element has "nullability
// information" associated with it), the row itself can be null only in
// relation to another object (its parent) in which it is a member.  The
// nullness of an element in the row can be queried via a 'bdlmxxx::ElemRef'
// returned through the 'elemRef' accessor to determine whether its
// contained object is null, e.g., 'row.elemRef(0).isNull()'.
//
///Usage
///-----
// This component is used exclusively by 'bdlmxxx_listimp' and 'bdlmxxx_tableimp' and
// should *not* be used directly by clients.  Therefore, a usage example is not
// provided.  Please refer to the 'bdlmxxx_listimp' and 'bdlmxxx_tableimp' components
// for guidance on usage.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_AGGREGATEOPTION
#include <bdlmxxx_aggregateoption.h>
#endif

#ifndef INCLUDED_BDLMXXX_DESCRIPTOR
#include <bdlmxxx_descriptor.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMREF
#include <bdlmxxx_elemref.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BDLMXXX_ROWLAYOUT
#include <bdlmxxx_rowlayout.h>
#endif

#ifndef INCLUDED_BDLB_BITSTRINGUTIL
#include <bdlb_bitstringutil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bdlmxxx {
                        // ==================
                        // class RowData
                        // ==================

class RowData {
    // This class provides the lowest level container for holding row objects
    // (for both 'bdem' lists and tables).  An object of this class holds (but
    // does not own) a row layout, the management of which is the
    // responsibility of clients of this class, and stores the sequence of
    // elements corresponding to that row layout.  As such, this class is also
    // responsible for the memory management, construction, and destruction of
    // the contained elements.
    //
    // Note that although this class provides all of the methods required of a
    // value-semantic container except for a copy constructor, the assignment
    // operator only copies the row data and *not* the (held) row layout.  This
    // class provides the externalization functions 'bdexStreamInImp' and
    // 'bdexStreamOutImp' by which a 'RowData' can be streamed to a
    // program built against an older version of 'bdem' provided that the
    // features being used are compatible with that of the older version.
    //
    // This class is for *internal* *use* *only* (specifically, by
    // 'bdlmxxx_listimp' and 'bdlmxxx_tableimp')  It is subject to change without
    // notice and should *not* be used by client code outside of the 'bdem'
    // package.

    // DATA
    const RowLayout *d_rowLayout_p;            // indexable element type
                                                    // and offset information;
                                                    // held (not owned)

    void                 *d_rowData_p;              // aligned memory for the
                                                    // row elements

    int                  *d_nullnessBitsArray_p;    // bits signifying nullness
                                                    // of individual elements

    int                   d_dataSize;               // size (in bytes) of the
                                                    // row data footprint

    int                   d_nullnessBitsArraySize;  // size (in bytes) of
                                                    // nullness bits array

    AggregateOption::AllocationStrategy
                          d_allocationStrategy;     // allocation strategy used
                                                    // to construct rows and
                                                    // their elements

    bslma::Allocator     *d_allocator_p;            // memory allocator (held,
                                                    // not owned)

    // FRIENDS
    friend bool operator==(const RowData&, const RowData&);

  private:
    // PRIVATE MANIPULATORS
    void constructData(void *dstRowData, const RowLayout *rowLayout);
        // Construct, beginning at the specified 'dstRowData' address, a
        // sequence of elements whose types are the same as that of the
        // specified 'rowLayout', with each element set to null and having its
        // respective unset value (see 'bdltuxxx_unset').  The behavior is
        // undefined unless the memory segment addressed by 'dstRowData' is at
        // least 'rowLayout->totalOffset()' bytes in size.

    void constructData(void                 *dstRowData,
                       const RowLayout *rowLayout,
                       const RowData&   srcRowData,
                       int                   startIndex,
                       int                   numElements);
        // Construct, beginning at the specified 'dstRowData' address, a
        // sequence of elements whose types are the same as that of the
        // specified 'rowLayout' with elements set to the values of the
        // specified 'numElements' beginning at the specified 'startIndex' in
        // the specified 'srcRowData'.  The behavior is undefined unless
        // '0 <= startIndex', '0 <= numElements',
        // 'startIndex + numElements <= srcRowData.length()',
        // 'numElements == rowLayout->length()', the types of the elements in
        // 'rowLayout' are the same as those of the 'numElements' beginning at
        // at 'startIndex' in the row layout held by 'srcRowData', and the
        // memory segment addressed by 'dstRowData' is at least
        // 'rowLayout->totalOffset()' bytes in size.

    void init(const RowLayout *rowLayout);
        // Initialize this row data object to have the sequence of element
        // types that is the same as that of the specified 'rowLayout', with
        // each element set to null and having its respective unset value (see
        // 'bdltuxxx_unset').

    void init(const RowLayout *rowLayout,
              const RowData&   srcRowData,
              int                   startIndex,
              int                   numElements);
        // Initialize this row data object to have the sequence of element
        // types that is the same as that of the specified 'rowLayout', with
        // elements set to the values of the specified 'numElements' beginning
        // at the specified 'startIndex' in the specified 'srcRowData'.  The
        // behavior is undefined unless '0 <= startIndex', '0 <= numElements',
        // 'startIndex + numElements <= srcRowData.length()',
        // 'numElements == rowLayout->length()', and the types of the elements
        // in 'rowLayout' are the same as those of the 'numElements' beginning
        // at 'startIndex' in the row layout held by 'srcRowData'.

    void resize(int numElements, int dataSize);
        // Resize this row data to have sufficient capacity for storing the
        // specified 'numElements' and having the specified 'dataSize' (in
        // bytes).  The behavior is undefined unless '0 <= numElements' and
        // '0 <= dataSize'.

    // PRIVATE ACCESSORS
    bool isDataEqual(const RowData& rhs) const;
        // Return 'true' if the element values of this row data are the same as
        // the corresponding element values of the specified 'rhs' row data,
        // and 'false' otherwise.  The behavior is undefined unless this row
        // data and 'rhs' hold row layouts, they have the same number of
        // elements, and corresponding elements at each index position have the
        // same type.

  private:
    // NOT IMPLEMENTED
    RowData(const RowData&);

  public:
    // CLASS DATA
    static const Descriptor  s_rowAttr;
        // Container for the 'Descriptor' attributes of 'RowData'.
        // Note that the attributes for construction, destruction, assignment,
        // and move are null.

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    RowData(AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma::Allocator                         *basicAllocator = 0);
        // Create an empty row data object using the specified memory
        // 'allocationStrategy'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    RowData(const RowLayout                     *rowLayout,
                 AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma::Allocator                         *basicAllocator = 0);
        // Create a row data object having the sequence of element types that
        // is the same as that of the specified 'rowLayout', with each element
        // set to null and having its respective unset value (see
        // 'bdltuxxx_unset'), and using the specified memory 'allocationStrategy'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  'rowLayout' is held by the newly-constructed row data object
        // until one of the 'reset' methods is called, or until the object is
        // destroyed.

    RowData(const RowLayout                     *rowLayout,
                 const RowData&                       other,
                 AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma::Allocator                         *basicAllocator = 0);
        // Create a row data object having the sequence of element types that
        // is the same as that of the specified 'rowLayout', with each element
        // set to the value of the corresponding element in the specified
        // 'other' row data, and using the specified memory
        // 'allocationStrategy'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'rowLayout' is held by the
        // newly-constructed row data object until one of the 'reset' methods
        // is called, or until the object is destroyed.  The behavior is
        // undefined unless the number and type of elements in 'rowLayout' are
        // the same as those of the row layout held by 'other'.

    RowData(const RowLayout                     *rowLayout,
                 const RowData&                       other,
                 int                                       startIndex,
                 int                                       numElements,
                 AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma::Allocator                         *basicAllocator = 0);
        // Create a row data object having the sequence of element types that
        // is the same as that of the specified 'rowLayout', with elements set
        // to the values of the specified 'numElements' beginning at the
        // specified 'startIndex' in the specified 'other' row data, and using
        // the specified memory 'allocationStrategy'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  'rowLayout' is
        // held by the newly-constructed row data object until one of the
        // 'reset' methods is called, or until the object is destroyed.  The
        // behavior is undefined unless '0 <= startIndex', '0 <= numElements',
        // 'startIndex + numElements <= other.length()',
        // 'numElements == rowLayout->length()', and the types of the elements
        // in 'rowLayout' are the same as those of the 'numElements' beginning
        // at 'startIndex' in the row layout held by 'other'.

    ~RowData();
        // Destroy this row data object.  If this object was constructed with
        // any memory allocation strategy other than 'BDEM_PASS_THROUGH', then
        // destructors of individually contained elements are not invoked.  The
        // memory used by those elements will be released efficiently (all at
        // once) when the managed memory allocator that was supplied at
        // construction is destroyed.

    // MANIPULATORS
    RowData& operator=(const RowData& rhs);
        // Assign to this row data the element values in the specified 'rhs'
        // row data, and return a reference to this modifiable row data.  The
        // behavior is undefined unless this row data and 'rhs' hold row
        // layouts, they have the same number of elements, and corresponding
        // elements at each index position have the same type.  Note that after
        // the assignment, both objects will have the same sequence of elements
        // (types and values).

    void *elemData(int index);
        // Return the address of the modifiable element value at the specified
        // 'index' in this row data.  The behavior is undefined unless
        // '0 <= index < length()'.

    ElemRef elemRef(int index);
        // Return a 'bdem' element reference to the modifiable element value at
        // the specified 'index' in this row data.  The behavior is undefined
        // unless '0 <= index < length()'.

    void *insertElement(int dstIndex, const void *value);
        // Insert into this row data, at the specified 'dstIndex', an element
        // having the specified 'value' and having the same type as the element
        // at 'dstIndex' in the row layout held by this object.  The client
        // must insert the corresponding entry into the row layout held by this
        // object immediately prior to calling this method, and must restore
        // the row layout to its previous state in the event that an exception
        // is thrown during the insertion.  The behavior is undefined unless
        // '0 <= dstIndex <= length()', and the actual type of 'value' is the
        // same as the type at 'dstIndex' in the row layout held by this
        // object.  Note that this method is alias-safe.

    void *insertElementRaw(int dstIndex);
        // Insert into this row data, at the specified 'dstIndex', an
        // uninitialized element having the same type as the element at
        // 'dstIndex' in the row layout held by this object.  The inserted
        // element will be non-null.  The client must insert the corresponding
        // entry into the row layout held by this object immediately prior to
        // calling this method, and must restore the row layout to its previous
        // state in the event that an exception is thrown during the insertion.
        // The behavior is undefined unless '0 <= dstIndex <= length()'.

    void insertElements(int                 dstIndex,
                        const RowData& srcRowData,
                        int                 srcIndex,
                        int                 numElements);
        // Insert into this row data, beginning at the specified 'dstIndex',
        // the values of the specified 'numElements' beginning at the specified
        // 'srcIndex' of the specified 'srcRowData'.  The client must insert
        // the corresponding 'numElements' entries into the row layout held by
        // this object immediately prior to calling this method, and must
        // restore the row layout to a consistent state in the event that an
        // exception is thrown during the insertion of any of the new elements.
        // The behavior is undefined unless '0 <= dstIndex <= length()',
        // '0 <= srcIndex', '0 <= numElements',
        // 'srcIndex + numElements <= srcRowData.length()', the types of the
        // 'numElements' beginning at 'dstIndex' in the row layout held by this
        // object are the same as those of the 'numElements' beginning at
        // 'startIndex' in the row layout held by 'srcRowData', and
        // 'srcRowData' is *not* an alias for this row data object.

    void *insertNullElement(int dstIndex);
        // Insert into this row data, at the specified 'dstIndex', a null
        // element having the same type as the element at 'dstIndex' in the
        // row layout held by this object.  The client must insert the
        // corresponding entry into the row layout held by this object
        // immediately prior to calling this method, and must restore the row
        // layout to its previous state in the event that an exception is
        // thrown during the insertion.  The behavior is undefined unless
        // '0 <= dstIndex <= length()'.  Note that, if accessed, the value of
        // the inserted element will be the corresponding unset value for its
        // element type (see 'bdltuxxx_unset').

    void insertNullElements(int dstIndex, int numElements);
        // Insert into this row data, at the specified 'dstIndex', the
        // specified null 'numElements' having the types of the element types
        // at 'dstIndex' in the row layout held by this object.  The client
        // must insert the corresponding 'numElements' entries into the row
        // layout held by this object immediately prior to calling this method,
        // and must restore the row layout to a consistent state in the event
        // that an exception is thrown during the insertion of any of the new
        // elements.  The behavior is undefined unless
        // '0 <= dstIndex <= length()' and '0 <= numElements'.  Note that, if
        // accessed, the values of the inserted elements will be the
        // corresponding unset value for the respective element types (see
        // 'bdltuxxx_unset').

    void makeNull(int index);
        // Set the value of the element at the specified 'index' in this row
        // data to null.  The behavior is undefined unless
        // '0 <= index < length()'.  Note that, if accessed, the value will be
        // the corresponding unset value for its element type (see
        // 'bdltuxxx_unset').

    void makeAllNull();
        // Set the value of each element in this row data to null.  The
        // behavior is undefined unless this row data holds a row layout.  Note
        // that, if accessed, the values will be the corresponding unset values
        // for the respective element types (see 'bdltuxxx_unset').

    void removeElement(int index);
        // Remove from this row data the element at the specified 'index'.  The
        // client must remove the corresponding entry from the row layout held
        // by this object immediately after calling this method.  The behavior
        // is undefined unless '0 <= index < length()'.

    void removeElements(int startIndex, int numElements);
        // Remove from this row data the specified 'numElements' beginning at
        // the specified 'startIndex'.  The client must remove the
        // corresponding 'numElements' entries from the row layout held by this
        // object immediately after calling this method.  The behavior is
        // undefined unless '0 <= startIndex', '0 <= numElements', and
        // 'startIndex + numElements <= length()'.

    void replaceValues(const RowData& other);
        // Replace the element values stored by this object with the
        // corresponding element values of the specified 'other' row data
        // object.  The behavior is undefined unless the number and type of
        // elements in the row layout held by this object are the same as those
        // of the row layout held by 'other'.  Note that this method is
        // alias-safe.

    void reset();
        // Reset this row data to its default constructed state, disassociating
        // the row layout held by this object, if any.  If this object was
        // constructed with any memory allocation strategy other than
        // 'BDEM_PASS_THROUGH', then destructors of individually contained
        // elements are not invoked.  The memory used by those elements will be
        // released efficiently (all at once) when the managed memory allocator
        // that was supplied at construction is destroyed.  This method has no
        // effect if this row data does not currently hold a row layout.

    void reset(const RowLayout *rowLayout);
        // Reset this row data object to have the sequence of element types
        // that is the same as that of the specified 'rowLayout', with each
        // element set to null and having its respective unset value (see
        // 'bdltuxxx_unset').  'rowLayout' is held by this row data object until
        // one of the 'reset' methods is again called, or until this object is
        // destroyed.  If this object was constructed with any memory
        // allocation strategy other than 'BDEM_PASS_THROUGH', then destructors
        // of individually contained elements are not invoked.  The memory used
        // by those elements will be released efficiently (all at once) when
        // the managed memory allocator that was supplied at construction is
        // destroyed.

    void reset(const RowLayout *rowLayout,
               const RowData&   srcRowData);
        // Reset this row data object to have the sequence of element types
        // that is the same as that of the specified 'rowLayout', with each
        // element set to the value of the corresponding element in the
        // specified 'srcRowData'.  'rowLayout' is held by this row data object
        // until one of the 'reset' methods is again called, or until this
        // object is destroyed.  If this object was constructed with any memory
        // allocation strategy other than 'BDEM_PASS_THROUGH', then destructors
        // on individually contained elements are not invoked.  The memory used
        // by those elements will be released efficiently (all at once) when
        // the managed memory allocator that was supplied at construction is
        // destroyed.  The behavior is undefined unless the number and type of
        // elements in 'rowLayout' are the same as those of the row layout held
        // by 'srcRowData', and 'srcRowData' is *not* an alias for this row
        // data object.

    void setValue(int index, const void *value);
        // Set the value of the element at the specified 'index' in this row
        // data to the specified 'value'.  If the indexed element is null, it
        // is made non-null.  The behavior is undefined unless
        // '0 <= index < length()', and the actual type of 'value' is the same
        // as the type at 'index' in the row layout held by this object.  Note
        // that this method is alias-safe.

    void swap(RowData& other);
        // Swap the value of this row data with that of the specified 'other'
        // row data in constant time.  The behavior is undefined unless
        // 'other' has the same allocator and was constructed with the same
        // allocation strategy as this row data.

    void swapElements(int index1, int index2);
        // Swap the type and value of the elements at the specified 'index1'
        // and 'index2' positions in this row data in constant time without the
        // possibility of an exception being thrown (i.e., this method provides
        // the no-throw guarantee).  The client must swap the corresponding
        // entries in the row layout held by this object immediately before
        // calling this method.  The behavior is undefined unless
        // '0 <= index1 < length()' and '0 <= index2 < length()'.

    template <class STREAM>
    STREAM& bdexStreamInImp(
                         STREAM&                                stream,
                         int                                    version,
                         const DescriptorStreamIn<STREAM> *strmAttrLookup,
                         const Descriptor *const          *attrLookup);
        // Read the row data from the specified input 'stream' and return a
        // reference to the modifiable 'stream'.  The previous value of '*this'
        // is discarded.  The 'version' is mandatory and indicates the expected
        // input stream format.  'strmAttLookup' maps each known element type
        // to a corresponding function for streaming in that type.
        // 'attrLookup' maps each known element type to a descriptor (see
        // bdlmxxx_descriptor).
        //
        // If 'version' is not supported, 'stream' is marked invalid and the
        // row data is left unchanged.  If 'stream' is initially invalid, the
        // row data is left unchanged.  If 'stream' becomes invalid during this
        // operation, 'stream' is marked invalid; row data is valid, but its
        // value is unspecified.  Decoding will fail (incorrect data will be
        // read and/or the stream will become invalid) unless the types of
        // objects encoded in 'stream' are the same as the sequence of element
        // types in the row layout held by this object and 'stream's version
        // is the same as 'version'.
        //
        // Note that the 'STREAM' type must adhere to the protocol specified by
        // 'bdlxxxx::InStream' (see the package-group-level documentation for more
        // information on 'bdex' streaming of container types).

    // ACCESSORS
    const void *elemData(int index) const;
        // Return the address of the non-modifiable element value at the
        // specified 'index' in this row data.  The behavior is undefined
        // unless '0 <= index < length()'.

    ConstElemRef elemRef(int index) const;
        // Return a 'bdem' element reference to the non-modifiable element
        // value at the specified 'index' in this row data.  The behavior is
        // undefined unless '0 <= index < length()'.

    bool isAnyInRangeNonNull(int startIndex, int numElements) const;
        // Return 'true' if any of the specified 'numElements' beginning at the
        // specified 'startIndex' in this row data are non-null, and 'false'
        // otherwise.  The behavior is undefined unless
        // '0 <= startIndex < length()', '0 <= numElements', and
        // 'startIndex + numElements <= length()'.

    bool isAnyInRangeNull(int startIndex, int numElements) const;
        // Return 'true' if any of the specified 'numElements' beginning at the
        // specified 'startIndex' in this row data are null, and 'false'
        // otherwise.  The behavior is undefined unless
        // '0 <= startIndex < length()', '0 <= numElements', and
        // 'startIndex + numElements <= length()'.

    bool isNull(int index) const;
        // Return 'true' if the element at the specified 'index' in this row
        // data is null, and 'false' otherwise.  The behavior is undefined
        // unless '0 <= index < length()'.

    int length() const;
        // Return the number of elements stored by this row data.  The behavior
        // is undefined unless this row data holds a row layout.

    const RowLayout *rowLayout() const;
        // Return the address of the non-modifiable row layout held by this row
        // data, or 0 if this object does not currently hold a row layout.

    template <class STREAM>
    STREAM& bdexStreamOutImp(
                 STREAM&                                 stream,
                 int                                     version,
                 const DescriptorStreamOut<STREAM> *strmAttrLookup) const;
        // Write this row data's element values to the specified output
        // 'stream', and return a reference to the modifiable 'stream'.  The
        // 'version' is mandatory and specifies the format of the output.  The
        // 'version' is *not* written to the stream.  If 'version' is not
        // supported, 'stream' is left unmodified.  Auxiliary information is
        // provided by 'strmAttrLookup', which maps each known element type to
        // a corresponding function for streaming in that type.
        //
        // The 'STREAM' type must adhere to the 'bdlxxxx::OutStream' protocol (see
        // the package-group-level documentation for more information on 'bdex'
        // streaming of container types).

    bsl::ostream& printRow(bsl::ostream& stream,
                           int           level = 0,
                           int           spacesPerLevel = 4,
                           bool          elementLabels = true) const;
        // Format this row data to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level',
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative suppress newlines and format the
        // entire output on one line.  Optionally specify an 'elementLabels'
        // flag indicating whether type names are output prior to each element
        // in the row.  If 'elementLabels' is not specified, type names are
        // output.  If 'stream' is not valid on entry, this operation has no
        // effect.  The behavior is undefined unless this row data holds a row
        // layout.
};

// FREE OPERATORS
bool operator==(const RowData& lhs, const RowData& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' row data objects have the
    // same value, and 'false' otherwise.  Two row data objects have the same
    // value if they have the same number of elements, and corresponding
    // elements at each index position have the same type and value.  The
    // behavior is undefined unless 'lhs' and 'rhs' hold row layouts.

bool operator!=(const RowData& lhs, const RowData& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' row data objects do not
    // have the same value, and 'false' otherwise.  Two row data objects do not
    // have the same value if they do not have the same number of elements, or
    // there are corresponding elements at some index position that do not have
    // the same type or value.  The behavior is undefined unless 'lhs' and
    // 'rhs' hold row layouts.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ------------------
                        // class RowData
                        // ------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
int RowData::length() const
{
    BSLS_ASSERT_SAFE(d_rowLayout_p);

    return d_rowLayout_p->length();
}

inline
const RowLayout *RowData::rowLayout() const
{
    return d_rowLayout_p;
}

// CLASS METHODS
inline
int RowData::maxSupportedBdexVersion()
{
    return 3;
}

// MANIPULATORS
inline
void *RowData::elemData(int index)
{
    BSLS_ASSERT_SAFE(rowLayout());
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    bdlb::BitstringUtil::set(d_nullnessBitsArray_p,
                            index,
                            false,
                            1);
    return (void *)((char *) d_rowData_p + (*d_rowLayout_p)[index].offset());
}

template <class STREAM>
STREAM&
RowData::bdexStreamInImp(
                         STREAM&                                stream,
                         int                                    version,
                         const DescriptorStreamIn<STREAM> *strmAttrLookup,
                         const Descriptor *const          *attrLookup)
{
    BSLS_ASSERT(rowLayout());

    switch (version) {  // Switch on the schema version (starting with 1).
      case 3: {
        const int len = length();

        int startIndex = 0;
        while (startIndex < len) {
            int endIndex = startIndex + 32;
            endIndex = endIndex < len ? endIndex : len;

            unsigned int bitmap;
            stream.getUint32(bitmap);
            if (!stream) {
                return stream;                                        // RETURN
            }

            unsigned int bit = 1U << 31;
            for (int i = startIndex; i < endIndex; ++i, bit >>= 1) {
                if (! (bitmap & bit)) {
                    const RowLayoutEntry& e = (*d_rowLayout_p)[i];
                    ElemType::Type type = (ElemType::Type)
                                                    e.attributes()->d_elemEnum;
                    const DescriptorStreamIn<STREAM> *elemStrmAttr =
                                                         &strmAttrLookup[type];

                    version = ElemType::isAggregateType(type) ? version
                                                                   : 1;

                    elemStrmAttr->streamIn((char *) d_rowData_p + e.offset(),
                                           stream,
                                           version,
                                           strmAttrLookup,
                                           attrLookup);

                    if (!stream) {
                        return stream;                                // RETURN
                    }

                    // If set then element cannot be null, so set non null.

                    bdlb::BitstringUtil::set(d_nullnessBitsArray_p, i, false);
                }
                else {
                    elemRef(i).makeNull();
                }
            }

            startIndex = endIndex;
        }
      } break;
      case 2: {
        const int len = length();

        int startIndex = 0;
        while (startIndex < len) {
            int endIndex = startIndex + 32;
            endIndex = endIndex < len ? endIndex : len;

            unsigned int bitmap;
            stream.getUint32(bitmap);
            if (!stream) {
                return stream;                                        // RETURN
            }

            unsigned int bit = 1U << 31;
            for (int i = startIndex; i < endIndex; ++i, bit >>= 1) {
                if (bitmap & bit) {
                    const RowLayoutEntry& e = (*d_rowLayout_p)[i];
                    ElemType::Type type = (ElemType::Type)
                                                    e.attributes()->d_elemEnum;
                    const DescriptorStreamIn<STREAM> *elemStrmAttr =
                                                         &strmAttrLookup[type];

                    // It is a known bug that 'version' is not propagated
                    // correctly below, but fixing this will break wire
                    // compatibility.  It *is* propagated correctly in
                    // subsequent streaming versions.

                    elemStrmAttr->streamIn((char *) d_rowData_p + e.offset(),
                                           stream,
                                           1,
                                           strmAttrLookup,
                                           attrLookup);

                    if (!stream) {
                        return stream;                                // RETURN
                    }

                    // If set then element cannot be null, so set non null.

                    bdlb::BitstringUtil::set(d_nullnessBitsArray_p, i, false);
                }
            }

            startIndex = endIndex;
        }
      } break;
      case 1: {
        const int len = length();

        for (int index = 0; index < len; ++index) {
            const RowLayoutEntry& e = (*d_rowLayout_p)[index];
            const DescriptorStreamIn<STREAM> *elemStrmAttr =
                                   &strmAttrLookup[e.attributes()->d_elemEnum];
            elemStrmAttr->streamIn((char *) d_rowData_p + e.offset(),
                                   stream,
                                   1,
                                   strmAttrLookup,
                                   attrLookup);

            if (!stream) {
                return stream;                                        // RETURN
            }
        }

        // Set all the elements to be non-null.

        bdlb::BitstringUtil::set(d_nullnessBitsArray_p, 0, false, len);
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}

// ACCESSORS
inline
const void *RowData::elemData(int index) const
{
    BSLS_ASSERT_SAFE(rowLayout());
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return (const void *)
                     ((char *) d_rowData_p + (*d_rowLayout_p)[index].offset());
}

inline
bool RowData::isNull(int index) const
{
    BSLS_ASSERT_SAFE(rowLayout());
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < length());

    return bdlb::BitstringUtil::get(d_nullnessBitsArray_p, index);
}

template <class STREAM>
STREAM&
RowData::bdexStreamOutImp(
                  STREAM&                                 stream,
                  int                                     version,
                  const DescriptorStreamOut<STREAM> *strmAttrLookup) const
{
    BSLS_ASSERT(rowLayout());

    switch (version) {  // Switch on the schema version (starting with 1).
      case 3: {
        const int len = length();

        int startIndex = 0;
        while (startIndex < len) {
            int endIndex = startIndex + 32;
            endIndex = endIndex < len ? endIndex : len;

            unsigned int bitmap = 0;
            unsigned int bit    = 1U << 31;
            for (int i = startIndex; i < endIndex; ++i, bit >>= 1) {
                bitmap |= bit * (int) isNull(i);
            }
            stream.putUint32(bitmap);

            bit = 1U << 31;
            for (int i = startIndex; i < endIndex; ++i, bit >>= 1) {
                if (!(bitmap & bit)) {
                    const RowLayoutEntry& e = (*d_rowLayout_p)[i];
                    ElemType::Type type =
                              (ElemType::Type) e.attributes()->d_elemEnum;
                    const DescriptorStreamOut<STREAM> *elemStrmAttr =
                                                         &strmAttrLookup[type];

                    version = ElemType::isAggregateType(type) ? version
                                                                   : 1;

                    elemStrmAttr->streamOut((char *) d_rowData_p + e.offset(),
                                            stream,
                                            version,
                                            strmAttrLookup);
                }
            }

            startIndex = endIndex;
        }
      } break;
      case 2: {
        const int len = length();

        int startIndex = 0;
        while (startIndex < len) {
            int endIndex = startIndex + 32;
            endIndex = endIndex < len ? endIndex : len;

            unsigned int bitmap = 0;
            unsigned int bit    = 1U << 31;
            for (int i = startIndex; i < endIndex; ++i, bit >>= 1) {
                const RowLayoutEntry& e = (*d_rowLayout_p)[i];
                bitmap |= bit * !e.attributes()->isUnset(
                                            (char *) d_rowData_p + e.offset());
            }
            stream.putUint32(bitmap);

            bit = 1U << 31;
            for (int i = startIndex; i < endIndex; ++i, bit >>= 1) {
                if (bitmap & bit) {
                    const RowLayoutEntry& e = (*d_rowLayout_p)[i];
                    ElemType::Type type =
                              (ElemType::Type) e.attributes()->d_elemEnum;
                    const DescriptorStreamOut<STREAM> *elemStrmAttr =
                                                         &strmAttrLookup[type];

                    // It is a known bug that 'version' is not propagated
                    // correctly below, but fixing this will break wire
                    // compatibility.  It *is* propagated correctly in
                    // subsequent streaming versions.

                    elemStrmAttr->streamOut((char *) d_rowData_p + e.offset(),
                                            stream,
                                            1,
                                            strmAttrLookup);
                }
            }

            startIndex = endIndex;
        }
      } break;
      case 1: {
        const int len = length();
        for (int index = 0; index < len; ++index) {
            const RowLayoutEntry& e = (*d_rowLayout_p)[index];
            const DescriptorStreamOut<STREAM> *elemStrmAttr =
                                   &strmAttrLookup[e.attributes()->d_elemEnum];
            elemStrmAttr->streamOut((char *) d_rowData_p + e.offset(),
                                    stream,
                                    1,
                                    strmAttrLookup);
        }
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
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
