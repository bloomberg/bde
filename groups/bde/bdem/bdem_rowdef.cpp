// bdem_rowdef.cpp     -*-C++-*-

#include <bdem_rowdef.h>

#include <bdes_objectbuffer.h>
#include <bdema_autodestructor.h>
#include <bdeu_print.h>

#include <cassert>
#include <ostream>

namespace BloombergLP {

                        // ---------
                        // CONSTANTS
                        // ---------

enum {
    GROW_FACTOR = 2  // multiplicative factor by which to grow 'd_size'
};

                        // ==========================
                        // class bdem_RowDef_AutoDtor
                        // ==========================

class bdem_RowDef_AutoDtor {
    // This class implements a proctor that, unless its 'release' method is
    // invoked, automatically *destroys* its managed objects at destruction.
    // By *destroy* we mean that each object's destructor is called
    // explicitly.  The managed objects are a sequence of elements within
    // a 'bdem_Row'.

  private:
    bdem_RowHeader *d_row;         // row that contains managed elements
    int             d_startIndex;  // starting index
    int             d_length;      // number of objects to manage

  private:
    // not implemented
    bdem_RowDef_AutoDtor(const bdem_RowDef_AutoDtor&);
    bdem_RowDef_AutoDtor& operator=(const bdem_RowDef_AutoDtor&);

  public:
    // CREATORS
    bdem_RowDef_AutoDtor(bdem_RowHeader *row,
                         int             index,
                         int             length);
        // Create a proctor object to manage a sequence of 'bdem_Row'
        // objects.  The range of the managed objects is defined by the
        // specified 'origin' and 'length'.  A positive 'length' represents
        // the sequence of objects starting at 'origin' and extending "up" to
        // 'length' (*not* including the object at the index position 'origin'
        // + 'length').
        //..
        //              length = 2
        //              |<----->|
        //       ___ ___ ___ ___ ___
        //      | 0 | 1 | 2 | 3 | 4 |
        //      `===^===^===^===^==='
        //              ^----------- origin
        //..
        // The behavior is undefined unless all objects within the managed
        // range (as defined above) are valid.

    ~bdem_RowDef_AutoDtor();
        // destroy all managed objects.  Note that the order in which the
        // managed objects are destroyed is undefined.

    // MANIPULATORS
    void release();
        // Release from management all objects currently managed by this
        // proctor.

    bdem_RowDef_AutoDtor& operator++();
        // Increase by one the length of the sequence of objects
        // managed by this proctor.
};

                        // ----------------
                        // INLINE FUNCTIONS
                        // ----------------

inline
bdem_RowDef_AutoDtor::bdem_RowDef_AutoDtor(bdem_RowHeader *row,
                                           int             index,
                                           int             length)
: d_row(row)
, d_startIndex(index)
, d_length(length)
{
}

inline
bdem_RowDef_AutoDtor::~bdem_RowDef_AutoDtor()
{
    if (d_length) {
        for (int i = d_startIndex; i < (d_startIndex + d_length); ++i) {
            d_row->d_rowDef->elemAttr(i)->
                                destroy(d_row->d_rowDef->rowElemData(d_row,i));
        }
    }
}

inline
void bdem_RowDef_AutoDtor::release()
{
    d_length = 0;
}

inline
bdem_RowDef_AutoDtor& bdem_RowDef_AutoDtor::operator++()
{
    ++d_length;
    return *this;
}

// ACCESSORS
bdem_RowHeader *bdem_RowDef::constructRow(void *memory) const
{
    bdem_RowHeader *row = (bdem_RowHeader *)memory;
    row->d_rowDef = (bdem_RowDef *)this;
    bdem_RowDef_AutoDtor autoDtor(row, 0, 0);

    char *data = ((char *)row) + HEADER_SIZE;
    int len = d_offsetMap.length();

    // Invoke the default constructor on all new elements.
    for (int index = 0; index < len; ++index) {
        const bdem_IdxOffsetMapEntry& e = d_offsetMap[index];
        e.d_elemAttr->unsetConstruct(data + e.d_offset, d_allocMode,
                                    d_allocator);
        ++autoDtor;
    }

    autoDtor.release();
    return row;
}

bdem_RowHeader *bdem_RowDef::
                            constructRow(void                  *memory,
                                         const bdem_RowHeader&  original,
                                         int                    srcIndex) const
{
    bdem_RowHeader *row = (bdem_RowHeader *)memory;
    row->d_rowDef = (bdem_RowDef *)this;
    bdem_RowDef_AutoDtor autoDtor(row, 0, 0);

    char *data = ((char *)row) + HEADER_SIZE;
    char *originalData = ((char *)&original) + HEADER_SIZE;
    int len = d_offsetMap.length();

    // Invoke the default constructor on all new elements.
    const bdem_RowDef *originalRowDef = original.d_rowDef;
    for (int index = 0; index < len; ++index) {
        const bdem_IdxOffsetMapEntry& entry = d_offsetMap[index];
        const bdem_IdxOffsetMapEntry& originalEntry =
                          originalRowDef->d_offsetMap[index+srcIndex];

        assert(originalEntry.d_elemAttr->d_elemEnum ==
                                                 entry.d_elemAttr->d_elemEnum);


        entry.d_elemAttr->copyConstruct(data + entry.d_offset,
                                        originalData + originalEntry.d_offset,
                                        d_allocMode,
                                        d_allocator);
        ++autoDtor;
    }

    autoDtor.release();
    return row;
}

void bdem_RowDef::destroyRow(bdem_RowHeader *row)
{
    const bdem_RowDef *def = row->d_rowDef;
    assert(def);
    int len = def->d_offsetMap.length();
    assert(len >= 0);
    char *data = ((char *)row) + HEADER_SIZE;
    for (int i = 0; i < len; ++i) {
        const bdem_IdxOffsetMapEntry& e = def->d_offsetMap[i];
        e.d_elemAttr->destroy(data + e.d_offset);
    }
}

void bdem_RowDef::adjustRowSize(bdem_RowHeader **row_pp, int newDataSize)
{
    assert(row_pp);
    assert(*row_pp);

    bdem_RowDef *def = (*row_pp)->d_rowDef;
    int oldSize = def->d_rowSize;
    int newSizeMin = (newDataSize + HEADER_SIZE + ALIGNMENT - 1) &
                    ~(ALIGNMENT - 1);

    if (oldSize < newSizeMin) {
        int newSize = oldSize;
        while (newSize < newSizeMin) {
            newSize = newSize * GROW_FACTOR;
        };

        bdema_Allocator *allocator = def->d_allocator;
        bdem_RowHeader *newRow =
                                (bdem_RowHeader *)allocator->allocate(newSize);
        int oldLength = def->d_offsetMap.totalOffset();
        std::memcpy(newRow, *row_pp, oldLength + HEADER_SIZE);
        allocator->deallocate(*row_pp);
        def->d_rowSize = newSize;
        *row_pp = newRow;
    }
}

void bdem_RowDef::compactRow(bdem_RowHeader **row_pp)
{
    bdem_RowHeader *oldRow = *row_pp;
    bdem_RowDef *oldRowDef = oldRow->d_rowDef;
    bdes_ObjectBuffer<bdem_RowDef> newRowDefBuf;

    bdem_RowDef *newRowDef = &newRowDefBuf.object();
    new (newRowDef) bdem_RowDef(*oldRowDef,
                                oldRowDef->d_allocMode,
                                oldRowDef->d_allocator);
    bdema_AutoDestructor<bdem_RowDef> rowdefAutoDtor(newRowDef, 1);

    bdem_RowHeader *newRow = newRowDef->constructRow(*oldRow);

    rowdefAutoDtor.release();


    if (!(bdem_AggregateOption::NODESTRUCT_FLAG & oldRowDef->d_allocMode)) {
        deleteRow(oldRow);
    }

    oldRowDef->~bdem_RowDef();
    bdeimp_BitwiseCopy<bdem_RowDef>::copy(oldRowDef, newRowDef);
    newRow->d_rowDef=oldRowDef;

    *row_pp = newRow;
}


void  bdem_RowDef::insertElements(bdem_RowHeader        **row_pp,
                                  int                     dstIndex,
                                  const bdem_RowHeader&   srcRow,
                                  int                     srcIndex,
                                  int                     numElements)
{
    bdem_RowDef *def = (*row_pp)->d_rowDef;

    bdem_RowDef *srcRowDef = srcRow.d_rowDef;

    bdem_RowDef tempRowDef(srcRowDef->d_offsetMap,
                           srcIndex,
                           numElements,
                           def->d_allocMode,
                           def->d_allocator);

    bdem_RowHeader *tempRow = tempRowDef.constructRow(srcRow, srcIndex);
    bdema_AutoDeallocator<bdema_Allocator>
                                     rowAutoDealloc(tempRow, def->d_allocator);
    bdem_RowDef_AutoDtor autoDtor(tempRow, 0, numElements);
    int newDataSize;
    def->d_offsetMap.adjustedTotalOffset(&newDataSize,
                                         srcRow.d_rowDef->d_offsetMap,
                                         srcIndex,
                                         numElements);
    def->d_offsetMap.reserveCapacity(def->d_offsetMap.length() + numElements);
    adjustRowSize(row_pp, newDataSize);
    char *data = ((char *)*row_pp) + HEADER_SIZE;

    for(int i = 0; i < numElements; i++) {
        int insertIndex = dstIndex + i;
        const bdem_Descriptor *elemAttr =
                                 tempRowDef.d_offsetMap[i].d_elemAttr;
        int elemOffset;
        def->d_offsetMap.insertionOffset(&elemOffset, elemAttr);
        void *elemData = data + elemOffset;
        const void *srcElemData = tempRowDef.rowElemData(tempRow, i);
        std::memcpy(elemData, srcElemData, elemAttr->d_size);
        def->d_offsetMap.insert(insertIndex, elemAttr);
    }

    autoDtor.release();
}

void bdem_RowDef::
            insertUnsetElements(bdem_RowHeader               **row_pp,
                                int                            dstIndex,
                                const bdem_ElemType::Type      elementTypes[],
                                int                            numElements,
                                const bdem_Descriptor *const   attrLookupTbl[])
{
    bdem_RowDef *def = (*row_pp)->d_rowDef;

    int newDataSize;
    def->d_offsetMap.adjustedTotalOffset(&newDataSize,
                                         elementTypes,
                                         numElements,
                                         attrLookupTbl);
    def->d_offsetMap.reserveCapacity(def->d_offsetMap.length() + numElements);
    adjustRowSize(row_pp, newDataSize);
    char *data = ((char *)*row_pp) + HEADER_SIZE;

    for(int i = 0; i < numElements; i++) {
        int insertIndex = dstIndex + i;
        const bdem_Descriptor *elemAttr = attrLookupTbl[elementTypes[i]];
        int elemOffset;
        def->d_offsetMap.insertionOffset(&elemOffset, elemAttr);
        void *elemData = data + elemOffset;
        elemAttr->unsetConstruct(elemData, def->d_allocMode, def->d_allocator);
        def->d_offsetMap.insert(insertIndex, elemAttr);
    }
}

void *bdem_RowDef::insertElementRaw(bdem_RowHeader        **row_pp,
                                    int                     dstIndex,
                                    const bdem_Descriptor  *elemAttr)
{
    bdem_RowDef *def = (*row_pp)->d_rowDef;
    int newDataSize;
    def->d_offsetMap.adjustedTotalOffset(&newDataSize, elemAttr);

    adjustRowSize(row_pp, newDataSize);
    def->d_offsetMap.insert(dstIndex, elemAttr);
    return rowElemData(*row_pp, dstIndex);
}

void *bdem_RowDef::insertElement(bdem_RowHeader        **row_pp,
                                 int                     dstIndex,
                                 const void             *value,
                                 const bdem_Descriptor  *elemAttr)
{
    bdem_RowDef *def = (*row_pp)->d_rowDef;

    if (def->rowSize() > (HEADER_SIZE + 128 * def->numElements())) {
       compactRow(row_pp); 
    }
    // Compute what the length and offset attributes of the row will be
    // after we insert into it.
    int newDataSize;
    def->d_offsetMap.adjustedTotalOffset(&newDataSize, elemAttr);
    int elemOffset;
    def->d_offsetMap.insertionOffset(&elemOffset, elemAttr);

    // Make room in the map.  This does not change the logical value of the
    // map, so there is no special exception-handling logic.
    def->d_offsetMap.reserveCapacity(def->numElements()+1);

    // Make room in the row.  This does not change the logical value of the
    // row, so there is no special exception-handling logic.
    if ((char*) *row_pp <= (char*) value &&
        (char*) value < (char*) *row_pp + def->rowSize())
    {
        // Value being inserted is within this row (i.e., we are inserting a
        // second copy).  Remember the value's offset within the row,
        // resize the row, and then reconstruct the value pointer from the
        // new row pointer and the offset.
        unsigned valueOffset = (char*) value - (char*) *row_pp;
        adjustRowSize(row_pp, newDataSize);
        value = (char*) *row_pp + valueOffset;
    }
    else
    {
        adjustRowSize(row_pp, newDataSize);
    }

    // Copy construct the value into the row.
    void *elemData = ((char *)*row_pp) + HEADER_SIZE + elemOffset;
    elemAttr->copyConstruct(elemData, value,
                            def->d_allocMode,
                            def->d_allocator);

    // Only now do we logically insert the element into the row definition.
    // This insert cannot throw an exception.
    def->d_offsetMap.insert(dstIndex, elemAttr);

    return elemData;
}

void *bdem_RowDef::insertUnsetElement(bdem_RowHeader        **row_pp,
                                      int                     dstIndex,
                                      const bdem_Descriptor  *elemAttr)
{
    bdem_RowDef *def = (*row_pp)->d_rowDef;

    int newDataSize;
    def->d_offsetMap.adjustedTotalOffset(&newDataSize, elemAttr);
    int elemOffset;
    def->d_offsetMap.insertionOffset(&elemOffset, elemAttr);
    def->d_offsetMap.reserveCapacity(def->d_offsetMap.length()+1);

    adjustRowSize(row_pp, newDataSize);

    void *elemData = ((char *)*row_pp) + HEADER_SIZE + elemOffset;
    elemAttr->unsetConstruct(elemData,
                             def->d_allocMode,
                             def->d_allocator);
    def->d_offsetMap.insert(dstIndex, elemAttr);
    return elemData;
}

void bdem_RowDef::replaceValues(bdem_RowHeader        *row,
                                int                    dstIndex,
                                const bdem_RowHeader&  srcRow,
                                int                    srcIndex,
                                int                    numElements)
{
    bdem_RowDef *def = row->d_rowDef;

    assert(dstIndex >= 0 &&
           dstIndex + numElements <= def->d_offsetMap.length());
    assert(srcIndex >= 0 &&
           srcIndex + numElements <= srcRow.d_rowDef->d_offsetMap.length());
    assert(numElements >= 0);

    if (row != &srcRow || dstIndex != srcIndex) {
        char *myData = ((char *)row) + HEADER_SIZE;
        const char *srcData =  ((char *)&srcRow) + HEADER_SIZE;
        const int endIndex = srcIndex + numElements;

        // If either row contains any aggregate, a possibility of overlap
        // must be considered.
        // 1) Allocate temporary space equal in size to max space used in
        //    row.  However, only those columns being modified will be
        //    used.
        // 2) Insert new data to temporary copy using copy constructor.
        // 3) Destroy existing copy (run dtor on all modified elements).
        // 4) Copy temporary items back to row with std::memcpy.
        // 5) Deallocate temporary space.

        // Allocate temporary space for the row.

        const int sizeNeeded = (HEADER_SIZE + def->d_offsetMap.totalOffset() +
                                             ALIGNMENT - 1) & ~(ALIGNMENT - 1);
        bdema_Allocator *internalAllocator = def->d_allocator;
        bdem_RowHeader *tempRow =
                     (bdem_RowHeader *)internalAllocator->allocate(sizeNeeded);
        tempRow->d_rowDef = def;
        char *tempData=((char *)tempRow) + HEADER_SIZE;

        bdema_AutoDeallocator<bdema_Allocator>
                                       autoDealloc(tempRow, internalAllocator);

        // Copy elements one by one using copy ctor.  In the event of
        // an exception all the elements already constructed are
        // destroyed.

        bdem_RowDef_AutoDtor autoDtor(tempRow, dstIndex, 0);

        int dindex = dstIndex;
        for (int index = srcIndex; index < endIndex; ++index, ++dindex) {
            const bdem_IdxOffsetMapEntry& myMapEntry =
                                             def->d_offsetMap[dindex];
            const bdem_IdxOffsetMapEntry& srcMapEntry =
                                  srcRow.d_rowDef->d_offsetMap[index];

            assert(srcMapEntry.d_elemAttr->d_elemEnum ==
                                            myMapEntry.d_elemAttr->d_elemEnum);

            myMapEntry.d_elemAttr->
                                  copyConstruct(tempData + myMapEntry.d_offset,
                                                srcData + srcMapEntry.d_offset,
                                                def->d_allocMode,
                                                internalAllocator);
            ++autoDtor;
        }

        autoDtor.release();

        // Run dtor on all existing elements before performing a std::memcpy
        // from the temporary copy.  No exceptions are expected in the
        // dtor.

        int endDstIndex = dstIndex + numElements;
        for (int index = dstIndex; index < endDstIndex; ++index) {
            const bdem_IdxOffsetMapEntry& myMapEntry = def->d_offsetMap[index];
            char *destPtr = myData + myMapEntry.d_offset;
            myMapEntry.d_elemAttr->destroy(destPtr);
            std::memcpy(destPtr,
                   tempData + myMapEntry.d_offset,
                   myMapEntry.d_elemAttr->d_size);
        }

        // Automatically deallocate tempBuffer using the autodeallocator.
    }
}

void bdem_RowDef::removeElement(bdem_RowHeader **row_pp, int index)
{
    bdem_RowDef *def = (*row_pp)->d_rowDef;

    assert(index >= 0);
    assert(index < def->d_offsetMap.length());

    const bdem_IdxOffsetMapEntry& e = def->d_offsetMap[index];
    e.d_elemAttr->destroy(((char *)*row_pp) + HEADER_SIZE + e.d_offset);

    def->d_offsetMap.remove(index);
}

void bdem_RowDef::removeElements(bdem_RowHeader **row_pp,
                                 int              startIndex,
                                 int              numElements)
{
    bdem_RowDef *def = (*row_pp)->d_rowDef;

    assert(startIndex >= 0);
    assert(numElements >= 0);
    assert(startIndex + numElements <= def->d_offsetMap.length());

    const int endIndex = startIndex + numElements;

    for (int index = startIndex; index < endIndex; ++index) {
        const bdem_IdxOffsetMapEntry& e = def->d_offsetMap[index];
        e.d_elemAttr->destroy(((char *)*row_pp) + HEADER_SIZE + e.d_offset);
    }

    def->d_offsetMap.remove(startIndex, numElements);
}

void bdem_RowDef::removeAllElements(bdem_RowHeader **row_pp)
{
    bdem_RowDef *def = (*row_pp)->d_rowDef;

    if ( !(bdem_AggregateOption::NODESTRUCT_FLAG & def->d_allocMode) ) {

        const int endIndex = def->d_offsetMap.length();

        for (int index = 0; index < endIndex; ++index) {
            const bdem_IdxOffsetMapEntry& e = def->d_offsetMap[index];
            e.d_elemAttr->
                         destroy(((char *)*row_pp) + HEADER_SIZE + e.d_offset);
        }
    }
    def->d_offsetMap.removeAll();
}

void bdem_RowDef::makeAllUnset(bdem_RowHeader *row)
{
    bdem_RowDef *def = row->d_rowDef;

    makeRangeUnset(row, 0, def->d_offsetMap.length());
}

void bdem_RowDef::
           makeRangeUnset(bdem_RowHeader *row, int startIndex, int numElements)
{
    bdem_RowDef *def = row->d_rowDef;

    assert(startIndex >= 0);
    assert(numElements >= 0);

    char *data = ((char *)row) + HEADER_SIZE;

    const int endIndex = startIndex + numElements;
    for (int i = startIndex; i < endIndex; ++i) {
        const bdem_IdxOffsetMapEntry& e = def->d_offsetMap[i];
        e.d_elemAttr->makeUnset(data + e.d_offset);
    }
}

int bdem_RowDef::isAnyInRangeSet(const bdem_RowHeader& row,
                                 int                   startIndex,
                                 int                   numElements)
{
    bdem_RowDef *def = row.d_rowDef;

    const int endIndex = startIndex + numElements;
    const char *data = ((char *)&row) + HEADER_SIZE;
    for (int i = startIndex; i < endIndex; ++i) {
        const bdem_IdxOffsetMapEntry& e = def->d_offsetMap[i];
        if (! e.d_elemAttr->isUnset(data + e.d_offset)) {
            return 1;
        }
    }
    return 0;
}

int bdem_RowDef::isAnyInRangeUnset(const bdem_RowHeader& row,
                                   int                   startIndex,
                                   int                   numElements)
{
    bdem_RowDef *def = row.d_rowDef;

    assert(startIndex >= 0);
    assert(numElements >= 0);
    assert(startIndex + numElements <=
                        def->d_offsetMap.length());
    int endIndex = startIndex + numElements;
    const char *data = ((char *)&row) + HEADER_SIZE;
    for (int i = startIndex; i < endIndex; ++i) {
        const bdem_IdxOffsetMapEntry& e = def->d_offsetMap[i];
        if (e.d_elemAttr->isUnset(data + e.d_offset)) {
            return 1;
        }
    }
    return 0;
}

int bdem_RowDef::areRowsEqualRaw(const bdem_RowHeader& lhs,
                                 const bdem_RowHeader& rhs)
{
    if (&lhs == &rhs) return 1;

    const int len = lhs.d_rowDef->d_offsetMap.length();
    const char *lhsData = ((char *)&lhs) + HEADER_SIZE;
    const char *rhsData = ((char *)&rhs) + HEADER_SIZE;

    for (int index = 0; index < len; ++index) {
        const bdem_IdxOffsetMapEntry& lhsEntry =
                                     lhs.d_rowDef->d_offsetMap[index];
        const bdem_IdxOffsetMapEntry& rhsEntry =
                                     rhs.d_rowDef->d_offsetMap[index];

        assert(rhsEntry.d_elemAttr->d_elemEnum ==
                                              lhsEntry.d_elemAttr->d_elemEnum);

        if (!lhsEntry.d_elemAttr->areEqual(lhsData + lhsEntry.d_offset,
                                           rhsData + rhsEntry.d_offset)) {
            return 0;
        }
    }
    return 1;
}

int bdem_RowDef::areRowsEqual(const bdem_RowHeader& lhs,
                              const bdem_RowHeader& rhs)
{
    if (&lhs == &rhs) {
        return 1;
    }

    if (!lhs.d_rowDef->
                    d_offsetMap.areElemTypesEqual(rhs.d_rowDef->d_offsetMap)) {
        return 0;
    }

    return areRowsEqualRaw(lhs, rhs);
}

void bdem_RowDef::setColumnValueRaw(bdem_RowHeader *rows[],
                                    int             numRows,
                                    int             colIdx,
                                    const void     *value)
{
    if (0 == numRows) {
        return;
    }

    bdem_RowDef *def = (*rows)->d_rowDef;

    const bdem_IdxOffsetMapEntry& e = def->d_offsetMap[colIdx];
    int size = e.d_elemAttr->d_size;

    int offset = HEADER_SIZE + e.d_offset;

    for (int i = 0; i < numRows; ++i) {
        char *row = (char *)rows[i];
        void *itemData = row + offset;
        std::memcpy(itemData,value,size);
    }
}

void bdem_RowDef::setColumnValue(bdem_RowHeader *rows[],
                                 int             numRows,
                                 int             colIdx,
                                 const void     *value)
{
    if (0 == numRows) {
        return;
    }

    bdem_RowDef *def = (*rows)->d_rowDef;

    const bdem_IdxOffsetMapEntry& e = def->d_offsetMap[colIdx];
    int offset = HEADER_SIZE + e.d_offset;

    // Look up function for assigning this element type.
    void (*assign)(void *lhs, const void *rhs);
    assign = e.d_elemAttr->assign;

    // There is a danger that the value is the actual table being modified is
    // indirectly contained within one of the elements being modified.  If
    // this is the case, then the value might be altered or destroyed after
    // the first or subsequent assignment.  For this reason, we assign it only
    // once, into row 0, and thereafter use row 0 as our starting value.
    void *rowZeroElemData = ((char *)rows[0]) + offset;
    assign(rowZeroElemData,value);
    for (int i = 1; i < numRows; ++i) {
        char *row = (char *)rows[i];
        void *itemData = row + offset;
        assign(itemData,rowZeroElemData);
    }
}

void bdem_RowDef::makeColumnUnset(bdem_RowHeader *rows[],
                                  int             numRows,
                                  int             colIdx)
{
    if (0 == numRows) {
        return;
    }

    bdem_RowDef *def = (*rows)->d_rowDef;

    const bdem_IdxOffsetMapEntry& e = def->d_offsetMap[colIdx];
    int offset = HEADER_SIZE + e.d_offset;

    // Look up function for assigning this element type.
    void (*makeUnset)(void *obj);
    makeUnset = e.d_elemAttr->makeUnset;

    for (int i = 0; i < numRows; ++i) {
        char *row = (char *)rows[i];
        void *itemData = row + offset;
        makeUnset(itemData);
    }
}

std::ostream& bdem_RowDef::printRow(std::ostream&         stream,
                                    const bdem_RowHeader& row,
                                    int                   level,
                                    int                   spacesPerLevel,
                                    bool                  columnLabels)
{
    int nestedLevel, nestedSpacesPerLevel;

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '{';

    if (level < 0) {
        level = -level;
    }

    if (0 <= spacesPerLevel) {
        // Newlines only if spacesPerLevel is non-negative
        stream << '\n';
        nestedLevel = level + 1;
        nestedSpacesPerLevel = spacesPerLevel;
    }
    else {
        // If spacesPerLevel is negative, just put one space between fields
        // and supress newlines when formatting each field.
        nestedLevel = 1;
        nestedSpacesPerLevel = -1;
    }

    const int len = row.d_rowDef->d_offsetMap.length();
    const char *data = ((char *)&row) + HEADER_SIZE;

    for (int index = 0; index < len; ++index) {
        const bdem_IdxOffsetMapEntry& e = row.d_rowDef->d_offsetMap[index];
        const bdem_Descriptor *elemAttr = e.d_elemAttr;
        bdem_ElemType::Type elemTypeEnum =
            static_cast<bdem_ElemType::Type>(elemAttr->d_elemEnum);

        bdeu_Print::indent(stream, nestedLevel, nestedSpacesPerLevel);

        if (columnLabels) {
            stream << elemTypeEnum << ' ';
        }

        elemAttr->print(data + e.d_offset, stream, -nestedLevel,
                        nestedSpacesPerLevel);
    }

    if (0 <= spacesPerLevel) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "}\n";
    }
    else {
        stream << " }";
    }


    return stream << std::flush;
}

                        // --------------
                        // ROW DESCRIPTOR
                        // --------------

namespace {

void makeRowUnset(void *obj) {
    bdem_RowDef::makeAllUnset(static_cast<bdem_RowHeader*>(obj));
}

int isRowUnset(const void *obj) {
    // Return true only if row has zero length.
    return ! bdem_RowDef::rowLength(*static_cast<const bdem_RowHeader*>(obj));
}

int areRowsEqual(const void *lhs, const void *rhs) {
    const bdem_RowHeader& row1 = *static_cast<const bdem_RowHeader*>(lhs);
    const bdem_RowHeader& row2 = *static_cast<const bdem_RowHeader*>(rhs);

    return bdem_RowDef::areRowsEqual(row1, row2);
}

std::ostream& printRow(const void    *obj,
                       std::ostream&  stream,
                       int            level,
                       int            spacesPerLevel) {
    return bdem_RowDef::printRow(stream,
                                 *static_cast<const bdem_RowHeader*>(obj),
                                 level, spacesPerLevel);
}

} // Close unnamed namespace

const bdem_Descriptor bdem_RowHeader::d_rowAttr = {
    bdem_ElemType::ROW,                      // element enumeration
    sizeof(bdem_RowHeader),                  // [minimum] size
    bdes_AlignmentOf<bdem_RowHeader>::VALUE, // alignment
    0,                                       // unsetConstruct,
    0,                                       // copyConstruct
    0,                                       // destroy
    0,                                       // assign
    0,                                       // move
    &makeRowUnset,                           // makeUnset
    &isRowUnset,                             // isUnset
    &areRowsEqual,                           // areEqual
    &printRow                                // print
};

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
