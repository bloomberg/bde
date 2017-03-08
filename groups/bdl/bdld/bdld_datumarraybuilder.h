// bdld_datumarraybuilder.h                                           -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMARRAYBUILDER
#define INCLUDED_BDLD_DATUMARRAYBUILDER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a utility to build a 'Datum' object holding an array.
//
//@CLASSES:
//  bdld::DatumArrayBuilder: utility to build a 'Datum' object holding an array
//
//@SEE ALSO: bdld_datum
//
//@DESCRIPTION: This component defines a mechanism, 'bdld::DatumArrayBuilder',
// used to populate a 'Datum' array value in an exception-safe manner.  In
// addition to providing exception safety, a 'DatumArrayBuilder' is
// particularly useful when the length of the array to be constructed is not
// known in advance.  The user can append elements to the datum array as
// needed, and when there are no more elements to append the user calls
// 'commit' and ownership of the populated 'Datum' object is transferred to the
// caller.  After the call to 'commit', no additional elements can be appended
// to the 'Datum' array value.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'DatumArrayBuilder' to Create a 'Datum' array.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
//
// First we define a function 'nextValue' that we will use to tokenize the
// input string:
//..
//  bsl::size_t nextValue(bsl::string *value, const bsl::string& input)
//      // Extract the next value from a list of comma separated values in the
//      // specified 'input' string and load it in the specified 'value'.
//      // Return the index of the next value within 'input'.
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
//..
// Next, we define a function 'convertToDatum' that will convert a string
// token into a 'Datum' scalar value:
//..
//  bdld::Datum convertToDatum(const bsl::string&  token,
//                             bslma::Allocator   *basicAllocator)
//      // Convert the specified 'token' into the appropriate type of scalar
//      // value and then create and return a 'Datum' object using that value.
//      // Use the specified 'basicAllocator' to supply memory.
//  {
//      bool isInteger = true;
//      bool isDouble = false;
//      bool isBoolean = false;
//      for (bsl::size_t i = 0; i < token.size(); ++i) {
//          if (!isdigit(token[i])) {
//              if ('.' == token[i] && !isDouble) {
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
//          if ("true" == token || "false" == token) {
//              isBoolean = true;
//          }
//      }
//
//      if (isInteger) { // integer token
//          return bdld::Datum::createInteger(atoi(token.c_str()));
//      }
//      else if (isDouble) { // double token
//          return bdld::Datum::createDouble(atof(token.c_str()));
//      }
//      else if (isBoolean) { // boolean token
//          return bdld::Datum::createBoolean("true" == token ? true : false);
//      }
//      else { // string value
//          return bdld::Datum::copyString(token, basicAllocator);
//      }
//  }
//..
// Now, in our example main, we tokenize an input string "2.34,4,hi there,true"
// to populate a 'Datum' array containing the values '[2.34, 4, "hi there",
// true]':
//..
//  void exampleMain() {
//      bslma::TestAllocator allocator;
//      const bsl::string    input("2.34,4,hi there,true", &allocator);
//..
// Here, we create a 'DatumArrayBuilder', and iterate over the parsed tokens
// from 'input', using 'pushBack' on the array builder to add the tokens to a
// 'Datum' array value:
//..
//      bdld::DatumArrayBuilder builder(0, &allocator);
//
//      bsl::string str(input, &allocator);
//
//      bsl::string value;
//      bsl::size_t nextIndex;
//      do {
//          nextIndex = nextValue(&value, str);
//          builder.pushBack(convertToDatum(value, &allocator));
//          if (bsl::string::npos == nextIndex) {
//              break;
//          }
//          str = str.substr(nextIndex + 1);
//      } while (bsl::string::npos != nextIndex);
//
//      bdld::Datum result = builder.commit();
//..
// Notice that calling 'commit' on the 'DatumArrayBuilder' adopts ownership
// for the returned 'Datum' object, and that the behavior is undefined if
// 'pushBack' is called after 'commit'.
//
// Finally, we verify that 'result' has the expected array value, and destroy
// the created 'Datum' value:
//..
//      assert(true       == result.isArray());
//      assert(4          == result.theArray().length());
//      assert(true       == result.theArray()[0].isDouble());
//      assert(2.34       == result.theArray()[0].theDouble());
//      assert(true       == result.theArray()[1].isInteger());
//      assert(4          == result.theArray()[1].theInteger());
//      assert(true       == result.theArray()[2].isString());
//      assert("hi there" == result.theArray()[2].theString());
//      assert(true       == result.theArray()[3].isBoolean());
//      assert(true       == result.theArray()[3].theBoolean());
//
//      // Destroy the 'Datum' object.
//      bdld::Datum::destroy(result, &allocator);
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLD_DATUM
#include <bdld_datum.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
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

                          // =======================
                          // class DatumArrayBuilder
                          // =======================

class DatumArrayBuilder {
    // This 'class' provides a mechanism to build a 'Datum' object having an
    // array value in an exception-safe manner.

  public:
    // TYPES
    typedef bsls::Types::size_type SizeType;
        // 'SizeType' is an alias for an unsigned integral value, representing
        // the capacity or length of a datum array.

  private:
    // DATA
    DatumMutableArrayRef  d_array;        // mutable access to the datum array
    SizeType              d_capacity;     // capacity of the datum array
    bslma::Allocator     *d_allocator_p;  // allocator for memory

  private:
    // NOT IMPLEMENTED
    DatumArrayBuilder(const DatumArrayBuilder&);
    DatumArrayBuilder& operator=(const DatumArrayBuilder&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumArrayBuilder,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit DatumArrayBuilder(bslma::Allocator *basicAllocator  = 0);
    explicit DatumArrayBuilder(SizeType          initialCapacity,
                               bslma::Allocator *basicAllocator  = 0);
        // Create a 'DatumArrayBuilder' object that will administer the process
        // of building a 'Datum' array.  Optionally specify an
        // 'initialCapacity' for the array.  If 'initialCapacity' is not
        // supplied, the initial capacity of the array is 0.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~DatumArrayBuilder();
        // Destroy this object.  If this object is holding a 'Datum' array that
        // has not been adopted, then the 'Datum' array is disposed after
        // destroying each of its elements.

    // MANIPULATORS
    void append(const Datum *values, SizeType length);
        // Append the specified array 'values' having the specified 'length' to
        // the 'Datum' array being built by this object.  The behavior is
        // undefined unless '0 != length' and '0 != values' and each element in
        // 'values' that needs dynamic memory, is allocated with the same
        // allocator that was used to construct this object.  The behavior is
        // also undefined if 'commit' has already been called on this object.

    Datum commit();
        // Return a 'Datum' array value holding the elements supplied to
        // 'pushBack' or 'append'.  The caller is responsible for releasing the
        // resources of the returned 'Datum' object.  Calling this method
        // indicates that the caller is finished building the datum array and
        // no further values shall be appended.  It is undefined behavior to
        // call any method of this object, other than the destructor, after
        // 'commit' has been called.

    void pushBack(const Datum& value);
        // Append the specified 'value' to the 'Datum' array being built by
        // this object.  The behavior is undefined if 'value' needs dynamic
        // memory and was allocated using a different allocator than the one
        // used to construct this object.  The behavior is also undefined if
        // 'commit' has already been called on this object.

    // ACCESSORS
    SizeType capacity() const;
        // Return the capacity of the held 'Datum' array.  The behavior is
        // undefined if 'commit' has already been called on this object.  Note
        // that similar to the capacity of a 'vector', the returned capacity
        // has no bearing on the value of the 'Datum' array being constructed,
        // but does indicate at which point additional memory will be required
        // to grow the 'Datum' array being built.

    SizeType size() const;
        // Return the size of the held 'Datum' array.  The behavior is
        // undefined if 'commit' has already been called on this object.
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
