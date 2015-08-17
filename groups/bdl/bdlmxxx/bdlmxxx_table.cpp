// bdlmxxx_table.cpp                                                  -*-C++-*-
#include <bdlmxxx_table.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_table_cpp,"$Id$ $CSID$")

#include <bdlmxxx_functiontemplates.h>

#include <bdlmxxx_choice.h>         // for testing only
#include <bdlmxxx_choicearray.h>    // for testing only
#include <bdlmxxx_list.h>           // for testing only
#include <bdlmxxx_row.h>            // for testing only

#include <bslmf_assert.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>

#include <bsl_vector.h>

// STATIC HELPER FUNCTIONS
template <class TYPE>
static
const TYPE *vectorData(const bsl::vector<TYPE>& vector)
    // Return the address of the non-modifiable first element of the specified
    // 'vector', or 0 if 'vector' is empty.
{
    return vector.empty() ? 0 : &vector.front();
}

namespace BloombergLP {

namespace bdlmxxx {
                        // ----------------
                        // class Table
                        // ----------------

// CREATORS
Table::Table(const bsl::vector<ElemType::Type>&  columnTypes,
                       bslma::Allocator                        *basicAllocator)
: d_tableImp(vectorData(columnTypes),
             columnTypes.size(),
             ElemAttrLookup::lookupTable(),
             AggregateOption::BDEM_PASS_THROUGH,
             basicAllocator)
{
}

Table::Table(
                  const bsl::vector<ElemType::Type>&   columnTypes,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_tableImp(vectorData(columnTypes),
             columnTypes.size(),
             ElemAttrLookup::lookupTable(),
             allocationStrategy,
             basicAllocator)
{
}

Table::Table(
                  const bsl::vector<ElemType::Type>&   columnTypes,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemory,
                  bslma::Allocator                         *basicAllocator)
: d_tableImp(vectorData(columnTypes),
             columnTypes.size(),
             ElemAttrLookup::lookupTable(),
             allocationStrategy,
             initialMemory,
             basicAllocator)
{
    BSLS_ASSERT(0 <= initialMemory);
}

// MANIPULATORS
void Table::reset(const bsl::vector<ElemType::Type>& columnTypes)
{
    d_tableImp.reset(vectorData(columnTypes),
                     columnTypes.size(),
                     ElemAttrLookup::lookupTable());
}

void Table::setColumnValue(int                      columnIndex,
                                const ConstElemRef& value)
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
void Table::columnTypes(bsl::vector<ElemType::Type> *result) const
{
    BSLS_ASSERT(result);

    result->clear();
    result->reserve(numColumns());

    for (int i = 0; i < numColumns(); ++i) {
        result->push_back(columnType(i));
    }
}
}  // close package namespace

// Ensure that 'bdlmxxx::Table' and 'bdlmxxx::TableImp' really are aliases of one
// another.

BSLMF_ASSERT(sizeof(bdlmxxx::Table) == sizeof(bdlmxxx::TableImp));
BSLMF_ASSERT((int) bsls::AlignmentFromType<bdlmxxx::Table>::VALUE
          == (int) bsls::AlignmentFromType<bdlmxxx::TableImp>::VALUE);

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
