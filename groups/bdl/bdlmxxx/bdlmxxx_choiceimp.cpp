// bdlmxxx_choiceimp.cpp                                                 -*-C++-*-
#include <bdlmxxx_choiceimp.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_choiceimp_cpp,"$Id$ $CSID$")

#include <bdlmxxx_functiontemplates.h>
#include <bdlmxxx_properties.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_rawdeleterproctor.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
bdlmxxx::ChoiceHeader::DescriptorCatalog *
createCatalog(const bdlmxxx::ElemType::Type    *selectionTypes,
              int                           numSelectionTypes,
              const bdlmxxx::Descriptor *const *attrLookupTbl,
              bdlmxxx::AllocatorManager        *allocatorManager)
    // Create a catalog having the specified initial 'numSelectionTypes' in the
    // specified 'selectionTypes' array, further described by the specified
    // 'attrLookupTbl', and using the allocator and allocation strategy
    // specified by 'allocatorManager'.
{
    BSLS_ASSERT(0 <= numSelectionTypes);
    BSLS_ASSERT(allocatorManager);

    typedef bdlmxxx::ChoiceHeader::DescriptorCatalog DescriptorCatalog;

    bslma::Allocator *internalAllocator =
                                         allocatorManager->internalAllocator();

    DescriptorCatalog *result =
                 new (*internalAllocator) DescriptorCatalog(internalAllocator);

    bslma::RawDeleterProctor<DescriptorCatalog, bslma::Allocator>
                                     catalogProctor(result, internalAllocator);

    result->reserve(numSelectionTypes);

    for (int i = 0; i < numSelectionTypes; ++i) {
        BSLS_ASSERT((unsigned)selectionTypes[i]
                                    < (unsigned)bdlmxxx::ElemType::BDEM_NUM_TYPES);

        result->push_back(attrLookupTbl[selectionTypes[i]]);
    }

    catalogProctor.release();

    return result;
}

static
bdlmxxx::ChoiceHeader::DescriptorCatalog *
copyCatalog(const bdlmxxx::ChoiceHeader::DescriptorCatalog&  original,
            bdlmxxx::AllocatorManager                       *allocatorManager)
    // Create a copy of the specified 'original' catalog using the allocator
    // and allocation strategy specified by 'allocatorManager'.
{
    BSLS_ASSERT(allocatorManager);

    typedef bdlmxxx::ChoiceHeader::DescriptorCatalog DescriptorCatalog;

    bslma::Allocator *internalAllocator =
                                         allocatorManager->internalAllocator();

    DescriptorCatalog *result =
                 new (*internalAllocator) DescriptorCatalog(original,
                                                            internalAllocator);
    return result;
}

static
void destroyCatalog(bdlmxxx::ChoiceHeader::DescriptorCatalog *catalog,
                    bdlmxxx::AllocatorManager                *allocatorManager)
    // Destroy the specified 'catalog' using the allocator specified by
    // 'allocatorManager'.
{
    BSLS_ASSERT(allocatorManager);

    if (!(bdlmxxx::AggregateOption::BDEM_NODESTRUCT_FLAG &
                                     allocatorManager->allocationStrategy())) {
        bslma::Allocator *allocator = allocatorManager->internalAllocator();
        allocator->deleteObject(catalog);
    }
}

                       // ==============================
                       // class bdem_ChoiceImp_AttrFuncs
                       // ==============================

struct bdem_ChoiceImp_AttrFuncs {
    // Namespace for static functions to store in a 'bdlmxxx::Descriptor'.

    // CLASS METHODS
    static
    void defaultConstruct(
                  void                                     *obj,
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc);
        // Construct a choice object into raw memory.  The prototype for this
        // function must match the defaultConstruct function pointer in
        // bdem_Descriptor.

    static
    void copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc);
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
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc)
{
    BSLS_ASSERT(obj);

    new (obj) bdlmxxx::ChoiceImp(allocationStrategy, alloc);
}

void
bdem_ChoiceImp_AttrFuncs::copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc)
{
    BSLS_ASSERT(obj);
    BSLS_ASSERT(rhs);

    const bdlmxxx::ChoiceImp& rhsChoice =
                                     *static_cast<const bdlmxxx::ChoiceImp *>(rhs);
    new (obj) bdlmxxx::ChoiceImp(rhsChoice, allocationStrategy, alloc);
}

void bdem_ChoiceImp_AttrFuncs::makeUnset(void *obj)
{
    BSLS_ASSERT(obj);

    static_cast<bdlmxxx::ChoiceImp *>(obj)->reset();
}

bool bdem_ChoiceImp_AttrFuncs::isUnset(const void *obj)
{
    BSLS_ASSERT(obj);

    const bdlmxxx::ChoiceImp& choice = *static_cast<const bdlmxxx::ChoiceImp *>(obj);
    return -1 == choice.selector() && 0 == choice.numSelections();
}

bsl::ostream& bdem_ChoiceImp_AttrFuncs::print(const void    *obj,
                                              bsl::ostream&  stream,
                                              int            level,
                                              int            spacesPerLevel)
{
    BSLS_ASSERT(obj);

    static_cast<const bdlmxxx::ChoiceImp *>(obj)->print(stream,
                                                    level,
                                                    spacesPerLevel);
    return stream;
}

                        // --------------------
                        // class bdlmxxx::ChoiceImp
                        // --------------------

// CLASS DATA
const bdlmxxx::Descriptor bdlmxxx::ChoiceImp::s_choiceAttr =
{
    bdlmxxx::ElemType::BDEM_CHOICE,
    sizeof(bdlmxxx::ChoiceImp),
    bsls::AlignmentFromType<bdlmxxx::ChoiceImp>::VALUE,
    &bdem_ChoiceImp_AttrFuncs::defaultConstruct,
    &bdem_ChoiceImp_AttrFuncs::copyConstruct,
    &bdlmxxx::FunctionTemplates::destroy<bdlmxxx::ChoiceImp>,
    &bdlmxxx::FunctionTemplates::assign<bdlmxxx::ChoiceImp>,
    &bdlmxxx::FunctionTemplates::bitwiseMove<bdlmxxx::ChoiceImp>,
    &bdem_ChoiceImp_AttrFuncs::makeUnset,
    &bdem_ChoiceImp_AttrFuncs::isUnset,
    &bdlmxxx::FunctionTemplates::areEqual<bdlmxxx::ChoiceImp>,
    &bdem_ChoiceImp_AttrFuncs::print
};

namespace bdlmxxx {
// CREATORS
ChoiceImp::ChoiceImp(bslma::Allocator *basicAllocator)
: d_allocatorManager(AggregateOption::BDEM_PASS_THROUGH, basicAllocator)
, d_header(createCatalog(0, 0, 0, &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
}

ChoiceImp::ChoiceImp(
    AggregateOption::AllocationStrategy  allocationStrategy,
    bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_header(createCatalog(0, 0, 0, &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
}

ChoiceImp::ChoiceImp(
                  const ElemType::Type                 selectionTypes[],
                  int                                       numSelectionTypes,
                  const Descriptor *const              attrLookupTbl[],
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_header(createCatalog(selectionTypes,
                         numSelectionTypes,
                         attrLookupTbl,
                         &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
    BSLS_ASSERT(0 <= numSelectionTypes);
}

ChoiceImp::ChoiceImp(
                  const ChoiceHeader&                  choiceHeader,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_header(copyCatalog(*choiceHeader.catalog(), &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
    DescriptorCatalog *catalog =
                          const_cast<DescriptorCatalog *>(d_header.catalog());
    bslma::Allocator  *internalAllocator =
                                        d_allocatorManager.internalAllocator();

    // Proctor for the catalog
    bslma::RawDeleterProctor<DescriptorCatalog, bslma::Allocator>
                                    catalogProctor(catalog, internalAllocator);

    d_header = choiceHeader;

    catalogProctor.release();
}

ChoiceImp::ChoiceImp(const ChoiceImp&  original,
                               bslma::Allocator      *basicAllocator)
: d_allocatorManager(AggregateOption::BDEM_PASS_THROUGH, basicAllocator)
, d_header(copyCatalog(*original.d_header.catalog(), &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
    DescriptorCatalog *catalog =
                          const_cast<DescriptorCatalog *>(d_header.catalog());
    bslma::Allocator  *internalAllocator =
                                        d_allocatorManager.internalAllocator();

    // Proctor for the catalog
    bslma::RawDeleterProctor<DescriptorCatalog, bslma::Allocator>
                                    catalogProctor(catalog, internalAllocator);

    d_header = original.d_header;

    catalogProctor.release();
}

ChoiceImp::ChoiceImp(
                  const ChoiceImp&                     original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
, d_header(copyCatalog(*original.d_header.catalog(), &d_allocatorManager),
           d_allocatorManager.allocationStrategy())
{
    DescriptorCatalog *catalog =
                          const_cast<DescriptorCatalog *>(d_header.catalog());
    bslma::Allocator  *internalAllocator =
                                        d_allocatorManager.internalAllocator();

    // Proctor for the catalog
    bslma::RawDeleterProctor<DescriptorCatalog, bslma::Allocator>
                                    catalogProctor(catalog, internalAllocator);

    d_header = original.d_header;

    catalogProctor.release();
}

ChoiceImp::~ChoiceImp()
{
    // Ensure that choice header will not use allocator or descriptor catalog
    // during destruction.

    d_header.reset();

    destroyCatalog(const_cast<DescriptorCatalog *>(d_header.catalog()),
                   &d_allocatorManager);
}

// MANIPULATORS
ChoiceImp& ChoiceImp::operator=(const ChoiceImp& rhs)
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

ElemRef ChoiceImp::makeSelection(int index)
{
    BSLS_ASSERT(-1 <= index);
    BSLS_ASSERT(      index < numSelections());

    return ElemRef(d_header.makeSelection(index),
                        d_header.selectionDescriptor(index),
                        &d_header.flags(),
                        (int)ChoiceHeader::BDEM_NULLNESS_FLAG);
}

ElemRef ChoiceImp::makeSelection(int index, const void *value)
{
    BSLS_ASSERT(-1 <= index);
    BSLS_ASSERT(      index < numSelections());

    return ElemRef(d_header.makeSelection(index, value),
                        d_header.selectionDescriptor(index),
                        &d_header.flags(),
                        (int)ChoiceHeader::BDEM_NULLNESS_FLAG);
}

void ChoiceImp::reset(const ElemType::Type    selectionTypes[],
                           int                          numSelectionTypes,
                           const Descriptor *const attrLookupTbl[])
{
    BSLS_ASSERT(0 <= numSelectionTypes);

    d_header.reset();

    DescriptorCatalog *catalog =
                           const_cast<DescriptorCatalog *>(d_header.catalog());

    catalog->clear();
    catalog->resize(numSelectionTypes);

    for (int i = 0; i < numSelectionTypes; ++i) {
        BSLS_ASSERT((unsigned)selectionTypes[i]
                                    < (unsigned)ElemType::BDEM_NUM_TYPES);

        (*catalog)[i] = attrLookupTbl[selectionTypes[i]];
    }
}

void *ChoiceImp::setSelectionValue(const void *value)
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
bsl::ostream& ChoiceImp::print(bsl::ostream& stream,
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
    bdlb::Print::indent(stream, nestedLevel, nestedSpacesPerLevel);
    stream << "Selection Types: [";
    int len = numSelections();
    for (int i = 0; i < len; ++i) {
        bdlb::Print::newlineAndIndent(stream,
                                     nestedLevel + 1,
                                     nestedSpacesPerLevel);
        stream << selectionType(i);
    }
    bdlb::Print::newlineAndIndent(stream, nestedLevel, nestedSpacesPerLevel);
    stream << ']';
    if (0 <= spacesPerLevel) {
        stream << '\n';
    }
    stream << bsl::flush;

    d_header.print(stream, nestedLevel, nestedSpacesPerLevel);

    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, level, nestedSpacesPerLevel);
        stream << "}\n";
    }
    else {
        stream << " }";
    }
    return stream << bsl::flush;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
