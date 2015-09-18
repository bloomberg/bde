// bdld_datumarraybuilder.h                                           -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMARRAYBUILDER
#define INCLUDED_BDLD_DATUMARRAYBUILDER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a utility to build 'Datum' object holding an array.
//
//@CLASSES:
//   DatumArrayBuilder: utility to build an array of 'Datum' objects
//
//@AUTHOR: Rishi Wani (pwani)
//
//@SEE ALSO: bdld_datum
//
//@DESCRIPTION: This component provides a utility 'class' to build 'Datum'
// object holding an array of 'Datum' objects.  This 'class' is especially
// useful when the length of the array to be constructed is not known in
// advance.  The user can append elements to the datum array.  When the length
// of the datum array exceeds its capacity, the datum array grows.  The user
// can indicate that it does not have more elements to append, and the datum
// array is then adopted into a 'Datum' object and returned to the user.  The
// user should not try to append any more elements to the datum array then.
//
///Usage
///-----
// Suppose we receive a string that is constructed by streaming a bunch of
// values together in the format shown below:
//..
//  "2.34,4,hi there,true"
//..
// Notice that the values are separated by a ','.  Also note that a ',' is not
// allowed to be part of a string value to simplify the implementation of the
// utility that parses this string.  The following code snippets illustrate how
// to create a 'Datum' object that holds an array of 'Datum' objects
// constructed using the streamed values.
//..
//  bsl::size_t nextValue(bsl::string *value, const bsl::string& input);
//      // Extract the next value from a list of comma separated values in the
//      // specified 'input' string and load it in the specified 'value'.
//      // Return the index of the next value within 'input'.
//
//  bsl::size_t nextValue(bsl::string *value, const bsl::string& input)
//  {
//      if (input.empty()) {
//          return bsl::string::npos;
//      }
//      int start = 0;
//      bsl::size_t nextIndex = input.find(',', start);
//      if (bsl::string::npos != nextIndex) {
//          *value = input.substr(start, nextIndex - start);
//      }
//      else {
//          *value = input.substr(start);
//      }
//      return nextIndex;
//  }
//
//  Datum convertToDatum(const bsl::string& value,
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
//  const bsl::string input("2.34,4,hi there,true", alloc);
//
//  // Create a builder object.
//  DatumArrayBuilder builder(alloc);
//
//  bsl::string str(input, alloc);
//
//  bsl::string value;
//  int numValues = 0;
//  bsl::size_t nextIndex;
//  do {
//      nextIndex = nextValue(&value, str);
//      ++numValues;
//      builder.pushBack(convertToDatum(value, alloc));
//      if (bsl::string::npos == nextIndex) {
//          break;
//      }
//      str = str.substr(nextIndex + 1);
//  } while (bsl::string::npos != nextIndex);
//
//  Datum result = builder.commit();
//
//  assert(result.isArray());
//  assert(numValues == result.theArray().length());
//  assert(result.theArray()[0].isDouble());
//  assert(2.34 == result.theArray()[0].theDouble());
//  assert(result.theArray()[1].isInteger());
//  assert(4 == result.theArray()[1].theInteger());
//  assert(result.theArray()[2].isString());
//  assert("hi there" == result.theArray()[2].theString());
//  assert(result.theArray()[3].isBoolean());
//  assert(true == result.theArray()[3].theBoolean());
//
//  // Destroy the 'Datum' object.
//  Datum::destroy(result, alloc);
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

                          // =======================
                          // class DatumArrayBuilder
                          // =======================

class DatumArrayBuilder {
    // This 'class' provides a utility to build a 'Datum' object holding an
    // array of 'Datum' objects.

  public:
    // TYPES
    typedef bsls::Types::size_type SizeType;
        // 'SizeType' is an alias for a signed value, representing the capacity
        // or length of a datum array.

  private:
    // DATA
    DatumArrayRef     d_array;        // mutable access to the datum array
    SizeType          d_capacity;     // capacity of the datum array
    bslma::Allocator *d_allocator_p;  // allocator for memory

  private:
    // NOT IMPLEMENTED
    DatumArrayBuilder(const DatumArrayBuilder&);
    DatumArrayBuilder& operator=(const DatumArrayBuilder&);

  public:
    // CREATORS
    explicit DatumArrayBuilder(bslma::Allocator *basicAllocator);
        // Create a 'DatumArrayBuilder' object.  Note that this holds a copy of
        // the specified 'basicAllocator' pointer, but does not allocate any
        // memory.  A datum array will be created and memory will be allocated
        // when pushBack/append is called.  The behavior is undefined unless
        // '0 != basicAllocator'.

    DatumArrayBuilder(SizeType          initialCapacity,
                      bslma::Allocator *basicAllocator);
        // Create a 'DatumArrayBuilder' object. This constructor creates a
        // datum array having the specified 'initialCapacity' using the
        // specified 'basicAllocator'.  The behavior is undefined unless
        // '0 < initialCapacity' and '0 != basicAllocator'.

    ~DatumArrayBuilder();
        // Destroy this object.  If this object is holding a datum array that
        // has not been adopted, then the datum array is disposed after
        // destroying each of its elements.

    // MANIPULATORS
    void pushBack(const Datum& value);
        // Append the specified 'value' to the end of the held datum array.  If
        // the datum array is full, a new datum array with larger capacity is
        // allocated and any previous datum array is disposed after copying its
        // elements.  The behavior is undefined if 'value' needs dynamic memory
        // and it was allocated using a different allocator than the one used
        // to construct this object.  The behavior is also undefined if
        // 'commit' has already been called on this object.

    void append(const Datum *values, SizeType length);
        // Append the specified array 'values' having the specified 'length' to
        // the end of the held datum array.  Note that if the datum array is
        // full, a new datum array with larger capacity is allocated and the
        // previous datum array is disposed after copying its elements.  The
        // behavior is undefined unless '0 != length' and '0 != values' and
        // each element in 'values' that needs dynamic memory, is allocated
        // with the same allocator that was used to construct this object.  The
        // behavior is undefined if 'commit' has already been called on this
        // object.

    Datum commit();
        // Return a 'Datum' object holding an array of 'Datum' objects built
        // using 'pushBack' or 'append'.  This method indicates that the caller
        // is finished building the datum array and no further values shall be
        // appended.  It is undefined behavior to call any method of this
        // object, other than the destructor, after 'commit' has been called.

    // ACCESSORS
    SizeType capacity() const;
        // Return the capacity.  The behavior is undefined if 'commit' has
        // already been called on this object.
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
