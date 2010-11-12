// bdem_list.cpp                                                      -*-C++-*-
#include <bdem_list.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_list_cpp,"$Id$ $CSID$")

#include <bdem_row.h>        // for testing only

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

                        // ---------------
                        // class bdem_List
                        // ---------------

// CREATORS
bdem_List::bdem_List(const bsl::vector<bdem_ElemType::Type>&  elementTypes,
                     bslma_Allocator                         *basicAllocator)
: d_listImp(vectorData(elementTypes),
            elementTypes.size(),
            bdem_ElemAttrLookup::lookupTable(),
            bdem_AggregateOption::BDEM_PASS_THROUGH,
            basicAllocator)
{
}

bdem_List::bdem_List(
                  const bsl::vector<bdem_ElemType::Type>&   elementTypes,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_listImp(vectorData(elementTypes),
            elementTypes.size(),
            bdem_ElemAttrLookup::lookupTable(),
            allocationStrategy,
            basicAllocator)
{
}

bdem_List::bdem_List(
                  const bsl::vector<bdem_ElemType::Type>&   elementTypes,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_listImp(vectorData(elementTypes),
            elementTypes.size(),
            bdem_ElemAttrLookup::lookupTable(),
            allocationStrategy,
            bdem_ListImp::InitialMemory(initialMemorySize),
            basicAllocator)
{
    BSLS_ASSERT(0 <= initialMemorySize);
}

// MANIPULATORS
void bdem_List::insertNullElements(
                         int                                     dstIndex,
                         const bsl::vector<bdem_ElemType::Type>& elementTypes)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    d_listImp.insertNullElements(dstIndex,
                                 vectorData(elementTypes),
                                 elementTypes.size(),
                                 bdem_ElemAttrLookup::lookupTable());
}

void bdem_List::reset(const bsl::vector<bdem_ElemType::Type>& elementTypes)
{
    d_listImp.reset(vectorData(elementTypes),
                    elementTypes.size(),
                    bdem_ElemAttrLookup::lookupTable());
}

// ACCESSORS
void bdem_List::elemTypes(bsl::vector<bdem_ElemType::Type> *result) const
{
    BSLS_ASSERT(result);

    const int len = length();

    result->clear();
    result->reserve(len);

    for (int i = 0; i < len; ++i) {
        result->push_back(elemType(i));
    }
}

// Make sure that 'bdem_List' and 'bdem_ListImp' really are aliases of one
// another.

BSLMF_ASSERT(sizeof(bdem_List) == sizeof(bdem_ListImp));
BSLMF_ASSERT((int) bsls_AlignmentFromType<bdem_List>::VALUE ==
             (int) bsls_AlignmentFromType<bdem_ListImp>::VALUE);

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
