// bdld_datummapbuilder.cpp                                           -*-C++-*-
#include <bdld_datummapbuilder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdld_datummapbuilder.cpp, "$Id$ $CSID$")

#include <bdld_datum.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace bdld {

namespace {

typedef DatumMapBuilder::allocator_type allocator_type;

static DatumMapBuilder::SizeType getNewCapacity(
                                            DatumMapBuilder::SizeType capacity,
                                            DatumMapBuilder::SizeType size)
    // Calculate the new capacity needed to accommodate data having the
    // specified 'size' for the datum map having the specified 'capacity'.
{
    // Maximum allowed size (theoretical limit)
    static const DatumMapBuilder::SizeType MAX_BYTES =
                         bsl::numeric_limits<DatumMapBuilder::SizeType>::max();

    BSLS_ASSERT(size < MAX_BYTES/2);
    (void)MAX_BYTES;

    capacity = capacity ? capacity : 1;
    while (capacity < size ) {
        capacity *= 2;
    }

    // Verify capacity at outer size limits.
    BSLS_ASSERT(capacity >= size);

    return capacity;
}

static void createMapStorage(DatumMutableMapRef        *mapping,
                             DatumMapBuilder::SizeType  capacity,
                             const allocator_type&      allocator)
    // Load the specified 'mapping' with a reference to newly created datum map
    // having the specified 'capacity', using the specified 'allocator'.
{
    Datum::createUninitializedMap(mapping, capacity, allocator.mechanism());
    // Initialize the memory.

    bsl::uninitialized_fill_n(mapping->data(), capacity, DatumMapEntry());
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

                          // ---------------------
                          // class DatumMapBuilder
                          // ---------------------
// TRAITS
BSLMF_ASSERT(bslma::UsesBslmaAllocator<DatumMapBuilder>::value);

// CREATORS
DatumMapBuilder::DatumMapBuilder()
: d_capacity(0)
, d_sorted(false)
, d_allocator()
{
}

DatumMapBuilder::DatumMapBuilder(const allocator_type& allocator)
: d_capacity(0)
, d_sorted(false)
, d_allocator(allocator)
{
}

DatumMapBuilder::DatumMapBuilder(SizeType              initialCapacity,
                                 const allocator_type& allocator)
: d_capacity(initialCapacity)
, d_sorted(false)
, d_allocator(allocator)
{
    // Do not create a datum map, if 'initialCapacity' is 0.  Defer this to the
    // first call to 'pushBack' or 'append'.

    if (initialCapacity) {
        createMapStorage(&d_mapping, d_capacity, d_allocator);
    }
}

DatumMapBuilder::~DatumMapBuilder()
{
    if (d_mapping.data()) {
        for (SizeType i = 0; i < *d_mapping.size(); ++i) {
            Datum::destroy(d_mapping.data()[i].value(),
                           d_allocator.mechanism());
        }
        Datum::disposeUninitializedMap(d_mapping, d_allocator.mechanism());
    }
}

// MANIPULATORS
void DatumMapBuilder::append(const DatumMapEntry *entries, SizeType size)
{
    BSLS_ASSERT(0 != entries && 0 != size);

    // Get the new map capacity.

    SizeType newCapacity =
        d_capacity ?  getNewCapacity(d_capacity, *d_mapping.size() + size)
                   :  getNewCapacity(d_capacity, size);

    // If the initial capacity was zero, create an empty map with the new
    // capacity.

    if (!d_capacity) {
        d_capacity = newCapacity;
        createMapStorage(&d_mapping, d_capacity, d_allocator);
        *d_mapping.sorted() = d_sorted;
    }
    if (newCapacity != d_capacity) {
        // Capacity has to be increased.

        d_capacity = newCapacity;

        // Create a new map with the higher capacity.

        DatumMutableMapRef mapping;
        createMapStorage(&mapping, d_capacity, d_allocator);

        // Copy the existing data and dispose the old map.

        *mapping.size() = *d_mapping.size();
        bsl::memcpy((void *)mapping.data(),
                    d_mapping.data(),
                    sizeof(DatumMapEntry) * (*d_mapping.size()));
        Datum::disposeUninitializedMap(d_mapping, d_allocator.mechanism());
        d_mapping = mapping;
    }

    // Copy the new elements.

    bsl::memcpy((void *)(d_mapping.data() + *d_mapping.size()),
                entries,
                sizeof(DatumMapEntry) * size);
    *d_mapping.size() += size;
}

Datum DatumMapBuilder::commit()
{
    // Make sure the map is sorted.

    BSLS_ASSERT_SAFE(0 == d_capacity || false == *d_mapping.sorted() ||
                     bsl::adjacent_find(d_mapping.data(),
                                        d_mapping.data() + *d_mapping.size(),
                                        compareGreater) ==
                         d_mapping.data() + *d_mapping.size());

    Datum result = Datum::adoptMap(d_mapping);
    d_mapping    = DatumMutableMapRef();
    d_capacity   = 0;
    return result;
}

void DatumMapBuilder::pushBack(const bslstl::StringRef& key,
                               const Datum&             value)
{
    DatumMapEntry entry(key, value);
    append(&entry, 1);
}

void DatumMapBuilder::setSorted(bool value)
{
    if (d_mapping.data()) {
        *d_mapping.sorted() = value;
    }
    else {
        d_sorted = false;
    }
}

Datum DatumMapBuilder::sortAndCommit()
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
