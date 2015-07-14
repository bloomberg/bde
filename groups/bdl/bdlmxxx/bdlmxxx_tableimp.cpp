// bdlmxxx_tableimp.cpp                                                  -*-C++-*-
#include <bdlmxxx_tableimp.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_tableimp_cpp,"$Id$ $CSID$")

#include <bdlmxxx_functiontemplates.h>

#include <bdlb_print.h>

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

static bool geometricMemoryGrowthFlag = true;

// LOCAL CONSTANTS
enum {
    BDEM_BITS_PER_INT = 32  // the number of bits in an 'int'
};

// STATIC HELPER FUNCTIONS
static
int computeRowSize(const bdlmxxx::RowLayout *layout)
{
    int rowSize = sizeof(bdlmxxx::RowData) + layout->totalOffset();

    rowSize += (layout->length() + BDEM_BITS_PER_INT - 1)
                                             / BDEM_BITS_PER_INT * sizeof(int);
    return (rowSize + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
                              & ~(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
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
    // Namespace for static methods to store in a 'bdlmxxx::Descriptor' structure.

    // CLASS METHODS
    static
    void defaultConstruct(
                  void                                     *obj,
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc);
        // Construct a table object into raw memory.  The prototype for this
        // function must match the defaultConstruct function pointer in
        // bdem_Descriptor.

    static
    void copyConstruct(
                  void                                     *obj,
                  const void                               *original,
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc);
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
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc)
{
    BSLS_ASSERT(obj);

    new (obj) bdlmxxx::TableImp(allocationStrategy, alloc);
}

void bdem_TableImp_AttrFuncs::copyConstruct(
                  void                                     *obj,
                  const void                               *original,
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc)
{
    BSLS_ASSERT(obj);
    BSLS_ASSERT(original);

    const bdlmxxx::TableImp& origTable = *static_cast<const bdlmxxx::TableImp *>(
                                                                     original);
    new (obj) bdlmxxx::TableImp(origTable, allocationStrategy, alloc);
}

bool bdem_TableImp_AttrFuncs::isEmpty(const void *obj)
{
    BSLS_ASSERT(obj);

    const bdlmxxx::TableImp& theTable = *static_cast<const bdlmxxx::TableImp *>(obj);
    return 0 == theTable.numRows() && 0 == theTable.numColumns();
}

bsl::ostream& bdem_TableImp_AttrFuncs::print(const void    *obj,
                                             bsl::ostream&  stream,
                                             int            level,
                                             int            spacesPerLevel)
{
    BSLS_ASSERT(obj);

    static_cast<const bdlmxxx::TableImp *>(obj)->print(stream,
                                                   level,
                                                   spacesPerLevel);
    return stream;
}

                        // -------------------
                        // class bdlmxxx::TableImp
                        // -------------------

// CLASS DATA
const bdlmxxx::Descriptor bdlmxxx::TableImp::s_tableAttr =
{
    bdlmxxx::ElemType::BDEM_TABLE,
    sizeof(bdlmxxx::TableImp),
    bsls::AlignmentFromType<bdlmxxx::TableImp>::VALUE,
    &bdem_TableImp_AttrFuncs::defaultConstruct,
    &bdem_TableImp_AttrFuncs::copyConstruct,
    &bdlmxxx::FunctionTemplates::destroy<bdlmxxx::TableImp>,
    &bdlmxxx::FunctionTemplates::assign<bdlmxxx::TableImp>,
    &bdlmxxx::FunctionTemplates::bitwiseMove<bdlmxxx::TableImp>,
    &bdlmxxx::FunctionTemplates::removeAll<bdlmxxx::TableImp>,
    &bdem_TableImp_AttrFuncs::isEmpty,
    &bdlmxxx::FunctionTemplates::areEqual<bdlmxxx::TableImp>,
    &bdem_TableImp_AttrFuncs::print
};

namespace bdlmxxx {
// CREATORS
TableImp::TableImp(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        RowLayout(d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    bslma::AutoRawDeleter<RowLayout, bslma::Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

TableImp::TableImp(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        RowLayout(d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= initialMemorySize);

    bslma::AutoRawDeleter<RowLayout, bslma::Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

TableImp::TableImp(
               const ElemType::Type                 columnTypes[],
               int                                       numColumns,
               const Descriptor                    *const *attrLookupTbl,
               AggregateOption::AllocationStrategy  allocationStrategy,
               bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        RowLayout(columnTypes,
                                       numColumns,
                                       attrLookupTbl,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= numColumns);

    bslma::AutoRawDeleter<RowLayout, bslma::Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

TableImp::TableImp(
               const ElemType::Type                 columnTypes[],
               int                                       numColumns,
               const Descriptor                    *const *attrLookupTbl,
               AggregateOption::AllocationStrategy  allocationStrategy,
               int                                       initialMemorySize,
               bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        RowLayout(columnTypes,
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

    bslma::AutoRawDeleter<RowLayout, bslma::Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

TableImp::TableImp(
                  const RowLayout&                     rowLayout,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        RowLayout(rowLayout,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    bslma::AutoRawDeleter<RowLayout, bslma::Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

TableImp::TableImp(
                  const RowLayout&                     rowLayout,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        RowLayout(rowLayout,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= initialMemorySize);

    bslma::AutoRawDeleter<RowLayout, bslma::Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

TableImp::TableImp(
                  const TableImp&                      original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        RowLayout(*original.d_rowLayout_p,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.originalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    bslma::AutoRawDeleter<RowLayout, bslma::Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits = original.d_nullBits;

    const int numRows = original.d_rows.size();

    d_rows.resize(numRows);
    d_rowPool.reserveCapacity(numRows);

    if (numRows > 0) {
        bslma::AutoRawDeleter<RowData, bdlma::Pool>
                                        rowsAutoDel(&d_rows[0], &d_rowPool, 0);

        for (int i = 0; i < numRows; ++i) {
            d_rows[i] = new (d_rowPool) RowData(
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

TableImp::TableImp(
                  const TableImp&                      original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        RowLayout(*original.d_rowLayout_p,
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.internalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= initialMemorySize);

    bslma::AutoRawDeleter<RowLayout, bslma::Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits = original.d_nullBits;

    const int numRows = original.d_rows.size();

    d_rows.resize(numRows);
    d_rowPool.reserveCapacity(numRows);

    if (numRows > 0) {
        bslma::AutoRawDeleter<RowData, bdlma::Pool>
                                        rowsAutoDel(&d_rows[0], &d_rowPool, 0);

        for (int i = 0; i < numRows; ++i) {
            d_rows[i] = new (d_rowPool) RowData(
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

TableImp::TableImp(
                  const RowData&                       rowData,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        RowLayout(*rowData.rowLayout(),
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.internalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    bslma::AutoRawDeleter<RowLayout, bslma::Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

TableImp::TableImp(
                  const RowData&                       rowData,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  int                                       initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
, d_rowLayout_p(new (*d_allocatorManager.internalAllocator())
                        RowLayout(*rowData.rowLayout(),
                                       d_allocatorManager.internalAllocator()))
, d_rowPool(computeRowSize(d_rowLayout_p),
            d_allocatorManager.internalAllocator())
, d_rows(d_allocatorManager.internalAllocator())
, d_nullBits(d_allocatorManager.internalAllocator())
{
    BSLS_ASSERT(0 <= initialMemorySize);

    bslma::AutoRawDeleter<RowLayout, bslma::Allocator>
                                proctor(&d_rowLayout_p,
                                        d_allocatorManager.internalAllocator(),
                                        1);
    d_nullBits.resize(nullBitsArraySize(0));
    proctor.release();
}

TableImp::~TableImp()
{
    if (!(AggregateOption::BDEM_NODESTRUCT_FLAG &
                                    d_allocatorManager.allocationStrategy())) {
        // No pooling or write-once options.  Delete every row.

        const int numRows = d_rows.size();
        for (int i = 0; i < numRows; ++i) {
            d_rows[i]->~RowData();
        }
        d_allocatorManager.internalAllocator()->deleteObject(d_rowLayout_p);
    }
}

// MANIPULATORS
TableImp& TableImp::operator=(const TableImp& rhs)
{
    // Create 'newTable' as copy of the rhs, but using the same
    // allocation options as were used to construct '*this'.  Then swap
    // '*this' with the temporary table.  The old value of '*this' will
    // be destroyed when 'newTable' goes out of scope.

    if (this != &rhs) {
        TableImp newTable(rhs,
                               d_allocatorManager.allocationStrategy(),
                               d_allocatorManager.originalAllocator());

        bdlimpxxx::BitwiseCopy<TableImp>::swap(this, &newTable);
    }

    return *this;
}

void TableImp::makeRowsNull(int startRowIndex, int numRows)
{
    BSLS_ASSERT(0 <= startRowIndex);
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(startRowIndex + numRows <= this->numRows());

    const int endIndex = startRowIndex + numRows;
    for (int i = startRowIndex; i < endIndex; ++i) {
        d_rows[i]->makeAllNull();
    }
    bdlb::BitstringUtil::set(&d_nullBits.front(),
                            startRowIndex,
                            true,
                            numRows);
}

void TableImp::removeRow(int rowIndex)
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());

    RowData *rowData = d_rows[rowIndex];
    rowData->~RowData();
    d_rowPool.deallocate(rowData);
    d_rows.erase(d_rows.begin() + rowIndex);
    bdlb::BitstringUtil::removeAndFill0(&d_nullBits.front(),
                                       d_rows.size() + 1,
                                       rowIndex,
                                       1);
}

bsl::size_t TableImp::capacityRaw() const
{
    return d_rows.capacity();
}

void TableImp::reserveRaw(bsl::size_t numRows)
{
    if(capacityRaw() >= numRows) {
        return;                                                       // RETURN
    }

    const int newSize = nullBitsArraySize(numRows);

    // Reserve 2 times BSLS_MAX_ALIGNMENT initialization of the 'RowData'
    // when inserting a row (once for the (empty) data and once for the new
    // 'nullBits' vector).  Add the memory necessary for the new size of
    // 'd_nullBits'.  Note that this calculation leaves out the memory used by
    // the array of null bits in the initialization of 'RowData', because
    // it's not possible to access the function to calculate its size from
    // here: the extra memory obtained by invoking 'reserve' on a sequential
    // allocator or a multi pool ('BDEM_WRITE_ONCE' and 'BDEM_PASS_THROUGH'
    // respectively) seems to be enough to make up for this approximation.
    // Also note that in case of 'BDEM_PASS_THROUGH' the
    // 'd_allocatorManager.reserveMemory' call will have no effect.

    d_allocatorManager.reserveMemory(
                                    2 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
                                      * (numRows - capacityRaw())
                                      +  sizeof(int) * newSize);
    d_rowPool.reserveCapacity(numRows - capacityRaw());
    d_rows.reserve(numRows);
    d_nullBits.reserve(newSize);
}

void TableImp::reset(const ElemType::Type  columnTypes[],
                          int                        numColumns,
                          const Descriptor     *const *attrLookupTbl)
{
    BSLS_ASSERT(0 <= numColumns);

    TableImp newTable(columnTypes,
                           numColumns,
                           attrLookupTbl,
                           d_allocatorManager.allocationStrategy(),
                           d_allocatorManager.originalAllocator());

    bdlimpxxx::BitwiseCopy<TableImp>::swap(this, &newTable);
}

RowData& TableImp::insertRow(int                 dstRowIndex,
                                       const RowData& srcRow)
{
    BSLS_ASSERT(0 <= dstRowIndex);
    BSLS_ASSERT(     dstRowIndex <= numRows());

    const int newSize = nullBitsArraySize(d_rows.size() + 1);
    if ((int)d_nullBits.size() < newSize) {
        d_nullBits.resize(newSize, 0);
    }

    // Reserve capacity of 'd_rowPool' and 'd_rows' to ensure that 'new' and
    // 'insert' won't throw below.  Note that the 'RowData' c'tor,
    // however, may allocate and might throw.

    d_rowPool.reserveCapacity(1);
    if (!geometricMemoryGrowthFlag) {
        d_rows.reserve(numRows() + 1);
    }

    RowData *newRow = new (d_rowPool) RowData(
                                       d_rowLayout_p,
                                       srcRow,
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.internalAllocator());
    bslma::RawDeleterProctor<RowData, bdlma::Pool>
                                                   proctor(newRow, &d_rowPool);

    d_rows.insert(d_rows.begin() + dstRowIndex, newRow);

    // We won't throw after this.

    proctor.release();

    // Make the newly-added row non-null, since 'srcRow' can't indicate
    // otherwise.

    bdlb::BitstringUtil::insert0(&d_nullBits.front(),
                                d_rows.size() - 1,
                                dstRowIndex,
                                1);

    return *d_rows[dstRowIndex];
}

void TableImp::insertRows(int                  dstRowIndex,
                               const TableImp& srcTable,
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

    // Reserve capacity of 'd_rowPool' and 'd_rows' to ensure that 'new' and
    // 'insert' won't throw below.  Note that the 'RowData' c'tor,
    // however, may allocate and might throw.

    d_rowPool.reserveCapacity(numRows);
    if (!geometricMemoryGrowthFlag) {
        d_rows.reserve(originalSize + numRows);
    }

    bsl::vector<RowData *> tempRows;
    tempRows.resize(numRows);

    bslma::AutoRawDeleter<RowData, bdlma::Pool>
                                      rowsAutoDel(&tempRows[0], &d_rowPool, 0);

    for (int i = 0; i < numRows; ++i) {
        // Bear in mind 'RowData' c'tor might throw.

        tempRows[i] = new (d_rowPool) RowData(
                                       d_rowLayout_p,
                                       *srcTable.d_rows[srcRowIndex + i],
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.internalAllocator());
        ++rowsAutoDel;
    }

    // this might throw

    d_rows.insert(d_rows.begin() + dstRowIndex,
                  tempRows.begin(),
                  tempRows.end());

    // Can't throw from here on.

    rowsAutoDel.release();

    if (isAliasPossible) {
        // Copy nullness bits of source rows to temporary.

        bdlb::BitstringUtil::copyRaw(&tempNullBits.front(),
                                    0,
                                    &srcTable.d_nullBits.front(),
                                    srcRowIndex,
                                    numRows);
    }

    // Open up space for nullness bits of new rows.

    bdlb::BitstringUtil::insertRaw(&d_nullBits.front(),
                                  originalSize,
                                  dstRowIndex,
                                  numRows);

    if (isAliasPossible) {
        bdlb::BitstringUtil::copyRaw(&d_nullBits.front(),
                                    dstRowIndex,
                                    &tempNullBits.front(),
                                    0,
                                    numRows);
    }
    else {
        bdlb::BitstringUtil::copyRaw(&d_nullBits.front(),
                                    dstRowIndex,
                                    &srcTable.d_nullBits.front(),
                                    srcRowIndex,
                                    numRows);
    }
}

void TableImp::insertNullRows(int dstRowIndex, int numRows)
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

    // Reserve capacity of 'd_rowPool' and 'd_rows' to ensure that 'new' and
    // 'insert' won't throw below.  Note that the 'RowData' c'tor,
    // however, may allocate and might throw.

    d_rowPool.reserveCapacity(numRows);
    if (!geometricMemoryGrowthFlag) {
        d_rows.reserve(this->numRows() + numRows);
    }

    for (int i = 0; i < numRows; ++i) {
        // 'RowData' c'tor might allocate and might throw, but 'd_rows'
        // and 'd_nullBits' will be in a valid state if it does.

        RowData *newRow = new (d_rowPool) RowData(
                                       d_rowLayout_p,
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.internalAllocator());

        bslma::RawDeleterProctor<RowData, bdlma::Pool>
                                                   proctor(newRow, &d_rowPool);

        // might throw

        d_rows.insert(d_rows.begin() + dstRowIndex + i, newRow);

        // won't throw for rest of loop

        proctor.release();

        bdlb::BitstringUtil::insert1(&d_nullBits.front(),
                                    d_rows.size() - 1,
                                    dstRowIndex + i,
                                    1);
    }
}

void TableImp::removeRows(int rowIndex, int numRows)
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(rowIndex + numRows <= this->numRows());

    for (int i = 0; i < numRows; ++i) {
        RowData *rowData = d_rows[rowIndex + i];
        rowData->~RowData();
        d_rowPool.deallocate(rowData);
    }

    d_rows.erase(d_rows.begin() + rowIndex,
                 d_rows.begin() + rowIndex + numRows);

    const int newLen = d_rows.size();

    if (0 < newLen) {
        bdlb::BitstringUtil::removeAndFill0(&d_nullBits.front(),
                                           d_rows.size() + numRows,
                                           rowIndex,
                                           numRows);
    }

    d_nullBits.resize(nullBitsArraySize(newLen));
}

void TableImp::removeAll()
{
    TableImp newTable(d_allocatorManager.allocationStrategy(),
                           d_allocatorManager.originalAllocator());

    bdlimpxxx::BitwiseCopy<TableImp>::swap(this, &newTable);
}

ElemRef TableImp::rowElemRef(int rowIndex)
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());

    const int arrayIdx = rowIndex / BDEM_BITS_PER_INT;
    const int offset   = rowIndex % BDEM_BITS_PER_INT;

    return ElemRef(d_rows[rowIndex],
                        &RowData::s_rowAttr,
                        &d_nullBits[arrayIdx],
                        offset);
}

// ACCESSORS
bool TableImp::isAnyInColumnNull(int columnIndex) const
{
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(     columnIndex < numColumns());

    const int length = numRows();
    for (int i = 0; i < length; ++i) {
        if (bdlb::BitstringUtil::get(&d_nullBits.front(), i)
         || d_rows[i]->elemRef(columnIndex).isNull()) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bool TableImp::isAnyInColumnNonNull(int columnIndex) const
{
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(     columnIndex < numColumns());

    const int length = numRows();
    for (int i = 0; i < length; ++i) {
        if (!bdlb::BitstringUtil::get(&d_nullBits.front(), i)
         && d_rows[i]->elemRef(columnIndex).isNonNull()) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bool TableImp::isAnyRowNonNull(int rowIndex, int numRows) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(rowIndex + numRows <= this->numRows());

    const int index = bdlb::BitstringUtil::find0AtSmallestIndexGE(
                                                           &d_nullBits.front(),
                                                           d_rows.size(),
                                                           rowIndex);
    return -1 == index || index - rowIndex >= numRows ? false : true;
}

bool TableImp::isAnyRowNull(int rowIndex, int numRows) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(0 <= numRows);
    BSLS_ASSERT(rowIndex + numRows <= this->numRows());

    const int index = bdlb::BitstringUtil::find1AtSmallestIndexGE(
                                                           &d_nullBits.front(),
                                                           d_rows.size(),
                                                           rowIndex);
    return -1 == index || index - rowIndex >= numRows ? false : true;
}

bool TableImp::isAnyNull() const
{
    const int length = numRows();
    const int width  = numColumns();

    if (0 == width) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < length; ++i) {
        if (bdlb::BitstringUtil::get(&d_nullBits.front(), i)
         || d_rows[i]->isAnyInRangeNull(0, width)) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bool TableImp::isAnyNonNull() const
{
    const int length = numRows();
    const int width  = numColumns();
    for (int i = 0; i < length; ++i) {
        if (!bdlb::BitstringUtil::get(&d_nullBits.front(), i)
         && d_rows[i]->isAnyInRangeNonNull(0, width)) {
            return true;                                              // RETURN
        }
    }
    return false;
}

ConstElemRef TableImp::rowElemRef(int rowIndex) const
{
    BSLS_ASSERT(0 <= rowIndex);
    BSLS_ASSERT(     rowIndex < numRows());

    const int arrayIdx = rowIndex / BDEM_BITS_PER_INT;
    const int offset   = rowIndex % BDEM_BITS_PER_INT;

    return ConstElemRef(d_rows[rowIndex],
                             &RowData::s_rowAttr,
                             &d_nullBits[arrayIdx],
                             offset);
}

bsl::ostream& TableImp::print(bsl::ostream& stream,
                                   int           level,
                                   int           spacesPerLevel) const
{
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << '{';

    if (level < 0) {
        level = -level;
    }

    int nestedLevel, nestedSpacesPerLevel;
    if (0 <= spacesPerLevel) {

        // Output a new line only if spacesPerLevel is non-negative

        stream << '\n';
        nestedLevel = level + 1;
        nestedSpacesPerLevel = spacesPerLevel;
    }
    else {

        // If 'spacesPerLevel' is negative, just put one space between rows
        // and suppress new lines when formatting each row.

        nestedLevel = 1;
        nestedSpacesPerLevel = -1;
    }

    // Print out column types:

    bdlb::Print::indent(stream, nestedLevel, nestedSpacesPerLevel);
    stream << "Column Types: [";
    for (int column = 0; column < numColumns(); ++column) {
        bdlb::Print::newlineAndIndent(stream,
                                     nestedLevel + 1,
                                     nestedSpacesPerLevel);
        stream << columnType(column);
    }
    bdlb::Print::newlineAndIndent(stream, nestedLevel, nestedSpacesPerLevel);
    stream << ']';
    if (0 <= spacesPerLevel) {
        stream << '\n';
    }
    stream << bsl::flush;

    const int numRows = d_rows.size();
    for (int i = 0; i < numRows; ++i) {
        bdlb::Print::indent(stream, nestedLevel, nestedSpacesPerLevel);
        stream << "Row " << i << ": ";
        if (bdlb::BitstringUtil::get(&d_nullBits.front(), i)) {
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

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << '}';
    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream << bsl::flush;
}
}  // close package namespace

// FREE OPERATORS
bool bdlmxxx::operator==(const TableImp& lhs, const TableImp& rhs)
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
    if (!bdlb::BitstringUtil::areEqual(lhsNullbits,
                                      0,
                                      rhsNullbits,
                                      0,
                                      numRows)) {
        return false;                                                 // RETURN
    }

    // Check for same element values, in non-null rows only.

    for (int i = 0; i < numRows; ++i) {
        if (!bdlb::BitstringUtil::get(lhsNullbits, i)
         && *lhs.d_rows[i] != *rhs.d_rows[i]) {
            return false;                                             // RETURN
        }
    }

    return true;
}

namespace bdlmxxx {
// PRIVATE GEOMETRIC MEMORY GROWTH
void TableImp_enableGeometricMemoryGrowth()
{
    geometricMemoryGrowthFlag = true;
}

void TableImp_disableGeometricMemoryGrowth()
{
    geometricMemoryGrowthFlag = false;
}
}  // close package namespace

bool bdem_TableImp_isGeometricMemoryGrowth()
{
    return geometricMemoryGrowthFlag;
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
