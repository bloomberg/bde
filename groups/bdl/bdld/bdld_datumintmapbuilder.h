// bdld_datumintmapbuilder.h                                          -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMINTMAPBUILDER
#define INCLUDED_BDLD_DATUMINTMAPBUILDER

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a utility to build a 'Datum' object holding an int-map.
//
//@CLASSES:
//  bdld::DatumIntMapBuilder: utility to build 'Datum' objects holding int-maps
//
//@SEE_ALSO: bdld_datum
//
//@DESCRIPTION: This component defines a mechanism, 'bdld::DatumIntMapBuilder',
// used to populate a 'Datum' int-map value in an exception-safe manner.  In
// addition to providing exception safety, a 'DatumIntMapBuilder' is
// particularly useful when the size of the int-map to be constructed is not
// known in advance.  The user can append elements to the datum int-map as
// needed, and when there are no more elements to append the user calls
// 'commit' or 'sortAndCommit' and ownership of the populated 'Datum' object is
// transferred to the caller.  After calling 'commit' or 'sortAndCommit', no
// additional elements can be appended to the 'Datum' int-map value.  Note that
// 'sortAndCommit' method will sort the populated int-map (by keys) and tag the
// resulting 'Datum' int-map value as sorted.  Also note that the user can
// insert elements in a (ascending) sorted order and tag the int-map as sorted.
// The behavior is undefined if unsorted int-map is tagged sorted.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// Suppose we need a data map for some UX data.  The keys of the map are 32-bit
// integers; and the values in that map can be different types.  The following
// code illustrates how to use 'bdld::DatumIntMapBuilder' to create such map
// easily.
//
// First, we need data to fill our int-map:
//..
//  bslma::TestAllocator ta("test", veryVeryVerbose);
//
//  DatumIntMapEntry formData[] = {
//      DatumIntMapEntry(1, Datum::createStringRef("Bart", &ta)),
//      DatumIntMapEntry(2, Datum::createStringRef("Simpson", &ta)),
//      DatumIntMapEntry(3, Datum::createStringRef("male", &ta)),
//      DatumIntMapEntry(4, Datum::createInteger(10))
//  };
//
//  const size_t DATA_SIZE  = sizeof(formData) / sizeof(DatumIntMapEntry);
//..
// Next, we create an object of 'DatumIntMapBuilder' class with initial
// capacity sufficient for storing all our data:
//..
//  DatumIntMapBuilder builder(DATA_SIZE, &ta);
//..
// Then, we load our builder with these data:
//..
//  for (size_t i = 0; i < DATA_SIZE; ++i) {
//      builder.pushBack(formData[i].key(), formData[i].value());
//  }
//..
// Next, we adopt the int-map, held by our builder, by newly created 'Datum'
// object:
//..
//  Datum form = builder.commit();
//..
// Now, we can check that all data have been correctly added to the int-map at
// the required order:
//..
//  assert(true == form.isIntMap());
//  assert(DATA_SIZE == form.theIntMap().size());
//
//  assert(1           == form.theIntMap()[0].key());
//  assert(true        == form.theIntMap()[0].value().isString());
//  assert("Bart"      == form.theIntMap()[0].value().theString());
//
//  assert(2           == form.theIntMap()[1].key());
//  assert(true        == form.theIntMap()[1].value().isString());
//  assert("Simpson"   == form.theIntMap()[1].value().theString());
//
//  assert(3           == form.theIntMap()[2].key());
//  assert(true        == form.theIntMap()[2].value().isString());
//  assert("male"      == form.theIntMap()[2].value().theString());
//
//  assert(4           == form.theIntMap()[3].key());
//  assert(true        == form.theIntMap()[3].value().isInteger());
//  assert(10          == form.theIntMap()[3].value().theInteger());
//..
// Finally, we destroy the 'Datum' object to release all allocated memory
// correctly:
//..
//  Datum::destroy(form, &ta);
//  assert(0 == ta.numBytesInUse());
//..

#include <bdlscm_version.h>

#include <bdld_datum.h>

#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_types.h>

namespace BloombergLP {
namespace bdld {

                          // ========================
                          // class DatumIntMapBuilder
                          // ========================

class DatumIntMapBuilder {
    // This 'class' provides a mechanism to build a 'Datum' object having an
    // int-map value in an exception-safe manner.  See 'bdld_datum' for more
    // information about integer maps.

  public:
    // TYPES
    typedef bsls::Types::size_type SizeType;
        // 'SizeType' is an alias for a unsigned integral value, representing
        // the capacity or size of a datum int-map.

    typedef bsl::allocator<char> allocator_type;

  private:
    // DATA
    DatumMutableIntMapRef d_mapping;   // mutable access to the datum int-map

    SizeType              d_capacity;  // capacity of the datum int-map

    bool                  d_sorted;    // underlying int-map is sorted

    allocator_type        d_allocator; // allocator for memory

  private:
    // NOT IMPLEMENTED
    DatumIntMapBuilder(const DatumIntMapBuilder&);
    DatumIntMapBuilder& operator=(const DatumIntMapBuilder&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumIntMapBuilder,
                                   bslma::UsesBslmaAllocator);
        // 'DatumIntMapBuilder' is allocator-aware.

    // CREATORS
    DatumIntMapBuilder();
    explicit DatumIntMapBuilder(const allocator_type& allocator);
    explicit DatumIntMapBuilder(
                           SizeType              initialCapacity,
                           const allocator_type& allocator = allocator_type());
        // Create a 'DatumIntMapBuilder' object that will administer the
        // process of building a 'Datum' int-map.  Optionally specify an
        // 'initialCapacity' for the int-map.  If 'initialCapacity' is not
        // supplied, the initial capacity of the int-map is 0.  Optionally
        // specify an 'allocator' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory; otherwise, the default allocator is used.

    ~DatumIntMapBuilder();
        // Destroy this object.  If this object is holding a datum int-map that
        // has not been adopted, then the datum int-map is disposed after
        // destroying each of its elements.

    // MANIPULATORS
    void append(const DatumIntMapEntry *entries, SizeType size);
        // Append the specified array 'entries' having the specified 'size' to
        // the 'Datum' int-map being build by this object.  The behavior is
        // undefined unless '0 != entries && 0 != size' and each element in
        // 'entries' that needs dynamic memory is allocated with the same
        // allocator that was used to construct this object.  The behavior is
        // undefined if 'commit' or 'sortAndCommit' has already been called on
        // this object.

    Datum commit();
        // Return a 'Datum' int-map value holding the elements supplied to
        // 'pushBack' or 'append'.  The caller is responsible for releasing the
        // resources of the returned 'Datum' object.  Calling this method
        // indicates that the caller is finished building the 'Datum' int-map
        // and no further values shall be appended.  The behavior is undefined
        // if any method of this object, other than its destructor, is called
        // after 'commit' invocation.

    void pushBack(int key, const Datum& value);
        // Append the entry with the specified 'key' and the specified 'value'
        // to the 'Datum' int-map being build by this object.  The behavior is
        // undefined if 'value' needs dynamic memory and was allocated using a
        // different allocator than the one used to construct this object.  The
        // behavior is also undefined if 'commit' or 'sortAndCommit' has
        // already been called on this object.

    void setSorted(bool value);
        // Mark the Datum int-map being built by this object as sorted if the
        // specified 'value' is 'true' and mark it unsorted otherwise.  This
        // function does not sort the int-map entries, or mark them to be
        // sorted later; the function should be used to indicate if the entries
        // are being appended in sorted order.  The behavior is undefined if
        // 'commit' or 'sortAndCommit' has already been called on this object.
        // The behavior is also undefined if the int-map being constructed is
        // marked sorted, but the entries are not appended in sorted order.
        // Note also that the int-map being constructed is marked unsorted by
        // default.

    Datum sortAndCommit();
        // Return a 'Datum' int-map value holding the elements supplied to
        // 'pushBack' or 'append' sorted by their keys.  The caller is
        // responsible for releasing the resources of the returned 'Datum'
        // object.  Calling this method indicates that the caller is finished
        // building the 'Datum' int-map and no further values shall be
        // appended.  The behavior is undefined if any method of this object,
        // other than its destructor, is called after 'sortAndCommit'
        // invocation.

    // ACCESSORS
    SizeType capacity() const;
        // Return the capacity of the held 'Datum' int-map.  The behavior is
        // undefined if 'commit' or 'sortAndCommit' has already been called on
        // this object.  Note that similar to the capacity of a 'vector', the
        // returned capacity has no bearing on the value of the 'Datum' being
        // constructed, but does indicate at which point additional memory will
        // be required to grow the 'Datum' int-map being built.

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.

    SizeType size() const;
        // Return the size of the held 'Datum' int-map.  The behavior is
        // undefined if 'commit' or 'sortAndCommit' has already been called on
        // this object.
};

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class DatumIntMapBuilder
                          // ------------------------

// ACCESSORS
inline
DatumIntMapBuilder::SizeType DatumIntMapBuilder::capacity() const
{
    return d_capacity;
}

inline
DatumIntMapBuilder::allocator_type DatumIntMapBuilder::get_allocator() const
{
    return d_allocator;
}

inline
DatumIntMapBuilder::SizeType DatumIntMapBuilder::size() const
{
    if (d_capacity) {
        return *d_mapping.size();                                     // RETURN
    }
    return 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
