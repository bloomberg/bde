// bdema_managedptr.h                                                 -*-C++-*-
#ifndef INCLUDED_BDEMA_MANAGEDPTR
#define INCLUDED_BDEMA_MANAGEDPTR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a managed pointer class.
//
//@CLASSES:
//     bdema_ManagedPtr: proctor for automatic memory management
// bdema_ManagedPtrUtil: Namespace for deleter for stack-allocated objects
//
//@AUTHOR: Ilougino Rocha (irocha), Pablo Halpern (phalpern),
//         Alisdair Meredith (ameredith1@bloomberg.net)
//
//@SEE_ALSO: bslmf_ispolymporphic
//
//@DESCRIPTION: This component provides a proctor, similar to 'bsl::auto_ptr',
// that supports user-specified deleters.  The proctor is responsible for the
// automatic destruction of the object referenced by the managed pointer.  As
// a "smart pointer", this object offers an interface similar to a native
// pointer, supporting dereference operators (*, ->), (in)equality comparison
// and testing as if it were a boolean value.  However, like 'bsl::auto_ptr' it
// has unusual "copy-semantics" that transfer ownership of the managed object,
// rather than making a copy.  It should be noted that this signature does not
// satisfy the requirements for an element-type stored in any of the standard
// library containers.  Note that this component will fail to compile when
// instantiated for a class that gives a false-positive for the type trait
// 'bslmf::IsPolymorphic'.  See the 'bslmf_ispolymporphic' component for more
// details.
//
///Deleters
///--------
// When a managed pointer is destroyed, the managed object is destroyed using
// the user supplied "deleter".  A deleter is simply a function that is invoked
// with two arguments, a pointer to the object to be destroyed, and a pointer
// to a 'cookie' that is supplied at the same time as the 'deleter' and managed
// object.  The meaning of the 'cookie' depends on the deleter.  Typically a
// deleter function will accept two 'void *' pointers and internally cast them
// to the appropriate types for pointers to the 'cookie' and managed object.
// This component still supports (deprecated) legacy deleters that expect to be
// passed pointers to the specific 'cookie' and managed object types in use.
// This latter form of deleter is now deprecated as it relies on undefined
// behavior, casting such function pointers to the correct form (taking two
// 'void *' arguments) and invoking the function with two 'void *' pointer
// arguments.  While this is undefined behavior, it is known to have the
// desired effect on all platforms currently in use.
//
///Factories
///---------
// An object that will be managed by a 'bdema_ManagedPtr' object is typically
// dynamically allocated and destroyed by a factory.  For the purposes of this,
// component, a factory is any class that provides a 'deleteObject' function
// taking a single argument of the (pointer) type of the managed pointer.
// E.g., 'bslma::Allocator' is a commonly used factory, and the currently
// installed default allocator is the factory that is assumed to be used if
// neither a factory nor deleter are specified when supplying a pointer to be
// managed.
//
///Aliasing
///--------
// In a managed pointer, the pointer value (the value returned by the 'ptr'
// method) and the pointer to the managed object need not have the same value.
// The 'loadAlias' method allows a managed pointer to be created as an "alias"
// to another managed pointer (possibly of a different type), which we'll call
// the "original" managed pointer.  When 'ptr' is invoked on the alias, the
// aliased pointer value is returned, but when the managed pointer is
// destroyed, the original managed object will be passed to the deleter.  (See
// also the documentation of the 'alias' constructor or of the 'loadAlias'
// method.)
//
///Exception Safety
///----------------
// The principal usage of a managed pointer is to guarantee that a local object
// will be deallocated properly should an operation throw after its allocation.
// In this, it is very similar to 'bsl::auto_ptr'.  It is required for the
// proper functioning of this component that a deleter does not throw at
// invocation (upon destruction or re-assignment of the managed pointer).
//
///Type Casting
///------------
// 'bdema_ManagedPtr' objects can be implicitly and explicitly cast to
// different types in the same way that native pointers can.
//
///Implicit Casting
/// - - - - - - - -
// As with native pointers, a managed pointer of the type 'B' that is derived
// from the type 'A', can be directly assigned to a 'bdema_ManagedPtr' of 'A'.
// Likewise a managed pointer of type 'B' can be directly assigned to a
// 'bdema_ManagedPtr' of 'const B'.
//
///Explicit Casting
/// - - - - - - - -
// Through "aliasing", a managed pointer of any type can be explicitly cast
// to a managed pointer of any other type using any legal cast expression.  See
// the 'casting' example below for more details.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Implementing a protocol
/// - - - - - - - - - - - - - - - - -
// We demonstrate using 'bdema_ManagedPtr' to configure and return a managed
// object implementing an abstract protocol.
//
// First we define our protocol, 'Shape', a type of object that knows how to
// compute its 'area'.  Note that for expository reasons only, we do *nor*
// give 'Shape' a virtual destructor.
//..
//  struct Shape {
//      virtual double area() const = 0;
//          // Return the 'area' of this shape.
//  };
//..
// Then we define a couple of classes that implement the 'Shape' protocol, a
// 'Circle' and a 'Square'.
//..
//  class Circle : public Shape {
//    private:
//      // DATA
//      double d_radius;
//
//    public:
//      // CREATORS
//      explicit Circle(double r);
//          // Create a 'Circle' object having radius 'r'.
//
//      // ACCESSORS
//      virtual double area() const;
//          // Return the area of this Circle, given by the forumula pi*r*r.
//  };
//
//  class Square : public Shape {
//    private:
//      // DATA
//      double d_sideLength;
//
//    public:
//      // CREATORS
//      explicit Square(double side);
//          // Create a 'Square' having sides of length 'side'.
//
//      // ACCESSORS
//      virtual double area() const;
//          // Return the area of this Square, given by the forumula side*side
//  };
//..
// Next we implement the methods for 'Circle' and 'Square'.
//..
//  Circle::Circle(double r)
//  : d_radius(r)
//  {
//  }
//
//  double Circle::area() const {
//      return 3.141592653589793238462 * d_radius * d_radius;
//  }
//
//  Square::Square(double side)
//  : d_sideLength(side)
//  {
//  }
//
//  double Square::area() const {
//      return d_sideLength * d_sideLength;
//  }
//..
// Then we define an enumeration that lists each implementation of the 'Shape'
// protocol.
//..
//  struct Shapes {
//      enum VALUES { SHAPE_CIRCLE, SHAPE_SQUARE };
//  };
//..
// Now we can define a function that will return a 'Circle' object or a
// 'Square' object according to the specified 'kind' parameter, and having its
// 'dimension' specified by the caller.
//..
//  bdema_ManagedPtr<Shape> makeShape(Shapes::VALUES kind, double dimension)
//  {
//      bslma::Allocator *alloc = bslma::Default::defaultAllocator();
//      bdema_ManagedPtr<Shape> result;
//      switch (kind) {
//          case Shapes::SHAPE_CIRCLE : {
//              Circle *circ = new(*alloc)Circle(dimension);
//              result.load(circ);
//              break;
//          }
//          case Shapes::SHAPE_SQUARE : {
//              Square *sqr = new(*alloc)Square(dimension);
//              result.load(sqr);
//              break;
//          }
//      };
//      return result;
//  }
//..
// Then, we can use our function to create shapes of different kinds, and check
// that they report the correct area.  Note that are using a radius of '1.0'
// for the 'Circle' and integral side-length for the 'Square' to support an
// accurate 'operator==' with floating-point quantities.  Also note that,
// despite the destructor for 'Shape' being non-virtual, the correct destructor
// for the appropriate concrete 'Shape' type is called.  This is because the
// destructor is captured when the 'bdema_ManagedPtr' constructor is called,
// and has access to the complete type of each shape object.
//..
//  void testShapes()
//  {
//      bdema_ManagedPtr<Shape> shape = makeShape(Shapes::SHAPE_CIRCLE, 1.0);
//      assert(0 != shape);
//      assert(3.141592653589793238462 == shape->area());
//
//      shape = makeShape(Shapes::SHAPE_SQUARE, 2.0);
//      assert(0 != shape);
//      assert(4.0 == shape->area());
//  }
//..
// Next, we observe that as we are creating objects dynamically, we should pass
// an allocator to the 'makeShape' function, rather than simply accepting the
// default allocator each time.  Note that when we do this, we pass the user's
// allocator to the 'bdema_ManagedPtr' object as the "factory".
//..
//  bdema_ManagedPtr<Shape> makeShape(Shapes::VALUES    kind,
//                                    double            dimension,
//                                    bslma::Allocator *allocator)
//  {
//      bslma::Allocator *alloc = bslma::Default::allocator(allocator);
//      bdema_ManagedPtr<Shape> result;
//      switch (kind) {
//      case Shapes::SHAPE_CIRCLE : {
//              Circle *circ = new(*alloc)Circle(dimension);
//              result.load(circ, alloc);
//              break;
//          }
//      case Shapes::SHAPE_SQUARE : {
//              Square *sqr = new(*alloc)Square(dimension);
//              result.load(sqr, alloc);
//              break;
//          }
//      };
//      return result;
//  }
//..
// Finally we repeat the earlier test, additionally passing a test allocator:
//..
//  void testShapesToo()
//  {
//      bslma::TestAllocator ta("object");
//
//      bdema_ManagedPtr<Shape> shape =
//                                   makeShape(Shapes::SHAPE_CIRCLE, 1.0, &ta);
//      assert(0 != shape);
//      assert(3.141592653589793238462 == shape->area());
//
//      shape = makeShape(Shapes::SHAPE_SQUARE, 3.0, &ta);
//      assert(0 != shape);
//      assert(9.0 == shape->area());
//  }
//..
//
///Example 2: Aliasing
///- - - - - - - - - -
// Suppose that we wish to give access to an item in a temporary
// array via a pointer which we'll call the "finger".  The finger is the only
// pointer to the array or any part of the array, but the entire array must be
// valid until the finger is destroyed, at which time the entire array must be
// deleted.  We handle this situation by first creating a managed pointer to
// the entire array, then creating an alias of that pointer for the finger.
// The finger takes ownership of the array instance, and when the finger is
// destroyed, it is the array's address, rather than the finger, that is passed
// to the deleter.
//
// First, let's say our array stores data acquired from a ticker plant
// accessible by a global 'getQuote' function:
//..
//  struct Ticker {
//
//      static double getQuote() // From ticker plant.  Simulated here
//      {
//          static const double QUOTES[] = {
//          7.25, 12.25, 11.40, 12.00, 15.50, 16.25, 18.75, 20.25, 19.25, 21.00
//          };
//          static const int NUM_QUOTES = sizeof(QUOTES) / sizeof(QUOTES[0]);
//          static int index = 0;
//
//          double ret = QUOTES[index];
//          index = (index + 1) % NUM_QUOTES;
//          return ret;
//      }
//  };
//..
// Then, we want to find the first quote larger than a specified threshold, but
// would also like to keep the earlier and later quotes for possible
// examination.  Our 'getFirstQuoteLargerThan' function must allocate memory
// for an array of quotes (the threshold and its neighbors).  It thus returns
// a managed pointer to the desired value:
//..
//  const double END_QUOTE = -1;
//
//  bdema_ManagedPtr<double>
//  getFirstQuoteLargerThan(double threshold, bslma::Allocator *allocator)
//  {
//      assert(END_QUOTE < 0 && 0 <= threshold);
//..
// Next, we allocate our array with extra room to mark the beginning and end
// with a special 'END_QUOTE' value:
//..
//      const int MAX_QUOTES = 100;
//      int numBytes = (MAX_QUOTES + 2) * sizeof(double);
//      double *quotes = (double*) allocator->allocate(numBytes);
//      quotes[0] = quotes[MAX_QUOTES + 1] = END_QUOTE;
//..
// Then, we create a managed pointer to the entire array:
//..
//      bdema_ManagedPtr<double> managedQuotes(quotes, allocator);
//..
// Next, we read quotes until the array is full, keeping track of the first
// quote that exceeds the threshold.
//..
//      double *finger = 0;
//
//      for (int i = 1; i <= MAX_QUOTES; ++i) {
//          double quote = Ticker::getQuote();
//          quotes[i] = quote;
//          if (!finger && quote > threshold) {
//              finger = &quotes[i];
//          }
//      }
//..
// Now, we use the alias constructor to create a managed pointer that points
// to the desired value (the finger) but manages the entire array:
//..
//      return bdema_ManagedPtr<double>(managedQuotes, finger);
//  }
//..
// Then, our main program calls 'getFirstQuoteLargerThan' like this:
//..
//  int aliasExample()
//  {
//      bslma::TestAllocator ta;
//      bdema_ManagedPtr<double> result = getFirstQuoteLargerThan(16.00, &ta);
//      assert(*result > 16.00);
//      assert(1 == ta.numBlocksInUse());
//      if (g_verbose) bsl::cout << "Found quote: " << *result << bsl::endl;
//..
// Next, We also print the preceding 5 quotes in last-to-first order:
//..
//      if (g_verbose) bsl::cout << "Preceded by:";
//      int i;
//      for (i = -1; i >= -5; --i) {
//          double quote = result.ptr()[i];
//          if (END_QUOTE == quote) {
//              break;
//          }
//          assert(quote < *result);
//          if (g_verbose) bsl::cout << ' ' << quote;
//      }
//      if (g_verbose) bsl::cout << bsl::endl;
//..
// Then, to move the finger, e.g., to the last position printed, one must be
// careful to retain the ownership of the entire array.  Using the statement
// 'result.load(result.ptr()-i)' would be an error, because it would first
// compute the pointer value 'result.ptr()-i' of the argument, then release the
// entire array before starting to manage what has now become an invalid
// pointer.  Instead, 'result' must retain its ownership to the entire array,
// which can be attained by:
//..
//      result.loadAlias(result, result.ptr()-i);
//..
// Finally, if we reset the result pointer, the entire array is deallocated:
//..
//      result.clear();
//      assert(0 == ta.numBlocksInUse());
//      assert(0 == ta.numBytesInUse());
//
//      return 0;
//  }
//..
//
///Example 3: Dynamic Objects and Factories
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to track the number of objects currently managed by
// 'bdema_ManagedPtr' objects.
//
// First we define a factory type, that holds an allocator and a usage-counter.
// Note that such a type cannot sensibly be copied, as the notion 'count'
// becomes confused.
//..
//  class CountedFactory {
//      // DATA
//      int               d_count;
//      bslma::Allocator *d_allocator;
//
//      // NOT IMPLEMENTED
//      CountedFactory(const CountedFactory&);
//      CountedFactory& operator=(const CountedFactory&);
//
//    public:
//      // CREATORS
//      explicit CountedFactory(bslma::Allocator *alloc = 0);
//          // Create a 'CountedFactory' object which uses the supplied
//          // allocator 'alloc'.
//
//      ~CountedFactory();
//          // Destroy this object.
//..
// Next, we provide the 'createObject' and 'deleteObject' functions that are
// standard for factory objects.  Note that the 'deleteObject' function
// signature has the form required by 'bdeam_ManagedPtr' for a factory.
//..
//      // MANIPULATORS
//      template <class TYPE>
//      TYPE *createObject();
//          // Return a pointer to a newly allocated object of type 'TYPE'
//          // created using its default constructor.  Memory for the object
//          // is supplied by the allocator supplied to this factory's
//          // constructor, and the count of valid object is incremented.
//
//      template <class TYPE>
//      void deleteObject(const TYPE *target);
//          // Destroy the object pointed to be 'target' and reclaim the
//          // memory.  Decrement the count of currently valid objects.
//..
// Then, we round out the class with the ability to query the 'count' of
// currently allocated objects.
//..
//      // ACCESSORS
//      int count() const;
//          // Return the number of currently valid objects allocated by this
//          // factory.
//  };
//..
// Next, we define the operations declared by the class.
//..
//  CountedFactory::CountedFactory(bslma::Allocator *alloc)
//  : d_count(0)
//  , d_allocator(bslma::Default::allocator(alloc))
//  {
//  }
//
//  CountedFactory::~CountedFactory()
//  {
//      assert(0 == d_count);
//  }
//
//  template <class TYPE>
//  TYPE *CountedFactory::createObject()
//  {
//      TYPE *result = new(*d_allocator)TYPE;
//      ++d_count;
//      return result;
//  }
//
//  template <class TYPE>
//  void CountedFactory::deleteObject(const TYPE *object)
//  {
//      d_allocator->deleteObject(object);
//      --d_count;
//  }
//
//  inline
//  int CountedFactory::count() const
//  {
//      return d_count;
//  }
//..
// Then, we can create a test function to illustrate how such a factory would
// be used with 'bdema_ManagedPtr'.
//..
//  void testCountedFactory()
//  {
//..
// Next, we declare a test allocator, and an object of our 'CountedFactory'
// type using that allocator.
//..
//      bslma::TestAllocator ta;
//      CountedFactory cf(&ta);
//..
// Then, we open a new local scope and declare an array of managed pointers.
// We need a local scope in order to observe the behavior of the destructors at
// end of the scope, and use an array as an easy way to count more than one
// object.
//..
//      {
//          bdema_ManagedPtr<int> pData[4];
//..
// Next, we load each managed pointer in the array with a new 'int' using our
// factory 'cf' and assert that the factory 'count' is correct after each new
// 'int' is created.
//..
//          int i = 0;
//          while (i != 4) {
//              pData[i++].load(cf.createObject<int>(), &cf);
//              assert(cf.count() == i);
//          }
//..
// Then, we 'clear' the contents of a single managed pointer in the array, and
// assert that the factory 'count' is appropriately reduced.
//..
//          pData[1].clear();
//          assert(3 == cf.count());
//..
// Next, we 'load' a managed pointer with another new 'int' value, again using
// 'cf' as the factory, and assert that the 'count' of valid objects remains
// the same (destroy one object and add another).
//..
//          pData[2].load(cf.createObject<int>(), &cf);
//          assert(3 == cf.count());
//      }
//..
// Finally, we allow the array of managed pointers to go out of scope and
// confirm that when all managed objects are destroyed, the factory 'count'
// falls to zero, and does not overshoot.
//..
//      assert(0 == cf.count());
//  }
//..
//
///Example 4: Type Casting
///- - - - - - - - - - - -
// 'bdema_ManagedPtr' objects can be implicitly and explicitly cast to
// different types in the same way that native pointers can.
//
///Implicit Conversion
/// -  -  -  -  -  - -
// As with native pointers, a pointer of the type 'B' that is publicly derived
// from the type 'A', can be directly assigned a 'bdema_ManagedPtr' of 'A'.
//
// First, consider the following code snippets:
//..
//  void implicitCastingExample()
//  {
//..
// If the statements:
//..
//      bslma::TestAllocator localDefaultTa;
//      bslma::TestAllocator localTa;
//
//      bslma::DefaultAllocatorGuard guard(&localDefaultTa);
//
//      int numdels = 0;
//
//      {
//          B *b_p = 0;
//          A *a_p = b_p;
//..
// are legal expressions, then the statements
//..
//          bdema_ManagedPtr<A> a_mp1;
//          bdema_ManagedPtr<B> b_mp1;
//
//          assert(!a_mp1 && !b_mp1);
//
//          a_mp1 = b_mp1;      // conversion assignment of nil ptr to nil
//          assert(!a_mp1 && !b_mp1);
//
//          B *b_p2 = new (localDefaultTa) B(&numdels);
//          bdema_ManagedPtr<B> b_mp2(b_p2);    // default allocator
//          assert(!a_mp1 && b_mp2);
//
//          a_mp1 = b_mp2;      // conversion assignment of nonnil ptr to nil
//          assert(a_mp1 && !b_mp2);
//
//          B *b_p3 = new (localTa) B(&numdels);
//          bdema_ManagedPtr<B> b_mp3(b_p3, &localTa);
//          assert(a_mp1 && b_mp3);
//
//          a_mp1 = b_mp3;      // conversion assignment of nonnil to nonnil
//          assert(a_mp1 && !b_mp3);
//
//          a_mp1 = b_mp3;      // conversion assignment of nil to nonnil
//          assert(!a_mp1 && !b_mp3);
//
//          // constructor conversion init with nil
//          bdema_ManagedPtr<A> a_mp4(b_mp3, b_mp3.ptr());
//          assert(!a_mp4 && !b_mp3);
//
//          // constructor conversion init with nonnil
//          B *p_b5 = new (localTa) B(&numdels);
//          bdema_ManagedPtr<B> b_mp5(p_b5, &localTa);
//          bdema_ManagedPtr<A> a_mp5(b_mp5, b_mp5.ptr());
//          assert(a_mp5 && !b_mp5);
//          assert(a_mp5.ptr() == p_b5);
//
//          // constructor conversion init with nonnil
//          B *p_b6 = new (localTa) B(&numdels);
//          bdema_ManagedPtr<B> b_mp6(p_b6, &localTa);
//          bdema_ManagedPtr<A> a_mp6(b_mp6);
//          assert(a_mp6 && !b_mp6);
//          assert(a_mp6.ptr() == p_b6);
//
//          struct S {
//              int d_i[10];
//          };
//
//          assert(200 == numdels);
//      }
//
//      assert(400 == numdels);
//  } // implicitCastingExample()
//..
//
///Explicit Conversion
/// -  -  -  -  -  - -
// Through "aliasing", a managed pointer of any type can be explicitly
// converted to a managed pointer of any other type using any legal cast
// expression.  For example, to static-cast a managed pointer of type A to a
// shared pointer of type B, one can simply do the following:
//..
//  void explicitCastingExample() {
//
//      bdema_ManagedPtr<A> a_mp;
//      bdema_ManagedPtr<B> b_mp1(a_mp, static_cast<B*>(a_mp.ptr()));
//..
// or even use the less safe "C"-style casts:
//..
//      // bdema_ManagedPtr<A> a_mp;
//      bdema_ManagedPtr<B> b_mp2(a_mp, (B*)(a_mp.ptr()));
//
//  } // explicitCastingExample()
//..
// Note that when using dynamic cast, if the cast fails, the target managed
// pointer will be reset to an unset state, and the source will not be
// modified.  Consider for example the following snippet of code:
//..
//  void processPolymorphicObject(bdema_ManagedPtr<A> aPtr,
//                                bool *castSucceeded)
//  {
//      bdema_ManagedPtr<B> bPtr(aPtr, dynamic_cast<B*>(aPtr.ptr()));
//      if (bPtr) {
//          assert(!aPtr);
//          *castSucceeded = true;
//      }
//      else {
//          assert(aPtr);
//          *castSucceeded = false;
//      }
//  }
//..
// If the value of 'aPtr' can be dynamically cast to 'B*' then ownership is
// transferred to 'bPtr', otherwise 'aPtr' is to be modified.  As previously
// stated, the managed object will be destroyed correctly regardless of how
// it is cast.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTRDELETER
#include <bdema_managedptrdeleter.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTR
#include <bslma_managedptr.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_ISVOID
#include <bslmf_isvoid.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NULLPTR
#include <bsls_nullptr.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_ADDREFERENCE
#include <bslmf_addreference.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLS_UNSPECIFIEDBOOL
#include <bsls_unspecifiedbool.h>
#endif

#endif

namespace BloombergLP {

namespace bslma { class Allocator; }

                           // ======================
                           // class bdema_ManagedPtr
                           // ======================

template <class TARGET_TYPE>
class bdema_ManagedPtr : public bslma::ManagedPtr<TARGET_TYPE> {
    // This class is a "smart pointer" that refers to a *target* object
    // accessed via a pointer to the specified parameter type, 'TARGET_TYPE',
    // and that supports sole ownership of a *managed* object that is
    // potentially of a different type, and may be an entirely different object
    // from the target object.  A managed pointer ensures that the object it
    // manages is destroyed when the managed pointer is destroyed (or
    // re-assigned), using the "deleter" supplied along with the managed
    // object.  The target object referenced by a managed pointer may be
    // accessed using either the '->' operator, or the dereference operator
    // (operator '*').  The specified 'TARGET_TYPE' may be 'const'-qualified,
    // but may not be 'volatile'-qualified, nor may it be a reference type.
    //
    // A managed pointer may be *empty*, in which case it neither refers to a
    // target object nor owns a managed object.  An empty managed pointer is
    // the equivalent of a null pointer: Such a managed pointer is not
    // de-referenceable, and tests as 'false' in boolean expressions.
    //
    // A managed pointer for which the managed object is not the same object as
    // the target is said to *alias* the managed object (see the section
    // "Aliasing" in the component-level documentation).

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdema_ManagedPtr,
                                  bslalg::TypeTraitHasPointerSemantics,
                                  bslalg::TypeTraitBitwiseMoveable);

    // INTERFACE TYPES

    typedef bdema_ManagedPtrDeleter::Deleter DeleterFunc;
        // Alias for a function-pointer type for functions used to destroy the
        // object managed by a 'bdema_ManagedPtr' object.

  private:
    // NOT IMPLEMENTED
    template <class MANAGED_TYPE>
    bdema_ManagedPtr(MANAGED_TYPE *, bsl::nullptr_t);
        // It is never defined behavior to pass a null pointer literal as a
        // factory, unless the specified 'ptr' is also a null pointer literal.

  private:
    // NOT IMPLEMENTED
    template <class MANAGED_TYPE, class COOKIE_TYPE>
    bdema_ManagedPtr(MANAGED_TYPE *, COOKIE_TYPE *, bsl::nullptr_t);
        // It is never defined behavior to pass a null literal as a deleter,
        // unless the 'object' pointer is also a null pointer literal.

  private:
    // NOT IMPLEMENTED
    template <class MANAGED_TYPE>
    void load(MANAGED_TYPE *, bsl::nullptr_t, bsl::nullptr_t);
    template <class COOKIE_TYPE>
    void load(TARGET_TYPE *, COOKIE_TYPE *, bsl::nullptr_t);
        // It is never defined behavior to pass a null literal as a deleter,
        // unless the 'object' pointer is also a null pointer literal.

  private:
    // NOT IMPLEMENTED
    void operator==(const bdema_ManagedPtr&) const;
    void operator!=(const bdema_ManagedPtr&) const;
        // These two operator overloads are declared as 'private' but never
        // defined in order to eliminate accidental equality comparisons that
        // would occur through the implicit conversion to 'BoolType'.  Note
        // that the return type of 'void' is chosen as it will often produce a
        // clearer error message than relying on the 'private' control failure.
        // Note that these private operators will not be needed with C++11,
        // where an 'explicit operator bool()' conversion operator would be
        // preferred.

    // FRIENDS
    template <class ALIASED_TYPE>
    friend class bdema_ManagedPtr;  // required only for alias support

  public:
    // CREATORS
    explicit bdema_ManagedPtr(bsl::nullptr_t = 0, bsl::nullptr_t = 0);
        // Create an empty managed pointer.  Note that this constructor is
        // necessary to match null-pointer literal arguments, in order to break
        // ambiguities and provide valid type deduction with the other
        // constructor templates in this class.

    template <class MANAGED_TYPE>
    explicit bdema_ManagedPtr(MANAGED_TYPE *ptr);
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that uses the currently installed default allocator to
        // destroy the managed object when invoked (e.g., when this managed
        // pointer object is destroyed), unless '0 == ptr', in which case
        // create an empty managed pointer.  The deleter will invoke the
        // destructor of 'MANAGED_TYPE' rather than the destructor of
        // 'TARGET_TYPE'.  This constructor will not compile unless
        // 'MANAGED_TYPE *' is convertible to 'TARGET_TYPE *'.  The behavior is
        // undefined unless the managed object (if any) can be destroyed by the
        // currently installed default allocator, or if the the lifetime of the
        // managed object is already managed by another object.  Note that this
        // behavior allows 'bdema_ManagedPtr' to be defined for 'void'
        // pointers, and to call the correct destructor for the managed object,
        // even if the destructor for 'TARGET_TYPE' is not declared as
        // 'virtual'.

    bdema_ManagedPtr(bdema_ManagedPtr& other);
        // Create a managed pointer having the same target object as the
        // specified 'other', and transfer the ownership of the object
        // managed by 'other' (if any) to this managed pointer, then reset
        // 'other' as empty.

    bdema_ManagedPtr(bslma::ManagedPtr_Ref<TARGET_TYPE> ref);
        // Create a managed pointer having the same target object as the
        // managed pointer referenced by the specified 'ref', and transfer
        // ownership of the managed object owned by the managed pointer
        // referenced by 'ref', then reset the managed pointer referenced by
        // 'ref' as empty.  This constructor is used to create a managed
        // pointer from a managed pointer rvalue, or from a managed pointer to
        // a "compatible" type, where "compatible" means a built-in conversion
        // from 'COMPATIBLE_TYPE *' to 'TARGET_TYPE *' is defined, e.g.,
        // 'derived *' -> 'base *', 'int *' -> 'const int *', or
        // 'anyType *' -> 'void *'.

    template <class ALIASED_TYPE>
    bdema_ManagedPtr(bdema_ManagedPtr<ALIASED_TYPE>& alias, TARGET_TYPE *ptr);
        // Create a managed pointer that takes ownership of the object managed
        // by the specified 'alias', but which uses the specified 'ptr' to
        // refer to its target object, unless '0 == ptr', in which case create
        // an empty managed pointer.  Reset 'alias' as empty if ownership of
        // its managed object is transferred.  The behavior is undefined if
        // 'alias' is empty, but '0 != ptr'.  Note that destroying or
        // re-assigning a managed pointer created with this constructor will
        // destroy the object originally managed by 'alias' (unless 'release'
        // is called first); the destructor for '*ptr' is not called directly.

    template <class MANAGED_TYPE, class FACTORY_TYPE>
    bdema_ManagedPtr(MANAGED_TYPE *ptr, FACTORY_TYPE *factory);
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that will call 'factory->deleteObject(ptr)' to destroy the
        // managed object when invoked (e.g., when this managed pointer object
        // is destroyed), unless '0 == ptr', in which case create an empty
        // managed pointer.  The deleter will invoke the destructor of
        // 'MANAGED_TYPE' rather than the destructor of 'TARGET_TYPE'.  This
        // constructor will not compile unless 'MANAGED_TYPE *' is convertible
        // to 'TARGET_TYPE *'.  The behavior is undefined unless the managed
        // object (if any) can be destroyed by the specified 'factory', or if
        // the the lifetime of the managed object is already managed by another
        // object.  Note that 'bslma::Allocator', and any class publicly and
        // unambiguously derived from 'bslma::Allocator', meets the
        // requirements for 'FACTORY_TYPE'.

    template <class FACTORY_TYPE>
    bdema_ManagedPtr(bsl::nullptr_t, FACTORY_TYPE *factory);
        // Create an empty managed pointer.  Note that the specified 'factory'
        // is ignored, as an empty managed pointer does not call its deleter.

    bdema_ManagedPtr(TARGET_TYPE *ptr, void *cookie, DeleterFunc deleter);
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that will invoke the specified 'deleter' with the address of
        // the currently managed object, and with the specified 'cookie' (that
        // the deleter can use for its own purposes), unless '0 == ptr', in
        // which case create an empty managed pointer.  This constructor will
        // not compile unless 'MANAGED_TYPE*' is convertible to 'TARGET_TYPE*'.
        // The deleter will invoke the destructor of 'MANAGED_TYPE' rather than
        // the destructor of 'TARGET_TYPE'.  The behavior is undefined if 'ptr'
        // is already managed by another object, or if
        // '0 == deleter && 0 != ptr'.  Note that this declaration is required
        // only because the deprecated overloads create an ambiguity in this
        // case.  It should be removed when the deprecated overloads are
        // removed.

    template <class MANAGED_TYPE>
    bdema_ManagedPtr(MANAGED_TYPE *ptr, void *cookie, DeleterFunc deleter);
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that will invoke the specified 'deleter' with the address of
        // the currently managed object, and with the specified 'cookie' (that
        // the deleter can use for its own purposes), unless '0 == ptr', in
        // which case create an empty managed pointer.  This constructor will
        // not compile unless 'MANAGED_TYPE*' is convertible to 'TARGET_TYPE*'.
        // The deleter will invoke the destructor of 'MANAGED_TYPE' rather than
        // the destructor of 'TARGET_TYPE'.  The behavior is undefined if 'ptr'
        // is already managed by another object, or if
        // '0 == deleter && 0 != ptr'.

    template <class MANAGED_TYPE, class MANAGED_BASE>
    bdema_ManagedPtr(MANAGED_TYPE *ptr,
                     void         *cookie,
                     void        (*deleter)(MANAGED_BASE*, void*));
        // [!DEPRECATED!] Instead, use:
        //..
        //  template <class MANAGED_TYPE>
        //  bdema_ManagedPtr(MANAGED_TYPE *ptr,
        //                   void         *cookie,
        //                   DeleterFunc   deleter);
        //..
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that will invoke the specified 'deleter' with the address of
        // the currently managed object, and with the specified 'cookie' (that
        // the deleter can use for its own purposes), unless '0 == ptr', in
        // which case create an empty managed pointer.  This constructor will
        // not compile unless 'MANAGED_TYPE*' is convertible to 'TARGET_TYPE*',
        // and 'MANAGED_TYPE *' is convertible to 'MANAGED_BASE *'.  The
        // deleter will invoke the destructor of 'MANAGED_TYPE' rather than the
        // destructor of 'TARGET_TYPE'.  The behavior is undefined if 'ptr' is
        // already managed by another object, or if '0 == deleter && 0 != ptr'.
        // Note that this constructor is needed only to avoid ambiguous type
        // deductions when passing a null pointer literal as the 'cookie' when
        // the user passes a deleter taking a type other than 'void *' for its
        // object type.  Note that this function is *deprecated* as it relies
        // on undefined compiler behavior for its implementation (that luckily
        // performs as required on every platform supported by BDE).

    template <class MANAGED_TYPE,
              class MANAGED_BASE,
              class COOKIE_TYPE,
              class COOKIE_BASE>
    bdema_ManagedPtr(MANAGED_TYPE *ptr,
                     COOKIE_TYPE  *cookie,
                     void        (*deleter)(MANAGED_BASE*, COOKIE_BASE *));
        // [!DEPRECATED!] Instead, use:
        //..
        //  template <class MANAGED_TYPE>
        //  bdema_ManagedPtr(MANAGED_TYPE *ptr,
        //                   void         *cookie,
        //                   DeleterFunc   deleter);
        //..
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that will invoke the specified 'deleter' with the address of
        // the currently managed object, and with the specified 'cookie' (that
        // the deleter can use for its own purposes), unless '0 == ptr', in
        // which case create an empty managed pointer.  This constructor will
        // not compile unless 'MANAGED_TYPE*' is convertible to 'TARGET_TYPE*',
        // and 'MANAGED_TYPE *' is convertible to 'MANAGED_BASE *'.  The
        // deleter will invoke the destructor of 'MANAGED_TYPE' rather than the
        // destructor of 'TARGET_TYPE'.  The behavior is undefined if 'ptr' is
        // already managed by another object, or if '0 == deleter && 0 != ptr'.
        // Note that this function is *deprecated* as it relies on undefined
        // compiler behavior for its implementation (that luckily performs as
        // required on every platform supported by BDE).

    ~bdema_ManagedPtr();
        // Destroy this managed pointer object.  Destroy the object managed by
        // this managed pointer by invoking the user-supplied deleter, unless
        // this managed pointer is empty, in which case the deleter will *not*
        // be called.

    // MANIPULATORS
    bdema_ManagedPtr& operator=(bdema_ManagedPtr& rhs);
        // If this object and the specified 'rhs' manage the same object,
        // return a reference to this managed pointer; otherwise destroy the
        // manged object owned by this managed pointer, then transfer ownership
        // of the managed object owned by the specified 'rhs', and set this
        // managed pointer to point to the target object currently referenced
        // by 'rhs', then reset 'rhs' as empty, and return a reference to this
        // managed pointer.

    bdema_ManagedPtr& operator=(bslma::ManagedPtr_Ref<TARGET_TYPE> ref);
        // If this object and the managed pointer reference by the specified
        // 'ref' manage the same object, return a reference to this managed
        // pointer; otherwise destroy the manged object owned by this managed
        // pointer, then transfer ownership of the managed object owned by the
        // managed pointer referenced by 'ref', and set this managed pointer to
        // point to the target object currently referenced the managed pointer
        // referenced by 'ref'; then reset the managed pointer referenced by
        // 'ref' as empty, and return a reference to this managed pointer.
        // This operator is (implicitly) used to assign from a managed pointer
        // rvalue, or from a managed pointer to a "compatible" type, where
        // "compatible" means a built-in conversion from 'MANAGED_TYPE *' to
        // 'TARGET_TYPE *' is defined, e.g., 'derived *' -> 'base *',
        // 'T *' -> 'const T *', or 'T *' -> 'void *'.

    bdema_ManagedPtr& operator=(bsl::nullptr_t);
        // Destroy the current managed object (if any) and reset this managed
        // pointer as empty.

    template <class REFERENCED_TYPE>
    operator bslma::ManagedPtr_Ref<REFERENCED_TYPE>();
        // Return a managed pointer reference, referring to this object.  Note
        // that this conversion operator is used implicitly to allow the
        // construction of managed pointers from rvalues because temporaries
        // cannot be passed by modifiable reference.

    void load(bsl::nullptr_t = 0, bsl::nullptr_t = 0);
        // Destroy the current managed object (if any) and reset this managed
        // pointer as empty.

    template <class MANAGED_TYPE>
    void load(MANAGED_TYPE *ptr);
        // Destroy the currently managed object, if any.  Then, set the target
        // object of this managed pointer to be that referenced by the
        // specified 'ptr', take ownership of '*ptr' as the currently managed
        // object, and set a deleter that uses the currently installed default
        // allocator to destroy the managed object when invoked (e.g., when
        // this managed pointer object is destroyed), unless '0 == ptr', in
        // which case reset this managed pointer as empty.  The deleter will
        // invoke the destructor of 'MANAGED_TYPE' rather than the destructor
        // of 'TARGET_TYPE'.  This function will not compile unless
        // 'MANAGED_TYPE *' is convertible to 'TARGET_TYPE *'.  The behavior is
        // undefined unless the managed object (if any) can be destroyed by the
        // currently installed default allocator, or if the the lifetime of the
        // managed object is already managed by another object.

    template <class MANAGED_TYPE, class FACTORY_TYPE>
    void load(MANAGED_TYPE *ptr, FACTORY_TYPE *factory);
        // Destroy the currently managed object, if any.  Then, set the target
        // object of this managed pointer to be that referenced by the
        // specified 'ptr', take ownership of '*ptr' as the currently managed
        // object, and set a deleter that calls 'factory->deleteObject(ptr)' to
        // destroy the managed object when invoked (e.g., when this managed
        // pointer object is destroyed), unless '0 == ptr', in which case reset
        // this managed pointer as empty.  The deleter will invoke the
        // destructor of 'MANAGED_TYPE' rather than the destructor of
        // 'TARGET_TYPE'.  This function will not compile unless
        // 'MANAGED_TYPE *' is convertible to 'TARGET_TYPE *'.  The behavior is
        // undefined unless the managed object (if any) can be destroyed by the
        // specified 'factory',  or if '0 == factory && 0 != ptr', or if the
        // the lifetime of the managed object is already managed by another
        // object.  Note that 'bslma::Allocator', and any class publicly and
        // unambiguously derived from 'bslma::Allocator', meets the
        // requirements for 'FACTORY_TYPE'.

    template <class FACTORY_TYPE>
    void load(bsl::nullptr_t, FACTORY_TYPE *factory);
        // Destroy the current managed object (if any) and reset this managed
        // pointer as empty.  Note that the specified 'factory' will be
        // ignored, as empty managed pointers do not invoke a deleter.

    template <class MANAGED_TYPE>
    void load(MANAGED_TYPE *ptr, void *cookie, DeleterFunc deleter);
        // Destroy the currently managed object, if any.  Then, set the target
        // object of this managed pointer to be that referenced by the
        // specified 'ptr', take ownership of '*ptr' as the currently managed
        // object, and set a deleter that will invoke the specified 'deleter'
        // with the address of the currently managed object, and with the
        // specified 'cookie' (that the deleter can use for its own purposes),
        // unless '0 == ptr', in which case reset this managed pointer as
        // empty.  The behavior is undefined if 'ptr' is already managed by
        // another object, or if '0 == deleter && 0 != ptr'.  Note that GCC 3.4
        // and earlier versions have a bug in template type deduction/overload
        // resolution that causes ambiguities if this signature is available.
        // This function will be restored on that platform once the deprecated
        // signatures are finally removed.

    void load(TARGET_TYPE *ptr, void *cookie, DeleterFunc deleter);
        // Destroy the currently managed object, if any.  Then, set the target
        // object of this managed pointer to be that referenced by the
        // specified 'ptr', take ownership of '*ptr' as the currently managed
        // object, and set a deleter that will invoke the specified 'deleter'
        // with the address of the currently managed object, and with the
        // specified 'cookie' (that the deleter can use for its own purposes),
        // unless '0 == ptr', in which case reset this managed pointer as
        // empty.  The behavior is undefined if 'ptr' is already managed by
        // another object, or if '0 == deleter && 0 != ptr'.  Note that this
        // declaration is required only because the deprecated overloads create
        // an ambiguity for this specific this case.  It should be removed when
        // the deprecated overloads are removed.

    template <class MANAGED_TYPE, typename COOKIE_TYPE>
    void load(MANAGED_TYPE *ptr, COOKIE_TYPE *cookie, DeleterFunc deleter);

    template <class MANAGED_TYPE, class MANAGED_BASE>
    void load(MANAGED_TYPE *ptr,
              void         *cookie,
              void        (*deleter)(MANAGED_BASE *, void*));

    template <class MANAGED_TYPE,
              class MANAGED_BASE,
              class COOKIE_TYPE,
              class COOKIE_BASE>
    void load(MANAGED_TYPE *ptr,
              COOKIE_TYPE  *factory,
              void        (*deleter)(MANAGED_BASE*, COOKIE_BASE *));
        // [!DEPRECATED!] Instead, use:
        //..
        //  template <class MANAGED_TYPE>
        //  void load(MANAGED_TYPE *ptr,
        //            void         *cookie,
        //            DeleterFunc   deleter);
        //..
        // Destroy the currently managed object, if any.  Then, set the target
        // object of this managed pointer to be that referenced by the
        // specified 'ptr', take ownership of '*ptr' as the currently managed
        // object, and set a deleter that will invoke the specified 'deleter'
        // with the address of the currently managed object, and with the
        // specified 'cookie' (that the deleter can use for its own purposes),
        // unless '0 == ptr', in which case reset this managed pointer as
        // empty.  The behavior is undefined if 'ptr' is already managed by
        // another object, or if '0 == deleter && 0 != ptr'.  Note that
        // this function is *deprecated* as it relies on undefined compiler
        // behavior for its implementation, but luckily perform as required for
        // all currently supported platforms; on platforms where the
        // non-deprecated overload is not available (e.g., GCC 3.4) code should
        // be written as if it were available, as an appropriate (deprecated)
        // overload will be selected with the correct (non-deprecated)
        // behavior.

    bsl::pair<TARGET_TYPE *, bdema_ManagedPtrDeleter> release();
        // Return a raw pointer to the current target object (if any) and the
        // deleter for the currently managed object, and reset this managed
        // pointer as empty.  It is undefined behavior to run the returned
        // deleter unless the returned pointer to target object is not null.

    // BASE CLASS METHODS

    using bslma::ManagedPtr<TARGET_TYPE>::clear;
    using bslma::ManagedPtr<TARGET_TYPE>::deleter;
    using bslma::ManagedPtr<TARGET_TYPE>::ptr;
    using bslma::ManagedPtr<TARGET_TYPE>::swap;
    using bslma::ManagedPtr<TARGET_TYPE>::operator*;
    using bslma::ManagedPtr<TARGET_TYPE>::operator->;
};

template <class TARGET_TYPE>
void swap(bdema_ManagedPtr<TARGET_TYPE>& a, bdema_ManagedPtr<TARGET_TYPE>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.

                    // ==================================
                    // struct bdema_ManagedPtrNoOpDeleter
                    // ==================================

struct bdema_ManagedPtrUtil {
    // This utility class provides a general no-op deleter, which is useful
    // when creating managed pointers to stack-allocated objects.

    static void noOpDeleter(void *, void *);
        // Deleter function that does nothing.
};

                     // =================================
                     // struct bdema_ManagedPtrNilDeleter
                     // =================================

template <class TARGET_TYPE>
struct bdema_ManagedPtrNilDeleter {
    // [!DEPRECATED!] Use 'bdema_ManagedPtrNoOpDeleter' instead.
    // This utility class provides a general no-op deleter, which is useful
    // when creating managed pointers to stack-allocated objects.  Note that
    // the non-template class 'bdema_ManagedPtrUtil' should be used in
    // preference to this deprecated class, avoiding both template bloat and
    // undefined behavior.

    static void deleter(void *, void *);
        // Deleter function that does nothing.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // ----------------------
                           // class bdema_ManagedPtr
                           // ----------------------

template <class TARGET_TYPE>
class bdema_ManagedPtr<volatile TARGET_TYPE>;
    // This specialization is declared but not defined, in order to provide an
    // early compile-fail check to catch misuse of managed pointer to volatile
    // types, which is explicitly called out as not supported in the primary
    // class template contract.

template <class TARGET_TYPE>
class bdema_ManagedPtr<TARGET_TYPE &>;
    // This specialization is declared but not defined, in order to provide an
    // early compile-fail check to catch misuse of managed pointer to reference
    // types, which is explicitly called out as not supported in the primary
    // class template contract.

// CREATORS
template <class TARGET_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(bsl::nullptr_t, bsl::nullptr_t)
: bslma::ManagedPtr<TARGET_TYPE>()
{
}

template <class TARGET_TYPE>
template <class FACTORY_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(bsl::nullptr_t, FACTORY_TYPE *)
: bslma::ManagedPtr<TARGET_TYPE>()
{
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(MANAGED_TYPE *ptr)
: bslma::ManagedPtr<TARGET_TYPE>(ptr)
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
}

template <class TARGET_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(
                                         bslma::ManagedPtr_Ref<TARGET_TYPE> ref)
: bslma::ManagedPtr<TARGET_TYPE>(ref)
{
}

template <class TARGET_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(bdema_ManagedPtr& other)
: bslma::ManagedPtr<TARGET_TYPE>(other)
{
}

template <class TARGET_TYPE>
template <class ALIASED_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(
                                        bdema_ManagedPtr<ALIASED_TYPE>&  alias,
                                        TARGET_TYPE                     *ptr)
: bslma::ManagedPtr<TARGET_TYPE>(alias, ptr)
{
//  This assertion is no longer valid, as 'alias' is moved by base constructor. 
//  BSLS_ASSERT_SAFE(0 != alias.ptr() || 0 == ptr);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE, class FACTORY_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(MANAGED_TYPE *ptr,
                                                FACTORY_TYPE *factory)
: bslma::ManagedPtr<TARGET_TYPE>(ptr, factory)
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);
}

template <class TARGET_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(TARGET_TYPE *ptr,
                                                void        *cookie,
                                                DeleterFunc  deleter)
: bslma::ManagedPtr<TARGET_TYPE>(ptr, cookie, deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(MANAGED_TYPE *ptr,
                                                void         *cookie,
                                                DeleterFunc   deleter)
: bslma::ManagedPtr<TARGET_TYPE>(ptr, cookie, deleter)
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));

    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE, class MANAGED_BASE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(MANAGED_TYPE *ptr,
                                         void                *cookie,
                                         void (*deleter)(MANAGED_BASE*, void*))
: bslma::ManagedPtr<TARGET_TYPE>(ptr, cookie, deleter)
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *,
                                       const MANAGED_BASE *>::VALUE));

    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE,
          class MANAGED_BASE,
          class COOKIE_TYPE,
          class COOKIE_BASE>
inline
bdema_ManagedPtr<TARGET_TYPE>::bdema_ManagedPtr(
                          MANAGED_TYPE *ptr,
                          COOKIE_TYPE  *cookie,
                          void        (*deleter)(MANAGED_BASE*, COOKIE_BASE *))
: bslma::ManagedPtr<TARGET_TYPE>(ptr, cookie, deleter)
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *,
                                       const MANAGED_BASE *>::VALUE));
    BSLMF_ASSERT((bslmf::IsConvertible<COOKIE_TYPE *, COOKIE_BASE *>::VALUE));

    // Note that the undefined behavior embodied in the 'reinterpret_cast'
    // above could be removed by inserting an additional forwarding function
    // truly of type 'DeleterFunc' which 'reinterpret_cast's each pointer
    // argument as part of its forwarding behavior.  We choose not to do this
    // on the grounds of simple efficiency, and there is currently no known
    // supported compiler that we use where this does not work as desired.

    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class TARGET_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::~bdema_ManagedPtr()
{
}

template <class TARGET_TYPE>
inline
void bdema_ManagedPtr<TARGET_TYPE>::load(bsl::nullptr_t, bsl::nullptr_t)
{
    this->clear();
}

template <class TARGET_TYPE>
inline
void bdema_ManagedPtr<TARGET_TYPE>::load(TARGET_TYPE *ptr,
                                         void        *cookie,
                                         DeleterFunc  deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    bslma::ManagedPtr<TARGET_TYPE>::load(ptr, cookie, deleter);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE>
inline
void bdema_ManagedPtr<TARGET_TYPE>::load(MANAGED_TYPE *ptr,
                                         void         *cookie,
                                         DeleterFunc   deleter)
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    bslma::ManagedPtr<TARGET_TYPE>::load(ptr, cookie, deleter);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE, typename COOKIE_TYPE>
inline
void bdema_ManagedPtr<TARGET_TYPE>::load(MANAGED_TYPE *ptr,
                                         COOKIE_TYPE  *cookie,
                                         DeleterFunc   deleter)
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    bslma::ManagedPtr<TARGET_TYPE>::load(ptr, cookie, deleter);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE>
inline
void bdema_ManagedPtr<TARGET_TYPE>::load(MANAGED_TYPE *ptr)
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));

    bslma::ManagedPtr<TARGET_TYPE>::load(ptr);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE, class FACTORY_TYPE>
inline
void
bdema_ManagedPtr<TARGET_TYPE>::load(MANAGED_TYPE *ptr, FACTORY_TYPE *factory)
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);

    bslma::ManagedPtr<TARGET_TYPE>::load(ptr, factory);
}

template <class TARGET_TYPE>
template <class FACTORY_TYPE>
inline
void
bdema_ManagedPtr<TARGET_TYPE>::load(bsl::nullptr_t, FACTORY_TYPE *)
{
    this->clear();
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE, class MANAGED_BASE>
inline
void bdema_ManagedPtr<TARGET_TYPE>::load(MANAGED_TYPE *ptr,
                                        void          *cookie,
                                        void (*deleter)(MANAGED_BASE *, void*))
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
    BSLMF_ASSERT((!bslmf::IsVoid<MANAGED_BASE>::VALUE));
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *,
                                       MANAGED_BASE *>::VALUE));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    bslma::ManagedPtr<TARGET_TYPE>::load(ptr, cookie, deleter);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE,
          class MANAGED_BASE,
          class COOKIE_TYPE,
          class COOKIE_BASE>
inline
void bdema_ManagedPtr<TARGET_TYPE>::load(MANAGED_TYPE *ptr,
                                 COOKIE_TYPE          *cookie,
                                 void (*deleter)(MANAGED_BASE*, COOKIE_BASE *))
{
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
    BSLMF_ASSERT((bslmf::IsConvertible<MANAGED_TYPE *,
                                       MANAGED_BASE *>::VALUE));
    BSLMF_ASSERT((bslmf::IsConvertible<COOKIE_TYPE *, COOKIE_BASE *>::VALUE));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    bslma::ManagedPtr<TARGET_TYPE>::load(ptr, cookie, deleter);
}

template <class TARGET_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>&
bdema_ManagedPtr<TARGET_TYPE>::operator=(bdema_ManagedPtr& rhs)
{
    bslma::ManagedPtr<TARGET_TYPE>::operator=(rhs);
    return *this;
}

template <class TARGET_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>& 
bdema_ManagedPtr<TARGET_TYPE>::operator=(
                                        bslma::ManagedPtr_Ref<TARGET_TYPE> ref)
{
    bslma::ManagedPtr<TARGET_TYPE>::operator=(ref);
    return *this;
}

template <class TARGET_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>&
bdema_ManagedPtr<TARGET_TYPE>::operator=(bsl::nullptr_t)
{
    this->clear();
    return *this;
}

template <typename TARGET_TYPE>
bsl::pair<TARGET_TYPE *, bdema_ManagedPtrDeleter>
bdema_ManagedPtr<TARGET_TYPE>::release()
{
    bslma::ManagedPtrDeleter deleter;
    TARGET_TYPE *ptr = bslma::ManagedPtr<TARGET_TYPE>::release(&deleter);    
    return bsl::pair<TARGET_TYPE *, bdema_ManagedPtrDeleter>(ptr, deleter);
}

template <class TARGET_TYPE>
template <class REFERENCED_TYPE>
inline
bdema_ManagedPtr<TARGET_TYPE>::operator
                                       bslma::ManagedPtr_Ref<REFERENCED_TYPE>()
{
    BSLMF_ASSERT((bslmf::IsConvertible<TARGET_TYPE *,
                                       REFERENCED_TYPE *>::VALUE));

    return bslma::ManagedPtr<TARGET_TYPE>::operator
                                      bslma::ManagedPtr_Ref<REFERENCED_TYPE>();
}

                      // --------------------------------
                      // class bdema_ManagedPtrNilDeleter
                      // --------------------------------

template <class TARGET_TYPE>
inline
void bdema_ManagedPtrNilDeleter<TARGET_TYPE>::deleter(void *, void *)
{
}

}  // close namespace BloombergLP

// FREE FUNCTIONS
template <class TARGET_TYPE>
inline
void BloombergLP::swap(bdema_ManagedPtr<TARGET_TYPE>& a,
                       bdema_ManagedPtr<TARGET_TYPE>& b)
{
    a.swap(b);
}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
