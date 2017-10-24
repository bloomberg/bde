// bdld_datumintmapbuilder.cpp                                        -*-C++-*-
#include <bdld_datumintmapbuilder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdld_datumintmapbuilder.cpp,"$Id$ $CSID$")

#include <bdld_datum.h>
#include <bslma_default.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace bdld {

namespace {

static DatumIntMapBuilder::SizeType getNewCapacity(
                                         DatumIntMapBuilder::SizeType capacity,
                                         DatumIntMapBuilder::SizeType size)
    // Calculate the new capacity needed to accommodate data having the
    // specified 'size' for the datum int-map having the specified 'capacity'.
{
    // Maximum allowed size (theoretical limit)
    static const DatumIntMapBuilder::SizeType k_MAX_BYTES =
                      bsl::numeric_limits<DatumIntMapBuilder::SizeType>::max();

    BSLS_ASSERT(size < k_MAX_BYTES / 2);
    (void)k_MAX_BYTES;

    capacity = capacity ? capacity : 1;
    while (capacity < size ) {
        capacity *= 2;
    }

    // Verify capacity at outer size limits.
    BSLS_ASSERT(capacity >= size);

    return capacity;
}

static void createMapStorage(DatumMutableIntMapRef        *mapping,
                             DatumIntMapBuilder::SizeType  capacity,
                             bslma::Allocator             *basicAllocator)
    // Load the specified 'mapping' with a reference to newly created datum
    // int-map having the specified 'capacity', using the specified
    // 'basicAllocator'.
{
    Datum::createUninitializedIntMap(mapping, capacity, basicAllocator);
    // Initialize the memory.

    bsl::uninitialized_fill_n(mapping->data(), capacity, DatumIntMapEntry());
}

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
static bool compareGreater(const DatumIntMapEntry& lhs,
                           const DatumIntMapEntry& rhs)
    // Return 'true' if key in the specified 'lhs' is greater than key in the
    // specified 'rhs' and 'false' otherwise.
{
    return lhs.key() > rhs.key();
}
#endif

static bool compareLess(const DatumIntMapEntry& lhs,
                        const DatumIntMapEntry& rhs)
    // Return 'true' if key in the specified 'lhs' is less than key in the
    // specified 'rhs' and 'false' otherwise.
{
    return lhs.key() < rhs.key();
}

}  // close unnamed namespace

                          // ------------------------
                          // class DatumIntMapBuilder
                          // ------------------------
// TRAITS
BSLMF_ASSERT(bslma::UsesBslmaAllocator<DatumIntMapBuilder>::value);

// CREATORS
DatumIntMapBuilder::DatumIntMapBuilder(bslma::Allocator *basicAllocator)
: d_capacity(0)
, d_sorted(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

DatumIntMapBuilder::DatumIntMapBuilder(SizeType          initialCapacity,
                                       bslma::Allocator *basicAllocator)
: d_capacity(initialCapacity)
, d_sorted(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // Do not create a datum int-map, if 'initialCapacity' is 0.  Defer this to
    // the first call to 'pushBack' or 'append'.

    if (initialCapacity) {
        createMapStorage(&d_mapping, d_capacity, d_allocator_p);
    }
}

DatumIntMapBuilder::~DatumIntMapBuilder()
{
    if (d_mapping.data()) {
        for (SizeType i = 0; i < *d_mapping.size(); ++i) {
            Datum::destroy(d_mapping.data()[i].value(), d_allocator_p);
        }
        Datum::disposeUninitializedIntMap(d_mapping, d_allocator_p);
    }
}

// MANIPULATORS
void DatumIntMapBuilder::append(const DatumIntMapEntry *entries,
                                SizeType                size)
{
    BSLS_ASSERT(0 != entries && 0 != size);

    // Get the new int-map capacity.

    SizeType newCapacity = d_capacity ?
                      getNewCapacity(d_capacity, *d_mapping.size() + size) :
                      getNewCapacity(d_capacity, size);

    // If the initial capacity was zero, create an empty int-map with the new
    // capacity.

    if (!d_capacity) {
        d_capacity = newCapacity;
        createMapStorage(&d_mapping, d_capacity, d_allocator_p);
        *d_mapping.sorted() = d_sorted;
    }
    if (newCapacity != d_capacity) {
        // Capacity has to be increased.

        d_capacity = newCapacity;

        // Create a new int-map with the higher capacity.

        DatumMutableIntMapRef mapping;
        createMapStorage(&mapping, d_capacity, d_allocator_p);

        // Copy the existing data and dispose the old int-map.

        *mapping.size() = *d_mapping.size();
        bsl::memcpy(mapping.data(),
                    d_mapping.data(),
                    sizeof(DatumIntMapEntry) * (*d_mapping.size()));
        Datum::disposeUninitializedIntMap(d_mapping, d_allocator_p);
        d_mapping = mapping;
    }

    // Copy the new elements.

    bsl::memcpy(d_mapping.data() + *d_mapping.size(),
                entries,
                sizeof(DatumIntMapEntry) * size);
    *d_mapping.size() += size;
}

Datum DatumIntMapBuilder::commit()
{
    // Make sure the int-map is sorted.

    BSLS_ASSERT_SAFE(0 == d_capacity ||
                     false == *d_mapping.sorted() ||
                     bsl::adjacent_find(d_mapping.data(),
                                        d_mapping.data() + *d_mapping.size(),
                                        compareGreater)
                         == d_mapping.data() + *d_mapping.size());

    Datum result = Datum::adoptIntMap(d_mapping);
    d_mapping    = DatumMutableIntMapRef();
    d_capacity   = 0;
    return result;
}

void DatumIntMapBuilder::pushBack(int key, const Datum& value)
{
    DatumIntMapEntry entry(key, value);
    append(&entry, 1);
}

void DatumIntMapBuilder::setSorted(bool value)
{
    if (d_mapping.data()) {
        *d_mapping.sorted() = value;
    }
    else {
        d_sorted = false;
    }
}

Datum DatumIntMapBuilder::sortAndCommit()
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
// Copyright 2017 Bloomberg Finance L.P.
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
