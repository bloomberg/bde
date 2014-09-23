// bsls_alignmenttotype.h                                             -*-C++-*-
#ifndef INCLUDED_BSLS_ALIGNMENTTOTYPE
#define INCLUDED_BSLS_ALIGNMENTTOTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function mapping an 'ALIGNMENT' to a primitive type.
//
//@CLASSES:
//  bsls::AlignmentToType: maps 'ALIGNMENT' to a so-aligned primitive type
//
//@SEE_ALSO: bsls_alignmentfromtype
//
//@DESCRIPTION: This component provides a meta-function,
// 'bsls::AlignmentToType', parameterized on an integral 'ALIGNMENT', that
// declares a 'typedef' ('Type'), which is an alias for a primitive type having
// the indicated 'ALIGNMENT' requirement.
//
///Usage
///-----
// Consider a parameterized type, 'my_AlignedBuffer', that provides aligned
// memory to store a user-defined type.  A 'my_AlignedBuffer' object is useful
// in situations where efficient (e.g., stack-based) storage is required.
//
// The 'my_AlignedBuffer' 'union' (defined below) takes a 'TYPE' and the
// 'ALIGNMENT' requirements for that type as template parameters, and provides
// an appropriately sized and aligned block of memory via the 'buffer'
// functions.  Note that 'my_AlignedBuffer' ensures that the returned memory is
// aligned correctly for the specified size by using
// 'bsls::AlignmentToType<ALIGNMENT>::Type', which provides a primitive type
// having the 'ALIGNMENT' requirement.  The class definition of
// 'my_AlignedBuffer' is as follows:
//..
//  template <class TYPE, int ALIGNMENT>
//  union my_AlignedBuffer {
//    private:
//      // DATA
//      char                                           d_buffer[sizeof(TYPE)];
//      typename bsls::AlignmentToType<ALIGNMENT>::Type d_align;  // force
//                                                                // alignment
//
//    public:
//      // MANIPULATORS
//      char *buffer();
//          // Return the address of the modifiable first byte of memory
//          // contained by this object as a 'char *' pointer.
//
//      TYPE& object();
//          // Return a reference to the modifiable 'TYPE' object stored in
//          // this buffer.  The referenced object has an undefined state
//          // unless a valid 'TYPE' object has been constructed in this
//          // buffer.
//
//      // ACCESSORS
//      const char *buffer() const;
//          // Return the address of the non-modifiable first byte of memory
//          // contained by this object as a 'const char *' pointer.
//
//      const TYPE& object() const;
//          // Return a reference to the non-modifiable 'TYPE' object stored in
//          // this buffer.  The referenced object has an undefined state
//          // unless a valid 'TYPE' object has been constructed in this
//          // buffer.
//  };
//..
// The function definitions of 'my_AlignedBuffer' are as follows:
//..
//  // MANIPULATORS
//  template <class TYPE, int ALIGNMENT>
//  inline
//  char *my_AlignedBuffer<TYPE, ALIGNMENT>::buffer()
//  {
//      return d_buffer;
//  }
//
//  template <class TYPE, int ALIGNMENT>
//  inline
//  TYPE& my_AlignedBuffer<TYPE, ALIGNMENT>::object()
//  {
//      return *reinterpret_cast<TYPE *>(this);
//  }
//
//  // ACCESSORS
//  template <class TYPE, int ALIGNMENT>
//  inline
//  const char *my_AlignedBuffer<TYPE, ALIGNMENT>::buffer() const
//  {
//      return d_buffer;
//  }
//
//  template <class TYPE, int ALIGNMENT>
//  inline
//  const TYPE& my_AlignedBuffer<TYPE, ALIGNMENT>::object() const
//  {
//      return *reinterpret_cast<const TYPE *>(this);
//  }
//..
// 'my_AlignedBuffer' can be used to construct buffers for different types and
// with varied alignment requirements.  Consider that we want to construct an
// object that stores the response of a floating-point operation.  If the
// operation is successful, then the response object stores a 'double' result;
// otherwise, it stores an error string of type 'string', which is based on the
// standard type 'string' (see 'bslstl_string').  For the sake of brevity, the
// implementation of 'string' is not explored here.  Here is the definition for
// the 'Response' class:
//..
//  class Response {
//..
// To create a 'my_AlignedBuffer' object we must specify the alignment value
// for our types.  For simplicity, we use a maximum alignment value for all
// types (assumed to be 8 here):
//..
//      enum { MAX_ALIGNMENT = 8 };
//..
// Note that we use 'my_AlignedBuffer' to allocate sufficient, aligned memory
// to store the result of the operation or an error message:
//..
//  private:
//    union {
//        my_AlignedBuffer<double, MAX_ALIGNMENT>      d_result;
//        my_AlignedBuffer<string, MAX_ALIGNMENT> d_errorMessage;
//    };
//..
// The 'isError' flag indicates whether the response object stores valid data
// or an error message:
//..
//  bool d_isError;
//..
// Below we provide a simple public interface suitable for illustration only:
//..
//  public:
//    // CREATORS
//    Response(double result);
//        // Create a response object that stores the specified 'result'.
//
//    Response(const string& errorMessage);
//        // Create a response object that stores the specified
//        // 'errorMessage'.
//
//    ~Response();
//        // Destroy this response object.
//..
// The manipulator functions allow clients to update the response object to
// store either a 'double' result or an error message:
//..
//  // MANIPULATORS
//  void setResult(double result);
//      // Update this object to store the specified 'result'.  After this
//      // operation 'isError' returns 'false'.
//
//  void setErrorMessage(const string& errorMessage);
//      // Update this object to store the specified 'errorMessage'.  After
//      // this operation 'isError' returns 'true'.
//..
// The 'isError' function informs clients whether a response object stores a
// result value or an error message:
//..
//      // ACCESSORS
//      bool isError() const;
//          // Return 'true' if this object stores an error message, and
//          // 'false' otherwise.
//
//      double result() const;
//          // Return the result value stored by this object.  The behavior is
//          // undefined unless 'false == isError()'.
//
//      const string& errorMessage() const;
//          // Return a reference to the non-modifiable error message stored by
//          // this object.  The behavior is undefined unless
//          // 'true == isError()'.
//  };
//..
// Below we provide the function definitions.  Note that we use the
// 'my_AlignedBuffer::buffer' function to access correctly aligned memory.
// Also note that 'my_AlignedBuffer' just provides the memory for an object;
// therefore, the 'Response' class is responsible for the construction and
// destruction of the specified objects.  Since our 'Response' class is for
// illustration purposes only, we ignore exception-safety concerns; nor do we
// supply an allocator to the string constructor, allowing the default
// allocator to be used instead:
//..
//  // CREATORS
//  Response::Response(double result)
//  {
//      new (d_result.buffer()) double(result);
//      d_isError = false;
//  }
//
//  Response::Response(const string& errorMessage)
//  {
//      new (d_errorMessage.buffer()) string(errorMessage);
//      d_isError = true;
//  }
//
//  Response::~Response()
//  {
//      if (d_isError) {
//          typedef string Type;
//          d_errorMessage.object().~Type();
//      }
//  }
//
//  // MANIPULATORS
//  void Response::setResult(double result)
//  {
//      if (!d_isError) {
//          d_result.object() = result;
//      }
//      else {
//          typedef string Type;
//          d_errorMessage.object().~Type();
//          new (d_result.buffer()) double(result);
//          d_isError = false;
//      }
//  }
//
//  void Response::setErrorMessage(const string& errorMessage)
//  {
//      if (d_isError) {
//          d_errorMessage.object() = errorMessage;
//      }
//      else {
//          new (d_errorMessage.buffer()) string(errorMessage);
//          d_isError = true;
//      }
//  }
//
//  // ACCESSORS
//  bool Response::isError() const
//  {
//      return d_isError;
//  }
//
//  double Response::result() const
//  {
//      assert(!d_isError);
//
//      return d_result.object();
//  }
//
//  const string& Response::errorMessage() const
//  {
//      assert(d_isError);
//
//      return d_errorMessage.object();
//  }
//..
// Clients of the 'Response' class can use it as follows:
//..
//  double value1 = 111.2, value2 = 92.5;
//
//  if (0 == value2) {
//      Response response("Division by 0");
//
//      // Return erroneous response
//  }
//  else {
//      Response response(value1 / value2);
//
//      // Process response object
//  }
//..

#ifndef INCLUDED_BSLS_ALIGNMENTIMP
#include <bsls_alignmentimp.h>
#endif

namespace BloombergLP {

namespace bsls {

                         // ======================
                         // struct AlignmentToType
                         // ======================

template <int ALIGNMENT>
struct AlignmentToType {
    // This 'struct' provides a 'typedef', 'Type', that aliases a primitive
    // type having the specified 'ALIGNMENT' requirement.

  private:
    // PRIVATE TYPES
    typedef typename AlignmentImpMatch::MaxPriority MaxPriority;
    typedef          AlignmentImpTag<ALIGNMENT>     Tag;

    enum {
        // Compute the priority of the primitive type corresponding to the
        // specified 'ALIGNMENT'.

        PRIORITY = sizeof(AlignmentImpMatch::match(Tag(),
                                                   Tag(),
                                                   MaxPriority()))
    };

  public:
    // TYPES
    typedef typename AlignmentImpPriorityToType<PRIORITY>::Type Type;
        // Alias for a primitive type that has the specified 'ALIGNMENT'
        // requirement.
};

}  // close package namespace


}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
