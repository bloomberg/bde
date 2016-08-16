// bdld_datummapowningkeysbuilder.h                                   -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMMAPOWNINGKEYSBUILDER
#define INCLUDED_BDLD_DATUMMAPOWNINGKEYSBUILDER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a utility to build a 'Datum' object holding a map.
//
//@CLASSES:
//  bdld::DatumMapOwningKeysBuilder: utility to build a 'Datum' map value
//
//@SEE ALSO: bdld_datum, bdld_datummapbuilder
//
//@DESCRIPTION: This component defines a mechanism,
// 'bdld::DatumMapOwningKeysBuilder', used to populate a 'Datum' map value in
// an exception-safe manner.  In addition to providing exception safety, a
// 'DatumMapOwningKeysBuilder' is particularly useful when the size of the map
// to be constructed is not known in advance.  The user can append elements to
// the datum map as needed, and when there are no more elements to append the
// user calls 'commit' or 'sortAndCommit' and ownership of the populated
// 'Datum' object is transferred to the caller.  After calling 'commit' or
// 'sortAndCommit', no additional elements can be appended to the 'Datum' map
// value.  Note that 'sortAndCommit' method will sort the populated map (by
// keys) and tag the resulting 'Datum' map value as sorted.  Also note that the
// user can insert elements in a (ascending) sorted order and tag the map as
// sorted.  The behaviour is undefined if unsorted map is tagged sorted.
//
// The only difference between this component and 'bdld_datummapbuilder' is
// that this component makes a copy of the map entries keys and the resulting
// 'Datum' object owns memory for the map entries keys.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// Suppose we need a map for some personal data.  The values in that map can be
// different types and keys must be protected from destruction as this map
// object can be passed out of scope.  The following code illustrates how to
// use 'bdld::DatumMapOwningKeysBuilder' to create such map easily.
//
// First, we need data to fill our map:
//..
//  bslma::TestAllocator ta("test", veryVeryVerbose);
//
//  bsl::string firstName = "firstName";
//  bsl::string lastName  = "lastName";
//  bsl::string gender    = "gender";
//  bsl::string age       = "age";
//
//  DatumMapEntry bartData[] = {
//      DatumMapEntry(firstName, Datum::createStringRef("Bart", &ta)),
//      DatumMapEntry(lastName,  Datum::createStringRef("Simpson", &ta)),
//      DatumMapEntry(gender,    Datum::createStringRef("male", &ta)),
//      DatumMapEntry(age,       Datum::createInteger(10))
//  };
//
//  const size_t DATA_SIZE  = sizeof(bartData) / sizeof(DatumMapEntry);
//  const size_t KEYS_SIZE  = firstName.length()
//                          + lastName.length()
//                          + gender.length()
//                          + age.length();
//..
// Next, we create an object of 'DatumMapOwningKeysBuilder' class with initial
// capacity sufficient for storing all our data:
//..
//  DatumMapOwningKeysBuilder builder(DATA_SIZE, KEYS_SIZE, &ta);
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

                      // ===============================
                      // class DatumMapOwningKeysBuilder
                      // ===============================

class DatumMapOwningKeysBuilder {
    // This 'class' provides a mechanism to build a 'Datum' object having a map
    // (owning keys) value in an exception-safe manner.

  public:
    // TYPES
    typedef Datum::SizeType SizeType;
        // 'SizeType' is an alias for a signed value, representing the
        // capacity, *keys-capacity*, size or *keys-size* of a datum-key-owning
        // map.

  private:
    // DATA
    DatumMutableMapOwningKeysRef  d_mapping;      // mutable access to the
                                                  // datum-key-owning map

    SizeType                      d_capacity;     // capacity of the
                                                  // datum-key-owning map

    SizeType                      d_keysCapacity; // keys-capacity of the
                                                  // datum-key-owning map (in
                                                  // bytes)

    bool                          d_sorted;       // underlying map is sorted
                                                  // or not

    bslma::Allocator             *d_allocator_p;  // pointer to the allocator

  private:
    // NOT IMPLEMENTED
    DatumMapOwningKeysBuilder(const DatumMapOwningKeysBuilder&);
    DatumMapOwningKeysBuilder& operator=(const DatumMapOwningKeysBuilder&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumMapOwningKeysBuilder,
                                   bslma::UsesBslmaAllocator);
        // 'DatumMapOwningKeysBuilder' objects use 'bslma::Allocator'.

    // CREATORS
    explicit DatumMapOwningKeysBuilder(bslma::Allocator *basicAllocator);
        // Create a 'DatumMapOwningKeysBuilder' object that will administer the
        // process of building a 'Datum' map (owning keys) using the specified
        // 'basicAllocator' to supply memory.  Note that no memory is allocated
        // until 'append' or 'pushBack' methods are called on this object.  The
        // behavior is undefined unless '0 != basicAllocator'.

    DatumMapOwningKeysBuilder(SizeType          initialCapacity,
                              SizeType          initialKeysCapacity,
                              bslma::Allocator *basicAllocator);
        // Create a 'DatumMapBuilder' object managing the ownership of 'Datum'
        // map (owning keys) having the specified 'initialCapacity' and
        // 'initialKeysCapacity' (in bytes) using the specified
        // 'basicAllocator' to supply memory.  The behavior is undefined unless
        // '0 != basicAllocator'.

    ~DatumMapOwningKeysBuilder();
        // Destroy this object.  If this object is holding a datum-key-owning
        // map that has not been adopted, then the datum-key-owning map is
        // disposed after destroying each of its elements.

    // MANIPULATORS
    void append(const DatumMapEntry *entries, SizeType size);
        // Append the specified array 'entries' having the specified 'size' to
        // the 'Datum' map (owning keys) being build by this object.  The
        // behavior is undefined unless and '0 != entries && 0 != size'  and
        // each element in 'entries' that needs dynamic memory, is allocated
        // with the same allocator that was used to construct this object.  The
        // behavior is undefined if 'commit' or 'sortAndCommit' has already
        // been called on this object.

    Datum commit();
        // Return a 'Datum' map (owning keys) value holding the elements
        // supplied to 'pushBack' or 'append'.  The caller is responsible for
        // releasing the resources of the returned 'Datum' object.  Calling
        // this method indicates that the caller is finished building the
        // 'Datum' map (owning keys) and no further values shall be appended.
        // The behavior is undefined if any method of this object, other than
        // its destructor, is called after 'commit' invocation.

    void pushBack(const bslstl::StringRef& key, const Datum& value);
        // Append the entry with the specified 'key' and the specified 'value'
        // to the 'Datum' map being build by this object.  The behavior is
        // undefined if 'value' needs dynamic memory and was allocated using a
        // different allocator than the one used to construct this object.  The
        // behavior is also undefined if 'commit' or 'sortAndCommit' has
        // already been called on this object.

    void setSorted(bool value);
        // Mark the Datum map (owning keys) being built by this object as
        // sorted if the specified 'value' is 'true' and mark it unsorted
        // otherwise.  This function does not sort the map entries, or mark
        // them to be sorted later; the function should be used to indicate if
        // the entries are being appended in sorted order.  The behavior is
        // undefined if 'commit' or 'sortAndCommit' has already been called on
        // this object.  The behavior is also undefined if the map being
        // constructed is marked sorted, but the entries are not appended in
        // sorted order.  Note also that the map being constructed is marked
        // unsorted by default.

    Datum sortAndCommit();
        // Return a 'Datum' map (owning keys) value holding the elements
        // supplied to 'pushBack' or 'append' sorted by their keys.  The caller
        // is responsible for releasing the resources of the returned 'Datum'
        // object.  Calling this method indicates that the caller is finished
        // building the 'Datum' map (owning keys) and no further values shall
        // be appended.  The behavior is undefined if any method of this
        // object, other than its destructor, is called after 'sortAndCommit'
        // invocation.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator associated with this object.

    SizeType capacity() const;
        // Return the capacity of the held 'Datum' map (owning keys).  The
        // behavior is undefined if 'commit' or 'sortAndCommit' has already
        // been called on this object.  Note that similar to the capacity of a
        // 'vector', the returned capacity has no bearing on the value of the
        // 'Datum' being constructed, but does indicate at which point
        // additional memory will be required to grow the 'Datum' map being
        // built.

    SizeType keysCapacity() const;
        // Return the keys-capacity of the held 'Datum' map (owning keys).  The
        // behavior is undefined if 'commit' or 'sortAndCommit' has already
        // been called on this object.  Note that similar to the capacity of a
        // 'vector', the returned capacity has no bearing on the value of the
        // 'Datum' being constructed, but does indicate at which point
        // additional memory will be required to grow the 'Datum' map being
        // built.

    SizeType size() const;
        // Return the size of the held 'Datum' map (owning keys).  The behavior
        // is undefined if 'commit' or 'sortAndCommit' has already been called
        // on this object.
};

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // class DatumMapOwningKeysBuilder
                      // -------------------------------

// ACCESSORS
inline
bslma::Allocator *DatumMapOwningKeysBuilder::allocator() const
{
    return d_allocator_p;
}

inline
DatumMapOwningKeysBuilder::SizeType DatumMapOwningKeysBuilder::capacity() const
{
    return d_capacity;
}

inline
DatumMapOwningKeysBuilder::SizeType DatumMapOwningKeysBuilder::keysCapacity()
                                                                          const
{
    return d_keysCapacity;
}

inline
DatumMapOwningKeysBuilder::SizeType DatumMapOwningKeysBuilder::size() const
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
