// bdem_tableimp.h                                                    -*-C++-*-
#ifndef INCLUDED_BDEM_TABLEIMP
#define INCLUDED_BDEM_TABLEIMP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide implementation of a 'bdem' table.
//
//@CLASSES:
//  bdem_TableImp: provides implementation of the 'bdem' table
//
//@SEE_ALSO: bdem_table
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component implements a 'bdem' table, a homogeneous
// sequence of rows having heterogeneous columns of 'bdem' elements.  This
// component is *used* *by* the 'bdem_Table' component.  Clients should use
// 'bdem_Table', and should not use this component directly.
//
///'bdem' Null States
///------------------
// The concept of null applies to each 'bdem' type.  In addition to the range
// of values in a given 'bdem' type's domain (e.g., '[ INT_MIN .. INT_MAX ]'
// for 'BDEM_INT'), each type has a null value.  When a 'bdem' element is null,
// it has an underlying (unique) designated unset value (or state) as indicated
// in the following TableImp:
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
// For example, suppose we have a 'bdem_TableImp', 'myTableImp', whose first
// element is
// of type 'BDEM_STRING':
//..
//  myTableImp.rowElemRef(0).theRow()[0].theModifiableString() = "HELLO";
//  assert(myTableImp.rowElemRef(0).theRow()[0].isNonNull());
//..
// Making the element null also makes it have the unset value (which, for
// 'BDEM_STRING', is the empty string):
//..
//  myTableImp.rowElemRef(0).theRow()[0].makeNull();
//  assert(myTableImp.rowElemRef(0).theRow()[0].isNull());
//  assert(myTableImp.rowElemRef(0).theRow()[0].theString().empty());
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

#ifndef INCLUDED_BDEMA_POOL
#include <bdema_pool.h>
#endif

#ifndef INCLUDED_BDEU_BITSTRINGUTIL
#include <bdeu_bitstringutil.h>
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

#ifndef INCLUDED_BSLMA_RAWDELETERPROCTOR
#include <bslma_rawdeleterproctor.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>        // for 'swap'
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>          // 'bsl::size_t'
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ===================
                        // class bdem_TableImp
                        // ===================

class bdem_TableImp {
    // This class implements a value-semantic, homogeneous, indexable sequence
    // container of 'bdem' row objects, each consisting of the same
    // heterogeneous sequence of scalar, array, and aggregate types, thus
    // producing a regular row-column structure, where all of the elements in a
    // column are of the same 'bdem' element type, but different columns can
    // represent objects of different ('bdem' element) types.  Each
    // 'bdem_table' object is typically configured at construction to have its
    // own fixed sequence of column types; however, once constructed, any
    // change to the sequence of column types -- e.g., via 'reset' -- requires
    // discarding all of the rows.  Copy assignment and 'bdexStreamIn' will
    // also necessarily reset the table's structure.  Note that the runtime
    // cost of assigning array or aggregate values may be prohibitive, which
    // argues for populating such values in place.  This class is *used* *by*
    // the 'bdem_list' class implementation.  Clients should use 'bdem_list',
    // and should not use this class in place.

    // PRIVATE TYPES
    typedef bslalg_PassthroughTrait<bdem_RowData,
                                    bslalg_TypeTraitBitwiseMoveable> MoveTrait;

    // DATA
    // NOTE: Do *NOT* change the order of the data members.  The implementation
    // relies on them being declared in this order.
    bdem_AllocatorManager            d_allocatorManager;
                                                 // object storing the
                                                 // allocation strategy and
                                                 // custom allocator

    bdem_RowLayout                  *d_rowLayout_p;
                                                 // row layout (owned)

    bdema_Pool                       d_rowPool;  // memory pool

    bsl::vector<bdem_RowData *>      d_rows;     // data rows (owned)

    bsl::vector<int>                 d_nullBits; // nullness bits for rows

    // FRIENDS
    friend bool operator==(const bdem_TableImp&, const bdem_TableImp&);

  private:
    // NOT IMPLEMENTED
    bdem_TableImp(const bdem_TableImp&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdem_TableImp,
                                  bslalg_TypeTraitUsesBslmaAllocator,
                                  MoveTrait);

    // CLASS DATA
    static const bdem_Descriptor d_tableAttr;
        // Attributes of the 'bdem_TableImp' class.  Contains pointers to
        // functions for construction, destruction, assignment, equality test,
        // etc.

    // CLASS METHODS
    template <class STREAM>
    static void
    streamInTable(void                                  *obj,
                  STREAM&                                stream,
                  int                                    version,
                  const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                  const bdem_Descriptor                 *const *attrLookup);
        // Stream into the table imp object whose address is specified by
        // 'obj' the data specified in 'stream' of the specified 'version'
        // using the specified 'strmAttrLookup' stream in descriptors and the
        // specified 'attrLookup' table of type descriptors.  Note
        // that the previous value of '*this' is discarded.  The 'version' is
        // mandatory and indicates the expected input stream format.
        // 'strmAttrLookup' maps each known element type to a corresponding
        // function for streaming in that type.  'attrLookup' maps each known
        // element type to a descriptor (see bdem_descriptor).

    template <class STREAM>
    static void
    streamOutTable(const void                             *obj,
                   STREAM&                                 stream,
                   int                                     version,
                   const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup);
        // Stream out the data in the table imp whose address is specified by
        // 'obj' to the specified 'stream' in the specified 'version' using
        // the specified 'strmAttrLookup' stream out descriptors.  The
        // 'version' is mandatory and specifies the format of the output.  The
        // 'version' is *not* written to the stream.  If 'version' is not
        // supported, 'stream' is left unmodified.  Auxiliary information is
        // provided by 'strmAttrLookup', which maps each known element type to
        // a corresponding function for streaming out that type.

    // CREATORS
    explicit
    bdem_TableImp(
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_TableImp(
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 int                                       initialMemorySize,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create an empty table (having no rows and no columns) using the
        // specified memory allocation strategy 'allocationStrategy'.  (The
        // meanings of the various 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  Optionally specify the 'initialMemorySize'
        // (in bytes) that will be preallocated in order to satisfy allocation
        // requests without replenishment (i.e., without internal allocation):
        // it has no effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined, if 'initialMemorySize' is specified, unless
        // '0 <= initialMemorySize'.

    bdem_TableImp(
               const bdem_ElemType::Type                 columnTypes[],
               int                                       numColumns,
               const bdem_Descriptor                    *const *attrLookupTbl,
               bdem_AggregateOption::AllocationStrategy  allocationStrategy,
               bslma_Allocator                          *basicAllocator = 0);
    bdem_TableImp(
               const bdem_ElemType::Type                 columnTypes[],
               int                                       numColumns,
               const bdem_Descriptor                    *const *attrLookupTbl,
               bdem_AggregateOption::AllocationStrategy  allocationStrategy,
               int                                       initialMemorySize,
               bslma_Allocator                          *basicAllocator = 0);
        // Create an empty table (having no rows) configured with the specified
        // 'numColumns' 'columnTypes'.  Use the specified memory allocation
        // strategy allocationStrategy' and the specified 'attrLookupTbl' table
        // for getting the 'bdem_Descriptor' values corresponding to the
        // various 'bdem_ElemType' values.  (The meanings of the various
        // 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  Optionally specify an 'initialMemorySize'
        // (in bytes) that will be preallocated in order to satisfy allocation
        // requests without replenishment (i.e., without internal allocation):
        // it has no effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'columnTypes' contains 'numElements' types,
        // 'attrLookupTbl' size is at least the maximum integer among those
        // represented by the 'bdem' element types contained by 'columnTypes',
        // and '0 <= initialMemorySize', if 'initialMemorySize' is specified.

    explicit
    bdem_TableImp(
                 const bdem_RowLayout&                     rowLayout,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_TableImp(
                 const bdem_RowLayout&                     rowLayout,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 int                                       initialMemorySize,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create an empty table (having no rows and no columns) using the
        // specified memory allocation strategy 'allocationStrategy'.  (The
        // meanings of the various 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  Optionally specify the 'initialMemorySize'
        // (in bytes) that will be preallocated in order to satisfy allocation
        // requests without replenishment (i.e., without internal allocation):
        // it has no effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined, if 'initialMemorySize' is specified, unless
        // '0 <= initialMemorySize'.

    bdem_TableImp(
                 const bdem_TableImp&                      original,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_TableImp(
                 const bdem_TableImp&                      original,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 int                                       initialMemorySize,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create a table having the value of the specified 'original' table
        // using the specified memory management 'allocationStrategy'.  (The
        // meanings of the various 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  Optionally specify an 'initialMemorySize'
        // (in bytes) that will be preallocated in order to satisfy allocation
        // requests without replenishment (i.e., without internal allocation):
        // it has no effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined, if 'initialMemorySize' is specified, unless
        // '0 <= initialMemorySize'.

    bdem_TableImp(
                 const bdem_RowData&                       rowData,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 bslma_Allocator                          *basicAllocator = 0);
    bdem_TableImp(
                 const bdem_RowData&                       rowData,
                 bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                 int                                       initialMemorySize,
                 bslma_Allocator                          *basicAllocator = 0);
        // Create an empty table configured by the specified 'rowDef', using
        // the specified memory allocation strategy 'allocationStrategy'.  (The
        // meanings of the various 'allocationStrategy' values are described in
        // 'bdem_aggregateoption'.)  Optionally specify an 'initialMemorySize'
        // (in bytes) that will be preallocated in order to satisfy allocation
        // requests without replenishment (i.e., without internal allocation):
        // it has no effect unless 'allocationStrategy' is 'BDEM_WRITE_ONCE' or
        // 'BDEM_WRITE_MANY'.  If 'initialMemorySize' is not specified, an
        // implementation-dependent value will be used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined, if 'initialMemorySize' is specified, unless
        // '0 <= initialMemorySize'.

    ~bdem_TableImp();
        // Destroy this table object.  If the table was constructed with any
        // memory-management mode other than 'BDEM_PASS_THROUGH', then
        // destructors are on individually contained elements are not invoked.
        // The memory used by those elements will be released efficiently (all
        // at once) when the internal (managed) allocator is destroyed.

    // MANIPULATORS
    bdem_TableImp& operator=(const bdem_TableImp& rhs);
        // Assign to this table the value of the specified 'rhs' object,
        // and return a reference to this modifiable table.  Note that the
        // initial rows and column structure of this table are discarded.

    void clear();
        // Remove all the rows and column definitions from this table, leaving
        // the table empty (and unset).  Note that all of the removed rows
        // resources are efficiently freed, and the rows themselves are
        // destroyed.  Also note that this method delegates to the method
        // 'removeAll', which is needed for 'bdem_functiontemplates'.

    bdem_RowData& insertRow(int dstRowIndex, const bdem_RowData& srcRow);
        // Insert into this table, at the specified 'dstRowIndex' the value of
        // the specified 'srcRow'.  Return a reference to the inserted
        // modifiable new row.  The behavior is undefined unless
        // '0 <= dstIndex <= numRows()' and the sequence of element types in
        // 'srcRow' is the same as the sequence of column types in this table.

    void insertRows(int                  dstRowIndex,
                    const bdem_TableImp& srcTable,
                    int                  srcRowIndex,
                    int                  numRows);
        // Insert into this table at the specified 'dstRowIndex' the value of
        // each of the specified 'numRows' starting at the specified
        // 'srcRowIndex' in the specified 'srcTable'.  The behavior is
        // undefined unless '0 <= dstRowIndex <= numRows()',
        // '0 <= srcRowIndex', '0 <= numRows',
        // 'srcRowIndex + numRows <= srcTable.numRows()', and the sequence of
        // element types in 'srcTable' is the same as the sequence of column
        // types in this table.

    void insertNullRows(int dstRowIndex, int numRows);
        // Insert into this table at the specified 'dstRowIndex' the specified
        // null 'numRows'.  The behavior is undefined unless
        // '0 <= dstRowIndex <= numRows()' and '0 <= numRows'.  Note that, if
        // accessed, the value at each column position of the appended rows
        // will be the corresponding unset value for their corresponding
        // element type (see 'bdetu_unset').

    void makeAllNull();
        // Set to null the value of each element in this table.  Note that, if
        // accessed, the value at each column position in each row will be the
        // corresponding unset value for its element type (see 'bdetu_unset').

    void makeColumnNull(int columnIndex);
        // Set to null the value of the element at the specified 'columnIndex'
        // in each each row of this table.  The behavior is undefined unless
        // '0 <= columnIndex < numColumns()'.  Note that, if accessed, the
        // values will be the corresponding unset values for their
        // corresponding element type (see 'bdetu_unset').

    void makeRowsNull(int startRowIndex, int numRows);
        // Set to null the value of each element in the specified 'numRows'
        // beginning at the specified 'startRowIndex' of this table.  The
        // behavior is undefined unless '0 <= startRowIndex', '0 <= numRows',
        // and 'startRowIndex + numRows <= numRows()'.  Note that, if accessed,
        // the values will be the corresponding unset values for their
        // corresponding element type (see 'bdetu_unset').

    void removeRow(int rowIndex);
        // Remove the row at the specified 'rowIndex' from this table.  All
        // row indices greater than 'rowIndex' are decreased by one index
        // position.  The behavior is undefined unless
        // '0 <= rowIndex < numRows()'.

    void removeRows(int rowIndex, int numRows);
        // Remove the specified 'numRows' from this table beginning at the
        // specified 'rowIndex' from this table.  Remaining row indices greater
        // than 'startRowIndex' are decreased by 'numRows' index positions.
        // The behavior is undefined unless '0 <= startRowIndex',
        // '0 <= numRows', and 'startRowIndex + numRows <= numRows()'.

    void removeAll();
        // Remove all the rows and column types from this table (leaving it
        // with its default constructed value).

    void reserveMemory(int numBytes);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numBytes' without replenishment (i.e., without
        // internal allocation).  The behavior is undefined unless
        // '0 <= numBytes'.  Note that this method has no effect unless the
        // internal allocation mode is 'BDEM_WRITE_ONCE' or 'BDEM_WRITE_MANY'.

    void reserveRaw(bsl::size_t numRows);
        // Reserve sufficient memory to satisfy allocation requests for at
        // least the specified 'numRows' with minimal replenishment (i.e.,
        // with mininal internal allocation). If '0 == numRows' the operation
        // has no effect.

    void reset(const bdem_ElemType::Type     columnTypes[],
               int                           numColumns,
               const bdem_Descriptor *const *attrLookupTbl);
        // Remove all the rows from this table and set the sequence of column
        // types in this table to be the same as the specified leading
        // 'numColumns' in the specified 'columnTypes' array, with an
        // additional description in the specified 'attrLookupTbl'.  The
        // behavior is undefined unless 'columnTypes' contains at least
        // 'numColumns' types, and 'attrLookupTbl' size is at least the maximum
        // integer among those represented by the 'bdem' element types
        // contained by 'columnTypes',

    bdem_ElemRef rowElemRef(int rowIndex);
        // Return a 'bdem' element reference to the modifiable row at the
        // specified 'rowIndex' in this table.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()'.

    void setColumnValueRaw(int columnIndex, const void *value);
    void setColumnValue(int columnIndex, const void *value);
        // Set the value of the element at the specified 'columnIndex' in
        // each row of this table to the value addressed by the specified
        // 'value'.  The "Raw" version is more efficient than the general
        // function, but it must only be called for types that are bit-wise
        // copyable.  The behavior is undefined unless
        // '0 <= columnIndex < numColumns()' and the type of the elements at
        // 'columnIndex' corresponds to actual type of the object addressed by
        // 'value' (e.g., if the column at 'columnIndex' is of type
        // 'bdem_ElemType::BDEM_SHORT_ARRAY', then 'value' must point to an
        // object of type 'bsl::vector<short>').

    void swapRows(int rowIndex1, int rowIndex2);
        // Exchange the value of each element in the row indicated by
        // 'rowIndex1' with that of the corresponding element in the row
        // indicated by 'rowIndex2' in constant time without the possibility of
        // an exception being thrown (i.e., this method provides the no-throw
        // guarantee).  The behavior is undefined unless
        // '0 <= rowIndex1 <= numRows()' and '0 <= rowIndex2 <= numRows()'.

    bdem_RowData& theModifiableRow(int rowIndex);
        // Return a reference to the modifiable row data corresponding to
        // the row at the specified 'rowIndex' in this table.  If the
        // referenced row is null, it is made non-null before returning, but
        // its value is not otherwise modified.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()'.

    template <class STREAM>
    STREAM& bdexStreamInImp(
                     STREAM&                                stream,
                     int                                    version,
                     const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                     const bdem_Descriptor                 *const *attrLookup);
        // Read the row definition and row values from the specified input
        // 'stream' and return a reference to 'stream'.  The previous value of
        // '*this' is discarded.  The 'version' is mandatory and indicates the
        // expected input stream format.  'strmAttrLookup' maps each known
        // element type to a corresponding function for streaming in that type.
        // 'attrLookup' maps each known element type to a descriptor (see
        // bdem_descriptor).
        //
        // If 'version' is not supported, 'stream' is marked invalid and table
        // is left unchanged.  If 'stream' is initially invalid, the table is
        // left unchanged.  If 'stream' becomes invalid during this operation,
        // 'stream' is marked invalid; table is valid, but its value is
        // unspecified.
        //
        // Note that the type 'STREAM' must adhere to the protocol specified
        // by 'bdex_InStream' (see the package-group-level documentation for
        // more information on 'bdex' streaming of container types).

    // ACCESSORS
    bdem_ElemType::Type columnType(int columnIndex) const;
        // Return the type of the column at the specified 'columnIndex' in this
        // table.  The behavior is undefined unless
        // '0 <= columnIndex < numColumns()'.

    bsl::size_t getCapacityRaw() const;
        // Return the number of rows for which memory has already been
        // allocated (whether inserted or not).  Note that
        // 'getCapacityRaw() - size()' represents the number of rows that
        // can be inserted with the guarantee of minimal memory replenishment
        // (minimal internal allocation).

    bool isAnyInColumnNull(int columnIndex) const;
        // Return 'true' if the value of an element at the specified
        // 'columnIndex' of any row in this table is null, and 'false'
        // otherwise.  The behavior is undefined unless
        // '0 <= columnIndex < numColumns()'.

    bool isAnyInColumnNonNull(int columnIndex) const;
        // Return 'true' if the value of an element at the specified
        // 'columnIndex' of any row in this table is non-null, and 'false'
        // otherwise.  The behavior is undefined unless
        // '0 <= columnIndex < numColumns()'.

    bool isAnyNull() const;
        // Return 'true' if the value of an element at any row/column position
        // in this table is null, and 'false' otherwise.

    bool isAnyNonNull() const;
        // Return 'true' if the value of an element at any row/column position
        // in this table is non-null, and 'false' otherwise.

    bool isAnyRowNonNull(int rowIndex, int numRows) const;
        // Return 'true' if the value of any of the specified 'numRows'
        // starting at the specified 'rowIndex' of this table is non-null,
        // and 'false' otherwise.  The behavior is undefined unless
        // '0 <= rowIndex', '0 <= numRows', and
        // 'rowIndex + numRows <= numRows()'.

    bool isAnyRowNull(int rowIndex, int numRows) const;
        // Return 'true' if the value of any of the specified 'numRows'
        // starting at the specified 'rowIndex' of this table is null, and
        // 'false' otherwise.  The behavior is undefined unless
        // '0 <= rowIndex', '0 <= numRows', and
        // 'rowIndex + numRows <= numRows()'.  Note that having every element
        // of a row being null, is not the same as the row itself being null:
        // The latter implies the former, but not vice versa.

    int numColumns() const;
        // Return the number of columns in this table.

    int numRows() const;
        // Return the number of rows in this table.

    bdem_ConstElemRef rowElemRef(int rowIndex) const;
        // Return a 'bdem' element reference to the non-modifiable row at the
        // specified 'rowIndex' in this table.  The behavior is undefined
        // unless '0 <= rowIndex < numRows()'.

    const bdem_RowData& theRow(int rowIndex) const;
        // Return a reference to the non-modifiable row data corresponding
        // to the row at the specified 'rowIndex' in this table.  The behavior
        // is undefined unless '0 <= rowIndex < numRows()'.

    template <class STREAM>
    STREAM& bdexStreamOutImp(
                 STREAM&                                 stream,
                 int                                     version,
                 const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const;
        // Write the table's row definition and row values to the specified
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
bool operator==(const bdem_TableImp& lhs, const bdem_TableImp& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' tables have the same
    // value, and 'false' otherwise.  Two tables have the same value if they
    // have the same number of rows and columns, the respective columns have
    // the same types, and at each (row, column) position the corresponding
    // elements have the same value.

bool operator!=(const bdem_TableImp& lhs, const bdem_TableImp& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' tables do not have the
    // same value, and 'false' otherwise.  Two tables do not have the same
    // value if they have differing numbers of rows or columns, the respective
    // column types differ in at least one column position, or corresponding
    // values at any (row, column) position are not the same.

// PRIVATE GEOMETRIC MEMORY GROWTH

void bdem_TableImp_enableGeometricMemoryGrowth();
    // Enable geometric memory growth, upon insertion, for 'bdem_TableImp'
    // objects in the current process.  This method is *not* *thread-safe*,
    // and should be invoked by the owner of 'main'.

void bdem_TableImp_disableGeometricMemoryGrowth();
    // Disable geometric memory growth, upon insertion, for 'bdem_TableImp'
    // objects in the current process.  This method is *not* *thread-safe*,
    // and should be invoked by the owner of 'main'.

bool bdem_TableImp_isGeometricMemoryGrowth();
    // Return 'true' if 'bdem_TableImp_enableGeometricMemoryGrowth' was
    // previously called and no other call to
    // 'bdem_TableImp_disableGeometricMemoryGrowth' was made, and return 'false'
    // otherwise.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // -------------------
                        // class bdem_TableImp
                        // -------------------

                        // -----------------
                        // Level-0 Functions
                        // -----------------

// ACCESSORS
inline
int bdem_TableImp::numColumns() const
{
    return d_rowLayout_p->length();
}

inline
int bdem_TableImp::numRows() const
{
    return static_cast<int>(d_rows.size());
}

                        // -------------------
                        // All Other Functions
                        // -------------------

// CLASS METHODS
template <class STREAM>
inline
void bdem_TableImp::streamInTable(
                         void                                  *obj,
                         STREAM&                                stream,
                         int                                    version,
                         const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                         const bdem_Descriptor           *const attrLookup[])
{
    BSLS_ASSERT_SAFE(obj);

    static_cast<bdem_TableImp *>(obj)->bdexStreamInImp(stream,
                                                       version,
                                                       strmAttrLookup,
                                                       attrLookup);
}

template <class STREAM>
inline
void bdem_TableImp::streamOutTable(
                        const void                             *obj,
                        STREAM&                                 stream,
                        int                                     version,
                        const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup)
{
    BSLS_ASSERT_SAFE(obj);

    static_cast<const bdem_TableImp *>(obj)->bdexStreamOutImp(stream,
                                                              version,
                                                              strmAttrLookup);
}

// MANIPULATORS
inline
void bdem_TableImp::clear()
{
    removeAll();
}

inline
void bdem_TableImp::makeColumnNull(int columnIndex)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    const bsl::size_t numRows = d_rows.size();
    for (bsl::size_t i = 0; i < numRows; ++i) {
        d_rows[i]->makeNull(columnIndex);
    }
}

inline
void bdem_TableImp::makeAllNull()
{
    makeRowsNull(0, numRows());
    bdeu_BitstringUtil::set(&d_nullBits.front(),
                            0,
                            true,
                            static_cast<int>(d_rows.size()));
}

inline
void bdem_TableImp::reserveMemory(int numBytes)
{
    BSLS_ASSERT_SAFE(0 <= numBytes);

    d_allocatorManager.reserveMemory(numBytes);
}

inline
void bdem_TableImp::swapRows(int rowIndex1, int rowIndex2)
{
    BSLS_ASSERT_SAFE(0 <= rowIndex1);
    BSLS_ASSERT_SAFE(     rowIndex1 < numRows());
    BSLS_ASSERT_SAFE(0 <= rowIndex2);
    BSLS_ASSERT_SAFE(     rowIndex2 < numRows());

    bdem_RowData *tmp = d_rows[rowIndex1];
    d_rows[rowIndex1] = d_rows[rowIndex2];
    d_rows[rowIndex2] = tmp;

    bdeu_BitstringUtil::swapRaw(&d_nullBits.front(),
                                rowIndex1,
                                &d_nullBits.front(),
                                rowIndex2,
                                1);
}

inline
bdem_RowData& bdem_TableImp::theModifiableRow(int rowIndex)
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < numRows());

    bdeu_BitstringUtil::set(&d_nullBits.front(), rowIndex, false);
    return *d_rows[rowIndex];
}

inline
void bdem_TableImp::setColumnValueRaw(int columnIndex, const void *value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());
    BSLS_ASSERT_SAFE(value);

    const bsl::size_t numRows = d_rows.size();
    for (bsl::size_t i = 0; i < numRows; ++i) {
        d_rows[i]->setValue(columnIndex, value);
    }
    bsl::memset(&d_nullBits.front(), 0, d_nullBits.size() * sizeof(int));
}

inline
void bdem_TableImp::setColumnValue(int columnIndex, const void *value)
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());
    BSLS_ASSERT_SAFE(value);

    const bsl::size_t numRows = d_rows.size();
    for (bsl::size_t i = 0; i < numRows; ++i) {
        d_rows[i]->setValue(columnIndex, value);
    }
    bsl::memset(&d_nullBits.front(), 0, d_nullBits.size() * sizeof(int));
}

template <class STREAM>
STREAM& bdem_TableImp::bdexStreamInImp(
                      STREAM&                                stream,
                      int                                    version,
                      const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                      const bdem_Descriptor                 *const *attrLookup)
{
    enum { BDEM_BITS_PER_INT = 32 };

    switch (version) {  // Switch on the schema version (starting with 1).
      case 3: {
        bdem_RowLayout layout(d_allocatorManager.internalAllocator());
        layout.bdexStreamIn(stream, 1, attrLookup);
        if (!stream) {
            return stream;                                            // RETURN
        }

        bdem_TableImp newTable(layout,
                               d_allocatorManager.allocationStrategy(),
                               d_allocatorManager.originalAllocator());
        bdeimp_BitwiseCopy<bdem_TableImp>::swap(this, &newTable);

        int numRows;
        stream.getLength(numRows);
        if (!stream || 0 == numRows) {
            return stream;                                            // RETURN
        }

        const int arraySize = (numRows + BDEM_BITS_PER_INT - 1)
                                                           / BDEM_BITS_PER_INT;
        d_nullBits.resize(arraySize);
        stream.getArrayInt32(&d_nullBits.front(),
                             static_cast<int>(d_nullBits.size()));
        if (!stream) {
            return stream;                                            // RETURN
        }

        const int initialNumRows = numRows > 1024 ? 1024 : numRows;
        d_rowPool.reserveCapacity(initialNumRows);
        d_rows.reserve(initialNumRows);

        int arrayIndex = 0;
        while (numRows) {
            unsigned int bit    = 1;
            const    int bitmap = d_nullBits[arrayIndex];
            const    int stride = numRows >= BDEM_BITS_PER_INT
                                                           ? BDEM_BITS_PER_INT
                                                           : numRows;
            for (int i = 0; i < stride; ++i) {
                bdem_RowData *tempRowPtr =
                    new (d_rowPool) bdem_RowData(
                                       d_rowLayout_p,
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.internalAllocator());

                if (!(bitmap & bit)) {
                    bslma_RawDeleterProctor<bdem_RowData, bdema_Pool>
                                                        rowProctor(tempRowPtr,
                                                                   &d_rowPool);
                    tempRowPtr->bdexStreamInImp(stream,
                                                version,
                                                strmAttrLookup,
                                                attrLookup);

                    if (!stream) {
                        removeRows(0, this->numRows());
                        return stream;                                // RETURN
                    }

                    d_rows.push_back(tempRowPtr);
                    rowProctor.release();
                }
                else {
                    d_rows.push_back(tempRowPtr);
                }
                bit <<= 1;
            }
            ++arrayIndex;
            numRows -= stride;
        }
      } break;
      case 2:                                                   // FALL THROUGH
      case 1: {
        bdem_RowLayout layout(d_allocatorManager.internalAllocator());
        layout.bdexStreamIn(stream, 1, attrLookup);
        if (!stream) {
            return stream;                                            // RETURN
        }

        bdem_TableImp newTable(layout,
                               d_allocatorManager.allocationStrategy(),
                               d_allocatorManager.originalAllocator());
        bdeimp_BitwiseCopy<bdem_TableImp>::swap(this, &newTable);

        int numRows;
        stream.getLength(numRows);
        if (!stream || 0 == numRows) {
            return stream;                                            // RETURN
        }

        const int arraySize = (numRows + BDEM_BITS_PER_INT - 1)
                                                           / BDEM_BITS_PER_INT;
        d_nullBits.resize(arraySize);
        bsl::memset(&d_nullBits.front(), 0, d_nullBits.size() * sizeof(int));

        const int initialNumRows = numRows > 1024 ? 1024 : numRows;
        d_rowPool.reserveCapacity(initialNumRows);
        d_rows.reserve(initialNumRows);

        for (int i = 0; i < numRows; ++i) {
            bdem_RowData *tempRowPtr =
                    new (d_rowPool) bdem_RowData(
                                       d_rowLayout_p,
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.internalAllocator());

            bslma_RawDeleterProctor<bdem_RowData, bdema_Pool> rowProctor(
                                                                   tempRowPtr,
                                                                   &d_rowPool);
            tempRowPtr->bdexStreamInImp(stream,
                                        version,
                                        strmAttrLookup,
                                        attrLookup);

            if (!stream) {
                removeRows(0, this->numRows());
                return stream;                                        // RETURN
            }

            d_rows.push_back(tempRowPtr);
            rowProctor.release();
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
bdem_ElemType::Type bdem_TableImp::columnType(int columnIndex) const
{
    BSLS_ASSERT_SAFE(0 <= columnIndex);
    BSLS_ASSERT_SAFE(     columnIndex < numColumns());

    return static_cast<bdem_ElemType::Type>(
                       (*d_rowLayout_p)[columnIndex].attributes()->d_elemEnum);
}

inline
const bdem_RowData& bdem_TableImp::theRow(int rowIndex) const
{
    BSLS_ASSERT_SAFE(0 <= rowIndex);
    BSLS_ASSERT_SAFE(     rowIndex < numRows());

    return *d_rows[rowIndex];
}

template <class STREAM>
STREAM& bdem_TableImp::bdexStreamOutImp(
                  STREAM&                                 stream,
                  int                                     version,
                  const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup) const
{
    enum { BDEM_BITS_PER_INT = 32 };

    switch (version) {  // Switch on the schema version (starting with 1).
      case 3: {
        d_rowLayout_p->bdexStreamOut(stream, 1);
        int numRows = this->numRows();
        stream.putLength(numRows);
        if (0 == numRows) {
            return stream;                                            // RETURN
        }

        stream.putArrayInt32(&d_nullBits.front(),
                             (numRows + BDEM_BITS_PER_INT - 1)
                                                          / BDEM_BITS_PER_INT);

        // Only stream out non-null rows.

        int arrayIndex = 0;
        int rowIndex   = 0;
        while (numRows) {
            unsigned int bit    = 1;
            const    int bitmap = d_nullBits[arrayIndex];
            const    int stride = numRows >= BDEM_BITS_PER_INT
                                                           ? BDEM_BITS_PER_INT
                                                           : numRows;
            for (int i = 0; i < stride; ++i) {
                if (!(bitmap & bit)) {
                    const bdem_RowData *rowPtr = d_rows[rowIndex];
                    rowPtr->bdexStreamOutImp(stream, version, strmAttrLookup);
                }
                ++rowIndex;
                bit <<= 1;
            }
            ++arrayIndex;
            numRows -= stride;
        }
      } break;
      case 2:                                                   // FALL THROUGH
      case 1: {
        d_rowLayout_p->bdexStreamOut(stream, 1);
        const int numRows = this->numRows();
        stream.putLength(numRows);
        for (int i = 0; i < numRows; ++i) {
            const bdem_RowData *rowPtr = d_rows[i];
            rowPtr->bdexStreamOutImp(stream, version, strmAttrLookup);
        }
      } break;
      default: {
        stream.invalidate();
      }
    }

    return stream;
}

// FREE OPERATORS
inline
bool operator!=(const bdem_TableImp& lhs, const bdem_TableImp& rhs)
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
