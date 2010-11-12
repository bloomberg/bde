// bdem_table.cpp                                                     -*-C++-*-
#include <bdem_table.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_table_cpp,"$Id$ $CSID$")

#include <bdem_functiontemplates.h>

#include <bdem_choice.h>         // for testing only
#include <bdem_choicearray.h>    // for testing only
#include <bdem_list.h>           // for testing only
#include <bdem_row.h>            // for testing only

#include <bslmf_assert.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>

#include <bsl_vector.h>

// STATIC HELPER FUNCTIONS
template <typename TYPE>
static
const TYPE *vectorData(const bsl::vector<TYPE>& vector)
    // Return the address of the non-modifiable first element of the specified
    // 'vector', or 0 if 'vector' is empty.
{
    return vector.empty() ? 0 : &vector.front();
}

namespace BloombergLP {

                        // ----------------
                        // class bdem_Table
                        // ----------------

// CREATORS
bdem_Table::bdem_Table(const bsl::vector<bdem_ElemType::Type>&  columnTypes,
                       bslma_Allocator                         *basicAllocator)
: d_tableImp(vectorData(columnTypes),
             columnTypes.size(),
             bdem_ElemAttrLookup::lookupTable(),
             bdem_AggregateOption::BDEM_PASS_THROUGH,
             basicAllocator)
{
}

bdem_Table::bdem_Table(
                  const bsl::vector<bdem_ElemType::Type>&   columnTypes,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_tableImp(vectorData(columnTypes),
             columnTypes.size(),
             bdem_ElemAttrLookup::lookupTable(),
             allocationStrategy,
             basicAllocator)
{
}

bdem_Table::bdem_Table(
                  const bsl::vector<bdem_ElemType::Type>&   columnTypes,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemory,
                  bslma_Allocator                          *basicAllocator)
: d_tableImp(vectorData(columnTypes),
             columnTypes.size(),
             bdem_ElemAttrLookup::lookupTable(),
             allocationStrategy,
             initialMemory,
             basicAllocator)
{
    BSLS_ASSERT(0 <= initialMemory);
}

// MANIPULATORS
void bdem_Table::reset(const bsl::vector<bdem_ElemType::Type>& columnTypes)
{
    d_tableImp.reset(vectorData(columnTypes),
                     columnTypes.size(),
                     bdem_ElemAttrLookup::lookupTable());
}

void bdem_Table::setColumnValue(int                      columnIndex,
                                const bdem_ConstElemRef& value)
{
    BSLS_ASSERT(0 <= columnIndex);
    BSLS_ASSERT(     columnIndex < numColumns());

    if (value.isNull()) {
        d_tableImp.makeColumnNull(columnIndex);
    }
    else {
        d_tableImp.setColumnValue(columnIndex, value.data());
    }
}

// ACCESSORS
void bdem_Table::columnTypes(bsl::vector<bdem_ElemType::Type> *result) const
{
    BSLS_ASSERT(result);

    result->clear();
    result->reserve(numColumns());

    for (int i = 0; i < numColumns(); ++i) {
        result->push_back(columnType(i));
    }
}

// Ensure that 'bdem_Table' and 'bdem_TableImp' really are aliases of one
// another.

BSLMF_ASSERT(sizeof(bdem_Table) == sizeof(bdem_TableImp));
BSLMF_ASSERT((int) bsls_AlignmentFromType<bdem_Table>::VALUE
          == (int) bsls_AlignmentFromType<bdem_TableImp>::VALUE);

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
