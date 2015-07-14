// bdlmxxx_rowlayout.cpp                                                 -*-C++-*-
#include <bdlmxxx_rowlayout.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_rowlayout_cpp,"$Id$ $CSID$")

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>

#include <bdlb_print.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace bdlmxxx {
                        // --------------------
                        // class RowLayout
                        // --------------------

// CREATORS
RowLayout::RowLayout(const InitialCapacity&  numElements,
                               bslma::Allocator       *basicAllocator)
: d_entries(basicAllocator)
, d_totalOffset(0)
{
    BSLS_ASSERT(0 <= numElements.d_i);

    d_entries.reserve(numElements.d_i);
}

RowLayout::RowLayout(const ElemType::Type     elementTypes[],
                               int                           numElements,
                               const Descriptor *const  attributesTable[],
                               bslma::Allocator             *basicAllocator)
: d_entries(basicAllocator)
, d_totalOffset(0)
{
    BSLS_ASSERT(0 <= numElements);

    d_entries.reserve(numElements);
    resetElemTypes(elementTypes, numElements, attributesTable);

    // Note that 'd_offset's and 'd_totalOffset' are set by 'compact'.
}

RowLayout::RowLayout(const RowLayout&  srcRowLayout,
                               int                    srcIndex,
                               int                    numElements,
                               bslma::Allocator      *basicAllocator)
: d_entries(basicAllocator)
, d_totalOffset(srcRowLayout.d_totalOffset)
{
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcRowLayout.length());

    d_entries.reserve(numElements);
    d_entries.insert(d_entries.end(),
                     srcRowLayout.d_entries.begin() + srcIndex,
                     srcRowLayout.d_entries.begin() + srcIndex + numElements);
}

// MANIPULATORS
RowLayout& RowLayout::operator=(const RowLayout& rhs)
{
    if (&rhs != this) {
        d_entries.reserve(rhs.d_entries.size());
        d_entries     = rhs.d_entries;
        d_totalOffset = rhs.d_totalOffset;
    }

    return *this;
}

void RowLayout::append(const Descriptor *elementAttributes)
{
    BSLS_ASSERT(elementAttributes);

    // Calculate the offset for this element.

    const int offset = insertionOffset(elementAttributes);

    d_entries.push_back(RowLayoutEntry(elementAttributes, offset));

    d_totalOffset = offset + elementAttributes->d_size;
}

void RowLayout::append(const ElemType::Type    elementTypes[],
                            int                          numElements,
                            const Descriptor *const attributesTable[])
{
    BSLS_ASSERT(0 <= numElements);

    const int dstLength   = length();
    const int totalLength = numElements + dstLength;

    d_entries.reserve(totalLength);
    for (int i = 0; i < numElements; ++i) {
        const Descriptor *desc = attributesTable[elementTypes[i]];
        const int offset = insertionOffset(desc);
        d_entries.push_back(RowLayoutEntry(desc, offset));
        d_totalOffset = offset + desc->d_size;
    }
}

void RowLayout::compact()
{
    enum {
        BDEM_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
    };

    const int len = length();

    int listHeads[BDEM_MAX_ALIGN + 1];

    // Create a set of linked lists of elements with identical alignment
    // requirements.  At the end of this loop, 'listHeads[m]' will contain the
    // index within 'd_entries' of the last element with alignment 'm', and
    // 'd_entries[n].offset()' will contain the index of the previous element
    // with the same alignment as element 'n'.

    for (int align = BDEM_MAX_ALIGN; align > 0; align /= 2) {
        listHeads[align] = -1;
    }

    for (int i = 0; i < len; ++i) {
        const int align = d_entries[i].attributes()->d_alignment;
        d_entries[i].offset() = listHeads[align];  // 'offset' temporarily
                                                   // changes its meaning
        listHeads[align] = i;
    }

    // Now iterate through the elements, starting with the most-strictly
    // aligned members and compute new offsets for each one.  Since all
    // alignments are powers of 2, we need to iterate through only the powers
    // of 2 in the 'listHeads' array.

    d_totalOffset = 0;
    for (int align = BDEM_MAX_ALIGN; align > 0; align /= 2) {
        // The items are in reverse order in the list; reverse them so that
        // in the end the offsets of items of a given alignment requirement
        // will be sorted in the same order as their 'listHeads' in the
        // row layout.

        int newHead = -1;
        int next;
        for (int j = listHeads[align]; -1 != j; j = next) {
            next = d_entries[j].offset();
            d_entries[j].offset() = newHead;
            newHead = j;
        }
        listHeads[align] = newHead;

        // Iterate through the linked list of elements with alignment 'align'.

        const int alignMinus1 = align - 1;
        for (int j = listHeads[align]; -1 != j; j = next) {
            next = d_entries[j].offset();
            d_entries[j].offset() = d_totalOffset;  // restored to original
                                                    // meaning
            d_totalOffset += d_entries[j].attributes()->d_size;

            BSLS_ASSERT(0 == (d_totalOffset & alignMinus1));
        }
    }
}

void RowLayout::insert(int                    dstIndex,
                            const Descriptor *elementAttributes)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex <= static_cast<int>(d_entries.size()));
    BSLS_ASSERT(elementAttributes);

    // Calculate the offset for this element.

    const int offset = insertionOffset(elementAttributes);

    d_entries.insert(d_entries.begin() + dstIndex,
                     RowLayoutEntry(elementAttributes, offset));

    d_totalOffset = offset + elementAttributes->d_size;
}

void RowLayout::insert(int                   dstIndex,
                            const RowLayout& srcRowLayout,
                            int                   srcIndex,
                            int                   numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex <= static_cast<int>(d_entries.size()));
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcRowLayout.length());

    if (0 == numElements) {
        return;
    }

    const int dstLength   = length();
    const int totalLength = numElements + dstLength;

    RowLayoutEntry dummy(srcRowLayout[0].attributes(), 0);

    d_entries.resize(totalLength, dummy);
    bsl::copy_backward(d_entries.begin() + dstIndex,
                       d_entries.begin() + dstLength,
                       d_entries.end());

    for (int i = 0; i < numElements; ++i) {
        const Descriptor *desc = srcRowLayout[srcIndex + i].attributes();

        // Calculate the appropriate offset for the next element.

        const int offset = insertionOffset(desc);

        d_entries[dstIndex + i] = RowLayoutEntry(desc, offset);
        d_totalOffset           = offset + desc->d_size;
    }
}

void RowLayout::insert(int                          dstIndex,
                            const ElemType::Type    elementTypes[],
                            int                          numElements,
                            const Descriptor *const attributesTable[])
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex <= static_cast<int>(d_entries.size()));
    BSLS_ASSERT(0 <= numElements);

    if (0 == numElements) {
        return;
    }

    const int dstLength   = length();
    const int dstEndIndex = dstIndex + numElements;
    const int totalLength = numElements + dstLength;

    RowLayoutEntry dummy(attributesTable[elementTypes[0]], 0);

    d_entries.resize(totalLength, dummy);
    bsl::copy_backward(d_entries.begin() + dstIndex,
                       d_entries.begin() + dstLength,
                       d_entries.end());

    for (int i = dstIndex; i < dstEndIndex; ++i) {
        // Calculate the appropriate offset for the next element.

        const Descriptor *desc =
                                   attributesTable[elementTypes[i - dstIndex]];
        const int offset = insertionOffset(desc);

        d_entries[i]  = RowLayoutEntry(desc, offset);
        d_totalOffset = offset + desc->d_size;
    }
}

void RowLayout::replace(int                    dstIndex,
                             const Descriptor *elementAttributes)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(dstIndex < static_cast<int>(d_entries.size()));
    BSLS_ASSERT(elementAttributes);

    // Calculate the offset for this element.

    const int offset = insertionOffset(elementAttributes);

    d_entries[dstIndex] = RowLayoutEntry(elementAttributes, offset);
    d_totalOffset       = offset + elementAttributes->d_size;
}

void RowLayout::replace(int                          dstIndex,
                             const ElemType::Type    elementTypes[],
                             int                          numElements,
                             const Descriptor *const attributesTable[])
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(dstIndex + numElements <= static_cast<int>(d_entries.size()));

    for (int i = dstIndex; i < dstIndex + numElements; ++i) {
        // Calculate the appropriate offset for the next element.

        const Descriptor *desc =
                                   attributesTable[elementTypes[i - dstIndex]];
        const int offset = insertionOffset(desc);

        d_entries[i]  = RowLayoutEntry(desc, offset);
        d_totalOffset = offset + desc->d_size;
    }
}

void RowLayout::replace(
                        int                                        dstIndex,
                        const bsl::vector<const Descriptor*>& srcArray,
                        int                                        srcIndex,
                        int                                        numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(dstIndex + numElements <= static_cast<int>(d_entries.size()));
    BSLS_ASSERT(srcIndex + numElements <= static_cast<int>(srcArray.size()));

    for (int i = dstIndex; i < dstIndex + numElements; ++i) {
        // Calculate the appropriate offset for the next element.

        const Descriptor *desc = srcArray[i - dstIndex + srcIndex];
        const int offset = insertionOffset(desc);

        d_entries[i]  = RowLayoutEntry(desc, offset);
        d_totalOffset = offset + desc->d_size;
    }
}

void RowLayout::resetElemTypes(
                                const ElemType::Type    elementTypes[],
                                int                          numElements,
                                const Descriptor *const attributesTable[])
{
    BSLS_ASSERT(0 <= numElements);

    d_entries.clear();
    d_entries.reserve(numElements);
    d_totalOffset = 0;

    for (int i = 0; i < numElements; ++i) {
        // Calculate the appropriate offset for the next element.

        const Descriptor *desc = attributesTable[elementTypes[i]];
        const int offset = insertionOffset(desc);

        d_entries.push_back(RowLayoutEntry(desc, offset));
        d_totalOffset = offset + desc->d_size;
    }

    compact();  // 'd_offset's and 'd_totalOffset' are set by 'compact'.
}

// ACCESSORS
int RowLayout::adjustedTotalOffset(
                          const ElemType::Type    elementTypes[],
                          int                          numElements,
                          const Descriptor *const attributesTable[]) const
{
    BSLS_ASSERT(0 <= numElements);

    int totalOffset = d_totalOffset;

    for (int i = 0; i < numElements; ++i) {
        // Calculate the appropriate offset for the next element.

        const Descriptor *desc = attributesTable[elementTypes[i]];
        const int alignMinus1 = desc->d_alignment - 1;
        const int offset      = (totalOffset + alignMinus1) & ~alignMinus1;

        totalOffset = offset + desc->d_size;
    }

    return totalOffset;
}

int RowLayout::adjustedTotalOffset(
                                       const RowLayout& srcRowLayout,
                                       int                   srcIndex,
                                       int                   numElements) const
{
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcRowLayout.length());

    int totalOffset = d_totalOffset;
    for (int i = 0; i < numElements; ++i) {
        // Calculate the appropriate offset for the next element.

        const Descriptor *desc =
                             srcRowLayout.d_entries[srcIndex + i].attributes();
        const int alignMinus1 = desc->d_alignment - 1;
        const int offset      = (totalOffset + alignMinus1) & ~alignMinus1;

        totalOffset = offset + desc->d_size;
    }

    return totalOffset;
}

bool RowLayout::areElemTypesEqual(const RowLayout& other) const
{
    const int len = length();

    if (len != other.length()) {
        return false;                                                 // RETURN
    }

    for (int i = 0; i < len; ++i) {
        if (d_entries[i].attributes()->d_elemEnum !=
                                 other.d_entries[i].attributes()->d_elemEnum) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bsl::ostream& RowLayout::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    char separator = '\n';
    if (spacesPerLevel < 0) {
        separator = ' ';
        spacesPerLevel = 0;
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << '{' << separator;

    if (level < 0) {
        level = -level;
    }

    const int levelPlus1 = level + 1;
    const int len        = length();

    for (int i = 0; i < len; ++i) {
        bdlb::Print::indent(stream, levelPlus1, spacesPerLevel);
        ElemType::Type elemEnum =
                    (ElemType::Type)d_entries[i].attributes()->d_elemEnum;
        stream << elemEnum << ' ' << d_entries[i].offset() << separator;
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << '}';
    if ('\n' == separator) {
        stream << '\n';
    }
    return stream;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
