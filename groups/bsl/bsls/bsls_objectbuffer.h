// bsls_objectbuffer.h                                                -*-C++-*-
#ifndef INCLUDED_BSLS_OBJECTBUFFER
#define INCLUDED_BSLS_OBJECTBUFFER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide raw buffer with size and alignment of user-specified type.
//
//@CLASSES:
//  bsls::ObjectBuffer: templatized buffer aligned to hold specified type
//
//@SEE_ALSO: bsls_alignmentfromtype
//
//@DESCRIPTION: This component provides a templated buffer type,
// 'bsls::ObjectBuffer', which is compile-time sized and aligned to hold a
// specified object type.  Defining a 'bsls::ObjectBuffer<T>' object does not
// cause the constructor for 'T' to be called.  Similarly, destroying the
// object buffer does not call the destructor for 'T'.  Instead, the user
// instantiates 'bsls::ObjectBuffer' with a specific type, then constructs an
// object of that type within that buffer.  When the object is no longer
// needed, the user must explicitly call its destructor.  A
// 'bsls::ObjectBuffer' can reside on the stack or within another object,
// including within a 'union'.
//
// Typically, a 'bsls::ObjectBuffer' is used in situations where efficient
// (e.g., stack-based) storage is required but where straight-forward
// initialization or destruction of an object is not possible.  For example,
// 'bsls::ObjectBuffer' can be used to construct an array where the number of
// used elements varies at run-time or where the element type does not have a
// default constructor.  It can also be used to create a 'union' containing
// non-POD element types.
//
///Usage
///-----
// The examples below use a value-semantic string class, 'my_String' which can
// be constructed from a null-terminated string and contains a member, 'c_str'
// which returns a null-terminated string.  'my_String' does not have a default
// constructor and thus cannot be used in C-style arrays or unions.
//
///Example 1: Creating a Dynamic Array of Objects
/// - - - - - - - - - - - - - - - - - - - - - - -
// Here we use 'bsls::ObjectBuffer' to create a variable-length array of
// 'my_String' objects.  For efficiency, the array is created on the stack as
// a fixed-sized array of 'bsls::ObjectBuffer<my_String>' objects and the
// length is kept in a separate variable.  Only 'len' calls are made to the
// 'my_String' constructor, with the unused array elements left as raw
// memory.  An array directly containing 'my_String' objects would not have
// been possible because 'my_String' does not have a default constructor.
//
// WARNING: the 'manipulateStrings' function below is not exception-safe.
// If an exception is thrown anywhere within the function (e.g., from a
// constructor call), the destructor will not be called on the constructed
// string objects.  This logic would typically be augmented with guard objects
// that call destructors in case of exception.
//..
//  void manipulateStrings(const my_String* stringArray, int len)
//  {
//      assert(len <= 10);
//
//      bsls::ObjectBuffer<my_String> tempArray[10];
//      for (int i = 0; i < len; ++i) {
//          new (tempArray[i].buffer()) my_String(stringArray[i]);
//          assert(stringArray[i] == tempArray[i].object())
//      }
//
//      for (int i = 0; i < len; ++i)
//      {
//          my_String& s = tempArray[i].object();
//          // ... String manipulations go here.  's' might be analyzed,
//          // appended-to, passed to other functions, etc.
//      }
//
//      while (len) {
//          // Destroy strings.  Although not critical to this example, we
//          // follow the general rule of destroying the objects in reverse
//          // order of their construction, thus mimicking the
//          // compiler-generated destruction order for normal array objects.
//          tempArray[--len].object().~my_String();
//      }
//  }
//
//  int main()
//  {
//      const my_String INARRAY[3] = {
//          my_String("hello"),
//          my_String("goodbye"),
//          my_String("Bloomberg")
//      };
//
//      manipulateStrings(INARRAY, 3);
//
//      return 0;
//  }
//..
//
///Example 2: Containing Different Object Types
/// - - - - - - - - - - - - - - - - - - - - - -
// Here we use 'bsls::ObjectBuffer' to compose a variable-type object capable
// of holding a string or an integer:
//..
//  class my_Union
//  {
//    public:
//      enum TypeTag { INT, STRING };
//
//    private:
//      TypeTag                           d_type;
//      union {
//          int                           d_int;
//          bsls::ObjectBuffer<my_String> d_string;
//      };
//
//    public:
//      my_Union(int i = 0) : d_type(INT) { d_int = i; }            // IMLPICIT
//      my_Union(const my_String& s) : d_type(STRING) {             // IMLPICIT
//          new (d_string.buffer()) my_String(s); }
//      my_Union(const char *s) : d_type(STRING) {                  // IMLPICIT
//          new (d_string.buffer()) my_String(s); }
//      my_Union(const my_Union& rhs) : d_type(rhs.d_type) {
//          if (INT == d_type) {
//              d_int = rhs.d_int;
//          }
//          else {
//              new (d_string.buffer()) my_String(rhs.d_string.object());
//          }
//      }
//      ~my_Union() {
//          if (STRING == d_type) d_string.object().~my_String(); }
//
//      my_Union& operator=(const my_Union& rhs) {
//          if (INT == d_type) {
//              if (INT == rhs.d_type) {
//                  d_int = rhs.d_int;
//              }
//              else { // if STRING == rhs.d_type
//                  new (d_string.buffer()) my_String(rhs.d_string.object());
//              }
//          }
//          else { // if (STRING == d_type)
//              if (INT == rhs.d_type) {
//                  d_string.object().~my_String();
//                  d_int = rhs.d_int;
//              }
//              else { // if STRING == rhs.d_type
//                  d_string.object() = rhs.d_string.object();
//              }
//          }
//          d_type = rhs.d_type;
//          return *this;
//      }
//
//      TypeTag typeTag() const { return d_type; }
//
//      int asInt() const {
//          return INT == d_type ?
//                          d_int : static_cast<int>(
//                                    strtol(d_string.object().c_str(), 0, 0));
//  }
//
//      my_String asString() const {
//          if (INT == d_type) {
//              char temp[15];
//              sprintf(temp, "%d", d_int);
//              return my_String(temp);
//          }
//          else {
//              return d_string.object();
//          }
//      }
//  };
//
//  int main()
//  {
//      assert(sizeof(bsls::ObjectBuffer<my_String>) == sizeof(my_String));
//
//      // Create a 'my_Union' object containing a string.
//      const my_Union U1("hello");
//      assert(my_Union::STRING == U1.typeTag());
//      assert(0 == U1.asInt());
//      assert("hello" == U1.asString());
//
//      // Create a 'my_Union' object containing an integer.
//      const my_Union U2(123);
//      assert(my_Union::INT == U2.typeTag());
//      assert(123 == U2.asInt());
//      assert("123" == U2.asString());
//
//      // Create a 'my_Union' object containing a string that can be
//      // interpreted as an integer.
//      const my_Union U3("0x456");
//      assert(my_Union::STRING == U3.typeTag());
//      assert(0x456 == U3.asInt());
//      assert("0x456" == U3.asString());
//
//      // Copy-construct a 'my_Union' object containing a string.
//      my_Union u4(U3);
//      assert(my_Union::STRING == u4.typeTag());
//      assert(0x456 == u4.asInt());
//      assert("0x456" == u4.asString());
//
//      // Use assignment to change 'u4' from string to integer.
//      u4 = U2;
//      assert(my_Union::INT == u4.typeTag());
//      assert(123 == u4.asInt());
//      assert("123" == u4.asString());
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLS_ALIGNMENTFROMTYPE
#include <bsls_alignmentfromtype.h>
#endif

namespace BloombergLP {

namespace bsls {

                        // ==================
                        // union ObjectBuffer
                        // ==================

template <class TYPE>
union ObjectBuffer {
    // An instance of this union is a raw block of memory suitable for storing
    // an object of type 'TYPE'.  Specifically, the size and alignment of this
    // union exactly matches that of 'TYPE'.  A 'TYPE' object can be
    // constructed into a 'ObjectBuffer' using the placement 'new'
    // operator and can be destroyed by explicitly calling its destructor,
    // '~TYPE()'.  It is the user's responsibility to perform this
    // construction and destruction; a 'AlignedBuffer' object does not
    // manage the construction or destruction of any other objects.
    //
    // Note that a collaboration is implied between 'ObjectBuffer' and
    // the user.  A 'ObjectBuffer' provides aligned memory and the user
    // handles construction and destruction of the object contained within
    // that memory.

  private:
    // Buffer correctly sized and aligned for object of type 'TYPE'.
    char                                   d_buffer[sizeof(TYPE)];
    typename AlignmentFromType<TYPE>::Type d_align;

  public:
    // CREATORS
    // Note that we deliberately omit defining constructors and destructors in
    // order to keep this union "POD-like".  In particular, a
    // 'ObjectBuffer' may be used as a member in another 'union'.
    // Copying a 'ObjectBuffer' by assignment or copy construction will
    // result in a bit-wise copy and will not invoke 'TYPE's assignment
    // operator or copy constructor.

    // MANIPULATORS
    TYPE& object();
        // Return a modifiable reference to the 'TYPE' object occupying this
        // buffer.  The referenced object has undefined state unless a valid
        // 'T' object has been constructed in this buffer.

    char *buffer();
        // Return a the address of the first byte of this object, cast to a
        // 'char*' pointer.

    // ACCESSORS
    const TYPE& object() const;
        // Return a const reference to the 'TYPE' object occupying this
        // buffer.  The referenced object has undefined state unless a valid
        // 'T' object has been constructed in this buffer.

    const char *buffer() const;
        // Return a the address of the first byte of this object, cast to a
        // 'const char*' pointer.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// MANIPULATORS
template <class TYPE>
inline
TYPE& ObjectBuffer<TYPE>::object()
{
    return *reinterpret_cast<TYPE*>(this);
}

template <class TYPE>
inline
char *ObjectBuffer<TYPE>::buffer()
{
    return d_buffer;
}

// ACCESSORS
template <class TYPE>
inline
const TYPE& ObjectBuffer<TYPE>::object() const
{
    return *reinterpret_cast<const TYPE*>(this);
}

template <class TYPE>
inline
const char *ObjectBuffer<TYPE>::buffer() const
{
    return d_buffer;
}

}  // close package namespace

// ============================================================================
//                          BACKWARD COMPATIBILITY
// ============================================================================



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
