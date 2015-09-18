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
//   DatumMapBuilder: utility to build map of 'Datum' objects
//
//@AUTHOR: Rishi Wani (pwani)
//
//@SEE ALSO: bdld_datum, bdld_datummapref
//
//@DESCRIPTION: This component provides a utility 'class' to build a 'Datum'
// object holding a map of 'Datum' objects that is keyed by string keys.  This
// 'class' is especially useful when the size of the map to be constructed is
// not known in advance.  The user can append elements to the datum map.  When
// the size of the datum map exceeds its capacity, the datum map grows.  The
// user can indicate that it does not have more elements to append (by calling
// 'commit'), and the datum map is then adopted into a 'Datum' object and
// returned to the user.  The user should not try to append any more elements
// to the datum map then.  The user can indicate that the elements need to be
// sorted (by keys) by calling 'sortAndCommit' and the elements will be sorted
// before the map is adopted into a 'Datum' object.  The user can also insert
// elements in a sorted order and tag the map as sorted.  It is undefined
// behavior to tag the map as sorted unless all of the elements are added in
// ascending order.
//
//
///Usage
///-----
// Suppose we receive a string that is constructed by streaming multiple key
// and value pairs together in the format shown below:
//..
//  "(first,2.34),(second,4),(third,hi there),(fourth,true)"
//..
// Notice that the values are separated by a ','.  Also note that a ',' is not
// allowed to be part of a string value to simplify the implementation of the
// utility that parses this string.  The following code snippets illustrate how
// to create a 'Datum' object that holds a map of 'Datum' objects constructed
// using the streamed values.
//..
//  bsl::size_t nextEntry(bsl::string        *key,
//                        bsl::string        *value,
//                        const bsl::string&  input);
//     // Extract the next key and value pair from a list of comma separated
//     // entries in the specified 'input' string.  Load the key into the
//     // specified 'key' and value in the specified 'value'. Return the index
//     // of the next entry within 'input'.
//
//  bsl::size_t nextEntry(bsl::string        *key,
//                        bsl::string        *value,
//                        const bsl::string&  input)
//  {
//      if (input.empty() || input[0] != '(') {
//        return bsl::string::npos;                                   // RETURN
//    }
//
//    const bsl::size_t start = 0;
//    const bsl::size_t nextIndex = input.find(')', start);
//    if (bsl::string::npos == nextIndex) {
//        return bsl::string::npos;                                   // RETURN
//    }
//
//    const bsl::size_t keyIndex = start + 1;
//    const bsl::size_t valueIndex = input.find(',', keyIndex);
//    if (valueIndex >= nextIndex) {
//        return bsl::string::npos;                                   // RETURN
//    }
//
//    *key = input.substr(keyIndex, valueIndex - keyIndex);
//    *value = input.substr(valueIndex + 1, nextIndex - valueIndex - 1);
//    return nextIndex == (input.size() - 1) ?
//                                bsl::string::npos : nextIndex + 1;
//    }
//  }
//
//  Datum convertToDatum(const bsl::string&  value,
//                       bslma::Allocator   *basicAllocator);
//      // Convert the specified 'value' into the appropriate type of scalar
//      // value and then create and return a 'Datum' object using that value.
//      // Use the specified 'basicAllocator' to allocate memory.
//
//  Datum convertToDatum(const bsl::string&  value,
//                       bslma::Allocator   *basicAllocator)
//  {
//      bool isInteger = true;
//      bool isDouble = false;
//      bool isBoolean = false;
//      for (int i = 0; i < value.size(); ++i) {
//          if (!isdigit(value[i])) {
//              if ('.' == value[i] && !isDouble) {
//                  isDouble = true;
//                  isInteger = false;
//                  continue;
//              }
//              isInteger = false;
//              isDouble = false;
//              break;
//          }
//      }
//
//      if (!isInteger && !isDouble) {
//          if ("true" == value || "false" == value) {
//              isBoolean = true;
//          }
//      }
//
//      if (isInteger) { // integer value
//          return Datum::createInteger(atoi(value.c_str()));
//      }
//      else if (isDouble) { // double value
//          return Datum::createDouble(atof(value.c_str()));
//      }
//      else if (isBoolean) { // boolean value
//          return Datum::createBoolean(
//              "true" == value ? true : false);
//      }
//      else { // string value
//          return Datum::copyString(value, basicAllocator);
//      }
//  }
//
//  // Create a test allocator.
//  bslma::TestAllocator defaultAlloc(1);
//  bslma::Allocator *alloc = &defaultAlloc;
//
//  const bsl::string input(
//      "(first,2.34),(second,4),(third,hi there),(fourth,true)", alloc);
//
//  // Create a builder object.
//  DatumMapBuilder builder(alloc);
//
//  bsl::string str(input, alloc);
//  bsl::vector<const char *> keys(alloc);
//
//  bsl::string key;
//  bsl::string value;
//  int numEntries = 0;
//  bsl::size_t nextIndex;
//  do {
//      nextIndex = nextEntry(&key, &value, str);
//      char *keyCopy = new char[key.size()];
//      strncpy(keyCopy, key.c_str(), key.size());
//      keys.push_back(keyCopy);
//      builder.pushBack(bslstl::StringRef(keys[numEntries],
//                                         bsl::strlen(keys[numEntries])),
//                       convertToDatum(value, alloc));
//      ++numEntries;
//      if (bsl::string::npos == nextIndex) {
//          break;
//      }
//      str = str.substr(nextIndex + 1);
//  } while (bsl::string::npos != nextIndex);
//
//  Datum result = builder.commit();

//  assert(result.isMap());
//  assert(numEntries == result.theMap().size());
//  assert(keys[0] == bsl::string(result.theMap()[0].key()));
//  assert(result.theMap()[0].value().isDouble());
//  assert(2.34 == result.theMap()[0].value().theDouble());
//  assert(keys[1] == bsl::string(result.theMap()[1].key()));
//  assert(result.theMap()[1].value().isInteger());
//  assert(4 == result.theMap()[1].value().theInteger());
//  assert(keys[2] == bsl::string(result.theMap()[2].key()));
//  assert(result.theMap()[2].value().isString());
//  assert("hi there" == result.theMap()[2].value().theString());
//  assert(keys[3] == bsl::string(result.theMap()[3].key()));
//  assert(result.theMap()[3].value().isBoolean());
//  assert(true == result.theMap()[3].value().theBoolean());
//
//  // Destroy the 'Datum' object.
//  Datum::destroy(result, alloc);
//
//  // Destroy the keys.
//  for (int i = 0; i < keys.size(); ++i) {
//      delete[] const_cast<char *>(keys[i]);
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLD_DATUM
#include <bdld_datum.h>
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
        // 'SizeType' is an alias for a signed value, representing the capacity
        // or size of a datum map.
  private:
    // DATA
    DatumMapRef       d_mapping;      // mutable access to the datum map
    SizeType          d_capacity;     // capacity of the datum map
    bool              d_sorted;       // underlying map is sorted or not
    bslma::Allocator *d_allocator_p;  // allocator for memory

  private:
    // NOT IMPLEMENTED
    DatumMapBuilder(const DatumMapBuilder&);
    DatumMapBuilder& operator=(const DatumMapBuilder&);

  public:
    // CREATORS
    explicit DatumMapBuilder(bslma::Allocator *basicAllocator);
        // Create a 'DatumMapBuilder' object.  Note that this holds a copy of
        // the specified 'basicAllocator' pointer, but does not allocate any
        // memory.  A datum map will be created and memory will be allocated
        // when pushBack/append is called.  The behavior is undefined unless
        // '0 != basicAllocator'.

    DatumMapBuilder(SizeType          initialCapacity,
                    bslma::Allocator *basicAllocator);
        // Create a 'DatumMapBuilder' object. This constructor creates a datum
        // map having the specified 'initialCapacity' using the specified
        // 'basicAllocator'.  The behavior is undefined unless
        // '0 < initialCapacity' and '0 != basicAllocator'.

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
        // undefined unless '0 != length' and '0 != entries' and each element
        // in 'entries' that needs dynamic memory, is allocated with the same
        // allocator that was used to construct this object.  The behavior is
        // undefined if 'commit' or 'sortAndCommit' has already been called on
        // this object.

    Datum commit();
        // Return a 'Datum' object holding a map of 'Datum' objects built using
        // 'pushBack' or 'append'.  This method indicates that the caller is
        // finished building the datum map and no further values shall be
        // appended.  It is undefined behavior to call any method of this
        // object, other than its destructor, after 'commit' has been called.

    void setSorted(bool value);
        // Indicate that the underlying map is sorted if the specified 'value'
        // is 'true' and unsorted otherwise.  The behavior is undefined if
        // 'commit' or 'sortAndCommit' has already been called on this object.
        // Note that the map is unsorted by default.

    Datum sortAndCommit();
        // Return a 'Datum' object holding a map of 'Datum' objects build using
        // 'pushBack' or 'append'.  Sort the elements of the map.  This method
        // indicates that the caller is finished building the datum map and no
        // further values shall be appended.  It is undefined behavior to call
        // any method of this object, other than its destructor, after
        // 'sortAndCommit' has been called.

    // ACCESSORS
    SizeType capacity() const;
        // Return the capacity.  The behavior is undefined if 'commit' or
        // 'sortAndCommit' has already been called on this object.
};

}  // close bdld namespace
}  // close BloombergLP namespace

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
