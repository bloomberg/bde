// bdlmxxx_choiceheader.cpp                                              -*-C++-*-
#include <bdlmxxx_choiceheader.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_choiceheader_cpp,"$Id$ $CSID$")

#include <bdlmxxx_elemtype.h>
#include <bdlmxxx_functiontemplates.h>   // for testing only
#include <bdlmxxx_properties.h>

#include <bslma_deallocatorproctor.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                    // --------------------------------------
                    // CHOICE_ARRAY_ITEM DESCRIPTOR FUNCTIONS
                    // --------------------------------------

static
void makeChoiceItemUnset(void *obj)
{
    BSLS_ASSERT(obj);

    bdlmxxx::ChoiceHeader& header = *static_cast<bdlmxxx::ChoiceHeader *>(obj);
    header.reset();
}

static
bool isChoiceItemUnset(const void *obj)
{
    BSLS_ASSERT(obj);

    return static_cast<const bdlmxxx::ChoiceHeader *>(obj)->isSelectionNull();
}

static
bool areChoiceItemsEqual(const void *lhs, const void *rhs)
{
    BSLS_ASSERT(lhs);
    BSLS_ASSERT(rhs);

    const bdlmxxx::ChoiceHeader& lhsChoiceItem =
                                  *static_cast<const bdlmxxx::ChoiceHeader *>(lhs);
    const bdlmxxx::ChoiceHeader& rhsChoiceItem =
                                  *static_cast<const bdlmxxx::ChoiceHeader *>(rhs);

    return lhsChoiceItem == rhsChoiceItem;
}

static
bsl::ostream& printChoiceItem(const void    *obj,
                              bsl::ostream&  stream,
                              int            level,
                              int            spacesPerLevel)
{
    BSLS_ASSERT(obj);

    const bdlmxxx::ChoiceHeader& choiceItem =
                                  *static_cast<const bdlmxxx::ChoiceHeader *>(obj);
    return choiceItem.print(stream, level, spacesPerLevel);
}

                        // -----------------------
                        // class bdlmxxx::ChoiceHeader
                        // -----------------------

// CLASS DATA
const bdlmxxx::Descriptor bdlmxxx::ChoiceHeader::s_choiceItemAttr =
{
    bdlmxxx::ElemType::BDEM_CHOICE_ARRAY_ITEM,              // element enumeration
    sizeof(bdlmxxx::ChoiceHeader),                          // size
    bsls::AlignmentFromType<bdlmxxx::ChoiceHeader>::VALUE,  // alignment
    0,                                                  // unsetConstruct,
    0,                                                  // copyConstruct
    0,                                                  // destroy
    0,                                                  // assign
    0,                                                  // move
    &makeChoiceItemUnset,                               // makeUnset
    &isChoiceItemUnset,                                 // isUnset
    &areChoiceItemsEqual,                               // areEqual
    &printChoiceItem                                    // print
};

namespace bdlmxxx {
// CREATORS
ChoiceHeader::ChoiceHeader(
                         const DescriptorCatalog                  *catalogPtr,
                         AggregateOption::AllocationStrategy  allocMode)
: d_catalog_p(catalogPtr)
, d_selector(-1)
, d_flags(1)
, d_allocMode(AggregateOption::AllocationStrategy(
                   allocMode & ~AggregateOption::BDEM_OWN_ALLOCATOR_FLAG))
{
    d_selection_p = 0;  // eases debugging
}

ChoiceHeader::ChoiceHeader(
                           const ChoiceHeader&                 original,
                           AggregateOption::AllocationStrategy allocMode)
: d_catalog_p(original.d_catalog_p)
, d_selector(-1)
, d_flags(1)
, d_allocMode(AggregateOption::AllocationStrategy(
                   allocMode & ~AggregateOption::BDEM_OWN_ALLOCATOR_FLAG))
{
    d_selection_p = 0;           // eases debugging
    makeSelection(original.d_selector, original.selectionPointer());
    d_flags = original.d_flags;  // 'makeSelection' set as non-null
}

ChoiceHeader::~ChoiceHeader()
{
    reset();
}

// MANIPULATORS
ChoiceHeader& ChoiceHeader::operator=(const ChoiceHeader& rhs)
{
    if (this != &rhs) {
        // Assume that catalog management is done at a higher level.

        makeSelection(rhs.selector(), rhs.selectionPointer());
        d_flags = rhs.d_flags;  // 'makeSelection' set as non-null
    }
    return *this;
}

void *ChoiceHeader::makeSelection(int index)
{
    BSLS_ASSERT(-1 <= index);
    BSLS_ASSERT(      index < numSelections());

    const Descriptor *descriptorPtr = selectionDescriptor(index);
    void *dataPtr = 0;

    if (-1 == index) {
        reset();
    }
    else if (descriptorPtr == selectionDescriptor(selector())) {
        // Data type has not changed, use 'makeUnset' instead of 'reset'.

        dataPtr = selectionPointer();
        descriptorPtr->makeUnset(dataPtr);
        setNullnessBit();
    }
    else if (static_cast<unsigned>(descriptorPtr->d_size)
                                                    <= sizeof d_selectionBuf) {
        // Data type has changed and new data type fits in 'd_selectionBuf'.
        // Call 'reset' to set 'd_selector' to -1.  If an exception occurs
        // during 'unsetConstruct', 'd_selector' will safely remain -1.

        reset();
        dataPtr = &d_selectionBuf;
        descriptorPtr->unsetConstruct(dataPtr, d_allocMode, allocator());
    }
    else {
        // Data type has changed and new data type is larger than
        // 'd_selectionBuf'.  We must allocate space for the new value.
        // Allocate new memory and protect it from leaking on exception.

        dataPtr = allocator()->allocate(descriptorPtr->d_size);
        bslma::DeallocatorProctor<bslma::Allocator> autoDealloc(dataPtr,
                                                                allocator());

        // Construct an unset value, which *can* throw.

        descriptorPtr->unsetConstruct(dataPtr, d_allocMode, allocator());
        autoDealloc.release();

        reset();  // clear old 'd_selection_p'
        d_selection_p = dataPtr;
    }

    d_selector = index;

    return dataPtr;
}

void *ChoiceHeader::makeSelection(int index, const void *value)
{
    BSLS_ASSERT(-1 <= index);
    BSLS_ASSERT(      index < numSelections());

    const Descriptor *descriptorPtr = selectionDescriptor(index);
    void *dataPtr = 0;

    // Verify that descriptor at index matches 'descriptorPtr'.

    if (-1 == index) {
        reset();
    }
    else if (descriptorPtr == selectionDescriptor(selector())) {
        // Data type has not changed, so use assignment.

        dataPtr = selectionPointer();
        descriptorPtr->assign(dataPtr, value);
        clearNullnessBit();
    }
    else if (static_cast<unsigned>(descriptorPtr->d_size)
                                                    <= sizeof d_selectionBuf) {
        // Data type has changed and new data type fits in 'd_selectionBuf'.
        // There is a chance that 'value' is a sub-part of the current
        // selection ("aliasing").  Thus, we must make a copy of 'value' BEFORE
        // destroying the current selection.  For efficiency and exception
        // safety, we construct a new object in a separate memory space then
        // bit-wise copy it into the current selection.  This works because all
        // 'bdem' types are bit-wise moveable.  If an exception is thrown, the
        // original contents of 'd_selectionBuf' is left unchanged.

        bsls::AlignedBuffer<sizeof d_selectionBuf> temp;
        descriptorPtr->copyConstruct(&temp,
                                     value,
                                     d_allocMode,
                                     allocator());

        reset();                // Destroy old contents of 'd_selectionBuf'.
        d_selectionBuf = temp;  // Bit-wise copy 'temp' to 'd_selectionBuf'.

        dataPtr = &d_selectionBuf;
        clearNullnessBit();

        // Destructor for 'temp' is a no-op.  This is good, since the
        // contents of 'temp' has been "moved" to 'd_selectionBuf'.
    }
    else {
        // Data type had changed and new data type is larger than
        // 'd_selectionBuf'.  We must allocate space for the new value.

        // There is a chance that 'value' is a sub-part of the current
        // selection ("aliasing").  For this reason, as well as for
        // exception safety, we must allocate and construct a copy of
        // 'value' BEFORE destroying the current selection.

        // Allocate new memory and protect it from leaking on exception.

        dataPtr = allocator()->allocate(descriptorPtr->d_size);
        bslma::DeallocatorProctor<bslma::Allocator> autoDealloc(dataPtr,
                                                                allocator());

        // Construct a copy of 'value', which *can* throw.

        descriptorPtr->copyConstruct(dataPtr,
                                     value,
                                     d_allocMode,
                                     allocator());
        autoDealloc.release();

        reset();  // clear old 'd_selection_p'
        d_selection_p = dataPtr;
        clearNullnessBit();
    }

    d_selector = index;

    return dataPtr;
}

void *ChoiceHeader::selectionPointer()
{
    if (-1 == d_selector) {
        return 0;                                                     // RETURN
    }

    const Descriptor *descriptorPtr = (*d_catalog_p)[d_selector];
    return (static_cast<unsigned>(descriptorPtr->d_size)
                                    > sizeof d_selectionBuf) ? d_selection_p
                                                             : &d_selectionBuf;
}

void ChoiceHeader::reset()
{
    if (d_selector >= 0) {
        const Descriptor *descriptorPtr = (*d_catalog_p)[d_selector];
        if (static_cast<unsigned>(descriptorPtr->d_size)
                                                     > sizeof d_selectionBuf) {
            // Destroy and deallocate external value.

            if (!(d_allocMode & AggregateOption::BDEM_NODESTRUCT_FLAG)) {
                descriptorPtr->destroy(d_selection_p);
            }
            allocator()->deallocate(d_selection_p);
        }
        else if (!(d_allocMode & AggregateOption::BDEM_NODESTRUCT_FLAG)) {
            // Destroy in-place value.

            descriptorPtr->destroy(&d_selectionBuf);
        }

        d_selector    = -1;
        d_selection_p = 0;   // eases debugging
    }
    setNullnessBit();  // make null
}

// ACCESSORS
const Descriptor *ChoiceHeader::selectionDescriptor(int index) const
{
    BSLS_ASSERT(-1 <= index);
    BSLS_ASSERT(      index < numSelections());

    return -1 == index ? &Properties::s_voidAttr : (*d_catalog_p)[index];
}

bsl::ostream& ChoiceHeader::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    int nestedLevel, nestedSpacesPerLevel;
    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << '{';
    if (level < 0) {
        level = -level;
    }

    if (0 <= spacesPerLevel) {
        // Output newlines only if 'spacesPerLevel' is non-negative.

        stream << '\n';
        nestedLevel = level + 1;
        nestedSpacesPerLevel = spacesPerLevel;
    }
    else {
        // If 'spacesPerLevel' is negative, just put one space between fields
        // and suppress newlines when formatting each field.

        nestedLevel = 1;
        nestedSpacesPerLevel = -1;
    }

    bdlb::Print::indent(stream, nestedLevel, nestedSpacesPerLevel);

    // Output element type name followed by value.

    const Descriptor *descriptorPtr = selectionDescriptor(selector());
    if (d_selector < 0) {
        stream << "VOID";
    }
    else if (static_cast<unsigned>(descriptorPtr->d_elemEnum)
                      < static_cast<unsigned>(ElemType::BDEM_NUM_TYPES)) {
        ElemType::Type elemTypeEnum =
                   static_cast<ElemType::Type>(descriptorPtr->d_elemEnum);
        stream << elemTypeEnum;
    }
    else {
        // Out-of-range.  Print type as integer.

        stream << descriptorPtr->d_elemEnum;
    }

    stream << ' ';

    if (isSelectionNull()) {
        stream << "NULL";
        if (0 <= spacesPerLevel) {
            stream << "\n";
        }
    }
    else {
        descriptorPtr->print(selectionPointer(),
                             stream,
                             -nestedLevel,
                             nestedSpacesPerLevel);
    }

    if (0 <= spacesPerLevel) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
        stream << "}\n";
    }
    else {
        stream << " }";
    }

    return stream << bsl::flush;
}
}  // close package namespace

// FREE OPERATORS
bool bdlmxxx::operator==(const ChoiceHeader& lhs, const ChoiceHeader& rhs)
{
    return lhs.selector() == rhs.selector()
        && *lhs.catalog() == *rhs.catalog()
        && lhs.flags()    == rhs.flags()
        && lhs.selectionDescriptor(lhs.selector())->areEqual(
                                                       lhs.selectionPointer(),
                                                       rhs.selectionPointer());
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
