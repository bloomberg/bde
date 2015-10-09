// bslmf_movableref.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_MOVABLEREF
#define INCLUDED_BSLMF_MOVABLEREF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a vocabulary type to enable move semantics.
//
//@CLASSES:
//  bslmf::MovableRef: a template indicating that an object can be moved from
//  bslmf::MovableRefUtil: a namespace for functions dealing with movables
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
// object is called /moving an object/.  The purpose of 'bslmf::MoveableRef<T>'
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
//
///Use of 'MovableRef<TYPE>' Parameters
///------------------------------------
// There are a number of differences how 'MovableRef<TYPE>' parameters are
// handled between C++03 and C++11 implementations.  Due to the language
// differences there is no way to avoid these.  This component enables use of
// move semantics in both C++03 and C++11 when done right.  It doesn't try to
// make implementation of move semantics easier.  Here are some notes to keep
// in mind when using this component:
//
//: 1 When using a 'TYPE&&' in a context where 'TYPE' is deduced, the resulting
//:   reference does normally *not* refer to an object that can be moved from!
//:   If 'bslmf::MovableRef<TYPE>' would deduce the type when using a C++11
//:   implementation the name would be rather misleading.  Thus, the 'TYPE'
//:   won't be deduced.  When using a C++03 the type /can/ be deduced.
//:   However, a program depending on the 'TYPE' being deduced from a
//:   'bslmf::MovableRef<TYPE>' will not compile with a C++11 implementation.
//:
//: 2 Returning 'MovableRef<TYPE>' (or 'TYPE&&') from a function is almost
//:   always wrong.  In particular note that the same life-time issues apply to
//:   'MovableRef<TYPE>' as they do to references of objects: when returning a
//:   reference the object referred to cannot be on the stack, i.e., returning
//:   a 'MovableRef<TYPE>' refering to a local variable or a by-value function
//:   parameter is certainly wrong.  Returning a 'MovableRef<TYPE>' to a
//:   function parameter received as a reference type can be correct.
//:
//: 3 Using the argument of type 'MovableRef<TYPE>' directly in a function
//:   typically results in incorrect behavior either when using C++03 or when
//:   using C++11.  Instead, use these arguments together with
//:   'MovableRefUtil::move()', 'MovableRefUtil::access()', or bind them to a
//:   non-'const' l-value reference.
//
// The purpose of 'access(x)' is to use the same notation for member access to
// 'x' independent on whether it is an actual l-value reference or an
// 'MovableRef<TYPE>'.  For a concrete examples assume 'x' is a
// 'bsl::pair<A, B>'.  When using a C++11 implementation
// 'MovableRef<bsl::pair<A, B> >' is really just a 'bsl::pair<A, B>&&' and the
// elements could be accessed using 'x.first' and 'x.second'.  For a C++03
// implementation 'MovableRef<bsl::pair<A, B> >' is a class type and 'x.first'
// and 'x.second' are not available.  Instead, a reference to the pair needs to
// be obtained which could be done using 'static_cast<bsl::pair<A, B >&>(x)' or
// by using a named variable.  To unify the notation between the C++03 and
// C++11 implementation, simultanously simplifying the C++03 use
// 'MovableRefUtil::access(x)' can be used.
//
///Template Deduction and Argument Forwarding
///------------------------------------------
// C++11 has two entirely different uses of the notation 'T&&':
//
//: 1 In contexts where the type 'T' is not deduced the notation implies that
//:   only an r-value can be bound to the corresponding reference.  For this
//:   use 'T&&' is truly an r-value reference.
//: 2 In contexts where the type 'T' is deduced the notation implies that the
//:   type 'T' will include information on whether the entity bound is an
//:   r-value or an l-value.
//
// The use of 'bslmf::MovableRef<T>' and 'T&&' can only indicate that the
// reference is refering to an object whose content can be transferred if the
// type 'T' is not deduced.  Also, the C++03 implementation cannot distinguish
// between r-value and l-value, i.e., the component should be used only in
// contexts where 'T' is not deduced.
//
///Usage
///-----
// There are two sides of move semantics:
//
//: 1 Classes or class templates that are _move-enabled_, i.e., which can
//:   transfer their internal representation to another object in some
//:   situations.  To become move-enabled a class needs to implement, at
//:   least, a move constructor.  It should probably also implement a move
//:   assignment.
//: 2 Users of a potentially move-enabled class may take advantage of moving
//:   objects by explicitly indicating that ownership of resources may be
//:   transferred.  When using C++11 the compiler can automatically detect
//:   some situations where it is safe to move objects but this features is
//:   not available with C++03.
//
// The usage example below demonstrate both use cases using a simplified
// version of 'std::vector<T>'.  The class template is simplified to
// concentrate on the aspects relevant to 'bslmf::MovableRef<T>'.  Most of the
// operations are just normal implementations to create a container.  The last
// two operations described are using move operations.
//
// The definition of the 'vector<TYPE>' class template is rather straight
// forward.  For simplicity a few trivial operations are implemented directly
// in the class definition:
//..
//  template <class TYPE>
//  class vector
//  {
//      TYPE *d_begin;
//      TYPE *d_end;
//      TYPE *d_endBuffer;
//
//      static void swap(TYPE*& a, TYPE*& b);
//          // Swap the specified pointers 'a' and 'b'.
//
//    public:
//      vector();
//          // Create an empty vector.
//
//      vector(bslmf::MovableRef<vector> other);                    // IMPLICIT
//          // Create a vector by transfering the content of the specified
//          // 'other'.
//
//      vector(const vector& other);
//          // Create a vector by copying the content of the specified 'other'.
//
//      vector& operator= (vector other);
//          // Assign a vector by copying the content of the specified 'other'
//          // and return a reference to this object.  Note that 'other' is
//          // passed by value to have the copy or move already be done, or
//          // even elided.  Within the body of the assignment operator the
//          // content of 'this' and 'other' are simply swapped.
//
//      ~vector();
//          // Destroy the vector's elements and release any allocated memory.
//
//      TYPE&       operator[](int index)      { return this->d_begin[index]; }
//          // Return a reference to the object at the specified 'index'.
//
//      const TYPE& operator[](int index) const{ return this->d_begin[index]; }
//          // Return a reference to the object at the specified 'index'.
//
//      TYPE       *begin()       { return this->d_begin; }
//          // Return a pointer to the first element.
//
//      const TYPE *begin() const { return this->d_begin; }
//          // Return a pointer to the first element.
//
//      int capacity() const { return int(this->d_endBuffer - this->d_begin); }
//          // Return the capacity of the vector.
//
//      bool empty() const { return this->d_begin == this->d_end; }
//          // Return 'true' if the vector is empty and 'false' otherwise.
//
//      TYPE       *end()       { return this->d_end; }
//          // Return a pointer to the end of the range.
//
//      const TYPE *end() const { return this->d_end; }
//          // Return a pointer to the end of the range.
//
//      void push_back(const TYPE& value);
//          // Append a copy of the specified 'value' to the vector.
//
//      void push_back(bslmf::MovableRef<TYPE> value);
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
//      void swap(vector& other);
//          // Swap the content of the vector with the specified 'other'.
//  };
//..
// The class stores pointers to the begin and the end of the elements as well
// as a pointer to the end of the allocated buffer.  If there are no elements,
// null pointers are stored.  There a number of accessors similar to the
// accessors used by 'std::vector<TYPE>'.
//
// The default constructor creates an empty 'vector<TYPE>' by simply
// initializing all member pointers to be null pointers:
//..
//  template <class TYPE>
//  vector<TYPE>::vector()
//      : d_begin()
//      , d_end()
//      , d_endBuffer()
//  {
//  }
//..
// To leverage already implemented functionality some of the member functions
// operate on a temporary 'vector<TYPE>' and move the result into place using
// the 'swap()' member function that simply does a memberwise 'swap()' (the
// function swapping pointers is implemented here to avoid any dependency on
// functions defined in another level):
//..
//  template <class TYPE>
//  void vector<TYPE>::swap(TYPE*& a, TYPE*& b)
//  {
//      TYPE *tmp = a;
//      a = b;
//      b = tmp;
//  }
//  template <class TYPE>
//  void vector<TYPE>::swap(vector& other)
//  {
//      this->swap(this->d_begin, other.d_begin);
//      this->swap(this->d_end, other.d_end);
//      this->swap(this->d_endBuffer, other.d_endBuffer);
//  }
//..
// The member function 'reserve()' arranges for the 'vector<TYPE>' to have
// enough capacity for the number of elements specified as argument.  The
// function first creates an empty 'vector<TYPE>' called 'tmp' and sets 'tmp'
// up to have enough capacity by allocating sufficient memory and assigning the
// different members to point to the allocated buffer.  The function then
// iterates over the elements of 'this' and for each element it constructs a
// new element in 'tmp'.
//..
//  template <class TYPE>
//  void vector<TYPE>::reserve(int newCapacity)
//  {
//      if (this->capacity() < newCapacity) {
//          vector tmp;
//          int    size = int(sizeof(TYPE) * newCapacity);
//          tmp.d_begin = static_cast<TYPE*>(operator new(size));
//          tmp.d_end = tmp.d_begin;
//          tmp.d_endBuffer = tmp.d_begin + newCapacity;
//
//          for (TYPE* it = this->d_begin; it != this->d_end; ++it) {
//              new (tmp.d_end) TYPE(*it);
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
//  template <class TYPE>
//  vector<TYPE>::~vector()
//  {
//      if (this->d_begin) {
//          while (this->d_begin != this->d_end) {
//              --this->d_end;
//              this->d_end->~TYPE();
//          }
//          operator delete(this->d_begin);
//      }
//  }
//..
// Using 'reserve()' and constructing the elements it is straight forward to
// implement the copy constructor.  First the member pointers are initialed to
// null.  If 'other' is empty there is nothing further to do as it is desirable
// to not allocate a buffer for an empty 'vector'.  If there are elements to
// copy the buffer is set up by calling 'reserve()' to create sufficient
// capacity.  Once that is done elements are copied by iterating over the
// elements of 'other' and constructing elements using placement new in the
// appropriate location.
//..
//  template <class TYPE>
//  vector<TYPE>::vector(const vector& other)
//      : d_begin()
//      , d_end()
//      , d_endBuffer()
//  {
//      if (!other.empty()) {
//          this->reserve(4 < other.size()? other.size(): 4);
//
//          ASSERT(other.size() <= this->capacity());
//          for (TYPE* it = other.d_begin; it != other.d_end; ++it) {
//              new (this->d_end) TYPE(*it);
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
//  template <class TYPE>
//  vector<TYPE>& vector<TYPE>::operator= (vector other)
//  {
//      this->swap(other);
//      return *this;
//  }
//..
// To complete the normal C++03 operations of 'vector<TYPE>' the only remaining
// member function is 'push_back()'.  This function calls 'reserve()' to obtain
// more capacity if the current capacity is filled and then constructs the new
// element at the location pointed to by 'd_end':
//..
//  template <class TYPE>
//  void vector<TYPE>::push_back(const TYPE& value)
//  {
//      if (this->d_end == this->d_endBuffer) {
//          this->reserve(this->size()? int(1.5 * this->size()): 4);
//      }
//      assert(this->d_end != this->d_endBuffer);
//      new(this->d_end) TYPE(value);
//      ++this->d_end;
//  }
//..
// The first operation actually demonstrating the use of 'MovableRef<TYPE>' is
// the move constructor:
//..
//  template <class TYPE>
//  vector<TYPE>::vector(bslmf::MovableRef<vector> other)
//      : d_begin(bslmf::MovableRefUtil::access(other).d_begin)
//      , d_end(bslmf::MovableRefUtil::access(other).d_end)
//      , d_endBuffer(bslmf::MovableRefUtil::access(other).d_endBuffer)
//  {
//      vector& reference(other);
//      reference.d_begin = 0;
//      reference.d_end = 0;
//      reference.d_endBuffer = 0;
//  }
//..
// This constructor gets an 'MovableRef<vector<TYPE> >' passed as argument that
// indicates that the referenced objects can be modified as long as it is left
// in a state meeting the class invariants.  The implementation of this
// constructor first copies the 'd_begin', 'd_end', and 'd_capacity' members of
// 'other'.  Since 'other' is either an object of type
// 'MovableRef<vector<TYPE> >' (when compiling using a C++03 compiler) or an
// r-value reference 'vector<TYPE>&&' the members are accessed using
// 'MovableRefUtil::access(other)' to get a reference to a 'vector<TYPE>'.
// Within the body of the constructor an l-value reference is obtained either
// via the conversion operator of 'MovableRef<T>' or directly as 'other' is
// just an l-value when compiling with a C++11 compiler.  This reference is
// used to set the pointer members of the object referenced by 'other' to '0'
// completing the move of the content to the object under construction.
//
// Finally, a move version of 'push_back()' is provided: it takes an
// 'MovableRef<TYPE>' as argument.  The type of this argument indicates that
// the state can be transferred and after arranging enough capacity in the
// 'vector<TYPE>' object a new element is move constructed at the position
// 'd_end':
//..
//  template <class TYPE>
//  void vector<TYPE>::push_back(bslmf::MovableRef<TYPE> value)
//  {
//      if (this->d_end == this->d_endBuffer) {
//          this->reserve(this->size()? int(1.5 * this->size()): 4);
//      }
//      assert(this->d_end != this->d_endBuffer);
//      new(this->d_end) TYPE(bslmf::MovableRefUtil::move(value));
//      ++this->d_end;
//  }
//..
// Note that this implementation of 'push_back()' uses
// 'bslmf::MovableRefUtil::move(value)' to move the argument.  For a C++03
// implementation the argument would be moved even when using 'value' directly
// because the type of 'value' stays 'bslmf::MovableRef<TYPE>'.  However, for a
// C++11 implementation the argument 'value' is an l-value and using it
// directly would result in a copy.
//
// To demonstrate the newly created 'vector<TYPE>' class in action, first a
// 'vector<int>' is created and filled with a few elements:
//..
//  vector<int> vector0;
//  for (int i = 0; i != 5; ++i) {
//      vector0.push_back(i);
//  }
//  for (int i = 0; i != 5; ++i) {
//      assert(vector0[i] == i);
//  }
//..
// To verify that copying of 'vector<TYPE>' objects works, a copy is created:
//..
//  vector<int> vector1(vector0);
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
//  const int *first = vector0.begin();
//  vector<int> vector2(bslmf::MovableRefUtil::move(vector0));
//  assert(first == vector2.begin());
//..
// When create a 'vector<vector<int> >' and using 'push_back()' on this object
// with 'vector2' a copy should be inserted:
//..
//  vector<vector<int> > vvector;
//  vvector.push_back(vector2);                          // copy
//  assert(vector2.size() == 5);
//  assert(vvector.size() == 1);
//  assert(vvector[0].size() == vector2.size());
//  assert(vvector[0].begin() != first);
//  for (int i = 0; i != 5; ++i) {
//      assert(vvector[0][i] == i);
//      assert(vector2[i] == i);
//  }
//..
// When adding another element by moving 'vector2' the 'begin()' of the newly
// inserted element will be the same as 'first', i.e., the representation is
// transferred:
//..
//  vvector.push_back(bslmf::MovableRefUtil::move(vector2)); // move
//  assert(vvector.size() == 2);
//  assert(vvector[1].begin() == first);
//  assert(vvector[1].size() == 5);
//..
// Compiling this code with both C++03 and C++11 compilers shows that there is
// no need for conditional compilation in when using 'MovableRef<TYPE>' while
// move semantics is enabled in both modes.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

namespace BloombergLP {

namespace bslmf {

// ----------------------------------------------------------------------------

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) \
         && defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
#    define BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES
    // This macro indicates whether the component uses C++11 r-value references
    // to implement 'bslmf::MovableRef<TYPE>'.  It will evaluate to 'false' for
    // C++03 implementations and to 'true' for proper C++11 implementations.
    // For partial C++11 implementations it may evaluate to 'false' because
    // both r-value reference and alias templates need to be supported.
#endif

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

template <class TYPE>
struct MovableRef_Helper {
    // The class template 'MovableRef_Helper' just defines a nested type
    // 'type' that is used by an alias template.  Using this indirection the
    // template argument of the alias template is prevented from being deduced.
    using type = TYPE&&;
        // The type 'type' defined to be an r-value reference to the argument
        // type of 'MovableRef_Helper.
};

template <class TYPE>
using MovableRef = typename MovableRef_Helper<TYPE>::type;
    // The alias template 'MovableRef<TYPE>' yields an r-value reference of
    // type 'TYPE&&'.

#else // support r-value references and alias templates

struct MovableRefUtil;

template <class TYPE>
class MovableRef {
    // The class template 'MovableRef<TYPE>' provides a reference to a movable
    // object of type 'TYPE'.  Put differently, a function receiving an object
    // this class template can transfer (move) the representation to a
    // different object and leave the referenced object in an unspecified,
    // although valid (i.e., it obeys all class invariants), state.  With
    // C++11 an r-value reference ('TYPE&&') is used to represent the same
    // semantics.
    friend struct MovableRefUtil;
    TYPE *d_pointer;

    // PRIVATE CONSTRUCTORS
    explicit MovableRef(TYPE *pointer);
        // Create an 'MovableRef<TYPE>' object referencing the object pointed
        // to by the specified 'pointer'.  The behavior is undefined if
        // 'pointer' does not point to an object.  This constructor is private
        // because a C++11 r-value reference cannot be created like this.  For
        // information on how to create objects of type 'MovableRef<TYPE>' see
        // 'MovableRefUtil::move()'.

  public:
    // ACCESSORS
    operator TYPE&() const;
        // Return a reference to the referenced object.  In contexts where a
        // reference to an object of type 'TYPE' is needed, a
        // 'MovableRef<TYPE>' behaves like such a reference.  For information
        // on how to access the the reference in contexts where no conversion
        // can be used see 'MovableRefUtil::access()'.
};

#endif // support r-value references and alias templates

// ----------------------------------------------------------------------------

struct MovableRefUtil {
    // This 'struct' provides a collection of utility functions operating on
    // objects of type 'MovableRef<TYPE>'.  The primary use of these utilities
    // to create a consistent notation for using the C++03 'MovableRef<TYPE>'
    // objects and the C++11 'TYPE&&' r-value references.

    template <class TYPE>
    static TYPE& access(TYPE& lvalue);
        // Return a reference to the specified 'lvalue'.  This overload of
        // access is used when accessing an argument passed by
        // 'bslma::MovableRef<TYPE>' with a C++11 implementation: the 'TYPE&&'
        // argument is a movable l-value.
        //
        // Please see the component-level documentation for more information on
        // this function.

    template <class TYPE>
    static TYPE& access(MovableRef<TYPE>& lvalue);
        // Return a reference to the object referenced by the specified
        // 'lvalue' object.  This reference might be obtained by a conversion
        // of 'lvalue' to 'TYPE&' in contexts where a conversion is viable.
        // When a conversion isn't applicable, e.g., when caling a member of
        // 'TYPE', the reference can be accessed using 'access()'.  Since the
        // same notation should be applicable to the C++03 'MovableRef<TYPE>'
        // objects and a C++11 r-value reference 'TYPE&&', a member function
        // cannot be used directly.  Note that this overload will never deduce
        // the argument with a C++11 implementation.
        //
        // Please see the component-level documentation for more information on
        // this function.

#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    template <class TYPE>
    static MovableRef<TYPE> move(TYPE& lvalue);
        // Return a movable reference of type 'MovableRef<TYPE>' from the
        // specified 'lvalue'.  For a C++03 implementation this function
        // behaves like a factory for 'MovableRef<TYPE> objects.  For a C++11
        // implementation this function behaves exactly like 'std::move(value)'
        // applied to l-values.
#endif

    template <class TYPE>
    static MovableRef<typename bsl::remove_reference<TYPE>::type>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
        move(TYPE&& reference);
#else  // support r-value references and alias templates
        move(MovableRef<TYPE> reference);
#endif // support r-value references and alias templates
        // Return a movable reference to the object referred to by the
        // specified 'reference'.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

// ----------------------------------------------------------------------------
//                              class MovableRef
// ----------------------------------------------------------------------------

#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)

template <class TYPE>
inline
MovableRef<TYPE>::MovableRef(TYPE *pointer)
    : d_pointer(pointer) {
    BSLS_ASSERT(0 != pointer);
}

template <class TYPE>
inline
MovableRef<TYPE>::operator TYPE&() const {
    return *d_pointer;
}

#endif // support r-value references and alias templates

// ----------------------------------------------------------------------------
//                          struct MovableRefUtil
// ----------------------------------------------------------------------------

template <class TYPE>
inline
TYPE& MovableRefUtil::access(TYPE& lvalue) {
    return lvalue;
}

template <class TYPE>
inline
TYPE& MovableRefUtil::access(MovableRef<TYPE>& lvalue) {
    return lvalue;
}

#if !defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
template <class TYPE>
inline
MovableRef<TYPE> MovableRefUtil::move(TYPE& lvalue) {
    return MovableRef<TYPE>(bsls::Util::addressOf(lvalue));
}
#endif // support r-value references and alias templates

template <class TYPE>
inline
MovableRef<typename bsl::remove_reference<TYPE>::type>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
MovableRefUtil::move(TYPE&& rvalue) {
    return static_cast<typename bsl::remove_reference<TYPE>::type&&>(rvalue);
#else  // support r-value references and alias templates
MovableRefUtil::move(MovableRef<TYPE> rvalue) {
    return rvalue;
#endif // support r-value references and alias templates
}

// ----------------------------------------------------------------------------

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------

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
