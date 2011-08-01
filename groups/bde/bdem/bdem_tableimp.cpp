// bdem_tableimp.cpp                                                  -*-C++-*-
#include <bdem_tableimp.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_tableimp_cpp,"$Id$ $CSID$")

#include <bdem_functiontemplates.h>
#include <bdeu_print.h>

#include <bslma_autorawdeleter.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_rawdeleterproctor.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

namespace BloombergLP {
// LOCAL VARIABLES

static bool bdem_TableImp_geometricMemoryGrowthFlag = false;

// LOCAL CONSTANTS
enum {
    BDEM_BITS_PER_INT = 32  // the number of bits in an 'int'
};

// STATIC HELPER FUNCTIONS
static
int computeRowSize(const bdem_RowLayout *layout)
{
    int rowSize = sizeof(bdem_RowData) + layout->totalOffset();

    rowSize += (layout->length() + BDEM_BITS_PER_INT - 1)
                                             / BDEM_BITS_PER_INT * sizeof(int);
    return (rowSize + bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
                               & ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
}


    static
int nullBitsArraySize(int numBits)
    // Return the size of the null bits array required to store at least
    // the specified 'numBits'.  The behavior is undefined unless
    // '0 <= numBits'.
{
    BSLS_ASSERT(0 <= numBits);

    const int arraySize = (numBits + BDEM_BITS_PER_INT - 1) /
                                                             BDEM_BITS_PER_INT;

    return arraySize ? arraySize : 1;
}

                        // =============================
                        // class bdem_TableImp_AttrFuncs
                        // =============================

struct bdem_TableImp_AttrFuncs {
    // Namespace for static methods to store in a 'bdem_Descriptor' structure.

    // CLASS METHODS
    static
    void defaultConstruct(
                  void                                     *obj,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc);
        // Construct a table object into raw memory.  The prototype for this
        // function must match the defaultConstruct function pointer in
        // bdem_Descriptor.

    static
    void copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc);
        // Copy construct a table object into raw memory.  The prototype for
        // this function must match the copyConstruct function pointer in
        // bdem_Descriptor.

    static
    bool isEmpty(const void *obj);
        // Return 'true' if table has no rows and no columns, and 'false'
        // otherwise.

    static
    bsl::ostream& print(const void    *obj,
                        bsl::ostream&  stream,
                        int            level,
                        int            spacesPerLevel);
        // Print this table.
};

                        // -----------------------------
                        // class bdem_TableImp_AttrFuncs
                        // -----------------------------

// CLASS METHODS
void bdem_TableImp_AttrFuncs::defaultConstruct(
                  void                                     *obj,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc)
{
    BSLS_ASSERT(obj);

    new (obj) bdem_TableImp(allocationStrategy, alloc);
}

void bdem_TableImp_AttrFuncs::copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc)
{
    BSLS_ASSERT(obj);
    BSLS_ASSERT(rhs);

    const bdem_TableImp& rhsTable = *static_cast<const bdem_TableImp *>(rhs);
    new (obj) bdem_TableImp(rhsTable, allocationStrategy, alloc);
}

bool bdem_TableImp_AttrFuncs::isEmpty(const void *obj)
{
    BSLS_ASSERT(obj);

    const bdem_TableImp& theTable = *static_cast<const bdem_TableImp *>(obj);
    return 0 == theTable.numRows() && 0 == theTable.numColumns();
}

bsl::ostream& bdem_TableImp_AttrFuncs::print(const void    *obj,
                                             bsl::ostream&  stream,
                                             int            level,
                                             int            spacesPerLevel)
{
    BSLS_ASSERT(obj);

    static_cast<const bdem_TableImp *>(obj)->print(stream,
                                                   level,
                                                   spacesPerLevel);
    return stream;
}

                        // -------------------
                        // class bdem_TableImp
                        // -------------------

// CLASS DATA
const bdem_Descriptor bdem_TableImp::d_tableAttr =
{
    bdem_ElemType::BDEM_TABLE,
    sizeof(bdem_TableImp),
    bsls_AlignmentFromType<bdem_TableImp>::VALUE,
    &bdem_TableImp_AttrFuncs::defaultConstruct,
    &bdem_TableImp_AttrFuncs::copyConstruct,
    &bdem_FunctionTemplates::destroy<bdem_TableImp>,
    &bdem_FunctionTemplates::assign<bdem_TableImp>,
    &bdem_FunctionTemplates::bitwiseMove<bdem_TableImp>,
    &bdem_FunctionTemplates::removeAll<bdem_TableImp>,
    &bdem_TableImp_AttrFuncs::isEmpty,
    &bdem_FunctionTemplates::areEqual<bdem_TableImp>,
    &bdem_TableImp_AttrFuncs::print
};

// CREATORS
bdem_TableImp::bdem_TableImp(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        bdem_RowLayout(d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    bslma_AutoRawDeleter<bdem_RowLayout, bslma_Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

bdem_TableImp::bdem_TableImp(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        bdem_RowLayout(d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= initialMemorySize);

    bslma_AutoRawDeleter<bdem_RowLayout, bslma_Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

bdem_TableImp::bdem_TableImp(
               const bdem_ElemType::Type                 columnTypes[],
               int                                       numColumns,
               const bdem_Descriptor                    *const *attrLookupTbl,
               bdem_AggregateOption::AllocationStrategy  allocationStrategy,
               bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        bdem_RowLayout(columnTypes,
                                       numColumns,
                                       attrLookupTbl,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= numColumns);

    bslma_AutoRawDeleter<bdem_RowLayout, bslma_Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

bdem_TableImp::bdem_TableImp(
               const bdem_ElemType::Type                 columnTypes[],
               int                                       numColumns,
               const bdem_Descriptor                    *const *attrLookupTbl,
               bdem_AggregateOption::AllocationStrategy  allocationStrategy,
               int                                       initialMemorySize,
               bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        bdem_RowLayout(columnTypes,
                                       numColumns,
                                       attrLookupTbl,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= numColumns);
    BSLS_ASSERT(0 <= initialMemorySize);

    bslma_AutoRawDeleter<bdem_RowLayout, bslma_Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

bdem_TableImp::bdem_TableImp(
                  const bdem_RowLayout&                     rowLayout,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        bdem_RowLayout(rowLayout,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    bslma_AutoRawDeleter<bdem_RowLayout, bslma_Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

bdem_TableImp::bdem_TableImp(
                  const bdem_RowLayout&                     rowLayout,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        bdem_RowLayout(rowLayout,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= initialMemorySize);

    bslma_AutoRawDeleter<bdem_RowLayout, bslma_Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

bdem_TableImp::bdem_TableImp(
                  const bdem_TableImp&                      original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        bdem_RowLayout(*original.d_rowLayout_p,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    bslma_AutoRawDeleter<bdem_RowLayout, bslma_Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits = original.d_nullBits;

    const int numRows = original.d_rows.size();

    d_rows.resize(numRows);
    d_rowPool.reserveCapacity(numRows);

    if (numRows > 0) {
        bslma_AutoRawDeleter<bdem_RowData, bdema_Pool>
                                        rowsAutoDel(&d_rows[0], &d_rowPool, 0);

        for (int i = 0; i < numRows; ++i) {
            d_rows[i] = new (d_rowPool) bdem_RowData(
                                       d_rowLayout_p,
                                       *original.d_rows[i],
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.internalAllocator());
            ++rowsAutoDel;
        }

        rowsAutoDel.release();
    }

    proctor.release();
}

bdem_TableImp::bdem_TableImp(
                  const bdem_TableImp&                      original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        bdem_RowLayout(*original.d_rowLayout_p,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.internalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= initialMemorySize);

    bslma_AutoRawDeleter<bdem_RowLayout, bslma_Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits = original.d_nullBits;

    const int numRows = original.d_rows.size();

    d_rows.resize(numRows);
    d_rowPool.reserveCapacity(numRows);

    if (numRows > 0) {
        bslma_AutoRawDeleter<bdem_RowData, bdema_Pool>
                                        rowsAutoDel(&d_rows[0], &d_rowPool, 0);

        for (int i = 0; i < numRows; ++i) {
            d_rows[i] = new (d_rowPool) bdem_RowData(
                                       d_rowLayout_p,
                                       *original.d_rows[i],
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.internalAllocator());
            ++rowsAutoDel;
        }

        rowsAutoDel.release();
    }

    proctor.release();
}

bdem_TableImp::bdem_TableImp(
                  const bdem_RowData&                       rowData,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        bdem_RowLayout(*rowData.rowLayout(),
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.internalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    bslma_AutoRawDeleter<bdem_RowLayout, bslma_Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

bdem_TableImp::bdem_TableImp(
                  const bdem_RowData&                       rowData,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        bdem_RowLayout(*rowData.rowLayout(),
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.internalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= initialMemorySize);

    bslma_AutoRawDeleter<bdem_RowLayout, bslma_Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

bdem_TableImp::~bdem_TableImp()
{
    if (!(bdem_AggregateOption::BDEM_NODESTRUCT_FLAG &
                                    d_allocatorManager.allocationStrategy())) {
        // No pooling or write-once options.  Delete every row.

        const int numRows = d_rows.size();
        for (int i = 0; i < numRows; ++i) {
            d_rows[i]->~bdem_RowData();
        }
        d_allocatorManager.internalAllocator()->deleteObject(d_rowLayout_p);
    }
}

// MANIPULATORS
bdem_TableImp& bdem_TableImp::operator=(const bdem_TableImp& rhs)
{
    // Create 'newTable' as copy of the rhs, but using the same
    // allocation options as were used to construct '*this'.  Then swap
    // '*this' with the temporary table.  The old value of '*this' will
    // be destroyed when 'newTable' goes out of scope.

    if (this != &rhs) {
        bdem_TableImp newTable(rhs,
                               d_allocatorManager.allocationStrategy(),
                               d_allocatorManager.originalAllocator());

        bdeimp_BitwiseCopy<bdem_TableImp>::swap(this, &newTable);
    }

    return *this;
}

void bdem_TableImp::makeRowsNull(int startRowIndex, int numRows)
{
    BSLS_ASSERT(0 <= startRowIndex);
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(startRowIndex + numRows <= this->numRows());

    const int endIndex = startRowIndex + numRows;
    for (int i = startRowIndex; i < endIndex; ++i) {
        d_rows[i]->makeAllNull();
    }
    bdeu_BitstringUtil::set(&d_nullBits.front(),
                            startRowIndex,
                            true,
                            numRows);
}

void bdem_TableImp::removeRow(int rowIndex)
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());

    bdem_RowData *rowData = d_rows[rowIndex];
    rowData->~bdem_RowData();
    d_rowPool.deallocate(rowData);
    d_rows.erase(d_rows.begin() + rowIndex);
    bdeu_BitstringUtil::removeAndFill0(&d_nullBits.front(),
                                       d_rows.size() + 1,
                                       rowIndex,
                                       1);
}

bsl::size_t bdem_TableImp::getRowsCapacity() const 
{
    return d_rows.capacity();
}

void bdem_TableImp::reserveRowsRaw(bsl::size_t numRows)
{
    d_allocatorManager.reserveMemory(8 * numRows);
    d_rowPool.reserveCapacity(numRows);
    d_rows.reserve(d_rows.size() + numRows);
}

void bdem_TableImp::reset(const bdem_ElemType::Type  columnTypes[],
                          int                        numColumns,
                          const bdem_Descriptor     *const *attrLookupTbl)
{
    BSLS_ASSERT(0 <= numColumns);

    bdem_TableImp newTable(columnTypes,
                           numColumns,
                           attrLookupTbl,
                           d_allocatorManager.allocationStrategy(),
                           d_allocatorManager.originalAllocator());

    bdeimp_BitwiseCopy<bdem_TableImp>::swap(this, &newTable);
}

bdem_RowData& bdem_TableImp::insertRow(int                 dstRowIndex,
                                       const bdem_RowData& srcRow)
{
    BSLS_ASSERT(0 <= dstRowIndex);
    BSLS_ASSERT(     dstRowIndex <= numRows());

    const int newSize = nullBitsArraySize(d_rows.size() + 1);
    if ((int)d_nullBits.size() < newSize) {
        d_nullBits.resize(newSize, 0);
    }

    if (!bdem_TableImp_geometricMemoryGrowthFlag) {
        d_rows.reserve(numRows() + 1);
        d_rowPool.reserveCapacity(1);
    }

    bdem_RowData *newRow = new (d_rowPool) bdem_RowData(
                                       d_rowLayout_p,
                                       srcRow,
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.internalAllocator());

    d_rows.insert(d_rows.begin() + dstRowIndex, newRow);

    // Make the newly-added row non-null, since 'srcRow' can't indicate
    // otherwise.

    bdeu_BitstringUtil::insert0(&d_nullBits.front(),
                                d_rows.size() - 1,
                                dstRowIndex,
                                1);

    return *d_rows[dstRowIndex];
}

void bdem_TableImp::insertRows(int                  dstRowIndex,
                               const bdem_TableImp& srcTable,
                               int                  srcRowIndex,
                               int                  numRows)
{
    BSLS_ASSERT(0 <= dstRowIndex);
    BSLS_ASSERT(     dstRowIndex <= this->numRows());
    BSLS_ASSERT(0 <= srcRowIndex);
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(srcRowIndex + numRows <= srcTable.numRows());

    if (0 == numRows) {
        return;                                                       // RETURN
    }

    const int originalSize = this->numRows();
    const int newSize      = nullBitsArraySize(originalSize + numRows);
    if ((int)d_nullBits.size() < newSize) {
        d_nullBits.resize(newSize, 0);
    }

    // Management of nullness bits gets complicated in the presence of
    // aliasing.  We take a straightforward conservative approach: If there is
    // a *potential* for aliasing, the nullness bits of the source rows are
    // copied to a temporary.  Note that 'bsl::vector' does not incur an
    // allocation if its initial size is 0 (the case where no alias is
    // possible, the most common case).

    const bool isAliasPossible = this == &srcTable;  // keep it simple
    bsl::vector<int> tempNullBits(isAliasPossible ? nullBitsArraySize(numRows)
                                                  : 0);

    // 'tempRows' is used to address aliasing concerns.


    if (!bdem_TableImp_geometricMemoryGrowthFlag) {
        d_rows.reserve(originalSize + numRows);
        d_rowPool.reserveCapacity(numRows);
    }
   
    bsl::vector<bdem_RowData *> tempRows;
    tempRows.resize(numRows);

    bslma_AutoRawDeleter<bdem_RowData, bdema_Pool>
                                      rowsAutoDel(&tempRows[0], &d_rowPool, 0);

    for (int i = 0; i < numRows; ++i) {
        tempRows[i] = new (d_rowPool) bdem_RowData(
                                       d_rowLayout_p,
                                       *srcTable.d_rows[srcRowIndex + i],
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.internalAllocator());
        ++rowsAutoDel;
    }

    d_rows.insert(d_rows.begin() + dstRowIndex,
                  tempRows.begin(),
                  tempRows.end());

    // Can't throw from here on.

    rowsAutoDel.release();

    if (isAliasPossible) {

        // Copy nullness bits of source rows to temporary.

        bdeu_BitstringUtil::copyRaw(&tempNullBits.front(),
                                    0,
                                    &srcTable.d_nullBits.front(),
                                    srcRowIndex,
                                    numRows);
    }

    // Open up space for nullness bits of new rows.

    bdeu_BitstringUtil::insertRaw(&d_nullBits.front(),
                                  originalSize,
                                  dstRowIndex,
                                  numRows);

    if (isAliasPossible) {
        bdeu_BitstringUtil::copyRaw(&d_nullBits.front(),
                                    dstRowIndex,
                                    &tempNullBits.front(),
                                    0,
                                    numRows);
    }
    else {
        bdeu_BitstringUtil::copyRaw(&d_nullBits.front(),
                                    dstRowIndex,
                                    &srcTable.d_nullBits.front(),
                                    srcRowIndex,
                                    numRows);
    }
}

void bdem_TableImp::insertNullRows(int dstRowIndex, int numRows)
{
    BSLS_ASSERT(0 <= dstRowIndex);
    BSLS_ASSERT(     dstRowIndex <= this->numRows());
    BSLS_ASSERT(0 <= numRows);

    if (0 == numRows) {
        return;                                                       // RETURN
    }

    const int newSize = nullBitsArraySize(this->numRows() + numRows);
    if ((int)d_nullBits.size() < newSize) {
        d_nullBits.resize(newSize, 0);
    }

    
    if (!bdem_TableImp_geometricMemoryGrowthFlag) {
        d_rowPool.reserveCapacity(numRows);
        d_rows.reserve(this->numRows() + numRows);
    }

    for (int i = 0; i < numRows; ++i) {
        // If allocate() or constructor throws an exception,
        // previously-inserted rows will be unaffected and table will remain
        // in a valid state.

        bdem_RowData *newRow = new (d_rowPool) bdem_RowData(
                                       d_rowLayout_p,
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.internalAllocator());

        d_rows.insert(d_rows.begin() + dstRowIndex + i, newRow);

        bdeu_BitstringUtil::insert1(&d_nullBits.front(),
                                    d_rows.size() - 1,
                                    dstRowIndex + i,
                                    1);
    }
}

void bdem_TableImp::removeRows(int rowIndex, int numRows)
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(rowIndex + numRows <= this->numRows());

    for (int i = 0; i < numRows; ++i) {
        bdem_RowData *rowData = d_rows[rowIndex + i];
        rowData->~bdem_RowData();
        d_rowPool.deallocate(rowData);
    }

    d_rows.erase(d_rows.begin() + rowIndex,
                 d_rows.begin() + rowIndex + numRows);

    const int newLen = d_rows.size();

    if (0 < newLen) {
        bdeu_BitstringUtil::removeAndFill0(&d_nullBits.front(),
                                           d_rows.size() + numRows,
                                           rowIndex,
                                           numRows);
    }

    d_nullBits.resize(nullBitsArraySize(newLen));
}

void bdem_TableImp::removeAll()
{
    bdem_TableImp newTable(d_allocatorManager.allocationStrategy(),
                           d_allocatorManager.originalAllocator());

    bdeimp_BitwiseCopy<bdem_TableImp>::swap(this, &newTable);
}

bdem_ElemRef bdem_TableImp::rowElemRef(int rowIndex)
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());

    const int arrayIdx = rowIndex / BDEM_BITS_PER_INT;
    const int offset   = rowIndex % BDEM_BITS_PER_INT;

    return bdem_ElemRef(d_rows[rowIndex],
                        &bdem_RowData::d_rowAttr,
                        &d_nullBits[arrayIdx],
                        offset);
}

// ACCESSORS
bool bdem_TableImp::isAnyInColumnNull(int columnIndex) const
{
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(     columnIndex < numColumns());

    const int length = numRows();
    for (int i = 0; i < length; ++i) {
        if (bdeu_BitstringUtil::get(&d_nullBits.front(), i)
         || d_rows[i]->elemRef(columnIndex).isNull()) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bool bdem_TableImp::isAnyInColumnNonNull(int columnIndex) const
{
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(     columnIndex < numColumns());

    const int length = numRows();
    for (int i = 0; i < length; ++i) {
        if (!bdeu_BitstringUtil::get(&d_nullBits.front(), i)
         && d_rows[i]->elemRef(columnIndex).isNonNull()) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bool bdem_TableImp::isAnyRowNonNull(int rowIndex, int numRows) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(rowIndex + numRows <= this->numRows());

    const int index = bdeu_BitstringUtil::find0AtSmallestIndexGE(
                                                           &d_nullBits.front(),
                                                           d_rows.size(),
                                                           rowIndex);
    return -1 == index || index - rowIndex >= numRows ? false : true;
}

bool bdem_TableImp::isAnyRowNull(int rowIndex, int numRows) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(rowIndex + numRows <= this->numRows());

    const int index = bdeu_BitstringUtil::find1AtSmallestIndexGE(
                                                           &d_nullBits.front(),
                                                           d_rows.size(),
                                                           rowIndex);
    return -1 == index || index - rowIndex >= numRows ? false : true;
}

bool bdem_TableImp::isAnyNull() const
{
    const int length = numRows();
    const int width  = numColumns();

    if (0 == width) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < length; ++i) {
        if (bdeu_BitstringUtil::get(&d_nullBits.front(), i)
         || d_rows[i]->isAnyInRangeNull(0, width)) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bool bdem_TableImp::isAnyNonNull() const
{
    const int length = numRows();
    const int width  = numColumns();
    for (int i = 0; i < length; ++i) {
        if (!bdeu_BitstringUtil::get(&d_nullBits.front(), i)
         && d_rows[i]->isAnyInRangeNonNull(0, width)) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bdem_ConstElemRef bdem_TableImp::rowElemRef(int rowIndex) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());

    const int arrayIdx = rowIndex / BDEM_BITS_PER_INT;
    const int offset   = rowIndex % BDEM_BITS_PER_INT;

    return bdem_ConstElemRef(d_rows[rowIndex],
                             &bdem_RowData::d_rowAttr,
                             &d_nullBits[arrayIdx],
                             offset);
}

bsl::ostream& bdem_TableImp::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '{';

    if (level < 0) {
        level = -level;
    }

    int nestedLevel, nestedSpacesPerLevel;
    if (0 <= spacesPerLevel) {

        // Newlines only if spacesPerLevel is non-negative

        stream << '\n';
        nestedLevel = level + 1;
        nestedSpacesPerLevel = spacesPerLevel;
    }
    else {

        // If 'spacesPerLevel' is negative, just put one space between rows
        // and suppress newlines when formatting each row.

        nestedLevel = 1;
        nestedSpacesPerLevel = -1;
    }

    // Print out column types:

    bdeu_Print::indent(stream, nestedLevel, nestedSpacesPerLevel);
    stream << "Column Types: [";
    for (int column = 0; column < numColumns(); ++column) {
        bdeu_Print::newlineAndIndent(stream,
                                     nestedLevel + 1,
                                     nestedSpacesPerLevel);
        stream << columnType(column);
    }
    bdeu_Print::newlineAndIndent(stream, nestedLevel, nestedSpacesPerLevel);
    stream << ']';
    if (0 <= spacesPerLevel) {
        stream << '\n';
    }
    stream << bsl::flush;

    const int numRows = d_rows.size();
    for (int i = 0; i < numRows; ++i) {
        bdeu_Print::indent(stream, nestedLevel, nestedSpacesPerLevel);
        stream << "Row " << i << ": ";
        if (bdeu_BitstringUtil::get(&d_nullBits.front(), i)) {
            stream << "NULL";
        }
        else {

            // Print row without column labels.  Don't indent first line.

            d_rows[i]->printRow(stream,
                                -nestedLevel,
                                nestedSpacesPerLevel,
                                false);
        }
        stream << bsl::flush;
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << '}';
    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream << bsl::flush;
}

// FREE OPERATORS
bool operator==(const bdem_TableImp& lhs, const bdem_TableImp& rhs)
{
    if (&lhs == &rhs) {  // self comparison
        return true;                                                  // RETURN
    }

    // Check for same number of rows and same column-type sequence.

    const int numRows = lhs.d_rows.size();
    if (numRows != (int)rhs.d_rows.size()
     || !lhs.d_rowLayout_p->areElemTypesEqual(*rhs.d_rowLayout_p)) {
        return false;                                                 // RETURN
    }

    if (0 == numRows) {
        return true;                                                  // RETURN
    }

    // Check for same null-row bits.

    const int *lhsNullbits = &lhs.d_nullBits.front();
    const int *rhsNullbits = &rhs.d_nullBits.front();
    if (!bdeu_BitstringUtil::areEqual(lhsNullbits,
                                      0,
                                      rhsNullbits,
                                      0,
                                      numRows)) {
        return false;
    }

    // Check for same element values, in non-null rows only.

    for (int i = 0; i < numRows; ++i) {
        if (!bdeu_BitstringUtil::get(lhsNullbits, i)
         && *lhs.d_rows[i] != *rhs.d_rows[i]) {
            return false;                                             // RETURN
        }
    }

    return true;
}

// PRIVATE GEOMETRIC MEMORY GROWTH
void bdem_TableImp_enableGeometricMemoryGrowth()
{
    bdem_TableImp_geometricMemoryGrowthFlag = true;
}

void bdem_TableImp_disableGeometricMemoryGrowth()
{
    bdem_TableImp_geometricMemoryGrowthFlag = false;
}

bool bdem_TableImp_isGeometricMemoryGrowth()
{
    return bdem_TableImp_geometricMemoryGrowthFlag;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
