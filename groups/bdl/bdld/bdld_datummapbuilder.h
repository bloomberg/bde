// bdld_datummapbuilder.h                                             -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMMAPBUILDER
#define INCLUDED_BDLD_DATUMMAPBUILDER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a utility to build a 'Datum' object holding map.
//
//@CLASSES:
//  bdld::DatumMapBuilder: utility to build map of 'Datum' objects
//
//@SEE ALSO: bdld_datum, bdld_datummapref
//
//@DESCRIPTION: This component provides a utility 'bdld::DatumMapBuilder' to
// build a 'Datum' object holding a map of 'Datum' objects that is keyed by
// string keys.  This 'class' is especially useful when the size of the map to
// be constructed is not known in advance.  The user can append elements to the
// datum map.  When the size of the datum map exceeds its capacity, the datum
// map grows.  The user can indicate that it does not have more elements to
// append (by calling 'commit'), and the datum map is then adopted into a
// 'Datum' object and returned to the user.  The user should not try to append
// any more elements to the datum map then.  The user can indicate that the
// elements need to be sorted (by keys) by calling 'sortAndCommit' and the
// elements will be sorted before the map is adopted into a 'Datum' object.
// The user can also insert elements in a sorted order and tag the map as
// sorted.  It is undefined behavior to tag the map as sorted unless all of the
// elements are added in ascending order.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// Suppose we need a map for some personal data.  And the values in that map
// can be different types.   The following code illustrates how to use
// 'bdld::DatumMapBuilder' to create such map easily.
//
// First, we need data to fill our map:
//..
//  bslma::TestAllocator ta("test", veryVeryVerbose);
//
//  DatumMapEntry bartData[] = {
//      DatumMapEntry(StringRef("firstName"),
//                    Datum:: createStringRef("Bart", &ta)),
//      DatumMapEntry(StringRef("lastName"),
//                    Datum:: createStringRef("Simpson", &ta)),
//      DatumMapEntry(StringRef("sex"), Datum::createStringRef("male", &ta)),
//      DatumMapEntry(StringRef("age"), Datum::createInteger(10))
//  };
//
//  const size_t DATA_SIZE  = sizeof(bartData) / sizeof(DatumMapEntry);
//..
// Next, we create an object of 'DatumMapBuilder' class with initial capacity
// sufficient for storing all our data:
//..
//  DatumMapBuilder builder(DATA_SIZE, &ta);
//..
// Then, we load our builder with these data:
//..
//  for (size_t i = 0; i < DATA_SIZE; ++i) {
//      builder.pushBack(bartData[i].key(), bartData[i].value());
//  }
//..
// Next, we adopt the map, held by our builder, by newly created 'Datum'
// object:
//..
//  Datum bart = builder.commit();
//..
// Now, we can check that all data have been correctly added to the map at the
// required order:
//..
//  assert(true == bart.isMap());
//  assert(DATA_SIZE == bart.theMap().size());
//
//  assert("firstName" == bart.theMap()[0].key());
//  assert(true        == bart.theMap()[0].value().isString());
//  assert("Bart"      == bart.theMap()[0].value().theString());
//
//  assert("lastName"  == bart.theMap()[1].key());
//  assert(true        == bart.theMap()[1].value().isString());
//  assert("Simpson"   == bart.theMap()[1].value().theString());
//
//  assert("sex"       == bart.theMap()[2].key());
//  assert(true        == bart.theMap()[2].value().isString());
//  assert("male"      == bart.theMap()[2].value().theString());
//
//  assert("age"       == bart.theMap()[3].key());
//  assert(true        == bart.theMap()[3].value().isInteger());
//  assert(10          == bart.theMap()[3].value().theInteger());
//..
// Finally, we destroy the 'Datum' object to release all allocated memory
// correctly:
//..
//  Datum::destroy(bart, &ta);
//  assert(0 == ta.numBytesInUse());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLD_DATUM
#include <bdld_datum.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

namespace bdld {

                           // =====================
                           // class DatumMapBuilder
                           // =====================

class DatumMapBuilder {
    // This 'class' provides a utility to build 'Datum' object holding a map of
    // 'Datum' objects.

  public:
    // TYPES
    typedef bsls::Types::size_type SizeType;
        // 'SizeType' is an alias for a unsigned integral value, representing
        // the capacity or size of a datum map.

  private:
    // DATA
    DatumMutableMapRef  d_mapping;      // mutable access to the datum map
    SizeType            d_capacity;     // capacity of the datum map
    bool                d_sorted;       // underlying map is sorted or not
    bslma::Allocator   *d_allocator_p;  // allocator for memory

  private:
    // NOT IMPLEMENTED
    DatumMapBuilder(const DatumMapBuilder&);
    DatumMapBuilder& operator=(const DatumMapBuilder&);

  public:
    // CREATORS
    explicit DatumMapBuilder(SizeType          initialCapacity = 0,
                             bslma::Allocator *basicAllocator  = 0);
        // Create a 'DatumMapBuilder' object having the optionally specified
        // 'initialCapacity' and optionally specified 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently-installed
        // default allocator is used.

    ~DatumMapBuilder();
        // Destroy this object. If this object is holding a datum map that has
        // not been adopted, then the datum map is disposed after destroying
        // each of its elements.

    // MANIPULATORS
    void pushBack(const bslstl::StringRef& key, const Datum& value);
        // Append the entry with the specified 'key' and the specified 'value'
        // to the end of the held datum map.  If the datum map is full, a new
        // datum map with larger capacity is allocated and any previous datum
        // map is disposed after copying its elements.  The behavior is
        // undefined if 'entry' needs dynamic memory and it was allocated using
        // a different allocator than the one used to construct this object.
        // The behavior is also undefined if 'commit' or 'sortAndCommit' has
        // already been called on this object.

    void append(const DatumMapEntry *entries, SizeType size);
        // Append the specified array 'entries' having the specified 'size' to
        // the end of the held datum map.  Note that if the datum map is full,
        // a new datum map with larger capacity is allocated and the previous
        // datum map is disposed after copying its elements.  The behavior is
        // undefined unless and '0 != entries && 0 != size'  and each element
        // in 'entries' that needs dynamic memory, is allocated with the same
        // allocator that was used to construct this object.  The behavior is
        // undefined if 'commit' or 'sortAndCommit' has already been called on
        // this object.

    Datum commit();
        // Return a 'Datum' object holding a map of 'Datum' objects built using
        // 'pushBack' or 'append'.  This method indicates that the caller is
        // finished building the datum map and no further values shall be
        // appended.  The behavior is undefined if any method of this object,
        // other than its destructor, is called after 'commit' invocation.

    void setSorted(bool value);
        // Indicate that the underlying map is sorted if the specified 'value'
        // is 'true' and unsorted otherwise.  The behavior is undefined if
        // 'commit' or 'sortAndCommit' has already been called on this object.
        // Note that the map is unsorted by default.

    Datum sortAndCommit();
        // Return a 'Datum' object holding a map of 'Datum' objects build using
        // 'pushBack' or 'append'.  Sort the elements of the map.  This method
        // indicates that the caller is finished building the datum map and no
        // further values shall be appended.  The behavior is undefined if any
        // method of this object, other than its destructor, is called after
        // 'sortAndCommit' invocation.

    // ACCESSORS
    SizeType capacity() const;
        // Return the capacity.  The behavior is undefined if 'commit' or
        // 'sortAndCommit' has already been called on this object.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumMapBuilder, bslma::UsesBslmaAllocator);
        // 'DatumMapBuilder' objects use 'bslma::Allocator'.
};

}  // close package namespace
}  // close enterprise namespace

#endif

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
