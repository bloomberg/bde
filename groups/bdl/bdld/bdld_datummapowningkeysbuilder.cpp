// bdld_datummapowningkeysbuilder.cpp                                 -*-C++-*-
#include <bdld_datummapowningkeysbuilder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdld_datummapowningkeysbuilder.cpp,"$Id$ $CSID$")

#include <bdld_datum.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsl_algorithm.h>
#include <bsl_memory.h>
#include <bsl_cstring.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace bdld {

namespace {

static DatumMapOwningKeysBuilder::SizeType getNewCapacity(
                                  DatumMapOwningKeysBuilder::SizeType capacity,
                                  DatumMapOwningKeysBuilder::SizeType size)
    // Calculate the new capacity needed to accommodate data/keys having the
    // specified 'size' for the datum-key-owning map having the specified
    // 'capacity' as its capacity/'keys-capacity'.
{
    // Maximum allowed size (theoretical limit)
    static const DatumMapOwningKeysBuilder::SizeType MAX_BYTES =
               bsl::numeric_limits<DatumMapOwningKeysBuilder::SizeType>::max();

    BSLS_ASSERT(size < MAX_BYTES/2);
    (void)MAX_BYTES;

    capacity = capacity ? capacity : 1;
    while (capacity < size) {
        capacity *= 2;
    }

    // Verify capacity at outer size limits.
    BSLS_ASSERT(capacity >= size);

    return capacity;
}

static void createMapStorage(
                           DatumMutableMapOwningKeysRef        *mapping,
                           DatumMapOwningKeysBuilder::SizeType  capacity,
                           DatumMapOwningKeysBuilder::SizeType  keysCapacity,
                           bslma::Allocator                    *basicAllocator)
    // Load the specified 'mapping' with a reference to newly created
    // datum-key-owning map having the specified 'capacity' and 'keysCapacity'
    // using the specified 'basicAllocator'.
{
    Datum::createUninitializedMap(mapping,
                                  capacity,
                                  keysCapacity,
                                  basicAllocator);
    // Initialize the memory.

    bsl::uninitialized_fill_n(mapping->data(), capacity, DatumMapEntry());
    bsl::uninitialized_fill_n(mapping->keys(), keysCapacity, char());
}

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
static bool compareGreater(const DatumMapEntry& lhs, const DatumMapEntry& rhs)
    // Return 'true' if key in the specified 'lhs' is greater than key in the
    // specified 'rhs' and 'false' otherwise.
{
    return lhs.key() > rhs.key();
}
#endif

static bool compareLess(const DatumMapEntry& lhs, const DatumMapEntry& rhs)
    // Return 'true' if key in the specified 'lhs' is less than key in the
    // specified 'rhs' and 'false' otherwise.
{
    return lhs.key() < rhs.key();
}

}  // close unnamed namespace

                       // -------------------------------
                       // class DatumMapOwningKeysBuilder
                       // -------------------------------
// TRAITS
BSLMF_ASSERT(bslma::UsesBslmaAllocator<DatumMapOwningKeysBuilder>::value);

// CREATORS
DatumMapOwningKeysBuilder::DatumMapOwningKeysBuilder(
                                              bslma::Allocator *basicAllocator)
: d_capacity(0)
, d_keysCapacity(0)
, d_sorted(false)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(basicAllocator);

    // Do not create a datum map.  Defer this to the first call to 'pushBack'
    // or 'append'.
}

DatumMapOwningKeysBuilder::DatumMapOwningKeysBuilder(
                                         SizeType          initialCapacity,
                                         SizeType          initialKeysCapacity,
                                         bslma::Allocator *basicAllocator)
: d_capacity(initialCapacity)
, d_keysCapacity(initialKeysCapacity)
, d_sorted(false)
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(0 != basicAllocator);

    // Do not create a datum map, if 'initialCapacity' and
    // 'initialKeysCapacity' are both 0.  Defer this to the first call to
    // 'pushBack' or 'append'.

    if (initialCapacity && initialKeysCapacity) {
        createMapStorage(&d_mapping,
                         d_capacity,
                         d_keysCapacity,
                         basicAllocator);
    }
}

DatumMapOwningKeysBuilder::~DatumMapOwningKeysBuilder()
{
    if (d_mapping.data()) {
        for (SizeType i = 0; i < *d_mapping.size(); ++i) {
            Datum::destroy(d_mapping.data()[i].value(), d_allocator_p);
        }
        Datum::disposeUninitializedMap(d_mapping, d_allocator_p);
    }
}

// MANIPULATORS
void DatumMapOwningKeysBuilder::append(const DatumMapEntry *entries,
                                       SizeType             size)
{
    BSLS_ASSERT(0 != entries && 0 != size);

    // Get the new map capacity.

    SizeType newCapacity = d_capacity ?
                         getNewCapacity(d_capacity, *d_mapping.size() + size) :
                         getNewCapacity(d_capacity, size);

    SizeType totalSizeOfNewKeys = 0;
    for (SizeType i = 0; i < size; ++i) {
        totalSizeOfNewKeys += entries[i].key().length();
    }


    SizeType totalSizeOfCurrentKeys = 0;
    if (0 != d_mapping.keys() && 0 != *d_mapping.size()) {
        const DatumMapEntry& lastElement =
                                       d_mapping.data()[*d_mapping.size() - 1];
        totalSizeOfCurrentKeys = lastElement.key().end() - d_mapping.keys();
    }

    // Get the new keys-capacity for the map.

    SizeType newKeysCapacity = d_keysCapacity ?
              getNewCapacity(d_keysCapacity,
                             totalSizeOfCurrentKeys + totalSizeOfNewKeys) :
              getNewCapacity(d_keysCapacity, totalSizeOfNewKeys);

    // If the initial capacity was zero, create an empty map with the new
    // capacities.

    if (!d_capacity && !d_keysCapacity) {
        d_capacity = newCapacity;
        d_keysCapacity = newKeysCapacity;
        createMapStorage(&d_mapping,
                         d_capacity,
                         d_keysCapacity,
                         d_allocator_p);
        *d_mapping.sorted() = d_sorted;
    }

    // Either one of the two capacities or both have to be increased.

    if (newCapacity != d_capacity || newKeysCapacity != d_keysCapacity) {
        // Capacity(s) has to be increased.

        d_capacity     = newCapacity;
        d_keysCapacity = newKeysCapacity;

        // Create a new map with the higher capacity(s).

        DatumMutableMapOwningKeysRef mapping;

        createMapStorage(&mapping, d_capacity, d_keysCapacity, d_allocator_p);

        // Copy the existing data and dispose the old map.  Copy all the keys
        // in a single operation.

        bsl::memcpy(mapping.keys(), d_mapping.keys(), totalSizeOfCurrentKeys);

        *mapping.size() = *d_mapping.size();

        char *keyBegin = mapping.keys();
        for (SizeType i = 0; i < *mapping.size(); ++i) {
            const int KEY_LENGTH =
                          static_cast<int>(d_mapping.data()[i].key().length());
            bslstl::StringRef key(keyBegin, KEY_LENGTH);
            const Datum       value = d_mapping.data()[i].value();

            mapping.data()[i] = DatumMapEntry(key, value);

            // Determine the position where the next key was inserted by
            // computing the size of the current key.

            keyBegin += key.length();
        }

        Datum::disposeUninitializedMap(d_mapping, d_allocator_p);
        d_mapping = mapping;
    }

    // Copy the new elements.

    char *nextKeyPos = d_mapping.keys() + totalSizeOfCurrentKeys;
    for (SizeType i = 0; i < size; ++i) {
        bsl::memcpy(nextKeyPos,
                    entries[i].key().data(),
                    entries[i].key().length());

        bslstl::StringRef newKey(nextKeyPos,
                                 static_cast<int>(entries[i].key().length()));
        d_mapping.data()[*d_mapping.size() + i] =
                                     DatumMapEntry(newKey, entries[i].value());
        nextKeyPos += entries[i].key().length();
    }
    *d_mapping.size() += size;
}

Datum DatumMapOwningKeysBuilder::commit()
{
    // Make sure the map is sorted.

    BSLS_ASSERT_SAFE(!d_capacity ||
                     !*d_mapping.sorted() ||
                     bsl::adjacent_find(d_mapping.data(),
                                        d_mapping.data() + *d_mapping.size(),
                                        compareGreater)
                         == d_mapping.data() + *d_mapping.size());

    Datum result   = Datum::adoptMap(d_mapping);
    d_mapping      = DatumMutableMapOwningKeysRef();
    d_capacity     = 0;
    d_keysCapacity = 0;
    return result;
}

void DatumMapOwningKeysBuilder::pushBack(const bslstl::StringRef& key,
                                         const Datum&             value)
{
    DatumMapEntry entry(key, value);
    append(&entry, 1);
}

void DatumMapOwningKeysBuilder::setSorted(bool value)
{
    if (d_mapping.data()) {
        *d_mapping.sorted() = value;
    }
    else {
        d_sorted = true;
    }
}

Datum DatumMapOwningKeysBuilder::sortAndCommit()
{
    if (d_mapping.data()) {
        bsl::sort(d_mapping.data(),
                  d_mapping.data() + *d_mapping.size(),
                  compareLess);
        setSorted(true);
    }
    return commit();
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
