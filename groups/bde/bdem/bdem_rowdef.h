// bdem_rowdef.h             -*-C++-*-
#ifndef INCLUDED_BDEM_ROWDEF
#define INCLUDED_BDEM_ROWDEF

//@PURPOSE: Provide a container of data defining a row.
//
//@CLASSES:
//  bdem_RowHeader: class used to enable a single back-pointer in a row
//     bdem_RowDef: container for 'bdem_Row' metadata
//
//@SEE_ALSO: bdem_aggregate, bdem_aggregateoption, bdem_idxoffsetmap,
//           bdem_listrep, bdem_rowimp, bdem_tablerep
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION: This component manages creation and alteration of all
// data defining a row, in particular, the metadata for a 'bdem_Row'.  A row
// is a heterogeneous, indexable sequence of scalars, arrays, and/or other
// aggregate values.  A row definition prescribes the sequence of types of the
// values that a row may contain, and each value's offset in the row's memory.
//
// Clients creating rows will typically need to create a row definition first,
// and then use one of that definition's 'construct' functions to create
// (one or more) rows whose structure matches the definition.  Note that this
// usage is intended primarily for library developers.  Most clients of 'bdem'
// will use the 'bdem_List' and 'bdem_Table' interfaces to obtain rows for use
// in higher-level libraries and applications.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_AGGREGATEOPTION
#include <bdem_aggregateoption.h>
#endif

#ifndef INCLUDED_BDEM_DESCRIPTOR
#include <bdem_descriptor.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_IDXOFFSETMAP
#include <bdem_idxoffsetmap.h>
#endif

#ifndef INCLUDED_BDEMA_AUTODEALLOCATOR
#include <bdema_autodeallocator.h>
#endif

namespace BloombergLP {

class bdema_Allocator;
class bdem_RowDef;

                        // ====================
                        // class bdem_RowHeader
                        // ====================

class bdem_RowHeader {
    // Implementation structure used to enable a single back-pointer in a row.
    // The pointer addresses the "header" information of the row *and* the data
    // values of any contained elements.

    friend class bdem_RowDef;
    friend class bdem_RowDef_AutoDtor;

    union {
        bdem_RowDef                    *d_rowDef;
        bdes_Alignment::MaxAlignedType  d_align;
    };

    // NOT IMPLEMENTED
    bdem_RowHeader();
    bdem_RowHeader(const bdem_RowHeader& original);
    bdem_RowHeader& operator=(const bdem_RowHeader& rhs);

  public:
    // PUBLIC CLASS CONSTANTS
    static const bdem_Descriptor d_rowAttr;
        // Attributes of the 'bdem_RowHeader' class.  Contains null pointers
        // for construction, destruction, and assignment functions, but has
        // points for equality test, print, etc. functions.

    // CREATORS
    ~bdem_RowHeader();
};

                        // =================
                        // class bdem_RowDef
                        // =================

class bdem_RowDef {
    // This class contains the factored memory management implementation needed
    // to generate a 'bdem_RowHeader' object, which is

    enum { ALIGNMENT = bdes_Alignment::MAX_ALIGNMENT };
        // WARNING: The requested size of memory for the row should be a whole
        // multiple of MAX_ALIGNMENT, to ensure that alignment-based
        // allocators return properly aligned data.  Hence one should request
        // memory for a row as follows:
        //..
        //  int minSize = HEADER_SIZE + d_rowDef->d_offsetMap.totalOffset();
        //  int allocSize = (minSize + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT;
        //  allocator->allocate(allocSize);
        //..

    enum { HEADER_SIZE =
            (sizeof(bdem_RowHeader) + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT };
        // Memory needed by the row for its header.  The total memory needed
        // by the row is therefore
        // HEADER_SIZE + d_rowDef->d_offsetMap.totalOffset().

  private:
    bdem_IdxOffsetMap       d_offsetMap;  // indexable column offsets
    bdem_AggregateOption::AllocationStrategy
                            d_allocMode;  // pool option to be used in
                                          // construction of row elements
    bdema_Allocator        *d_allocator;  // memory allocator (held, not owned)
    int                     d_rowSize;    // the size of the row in memory

  private:
    // PRIVATE CREATORS
    bdem_RowDef(const bdem_IdxOffsetMap&                  offsets,
                int                                       srcIndex,
                int                                       numElements,
                bdem_AggregateOption::AllocationStrategy  allocMode,
                bdema_Allocator                          *basicAllocator);
        // Create a row definition for a row containing the specified
        // 'numElements'  number of elements from the specified 'offsets'
        // map, beginning with the specified 'srcIndex'.  Use the specified
        // 'basicAllocator' (in the manner indicated by the specified
        // 'allocMode' hint) as a memory source.  Note that the supplied
        // 'basicAllocator' is held but not owned.

    // PRIVATE MANIPULATORS
    static void adjustRowSize(bdem_RowHeader **row_pp, int newLength);
        // If the specified 'newLength' is greater than the number of bytes
        // in the data portion of the row indirectly addressed by the
        // specified 'row_pp', increase that row's memory so that it owns
        // 'newLength' bytes.

    void resetRowSize();
        // Calculate and cache the number of bytes needed by any row whose
        // structure will conform to this row definition.

    // NOT IMPLEMENTED
    bdem_RowDef(const bdem_RowDef&);
    bdem_RowDef& operator=(const bdem_RowDef&);

  public:
    // CLASS METHODS
    static int areRowsEqualRaw(const bdem_RowHeader& lhs,
                               const bdem_RowHeader& rhs);
        // Return 1 if the contents of the row specified by the 'lhs' equal
        // the contents of the row specified by the 'rhs' for all indices
        // between 0 and lhs.numElements(), and 0 otherwise.  The behavior is
        // undefined unless rhs.numElements() >= lhs.numElements() and the
        // respective elements in 'lhs' and 'rhs' have the same type for all
        // indices between 0 and lhs.numElements().

    static int areRowsEqual(const bdem_RowHeader& lhs,
                            const bdem_RowHeader& rhs);
        // Return 1 if both the element types and element values of the row
        // specified by 'lhs' equal the types and values of the row specified
        // by 'rhs'.

    static void compactRow(bdem_RowHeader **row_pp);
        // Reconstruct the row indirectly addressed by the specified 'row_pp'.
        // Note that this compacts the row by simply not recreating elements
        // which have been deleted from the row definition.

    static void destroyRow(bdem_RowHeader *row);
        // Destroy all items contained in the row associated with the specified
        // 'row' header.  Note that although items are destroyed, the row's
        // memory is not deallocated.

    static void deleteRow(bdem_RowHeader *row);
        // Destroy all items contained in the row associated with the specified
        // 'row' header and deallocate all memory held by the row.

    static void *insertElement(bdem_RowHeader        **row_pp,
                               int                     dstIndex,
                               const void             *value,
                               const bdem_Descriptor  *elemAttr);
        // Insert an element of type indicated by the specified 'elemAttr'
        // and the specified 'value' into the row indirectly addressed by
        // the specified 'row_pp' at the specified 'dstIndex'.

    static void *insertElementRaw(bdem_RowHeader        **row_pp,
                                  int                     dstIndex,
                                  const bdem_Descriptor  *elemAttr);
        // Insert an element of type indicated by the specified 'elemAttr'
        // into the row indirectly addressed by the specified 'row_pp' at the
        // specified 'dstIndex'.  Note that the entry is not initialized to
        // any value, including "unset".

    static void insertElements(bdem_RowHeader        **row_pp,
                               int                     dstIndex,
                               const bdem_RowHeader&   srcRow,
                               int                     srcIndex,
                               int                     numElements);
        // Insert into the row indirectly addressed by the specified 'row_pp'
        // the specified 'numElements' from the specified 'srcRow', starting
        // with the element at the specified 'srcIndex'.  Place these
        // 'numElements' items into the row beginning at the specified
        // 'dstIndex'

    static void *insertUnsetElement(bdem_RowHeader        **row_pp,
                                    int                     dstIndex,
                                    const bdem_Descriptor  *elemAttr);
        // Insert an "unset" value of the type specified by 'elemAttr' into the
        // row indirectly addressed by the specified 'row_pp'.

    static void insertUnsetElements(
                             bdem_RowHeader               **row_pp,
                             int                            dstIndex,
                             const bdem_ElemType::Type      elementTypes[],
                             int                            numElements,
                             const bdem_Descriptor *const   attrLookupTbl[]);
        // Insert into the row indirectly addressed by the specified 'row_pp'
        // the specified 'numElements' of "unset" elements, each of the type
        // indicated by the appropriate entry in the specified 'elementTypes'
        // array, and additionally described by the specified 'attrLookupTbl'.
        // Insert these elements into the row, beginning at the specified
        // 'dstIndex'.

    static int isAnyInRangeSet(const bdem_RowHeader& row,
                               int                   startIndex,
                               int                   numElements);
        // Return 1 if any of the specified 'numElements' beginning at the
        // specified 'startIndex' in the specified 'row' have a value other
        // than the canonical "unset" value for their type, and 0 otherwise.
        // The behavior is undefined unless 0 <= 'startIndex' and
        // 'startIndex' + 'numElements' < numElements().

    static int isAnyInRangeUnset(const bdem_RowHeader& row,
                                 int                   startIndex,
                                 int                   numElements);
        // Return 1 if any of the specified 'numElements' beginning at the
        // specified 'startIndex' in the specified 'row' have a value equal
        // to the canonical "unset" value for their type, and 0 otherwise.
        // The behavior is undefined unless 0 <= 'startIndex' and
        // 'startIndex' + 'numElements' < numElements().

    static void makeAllUnset(bdem_RowHeader *row);
        // Set all entries in the row referenced by the specified 'row' header
        // to the canonical "unset" value for items of their type.

    static void makeColumnUnset(bdem_RowHeader *rows[],
                                int             numRows,
                                int             colIdx);
        // For every row between index 0 and the specified 'numRows' - 1 in
        // the specified 'rows' array, set the item at the specified 'colIdx'
        // to the canonical "unset" value for its type.

    static void makeRangeUnset(bdem_RowHeader *row,
                               int             startIndex,
                               int             numElements);
        // Set the specified 'numElements' entries beginning at the specified
        // 'startIndex' in the specified 'row' to the canonical "unset" value
        // for their type.

    static void removeElement(bdem_RowHeader **row_pp, int index);
        // Remove the item at the specified 'index' in the row indirectly
        // addressed by the specified 'row_pp' by destroying (but not deleting)
        // the element's associated memory chunk, and removing its entry from
        // the index-offset map.

    static void removeElements(bdem_RowHeader **row_pp,
                               int              startIndex,
                               int              numElements);
        // Remove the specified 'numElements' items beginning with the
        // specified 'startIndex' from the row indirectly addressed by the
        // specified 'row_pp'.

    static void removeAllElements(bdem_RowHeader **row_pp);
        // Remove all entries from the index-offset map of the row indirectly
        // addressed by the specified 'row_pp'.  If the allocation mode does
        // not include NODESTRUCT_FLAG, also invoke the destructor for each
        // item in that row.  Note that this function does not delete memory.

    static void replaceValues(bdem_RowHeader        *row,
                              int                    dstIndex,
                              const bdem_RowHeader&  srcRow,
                              int                    srcIndex,
                              int                    numElements);
        // Replace the specified 'numElements' items starting at the specified
        // 'dstIndex' in the row referenced by the specified 'row' header with
        // the 'numElements' items in the specified 'srcRow' starting at
        // specified 'srcIndex'.

    static void replaceValue(bdem_RowHeader        *row,
                             int                    dstIndex,
                             const bdem_RowHeader&  srcRow,
                             int                    srcIndex);
        // Replace the entry at the specified 'dstIndex' in the row referenced
        // by the specified 'row' header with the entry at the specified
        // 'srcIndex' in the row referenced by the specified 'srcRow' header.
        // Behavior is undefined unless 0 <= 'dstIndex' < 'row'->numElements()
        // and 0 <= 'srcIndex' < srcRow.numElements().

    static const bdem_Descriptor *rowElemAttr(const bdem_RowHeader& row,
                                              int                   index);
        // Return a pointer to the element attribute structure describing
        // the element type held by the specified 'row' at the specified
        // 'index'.

    static void *rowElemData(bdem_RowHeader *row, int index);
        // Return a pointer to the raw memory occupied by the element at
        // the specified 'index' in the specified 'row'.  The behavior is
        // undefined unless 0 <= 'index' < numElements().

    static const void *rowElemData(const bdem_RowHeader *row, int index);
        // Return a pointer to the raw memory occupied by the element at
        // the specified 'index' in the specified 'row'.  The behavior is
        // undefined unless 0 <= 'index' < numElements().

    static bdem_ElemRef rowElemRef(bdem_RowHeader *row, int index);
        // Return a reference to the modifiable element at the specified
        // 'index' in the specified 'row'.  The behavior is undefined unless
        // 0 <= 'index' < numElements().

    static bdem_ConstElemRef rowElemRef(const bdem_RowHeader *row, int index);
        // Return a reference to the non-modifiable element at the specified
        // 'index' in the specified 'row'.  The behavior is undefined unless
        // 0 <= 'index' < numElements().

    static int rowLength(const bdem_RowHeader& row);
        // Return the number of elements in the row associated with the
        // specified 'row' header.

    static void setColumnValueRaw(bdem_RowHeader *rows[],
                                  int             numRows,
                                  int             colIdx,
                                  const void     *value);
        // For every row between index 0 and the specified 'numRows' - 1 in the
        // specified 'rows' array, copy the number of bytes comprising the size
        // of an element of the type stored at the specified 'colIdx' from the
        // specified 'value' into the row at the 'colIdx' position.

    static void setColumnValue(bdem_RowHeader *rows[],
                               int             numRows,
                               int             colIdx,
                               const void     *value);
        // Using the assignment operator identified in the element attributes
        // structure for items of the type of the entry at the specified
        // 'colIdx', set the entry at 'colIdx' in each of the specified 'rows'
        // between 0 and the specified 'numRows' - 1 to the specified 'value'.

    static std::ostream& printRow(std::ostream&         stream,
                                  const bdem_RowHeader& row,
                                  int                   level = 0,
                                  int                   spacesPerLevel = 4,
                                  bool                  columnLabels = true);
        // Format the specified 'row' to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative suppress newlines and format the
        // entire output on one line.  If 'columnLabels' is true, print the
        // type name prior to each field in the row.  If 'stream' is not valid
        // on entry, this operation has no effect.

    template <class STREAM>
    static
    STREAM& streamInRow(bdem_RowHeader                        *row,
                        STREAM&                                stream,
                        int                                    version,
                        const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                        const bdem_Descriptor *const           attrLookup[]);
        // Assign to the fields in 'row' the values read from the specified
        // input 'stream' and return a reference to the modifiable 'stream'.
        // The 'row' must already exist and have a valid header, i.e. as a
        // result calling 'constructRow()' on a specific 'bdem_RowDef' object.
        // The 'version' is mandatory and indicates the expected input stream
        // format.  'strmAttLookup' maps each known element type to a
        // corresponding function for streaming in that type.  'attrLookup'
        // maps each known element type to a descriptor (see bdem_descriptor).
        //
        // If 'version' is not supported, 'stream' is marked invalid and 'row'
        // is left unchanged.  If 'stream' is initially invalid, the row is
        // left unchanged.  If 'stream' becomes invalid during this operation,
        // 'stream' is marked invalid; row is valid, but its value is
        // unspecified.  Decoding will fail (incorrect data will be read
        // and/or the stream will become invalid) unless the types of objects
        // encoded in 'stream' match the sequence of element types in 'row's
        // definition and 'stream's version matches 'version'.
        //
        // Note that the type 'STREAM' must adhere to the protocol specified
        // by 'bdex_InStream' (see the package-group-level documentation for
        // more information on 'bdex' streaming of container types).

    template <class STREAM>
    static
    STREAM&
    streamOutRow(STREAM&                                 stream,
                 const bdem_RowHeader&                   row,
                 int                                     version,
                 const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup);
        // Write the 'row' value to the specified output 'stream' and return a
        // reference to the modifiable 'stream'.  The 'version' is mandatory
        // and specifies the format of the output.  The 'version' is *not*
        // written to the stream.  If 'version' is not supported, 'stream' is
        // left unmodified.  'strmAttLookup' maps each known element type to a
        // corresponding function for streaming in that type.
        //
        // The type 'STREAM' must adhere to the 'bdex_OutStream' protocol (see
        // the package-group-level documentation for more information on
        // 'bdex' streaming of container types).

    // CREATORS
    bdem_RowDef(bdem_AggregateOption::AllocationStrategy  allocMode,
                bdema_Allocator                          *basicAllocator);
        // Create an empty row definition, using the specified
        // 'basicAllocator' in the manner indicated by the specified
        // 'allocMode' hint as a memory source.  Note that the supplied
        // 'basicAllocator' is held but not owned.

    bdem_RowDef(const bdem_ElemType::Type                 elementTypes[],
                int                                       numElements,
                const bdem_Descriptor *const              attrLookupTbl[],
                bdem_AggregateOption::AllocationStrategy  allocMode,
                bdema_Allocator                          *basicAllocator);
        // Create a row definition containing the specified 'numElements'
        // entries, with index-offset map keys set to the address of the
        // element attribute structure appropriate to the type referenced in
        // the specified 'elementTypes' at each index, and values equal to
        // the memory offset within the row that the element would occupy.
        // Use the specified 'basicAllocator' in the manner indicated by the
        // specified 'allocMode' hint as a memory source.  Note that the
        // supplied 'basicAllocator' is held but not owned.

    bdem_RowDef(int                                       numElements,
                bdem_AggregateOption::AllocationStrategy  allocMode,
                bdema_Allocator                          *basicAllocator);
        // Create a row definition with initial capacity for the specified
        // 'numElements'.  Use the specified 'basicAllocator' in the manner
        // indicated by the specified 'allocMode' hint as a memory source.
        // Note that the supplied 'basicAllocator' is held but not owned.

    bdem_RowDef(const bdem_RowDef&                        original,
                bdem_AggregateOption::AllocationStrategy  allocMode,
                bdema_Allocator                          *basicAllocator);
        // Create a row definition structurally equivalent to the specified
        // 'original' row definition, using the specified 'basicAllocator' in
        // the manner indicated by the specified 'allocMode' hint as a memory
        // source.  Note that the supplied 'basicAllocator' is held but not
        // owned.

    bdem_RowDef(const bdem_RowHeader&                     row,
                bdem_AggregateOption::AllocationStrategy  allocMode,
                bdema_Allocator                          *basicAllocator);
        // Create a row definition structurally equivalent to the row
        // definition referenced by the specified 'row' header.  Use the
        // specified 'basicAllocator' in the manner indicated by the
        // specified 'allocMode' hint as a memory source.  Note that the
        // supplied 'basicAllocator' is held but not owned.

    ~bdem_RowDef();
        // Destroy this object.  Note that the held allocator is not destroyed
        // by this function.

    // MANIPULATORS
    bdem_RowHeader *constructRow();
        // Allocate memory for a row with structure conformant to this row
        // definition, and initialize the appropriate chunks of the row's
        // memory to contain "unset" elements of the indicated types.  Return
        // a pointer to the row's header.

    bdem_RowHeader *constructRow(const bdem_RowHeader& original,
                                 int                   srcIndex = 0);
        // Allocate memory for a row with structure conformant to the row
        // definition referenced by the specified 'original' row header
        // beginning with the element at the specified 'srcIndex', and
        // initialize the appropriate chunks of the row's memory to the values
        // of corresponding entries in the 'original' row.  Return a pointer
        // to the row's header.  The behavior is undefined unless
        // 0 <= srcIndex < original.numElements().

    void removeAll();
        // Remove all elements from this definition.  Note that 'numElements()'
        // is now 0.

    void swapElements(int index1, int index2);
        // Efficiently swap the elements (types and values) at the specified
        // 'index1' and 'index2' in this list.  The behavior is undefined
        // unless both 'index1' and 'index2' are in the range
        // [0 .. numElements() - 1].

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM&                      stream,
                         int                          version,
                         const bdem_Descriptor *const attrLookup[]);
        // Read a row definition from the specified 'stream', using the format
        // specified by 'version'.  'attrLookup' maps each known element type
        // to its corresponding object descriptor.  Return a reference to the
        // modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this row definition is unchanged.  Note that this method
        // is logically equivalent to the following:
        //
        // (See the package-group-level documentation for more information on
        // 'bdex' streaming of container types.)

    template <class STREAM>
    STREAM& streamIn(STREAM&                      stream,
                     int                          version,
                     const bdem_Descriptor *const attrLookup[]);
        // Read a row definition from the specified 'stream', using the format
        // specified by 'version'.  'attrLookup' maps each known element type
        // to its corresponding object descriptor.  Return a reference to the
        // modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this row definition is unchanged.  Note that this method
        // is logically equivalent to the following:
        //
        // DEPRECATED: Replaced by 'bdexStreamIn(stream, version, attrLookup)'

    // ACCESSORS
    int areElemTypesEqual(const bdem_RowDef& other) const;
        // Return 1 if the element type for each valid index in this row
        // definition equals the element type of the corresponding entry in
        // the specified 'other' row definition and vice versa, and 0
        // otherwise.

    bdem_RowHeader *constructRow(void *memory) const;
        // Initialize the appropriate chunks of specified 'memory' with
        // "unset" elements of the types defined in this row definition.
        // Return a pointer to the row's header.  The behavior is undefined
        // unless 0 != 'memory'.

    bdem_RowHeader *constructRow(void                  *memory,
                                 const bdem_RowHeader&  original,
                                 int                    srcIndex = 0) const;
        // Initialize the appropriate chunks of specified 'memory' with
        // elements of both type and value of elements in the specified
        // 'original' row, beginning with the element specified by 'srcIndex'.
        // Return a pointer to the row's header.  The behavior is undefined
        // unless 0 != 'memory' and
        // 0 <= 'srcIndex' < 'original'.numElements().

    const bdem_Descriptor *elemAttr(int index) const;
        // Return the element attributes structure describing the element type
        // for the row entry at the specified 'index'.  Behavior is undefined
        // unless 0 <= index < numElements().

    int numElements() const;
        // Return the number of elements in this row definition.

    int rowSize() const;
        // Return the number of bytes needed by any row whose structure
        // is conformant with this row definition.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    template <class STREAM>
    STREAM& streamOutRaw(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
        //
        // DEPRECATED: replaced by 'bdexStreamOut(stream, version)'
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================
inline
bdem_RowHeader::~bdem_RowHeader()
{
    bdem_RowDef::destroyRow(this);
}

                            // ------------
                            // MANIPULATORS
                            // ------------

inline
void bdem_RowDef::resetRowSize()
{
    d_rowSize = (HEADER_SIZE + d_offsetMap.totalOffset() + ALIGNMENT - 1)
                                                       & ~(ALIGNMENT - 1);
}

inline
bdem_RowHeader *bdem_RowDef::constructRow()
{
    bdem_RowHeader *row = (bdem_RowHeader *)d_allocator->allocate(d_rowSize);
    bdema_AutoDeallocator<bdema_Allocator> rowAutoDealloc(row, d_allocator);
    constructRow(row);
    rowAutoDealloc.release();
    return row;
}

inline
bdem_RowHeader *bdem_RowDef::
                     constructRow(const bdem_RowHeader& original, int srcIndex)
{
    void *mem = d_allocator->allocate(d_rowSize);
    bdema_AutoDeallocator<bdema_Allocator> rowAutoDealloc(mem, d_allocator);
    bdem_RowHeader *row = constructRow(mem, original, srcIndex);
    rowAutoDealloc.release();
    return row;
}

inline
void bdem_RowDef::removeAll()
{
    d_offsetMap.removeAll();
    resetRowSize();
}

inline
void bdem_RowDef::swapElements(int index1, int index2)
{
    // TBD -- is it a good idea to make commitment that row does not change?
    d_offsetMap.swap(index1, index2);
}


template <class STREAM>
STREAM&
bdem_RowDef::bdexStreamIn(STREAM&                      stream,
                          int                          version,
                          const bdem_Descriptor *const attrLookup[])
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 1: {
          bdem_IdxOffsetMap offsetMap(d_allocator);
          offsetMap.bdexStreamIn(stream, 1, attrLookup);
          bdeimp_BitwiseCopy<bdem_IdxOffsetMap>::swap(&d_offsetMap,
                                                      &offsetMap);
          resetRowSize();

      } break;
      default: {
          stream.invalidate();
      }
    }

    return stream;
}

template <class STREAM>
inline STREAM&
bdem_RowDef::streamIn(STREAM&                      stream,
                      int                          version,
                      const bdem_Descriptor *const attrLookup[])
{
    return bdexStreamIn(stream, version, attrLookup);
}


                            // --------
                            // CREATORS
                            // --------

inline
bdem_RowDef::
          bdem_RowDef(bdem_AggregateOption::AllocationStrategy  allocMode,
                      bdema_Allocator                          *basicAllocator)
: d_offsetMap(basicAllocator)
, d_allocMode(allocMode)
, d_allocator(basicAllocator)
{
    resetRowSize();
}

inline
bdem_RowDef::
         bdem_RowDef(const bdem_ElemType::Type                 elementTypes[],
                     int                                       numElements,
                     const bdem_Descriptor *const              attrLookupTbl[],
                     bdem_AggregateOption::AllocationStrategy  allocMode,
                     bdema_Allocator                          *basicAllocator)
: d_offsetMap(elementTypes, numElements, attrLookupTbl, basicAllocator)
, d_allocMode(allocMode)
, d_allocator(basicAllocator)
{
    resetRowSize();
}

inline
bdem_RowDef::
          bdem_RowDef(const bdem_RowDef&                        original,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      bdema_Allocator                          *basicAllocator)
: d_offsetMap(original.d_offsetMap,
              0,
              original.d_offsetMap.length(),
              basicAllocator)
, d_allocMode(allocMode)
, d_allocator(basicAllocator)
{
    d_offsetMap.compact();
    resetRowSize();
}

inline
bdem_RowDef::
          bdem_RowDef(const bdem_RowHeader&                     row,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      bdema_Allocator                          *basicAllocator)
: d_offsetMap(row.d_rowDef->d_offsetMap,
              0,
              row.d_rowDef->d_offsetMap.length(),
              basicAllocator)
, d_allocMode(allocMode)
, d_allocator(basicAllocator)
{
    d_offsetMap.compact();
    resetRowSize();
}

inline
bdem_RowDef::
         bdem_RowDef(const bdem_IdxOffsetMap&                  offsets,
                     int                                       srcIndex,
                     int                                       numElements,
                     bdem_AggregateOption::AllocationStrategy  allocMode,
                     bdema_Allocator                          *basicAllocator)
: d_offsetMap(offsets, srcIndex, numElements, basicAllocator)
, d_allocMode(allocMode)
, d_allocator(basicAllocator)
{
    resetRowSize();
}

inline
bdem_RowDef::
          bdem_RowDef(int                                       numElements,
                      bdem_AggregateOption::AllocationStrategy  allocMode,
                      bdema_Allocator                          *basicAllocator)
: d_offsetMap(bdem_IdxOffsetMap::InitialCapacity(numElements), basicAllocator)
, d_allocMode(allocMode)
, d_allocator(basicAllocator)
{
    resetRowSize();
}

inline
bdem_RowDef::~bdem_RowDef()
{
}

                            // ---------
                            // ACCESSORS
                            // ---------

inline
int bdem_RowDef::areElemTypesEqual(const bdem_RowDef& other) const
{
   return d_offsetMap.areElemTypesEqual(other.d_offsetMap);
}

inline
void bdem_RowDef::deleteRow(bdem_RowHeader *row)
{
    bdem_RowDef *def = row->d_rowDef;
    destroyRow(row);
    def->d_allocator->deallocate(row);
}

inline
const bdem_Descriptor *bdem_RowDef::elemAttr(int index) const
{
    return d_offsetMap[index].d_elemAttr;
}

inline
int bdem_RowDef::numElements() const
{
    return d_offsetMap.length();
}

inline
int bdem_RowDef::rowSize() const
{
    return d_rowSize;
}

template <class STREAM>
inline
STREAM& bdem_RowDef::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        d_offsetMap.bdexStreamOut(stream, 1);
      } break;
    }

    return stream;
}

template <class STREAM>
inline
STREAM& bdem_RowDef::streamOutRaw(STREAM& stream, int version) const
{
    return bdexStreamOut(stream, version);
}

// CLASS METHODS
inline
const bdem_Descriptor *bdem_RowDef::rowElemAttr(const bdem_RowHeader& row,
                                                int                   index)
{
    return row.d_rowDef->elemAttr(index);
}

inline
void* bdem_RowDef::rowElemData(bdem_RowHeader *row, int index)
{
    int offset = row->d_rowDef->d_offsetMap[index].d_offset;
    return ((char *)row) + HEADER_SIZE + offset;
}

inline
const void* bdem_RowDef::rowElemData(const bdem_RowHeader *row, int index)
{
    int offset = row->d_rowDef->d_offsetMap[index].d_offset;
    return ((const char *)row) + HEADER_SIZE + offset;
}

inline
bdem_ElemRef bdem_RowDef::rowElemRef(bdem_RowHeader *row, int index)
{
    const bdem_IdxOffsetMapEntry& e = row->d_rowDef->d_offsetMap[index];
    return bdem_ElemRef(((char *)row) + HEADER_SIZE + e.d_offset,
                        e.d_elemAttr);
}

inline
bdem_ConstElemRef bdem_RowDef::rowElemRef(const bdem_RowHeader *row, int index)
{
    const bdem_IdxOffsetMapEntry& e = row->d_rowDef->d_offsetMap[index];
    return bdem_ConstElemRef(((const char *)row) + HEADER_SIZE + e.d_offset,
                             e.d_elemAttr);
}

inline
int bdem_RowDef::rowLength(const bdem_RowHeader& row)
{
    return row.d_rowDef->numElements();
}

template <class STREAM>
STREAM& bdem_RowDef::streamInRow(
                         bdem_RowHeader                        *row,
                         STREAM&                                stream,
                         int                                    version,
                         const bdem_DescriptorStreamIn<STREAM> *strmAttrLookup,
                         const bdem_Descriptor *const           attrLookup[])
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 2: {
        const int len = row->d_rowDef->d_offsetMap.length();
        char *data = ((char *)row) + HEADER_SIZE;

        int startIndex = 0;
        while (startIndex < len) {
            int endIndex = startIndex + 32;
            endIndex = endIndex < len ? endIndex : len;

            unsigned int bitmap;
            stream.getUint32(bitmap);

            unsigned int bit = 1U << 31;
            for (int index = startIndex; index < endIndex; ++index) {
                const bdem_IdxOffsetMapEntry& e =
                                             row->d_rowDef->d_offsetMap[index];
                bdem_ElemRef elemRef(data + e.d_offset, e.d_elemAttr);
                if (bitmap & bit) {
                    const bdem_DescriptorStreamIn<STREAM> *elemStrmAttr =
                                     &strmAttrLookup[e.d_elemAttr->d_elemEnum];
                    elemStrmAttr->streamIn(data + e.d_offset,
                                           stream,
                                           1,
                                           strmAttrLookup,
                                           attrLookup);
                }
                else {
                    elemRef.makeUnset();
                }
                bit >>= 1;
            }

            startIndex = endIndex;
        }

//           for (int index = 0; index < len; ++index) {
//               const bdem_IdxOffsetMapEntry& e =
//                                     row->d_rowDef->d_offsetMap[index];
//           }
      } break;

      case 1: {
        const int len = row->d_rowDef->d_offsetMap.length();
        char *data = ((char *)row) + HEADER_SIZE;

        for (int index = 0; index < len; ++index) {
            const bdem_IdxOffsetMapEntry& e =
                                    row->d_rowDef->d_offsetMap[index];
            const bdem_DescriptorStreamIn<STREAM> *elemStrmAttr =
                                     &strmAttrLookup[e.d_elemAttr->d_elemEnum];
            elemStrmAttr->streamIn(data + e.d_offset,
                                   stream,
                                   1,
                                   strmAttrLookup,
                                   attrLookup);
        }
      } break;

      default: {
        stream.invalidate();
      }
    }

    return stream;
}

template <class STREAM>
STREAM& bdem_RowDef::streamOutRow(
                        STREAM&                                 stream,
                        const bdem_RowHeader&                   row,
                        int                                     version,
                        const bdem_DescriptorStreamOut<STREAM> *strmAttrLookup)
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 2: {
        const int len = row.d_rowDef->d_offsetMap.length();
        const char *data = ((char *)&row) + HEADER_SIZE;

        int startIndex = 0;
        while (startIndex < len) {
            int endIndex = startIndex + 32;
            endIndex = endIndex < len ? endIndex : len;

            unsigned int bitmap = 0;
            unsigned int bit = 1U << 31;
            for (int index = startIndex; index < endIndex; ++index) {
                const bdem_IdxOffsetMapEntry& e =
                                              row.d_rowDef->d_offsetMap[index];
                bdem_ConstElemRef elemRef(data + e.d_offset, e.d_elemAttr);
                bitmap |= bit * elemRef.isSet();
                bit >>= 1;
            }
            stream.putUint32(bitmap);

            for (int index = startIndex; index < endIndex; ++index) {
                const bdem_IdxOffsetMapEntry& e =
                                              row.d_rowDef->d_offsetMap[index];
                bdem_ConstElemRef elemRef(data + e.d_offset, e.d_elemAttr);
                if (elemRef.isSet()) {
                    const bdem_DescriptorStreamOut<STREAM> *elemStrmAttr =
                                     &strmAttrLookup[e.d_elemAttr->d_elemEnum];
                    elemStrmAttr->streamOut(data + e.d_offset,
                                            stream,
                                            1,
                                            strmAttrLookup);
                }
            }

            startIndex = endIndex;
        }
      } break;

      case 1: {
        const int len = row.d_rowDef->d_offsetMap.length();
        const char *data = ((char *)&row) + HEADER_SIZE;
        for (int index = 0; index < len; ++index) {
            const bdem_IdxOffsetMapEntry& e = row.d_rowDef->d_offsetMap[index];
            const bdem_DescriptorStreamOut<STREAM> *elemStrmAttr =
                                     &strmAttrLookup[e.d_elemAttr->d_elemEnum];
            elemStrmAttr->streamOut(data + e.d_offset,
                                    stream,
                                    1,
                                    strmAttrLookup);
        }
      } break;
    }

    return stream;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
