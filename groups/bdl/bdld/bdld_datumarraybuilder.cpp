// bdld_datumarraybuilder.cpp                                         -*-C++-*-
#include <bdld_datumarraybuilder.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdld_datumarraybuilder_cpp,"$Id$ $CSID$")

#include <bdld_datum.h>
#include <bsls_assert.h>
#include <bsl_cstring.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace bdld {

namespace {

static bsls::Types::size_type getNewCapacity(bsls::Types::size_type capacity,
                                             bsls::Types::size_type length)
    // Calculate the new capacity needed to accommodate data having the
    // specified 'length' for the datum array having the specified 'capacity'.
{
    // Maximum allowed array length (theoretical limit)
    static const bsl::size_t MAX_BYTES        = ~bsl::size_t(0) / 2;
    static const bsl::size_t MAX_ARRAY_LENGTH = MAX_BYTES / sizeof(Datum);

    if (length >= MAX_ARRAY_LENGTH / 2) {
        capacity = MAX_ARRAY_LENGTH;
    }
    else {
        capacity += !capacity; // get to 1 from 0 (no op afterwards)
        while (capacity < length) { // get higher than 1
            capacity *= 2;
        }
    }

    // Verify capacity at outer size limits.
    BSLS_ASSERT(capacity >= length);

    return capacity;
}

static void createArrayStorage(DatumMutableArrayRef   *array,
                               bsls::Types::size_type  capacity,
                               bslma::Allocator       *basicAllocator)
    // Load the specified 'array' with a reference to newly created datum array
    // having the specified 'capacity', using the specified specified
    // 'basicAllocator'.
{
    Datum::createUninitializedArray(array, capacity, basicAllocator);
    // Initialize the memory.
    bsl::uninitialized_fill_n(array->data(), capacity, Datum());
}

}  // close unnamed namespace

                          // -----------------------
                          // class DatumArrayBuilder
                          // -----------------------

// CREATORS
DatumArrayBuilder::DatumArrayBuilder(SizeType          initialCapacity,
                                     bslma::Allocator *basicAllocator)
: d_capacity(initialCapacity)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    // If 'initialCapacity' is 0, defer allocation of the array storage until
    // the first call to 'pushBack' or 'append'.
    if (initialCapacity) {
        createArrayStorage(&d_array, d_capacity, d_allocator_p);
    }
}

DatumArrayBuilder::~DatumArrayBuilder()
{
    if (d_array.data()) {
        for (SizeType i = 0; i < *d_array.length(); ++i) {
            Datum::destroy(d_array.data()[i], d_allocator_p);
        }
        Datum::disposeUninitializedArray(d_array, d_allocator_p);
    }
}

// MANIPULATORS
void DatumArrayBuilder::append(const Datum *values, SizeType length)
{
    BSLS_ASSERT(0 == d_capacity || 0 != d_array.data());

    // Get the new array capacity.
    SizeType newCapacity = d_capacity ?
                       getNewCapacity(d_capacity, *d_array.length() + length) :
                       getNewCapacity(d_capacity, length);

    // If the initial capacity was zero, create an array with the new capacity.
    if (!d_capacity) {
        createArrayStorage(&d_array, newCapacity, d_allocator_p);
        d_capacity = newCapacity;
    }
    else if (d_capacity < newCapacity) {
        // Create a new array with the higher capacity.
        DatumMutableArrayRef array;
        createArrayStorage(&array, newCapacity, d_allocator_p);

        // Copy the existing data and dispose the old array.
        *array.length() = *d_array.length();
        bsl::memcpy(array.data(),
                    d_array.data(),
                    sizeof(Datum) * (*d_array.length()));
        Datum::disposeUninitializedArray(d_array, d_allocator_p);

        d_array = array;
        d_capacity = newCapacity;
    }

    // Copy the new elements.
    bsl::memcpy(d_array.data() + *d_array.length(),
                values,
                sizeof(Datum) * length);
    *d_array.length() += length;
}

Datum DatumArrayBuilder::commit()
{
    Datum result = Datum::adoptArray(d_array);
    d_array = DatumMutableArrayRef();
    d_capacity = 0;
    return result;
}

void DatumArrayBuilder::pushBack(const Datum& value)
{
    append(&value, 1);
}

// ACCESSORS
DatumArrayBuilder::SizeType DatumArrayBuilder::capacity() const
{
    return d_capacity;
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
