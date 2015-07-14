// bdlmxxx_list.cpp                                                      -*-C++-*-
#include <bdlmxxx_list.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_list_cpp,"$Id$ $CSID$")

#include <bdlmxxx_row.h>        // for testing only

#include <bslmf_assert.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>

#include <bsl_vector.h>

// STATIC HELPER FUNCTIONS
template <typename TYPE>
static
const TYPE *vectorData(const bsl::vector<TYPE>& v)
    // Return the address of the non-modifiable first element of 'v', or 0 if
    // 'v' is empty.
{
    return v.empty() ? 0 : &v.front();
}

namespace BloombergLP {

namespace bdlmxxx {
                        // ---------------
                        // class List
                        // ---------------

// CREATORS
List::List(const bsl::vector<ElemType::Type>&  elementTypes,
                     bslma::Allocator                        *basicAllocator)
: d_listImp(vectorData(elementTypes),
            elementTypes.size(),
            ElemAttrLookup::lookupTable(),
            AggregateOption::BDEM_PASS_THROUGH,
            basicAllocator)
{
}

List::List(
                  const bsl::vector<ElemType::Type>&   elementTypes,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_listImp(vectorData(elementTypes),
            elementTypes.size(),
            ElemAttrLookup::lookupTable(),
            allocationStrategy,
            basicAllocator)
{
}

List::List(
                  const bsl::vector<ElemType::Type>&   elementTypes,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_listImp(vectorData(elementTypes),
            elementTypes.size(),
            ElemAttrLookup::lookupTable(),
            allocationStrategy,
            ListImp::InitialMemory(initialMemorySize),
            basicAllocator)
{
    BSLS_ASSERT(0 <= initialMemorySize);
}

// MANIPULATORS
void List::insertNullElements(
                         int                                     dstIndex,
                         const bsl::vector<ElemType::Type>& elementTypes)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    d_listImp.insertNullElements(dstIndex,
                                 vectorData(elementTypes),
                                 elementTypes.size(),
                                 ElemAttrLookup::lookupTable());
}

void List::reset(const bsl::vector<ElemType::Type>& elementTypes)
{
    d_listImp.reset(vectorData(elementTypes),
                    elementTypes.size(),
                    ElemAttrLookup::lookupTable());
}

// ACCESSORS
void List::elemTypes(bsl::vector<ElemType::Type> *result) const
{
    BSLS_ASSERT(result);

    const int len = length();

    result->clear();
    result->reserve(len);

    for (int i = 0; i < len; ++i) {
        result->push_back(elemType(i));
    }
}
}  // close package namespace

// Make sure that 'bdlmxxx::List' and 'bdlmxxx::ListImp' really are aliases of one
// another.

BSLMF_ASSERT(sizeof(bdlmxxx::List) == sizeof(bdlmxxx::ListImp));
BSLMF_ASSERT((int) bsls::AlignmentFromType<bdlmxxx::List>::VALUE ==
             (int) bsls::AlignmentFromType<bdlmxxx::ListImp>::VALUE);

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
