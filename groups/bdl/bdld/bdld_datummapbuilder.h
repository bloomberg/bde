// bdld_datummapbuilder.h                                             -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMMAPBUILDER
#define INCLUDED_BDLD_DATUMMAPBUILDER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a utility to build a 'Datum' object holding a map.
//
//@CLASSES:
//  bdld::DatumMapBuilder: utility to build a 'Datum' object holding a map
//
//@SEE ALSO: bdld_datum, bdld_datummapowningkeysbuilder
//
//@DESCRIPTION: This component defines a mechanism, 'bdld::DatumMapBuilder',
// used to populate a 'Datum' map value in an exception-safe manner.  In
// addition to providing exception safety, a 'DatumMapBuilder' is particularly
// useful when the size of the map to be constructed is not known in advance.
// The user can append elements to the datum map as needed, and when there are
// no more elements to append the user calls 'commit' or 'sortAndCommit' and
// ownership of the populated 'Datum' object is transferred to the caller.
// After calling 'commit' or 'sortAndCommit', no additional elements can be
// appended to the 'Datum' map value.  Note that 'sortAndCommit' method will
// sort the populated map (by keys) and tag the resulting 'Datum' map value as
// sorted.  Also note that the user can insert elements in a (ascending) sorted
// order and tag the map as sorted.  The behaviour is undefined if unsorted map
// is tagged sorted.
//
// The only difference between this component and
// 'bdld_datummapowningkeysbuilder' is that this component does not make a copy
// of the map entries keys and the resulting 'Datum' object does not own memory
// for the map entries keys.
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
//      DatumMapEntry(StringRef("gender"),
//                    Datum::createStringRef("male", &ta)),
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
//  assert("gender"    == bart.theMap()[2].key());
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
    // This 'class' provides a mechanism to build a 'Datum' object having a map
    // value in an exception-safe manner.

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
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumMapBuilder, bslma::UsesBslmaAllocator);
        // 'DatumMapBuilder' objects use 'bslma::Allocator'.

    // CREATORS
    explicit DatumMapBuilder(bslma::Allocator *basicAllocator  = 0);
    explicit DatumMapBuilder(SizeType          initialCapacity,
                             bslma::Allocator *basicAllocator  = 0);
        // Create a 'DatumMapBuilder' object that will administer the process
        // of building a 'Datum' map.  Optionally specify an 'initialCapacity'
        // for the map.  If 'initialCapacity' is not supplied, the initial
        // capacity of the map is 0.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~DatumMapBuilder();
        // Destroy this object. If this object is holding a datum map that has
        // not been adopted, then the datum map is disposed after destroying
        // each of its elements.

    // MANIPULATORS
    void append(const DatumMapEntry *entries, SizeType size);
        // Append the specified array 'entries' having the specified 'size' to
        // the 'Datum' map being build by this object.  The behavior is
        // undefined unless and '0 != entries && 0 != size'  and each element
        // in 'entries' that needs dynamic memory, is allocated with the same
        // allocator that was used to construct this object.  The behavior is
        // undefined if 'commit' or 'sortAndCommit' has already been called on
        // this object.

    Datum commit();
        // Return a 'Datum' map value holding the elements supplied to
        // 'pushBack' or 'append'.  The caller is responsible for releasing the
        // resources of the returned 'Datum' object.  Calling this method
        // indicates that the caller is finished building the 'Datum' map and
        // no further values shall be appended.  The behavior is undefined if
        // any method of this object, other than its destructor, is called
        // after 'commit' invocation.

    void pushBack(const bslstl::StringRef& key, const Datum& value);
        // Append the entry with the specified 'key' and the specified 'value'
        // to the 'Datum' map being build by this object.  The behavior is
        // undefined if 'value' needs dynamic memory and was allocated using a
        // different allocator than the one used to construct this object.  The
        // behavior is also undefined if 'commit' or 'sortAndCommit' has
        // already been called on this object.

    void setSorted(bool value);
        // Mark the Datum map being built by this object as sorted if the
        // specified 'value' is 'true' and mark it unsorted otherwise.  This
        // function does not sort the map entries, or mark them to be sorted
        // later; the function should be used to indicate if the entries are
        // being appended in sorted order.  The behavior is undefined if
        // 'commit' or 'sortAndCommit' has already been called on this object.
        // The behavior is also undefined if the map being constructed is
        // marked sorted, but the entries are not appended in sorted order.
        // Note also that the map being constructed is marked unsorted by
        // default.

    Datum sortAndCommit();
        // Return a 'Datum' map value holding the elements supplied to
        // 'pushBack' or 'append' sorted by their keys.  The caller is
        // responsible for releasing the resources of the returned 'Datum'
        // object.  Calling this method indicates that the caller is finished
        // building the 'Datum' map and no further values shall be appended.
        // The behavior is undefined if any method of this object, other than
        // its destructor, is called after 'sortAndCommit' invocation.

    // ACCESSORS
    SizeType capacity() const;
        // Return the capacity of the held 'Datum' map.  The behavior is
        // undefined if 'commit' or 'sortAndCommit' has already been called on
        // this object.  Note that similar to the capacity of a 'vector', the
        // returned capacity has no bearing on the value of the 'Datum' being
        // constructed, but does indicate at which point additional memory will
        // be required to grow the 'Datum' map being built.

    SizeType size() const;
        // Return the size of the held 'Datum' map.  The behavior is undefined
        // if 'commit' or 'sortAndCommit' has already been called on this
        // object.
};

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                      // ---------------------
                      // class DatumMapBuilder
                      // ---------------------

// ACCESSORS
inline
DatumMapBuilder::SizeType DatumMapBuilder::capacity() const
{
    return d_capacity;
}

inline
DatumMapBuilder::SizeType DatumMapBuilder::size() const
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
