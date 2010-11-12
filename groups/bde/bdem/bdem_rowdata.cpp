// bdem_rowdata.cpp                                                   -*-C++-*-
#include <bdem_rowdata.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_rowdata_cpp,"$Id$ $CSID$")

#include <bdem_properties.h>   // for testing only
#include <bdeu_print.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bsl_climits.h>    // 'CHAR_BIT'
#include <bsl_cstring.h>    // 'bsl::memcpy', 'bsl::memset'
#include <bsl_ostream.h>

namespace BloombergLP {

// LOCAL CONSTANTS
enum {
    // This 'enum' defines constants used by this component.

    GROWTH_FACTOR     = 2,                      // multiplicative growth factor
    BDEM_BITS_PER_INT = sizeof(int) * CHAR_BIT  // number of bits per 'int'
};

// STATIC HELPER FUNCTIONS
static
int calculateDataFootprintSize(int dataSize)
    // Return the size (in bytes) that will yield a maximally-aligned memory
    // block sufficient to hold a 'bdem_RowData' of 'dataSize' bytes.  The
    // behavior is undefined unless '0 <= dataSize'.
{
    BSLS_ASSERT(0 <= dataSize);

    dataSize = (dataSize + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
                               & ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);

    return dataSize ? dataSize : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
}

static
int calculateNullnessBitsArraySize(int numElements)
    // Return the size of the memory block (in bytes) that is sufficient to
    // represent the nullness bits for a 'bdem_RowData' having the specified
    // 'numElements'.  The behavior is undefined unless '0 <= numElements'.
    // Note that the value returned is a non-zero multiple of 'sizeof(int)'.
{
    BSLS_ASSERT(0 <= numElements);

    int minNumBytes = (numElements + BDEM_BITS_PER_INT - 1)
                                             / BDEM_BITS_PER_INT * sizeof(int);

    minNumBytes = (minNumBytes + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
                               & ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);

    return minNumBytes ? minNumBytes : bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;
}

                        // ------------------------
                        // ROW DESCRIPTOR FUNCTIONS
                        // ------------------------

static
void makeRowUnset(void *obj)
{
    BSLS_ASSERT(obj);

    static_cast<bdem_RowData *>(obj)->makeAllNull();
}

static
bool isRowUnset(const void *obj)
{
    BSLS_ASSERT(obj);

    const bdem_RowData& row = *static_cast<const bdem_RowData *>(obj);
    const int           len = row.length();

    return !row.isAnyInRangeNonNull(0, len);
}

static
bool areRowsEqual(const void *lhs, const void *rhs)
{
    BSLS_ASSERT(lhs);
    BSLS_ASSERT(rhs);

    const bdem_RowData& lhsRow = *static_cast<const bdem_RowData *>(lhs);
    const bdem_RowData& rhsRow = *static_cast<const bdem_RowData *>(rhs);

    return lhsRow == rhsRow;
}

static
bsl::ostream& printRowHeader(const void    *obj,
                             bsl::ostream&  stream,
                             int            level,
                             int            spacesPerLevel)
{
    BSLS_ASSERT(obj);

    return static_cast<const bdem_RowData *>(obj)->printRow(stream,
                                                            level,
                                                            spacesPerLevel);
}

                        // ===========================
                        // class bdem_RowData_AutoDtor
                        // ===========================

class bdem_RowData_AutoDtor {
    // This 'class' implements a proctor that, unless its 'release' method is
    // invoked, automatically *destroys* its managed objects at destruction.
    // By *destroy* we mean that each object's destructor is called explicitly.
    // The managed objects are a sequence of elements within a 'bdem_RowData'.

    // DATA
    void                 *d_rowData_p;    // row that contains managed elements
    const bdem_RowLayout *d_rowLayout_p;  // row layout
    int                   d_startIndex;   // starting index
    int                   d_length;       // number of objects to manage

    // NOT IMPLEMENTED
    bdem_RowData_AutoDtor(const bdem_RowData_AutoDtor&);
    bdem_RowData_AutoDtor& operator=(const bdem_RowData_AutoDtor&);

  public:
    // CREATORS
    bdem_RowData_AutoDtor(void                 *rowData,
                          const bdem_RowLayout *rowLayout,
                          int                   index,
                          int                   length);
        // Create a proctor to manage allocated memory for the specified
        // 'length' elements starting at the specified 'index' in the specified
        // 'rowData'.  The behavior is undefined unless all objects within the
        // managed range (as defined above) are valid.  Note that a positive
        // 'length' represents that the sequence of objects starting at
        // 'index' and extending "up" to 'length' (*not* including the object
        // at the index position 'index + length') should be managed.
        //..
        //          length = 2
        //          |<----->|
        //   ___ ___ ___ ___ ___
        //  | 0 | 1 | 2 | 3 | 4 |
        //  `===^===^===^===^==='
        //          ^----------- index
        //..

    ~bdem_RowData_AutoDtor();
        // Destroy all managed objects.  Note that the order in which the
        // managed objects are destroyed is undefined.

    // MANIPULATORS
    void release();
        // Release from management all objects currently managed by this
        // proctor.

    bdem_RowData_AutoDtor& operator++();
        // Increase by one the length of the sequence of objects managed by
        // this proctor.
};

                        // ---------------------------
                        // class bdem_RowData_AutoDtor
                        // ---------------------------

// CREATORS
inline
bdem_RowData_AutoDtor::bdem_RowData_AutoDtor(void                 *rowData,
                                             const bdem_RowLayout *rowLayout,
                                             int                   index,
                                             int                   length)
: d_rowData_p(rowData)
, d_rowLayout_p(rowLayout)
, d_startIndex(index)
, d_length(length)
{
}

bdem_RowData_AutoDtor::~bdem_RowData_AutoDtor()
{
    if (d_length) {
        const int endIndex = d_startIndex + d_length;

        for (int i = d_startIndex; i < endIndex; ++i) {
            const bdem_RowLayoutEntry& entry = (*d_rowLayout_p)[i];
            entry.attributes()->destroy((char *) d_rowData_p + entry.offset());
        }
    }
}

// MANIPULATORS
inline
void bdem_RowData_AutoDtor::release()
{
    d_length = 0;
}

inline
bdem_RowData_AutoDtor& bdem_RowData_AutoDtor::operator++()
{
    ++d_length;
    return *this;
}

                        // ------------------
                        // class bdem_RowData
                        // ------------------

// CLASS DATA
const bdem_Descriptor bdem_RowData::d_rowAttr =
{
    bdem_ElemType::BDEM_ROW,                           // element enumeration
    sizeof(bdem_RowData),                              // [minimum] size
    bsls_AlignmentFromType<bdem_RowData>::VALUE,       // alignment
    0,                                                 // unsetConstruct,
    0,                                                 // copyConstruct
    0,                                                 // destroy
    0,                                                 // assign
    0,                                                 // move
    &makeRowUnset,                                     // makeUnset
    &isRowUnset,                                       // isUnset
    &areRowsEqual,                                     // areEqual
    &printRowHeader                                    // print
};

// PRIVATE MANIPULATORS
void bdem_RowData::constructData(void                 *dstRowData,
                                 const bdem_RowLayout *rowLayout)
{
    BSLS_ASSERT(dstRowData);
    BSLS_ASSERT(rowLayout);

    bdem_RowData_AutoDtor elementsProctor(dstRowData, rowLayout, 0, 0);
    const int numElements = rowLayout->length();
    for (int i = 0; i < numElements; ++i) {
        const bdem_RowLayoutEntry&  entry      = (*rowLayout)[i];
        const bdem_Descriptor      *descriptor = entry.attributes();
        const int                   offset     = entry.offset();
        void                       *elemData   =
                                        (void *)((char *) dstRowData + offset);

        descriptor->unsetConstruct(elemData,
                                   d_allocationStrategy,
                                   d_allocator_p);
        ++elementsProctor;
    }

    elementsProctor.release();
}

void bdem_RowData::constructData(void                 *dstRowData,
                                 const bdem_RowLayout *rowLayout,
                                 const bdem_RowData&   srcRowData,
                                 int                   startIndex,
                                 int                   numElements)
{
    BSLS_ASSERT(dstRowData);
    BSLS_ASSERT(rowLayout);
    BSLS_ASSERT(0 <= startIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(numElements == rowLayout->length());
    BSLS_ASSERT(startIndex + numElements <= srcRowData.length());

    bdem_RowData_AutoDtor elementsProctor(dstRowData, rowLayout, 0, 0);
    for (int i = 0; i < numElements; ++i) {
        const bdem_RowLayoutEntry&  dstEntry      = (*rowLayout)[i];
        const bdem_Descriptor      *dstDescriptor = dstEntry.attributes();
        const int                   dstOffset     = dstEntry.offset();
        void                       *dstData       =
                                     (void *)((char *) dstRowData + dstOffset);

        dstDescriptor->copyConstruct(dstData,
                                     srcRowData.elemData(startIndex + i),
                                     d_allocationStrategy,
                                     d_allocator_p);
        ++elementsProctor;
    }

    elementsProctor.release();
}

void bdem_RowData::init(const bdem_RowLayout *rowLayout)
{
    BSLS_ASSERT(rowLayout);

    int   newBitsSize = calculateNullnessBitsArraySize(rowLayout->length());
    int   newDataSize = calculateDataFootprintSize(rowLayout->totalOffset());

    void *newData     = d_allocator_p->allocate(newDataSize);

    bslma_DeallocatorProctor<bslma_Allocator> dataProctor(newData,
                                                          d_allocator_p);

    void *newBitsArray = d_allocator_p->allocate(newBitsSize);

    bslma_DeallocatorProctor<bslma_Allocator> bitsArrayProctor(newBitsArray,
                                                               d_allocator_p);

    BSLS_ASSERT(newData);
    BSLS_ASSERT(newBitsArray);

    bsl::memset(newData, 0, newDataSize);
    bsl::memset(newBitsArray, 0, newBitsSize);

    constructData(newData, rowLayout);

    dataProctor.release();
    bitsArrayProctor.release();

    if (d_rowData_p) {
        d_allocator_p->deallocate(d_rowData_p);
        d_allocator_p->deallocate(d_nullnessBitsArray_p);
    }

    d_rowData_p             = newData;
    d_nullnessBitsArray_p   = (int *) newBitsArray;
    d_dataSize              = newDataSize;
    d_nullnessBitsArraySize = newBitsSize;
    d_rowLayout_p           = rowLayout;

    bdeu_BitstringUtil::set(d_nullnessBitsArray_p,
                            0,
                            true,
                            length());
}

void bdem_RowData::init(const bdem_RowLayout *rowLayout,
                        const bdem_RowData&   srcRowData,
                        int                   startIndex,
                        int                   numElements)
{
    BSLS_ASSERT(rowLayout);
    BSLS_ASSERT(0 <= startIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(numElements == rowLayout->length());
    BSLS_ASSERT(startIndex + numElements <= srcRowData.length());

    int newBitsSize = calculateNullnessBitsArraySize(rowLayout->length());
    int newDataSize = calculateDataFootprintSize(rowLayout->totalOffset());

    void *newData = d_allocator_p->allocate(newDataSize);

    bslma_DeallocatorProctor<bslma_Allocator> dataProctor(newData,
                                                          d_allocator_p);

    void *newBitsArray = d_allocator_p->allocate(newBitsSize);

    bslma_DeallocatorProctor<bslma_Allocator> bitsArrayProctor(newBitsArray,
                                                               d_allocator_p);

    BSLS_ASSERT(newData);
    BSLS_ASSERT(newBitsArray);

    bsl::memset(newData, 0, newDataSize);
    bsl::memset(newBitsArray, 0, newBitsSize);

    constructData(newData, rowLayout, srcRowData, startIndex, numElements);

    dataProctor.release();
    bitsArrayProctor.release();

    if (d_rowData_p) {
        d_allocator_p->deallocate(d_rowData_p);
        d_allocator_p->deallocate(d_nullnessBitsArray_p);
    }

    d_rowData_p             = newData;
    d_nullnessBitsArray_p   = (int *) newBitsArray;
    d_dataSize              = newDataSize;
    d_nullnessBitsArraySize = newBitsSize;
    d_rowLayout_p           = rowLayout;

    bdeu_BitstringUtil::copyRaw(d_nullnessBitsArray_p,
                                0,
                                srcRowData.d_nullnessBitsArray_p,
                                startIndex,
                                numElements);
}

void bdem_RowData::resize(int numElements, int dataSize)
{
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(0 <= dataSize);

    int newNullnessBitsSize = calculateNullnessBitsArraySize(numElements);
    int newDataSize         = calculateDataFootprintSize(dataSize);

    if (d_dataSize < newDataSize) {
        int memoryAllocSize = d_dataSize;
        while (memoryAllocSize < newDataSize) {
            memoryAllocSize *= GROWTH_FACTOR;
        }

        void *newData = d_allocator_p->allocate(memoryAllocSize);

        // Do a 'memcpy'; all 'bdem' types are bitwise moveable.

        bsl::memcpy(newData, d_rowData_p, d_dataSize);

        d_allocator_p->deallocate(d_rowData_p);

        d_rowData_p = newData;
        d_dataSize  = newDataSize;
    }

    if (d_nullnessBitsArraySize < newNullnessBitsSize) {
        int nullnessBitsSize = d_nullnessBitsArraySize;

        while (nullnessBitsSize < newNullnessBitsSize) {
            nullnessBitsSize *= GROWTH_FACTOR;
        }

        int *newNullnessBitsArray = (int *) d_allocator_p->allocate(
                                                             nullnessBitsSize);
        bsl::memcpy((char *) newNullnessBitsArray,
                    (char *) d_nullnessBitsArray_p,
                    d_nullnessBitsArraySize);

        // Set all the new nullness bits to 0.

        bsl::memset((char *) newNullnessBitsArray + d_nullnessBitsArraySize,
                    0,
                    nullnessBitsSize - d_nullnessBitsArraySize);

        d_allocator_p->deallocate(d_nullnessBitsArray_p);

        d_nullnessBitsArray_p   = newNullnessBitsArray;
        d_nullnessBitsArraySize = newNullnessBitsSize;
    }
}

// PRIVATE ACCESSORS
bool bdem_RowData::isDataEqual(const bdem_RowData& rhs) const
{
    const int len = length();

    if (!bdeu_BitstringUtil::areEqual(d_nullnessBitsArray_p,
                                      0,
                                      rhs.d_nullnessBitsArray_p,
                                      0,
                                      len)) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < len; ++i) {
        const bdem_RowLayoutEntry& lhsEntry = (*d_rowLayout_p)[i];
        const bdem_RowLayoutEntry& rhsEntry = (*rhs.d_rowLayout_p)[i];

        BSLS_ASSERT(lhsEntry.attributes()->d_elemEnum ==
                                            rhsEntry.attributes()->d_elemEnum);

        if (!lhsEntry.attributes()->areEqual(elemData(i), rhs.elemData(i))) {
            return false;                                             // RETURN
        }
    }
    return true;
}

// CREATORS
bdem_RowData::bdem_RowData(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_rowLayout_p(0)
, d_rowData_p(0)
, d_nullnessBitsArray_p(0)
, d_dataSize(0)
, d_nullnessBitsArraySize(0)
, d_allocationStrategy(allocationStrategy)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

bdem_RowData::bdem_RowData(
                  const bdem_RowLayout                     *rowLayout,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_rowLayout_p(0)
, d_rowData_p(0)
, d_nullnessBitsArray_p(0)
, d_dataSize(0)
, d_nullnessBitsArraySize(0)
, d_allocationStrategy(allocationStrategy)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(rowLayout);

    init(rowLayout);
}

bdem_RowData::bdem_RowData(
                  const bdem_RowLayout                     *rowLayout,
                  const bdem_RowData&                       other,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_rowLayout_p(0)
, d_rowData_p(0)
, d_nullnessBitsArray_p(0)
, d_dataSize(0)
, d_nullnessBitsArraySize(0)
, d_allocationStrategy(allocationStrategy)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(rowLayout);
    BSLS_ASSERT_SAFE(rowLayout->areElemTypesEqual(*other.rowLayout()));

    init(rowLayout, other, 0, rowLayout->length());
}

bdem_RowData::bdem_RowData(
                  const bdem_RowLayout                     *rowLayout,
                  const bdem_RowData&                       other,
                  int                                       startIndex,
                  int                                       numElements,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_rowLayout_p(0)
, d_rowData_p(0)
, d_nullnessBitsArray_p(0)
, d_dataSize(0)
, d_nullnessBitsArraySize(0)
, d_allocationStrategy(allocationStrategy)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    BSLS_ASSERT(rowLayout);
    BSLS_ASSERT(0 <= startIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(numElements == rowLayout->length());
    BSLS_ASSERT(startIndex + numElements <= other.length());

    init(rowLayout, other, startIndex, numElements);
}

bdem_RowData::~bdem_RowData()
{
    if (!(bdem_AggregateOption::BDEM_NODESTRUCT_FLAG & d_allocationStrategy)) {
        reset();
        d_allocator_p->deallocate(d_rowData_p);
        d_allocator_p->deallocate(d_nullnessBitsArray_p);
    }
}

// MANIPULATORS
bdem_RowData& bdem_RowData::operator=(const bdem_RowData& rhs)
{
    BSLS_ASSERT_SAFE(rowLayout()->areElemTypesEqual(*rhs.rowLayout()));

    if (&rhs != this) {
        bdem_RowData newRow(d_rowLayout_p,
                            rhs,
                            d_allocationStrategy,
                            d_allocator_p);

        bdeimp_BitwiseCopy<bdem_RowData>::swap(this, &newRow);
    }

    return *this;
}

bdem_ElemRef bdem_RowData::elemRef(int index)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());

    const bdem_RowLayoutEntry& e = (*d_rowLayout_p)[index];
    return bdem_ElemRef((void *)((char *) d_rowData_p + e.offset()),
                        e.attributes(),
                        d_nullnessBitsArray_p + (index / BDEM_BITS_PER_INT),
                        index % BDEM_BITS_PER_INT);
}

void *bdem_RowData::insertElement(int dstIndex, const void *value)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());
    BSLS_ASSERT(value);

    // This function assumes that the types have already been added to
    // 'd_rowLayout_p'.

    const int newLength      = d_rowLayout_p->length();
    const int newTotalOffset = d_rowLayout_p->totalOffset();

    if ((char *) d_rowData_p <= (const char *) value
     && (const char *) value < (char *) d_rowData_p + d_dataSize) {
        // Value being inserted is within this row (i.e., we are inserting a
        // second copy).  Remember the value's offset within the row, resize
        // the row, and then reconstruct the value pointer from the new row
        // pointer and the offset.

        const int valueOffset =
                 static_cast<int>((const char *) value - (char *) d_rowData_p);
        resize(newLength, newTotalOffset);
        value = (const void *)((char *) d_rowData_p + valueOffset);
    }
    else {
        resize(newLength, newTotalOffset);
    }

    void *data = elemData(dstIndex);
    const bdem_Descriptor *descriptor =
                                       (*d_rowLayout_p)[dstIndex].attributes();

    descriptor->copyConstruct(data,
                              value,
                              d_allocationStrategy,
                              d_allocator_p);

    bdeu_BitstringUtil::insert0(d_nullnessBitsArray_p,
                                newLength - 1,
                                dstIndex,
                                1);

    return data;
}

void *bdem_RowData::insertElementRaw(int dstIndex)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    // This function assumes that the types have already been added to
    // 'd_rowLayout_p'.

    const int newLength      = d_rowLayout_p->length();
    const int newTotalOffset = d_rowLayout_p->totalOffset();

    resize(newLength, newTotalOffset);

    void *data = elemData(dstIndex);

    bdeu_BitstringUtil::insert0(d_nullnessBitsArray_p,
                                newLength - 1,
                                dstIndex,
                                1);

    return data;
}

void bdem_RowData::insertElements(int                 dstIndex,
                                  const bdem_RowData& srcRowData,
                                  int                 srcIndex,
                                  int                 numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcRowData.length());

    // This function assumes that the types have already been added to
    // 'd_rowLayout_p'.

    const int newLength      = d_rowLayout_p->length();
    const int newTotalOffset = d_rowLayout_p->totalOffset();

    resize(newLength, newTotalOffset);

    bdem_RowData_AutoDtor proctor(d_rowData_p, d_rowLayout_p, dstIndex, 0);
    for (int i = 0; i < numElements; ++i) {
        const bdem_RowLayoutEntry& dstEntry = (*d_rowLayout_p)[dstIndex + i];
        const bdem_RowLayoutEntry& srcEntry =
                                     (*srcRowData.d_rowLayout_p)[srcIndex + i];

        BSLS_ASSERT(srcEntry.attributes()->d_elemEnum ==
                                            dstEntry.attributes()->d_elemEnum);

        void *dstElemData = (void *)((char *) d_rowData_p + dstEntry.offset());
        void *srcElemData =
                 (void *)((char *) srcRowData.d_rowData_p + srcEntry.offset());

        dstEntry.attributes()->copyConstruct(dstElemData,
                                             srcElemData,
                                             d_allocationStrategy,
                                             d_allocator_p);
        ++proctor;
    }

    proctor.release();

    bdeu_BitstringUtil::insertRaw(d_nullnessBitsArray_p,
                                  newLength - numElements,
                                  dstIndex,
                                  numElements);
    bdeu_BitstringUtil::copyRaw(d_nullnessBitsArray_p,
                                dstIndex,
                                srcRowData.d_nullnessBitsArray_p,
                                srcIndex,
                                numElements);
}

void *bdem_RowData::insertNullElement(int dstIndex)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    // This function assumes that the types have already been added to
    // 'd_rowLayout_p'.

    const int newLength      = d_rowLayout_p->length();
    const int newTotalOffset = d_rowLayout_p->totalOffset();

    resize(newLength, newTotalOffset);

    void *data =
          (void *)((char *) d_rowData_p + (*d_rowLayout_p)[dstIndex].offset());

    (*d_rowLayout_p)[dstIndex].attributes()->unsetConstruct(
                                                          data,
                                                          d_allocationStrategy,
                                                          d_allocator_p);

    bdeu_BitstringUtil::insert1(d_nullnessBitsArray_p,
                                newLength - 1,
                                dstIndex,
                                1);

    return data;
}

void bdem_RowData::insertNullElements(int dstIndex, int numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());
    BSLS_ASSERT(0 <= numElements);

    // This function assumes that the types have already been added to
    // 'd_rowLayout_p'.

    const int newLength      = d_rowLayout_p->length();
    const int newTotalOffset = d_rowLayout_p->totalOffset();

    resize(newLength, newTotalOffset);

    bdem_RowData_AutoDtor elementsProctor(d_rowData_p,
                                          d_rowLayout_p,
                                          dstIndex,
                                          0);

    for (int i = 0; i < numElements; ++i) {
        const bdem_RowLayoutEntry&  entry    = (*d_rowLayout_p)[dstIndex + i];
        void                       *elemData =
                               (void *)((char *) d_rowData_p + entry.offset());

        entry.attributes()->unsetConstruct(elemData,
                                           d_allocationStrategy,
                                           d_allocator_p);
        ++elementsProctor;
    }

    elementsProctor.release();

    bdeu_BitstringUtil::insert1(d_nullnessBitsArray_p,
                                newLength - numElements,
                                dstIndex,
                                numElements);
}

void bdem_RowData::makeNull(int index)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());

    elemRef(index).makeNull();
}

void bdem_RowData::makeAllNull()
{
    const int len = length();
    for (int i = 0; i < len; ++i) {
        const bdem_RowLayoutEntry& e = (*d_rowLayout_p)[i];
        e.attributes()->makeUnset(elemData(i));
    }

    bdeu_BitstringUtil::set(d_nullnessBitsArray_p, 0, true, len);
}

void bdem_RowData::removeElement(int index)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());

    // This function assumes that the type at 'index' has *not* yet been
    // removed from 'd_rowLayout_p'.

    const bdem_RowLayoutEntry& e = (*d_rowLayout_p)[index];
    e.attributes()->destroy((void *)((char *)d_rowData_p + e.offset()));

    bdeu_BitstringUtil::remove(d_nullnessBitsArray_p,
                               length() + 1,
                               index,
                               1);
}

void bdem_RowData::removeElements(int startIndex, int numElements)
{
    BSLS_ASSERT(0 <= startIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(startIndex + numElements <= length());

    // This function assumes that the types have *not* yet been removed from
    // 'd_rowLayout_p'.

    if (!(bdem_AggregateOption::BDEM_NODESTRUCT_FLAG & d_allocationStrategy)) {
        const int endIndex = startIndex + numElements;
        for (int i = startIndex; i < endIndex; ++i) {
            const bdem_RowLayoutEntry& e = (*d_rowLayout_p)[i];
            e.attributes()->destroy(
                                   (void *)((char *)d_rowData_p + e.offset()));
        }
    }

    bdeu_BitstringUtil::remove(d_nullnessBitsArray_p,
                               length() + numElements,
                               startIndex,
                               numElements);
}

void bdem_RowData::replaceValues(const bdem_RowData& other)
{
    BSLS_ASSERT(d_rowLayout_p->areElemTypesEqual(*other.rowLayout()));

    const int len = length();
    for (int i = 0; i < len; ++i) {
        const bdem_RowLayoutEntry& e = (*d_rowLayout_p)[i];
        e.attributes()->assign(elemData(i), other.elemData(i));
    }

    bdeu_BitstringUtil::copyRaw(d_nullnessBitsArray_p,
                                0,
                                other.d_nullnessBitsArray_p,
                                0,
                                len);
}

void bdem_RowData::reset()
{
    if (d_rowLayout_p) {
       if (!(bdem_AggregateOption::BDEM_NODESTRUCT_FLAG
                                                     & d_allocationStrategy)) {
            const int numElements = d_rowLayout_p->length();
            for (int i = 0; i < numElements; ++i) {
                const bdem_RowLayoutEntry&  entry      = (*d_rowLayout_p)[i];
                const bdem_Descriptor      *descriptor = entry.attributes();
                const int                   offset     = entry.offset();
                void                       *elemData   =
                                       (void *)((char *) d_rowData_p + offset);
                descriptor->destroy(elemData);
            }
        }
        d_rowLayout_p = 0;
    }
}

void bdem_RowData::reset(const bdem_RowLayout *rowLayout)
{
    BSLS_ASSERT(rowLayout);

    reset();

    init(rowLayout);
}

void bdem_RowData::reset(const bdem_RowLayout *rowLayout,
                         const bdem_RowData&   srcRowData)
{
    BSLS_ASSERT(rowLayout);
    BSLS_ASSERT_SAFE(rowLayout->areElemTypesEqual(*srcRowData.rowLayout()));

    reset();

    init(rowLayout, srcRowData, 0, srcRowData.length());
}

void bdem_RowData::setValue(int index, const void *value)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());
    BSLS_ASSERT(value);

    const bdem_RowLayoutEntry& e = (*d_rowLayout_p)[index];
    e.attributes()->assign((void *)((char *)d_rowData_p + e.offset()), value);
    bdeu_BitstringUtil::set(d_nullnessBitsArray_p, index, false, 1);
}

void bdem_RowData::swap(bdem_RowData& other)
{
    if (this != &other) {
        BSLS_ASSERT(d_allocationStrategy == other.d_allocationStrategy);
        BSLS_ASSERT(d_allocator_p        == other.d_allocator_p);

        bdeimp_BitwiseCopy<bdem_RowData>::swap(this, &other);
    }
}

void bdem_RowData::swapElements(int index1, int index2)
{
    BSLS_ASSERT(0 <= index1);
    BSLS_ASSERT(     index1 < length());
    BSLS_ASSERT(0 <= index2);
    BSLS_ASSERT(     index2 < length());

    // In general, 'bdeu_BitstringUtil::swapRaw' is not alias-safe, but it
    // works correctly for 1 bit.

    bdeu_BitstringUtil::swapRaw(d_nullnessBitsArray_p,
                                index1,
                                d_nullnessBitsArray_p,
                                index2,
                                1);
}

// ACCESSORS
bdem_ConstElemRef bdem_RowData::elemRef(int index) const
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());

    const bdem_RowLayoutEntry& e = (*d_rowLayout_p)[index];
    return bdem_ConstElemRef(
                           (void *)((char *) d_rowData_p + e.offset()),
                           e.attributes(),
                           d_nullnessBitsArray_p + (index / BDEM_BITS_PER_INT),
                           index % BDEM_BITS_PER_INT);
}

bool bdem_RowData::isAnyInRangeNonNull(int startIndex, int numElements) const
{
    BSLS_ASSERT(0 <= startIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(startIndex + numElements <= length());

    return 0 == numElements
           ? false
           : bdeu_BitstringUtil::isAny0(d_nullnessBitsArray_p,
                                        startIndex,
                                        numElements);
}

bool bdem_RowData::isAnyInRangeNull(int startIndex, int numElements) const
{
    BSLS_ASSERT(0 <= startIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(startIndex + numElements <= length());

    return 0 == numElements
           ? false
           : bdeu_BitstringUtil::isAny1(d_nullnessBitsArray_p,
                                        startIndex,
                                        numElements);
}

bsl::ostream& bdem_RowData::printRow(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel,
                                     bool          columnLabels) const
{
    int nestedLevel, nestedSpacesPerLevel;

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '{';

    if (level < 0) {
        level = -level;
    }

    if (0 <= spacesPerLevel) {

        // Emit newlines only if 'spacesPerLevel' is non-negative.

        stream << '\n';
        nestedLevel = level + 1;
        nestedSpacesPerLevel = spacesPerLevel;
    }
    else {

        // If 'spacesPerLevel' is negative, just put one space between fields
        // and suppress newlines when formatting each field.

        nestedLevel = 1;
        nestedSpacesPerLevel = -1;
    }

    const int   len  = length();
    const char *data = (char *) d_rowData_p;

    for (int index = 0; index < len; ++index) {
        const bdem_RowLayoutEntry&  e            = (*d_rowLayout_p)[index];
        const bdem_Descriptor      *elemAttr     = e.attributes();
        bdem_ElemType::Type         elemTypeEnum =
                        static_cast<bdem_ElemType::Type>(elemAttr->d_elemEnum);

        bdeu_Print::indent(stream, nestedLevel, nestedSpacesPerLevel);

        if (columnLabels) {
            stream << elemTypeEnum << ' ';
        }

        if (bdeu_BitstringUtil::get(d_nullnessBitsArray_p, index)) {
            stream << "NULL";
            if (nestedSpacesPerLevel >= 0) {
                stream << "\n";
            }
        }
        else {
            elemAttr->print(data + e.offset(),
                            stream,
                            -nestedLevel,
                            nestedSpacesPerLevel);
        }
    }

    if (0 <= spacesPerLevel) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "}\n";
    }
    else {
        stream << " }";
    }

    return stream << bsl::flush;
}

// FREE OPERATORS
bool operator==(const bdem_RowData& lhs, const bdem_RowData& rhs)
{
    return lhs.rowLayout()->areElemTypesEqual(*rhs.rowLayout())
        && lhs.isDataEqual(rhs);
}

bool operator!=(const bdem_RowData& lhs, const bdem_RowData& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
