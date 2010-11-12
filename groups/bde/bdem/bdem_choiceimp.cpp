// bdem_choiceimp.cpp                                                 -*-C++-*-
#include <bdem_choiceimp.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_choiceimp_cpp,"$Id$ $CSID$")

#include <bdem_functiontemplates.h>
#include <bdem_properties.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_rawdeleterproctor.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
bdem_ChoiceHeader::DescriptorCatalog *
createCatalog(const bdem_ElemType::Type    *selectionTypes,
              int                           numSelectionTypes,
              const bdem_Descriptor *const *attrLookupTbl,
              bdem_AllocatorManager        *allocatorManager)
    // Create a catalog having the specified initial 'numSelectionTypes' in the
    // specified 'selectionTypes' array, further described by the specified
    // 'attrLookupTbl', and using the allocator and allocation strategy
    // specified by 'allocatorManager'.
{
    BSLS_ASSERT(0 <= numSelectionTypes);
    BSLS_ASSERT(allocatorManager);

    typedef bdem_ChoiceHeader::DescriptorCatalog DescriptorCatalog;

    bslma_Allocator *internalAllocator = allocatorManager->internalAllocator();

    DescriptorCatalog *result =
                 new (*internalAllocator) DescriptorCatalog(internalAllocator);

    bslma_RawDeleterProctor<DescriptorCatalog, bslma_Allocator>
                                     catalogProctor(result, internalAllocator);

    result->reserve(numSelectionTypes);

    for (int i = 0; i < numSelectionTypes; ++i) {
        BSLS_ASSERT((unsigned)selectionTypes[i]
                                    < (unsigned)bdem_ElemType::BDEM_NUM_TYPES);

        result->push_back(attrLookupTbl[selectionTypes[i]]);
    }

    catalogProctor.release();

    return result;
}

static
bdem_ChoiceHeader::DescriptorCatalog *
copyCatalog(const bdem_ChoiceHeader::DescriptorCatalog&  original,
            bdem_AllocatorManager                       *allocatorManager)
    // Create a copy of the specified 'original' catalog using the allocator
    // and allocation strategy specified by 'allocatorManager'.
{
    BSLS_ASSERT(allocatorManager);

    typedef bdem_ChoiceHeader::DescriptorCatalog DescriptorCatalog;

    bslma_Allocator *internalAllocator = allocatorManager->internalAllocator();

    DescriptorCatalog *result =
                 new (*internalAllocator) DescriptorCatalog(original,
                                                            internalAllocator);
    return result;
}

static
void destroyCatalog(bdem_ChoiceHeader::DescriptorCatalog *catalog,
                    bdem_AllocatorManager                *allocatorManager)
    // Destroy the specified 'catalog' using the allocator specified by
    // 'allocatorManager'.
{
    BSLS_ASSERT(allocatorManager);

    if (!(bdem_AggregateOption::BDEM_NODESTRUCT_FLAG &
                                     allocatorManager->allocationStrategy())) {
        bslma_Allocator *allocator = allocatorManager->internalAllocator();
        allocator->deleteObject(catalog);
    }
}

                       // ==============================
                       // class bdem_ChoiceImp_AttrFuncs
                       // ==============================

struct bdem_ChoiceImp_AttrFuncs {
    // Namespace for static functions to store in a 'bdem_Descriptor'.

    // CLASS METHODS
    static
    void defaultConstruct(
                  void                                     *obj,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc);
        // Construct a choice object into raw memory.  The prototype for this
        // function must match the defaultConstruct function pointer in
        // bdem_Descriptor.

    static
    void copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc);
        // Copy construct a choice object into raw memory.  The prototype for
        // this function must match the copyConstruct function pointer in
        // bdem_Descriptor.

    static void makeUnset(void *obj);
        // Reset the 'obj' choice object to the default-constructed state.

    static bool isUnset(const void *obj);
        // Returns 'true' if this choice object has no selection, and 'false'
        // otherwise.

    static
    bsl::ostream& print(const void    *obj,
                        bsl::ostream&  stream,
                        int            level,
                        int            spacesPerLevel);
        // Print this choice.
};

                       // ------------------------------
                       // class bdem_ChoiceImp_AttrFuncs
                       // ------------------------------

// CLASS METHODS
void
bdem_ChoiceImp_AttrFuncs::defaultConstruct(
                  void                                     *obj,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc)
{
    BSLS_ASSERT(obj);

    new (obj) bdem_ChoiceImp(allocationStrategy, alloc);
}

void
bdem_ChoiceImp_AttrFuncs::copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc)
{
    BSLS_ASSERT(obj);
    BSLS_ASSERT(rhs);

    const bdem_ChoiceImp& rhsChoice =
                                     *static_cast<const bdem_ChoiceImp *>(rhs);
    new (obj) bdem_ChoiceImp(rhsChoice, allocationStrategy, alloc);
}

void bdem_ChoiceImp_AttrFuncs::makeUnset(void *obj)
{
    BSLS_ASSERT(obj);

    static_cast<bdem_ChoiceImp *>(obj)->reset();
}

bool bdem_ChoiceImp_AttrFuncs::isUnset(const void *obj)
{
    BSLS_ASSERT(obj);

    const bdem_ChoiceImp& choice = *static_cast<const bdem_ChoiceImp *>(obj);
    return -1 == choice.selector() && 0 == choice.numSelections();
}

bsl::ostream& bdem_ChoiceImp_AttrFuncs::print(const void    *obj,
                                              bsl::ostream&  stream,
                                              int            level,
                                              int            spacesPerLevel)
{
    BSLS_ASSERT(obj);

    static_cast<const bdem_ChoiceImp *>(obj)->print(stream,
                                                    level,
                                                    spacesPerLevel);
    return stream;
}

                        // --------------------
                        // class bdem_ChoiceImp
                        // --------------------

// CLASS DATA
const bdem_Descriptor bdem_ChoiceImp::d_choiceAttr =
{
    bdem_ElemType::BDEM_CHOICE,
    sizeof(bdem_ChoiceImp),
    bsls_AlignmentFromType<bdem_ChoiceImp>::VALUE,
    &bdem_ChoiceImp_AttrFuncs::defaultConstruct,
    &bdem_ChoiceImp_AttrFuncs::copyConstruct,
    &bdem_FunctionTemplates::destroy<bdem_ChoiceImp>,
    &bdem_FunctionTemplates::assign<bdem_ChoiceImp>,
    &bdem_FunctionTemplates::bitwiseMove<bdem_ChoiceImp>,
    &bdem_ChoiceImp_AttrFuncs::makeUnset,
    &bdem_ChoiceImp_AttrFuncs::isUnset,
    &bdem_FunctionTemplates::areEqual<bdem_ChoiceImp>,
    &bdem_ChoiceImp_AttrFuncs::print
};

// CREATORS
bdem_ChoiceImp::bdem_ChoiceImp(bslma_Allocator *basicAllocator)
: d_allocatorManager(bdem_AggregateOption::BDEM_PASS_THROUGH, basicAllocator)
, d_header(createCatalog(0, 0, 0, &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
}

bdem_ChoiceImp::bdem_ChoiceImp(
    bdem_AggregateOption::AllocationStrategy  allocationStrategy,
    bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_header(createCatalog(0, 0, 0, &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
}

bdem_ChoiceImp::bdem_ChoiceImp(
                  const bdem_ElemType::Type                 selectionTypes[],
                  int                                       numSelectionTypes,
                  const bdem_Descriptor *const              attrLookupTbl[],
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_header(createCatalog(selectionTypes,
                         numSelectionTypes,
                         attrLookupTbl,
                         &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
    BSLS_ASSERT(0 <= numSelectionTypes);
}

bdem_ChoiceImp::bdem_ChoiceImp(
                  const bdem_ChoiceHeader&                  choiceHeader,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_header(copyCatalog(*choiceHeader.catalog(), &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
    DescriptorCatalog *catalog =
                          const_cast<DescriptorCatalog *>(d_header.catalog());
    bslma_Allocator   *internalAllocator =
                                        d_allocatorManager.internalAllocator();

    // Proctor for the catalog
    bslma_RawDeleterProctor<DescriptorCatalog, bslma_Allocator>
                                    catalogProctor(catalog, internalAllocator);

    d_header = choiceHeader;

    catalogProctor.release();
}

bdem_ChoiceImp::bdem_ChoiceImp(const bdem_ChoiceImp&  original,
                               bslma_Allocator       *basicAllocator)
: d_allocatorManager(bdem_AggregateOption::BDEM_PASS_THROUGH, basicAllocator)
, d_header(copyCatalog(*original.d_header.catalog(), &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
    DescriptorCatalog *catalog =
                          const_cast<DescriptorCatalog *>(d_header.catalog());
    bslma_Allocator   *internalAllocator =
                                        d_allocatorManager.internalAllocator();

    // Proctor for the catalog
    bslma_RawDeleterProctor<DescriptorCatalog, bslma_Allocator>
                                    catalogProctor(catalog, internalAllocator);

    d_header = original.d_header;

    catalogProctor.release();
}

bdem_ChoiceImp::bdem_ChoiceImp(
                  const bdem_ChoiceImp&                     original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_header(copyCatalog(*original.d_header.catalog(), &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
    DescriptorCatalog *catalog =
                          const_cast<DescriptorCatalog *>(d_header.catalog());
    bslma_Allocator   *internalAllocator =
                                        d_allocatorManager.internalAllocator();

    // Proctor for the catalog
    bslma_RawDeleterProctor<DescriptorCatalog, bslma_Allocator>
                                    catalogProctor(catalog, internalAllocator);

    d_header = original.d_header;

    catalogProctor.release();
}

bdem_ChoiceImp::~bdem_ChoiceImp()
{
    // Ensure that choice header will not use allocator or descriptor catalog
    // during destruction.

    d_header.reset();

    destroyCatalog(const_cast<DescriptorCatalog *>(d_header.catalog()),
                   &d_allocatorManager);
}

// MANIPULATORS
bdem_ChoiceImp& bdem_ChoiceImp::operator=(const bdem_ChoiceImp& rhs)
{
    if (this != &rhs) {
        DescriptorCatalog *catalog =
                          const_cast<DescriptorCatalog *>(d_header.catalog());
        d_header.reset();

        *catalog = *rhs.d_header.catalog();
        d_header = rhs.d_header;
    }
    return *this;
}

bdem_ElemRef bdem_ChoiceImp::makeSelection(int index)
{
    BSLS_ASSERT(-1 <= index);
    BSLS_ASSERT(      index < numSelections());

    return bdem_ElemRef(d_header.makeSelection(index),
                        d_header.selectionDescriptor(index),
                        &d_header.flags(),
                        (int)bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

bdem_ElemRef bdem_ChoiceImp::makeSelection(int index, const void *value)
{
    BSLS_ASSERT(-1 <= index);
    BSLS_ASSERT(      index < numSelections());

    return bdem_ElemRef(d_header.makeSelection(index, value),
                        d_header.selectionDescriptor(index),
                        &d_header.flags(),
                        (int)bdem_ChoiceHeader::BDEM_NULLNESS_FLAG);
}

void bdem_ChoiceImp::reset(const bdem_ElemType::Type    selectionTypes[],
                           int                          numSelectionTypes,
                           const bdem_Descriptor *const attrLookupTbl[])
{
    BSLS_ASSERT(0 <= numSelectionTypes);

    d_header.reset();

    DescriptorCatalog *catalog =
                           const_cast<DescriptorCatalog *>(d_header.catalog());

    catalog->clear();
    catalog->resize(numSelectionTypes);

    for (int i = 0; i < numSelectionTypes; ++i) {
        BSLS_ASSERT((unsigned)selectionTypes[i]
                                    < (unsigned)bdem_ElemType::BDEM_NUM_TYPES);

        (*catalog)[i] = attrLookupTbl[selectionTypes[i]];
    }
}

void *bdem_ChoiceImp::setSelectionValue(const void *value)
{
    if (-1 == selector()) {
        return 0;                                                     // RETURN
    }

    void *selectionPtr = selectionPointer();
    d_header.clearNullnessBit();
    d_header.selectionDescriptor(selector())->assign(selectionPtr, value);
    return selectionPtr;
}

// ACCESSORS
bsl::ostream& bdem_ChoiceImp::print(bsl::ostream& stream,
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
    stream << "Selection Types: [";
    int len = numSelections();
    for (int i = 0; i < len; ++i) {
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

    d_header.print(stream, nestedLevel, nestedSpacesPerLevel);

    if (0 <= spacesPerLevel) {
        bdeu_Print::indent(stream, level, nestedSpacesPerLevel);
        stream << "}\n";
    }
    else {
        stream << " }";
    }
    return stream << bsl::flush;
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
