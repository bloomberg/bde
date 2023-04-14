// bslmf_movableref.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_MOVABLEREF
#define INCLUDED_BSLMF_MOVABLEREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a vocabulary type to enable move semantics.
//
//@CLASSES:
//  bslmf::MovableRef: a template indicating that an object can be moved from
//  bslmf::MovableRefUtil: a namespace for functions dealing with movables
//
//@MACROS:
//  BSLMF_MOVABLEREF_DEDUCE(t_TYPE): movable ref of 't_TYPE' that is deducible
//  BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES: defined if MovableRef<T> is 'T&&'
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a class template, 'bslmf::MovableRef'
// used to convey the information that an object will not be used anymore so
// that its representation can be transferred elsewhere.  In C++11 terminology
// an object represented by a 'bslmf::MovableRef<T>' can be moved from.  This
// component also provides a utility 'struct' 'bslmf::MovableRefUtil' that
// enables use of identical code for C++03 and C++11 to implement move
// semantics.
//
// An object is /movable/ when it isn't being used in a way depending on its
// current representation after an operation on this object.  For example, when
// passing a temporary object to a function the temporary object is movable: it
// can't be referred to other than in the function call.  When objects are no
// longer be used their internal representation can be transferred to another
// object.  Transferring the internal representation of an object to another
// object is called /moving an object/.  The purpose of 'bslmf::MovableRef<T>'
// is to indicate to a function that an object can be moved to another object.
//
// With a C++11 implementation 'bslmf::MovableRef<T>' is an alias template for
// 'T&&'.  With a C++03 implementation 'bslmf::MovableRef<T>' is a class
// template providing l-value access to a movable object.  The objective of
// this component is to provide a name for the concept of a movable object.
// Using a common name enables use of manual move semantics when using C++03.
// With C++11 additionally automatic move semantics is enabled resulting in
// moving objects known to the compiler to go out of scope, e.g., when passing
// a temporary object to a function or returning a local variable.
//
// Using 'bslmf::MovableRef<T>' to support movable types enables the
// implementation of move semantics that work with both C++03 and C++11 without
// conditional compilation of the user code.  Only the implementation of the
// component 'bslmf_movableref' uses conditional compilation to select the
// appropriate implementation choice.  For a C++11 implementation the use of
// 'bslmf::MovableRef<T>' instead of 'T&&' has the advantage that the 'T' will
// not be deduced an the argument is known to be movable: when a function takes
// a 'T&&' as argument with a deduced 'T' the deduced type may be an l-value
// reference that isn't necessarily movable.  When using 'bslmf::MovableRef<T>'
// with C++11 the type will not be deduced and to turn an l-value into a
// movable object it is necessary to explicitly use 'move()' at the call site.
//
// For consistent use across different versions of the C++ standard, a few
// utility functions are provided in the utility class 'bslmf::MovableRefUtil'.
// This class contains functions for moving and accessing objects.  To enable
// an identical notation to access an object with C++11 (where
// 'bslmf::MovableRef<T>' is just an l-value of type 'T') and with C++03 where
// 'bslmf::MovableRef<T>' is a class type referencing an l-value of type 'T',
// the function template 'bslmf::MovableRefUtil::access(r)' is provided.
// Similarly, the function 'bslmf::MovableRefUtil::move(r)' provides
// identical notation for producing a movable reference in both C++03 and
// C++11.
//
// In addition to the 'move' and 'access' functions, the
// 'bslmf::MovableRefUtil' namespace provides 7 metafunctions that closely
// correspond to similar metafunctions in the C++11 standard library (and
// which defer to the standard library where available).  These 7
// metafunctions and their C++11 equivalents are shown in the table below:
//..
//    +-----------------------------+------------------------------+
//    | MovableRefUtil trait        | C++11 standard trait         |
//    +-----------------------------+------------------------------+
//    | IsLvalueReference<t_TYPE>   | is_lvalue_reference<t_TYPE>  |
//    | IsMovableReference<t_TYPE>  | is_rvalue_reference<t_TYPE>  |
//    | IsReference<t_TYPE>         | is_reference<t_TYPE>         |
//    | RemoveReference<t_TYPE>     | remove_reference<t_TYPE>     |
//    | AddLvalueReference<t_TYPE>  | add_lvalue_reference<t_TYPE> |
//    | AddMovableReference<t_TYPE> | add_rvalue_reference<t_TYPE> |
//    | Decay<t_TYPE>               | decay<t_TYPE>                |
//    +-----------------------------+------------------------------+
//..
// Note that volatile-qualified 'MovableRef' objects are non-sensical; unlike
// const objects they do not occur "naturally" as a result of argument passing
// or template-argument deduction and there would be no reason for a program
// to create one on purpose.  In C++11, moreover, 'volatile MovableRef<T>' is
// an alias for 'T&& volatile', which is not a valid type.  The traits above,
// therefore, will fail to compile when instantiated with a volatile-qualified
// 'MovableRef'.  Note that, although 'volatile MovableRef<T>' doesn't make
// sense, 'MovableRef<volatile T>' is perfectly fine and are equivalent to
// 'volatile T&&'.
//
///Use of 'MovableRef<t_TYPE>' Parameters
///--------------------------------------
// There are a number of differences how 'MovableRef<t_TYPE>' parameters are
// handled between C++03 and C++11 implementations.  Due to the language
// differences there is no way to avoid these.  This component enables use of
// move semantics in both C++03 and C++11 when done right.  It doesn't try to
// make implementation of move semantics easier.  Here are some notes to keep
// in mind when using this component:
//
//: 1 When using a 't_TYPE&&' in a context where 't_TYPE' is deduced, the
//:   resulting reference does normally *not* refer to an object that can be
//:   moved from!  If 'bslmf::MovableRef<t_TYPE>' would deduce the type when
//:   using a C++11 implementation the name would be rather misleading.  Thus,
//:   the 't_TYPE' won't be deduced.  When using a C++03 the type /can/ be
//:   deduced.  However, a program depending on the 't_TYPE' being deduced from
//:   a 'bslmf::MovableRef<t_TYPE>' will not compile with a C++11
//:   implementation.
//:
//: 2 Returning 'MovableRef<t_TYPE>' (or 't_TYPE&&') from a function is almost
//:   always wrong.  In particular note that the same life-time issues apply to
//:   'MovableRef<t_TYPE>' as they do to references of objects: when returning
//:   a reference the object referred to cannot be on the stack, i.e.,
//:   returning a 'MovableRef<t_TYPE>' referring to a local variable or a
//:   by-value function parameter is certainly wrong.  Returning a
//:   'MovableRef<t_TYPE>' to a function parameter received as a reference type
//:   can be correct.
//:
//: 3 Using the argument of type 'MovableRef<t_TYPE>' directly in a function
//:   typically results in incorrect behavior either when using C++03 or when
//:   using C++11.  Instead, use these arguments together with
//:   'MovableRefUtil::move()', 'MovableRefUtil::access()', or bind them to a
//:   non-'const' l-value reference.
//
// The purpose of 'access(x)' is to use the same notation for member access to
// 'x' independent on whether it is an actual l-value reference or an
// 'MovableRef<t_TYPE>'.  For a concrete examples assume 'x' is a
// 'bsl::pair<A, B>'.  When using a C++11 implementation
// 'MovableRef<bsl::pair<A, B> >' is really just a 'bsl::pair<A, B>&&' and the
// elements could be accessed using 'x.first' and 'x.second'.  For a C++03
// implementation 'MovableRef<bsl::pair<A, B> >' is a class type and 'x.first'
// and 'x.second' are not available.  Instead, a reference to the pair needs to
// be obtained that could be done using 'static_cast<bsl::pair<A, B >&>(x)' or
// by using a named variable.  To unify the notation between the C++03 and
// C++11 implementation, simultaneously simplifying the C++03 use
// 'MovableRefUtil::access(x)' can be used.
//
///Template Deduction and Argument Forwarding
///------------------------------------------
// C++11 has two entirely different uses of the notation 'T&&':
//
//: 1 In contexts where the type 'T' is not deduced 'T&&' indicates an "rvalue
//:   reference".  The notation implies that the resources held by the
//:   referenced object can be reused, typically because the lifetime of the
//:   object is about to end.  An argument of type 'T&&' can bind to an rvalue
//:   of type 'T' or to an lvalue of type 'T' that has been explicitly "moved"
//:   by the caller.
//: 2 In contexts where the type 'T' is deduced 'T&&' indicates a "forwarding
//:   reference.  The argument can be either an rvalue or an lvalue and the
//:   called function can preserve the value category (rvalue or lvalue) when
//:   forwarding the reference to another function.
//
// The 'bslmf::MovableRef<T>' emulation of 'T&&' in C++03 works only for rvalue
// references, i.e., the first use of the notation.  The C++11 definition of
// 'bslmf::MovableRef<T>' is designed specifically to avoid deduction of 'T',
// thus preventing it from accidentally being used as a forwarding reference
// (which would have the wrong effect in C++03).
//
// For contexts where it is desirable to deduce 'T', the
// 'BSLMF_MOVABLEREF_DEDUCE' macro is provided.  When invoked like
// 'BSLMF_MOVABLEREF_DEDUCE(T)', this macro expands to 'bslmf::MovableRef<T>'
// in C++03, and a type alias to 'T&&' for which substitution fails if 'T&&'
// would be an lvalue reference in C++11 and later.  In both cases, the type
// 'T' is deducible, and substitution succeeds only if
// 'BSLMF_MOVABLEREF_DEDUCE(T)' deduces a movable reference.
//
///Usage
///-----
// There are two sides of move semantics:
//
//: 1 Classes or class templates that are _move-enabled_, i.e., which can
//:   transfer their internal representation to another object in some
//:   situations.  To become move-enabled a class needs to implement, at
//:   least, a move constructor.  It should probably also implement a move
//:   assignment operator.
//: 2 Users of a potentially move-enabled class may take advantage of moving
//:   objects by explicitly indicating that ownership of resources may be
//:   transferred.  When using C++11 the compiler can automatically detect
//:   some situations where it is safe to move objects but this features is
//:   not available with C++03.
//
// The usage example below demonstrate both use cases using a simplified
// version of 'std::Vector<T>'.  The class template is simplified to
// concentrate on the aspects relevant to 'bslmf::MovableRef<T>'.  Most of the
// operations are just normal implementations to create a container.  The last
// two operations described are using move operations.
//
// Assume we want to implement a class template similar to the standard library
// 'vector' facility.  First we declare the class template 'Vector<t_TYPE>'.
// The definition of the this class template is rather straight forward, and
// for simplicity a few trivial operations are implemented directly in the
// class definition:
//..
//  template <class t_TYPE>
//  class Vector
//  {
//      t_TYPE *d_begin;
//      t_TYPE *d_end;
//      t_TYPE *d_endBuffer;
//
//      static void swap(t_TYPE*& a, t_TYPE*& b);
//          // Swap the specified pointers 'a' and 'b'.
//
//    public:
//      Vector();
//          // Create an empty Vector.
//
//      Vector(bslmf::MovableRef<Vector> other);                    // IMPLICIT
//          // Create a Vector by transferring the content of the specified
//          // 'other'.
//
//      Vector(const Vector& other);
//          // Create a Vector by copying the content of the specified 'other'.
//
//      Vector& operator= (Vector other);
//          // Assign a Vector by copying the content of the specified 'other'
//          // and return a reference to this object.  Note that 'other' is
//          // passed by value to have the copy or move already be done, or
//          // even elided.  Within the body of the assignment operator the
//          // content of 'this' and 'other' are simply swapped.
//
//      ~Vector();
//          // Destroy the Vector's elements and release any allocated memory.
//
//      t_TYPE&       operator[](int index)    { return this->d_begin[index]; }
//          // Return a reference to the object at the specified 'index'.
//
//      const t_TYPE& operator[](int index) const
//                                             { return this->d_begin[index]; }
//          // Return a reference to the object at the specified 'index'.
//
//      t_TYPE     *begin()       { return this->d_begin; }
//          // Return a pointer to the first element.
//
//      const t_TYPE *begin() const { return this->d_begin; }
//          // Return a pointer to the first element.
//
//      int capacity() const { return int(this->d_endBuffer - this->d_begin); }
//          // Return the capacity of the Vector.
//
//      bool empty() const { return this->d_begin == this->d_end; }
//          // Return 'true' if the Vector is empty and 'false' otherwise.
//
//      t_TYPE     *end()       { return this->d_end; }
//          // Return a pointer to the end of the range.
//
//      const t_TYPE *end() const { return this->d_end; }
//          // Return a pointer to the end of the range.
//
//      void push_back(const t_TYPE& value);
//          // Append a copy of the specified 'value' to the Vector.
//
//      void push_back(bslmf::MovableRef<t_TYPE> value);
//          // Append an object moving the specified 'value' to the new
//          // location.
//
//      void reserve(int newCapacity);
//          // Reserve enough capacity to fit at least as many elements as
//          // specified by 'newCapacity'.
//
//      int size() const { return int(this->d_end - this->d_begin); }
//          // Return the size of the object.
//
//      void swap(Vector& other);
//          // Swap the content of the Vector with the specified 'other'.
//  };
//..
// The class stores pointers to the begin and the end of the elements as well
// as a pointer to the end of the allocated buffer.  If there are no elements,
// null pointers are stored.  There a number of accessors similar to the
// accessors used by 'std::Vector<t_TYPE>'.
//
// The default constructor creates an empty 'Vector<t_TYPE>' by simply
// initializing all member pointers to be null pointers:
//..
//  template <class t_TYPE>
//  Vector<t_TYPE>::Vector()
//      : d_begin()
//      , d_end()
//      , d_endBuffer()
//  {
//  }
//..
// To leverage already implemented functionality some of the member functions
// operate on a temporary 'Vector<t_TYPE>' and move the result into place using
// the 'swap()' member function that simply does a memberwise 'swap()' (the
// function swapping pointers is implemented here to avoid any dependency on
// functions defined in another level):
//..
//  template <class t_TYPE>
//  void Vector<t_TYPE>::swap(t_TYPE*& a, t_TYPE*& b)
//  {
//      t_TYPE *tmp = a;
//      a = b;
//      b = tmp;
//  }
//  template <class t_TYPE>
//  void Vector<t_TYPE>::swap(Vector& other)
//  {
//      this->swap(this->d_begin, other.d_begin);
//      this->swap(this->d_end, other.d_end);
//      this->swap(this->d_endBuffer, other.d_endBuffer);
//  }
//..
// The member function 'reserve()' arranges for the 'Vector<t_TYPE>' to have
// enough capacity for the number of elements specified as argument.  The
// function first creates an empty 'Vector<t_TYPE>' called 'tmp' and sets 'tmp'
// up to have enough capacity by allocating sufficient memory and assigning the
// different members to point to the allocated buffer.  The function then
// iterates over the elements of 'this' and for each element it constructs a
// new element in 'tmp'.
//..
//  template <class t_TYPE>
//  void Vector<t_TYPE>::reserve(int newCapacity)
//  {
//      if (this->capacity() < newCapacity) {
//          Vector tmp;
//          int    size = int(sizeof(t_TYPE) * newCapacity);
//          tmp.d_begin = static_cast<t_TYPE*>(operator new(size));
//          tmp.d_end = tmp.d_begin;
//          tmp.d_endBuffer = tmp.d_begin + newCapacity;
//
//          for (t_TYPE* it = this->d_begin; it != this->d_end; ++it) {
//              new (tmp.d_end) t_TYPE(*it);
//              ++tmp.d_end;
//          }
//          this->swap(tmp);
//      }
//  }
//..
// Any allocated data and constructed elements need to be release in the
// destructor.  The destructor does so by calling the destructor of the
// elements in the buffer from back to front.  Once the elements are destroyed
// the buffer is released:
//..
//  template <class t_TYPE>
//  Vector<t_TYPE>::~Vector()
//  {
//      if (this->d_begin) {
//          while (this->d_begin != this->d_end) {
//              --this->d_end;
//              this->d_end->~t_TYPE();
//          }
//          operator delete(this->d_begin);
//      }
//  }
//..
// Using 'reserve()' and constructing the elements it is straight forward to
// implement the copy constructor.  First the member pointers are initialed to
// null.  If 'other' is empty there is nothing further to do as it is desirable
// to not allocate a buffer for an empty 'Vector'.  If there are elements to
// copy the buffer is set up by calling 'reserve()' to create sufficient
// capacity.  Once that is done elements are copied by iterating over the
// elements of 'other' and constructing elements using placement new in the
// appropriate location.
//..
//  template <class t_TYPE>
//  Vector<t_TYPE>::Vector(const Vector& other)
//      : d_begin()
//      , d_end()
//      , d_endBuffer()
//  {
//      if (!other.empty()) {
//          this->reserve(4 < other.size()? other.size(): 4);
//
//          ASSERT(other.size() <= this->capacity());
//          for (t_TYPE* it = other.d_begin; it != other.d_end; ++it) {
//              new (this->d_end) t_TYPE(*it);
//              ++this->d_end;
//          }
//      }
//  }
//..
// A simple copy assignment operator can be implemented in terms of copy/move
// constructors, 'swap()', and destructor (in a real implementation the copy
// assignment would probably try to use already allocated objects).  In this
// implementation that argument is taken by value, i.e., the argument is
// already constructed using copy or move construction (which may have been
// elided), the content of 'this' is swapped with the content of 'other'
// leaving this in the desired state, and the destructor will release the
// former representation of 'this' when 'other' is destroyed':
//..
//  template <class t_TYPE>
//  Vector<t_TYPE>& Vector<t_TYPE>::operator= (Vector other)
//  {
//      this->swap(other);
//      return *this;
//  }
//..
// To complete the normal C++03 operations of 'Vector<t_TYPE>' the only
// remaining member function is 'push_back()'.  This function calls 'reserve()'
// to obtain more capacity if the current capacity is filled and then
// constructs the new element at the location pointed to by 'd_end':
//..
//  template <class t_TYPE>
//  void Vector<t_TYPE>::push_back(const t_TYPE& value)
//  {
//      if (this->d_end == this->d_endBuffer) {
//          this->reserve(this->size()? 2 * this->size() : 4);
//      }
//      assert(this->d_end != this->d_endBuffer);
//      new(this->d_end) t_TYPE(value);
//      ++this->d_end;
//  }
//..
// The first operation actually demonstrating the use of 'MovableRef<t_TYPE>'
// is the move constructor:
//..
//  template <class t_TYPE>
//  Vector<t_TYPE>::Vector(bslmf::MovableRef<Vector> other)
//      : d_begin(bslmf::MovableRefUtil::access(other).d_begin)
//      , d_end(bslmf::MovableRefUtil::access(other).d_end)
//      , d_endBuffer(bslmf::MovableRefUtil::access(other).d_endBuffer)
//  {
//      Vector& reference(other);
//      reference.d_begin = 0;
//      reference.d_end = 0;
//      reference.d_endBuffer = 0;
//  }
//..
// This constructor gets an 'MovableRef<Vector<t_TYPE> >' passed as argument
// that indicates that the referenced objects can be modified as long as it is
// left in a state meeting the class invariants.  The implementation of this
// constructor first copies the 'd_begin', 'd_end', and 'd_capacity' members of
// 'other'.  Since 'other' is either an object of type
// 'MovableRef<Vector<t_TYPE> >' (when compiling using a C++03 compiler) or an
// r-value reference 'Vector<t_TYPE>&&' the members are accessed using
// 'MovableRefUtil::access(other)' to get a reference to a 'Vector<t_TYPE>'.
// Within the body of the constructor an l-value reference is obtained either
// via the conversion operator of 'MovableRef<T>' or directly as 'other' is
// just an l-value when compiling with a C++11 compiler.  This reference is
// used to set the pointer members of the object referenced by 'other' to '0'
// completing the move of the content to the object under construction.
//
// Finally, a move version of 'push_back()' is provided: it takes an
// 'MovableRef<t_TYPE>' as argument.  The type of this argument indicates that
// the state can be transferred and after arranging enough capacity in the
// 'Vector<t_TYPE>' object a new element is move constructed at the position
// 'd_end':
//..
//  template <class t_TYPE>
//  void Vector<t_TYPE>::push_back(bslmf::MovableRef<t_TYPE> value)
//  {
//      if (this->d_end == this->d_endBuffer) {
//          this->reserve(this->size()? int(1.5 * this->size()): 4);
//      }
//      assert(this->d_end != this->d_endBuffer);
//      new(this->d_end) t_TYPE(bslmf::MovableRefUtil::move(value));
//      ++this->d_end;
//  }
//..
// Note that this implementation of 'push_back()' uses
// 'bslmf::MovableRefUtil::move(value)' to move the argument.  For a C++03
// implementation the argument would be moved even when using 'value' directly
// because the type of 'value' stays 'bslmf::MovableRef<t_TYPE>'.  However, for
// a C++11 implementation the argument 'value' is an l-value and using it
// directly would result in a copy.
//
// To demonstrate the newly created 'Vector<t_TYPE>' class in action, first a
// 'Vector<int>' is created and filled with a few elements:
//..
//  Vector<int> vector0;
//  for (int i = 0; i != 5; ++i) {
//      vector0.push_back(i);
//  }
//  for (int i = 0; i != 5; ++i) {
//      assert(vector0[i] == i);
//  }
//..
// To verify that copying of 'Vector<t_TYPE>' objects works, a copy is created:
//..
//  Vector<int> vector1(vector0);
//  assert(vector1.size() == 5);
//  assert(vector1.size() == vector0.size());
//  for (int i = 0; i != vector1.size(); ++i) {
//      assert(vector1[i] == i);
//      assert(vector1[i] == vector0[i]);
//  }
//..
// When using moving this 'vector0' to a new location the representation of the
// new object should use the original 'begin()':
//..
//  const int   *first = vector0.begin();
//  Vector<int>  vector2(bslmf::MovableRefUtil::move(vector0));
//  assert(first == vector2.begin());
//..
// When create a 'Vector<Vector<int> >' and using 'push_back()' on this object
// with 'vector2' a copy should be inserted:
//..
//  Vector<Vector<int> > vVector;
//  vVector.push_back(vector2);                          // copy
//  assert(vector2.size() == 5);
//  assert(vVector.size() == 1);
//  assert(vVector[0].size() == vector2.size());
//  assert(vVector[0].begin() != first);
//  for (int i = 0; i != 5; ++i) {
//      assert(vVector[0][i] == i);
//      assert(vector2[i] == i);
//  }
//..
// When adding another element by moving 'vector2' the 'begin()' of the newly
// inserted element will be the same as 'first', i.e., the representation is
// transferred:
//..
//  vVector.push_back(bslmf::MovableRefUtil::move(vector2)); // move
//  assert(vVector.size() == 2);
//  assert(vVector[1].begin() == first);
//  assert(vVector[1].size() == 5);
//..
// Compiling this code with both C++03 and C++11 compilers shows that there is
// no need for conditional compilation in when using 'MovableRef<t_TYPE>' while
// move semantics is enabled in both modes.
// ----------------------------------------------------------------------------

#include <bslscm_version.h>

#include <bslmf_addlvaluereference.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_decay.h>
#include <bslmf_enableif.h>
#include <bslmf_iscopyconstructible.h>
#include <bslmf_islvaluereference.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_isreference.h>
#include <bslmf_isrvaluereference.h>
#include <bslmf_removereference.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>
#include <bsls_util.h>

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) &&               \
    defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
#define BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES
// This macro indicates whether the component uses C++11 r-value references to
// implement 'bslmf::MovableRef<t_TYPE>'.  It will evaluate to 'false' for
// C++03 implementations and to 'true' for proper C++11 implementations.  For
// partial C++11 implementations it may evaluate to 'false' because both
// r-value reference and alias templates need to be supported.
#endif

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
#   define BSLMF_MOVABLEREF_DEDUCE(...)                                       \
        ::BloombergLP::bslmf::MovableRef_Deduced<__VA_ARGS__>
    // This macro expands to a movable reference to '__VA_ARGS__' for which the
    // arguments are deducible in all language versions.  Note that the
    // argument list of this macro is variadic in order to support deducing
    // template arguments, e.g., this macro supports uses like
    // 'BSLMF_MOVABLEREF_DEDUCE(bsl::pair<T1, T2>)' for which the  types 'T1'
    // and 'T2' are deducible, even though the macro argument contains a comma.
#else
#    define BSLMF_MOVABLEREF_DEDUCE(...)                                      \
        ::BloombergLP::bslmf::MovableRef<__VA_ARGS__>
#endif

namespace BloombergLP {
namespace bslmf {

struct MovableRefUtil;
    // forward declaration

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference;
    // forward declaration

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference;
    // forward declaration

template <class t_TYPE>
struct MovableRefUtil_Decay;
    // forward declaration

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits;
    // forward declaration

template <class t_TYPE>
struct MovableRefUtil_RemoveReference;
    // forward declaration

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

template <class t_TYPE>
struct MovableRef_Helper;
    // forward declaration

#endif

                               // ===============
                               // type MovableRef
                               // ===============

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

template <class t_TYPE>
using MovableRef = typename MovableRef_Helper<t_TYPE>::type;
    // The alias template 'MovableRef<t_TYPE>' yields an r-value reference of
    // type 't_TYPE&&'.

#else // if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

template <class t_TYPE>
class MovableRef {
    // The class template 'MovableRef<t_TYPE>' provides a reference to a
    // movable object of type 't_TYPE'.  Put differently, a function receiving
    // an object this class template can transfer (move) the representation to
    // a different object and leave the referenced object in an unspecified,
    // although valid (i.e., it obeys all class invariants), state.  With C++11
    // an r-value reference ('t_TYPE&&') is used to represent the same
    // semantics.

    // DATA
    t_TYPE *d_pointer;

    // PRIVATE CREATORS
    explicit MovableRef(t_TYPE *pointer);
        // Create an 'MovableRef<t_TYPE>' object referencing the object pointed
        // to by the specified 'pointer'.  The behavior is undefined if
        // 'pointer' does not point to an object.  This constructor is private
        // because a C++11 r-value reference cannot be created like this.  For
        // information on how to create objects of type 'MovableRef<t_TYPE>'
        // see 'MovableRefUtil::move()'.

    // FRIENDS
    friend struct MovableRefUtil;

  public:
    // ACCESSORS
    operator t_TYPE&() const;
        // Return a reference to the referenced object.  In contexts where a
        // reference to an object of type 't_TYPE' is needed, a
        // 'MovableRef<t_TYPE>' behaves like such a reference.  For information
        // on how to access the reference in contexts where no conversion can
        // be used see 'MovableRefUtil::access()'.
};

#endif // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

                            // =====================
                            // struct MovableRefUtil
                            // =====================

struct MovableRefUtil {
    // This 'struct' provides a collection of utility functions operating on
    // objects of type 'MovableRef<t_TYPE>'.  The primary use of these
    // utilities to create a consistent notation for using the C++03
    // 'MovableRef<t_TYPE>' objects and the C++11 't_TYPE&&' r-value
    // references.

  public:
    // TYPES
    template <class t_TYPE>
    struct IsLvalueReference
    : MovableRefUtil_PropertyTraits<t_TYPE>::IsLvalueReference {
        // This 'struct' template provides a Boolean metafunction that inherits
        // from 'bsl::true_type' if the specified 't_TYPE' is an lvalue
        // reference, and inherits from 'bsl::false_type' otherwise.
    };

    template <class t_TYPE>
    struct IsMovableReference
    : MovableRefUtil_PropertyTraits<t_TYPE>::IsMovableReference {
        // This 'struct' template provides a Boolean metafunction that inherits
        // from 'bsl::true_type' if the specified 't_TYPE' is a specialization
        // of 'MovableRef', and inherits from 'bsl::false_type' otherwise.
    };

    template <class t_TYPE>
    struct IsReference : MovableRefUtil_PropertyTraits<t_TYPE>::IsReference {
        // This 'struct' template provides a Boolean metafunction that inherits
        // from 'bsl::true_type' if the specified 't_TYPE' is either an lvalue
        // reference or a specialization of 'MovableRef', and inherits from
        // 'bsl::false_type' otherwise.
    };

    template <class t_TYPE>
    struct RemoveReference : MovableRefUtil_RemoveReference<t_TYPE> {
        // This 'struct' template provides a metafunction that, if the
        // specified 't_TYPE' is a reference type, defines a nested 'type'
        // typedef of the type to which 't_TYPE' refers, and defines a nested
        // 'type' typedef of 't_TYPE' otherwise.
    };

    template <class t_TYPE>
    struct AddLvalueReference : MovableRefUtil_AddLvalueReference<t_TYPE> {
        // This 'struct' template provides a metafunction that defines a nested
        // 'type' typedef that is an lvalue reference to 't_TYPE'.  If 't_TYPE'
        // is already an 'lvalue' reference, then 'type' is 't_TYPE'.
        // Otherwise, if 't_TYPE' is 'MovableRef<T2>', then 'type' is 'T2&'.
        // This transformation reflects the semantics of _reference collapsing_
        // in section [dec.ref] of the standard.
    };

    template <class t_TYPE>
    struct AddMovableReference : MovableRefUtil_AddMovableReference<t_TYPE> {
        // This 'struct' template provides a metafunction that defines a nested
        // 'type' typedef that, if 't_TYPE' is not a reference type, is
        // 'MovableRef<t_TYPE>'.  Otherwise, if 't_TYPE' is a specialization of
        // 'MovableRef', 'type' is the same as 't_TYPE'.  Otherwise, 'type' is
        // 't_TYPE&'.  This transformation reflects the semantics of
        // _reference collapsing_ in section [dec.ref] of the standard.
    };

    template <class t_TYPE>
    struct Decay : MovableRefUtil_Decay<t_TYPE> {
        // This 'struct' template provides a metafunction that defines a nested
        // 'type' typedef that applies lvalue-to-rvalue, array-to-pointer, and
        // function-to-pointer conversions that occur when an lvalue of type
        // 't_TYPE' is used as an rvalue, and also removes 'const', 'volatile',
        // and reference qualifiers from class types in order to model by-value
        // argument passing.  For the purpose of this type trait,
        // 'MovableRef<T>' is considered a (movable) reference-qualified 'T'.
        //
        // Formally, let 'U' be
        // 'MovableRefUtil::RemoveReference<t_TYPE>::type'.  If
        // 'bsl::is_array<U>::value' is true, the member typedef 'type' is
        // 'bsl::remove_extent<U>::type *'.  If 'bsl::is_function<U>::value' is
        // true, the member typedef 'type' is 'bsl::add_pointer<U>::type'.
        // Otherwise, the member typedef 'type' is 'bsl::remove_cv<U>::type'.
    };

    // CLASS METHODS
#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES
    template <class t_TYPE>
    static typename bsl::remove_reference<t_TYPE>::type& access(
                                           t_TYPE&& ref) BSLS_KEYWORD_NOEXCEPT;
#else
    template <class t_TYPE>
    static t_TYPE& access(t_TYPE& ref) BSLS_KEYWORD_NOEXCEPT;
    template <class t_TYPE>
    static t_TYPE& access(MovableRef<t_TYPE> ref) BSLS_KEYWORD_NOEXCEPT;
#endif
    // Return an lvalue reference to the object referenced by the specified
    // 'ref' object.  This function is used to provide a uniform interface to
    // members of an object reference by 'ref', regardless of whether 'ref' is
    // an 'MovableRef' or lvalue reference and whether the compiler supports
    // C++11 rvalue references.  This function is unnecessary (but allowed)
    // when simply converting 'ref' to 't_TYPE&'.
    //
    // Please see the component-level documentation for more information on
    // this function.

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES
    template <class t_TYPE>
    static BSLS_KEYWORD_CONSTEXPR
        MovableRef<typename bsl::remove_reference<t_TYPE>::type>
        move(t_TYPE&& reference) BSLS_KEYWORD_NOEXCEPT;
#else
    template <class t_TYPE>
    static MovableRef<t_TYPE> move(t_TYPE& reference) BSLS_KEYWORD_NOEXCEPT;
    template <class t_TYPE>
    static MovableRef<typename bsl::remove_reference<t_TYPE>::type> move(
                           MovableRef<t_TYPE> reference) BSLS_KEYWORD_NOEXCEPT;
#endif
    // Return a movable reference to the object referred to by the specified
    // 'reference'.  Note that the C++03 implementation of this function
    // behaves like a factory for 'MovableRef<t_TYPE>' objects.  The C++11
    // implementation of this function behaves exactly like 'std::move(value)'
    // applied to lvalues.

    template <class t_TYPE>
    static typename bsl::enable_if<
        !bsl::is_nothrow_move_constructible<t_TYPE>::value &&
            bsl::is_copy_constructible<t_TYPE>::value,
        const t_TYPE&>::type
    move_if_noexcept(t_TYPE& lvalue) BSLS_KEYWORD_NOEXCEPT
        // Return a const-qualified reference to the specified 'lvalue'.  This
        // function is selected by overload resolution if the move constructor
        // for 't_TYPE' might throw an exception.  Constructing a 't_TYPE'
        // object from the result will result in the copy constructor being
        // invoked rather than the (unsafe) move constructor.
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
        return lvalue;
    }

    template <class t_TYPE>
    static typename bsl::enable_if<
        !bsl::is_copy_constructible<t_TYPE>::value ||
            bsl::is_nothrow_move_constructible<t_TYPE>::value,
        MovableRef<t_TYPE> >::type
    move_if_noexcept(t_TYPE& lvalue) BSLS_KEYWORD_NOEXCEPT
        // Return a movable reference to the specified 'lvalue'.  This function
        // is selected by overload resolution if the move constructor for
        // 't_TYPE' is nothrow-move-constructible.  Constructing a 't_TYPE'
        // object from the result will result in the (safe) move constructor
        // being invoked.  Note that that the
        // 'bsl::is_nothrow_move_constructible' trait can be customized in
        // C++03 mode to indicate that a type is nothrow-move-constructible.
    {
        // The implementation is placed here in the class definition to work
        // around a Microsoft C++ compiler (version 16) bug where the
        // definition cannot be matched to the declaration when an 'enable_if'
        // is used.
#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES
        return static_cast<typename bsl::remove_reference<t_TYPE>::type&&>(
                                                                       lvalue);
#else
        return MovableRef<t_TYPE>(bsls::Util::addressOf(lvalue));
#endif
    }
};

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

                          // ========================
                          // struct MovableRef_Helper
                          // ========================

template <class t_TYPE>
struct MovableRef_Helper {
    // The class template 'MovableRef_Helper' just defines a nested type
    // 'type' that is used by an alias template.  Using this indirection the
    // template argument of the alias template is prevented from being deduced.

  public:
    // TYPES
    using type = t_TYPE&&;
        // The type 'type' defined to be an r-value reference to the argument
        // type of 'MovableRef_Helper.
};

                          // =======================
                          // type MovableRef_Deduced
                          // =======================

template <class t_TYPE,
          typename bsl::enable_if<!bsl::is_lvalue_reference<t_TYPE>::value,
                                  int>::type = 0>
using MovableRef_Deduced = t_TYPE&&;
    // This component-private alias template names the type 't_TYPE&&' if and
    // only if the specified 't_TYPE' is not an lvalue reference.

#endif // defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                              // ----------------
                              // class MovableRef
                              // ----------------

#ifndef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

// CREATORS
template <class t_TYPE>
inline
MovableRef<t_TYPE>::MovableRef(t_TYPE *pointer)
: d_pointer(pointer)
{
    BSLS_ASSERT(0 != pointer);
}

// ACCESSORS
template <class t_TYPE>
inline
MovableRef<t_TYPE>::operator t_TYPE&() const
{
    return *d_pointer;
}

#endif // defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

                           // ---------------------
                           // struct MovableRefUtil
                           // ---------------------

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

// CLASS METHODS
template <class t_TYPE>
inline
typename bsl::remove_reference<t_TYPE>::type& MovableRefUtil::access(
                                            t_TYPE&& ref) BSLS_KEYWORD_NOEXCEPT
{
    return ref;
}

#else // if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

template <class t_TYPE>
inline
t_TYPE& MovableRefUtil::access(t_TYPE& ref) BSLS_KEYWORD_NOEXCEPT
{
    return ref;
}

template <class t_TYPE>
inline
t_TYPE& MovableRefUtil::access(MovableRef<t_TYPE> ref) BSLS_KEYWORD_NOEXCEPT
{
    return ref;
}

#endif // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

template <class t_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR MovableRef<typename bsl::remove_reference<t_TYPE>::type>
MovableRefUtil::move(t_TYPE&& rvalue) BSLS_KEYWORD_NOEXCEPT
{
    return static_cast<typename bsl::remove_reference<t_TYPE>::type&&>(rvalue);
}

#else // if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

template <class t_TYPE>
inline
MovableRef<t_TYPE> MovableRefUtil::move(t_TYPE& lvalue) BSLS_KEYWORD_NOEXCEPT
{
    return MovableRef<t_TYPE>(bsls::Util::addressOf(lvalue));
}

template <class t_TYPE>
inline
MovableRef<typename bsl::remove_reference<t_TYPE>::type> MovableRefUtil::move(
                               MovableRef<t_TYPE> rvalue) BSLS_KEYWORD_NOEXCEPT
{
    return rvalue;
}

#endif // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

                    // ------------------------------------
                    // struct MovableRefUtil_PropertyTraits
                    // ------------------------------------

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits {
    // Component-private class: do not use.  Define Boolean-valued
    // movable-reference traits for the specified 't_TYPE'.

    // TYPES
    typedef bsl::is_lvalue_reference<t_TYPE> IsLvalueReference;
    typedef bsl::is_rvalue_reference<t_TYPE> IsMovableReference;
    typedef bsl::is_reference<t_TYPE>        IsReference;
};

#else // if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits {
    // Component-private class: do not use.  Define Boolean-valued
    // movable-reference traits for rvalues of the specified 't_TYPE'.

    typedef bsl::false_type IsLvalueReference;
    typedef bsl::false_type IsMovableReference;
    typedef bsl::false_type IsReference;
};

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits<t_TYPE&> {
    // Component-private class: do not use.  Define Boolean-valued
    // movable-reference traits for lvalues of the specified 't_TYPE'.

    typedef bsl::true_type  IsLvalueReference;
    typedef bsl::false_type IsMovableReference;
    typedef bsl::true_type  IsReference;
};

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits<MovableRef<t_TYPE> > {
    // Component-private class: do not use.  Define Boolean-valued
    // movable-reference traits for movable references to the specified
    // 't_TYPE'.

    typedef bsl::false_type IsLvalueReference;
    typedef bsl::true_type  IsMovableReference;
    typedef bsl::true_type  IsReference;
};

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits<MovableRef<t_TYPE>&>
: MovableRefUtil_PropertyTraits<MovableRef<t_TYPE> > {
};

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits<const MovableRef<t_TYPE> >
: MovableRefUtil_PropertyTraits<MovableRef<t_TYPE> > {
};

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits<const MovableRef<t_TYPE>&>
: MovableRefUtil_PropertyTraits<MovableRef<t_TYPE> > {
};

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits<volatile MovableRef<t_TYPE> >;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits<volatile MovableRef<t_TYPE>&>;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits<const volatile MovableRef<t_TYPE> >;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_PropertyTraits<const volatile MovableRef<t_TYPE>&>;
    // This partial 'struct' template specialization is not defined.

#endif // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

                    // -------------------------------------
                    // struct MovableRefUtil_RemoveReference
                    // -------------------------------------

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

template <class t_TYPE>
struct MovableRefUtil_RemoveReference : bsl::remove_reference<t_TYPE> {
    // This component-private 'struct' template provides a metafunction that,
    // if the specified 't_TYPE' is a reference type, defines a nested 'type'
    // typedef of the type to which 't_TYPE' refers, and defines a nested
    // 'type' typedef of 't_TYPE' otherwise.
};

#else // if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

template <class t_TYPE>
struct MovableRefUtil_RemoveReference {
    // TYPES
    typedef t_TYPE type;
};

template <class t_TYPE>
struct MovableRefUtil_RemoveReference<t_TYPE&> {
    // TYPES
    typedef t_TYPE type;
};

template <class t_TYPE>
struct MovableRefUtil_RemoveReference<bslmf::MovableRef<t_TYPE> > {
    // TYPES
    typedef t_TYPE type;
};

template <class t_TYPE>
struct MovableRefUtil_RemoveReference<bslmf::MovableRef<t_TYPE>&> {
    // TYPES
    typedef t_TYPE type;
};

template <class t_TYPE>
struct MovableRefUtil_RemoveReference<const bslmf::MovableRef<t_TYPE> > {
    // TYPES
    typedef t_TYPE type;
};

template <class t_TYPE>
struct MovableRefUtil_RemoveReference<const bslmf::MovableRef<t_TYPE>&> {
    // TYPES
    typedef t_TYPE type;
};

template <class t_TYPE>
struct MovableRefUtil_RemoveReference<volatile MovableRef<t_TYPE> >;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_RemoveReference<volatile MovableRef<t_TYPE>&>;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_RemoveReference<const volatile MovableRef<t_TYPE> >;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_RemoveReference<const volatile MovableRef<t_TYPE>&>;
    // This partial 'struct' template specialization is not defined.

#endif // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

                  // ----------------------------------------
                  // struct MovableRefUtil_AddLvalueReference
                  // ----------------------------------------

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference : bsl::add_lvalue_reference<t_TYPE> {
    // This component-private 'struct' template provides a metafunction that
    // defines a nested 'type' typedef that is an lvalue reference to 't_TYPE'.
    // If 't_TYPE' is already an 'lvalue' reference, then 'type' is 't_TYPE'.
    // Otherwise, if 't_TYPE' is 'MovableRef<T2>', then 'type' is 'T2&'.  This
    // transformation reflects the semantics of _reference collapsing_ in
    // section [dec.ref] of the standard.
};

#else // if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

template <>
struct MovableRefUtil_AddLvalueReference<void> {
    // TYPES
    typedef void type;
};

template <>
struct MovableRefUtil_AddLvalueReference<const void> {
    // TYPES
    typedef const void type;
};

template <>
struct MovableRefUtil_AddLvalueReference<volatile void> {
    // TYPES
    typedef volatile void type;
};

template <>
struct MovableRefUtil_AddLvalueReference<const volatile void> {
    // TYPES
    typedef const volatile void type;
};

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference {
    // TYPES
    typedef t_TYPE& type;
};

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference<t_TYPE&> {
    // TYPES
    typedef t_TYPE& type;
};

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference<MovableRef<t_TYPE> > {
    // TYPES
    typedef t_TYPE& type;
};

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference<MovableRef<t_TYPE>&> {
    // TYPES
    typedef t_TYPE& type;
};

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference<const MovableRef<t_TYPE> > {
    // TYPES
    typedef t_TYPE& type;
};

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference<const MovableRef<t_TYPE>&> {
    // TYPES
    typedef t_TYPE& type;
};

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference<volatile MovableRef<t_TYPE> >;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference<volatile MovableRef<t_TYPE>&>;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference<const volatile MovableRef<t_TYPE> >;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_AddLvalueReference<const volatile MovableRef<t_TYPE>&>;
    // This partial 'struct' template specialization is not defined.

#endif // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

                  // -----------------------------------------
                  // struct MovableRefUtil_AddMovableReference
                  // -----------------------------------------

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference : bsl::add_rvalue_reference<t_TYPE> {
    // This component-private 'struct' template provides a metafunction that
    // defines a nested 'type' typedef that, if 't_TYPE' is not a reference
    // type, is 'MovableRef<t_TYPE>'.  Otherwise, if 't_TYPE' is a
    // specialization of 'MovableRef', 'type' is the same as 't_TYPE'.
    // Otherwise, 'type' is 't_TYPE&'.  This transformation reflects the
    // semantics of
    // _reference collapsing_ in section [dec.ref] of the standard.
};

#else // if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

template <>
struct MovableRefUtil_AddMovableReference<void> {
    // TYPES
    typedef void type;
};

template <>
struct MovableRefUtil_AddMovableReference<const void> {
    // TYPES
    typedef const void type;
};

template <>
struct MovableRefUtil_AddMovableReference<volatile void> {
    // TYPES
    typedef volatile void type;
};

template <>
struct MovableRefUtil_AddMovableReference<const volatile void> {
    // TYPES
    typedef const volatile void type;
};

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference {
    // TYPES
    typedef bslmf::MovableRef<t_TYPE> type;
};

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference<t_TYPE&> {
    // TYPES
    typedef t_TYPE& type;
};

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference<MovableRef<t_TYPE> > {
    // TYPES
    typedef bslmf::MovableRef<t_TYPE> type;
};

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference<MovableRef<t_TYPE>&> {
    // TYPES
    typedef bslmf::MovableRef<t_TYPE> type;
};

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference<const MovableRef<t_TYPE> > {
    // TYPES
    typedef bslmf::MovableRef<t_TYPE> type;
};

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference<const MovableRef<t_TYPE>&> {
    // TYPES
    typedef bslmf::MovableRef<t_TYPE> type;
};

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference<volatile MovableRef<t_TYPE> >;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference<volatile MovableRef<t_TYPE>&>;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference<const volatile MovableRef<t_TYPE> >;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_AddMovableReference<const volatile MovableRef<t_TYPE>&>;
    // This partial 'struct' template specialization is not defined.

#endif // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

                         // ---------------------------
                         // struct MovableRefUtil_Decay
                         // ---------------------------

#ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES

template <class t_TYPE>
struct MovableRefUtil_Decay : bsl::decay<t_TYPE> {
    // This component-private 'struct' template provides a metafunction that
    // defines a nested 'type' typedef that applies lvalue-to-rvalue,
    // array-to-pointer, and function-to-pointer conversions that occur when an
    // lvalue of type 't_TYPE' is used as an rvalue, and also removes 'const',
    // 'volatile', and reference qualifiers from class types in order to model
    // by-value argument passing.  For the purpose of this type trait,
    // 'MovableRef<T>' is considered a (movable) reference-qualified 'T'.
    //
    // Formally, let 'U' be 'MovableRefUtil::RemoveReference<t_TYPE>::type'.
    // If 'bsl::is_array<U>::value' is true, the member typedef 'type' is
    // 'bsl::remove_extent<U>::type *'.  If 'bsl::is_function<U>::value' is
    // true, the member typedef 'type' is 'bsl::add_pointer<U>::type'.
    // Otherwise, the member typedef 'type' is 'bsl::remove_cv<U>::type'.
};

#elif !defined(BSLS_PLATFORM_CMP_IBM) || BSLS_PLATFORM_CMP_VERSION > 4097

///Implementation Note
///- - - - - - - - - -
// The following definition of 'MovableRefUtil_Decay' is for C++03 compilers,
// *except* versions of IBM XL C++ prior to 16.0.2.

template <class t_TYPE>
struct MovableRefUtil_Decay
: bsl::decay<typename MovableRefUtil_RemoveReference<t_TYPE>::type> {
};

#else

///Implementation Note
///- - - - - - - - - -
// The following implementation of 'MovableRefUtil_Decay' is exclusively for
// the IBM XL C++ line of compilers prior to version 16.0.2.  This line of
// compilers has a defect in which the compiler retains default-argument
// information in the type of functions that have default arguments.  If the
// program attempts to form a typedef to the type of a function with default
// arguments, the IBM XL C++ compiler rejects the typedef because it attempts
// to form it with default arguments.  Since default arguments cannot be
// specified anywhere except function declarations, compilation then fails.
//
// For example, the following code will be rejected:
//..
//  void f(int = 0);
//
//  template <class t_TYPE>
//  void g(t_TYPE)
//  {
//      typedef t_TYPE type; // ERROR: Default arguments cannot be specified
//                         //        in a 'typedef'.
//  }
//
//  void example()
//  {
//      g(f);              // ERROR: From the 'g' template instantiation here.
//  }
//..
// However, typedefs to *pointers* to such function types do not run afoul of
// this defect.  Fortunately, the specification of 'decay' requires
// transforming function types and reference-to-function types to
// pointer-to-function types.  So, with some careful metaprogramming, the
// following implementation avoids triggering this defect.

template <class t_TYPE, bool t_IS_FUNCTION = bsl::is_function<t_TYPE>::value>
struct MovableRefUtil_DecayImp;

template <class t_TYPE>
struct MovableRefUtil_DecayImp<t_TYPE, /* t_IS_FUNCTION */ false>
: bsl::decay<t_TYPE> {
};

template <class t_TYPE>
struct MovableRefUtil_DecayImp<t_TYPE, /* t_IS_FUNCTION */ true> {
    typedef t_TYPE *type;
};

template <class t_TYPE>
struct MovableRefUtil_Decay : MovableRefUtil_DecayImp<t_TYPE> {
};

template <class t_TYPE>
struct MovableRefUtil_Decay<t_TYPE&> : MovableRefUtil_DecayImp<t_TYPE> {
};

template <class t_TYPE>
struct MovableRefUtil_Decay<MovableRef<t_TYPE> >
: MovableRefUtil_DecayImp<t_TYPE> {
};

template <class t_TYPE>
struct MovableRefUtil_Decay<MovableRef<t_TYPE>&>
: MovableRefUtil_DecayImp<t_TYPE> {
};

template <class t_TYPE>
struct MovableRefUtil_Decay<const MovableRef<t_TYPE> >
: MovableRefUtil_DecayImp<t_TYPE> {
};

template <class t_TYPE>
struct MovableRefUtil_Decay<const MovableRef<t_TYPE>&>
: MovableRefUtil_DecayImp<t_TYPE> {
};

template <class t_TYPE>
struct MovableRefUtil_Decay<volatile MovableRef<t_TYPE> >;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_Decay<volatile MovableRef<t_TYPE>&>;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_Decay<const volatile MovableRef<t_TYPE> >;
    // This partial 'struct' template specialization is not defined.

template <class t_TYPE>
struct MovableRefUtil_Decay<const volatile MovableRef<t_TYPE>&>;
    // This partial 'struct' template specialization is not defined.

#endif // !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
