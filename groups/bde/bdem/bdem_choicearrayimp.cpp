// bdem_choicearrayimp.cpp                                            -*-C++-*-
#include <bdem_choicearrayimp.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_choicearrayimp_cpp,"$Id$ $CSID$")

#include <bdem_choiceheader.h>
#include <bdem_functiontemplates.h>

#include <bdem_properties.h>   // for testing only

#include <bdeu_print.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_rawdeleterproctor.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>

#include <bsl_algorithm.h>        // for 'equal'
#include <bsl_ostream.h>

#include <bsl_cstring.h>

namespace BloombergLP {

enum {
    BDEM_BITS_PER_INT = 32
};

// STATIC HELPER FUNCTIONS
static inline
int nullBitsArraySize(int numBits)
    // Return the size of the null bits array required to store at least
    // the specified 'numBits'.  The behavior is undefined unless
    // '0 <= numBits'.
{
    BSLS_ASSERT(0 <= numBits);

    return (numBits + BDEM_BITS_PER_INT - 1) / BDEM_BITS_PER_INT;
}

static
bdem_ChoiceHeader::DescriptorCatalog *
createCatalog(const bdem_ElemType::Type    *selectionTypes,
              int                           numSelectionTypes,
              const bdem_Descriptor *const *attrLookupTbl,
              bslma_Allocator              *internalAllocator)
    // Create a catalog having the specified initial 'numSelectionTypes' in the
    // specified 'selectionTypes' array, further described by the specified
    // 'attrLookupTbl', and using specified 'internalAllocator' to supply
    // memory.
{
    BSLS_ASSERT(0 <= numSelectionTypes);

    typedef bdem_ChoiceHeader::DescriptorCatalog DescriptorCatalog;

    DescriptorCatalog *result =
                 new (*internalAllocator) DescriptorCatalog(internalAllocator);

    bslma_RawDeleterProctor<DescriptorCatalog, bslma_Allocator>
                                     catalogProctor(result, internalAllocator);

    result->reserve(numSelectionTypes);

    for (int i = 0; i < numSelectionTypes; ++i) {
        BSLS_ASSERT(static_cast<unsigned>(selectionTypes[i])
                     < static_cast<unsigned>(bdem_ElemType::BDEM_NUM_TYPES));

        result->push_back(attrLookupTbl[static_cast<int>(selectionTypes[i])]);
    }

    catalogProctor.release();

    return result;
}

static
bdem_ChoiceHeader::DescriptorCatalog *
copyCatalog(const bdem_ChoiceHeader::DescriptorCatalog&  original,
            bslma_Allocator                             *internalAllocator)
    // Create a copy of the specified 'original' catalog using the specified
    // allocation mode and allocator.
{
    BSLS_ASSERT(internalAllocator);

    typedef bdem_ChoiceHeader::DescriptorCatalog DescriptorCatalog;

    return new (*internalAllocator) DescriptorCatalog(original,
                                                      internalAllocator);
}

static
void
destroyCatalog(bdem_ChoiceHeader::DescriptorCatalog     *catalog,
               bdem_AggregateOption::AllocationStrategy  allocationStrategy,
               bslma_Allocator                          *allocator)
{
    // Delete the descriptor catalog.
    if (!(allocationStrategy & bdem_AggregateOption::BDEM_NODESTRUCT_FLAG)) {
        BSLS_ASSERT(allocator);

        allocator->deleteObjectRaw(catalog);
    }
}

                 // -------------------------------------------
                 // class bdem_ChoiceArrayImp::CatalogContainer
                 // -------------------------------------------

// CREATORS
bdem_ChoiceArrayImp::CatalogContainer::~CatalogContainer()
{
    destroyCatalog(d_catalog_p,
                   d_allocationStrategy,
                   d_catalog_p->get_allocator().mechanism());
}

                     // ===================================
                     // class bdem_ChoiceArrayImp_AttrFuncs
                     // ===================================

struct bdem_ChoiceArrayImp_AttrFuncs {
    // Namespace for static functions to store in a bdem_Descriptor structure.

    // CLASS METHODS
    static
    void defaultConstruct(
                  void                                     *obj,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc);
        // Construct a choice array into raw memory.  The prototype for this
        // function must match the defaultConstruct function pointer in
        // 'bdem_Descriptor'.

    static
    void copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc);
        // Copy construct a choice array into raw memory.  The prototype for
        // this function must match the copyConstruct function pointer in
        // 'bdem_Descriptor'.

    static
    bool isEmpty(const void *obj);
        // Return 'true' if this choice array has no items and no selections
        // and 'false' otherwise.
};

                     // -----------------------------------
                     // class bdem_ChoiceArrayImp_AttrFuncs
                     // -----------------------------------

// CLASS METHODS
void bdem_ChoiceArrayImp_AttrFuncs::defaultConstruct(
                  void                                     *obj,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc)
{
    BSLS_ASSERT(obj);

    new (obj) bdem_ChoiceArrayImp(allocationStrategy, alloc);
}

void bdem_ChoiceArrayImp_AttrFuncs::copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc)
{
    BSLS_ASSERT(obj);
    BSLS_ASSERT(rhs);

    const bdem_ChoiceArrayImp& rhsChoiceArray =
                                 *static_cast<const bdem_ChoiceArrayImp*>(rhs);
    new (obj) bdem_ChoiceArrayImp(rhsChoiceArray, allocationStrategy, alloc);
}

bool bdem_ChoiceArrayImp_AttrFuncs::isEmpty(const void *obj)
{
    BSLS_ASSERT(obj);

    const bdem_ChoiceArrayImp& choiceArray =
                                 *static_cast<const bdem_ChoiceArrayImp*>(obj);
    return 0 == choiceArray.length() && 0 == choiceArray.numSelections();
}

                        // -------------------------
                        // class bdem_ChoiceArrayImp
                        // -------------------------

// CLASS DATA
const bdem_Descriptor bdem_ChoiceArrayImp::d_choiceArrayAttr =
{
    bdem_ElemType::BDEM_CHOICE_ARRAY,
    sizeof(bdem_ChoiceArrayImp),
    bsls_AlignmentFromType<bdem_ChoiceArrayImp>::VALUE,
    &bdem_ChoiceArrayImp_AttrFuncs::defaultConstruct,
    &bdem_ChoiceArrayImp_AttrFuncs::copyConstruct,
    &bdem_FunctionTemplates::destroy<bdem_ChoiceArrayImp>,
    &bdem_FunctionTemplates::assign<bdem_ChoiceArrayImp>,
    &bdem_FunctionTemplates::bitwiseMove<bdem_ChoiceArrayImp>,
    &bdem_FunctionTemplates::removeAll<bdem_ChoiceArrayImp>,
    &bdem_ChoiceArrayImp_AttrFuncs::isEmpty,
    &bdem_FunctionTemplates::areEqual<bdem_ChoiceArrayImp>,
    &bdem_FunctionTemplates::print<bdem_ChoiceArrayImp>
};

// CREATORS
bdem_ChoiceArrayImp::bdem_ChoiceArrayImp(bslma_Allocator *basicAllocator)
: d_allocatorManager(bdem_AggregateOption::BDEM_PASS_THROUGH, basicAllocator)
, d_catalogContainer(createCatalog(0,
                                   0,
                                   0,
                                   d_allocatorManager.internalAllocator()),
                     d_allocatorManager.allocationStrategy())
, d_headers(d_allocatorManager.internalAllocator())
, d_nullBits(1, 0, basicAllocator)
{
}

bdem_ChoiceArrayImp::bdem_ChoiceArrayImp(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_catalogContainer(createCatalog(0,
                                   0,
                                   0,
                                   d_allocatorManager.internalAllocator()),
                     d_allocatorManager.allocationStrategy())
, d_headers(d_allocatorManager.internalAllocator())
, d_nullBits(1, 0, basicAllocator)
{
}

bdem_ChoiceArrayImp::bdem_ChoiceArrayImp(
                  const bdem_ElemType::Type                 selectionTypes[],
                  int                                       numSelectionTypes,
                  const bdem_Descriptor *const              attrLookupTbl[],
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_catalogContainer(createCatalog(selectionTypes,
                                   numSelectionTypes,
                                   attrLookupTbl,
                                   d_allocatorManager.internalAllocator()),
                     d_allocatorManager.allocationStrategy())
, d_headers(d_allocatorManager.internalAllocator())
, d_nullBits(1, 0, basicAllocator)
{
    BSLS_ASSERT(0 <= numSelectionTypes);
}

bdem_ChoiceArrayImp::bdem_ChoiceArrayImp(
                                   const bdem_ChoiceArrayImp&  original,
                                   bslma_Allocator            *basicAllocator)
: d_allocatorManager(bdem_AggregateOption::BDEM_PASS_THROUGH, basicAllocator)
, d_catalogContainer(copyCatalog(*original.catalog(),
                                 d_allocatorManager.internalAllocator()),
                     d_allocatorManager.allocationStrategy())
, d_headers(original.d_headers.size(),
            bdem_ChoiceHeader(catalog(),
                              d_allocatorManager.allocationStrategy()),
            d_allocatorManager.internalAllocator())
, d_nullBits(original.d_nullBits, basicAllocator)
{
    const int numItems = length();
    for (int i = 0; i < numItems; ++i) {
        d_headers[i].makeSelection(original.d_headers[i].selector(),
                                   original.d_headers[i].selectionPointer());
    }
}

bdem_ChoiceArrayImp::bdem_ChoiceArrayImp(
                  const bdem_ChoiceArrayImp&                original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_catalogContainer(copyCatalog(*original.catalog(),
                                 d_allocatorManager.internalAllocator()),
                     d_allocatorManager.allocationStrategy())
, d_headers(original.d_headers.size(),
            bdem_ChoiceHeader(catalog(),
                              d_allocatorManager.allocationStrategy()),
            d_allocatorManager.internalAllocator())
, d_nullBits(original.d_nullBits, basicAllocator)
{
    const int numItems = length();
    for (int i = 0; i < numItems; ++i) {
        d_headers[i].makeSelection(original.d_headers[i].selector(),
                                   original.d_headers[i].selectionPointer());
    }
}

bdem_ChoiceArrayImp::~bdem_ChoiceArrayImp()
{
    d_headers.clear();  // Calling 'clear' will call the destructor for each
                        // header, which will call its 'reset' function.
}

// MANIPULATORS
bdem_ChoiceArrayImp&
bdem_ChoiceArrayImp::operator=(const bdem_ChoiceArrayImp& rhs)
{
    if (this != &rhs) {
        const int numItems = length();
        for (int i = 0; i < numItems; ++i) {
            d_headers[i].reset();
        }

        // Make all items non-null for exception-safety.
        bsl::memset((void *)&d_nullBits.front(),
                    0,
                    d_nullBits.size() * sizeof(int));

        DescriptorCatalog *catalogPtr = catalog();
        catalogPtr->clear();
        *catalogPtr = *rhs.catalog();

        const int rhsLen = rhs.length();
        if (rhsLen > numItems) {
            d_nullBits.resize(rhs.d_nullBits.size(), 0);
        }
        d_headers.resize(rhsLen,
                         bdem_ChoiceHeader(
                                     catalogPtr,
                                     d_allocatorManager.allocationStrategy()));

        for (int i = 0; i < rhsLen; ++i) {
            d_headers[i].makeSelection(rhs.d_headers[i].selector(),
                                       rhs.d_headers[i].selectionPointer());
        }
        d_nullBits = rhs.d_nullBits;
    }
    return *this;
}

void bdem_ChoiceArrayImp::insertNullItems(int dstIndex, int numItems)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());
    BSLS_ASSERT(0 <= numItems);

    if (0 == numItems) {
        return;                                                       // RETURN
    }

    const int newSize = nullBitsArraySize(length() + numItems);
    if ((int) d_nullBits.size() < newSize) {
        d_nullBits.resize(newSize);
    }

    bdem_ChoiceHeader header(catalog(),
                             d_allocatorManager.allocationStrategy());
    d_headers.insert(d_headers.begin() + dstIndex, numItems, header);

    bdeu_BitstringUtil::insert1(&d_nullBits.front(),
                                length() - numItems,
                                dstIndex,
                                numItems);
}

void bdem_ChoiceArrayImp::insertItem(int                      dstIndex,
                                     const bdem_ChoiceHeader& src)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    // Check if the header being added references an element inside the choice
    // array imp.  If it is, we need a temporary copy when inserting an empty
    // array element.

    if (!d_headers.empty() && &d_headers.front() <= &src
     && &src <= &d_headers.back()) {
        bdem_ChoiceHeader tempHdr(src);
        insertNullItems(dstIndex, 1);
        d_headers[dstIndex] = tempHdr;
    }
    else {
        insertNullItems(dstIndex, 1);
        d_headers[dstIndex] = src;
    }
    // Assume that the inserted choice header is non-null
    bdeu_BitstringUtil::set(&d_nullBits.front(), dstIndex, false);
}

void bdem_ChoiceArrayImp::insertItem(int                        dstIndex,
                                     const bdem_ChoiceArrayImp& srcArray,
                                     int                        srcIndex)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(     srcIndex < srcArray.length());

    bool nullBit = bdeu_BitstringUtil::get(&srcArray.d_nullBits.front(),
                                           srcIndex);
    if (this != &srcArray) {
        insertNullItems(dstIndex, 1);
        d_headers[dstIndex] = srcArray.theItem(srcIndex);
    }
    else {
        bdem_ChoiceHeader tempHdr(srcArray.theItem(srcIndex));
        insertNullItems(dstIndex, 1);
        d_headers[dstIndex] = tempHdr;
    }
    bdeu_BitstringUtil::set(&d_nullBits.front(), dstIndex, nullBit);
}

bdem_ElemRef bdem_ChoiceArrayImp::itemElemRef(int itemIndex)
{
    BSLS_ASSERT(0 <= itemIndex);
    BSLS_ASSERT(     itemIndex < length());

    const int arrayIdx = itemIndex / BDEM_BITS_PER_INT;
    const int offset   = itemIndex % BDEM_BITS_PER_INT;

    return bdem_ElemRef(&d_headers[itemIndex],
                        &bdem_ChoiceHeader::d_choiceItemAttr,
                        &d_nullBits[arrayIdx],
                        offset);
}

void bdem_ChoiceArrayImp::makeItemsNull(int index, int numItems)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(0 <= numItems);
    BSLS_ASSERT(index + numItems <= length());

    const int endIdx = index + numItems;
    for (int i = index; i < endIdx; ++i) {
        d_headers[i].reset();
    }
    bdeu_BitstringUtil::set(&d_nullBits.front(), index, true, numItems);
}

bdem_ElemRef bdem_ChoiceArrayImp::makeSelection(int itemIndex, int selection)
{
    BSLS_ASSERT( 0 <= itemIndex);
    BSLS_ASSERT(      itemIndex < length());
    BSLS_ASSERT(-1 <= selection);
    BSLS_ASSERT(      selection < numSelections());

    bdeu_BitstringUtil::set(&d_nullBits.front(), itemIndex, false);
    bdem_ChoiceHeader& header = d_headers[itemIndex];
    return bdem_ElemRef(header.makeSelection(selection),
                        header.selectionDescriptor(selection),
                        &header.flags(),
                        (int) bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

void bdem_ChoiceArrayImp::reset(const bdem_ElemType::Type    selectionTypes[],
                                int                          numSelections,
                                const bdem_Descriptor *const attrLookupTbl[])
{
    BSLS_ASSERT(0 <= numSelections);

    d_headers.clear();  // Calls the destructors for individual headers which
                        // will call the 'reset' method on each of them.
    d_nullBits.resize(1, 0);

    DescriptorCatalog *catalogPtr = catalog();
    catalogPtr->resize(numSelections);

    for (int i = 0; i < numSelections; ++i) {
        BSLS_ASSERT(static_cast<unsigned>(selectionTypes[i])
                     < static_cast<unsigned>(bdem_ElemType::BDEM_NUM_TYPES));
        (*catalogPtr)[i] = attrLookupTbl[static_cast<int>(selectionTypes[i])];
    }
}

void bdem_ChoiceArrayImp::reserveRaw(bsl::size_t numItems)
{
    const int newSize = nullBitsArraySize(numItems);
    d_nullBits.reserve(newSize);
    d_headers.reserve(numItems);
}

// ACCESSORS
bdem_ConstElemRef bdem_ChoiceArrayImp::itemElemRef(int itemIndex) const
{
    BSLS_ASSERT(0 <= itemIndex);
    BSLS_ASSERT(     itemIndex < length());

    const int arrayIdx = itemIndex / BDEM_BITS_PER_INT;
    const int offset   = itemIndex % BDEM_BITS_PER_INT;

    return bdem_ConstElemRef(&d_headers[itemIndex],
                             &bdem_ChoiceHeader::d_choiceItemAttr,
                             &d_nullBits[arrayIdx],
                             offset);
}

bsl::ostream& bdem_ChoiceArrayImp::print(bsl::ostream& stream,
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
        // If spacesPerLevel is negative, just put one space between rows
        // and suppress newlines when formatting each row.
        nestedLevel = 1;
        nestedSpacesPerLevel = -1;
    }

    // Print out column types:
    bdeu_Print::indent(stream, nestedLevel, nestedSpacesPerLevel);
    stream << "Selection Types: [";
    const int numTypes = numSelections();
    for (int i = 0; i < numTypes; ++i) {
        bdeu_Print::newlineAndIndent(stream,
                                     nestedLevel + 1,
                                     nestedSpacesPerLevel);
        stream << selectionType(i);
    }
    bdeu_Print::newlineAndIndent(stream, nestedLevel, nestedSpacesPerLevel);
    stream << ']';
    if (0 <= spacesPerLevel) {
        stream << '\n';
    }
    stream << bsl::flush;

    const int len = length();
    for (int i = 0; i < len; ++i) {
        bdeu_Print::indent(stream, nestedLevel, nestedSpacesPerLevel);
        stream << "Item " << i << ": ";
        if (bdeu_BitstringUtil::get(&d_nullBits.front(), i)) {
            stream << "{ NULL }";
        }
        else {
            d_headers[i].print(stream, -nestedLevel, nestedSpacesPerLevel);
        }
        stream << bsl::flush;
    }

    if (0 <= spacesPerLevel) {
        bdeu_Print::indent(stream, level, nestedSpacesPerLevel);
        stream << "}\n";
    }
    else {
        stream << " }";
    }
    return stream << bsl::flush;
}

// FREE OPERATORS
bool operator==(const bdem_ChoiceArrayImp& lhs, const bdem_ChoiceArrayImp& rhs)
{
    enum { BDEM_BITS_PER_INT = 32 };

    const int numItems          = lhs.d_headers.size();
    const int nullBitsArraySize = (numItems + BDEM_BITS_PER_INT - 1)
                                                           / BDEM_BITS_PER_INT;
    return lhs.d_headers  == rhs.d_headers
        && bsl::equal(&lhs.d_nullBits.front(),
                      &lhs.d_nullBits.front() + nullBitsArraySize,
                      &rhs.d_nullBits.front())
        && *lhs.catalog() == *rhs.catalog();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
