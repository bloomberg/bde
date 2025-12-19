// bslma_allocator.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLMA_ALLOCATOR
#define INCLUDED_BSLMA_ALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a pure abstract interface for memory-allocation mechanisms.
//
//@CLASSES:
//  bslma::Allocator: protocol class for memory allocation and deallocation
//
//@SEE_ALSO: bslma_newdeleteallocator, bslma_testallocator
//
//@DESCRIPTION: This component provides the base-level protocol (pure abstract
// interface) class, `bslma_allocator`, that serves as a ubiquitous vocabulary
// type for various memory allocation mechanisms.  The functional capabilities
// documented by this protocol are similar to those afforded by global
// operators `new` and `delete`: sufficiently (but not necessarily maximally)
// aligned memory is guaranteed for any object of a given size.  Clients of
// this abstract base class will typically accept a supplied allocator (often
// at construction) and use its `allocate` and `deallocate` methods instead of
// `new` and `delete` directly.
//
// The use of (abstract) allocators provides at least three distinct advantages
// over direct (hard-coded) calls to global `new` and `delete` (see
// `bslma_newdeleteallocator`):
//
// 1. The particular choice of allocator can be selected to improve performance
//    on a per-object basis.  Without allocators, the best we can do in C++ is
//    to overload the class-specific new and delete.  Class-specific allocators
//    tend to hoard memory even when most objects of the class have been
//    deallocated, and often mask memory leaks that would otherwise have been
//    detected.  See Lakos-96, Section 10.3.4.2, pp 705-711.
// 2. By providing extra capabilities (beyond `new` and `delete`) in the
//    derived class (see `bslma_managedallocator`), we can bypass the
//    individual destructors in a dynamically allocated type and remove all
//    memory for one or more associated object almost instantly.
// 3. The `bslma::Allocator` protocol, like any other protocol, isolates
//    clients from direct coupling with platform level facilities that are not
//    fully under their control.  By installing a test allocator (see
//    `bslma_testallocator`), we are able to orchestrate the white-box testing
//    of internal calls to global operators `new` and `delete` in a
//    platform-neutral manner.
//
///Thread Safety
///-------------
// Unless otherwise documented, a single allocator object is not safe for
// concurrent access by multiple threads.  Classes derived from
// `bslma::Allocator` that are specifically designed for concurrent access must
// be documented as such.  Unless specifically documented otherwise, separate
// objects of classes derived from `bslma::Allocator` may safely be used in
// separate threads.
//
///Allocators Versus Pools
///-----------------------
// An allocator and a pool are quite different.  For starters,
// `bslma::Allocator` is an abstract class used to obtain "raw" memory of
// arbitrary size.  A pool is a concrete data structure used to organize and
// supply memory according to specific needs (e.g., a consistent size).
// Concrete allocators may use pools in their implementations, and pools will
// always take a base `bslma::Allocator` protocol in their interface.  You can
// think of an allocator as a stream of memory that flows into a pool of
// memory.  Memory is allocated from the pool until it is dry; only then does
// new memory flow into the pool from the allocator.
//
///Overloaded Global Operators `new` and `delete`
///----------------------------------------------
// This component overloads the global operator `new` to allow convenient
// syntax for the construction of objects using the `bslma::Allocator`
// protocol.  The overloaded `new` operator defined in this component has a
// second parameter, `bslma::Allocator&`, that identifies the concrete
// (derived) allocator that will be used to supply memory.
//
// Consider the following use of standard placement syntax (supplied by
// `#include <new>`) along with a `bslma::Allocator`, used to allocate an
// arbitrary `TYPE`.
// ```
// void someFunction(bslma::Allocator *basicAllocator)
// {
//     TYPE *obj = new (basicAllocator->allocate(sizeof(TYPE))) TYPE(...);
//
//     // ...
// ```
// This style of usage is inconvenient and error prone; it is also *not*
// exception safe: If the constructor of `TYPE` throws an exception, the
// `basicAllocator->deallocate` method is never called.
//
// Providing an overloaded global operator `new`, taking a reference to a
// modifiable `bslma::Allocator` as an explicit argument allows for cleaner
// usage and guarantees that the `basicAllocator->deallocate` method is called
// in case of an exception:
// ```
// void someFunction(bslma::Allocator *basicAllocator)
// {
//     TYPE *obj = new (*basicAllocator) TYPE(...);
//
//     // ...
// ```
// Finally, the analogous version of operator `delete` should not be called
// directly: The overloaded operator `delete` supplied in this component is
// solely for the compiler to invoke in the event an exception is thrown during
// a failed construction.  Instead, the `bslma::Allocator` protocol provides
// `deleteObject` (a template member function parameterized by the type of the
// object being deleted), which is implemented *conceptually* as follows:
// ```
// template <class TYPE>
// void bslma::Allocator::deleteObject(TYPE *address)
// {
//     address->~TYPE();
//     this->deallocate(address);
// }
// ```
// Note that there is also a `deleteObjectRaw` which is more efficient when it
// is known that the `address` does *not* refer to a secondary base class of
// the object being deleted.
//
///Usage
///-----
// The `bslma::Allocator` protocol provided in this component defines a
// bilateral contract between suppliers and consumers of raw memory.  The
// following subsections illustrate (1) use, and (2) implementation of the
// abstract `bslma::Allocator` base class:
//
///Example 1: Derived Concrete Allocator
/// - - - - - - - - - - - - - - - - - -
// In order for the `bslma::Allocator` interface to be useful, we must supply a
// concrete allocator that implements it.  In this example we demonstrate how
// to adapt `operator new` and `operator delete` to this protocol base class.
//
// First, in a component `.h` file, we define a class, derived from
// `bslma::Allocator`, that provides concrete implementations of the `virtual`
// `allocate` and `deallocate` methods:
// ```
//  // my_newdeleteallocator.h
//  // ...
//
//  #include <bslma_allocator.h>
//  #include <bsls_keyword.h>
//  #include <new>
//
//  /// This class is a sample concrete implementation of the
//  /// `bslma::Allocator` protocol that provides direct access to the
//  /// system-supplied (native) global operators `new` and `delete`.
//  class my_NewDeleteAllocator : public bslma::Allocator {
//
//      // NOT IMPLEMENTED
//      my_NewDeleteAllocator(const my_NewDeleteAllocator&);
//      my_NewDeleteAllocator& operator=(const my_NewDeleteAllocator&);
//
//    public:
//      // CLASS METHODS
//
//      /// Return the address of a singleton object of
//      /// `my_NewDeleteAllocator`. Since `my_NewDeleteAllocator` has no
//      /// state, there is never a need for more than one.
//      static my_NewDeleteAllocator *singleton();
//
//      // CREATORS
//
//      /// Create an allocator that wraps the global (native) operators
//      /// `new` and `delete` to supply memory.  Note that all objects of
//      /// this class share the same underlying resource.
//      my_NewDeleteAllocator() { }
//
//      /// Destroy this allocator object.  Note that destroying this
//      /// allocator has no effect on any outstanding allocated memory.
//      ~my_NewDeleteAllocator() BSLS_KEYWORD_OVERRIDE;
//
//      // MANIPULATORS
//
//      /// Return a newly allocated block of memory of (at least) the
//      /// specified positive `size` (in bytes).  If `size` is 0, a null
//      /// pointer is returned with no other effect.  If this allocator
//      /// cannot return the requested number of bytes, then it will throw
//      /// a `std::bad_alloc` exception in an exception-enabled build, or
//      /// else will abort the program in a non-exception build.  The
//      /// behavior is undefined unless `0 <= size`.  Note that the
//      /// alignment of the address returned is the maximum alignment for
//      /// any type defined on this platform.  Also note that global
//      /// `operator new` is *not* called when `size` is 0 (in order to
//      /// avoid having to acquire a lock, and potential contention in
//      /// multi-threaded programs).
//      void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE;
//
//      /// Return the memory block at the specified `address` back to this
//      /// allocator.  If `address` is 0, this function has no effect.  The
//      /// behavior is undefined unless `address` was allocated using this
//      /// allocator object and has not already been deallocated.  Note
//      /// that global `operator delete` is *not* called when `address` is
//      /// 0 (in order to avoid having to acquire a lock, and potential
//      /// contention in multi-treaded programs).
//      void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;
//  };
// ```
// Next, in the component `.cpp` file, we define the `singleton()` method,
// which provides the typical way of obtaining an instance of this allocator:
// ```
//  // my_newdeleteallocator.cpp
//  #include <my_newdeleteallocator.h>
//
//  // CLASS METHODS
//  my_NewDeleteAllocator *my_NewDeleteAllocator::singleton()
//  {
//      static my_NewDeleteAllocator obj;
//      return &obj;
//  }
// ```
// Next, we implement the `bslma::Allocator` protocol by defining (also in the
// component `.cpp` file) the virtual methods:
// ```
//  // CREATORS
//  my_NewDeleteAllocator::~my_NewDeleteAllocator()
//  {
//  }
//
//  // MANIPULATORS
//  void *my_NewDeleteAllocator::allocate(size_type size)
//  {
//      return 0 == size ? 0 : ::operator new(size);
//  }
//
//  void my_NewDeleteAllocator::deallocate(void *address)
//  {
//      // While the C++ standard guarantees that calling delete(0) is safe
//      // (3.7.3.2 paragraph 3), some libc implementations take out a lock to
//      // deal with the free(0) case, so this check can improve efficiency of
//      // threaded programs.
//
//      if (address) {
//          ::operator delete(address);
//      }
//  }
// ```
// Now we can use `my_NewDeleteAllocator` to allocate and deallocate storage
// for (in this case, `int`) objects:
// ```
//  int main()
//  {
//      typedef int T;  // Can be any type
//
//      my_NewDeleteAllocator myA;
//      T *p = static_cast<T *>(myA.allocate(sizeof(T)));
//      new (p) T(5);    // Construct object at `p`.
//      assert(5 == *p);
//      p->~T();         // not needed for `int`, but important for class types
//      myA.deallocate(p);
// ```
// Finally, we repeat the previous example using the `singleton` object instead
// of constructing a new `my_NewDeleteAllocator` and using the `operator new`
// and `deleteObject` interface instead of raw `allocate`-construct and
// destroy-`deallocate`.  Note that these interfaces can be mixed and matched
// (e.g., `singleton` can be used with `allocate`):
// ```
//      p = new (*my_NewDeleteAllocator::singleton()) T(6);
//      assert(6 == *p);
//      my_NewDeleteAllocator::singleton()->deleteObject(p);
//  }
// ```
//
///Example 2: Container Objects
/// - - - - - - - - - - - - - -
// Allocators are often supplied to objects requiring dynamically-allocated
// memory at construction.  For example, consider the following
// `my_DoubleStack` class, which uses a `bslma::Allocator` to allocate memory.
//
// First, we define the class interface, which is a minimal subset of a typical
// container interface:
// ```
//  // my_doublestack.h
//  // ...
//
//  #include <bslma_allocator.h>
//  #include <my_NewDeleteAllocator.h>
//
//  /// dynamically growing stack of `double` values
//  class my_DoubleStack {
//      enum { k_INITIAL_SIZE = 1, k_GROWTH_FACTOR = 2 };
//
//      int               d_capacity;    // physical capacity of this stack (in
//                                       // elements)
//      int               d_size;        // number of available stack elements
//                                       // currently in use
//      double           *d_stack_p;     // dynamically allocated array of
//                                       // `d_capacity` elements
//      bslma::Allocator *d_allocator_p; // holds (but doesn't own) allocator
//
//    private:
//      /// Increase the capacity by `k_GROWTH_FACTOR`.
//      void increaseSize();
//
//    public:
//      // CREATORS
//      explicit my_DoubleStack(bslma::Allocator *basicAllocator = 0);
//      my_DoubleStack(const my_DoubleStack&  other,
//                     bslma::Allocator      *basicAllocator = 0);
//      ~my_DoubleStack();
//
//      // MANIPULATORS
//      my_DoubleStack& operator=(const my_DoubleStack& rhs);
//      void pop() { --d_size; }
//      void push(double value);
//
//      // ACCESSORS
//      double operator[](int i) const { return d_stack_p[i]; }
//      bslma::Allocator *allocator() const { return d_allocator_p; }
//      int capacity() const { return d_capacity; }
//      bool isEmpty() const { return 0 == d_size; }
//      int size() const { return d_size; }
//      double top() const { return d_stack_p[d_size - 1]; }
//  };
// ```
// Next, we define the constructor, which takes an optional `basicAllocator`
// supplied only at construction.  (We avoid use of the name `allocator` so as
// not to conflict with the STL use of the word, which differs slightly.)  If
// non-zero, the stack holds a pointer to this allocator, but does not own it.
// If no allocator is supplied, the implementation itself must either
// conditionally invoke global `new` and `delete` explicitly whenever dynamic
// memory must be managed (BAD IDEA) or (GOOD IDEA) install a default allocator
// that adapts use of these global operators to the `bslma_allocator` interface
// (see `bslma_default`).  The constructor uses the selected allocator to
// allocate memory via the `allocate` method.
// ```
//  // my_doublestack.cpp
//  #include <my_doublestack.h>
//  #include <bslma_allocator.h>
//  #include <bslma_default.h>    // for selecting a default allocator
//
//  // CREATORS
//  my_DoubleStack::my_DoubleStack(bslma::Allocator *basicAllocator)
//  : d_capacity(k_INITIAL_SIZE)
//  , d_size(0)
//  , d_allocator_p(basicAllocator ?
//                  basicAllocator : my_NewDeleteAllocator::singleton())
//      // The above initialization expression is roughly equivalent to
//      // `bslma::Default::allocator(basicAllocator)`
//  {
//      assert(d_allocator_p);
//      d_stack_p = (double *)
//          d_allocator_p->allocate(d_capacity * sizeof *d_stack_p);
//  }
// ```
// Next, we define a destructor that frees the memory held by the container
// using the allocator's `deallocate` method:
// ```
//  my_DoubleStack::~my_DoubleStack()
//  {
//      // CLASS INVARIANTS
//      assert(d_allocator_p);
//      assert(d_stack_p);
//      assert(0 <= d_size);
//      assert(0 <= d_capacity);
//      assert(d_size <= d_capacity);
//
//      d_allocator_p->deallocate(d_stack_p);
//  }
// ```
// Next, we define a `reallocation` function that expands a dynamic array of
// `double`s.  Even in this simplified implementation, all use of the allocator
// protocol is relegated to the `.cpp` file:
// ```
//  /// Reallocate memory in the specified `array` to the specified `newSize`
//  /// using the specified `basicAllocator`.  The specified `length` number of
//  /// leading elements are preserved.  Since the class invariant requires
//  /// that the physical capacity of the container may grow but never shrink;
//  /// the behavior is undefined unless `length <= newSize`.
//  static inline
//  void reallocate(double           **array,
//                  int                newSize,
//                  int                length,
//                  bslma::Allocator  *basicAllocator)
//  {
//      assert(array);
//      assert(1 <= newSize);
//      assert(0 <= length);
//      assert(basicAllocator);
//      assert(length <= newSize);        // enforce class invariant
//
//      double *tmp = *array;             // support exception neutrality
//      *array = (double *) basicAllocator->allocate(newSize * sizeof **array);
//
//      // COMMIT POINT
//
//      memcpy(*array, tmp, length * sizeof **array);
//      basicAllocator->deallocate(tmp);
//  }
// ```
// Next, we define the private `increaseSize` method to allocate more space for
// container elements as needed:
// ```
//  void my_DoubleStack::increaseSize()
//  {
//      int proposedNewSize = d_capacity * k_GROWTH_FACTOR;
//      assert(proposedNewSize > d_size);
//
//      // Reallocate might throw.
//      reallocate(&d_stack_p, proposedNewSize, d_size, d_allocator_p);
//
//      // Commit change only after `reallocate` succeeds.
//      d_capacity = proposedNewSize;
//  }
// ```
// Now we have what we need to implement the `push` method:
// ```
//  void my_DoubleStack::push(double value)
//  {
//      if (d_size >= d_capacity) {
//          increaseSize();
//      }
//      d_stack_p[d_size++] = value;
//  }
// ```
// Now, to test our stack class, we first verify that its constructor captures
// the allocator correctly; if supplied an allocator pointer, it holds on to
// that pointer, otherwise it uses `my_NewDeleteAllocator::singleton()`:
// ```
//  int main()
//  {
//      my_NewDeleteAllocator myA;
//
//      my_DoubleStack ds1(&myA); // Supply an allocator.
//      assert(ds1.allocator() == &myA);
//
//      my_DoubleStack ds2;       // Do not supply an allocator.
//      assert(ds2.allocator() == my_NewDeleteAllocator::singleton());
// ```
// Finally, we exercise and verify the behavior of the manipulators and
// accessors:
// ```
//      assert(ds2.isEmpty());
//      assert(1 == ds2.capacity());
//      ds2.push(1.25);
//      ds2.push(1.5);
//      ds2.push(1.75);
//
//      assert(! ds2.isEmpty());
//      assert(4    == ds2.capacity());
//      assert(3    == ds2.size());
//      assert(1.75 == ds2.top());
//      assert(1.25 == ds2[0]);
//      assert(1.5  == ds2[1]);
//      assert(1.75 == ds2[2]);
//
//      ds2.pop();
//      assert(4   == ds2.capacity());
//      assert(2   == ds2.size());
//      assert(1.5 == ds2.top());
//  }
// ```

#include <bslma_allocator.fwd.h>

#include <bslscm_version.h>

#include <bslma_deleterhelper.h>
#include <bslma_memoryresource.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_enableif.h>

#include <bsls_keyword.h>
#include <bsls_nullptr.h>
#include <bsls_platform.h>

#include <cstddef>       // for `std::size_t`, `std::ptrdiff_t`

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_cpp11.h>
#endif

namespace BloombergLP {

namespace bslma {

                        // ===============
                        // class Allocator
                        // ===============

/// This protocol class provides a pure abstract interface and contract for
/// clients and suppliers of raw memory.  If the requested memory cannot be
/// returned, the contract requires that an `std::bad_alloc` exception be
/// thrown.  Note that memory is guaranteed to be sufficiently aligned for
/// any object of the requested size on the current platform, which may be
/// less than the maximal alignment guarantee afforded by global
/// `operator new`.
class Allocator : public bsl::memory_resource {

  protected:
    // PROTECTED MANIPULATORS

    /// Return a newly allocated block of memory of (at least) the specified
    /// positive `bytes` and having at least the specified `alignment`.  Unless
    /// overriden in a derived class, the return value is
    /// `this->allocate(bytes)`.  If this allocator cannot return the requested
    /// number of bytes or cannot satisfy the alignment request, then it will
    /// throw a `std::bad_alloc` exception in an exception-enabled build, or
    /// else will abort the program in a non-exception build.  Unless overriden
    /// in a derived class, this function will forward the allocation request
    /// to the `allocate` virtual function, padding `bytes` and adjusting the
    /// return value as necessary to ensure sufficient alignment.  Note that if
    /// `bytes` is `0`, the same non-null value will be returned every time.
    void* do_allocate(std::size_t bytes,
                      std::size_t alignment) BSLS_KEYWORD_OVERRIDE;

    /// Return the memory block at the specified `p` address, having the
    /// specified `bytes` and specified `alignment`, back to this allocator.
    /// Unless overriden in a derived class, this function will forward the
    /// deallocation request to the `deallocate` virtual function, padding
    /// `bytes` and adjusting `p` as necessary to account for `alignment`
    /// values other than the natural alignment for an object of size `bytes`.
    /// The behavior is undefined unless `address` is a block allocated from
    /// this allocator object using the same `bytes` and `alignment` and not
    /// already deallocated.
    void do_deallocate(void        *p,
                       std::size_t  bytes,
                       std::size_t  alignment) BSLS_KEYWORD_OVERRIDE;

    // PROTECTED ACCESSORS

    /// Return `true` if this allocator is equal to the specified `other`
    /// allocator, meaning (at least) that a memory block allocated by one can
    /// be deallocated by the other; otherwise return `false`.  Unless
    /// overriden, this method returns `this == &other`.
    bool do_is_equal(const memory_resource& other) const
                                   BSLS_KEYWORD_NOEXCEPT BSLS_KEYWORD_OVERRIDE;

  public:
    // PUBLIC TYPES

    /// Alias for an unsigned integral type capable of representing the number
    /// of bytes in this platform's virtual address space.
    typedef std::size_t size_type;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // CLASS METHODS

    /// Throw `std::bad_alloc` if exceptions are enabled or abort the
    /// program otherwise.  Derived classes and helper functions will
    /// typically call this function when they are unable to satisfy an
    /// allocation request.  This function never returns.
    ///
    /// @DEPRECATED: Use `bsls::BslExceptionUtil::throwBadAlloc` instead.
    static void throwBadAlloc();
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

    // CREATORS

    /// Destroy this allocator.  Note that the behavior of destroying an
    /// allocator while memory is allocated from it is not specified; unless
    /// you *know* that it is valid to do so, don't!
    ~Allocator() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS

    /// Return a newly allocated block of memory of (at least) the specified
    /// positive `size` (in bytes).  If `size` is 0, a null pointer is
    /// returned with no other effect.  If this allocator cannot return the
    /// requested number of bytes, then it will throw a `std::bad_alloc`
    /// exception in an exception-enabled build, or else will abort the
    /// program in a non-exception build.  The behavior is undefined unless
    /// `0 <= size`.  Note that the alignment of the address returned
    /// conforms to the platform requirement for any object of the specified
    /// `size`.  Note that this virtual function hides a two-parameter
    /// non-virtual `allocate` method inherited from `bsl::memory_resource`;
    /// to access the inherited function, upcast the object to
    /// `bsl::memory_resource&` before calling the base-class function.
    virtual void *allocate(size_type size) = 0;

    /// Return the memory block at the specified `address` back to this
    /// allocator.  If `address` is 0, this function has no effect.  The
    /// behavior is undefined unless `address` was allocated using this
    /// allocator object and has not already been deallocated.  Note that
    /// this virtual function hides a three-parameter, non-virtual `deallocate`
    /// method inherited from `bsl::memory_resource`; to access the
    /// inherited function, upcast the object to `bsl::memory_resource&`
    /// before calling the base-class function.
    virtual void deallocate(void *address) = 0;

    /// Destroy the specified `object` based on its dynamic type and then
    /// use this allocator to deallocate its memory footprint.  Do nothing
    /// if `object` is a null pointer.  The behavior is undefined unless
    /// `object`, when cast appropriately to `void *`, was allocated using
    /// this allocator and has not already been deallocated.  Note that
    /// `dynamic_cast<void *>(object)` is applied if `TYPE` is polymorphic,
    /// and `static_cast<void *>(object)` is applied otherwise.
    template <class TYPE>
    void deleteObject(const TYPE *object);

    /// Destroy the specified `object` and then use this allocator to
    /// deallocate its memory footprint.  Do nothing if `object` is a null
    /// pointer.  The behavior is undefined unless `object` was allocated
    /// using this allocator, is **not** a secondary base class pointer --
    /// i.e., the address is (numerically) the same as when it was
    /// originally dispensed by this allocator, and has not already been
    /// deallocated.
    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);

    /// This function has no effect.  Note that it exists to support calling
    /// `deleteObject` will a null pointer literal, that would otherwise not
    /// deduce to a pointer type for the method above.  As calls to
    /// `deleteObject` with (typed) null pointer values have well-defined
    /// behavior, it should also support calls with a null pointer literal.
    void deleteObject(bsl::nullptr_t);

    /// This function has no effect.  Note that it exists to support calling
    /// `deleteObjectRaw` will a null pointer literal, that would otherwise
    /// not deduce to a pointer type for the method above.  As calls to
    /// `deleteObjectRaw` with (typed) null pointer values have well-defined
    /// behavior, it should also support calls with a null pointer literal.
    void deleteObjectRaw(bsl::nullptr_t);
};

}  // close package namespace
}  // close enterprise namespace

// FREE OPERATORS

// Note that the operators `new` and `delete` are declared outside the
// `BloombergLP` namespace so that they do not hide the standard placement
// `new` and `delete` operators (i.e.,
// `void *operator new(std::size_t, void *)` and
// `void operator delete(void *)`).
//
// Note also that only the scalar versions of operators `new` and `delete` are
// provided, because overloading `new` (and `delete`) with their array versions
// would cause dangerous ambiguity.  Consider what would have happened had we
// overloaded the array version of operator `new`:
// ```
//  void *operator new[](std::size_t size,
//                       BloombergLP::bslma::Allocator& basicAllocator)
// ```
// The user of the allocator class may expect to be able to use array
// `operator new` as follows:
// ```
//  new (*basicAllocator) my_Type[...];
// ```
// The problem is that this expression returns an array that cannot be safely
// deallocated.  On the one hand, there is no syntax in C++ to invoke an
// overloaded `operator delete` that, other than deallocating memory, would
// invoke the destructor.  On the other hand, the pointer returned by
// `operator new` cannot be passed to the `deallocate` method directly because
// the pointer is different from the one returned by the `allocate` method.
// The compiler offsets the value of this pointer by a header, which is used to
// maintain the number of objects in the array (so that the non-overloaded
// `operator delete` can destroy the right number of objects).

/// Return the memory allocated from the specified `basicAllocator` of at
/// least the specified `size` bytes, or 0 if `size` is 0.  The behavior is
/// undefined unless `0 <= static_cast<bslma::Allocator::size_type>(size)`.
/// Note that an object may allocate additional memory internally, requiring
/// the allocator to be passed in as a constructor argument:
/// ```
/// my_Type *createMyType(bslma::Allocator *basicAllocator)
/// {
///     return new (*basicAllocator) my_Type(..., basicAllocator);
/// }
/// ```
/// Note also that the analogous version of operator `delete` should *not*
/// be called directly.  Instead, this component provides a template member
/// function `deleteObject` parameterized by `TYPE` that effectively
/// performs the following operations:
/// ```
/// void deleteMyType(bslma::Allocator *basicAllocator, my_Type *address)
/// {
///     address->~my_Type();
///     basicAllocator->deallocate(address);
/// }
/// ```
/// See also `deleteObjectRaw` for better performance when `address` is
/// known not to be a secondary base type of the object being deleted.
inline
void *operator new(std::size_t                    size,
                   BloombergLP::bslma::Allocator& basicAllocator);

/// Use the specified `basicAllocator` to deallocate the memory at the
/// specified `address`.  The behavior is undefined unless `address` was
/// allocated using `basicAllocator` and has not already been deallocated.
/// This operator is supplied solely to allow the compiler to arrange for it
/// to be called in case of an exception.
inline
void operator delete(void                           *address,
                     BloombergLP::bslma::Allocator&  basicAllocator);

// NOTE: The following two operators are declared but never defined to force a
// link-time error should any code inadvertently use them.

/// Note that this operator is intentionally not defined.
void *operator new(
           std::size_t                    size,
           BloombergLP::bslma::Allocator *basicAllocator) BSLS_KEYWORD_DELETED;

/// Note that this operator is intentionally not defined.
void operator delete(
           void                          *address,
           BloombergLP::bslma::Allocator *basicAllocator) BSLS_KEYWORD_DELETED;

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {

namespace bslma {

                        // ---------------
                        // class Allocator
                        // ---------------

// MANIPULATORS
template <class TYPE>
inline
void Allocator::deleteObject(const TYPE *object)
{
    DeleterHelper::deleteObject(object, this);
}


inline
void Allocator::deleteObject(bsl::nullptr_t)
{
    // This function body is intentionally left blank.
}

template <class TYPE>
inline
void Allocator::deleteObjectRaw(const TYPE *object)
{
    DeleterHelper::deleteObjectRaw(object, this);
}

inline
void Allocator::deleteObjectRaw(bsl::nullptr_t)
{
    // This function body is intentionally left blank.
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

/// These aliases are defined for backward compatibility.
typedef bslma::Allocator bslma_Allocator;

typedef bslma::Allocator bdema_Allocator;

#endif  // BDE_OPENSOURCE_PUBLICATION

}  // close enterprise namespace

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

inline
void *operator new(std::size_t                    size,
                   BloombergLP::bslma::Allocator& basicAllocator)
{
    return basicAllocator.allocate(size);
}

inline
void operator delete(void                           *address,
                     BloombergLP::bslma::Allocator&  basicAllocator)
{
    basicAllocator.deallocate(address);
}

#ifdef BSLS_PLATFORM_CMP_MSVC
template <class t_TYPE,
          class = typename bsl::enable_if<bsl::is_same<
                                  t_TYPE,
                                  BloombergLP::bslma::Allocator>::value>::type>
void *operator new[](std::size_t size, t_TYPE& basicAllocator)
{
    BSLMF_ASSERT(sizeof(t_TYPE) == 0);
    // This assert fires if you have tried to allocate an array using
    // `operator new[]` with also specifying an allocator (on MSVC).  Without
    // this overload, the Microsoft compiler would just use the non-array
    // `operator new` and not give an error as other compilers do.
}
#endif

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
