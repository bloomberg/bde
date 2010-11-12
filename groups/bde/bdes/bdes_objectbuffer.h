// bdes_objectbuffer.h                  -*-C++-*-
#ifndef INCLUDED_BDES_OBJECTBUFFER
#define INCLUDED_BDES_OBJECTBUFFER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide raw buffer with size and alignment of user-specified type.
//
//@DEPRECATED: Use 'bsls_objectbuffer' instead.
//
//@CLASSES:
//  bdes_ObjectBuffer: deprecated, do *not* use
//
//@SEE_ALSO: bsls_alignment
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a templated buffer type,
// 'bdes_ObjectBuffer', which is compile-time sized and aligned to hold a
// specified object type.  Defining a 'bdes_ObjectBuffer<T>' object does not
// cause the constructor for 'T' to be called.  Similarly, destroying the
// object buffer does not call the destructor for 'T'.  Instead, the user
// instantiates 'bdes_ObjectBuffer' with a specific type, then constructs an
// object of that type within that buffer.  When the object is no longer
// needed, the user must explicitly call its destructor.  A
// 'bdes_ObjectBuffer' can reside on the stack or within another object,
// including within a 'union'.
//
// Typically, a 'bdes_ObjectBuffer' is used in situations where efficient
// (e.g., stack-based) storage is required but where straight-forward
// initialization or destruction of an object is not possible.  For example,
// 'bdes_ObjectBuffer' can be used to construct an array where the number of
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
///Usage Example 1:
///- - - - - - - -
// Here we use 'bdes_ObjectBuffer' to create a variable-length array of
// 'my_String' objects.  For efficiency, the array is created on the stack as
// a fixed-sized array of 'bdes_ObjectBuffer<my_String>' objects and the
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
//      bdes_ObjectBuffer<my_String> tempArray[10];
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
///Usage Example 2:
///- - - - - - - -
// Here we use 'bdes_ObjectBuffer' to compose a variable-type object capable
// of holding a string or an integer:
//..
//  class my_Union
//  {
//    public:
//      enum TypeTag { INT, STRING };
//
//    private:
//      TypeTag                          d_type;
//      union {
//          int                          d_int;
//          bdes_ObjectBuffer<my_String> d_string;
//      };
//
//    public:
//      my_Union(int i = 0) : d_type(INT) { d_int = i; }
//      my_Union(const my_String& s) : d_type(STRING) {
//          new (d_string.buffer()) my_String(s); }
//      my_Union(const char *s) : d_type(STRING) {
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
//              d_int : strtol(d_string.object().c_str(), 0, 0); }
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
//      assert(sizeof(bdes_ObjectBuffer<my_String>) == sizeof(my_String));
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

namespace BloombergLP {

    // No symbol is defined here (see the 'bsls_objectbuffer' component).
    // No alias for the 'bdes_ObjectBuffer' type is defined here, instead it is
    // defined in 'bsls_objectbuffer' so that clients that rely on it via
    // transitive includes may still have that alias defined.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
