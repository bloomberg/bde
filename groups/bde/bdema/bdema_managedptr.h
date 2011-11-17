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
//            bdema_ManagedPtr: proctor for automatic memory management
// bdema_ManagedPtrNoOpDeleter: deleter for stack-allocated objects
//
//@AUTHOR: Ilougino Rocha (irocha), Pablo Halpern (phalpern),
//         Alisdair Meredith (ameredith1@bloomberg.net)
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
// library containers.
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
// E.g., 'bslma_Allocator' is a commonly used factory, and the currently
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
// destroyed, the original managed object will be passed to the deleter. (See
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
// compute its 'area'.
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
// Next we implement the methods for a 'Circle'.
//..
//  Circle::Circle(double r)
//  : d_radius(r)
//  {
//  }
//
//  double Circle::area() const {
//      return 3.141592653589793238462 * d_radius * d_radius;
//  }
//..
// Then we implement the methods for a 'Square'..
//..
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
//      bslma_Allocator *alloc = bslma_Default::defaultAllocator();
//      bdema_ManagedPtr<Shape> result;
//      switch (kind) {
//      case Shapes::SHAPE_CIRCLE : {
//              Circle *circ = new(*alloc)Circle(dimension);
//              result.load(circ);
//              break;
//          }
//      case Shapes::SHAPE_SQUARE : {
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
// accurate 'operator==' with floating-point quantities.
//..
//  void testShapes()
//  {
//      bdema_ManagedPtr<Shape> shape = makeShape(Shapes::SHAPE_CIRCLE, 1.0);
//      ASSERT(0 != shape);
//      ASSERT(3.141592653589793238462 == shape->area());
//
//      shape = makeShape(Shapes::SHAPE_SQUARE, 2.0);
//      ASSERT(0 != shape);
//      ASSERT(4.0 == shape->area());
//  }
//..
// Next, we observe that as we are creating objects dynamically, we should pass
// an allocator to the 'makeShape' function, rather than simply accepting the
// default allocator each time.  Note that when we do this, we pass the user's
// allocator to the 'bdema_ManagedPtr' object as the "factory".
//..
//  bdema_ManagedPtr<Shape> makeShape(Shapes::VALUES   kind,
//                                    double           dimension,
//                                    bslma_Allocator *allocator)
//  {
//      bslma_Allocator *alloc = bslma_Default::allocator(allocator);
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
//      bslma_TestAllocator ta("object");
//
//      bdema_ManagedPtr<Shape> shape =
//                                   makeShape(Shapes::SHAPE_CIRCLE, 1.0, &ta);
//      ASSERT(0 != shape);
//      ASSERT(3.141592653589793238462 == shape->area());
//
//      shape = makeShape(Shapes::SHAPE_SQUARE, 3.0, &ta);
//      ASSERT(0 != shape);
//      ASSERT(9.0 == shape->area());
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
//      static double getQuote() // From ticker plant. Simulated here
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
//  getFirstQuoteLargerThan(double threshold, bslma_Allocator *allocator)
//  {
//      ASSERT( END_QUOTE < 0 && 0 <= threshold );
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
//      bslma_TestAllocator ta;
//      bdema_ManagedPtr<double> result = getFirstQuoteLargerThan(16.00, &ta);
//      ASSERT(*result > 16.00);
//      ASSERT(1 == ta.numBlocksInUse());
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
//          ASSERT(quote < *result);
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
//      ASSERT(0 == ta.numBlocksInUse());
//      ASSERT(0 == ta.numBytesInUse());
//
//      return 0;
//  }
//..
//
///Example 3: Factories and Deleters
///- - - - - - - - - - - - - - - - -
// Suppose we want to track the number of objects currently managed by
// 'bdema_ManagedPtr' objects.
//
// First we define a factory type, that holds an allocator and a usage-counter.
//..
//  class CountedFactory {
//      // DATA
//      int              d_count;
//      bslma_Allocator *d_allocator;
//..
// Then we note that such a type cannot be sensibly copied, as the notion of
// 'count' becomes confused.
//..
//      // NOT IMPLEMENTED
//      CountedFactory(const CountedFactory&);
//      CountedFactory& operator=(const CountedFactory&);
//..
// Next we declare a public constructor that can be used to create objects of
// this factory type, and a public destructor.
//..
//    public:
//      // CREATORS
//      explicit CountedFactory(bslma_Allocator *alloc = 0);
//          // Create a 'CountedFactory' object which uses the supplied
//          // allocator 'alloc'
//
//      ~CountedFactory();
//          // Destroy this object.
//..
// Then we provide the ability for the factory to create objects of any type
// requested by the user, using the allocator supplied at construction time.
//..
//      // MANIPULATORS
//      template <class TYPE>
//      TYPE *createObject();
//          // Return a pointer to a newly allocated object of type 'TYPE'
//          // created using its default constructor.  Memory for the object
//          // is supplied by the allocator supplied to this factory's
//          // constructor, and the count of valid object is incremented.
//..
// Now we provide the 'deleteObject' function required of factory types to be
// used with 'bdema_ManagedPtr'.
//..
//      template <class TYPE>
//      void deleteObject(const TYPE *target);
//          // Destroy the object pointed to be 'target' and reclaim the
//          // memory.  Decrement the count of currently valid objects.
//..
// Then we round out the class with the ability to query the 'count' of
// currently allocated objects.
//..
//      // ACCESSORS
//      int count() const;
//          // Return the number of currently valid objects allocated by this
//          // factory.
//  };
//..
// Next we define the operations declared by the class.
//..
//  CountedFactory::CountedFactory(bslma_Allocator *alloc)
//  : d_count(0)
//  , d_allocator(bslma_Default::allocator(alloc))
//  {
//  }
//
//  CountedFactory::~CountedFactory()
//  {
//      ASSERT(0 == d_count);
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
// Then we can create a test function to illustrate how such a factory would be
// used with 'bdema_ManagedPtr'.
//..
//  void testCountedFactory()
//  {
//..
// Next we declare a test allocator, and an object of our 'CountedFactory' type
// using that allocator.
//..
//      bslma_TestAllocator ta;
//      CountedFactory cf(&ta);
//..
// Then we open a new local scope and declare an array of managed pointers.  We
// need a local scope in order to observe the behavior of the destructors at
// end of the scope, and use an array as an easy way to count more than one
// object.
//..
//      {
//          bdema_ManagedPtr<int> pData[4];
//..
// Next we load each managed pointer in the array with a new 'int' using our
// factory 'cf' and assert that the factory 'count' is correct after each new
// 'int' is created.
//..
//          int i = 0;
//          while (i != 4) {
//              pData[i++].load(cf.createObject<int>(), &cf);
//              ASSERT(cf.count() == i);
//          }
//..
// Then we 'clear' the contents of a single managed pointer in the array, and
// assert that the factory 'count' is appropriately reduced.
//..
//          pData[1].clear();
//          ASSERT(3 == cf.count());
//..
// Next we 'load' a managed pointer with another new 'int' value, again using
// 'cf' as the factory, and assert that the 'count' of valid objects remains
// the same (destroy one object and add another).
//..
//          pData[2].load(cf.createObject<int>(), &cf);
//          ASSERT(3 == cf.count());
//      }
//..
// Finally, we allow the array of managed pointers to fall out of scope and
// confirm that when all managed objects are destroyed, the factory 'count'
// falls to zero, and does not overshoot.
//..
//      ASSERT(0 == cf.count());
//  }
//..
///Example 4: Type Casting
///- - - - - - - - - - - -
// 'bdema_ManagedPtr' objects can be implicitly and explicitly cast to
// different types in the same way that native pointers can.
//
///Implicit Casting
///-  -  -  -  -  -
// As with native pointers, a pointer of the type 'B' that is derived from the
// type 'A', can be directly assigned a 'bcema_SharedPtr' of 'A'.
//
// First, consider the following code snippets:
//..
//  void implicitCastingExample()
//  {
//..
// If the statements:
//..
//      bslma_TestAllocator localDefaultTa;
//      bslma_TestAllocator localTa;
//
//      bslma_DefaultAllocatorGuard guard(&localDefaultTa);
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
//          ASSERT(!a_mp1 && !b_mp1);
//
//          a_mp1 = b_mp1;      // conversion assignment of nil ptr to nil
//          ASSERT(!a_mp1 && !b_mp1);
//
//          B *b_p2 = new (localDefaultTa) B(&numdels);
//          bdema_ManagedPtr<B> b_mp2(b_p2);    // default allocator
//          ASSERT(!a_mp1 && b_mp2);
//
//          a_mp1 = b_mp2;      // conversion assignment of nonnil ptr to nil
//          ASSERT(a_mp1 && !b_mp2);
//
//          B *b_p3 = new (localTa) B(&numdels);
//          bdema_ManagedPtr<B> b_mp3(b_p3, &localTa);
//          ASSERT(a_mp1 && b_mp3);
//
//          a_mp1 = b_mp3;      // conversion assignment of nonnil to nonnil
//          ASSERT(a_mp1 && !b_mp3);
//
//          a_mp1 = b_mp3;      // conversion assignment of nil to nonnil
//          ASSERT(!a_mp1 && !b_mp3);
//
//          // constructor conversion init with nil
//          bdema_ManagedPtr<A> a_mp4(b_mp3, b_mp3.ptr());
//          ASSERT(!a_mp4 && !b_mp3);
//
//          // constructor conversion init with nonnil
//          B *p_b5 = new (localTa) B(&numdels);
//          bdema_ManagedPtr<B> b_mp5(p_b5, &localTa);
//          bdema_ManagedPtr<A> a_mp5(b_mp5, b_mp5.ptr());
//          ASSERT(a_mp5 && !b_mp5);
//          ASSERT(a_mp5.ptr() == p_b5);
//
//          // constructor conversion init with nonnil
//          B *p_b6 = new (localTa) B(&numdels);
//          bdema_ManagedPtr<B> b_mp6(p_b6, &localTa);
//          bdema_ManagedPtr<A> a_mp6(b_mp6);
//          ASSERT(a_mp6 && !b_mp6);
//          ASSERT(a_mp6.ptr() == p_b6);
//
//          struct S {
//              int d_i[10];
//          };
//
//          ASSERT(200 == numdels);
//      }
//
//      ASSERT(400 == numdels);
//  } // implicitCastingExample()
//..
//
///Explicit Casting
///-  -  -  -  -  -
// Through "aliasing", a managed pointer of any type can be explicitly cast
// to a managed pointer of any other type using any legal cast expression.
// For example, to static-cast a managed pointer of type A to a shared pointer
// of type B, one can simply do the following:
//..
//  void explicitCastingExample() {
//
//      bdema_ManagedPtr<A> a_mp;
//      bdema_ManagedPtr<B> b_mp1(a_mp, static_cast<B*>(a_mp.ptr()));
//      //..
//      // or even use the less safe "C"-style casts:
//      //..
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
//          ASSERT(!aPtr);
//          *castSucceeded = true;
//      }
//      else {
//          ASSERT(aPtr);
//          *castSucceeded = false;
//      }
//  }
//..
// If the value of 'aPtr' can be dynamically cast to 'B*' then ownership is
// transferred to 'bPtr', otherwise 'aPtr' is to be modified.  As previously
// stated, the managed object will be destroyed correctly regardless of how
// it is cast.
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTRDELETER
#include <bdema_managedptrdeleter.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR_FACTORYDELETER
#include <bdema_managedptr_factorydeleter.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTR_MEMBERS
#include <bdema_managedptr_members.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMF_ADDREFERENCE
#include <bslmf_addreference.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
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

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_UNSPECIFIEDBOOL
#include <bsls_unspecifiedbool.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

template <class BDEMA_TYPE>
class bdema_ManagedPtr_Ref;

                           // ======================
                           // class bdema_ManagedPtr
                           // ======================

template <class BDEMA_TYPE>
class bdema_ManagedPtr {
    // This class is a "smart pointer" that supports sole ownership of an
    // object of the specified parameter type, 'BDEMA_TYPE'.  A managed pointer
    // ensures that the object it manages is destroyed when the managed pointer
    // is destroyed, or re-assigned, using the appropriate deletion method.
    // The object (of the specified parameter type 'BDEMA_TYPE') pointed to by
    // a managed pointer instance may be accessed directly using the '->'
    // operator, or the dereference operator (operator '*') can be used to get
    // a reference to that object.  The specified 'BDEMA_TYPE' may be
    // 'const'-qualified, but may not be 'volatile'-qualified, nor may it be a
    // reference type.
    //
    // A managed pointer that does not own an object is in an *unset* state.
    // Such a state is the equivalent of a null pointer: the managed pointer
    // object is not deferenceable; managed pointers of the same type compare
    // equal for all unset states, and are never equal to a managed pointer
    // that owns an object.
    //
    // Note that the object managed by a managed pointer instance is usually
    // the same as the object pointed to by that instance (of the same
    // 'BDEMA_TYPE'), but this need not always be true in the presence of
    // conversions or "aliasing": the object pointed-to, of the specified
    // parameter type 'BDEMA_TYPE', may differ from the managed object of type
    // 'OTHERTYPE' (see the section "Aliasing" in the component-level
    // documentation).  Nevertheless, both shall exist or else the managed
    // pointer is in an *unset* state.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdema_ManagedPtr,
                                  bslalg_TypeTraitHasPointerSemantics,
                                  bslalg_TypeTraitBitwiseMoveable);

    // INTERFACE TYPES

    typedef bdema_ManagedPtrDeleter::Deleter DeleterFunc;
        // Alias for a function-pointer type for functions used to destroy the
        // object managed by a 'bdema_ManagedPtr' object.

  private:
    // PRIVATE TYPES
    typedef typename bsls_UnspecifiedBool<bdema_ManagedPtr>::BoolType BoolType;
        // 'BoolType' is an alias for an unspecified type that is implicitly
        // convertible to 'bool', but will not promote to 'int'.  This (opaque)
        // type can be used as an "unspecified boolean type" for converting a
        // managed pointer to 'bool' in contexts such as 'if (mp) { ... }'
        // without actually having a conversion to 'bool' or being less-than
        // comparable (either of which would also enable undesirable implicit
        // comparisons of managed pointers to 'int' and less-than comparisons).

    // DATA
    bdema_ManagedPtr_Members d_members;  // State managed by this object

    // PRIVATE UTILITY FUNCTION

    static void *stripBasePointerType(BDEMA_TYPE *ptr);
        // Return the value of the specified 'ptr' as a 'void *', after
        // stripping all 'const' and 'volatile' qualifiers from 'BDEMA_TYPE'.
        // This function avoids accidental type-safety errors when performing
        // the necessary sequence of casts.

    template <typename BDEMA_OTHER_TYPE>
    static void *stripCompletePointerType(BDEMA_OTHER_TYPE *ptr);
        // Return the value of the specified 'ptr' as a 'void *', after
        // stripping all 'const' and 'volatile' qualifiers from 'BDEMA_TYPE'.
        // This function avoids accidental type-safety errors when performing
        // the necessary sequence of casts.


  private:
    // NOT IMPLEMENTED
    template <class BDEMA_OTHER_TYPE>
    bdema_ManagedPtr(BDEMA_OTHER_TYPE *, bsl::nullptr_t);
        // It is never defined behavior to pass a null pointer literal as a
        // factory, unless the 'object' pointer is also a null pointer literal.

  private:
    // NOT IMPLEMENTED
    template <class BDEMA_FACTORY>
    bdema_ManagedPtr(bsl::nullptr_t, BDEMA_FACTORY *);
        // It is an error to pass a null pointer literal along with a non-null
        // factory.  If you really must create an empty managed pointer that
        // ignores the passed factory, pass a variable holding a null pointer
        // as the first argument.

  private:
    // NOT IMPLEMENTED
    template <class BDEMA_OTHER_TYPE, class BDEMA_COOKIE>
    bdema_ManagedPtr(BDEMA_OTHER_TYPE *,
                     BDEMA_COOKIE *,
                     bsl::nullptr_t);
        // It is never defined behavior to pass a null literal as a deleter,
        // unless the 'object' pointer is also a null pointer literal.

  private:
    // NOT IMPLEMENTED
    template <typename BDEMA_ANY_COOKIE>
    void load(BDEMA_TYPE *,
              BDEMA_ANY_COOKIE *,
              bsl::nullptr_t);
        // It is never defined behavior to pass a null literal as a deleter,
        // unless the 'object' pointer is also a null pointer literal.

  private:
    // NOT IMPLEMENTED
    template <class BDEMA_OTHER_TYPE>
    void load(BDEMA_OTHER_TYPE *,
              bsl::nullptr_t,
              bsl::nullptr_t);
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
    template <class OTHER>
    friend class bdema_ManagedPtr;  // required only for alias support

  public:
    // CREATORS
    explicit bdema_ManagedPtr(bsl::nullptr_t = 0, bsl::nullptr_t = 0);
        // Create a managed pointer that is in an unset state.  Note that this
        // constructor is necessary to match null-pointer literal arguments, in
        // order to break ambiguities and provide valid type deduction with the
        // other constructor templates in this class.

    template <class BDEMA_TARGET_TYPE>
    explicit bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr);
        // Create a managed pointer that manages the specified 'ptr' using the
        // current default allocator to destroy 'ptr' when this managed pointer
        // is destroyed or re-assigned, unless 'release' is called before then.
        // The object will be initialized to an unset state if '0 == ptr'.  The
        // deleter will invoke the destructor of 'BDEMA_TARGET_TYPE' and not
        // the destructor of 'BDEMA_TYPE'.  This allows 'bdema_ManagedPtr' to
        // be defined for 'void' pointers, and to call the correct destructor
        // for 'ptr', even if the destructor for 'BDEMA_TYPE' is not declared
        // as 'virtual'.  The behavior is undefined unless the object referred
        // to by 'ptr' can by destroyed by the current default allocator
        // (or '0 == ptr'), or if the the lifetime of object referred to by
        // 'ptr' is already managed by another object.  Note that this
        // constructor will not compile unless 'BDEMA_TARGET_TYPE *' is
        // convertible to 'BDEMA_TYPE *'.

    bdema_ManagedPtr(bdema_ManagedPtr& other);
        // Create a 'bdema_ManagedPtr' and transfer the ownership of the object
        // managed by the specified 'other' (if any) to this managed pointer.
        // Re-initialize 'other' to an unset state.

    bdema_ManagedPtr(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
        // Create a 'bdema_ManagedPtr' and transfer the value and ownership
        // from the managed pointer referred-to by the specified 'ref' object
        // to this managed pointer.  The managed pointer referred-to by 'ref'
        // will be re-initialized to an unset state.  This constructor is used
        // to construct from a managed pointer rvalue, or from a managed
        // pointer to a "compatible" type, where "compatible" means a built-in
        // conversion from 'OTHER_TYPE *' to 'BDEMA_TYPE *' is defined, e.g.,
        // 'derived *' -> 'base *', 'int *' -> 'const int *', or
        // 'any_type *' -> 'void *'.

    template <class BDEMA_OTHER_TYPE>
    bdema_ManagedPtr(bdema_ManagedPtr<BDEMA_OTHER_TYPE>& alias,
                     BDEMA_TYPE                         *ptr);
        // Create a managed pointer that takes ownership of the object managed
        // by the specified 'alias' pointer, but which has the specified 'ptr'
        // as its externally-visible value.  Re-initialize 'alias' to an unset
        // state unless '0 == ptr', in which case this object will be
        // initialized to an unset state and 'alias' continues to own its
        // managed object.  The value of 'ptr' is treated as an alias to all or
        // part of 'alias.ptr()'.  Note that destroying or re-assigning a
        // managed pointer created with this constructor will destroy the
        // object originally managed by 'alias' (unless 'release' is called
        // first); the destructor for '*ptr' is not called directly. Also note
        // that behavior is undefined if 'alias' is empty, but '0 != ptr'.
        // Further note that compilation will fail unless 'BDEMA_OTHER_TYPE *'
        // is convertible to 'BDEMA_TYPE*'.

    template <class BDEMA_TARGET_TYPE, class BDEMA_FACTORY>
    bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr, BDEMA_FACTORY *factory);
        // Create a managed pointer to manage the specified 'ptr' using the
        // specified 'factory' to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned (unless 'release' is called before then).
        // If '0 == ptr' then this object will be initialized to an unset
        // state.  The class 'BDEMA_FACTORY' can be any class that has a
        // 'deleteObject' method, which takes a single argument of type
        // 'BDEMA_TYPE *'.  The behavior is undefined unless '0 != factory', or
        // if 'ptr' is already managed by another object.  Note that
        // 'bslma_Allocator' or any class derived from 'bslma_Allocator' meets
        // the requirements for 'BDEMA_FACTORY'.

    bdema_ManagedPtr(BDEMA_TYPE *ptr, void *cookie, DeleterFunc deleter);
        // Create a managed pointer to manage the specified 'ptr' using the
        // specified 'deleter' to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned (unless 'release' is called before then).
        // If '0 == ptr', then this object will be initialized to an unset
        // state.  The behavior is undefined if 'ptr' is already managed by
        // another object, or if 'deleter' is null and 'ptr' is not null, or
        // if 'cookie' is null, 'ptr' is not null, and 'deleter' has undefined
        // behavior when passed a null cookie.  Note that this declaration is
        // required only because the deprecated overloads create an ambiguity
        // in this case.  It should be removed when the deprecated overloads
        // are removed.

    template <class BDEMA_TARGET_TYPE>
    bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr,
                     void              *cookie,
                     DeleterFunc        deleter);
        // Create a managed pointer to manage the specified 'ptr' using the
        // specified 'deleter' to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned (unless 'release' is called before then).
        // If '0 == ptr', then this object will be initialized to an unset
        // state.  The behavior is undefined if 'ptr' is already managed by
        // another object, or if 'deleter' is null and 'ptr' is not null, or if
        // 'cookie' is null, 'ptr' is not null, and 'deleter' has undefined
        // behavior when passed a null cookie.

    template <class BDEMA_TARGET_TYPE, class BDEMA_TARGET_BASE>
    bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr,
                     void *cookie,
                     void (*deleter)(BDEMA_TARGET_BASE*, void*));
        // [!DEPRECATED!] Instead, use:
        //..
        //  template <class BDEMA_TARGET_TYPE>
        //  void bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr,
        //                        void              *cookie,
        //                        DeleterFunc        deleter);
        //..
        //
        // Create a managed pointer to manage the specified 'ptr' using the
        // specified 'deleter' to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned (unless 'release' is called before then).
        // If '0 == ptr', then this object will be initialized to an unset
        // state.  The behavior is undefined unless '0 == cookie' and 'deleter'
        // accepts null pointers for its cookie.  The behavior is undefined if
        // 'ptr' is already managed by another object.  Note that this
        // constructor is needed only to avoid ambiguous type deductions when
        // passing a null pointer literal as the 'cookie' when the user passes
        // a deleter taking a type other than 'void *' for its object type.
        // Note that this function is *deprecated* as it relies on undefined
        // compiler behavior for its implementation (that happens to perform as
        // required on every known compiler).

    template <class BDEMA_TARGET_TYPE,
              class BDEMA_TARGET_BASE,
              class BDEMA_COOKIE,
              class BDEMA_COOKIE_BASE>
    bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr,
                     BDEMA_COOKIE      *cookie,
                     void (*deleter)(BDEMA_TARGET_BASE*, BDEMA_COOKIE_BASE *));
        // [!DEPRECATED!] Instead, use:
        //..
        //  template <class BDEMA_TARGET_TYPE>
        //  void bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr,
        //                        void              *cookie,
        //                        DeleterFunc        deleter);
        //..
        //
        // Create a managed pointer to manage the specified 'ptr' using the
        // specified 'deleter' and associated 'cookie' to destroy 'ptr' when
        // this managed pointer is destroyed or re-assigned (unless it is
        // released before then).  If 0 == 'ptr', then this object will be
        // initialized to an unset state.  The behavior is undefined if 'ptr'
        // is already managed by another object.  If '0 == cookie' then
        // behavior is undefined unless 'deleter' has defined behavior for null
        // cookies.  Note that this function is *deprecated* as it relies on
        // undefined compiler behavior for its implementation (that happens to
        // perform as required on every known compiler).

    ~bdema_ManagedPtr();
        // Destroy this managed pointer object.  Destroy the object managed by
        // this managed pointer by invoking the user-supplied deleter, unless
        // this managed pointer has an unset state, in which case the deleter
        // will *not* be called.

    // MANIPULATORS
    bdema_ManagedPtr& operator=(bdema_ManagedPtr& rhs);
        // Assign to this managed pointer the value and ownership of the
        // specified 'rhs' managed pointer, and return a reference to this
        // object.  Re-intitialize 'rhs' to an unset state and destroy the
        // previous contents of this pointer (if any) unless this pointer and
        // 'rhs' manage the same object.

    bdema_ManagedPtr& operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref);
        // Transfer the ownership of the object managed by the managed pointer
        // referenced by 'ref' to this managed pointer.  Re-initialize the
        // managed pointer referenced by 'ref' to an unset state, and destory
        // the object previously managed by this managed pointer (if any),
        // unless this object is the managed pointer referenced by 'ref'.  This
        // operator is (implicitly) used to assign from a managed pointer
        // rvalue, or from a managed pointer to a "compatible" type, where
        // "compatible" means a built-in conversion from 'OTHER_TYPE *' to
        // 'BDEMA_TYPE *' is defined, e.g., 'derived *' -> 'base *',
        // 'T *' -> 'const T *', or 'T *' -> 'void *'.

    template <class BDEMA_OTHER_TYPE>
    operator bdema_ManagedPtr_Ref<BDEMA_OTHER_TYPE>();
        // Return a managed pointer reference, referring to this object.  Note
        // that this conversion operator is used implicitly to allow the
        // construction of managed pointers from rvalues because temporaries
        // cannot be passed by modifiable reference.

    void clear();
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to an unset state.

    void load(bsl::nullptr_t =0, bsl::nullptr_t =0);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to an unset state.

    template <class BDEMA_TARGET_TYPE>
    void load(BDEMA_TARGET_TYPE *ptr);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the default
        // allocator to destroy 'ptr' when this managed pointer is destroyed
        // or re-assigned, unless 'release' is called before then.  If
        // '0 == 'ptr', then this object will be initialized to an unset state.
        // The behavior is undefined if 'ptr' is already managed by another
        // object.

    template <class BDEMA_TARGET_TYPE, class BDEMA_FACTORY>
    void load(BDEMA_TARGET_TYPE *ptr, BDEMA_FACTORY *factory);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'factory' deleter to destroy 'ptr' when this managed pointer is
        // destroyed or re-assigned, unless it is released before then.  If
        // '0 == ptr', then this object will be initialized to an unset state.
        // The behavior is undefined if 'ptr' is already managed by another
        // object or if '0 == factory && 0 != ptr'

    void load(BDEMA_TYPE *ptr, void *cookie, DeleterFunc  deleter);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'deleter' with arguments 'ptr' and the specified 'factory' to
        // destroy 'ptr' when this managed pointer is destroyed or re-assigned,
        // unless 'release' is called before then.  If '0 == ptr', then this
        // object will be re-initialized to an unset state.  The behavior is
        // undefined if 'ptr' is already managed by another object, or if
        // '0 == deleter' and '0 != ptr'.  Note that this declaration is
        // required only because the deprecated overloads create an ambiguity
        // in this case.  It should be removed when the deprecated overloads
        // are removed.

    template <class BDEMA_TARGET_TYPE>
    void load(BDEMA_TARGET_TYPE *ptr, void *cookie, DeleterFunc deleter);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'deleter' with arguments 'ptr' and the specified 'factory' to
        // destroy 'ptr' when this managed pointer is destroyed or re-assigned,
        // unless 'release' is called before then.  If '0 == ptr', then this
        // object will be re-initialized to an unset state.  The behavior is
        // undefined if 'ptr' is already managed by another object, or if
        // '0 == deleter && 0 != ptr'.

    template <class BDEMA_TARGET_TYPE, typename BDEMA_COOKIE>
    void load(BDEMA_TARGET_TYPE *ptr,
              BDEMA_COOKIE      *cookie,
              DeleterFunc        deleter);
        // [!DEPRECATED!] Instead, use:
        //..
        //  template <class BDEMA_TARGET_TYPE>
        //  void load(BDEMA_TARGET_TYPE *ptr,
        //            void              *cookie,
        //            DeleterFunc        deleter);
        //..
        //
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to manage the specified 'ptr' using the specified
        // 'deleter' with arguments 'ptr' and the specified 'factory' to
        // destroy 'ptr' when this managed pointer is destroyed or re-assigned,
        // unless it is released before then.  If '0 == ptr', then this object
        // will be initialized to an unset state. The behavior is undefined if
        // 'ptr' is already managed by another object.

    template <class BDEMA_TARGET_TYPE, class BDEMA_TARGET_BASE>
    void load(BDEMA_TARGET_TYPE *ptr,
              void              *cookie,
              void             (*deleter)(BDEMA_TARGET_BASE *, void*));
        // [!DEPRECATED!] Instead, use:
        //..
        //  template <class BDEMA_TARGET_TYPE>
        //  void load(BDEMA_TARGET_TYPE *ptr,
        //            void              *cookie,
        //            DeleterFunc        deleter);
        //..
        //
        // Destroy the current managed object (if any) and
        // re-initialize this managed pointer to manage the specified 'ptr'
        // using the specified 'deleter' with arguments 'ptr' and '0' when this
        // managed pointer is destroyed or re-assigned, unless it is released
        // before then.  If '0 == ptr', then this object will be initialized to
        // an unset state.  The behavior is undefined if 'ptr' is already
        // managed by another managed pointer, or if 'deleter' does not support
        // null pointer values in its second argument.  Note that this overload
        // is needed only to support passing a null pointer literal as the
        // factory object when the user passes a deleter taking 'BDEMA_TYPE'
        // rather than 'void *', and the factory type is not used.  Note that
        // this function is *deprecated* as it relies on undefined compiler
        // behavior for its implementation, that happens to perform as required
        // on every known compiler.

    template <class BDEMA_TARGET_TYPE,
              class BDEMA_TARGET_BASE,
              class BDEMA_COOKIE,
              class BDEMA_COOKIE_BASE>
    void
    load(BDEMA_TARGET_TYPE *ptr,
         BDEMA_COOKIE      *factory,
         void             (*deleter)(BDEMA_TARGET_BASE*, BDEMA_COOKIE_BASE *));
        // [!DEPRECATED!] Instead, use:
        //..
        //  template <class BDEMA_TARGET_TYPE>
        //  void load(BDEMA_TARGET_TYPE *ptr,
        //            void              *cookie,
        //            DeleterFunc        deleter);
        //..
        //
        // Destroy the current managed object (if any) and
        // re-initialize this managed pointer to manage the specified 'ptr'
        // using the specified 'deleter' with arguments 'ptr' and the specified
        // 'factory' to destroy 'ptr' when this managed pointer is destroyed or
        // re-assigned, unless it is released before then.  If '0 == ptr', then
        // this object will be initialized to an unset state.  The behavior is
        // undefined if 'ptr' is already managed by another managed pointer.
        // Note that this function is *deprecated* as it relies on undefined
        // compiler behavior for its implementation, that happens to perform
        // as required on every known compiler.

    template <class BDEMA_OTHER_TYPE>
    void loadAlias(bdema_ManagedPtr<BDEMA_OTHER_TYPE>& alias, BDEMA_TYPE *ptr);
        // Destroy the current managed object (if any) and re-initialize this
        // managed pointer to take ownership of the object managed by the
        // specified 'alias' managed pointer, but with the specified 'ptr' as
        // an externally-visible value.  The value of 'ptr' is treated as an
        // alias to all or part of 'alias.ptr()'.  Unless it is released first,
        // destroying or re-assigning this managed pointer will delete the
        // object originally managed by 'alias' -- the destructor for '*ptr' is
        // not called directly.  'alias' will be re-initialized to an unset
        // state, and that the object previously managed by 'alias' will not be
        // deleted until this managed pointer is destroyed or re-assigned.  The
        // behavior is undefined if 'ptr' is already managed by a managed
        // pointer other than 'alias'.

    bsl::pair<BDEMA_TYPE *, bdema_ManagedPtrDeleter> release();
        // Return a raw pointer to the current managed object (if any) and its
        // current deleter and factory through the 'bdema_ManagedPtrDeleter'
        // member of the return value, and re-initialize this managed pointer
        // to an unset state.  It is undefined behavior to run the deleter
        // unless the pointer to the returned managed object is not null.

    void swap(bdema_ManagedPtr& other);
        // Exchange the value and ownership of this managed pointer with the
        // specified 'other' managed pointer.

    // ACCESSORS
    operator BoolType() const;
        // Return a value of "unspecified bool" type that evaluates to 'false'
        // if this managed pointer is in an unset state, and 'true' otherwise.
        // Note that this conversion operator allows a managed pointer to be
        // used within a conditional context, such as within an 'if' or 'while'
        // statement, but does *not* allow managed pointers to be compared
        // (e.g., via '<' or '>').  Note that a superior solution is available
        // in C++11 using the 'explicit operator bool()' syntax, that removes
        // the need for a special boolean-like type and private equality
        // comparison operators.

    typename bslmf_AddReference<BDEMA_TYPE>::Type operator*() const;
        // Return a reference to the managed object.  The behavior is undefined
        // if this managed pointer is in an unset state, or if 'BDEMA_TYPE' is
        // 'void' or 'const void'.

    BDEMA_TYPE *operator->() const;
        // Return the address of the managed (modifiable) object, or 0 if this
        // managed pointer is in an unset state.

    BDEMA_TYPE *ptr() const;
        // Return the address of the managed (modifiable) object, or 0 if this
        // managed pointer is in an unset state.

    const bdema_ManagedPtrDeleter& deleter() const;
        // Return a non-modifiable reference to the deleter information
        // associated with this managed pointer.
};

template <class BDEMA_TYPE>
class bdema_ManagedPtr<volatile BDEMA_TYPE>;
    // This specialization provides an early compile-fail check to catch misuse
    // of managed pointer to volatile types, which is explicitly called out as
    // not supported in the primary class template contract.

template <class BDEMA_TYPE>
class bdema_ManagedPtr<BDEMA_TYPE &>;
    // This specialization provides an early compile-fail check to catch misuse
    // of managed pointer to reference types, which is explicitly called out as
    // not supported in the primary class template contract.

template <class BDEMA_TYPE>
void swap(bdema_ManagedPtr<BDEMA_TYPE>& a, bdema_ManagedPtr<BDEMA_TYPE>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.

                    // ==================================
                    // struct bdema_ManagedPtrNoOpDeleter
                    // ==================================

struct bdema_ManagedPtrNoOpDeleter {
    // This utility class provides a general no-op deleter, which is useful
    // when creating managed pointers to stack-allocated objects.

    static void deleter(void *, void *);
        // Deleter function that does nothing.
};

                     // =================================
                     // struct bdema_ManagedPtrNilDeleter
                     // =================================

template <class BDEMA_TYPE>
struct bdema_ManagedPtrNilDeleter : bdema_ManagedPtrNoOpDeleter {
    // [!DEPRECATED!] Use 'bdema_ManagedPtrNoOpDeleter' instead.
    // This utility class provides a general no-op deleter, which is useful
    // when creating managed pointers to stack-allocated objects.  Note that
    // the non-template class 'bdema_ManagedPtrNoOpDeleter' should be used in
    // preference to this deprecated class, avoiding both template bloat and
    // undefined behavior.
};

                    // ==================================
                    // private class bdema_ManagedPtr_Ref
                    // ==================================

template <class BDEMA_TYPE>
class bdema_ManagedPtr_Ref {
    // This struct holds a managed pointer reference, returned by the implicit
    // conversion operator in the class 'bdema_ManagedPtr'.  This struct is
    // used to allow the construction of managed pointers from temporary
    // managed pointer objects, since temporaries cannot bind to the reference
    // to a modifiable object used in the "copy constructor" and "copy
    // assignment operator" for 'bdema_ManagedPtr'.  Note that while no members
    // or methods of this class template depend on the specified 'BDEMA_TYPE',
    // it is important to carry this type into conversions to support passing
    // ownership of 'bdema_ManagedPtr_Members' pointers when assigning or
    // constructing 'bdema_ManagedPtr' objects.

    bdema_ManagedPtr_Members *d_base_p;  // non-null pointer to the managed
                                         // state of a 'bdema_ManagedPtr'
                                         // object.
  public:
    explicit bdema_ManagedPtr_Ref(bdema_ManagedPtr_Members *base);
        // Create a 'bdema_ManagedPtr_Ref' object having the specified 'base'
        // value for its 'base' attribute.  Behavior is undefined unless
        // '0 != base'.


    //! bdema_ManagedPtr_Ref(const bdema_ManagedPtr_Ref& original);
        // Create a 'bdema_ManagedPtr_Ref' object having the same 'd_base_p'
        // value as the specified 'original'.  Note that this trivial
        // constructor's definition is compiler generated.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~bdema_ManagedPtr_Ref();
        // Destroy this object.  Note that the referenced managed object is
        // *not* destroyed.
#endif

    //! bdema_ManagedPtr_Ref& operator=(const bdema_ManagedPtr_Ref& original);
        // Create a 'bdema_ManagedPtr_Ref' object having the same 'd_base_p'
        // as the specified 'original'.  Note that this trivial copy-assignment
        // operator's definition is compiler generated.

    // ACCESSORS
    bdema_ManagedPtr_Members *base() const;
        // Return a pointer to the managed state of a 'bdema_SharedPtr' object.
};

             // =================================================
             // private class bdema_ManagedPtr_FactoryDeleterType
             // =================================================

template <class BDEMA_TYPE, class BDEMA_FACTORY>
struct bdema_ManagedPtr_FactoryDeleterType {
    // This metafunction class-template provides a means to compute the
    // preferred deleter function for a factory class for those overloads of
    // 'bdema_ManagedPtr' that supply only a factory, and no additional deleter
    // function.  The intent is to use a common deleter function for all
    // allocators that implement the 'bslma_Allocator' protocol, rather than
    // create a special deleter function based on the complete type of each
    // allocator, each doing the same thing (invoking the virtual function
    // 'deleteObject').

    typedef
    typename bslmf_If<
                  bslmf_IsConvertible<BDEMA_FACTORY*, bslma_Allocator*>::VALUE,
                  bslma_Allocator,
                  BDEMA_FACTORY>::Type
             factory_type;  // alias to the preferred factory type

    typedef bdema_ManagedPtr_FactoryDeleter<BDEMA_TYPE, factory_type> Type;
        // Alias to the default 'deleter' policy for a managed object of type
        // 'BDEMA_TYPE' using the preferred factory type.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // ----------------------
                           // class bdema_ManagedPtr
                           // ----------------------

template <class BDEMA_TYPE>
inline
void *bdema_ManagedPtr<BDEMA_TYPE>::stripBasePointerType(BDEMA_TYPE *ptr)
{
    return const_cast<void*>(static_cast<const void*>(ptr));
}

template <class BDEMA_TYPE>
template <class BDEMA_OTHER_TYPE>
inline
void *bdema_ManagedPtr<BDEMA_TYPE>::stripCompletePointerType(
                                                         BDEMA_OTHER_TYPE *ptr)
{
    return const_cast<void*>(static_cast<const void*>(ptr));
}

// CREATORS
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(bsl::nullptr_t, bsl::nullptr_t)
: d_members()
{
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr)
: d_members(stripCompletePointerType(ptr),
            bslma_Default::allocator(),
            &bdema_ManagedPtr_FactoryDeleter<BDEMA_TARGET_TYPE,bslma_Allocator>
                                                                     ::deleter,
            stripBasePointerType(ptr))
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TARGET_TYPE *, BDEMA_TYPE *>::
                                                                       VALUE));
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(
                                          bdema_ManagedPtr_Ref<BDEMA_TYPE> ref)
: d_members(*ref.base())
{
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(bdema_ManagedPtr& other)
: d_members(other.d_members)
{
}

template <class BDEMA_TYPE>
template <class BDEMA_OTHER_TYPE>
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(
                                     bdema_ManagedPtr<BDEMA_OTHER_TYPE>& alias,
                                     BDEMA_TYPE                         *ptr)
: d_members()
{
    BSLS_ASSERT_SAFE( 0 != alias.ptr() || 0 == ptr );

    if(0 != ptr) {
        d_members.move(alias.d_members);
        d_members.setAliasPtr(stripBasePointerType(ptr));
    }
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE, class BDEMA_FACTORY>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr,
                                               BDEMA_FACTORY     *factory)
: d_members(stripCompletePointerType(ptr),
            factory,
            &bdema_ManagedPtr_FactoryDeleterType<BDEMA_TARGET_TYPE,
                                                 BDEMA_FACTORY>::Type::deleter,
            stripBasePointerType(ptr))
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TARGET_TYPE *, BDEMA_TYPE *>::
                                                                       VALUE));
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TYPE *ptr,
                                               void       *cookie,
                                               DeleterFunc deleter)
: d_members(stripBasePointerType(ptr), cookie, deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr,
                                               void              *cookie,
                                               DeleterFunc        deleter)
: d_members(stripCompletePointerType(ptr),
            cookie,
            deleter,
            stripBasePointerType(ptr))
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                      BDEMA_TYPE *>::VALUE));

    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE, class BDEMA_TARGET_BASE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(BDEMA_TARGET_TYPE *ptr,
                                    void                         *cookie,
                                    void (*deleter)(BDEMA_TARGET_BASE*, void*))
: d_members(stripCompletePointerType(ptr),
            0,
            reinterpret_cast<DeleterFunc>(deleter),
            stripBasePointerType(ptr))
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                      BDEMA_TYPE *>::VALUE));
    BSLMF_ASSERT((bslmf_IsConvertible<
                        BDEMA_TARGET_TYPE *,
                        const BDEMA_TARGET_BASE *>::VALUE));

    BSLS_ASSERT_SAFE(0 == cookie);
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE,
          class BDEMA_TARGET_BASE,
          class BDEMA_COOKIE,
          class BDEMA_COOKIE_BASE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::bdema_ManagedPtr(
                      BDEMA_TARGET_TYPE *ptr,
                      BDEMA_COOKIE      *cookie,
                      void (*deleter)(BDEMA_TARGET_BASE*, BDEMA_COOKIE_BASE *))
: d_members(stripCompletePointerType(ptr),
            static_cast<BDEMA_COOKIE_BASE *>(cookie),
            reinterpret_cast<DeleterFunc>(deleter),
            stripBasePointerType(ptr))
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                      BDEMA_TYPE *>::VALUE));
    BSLMF_ASSERT((bslmf_IsConvertible<
                        BDEMA_TARGET_TYPE *,
                        const BDEMA_TARGET_BASE *>::VALUE));
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_COOKIE *,
                                      BDEMA_COOKIE_BASE *>::VALUE));

    // Note that the undefined behavior embodied in the 'reinterpret_cast'
    // above could be removed by inserting an additional forwarding function
    // truly of type 'DeleterFunc' which 'reinterpret_cast's each pointer
    // argument as part of its forwarding behavior.  We choose not to do this
    // on the grounds of simple efficiency, and there is currently no known
    // supported compiler that we use where this does not work as desired.

    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::~bdema_ManagedPtr()
{
    d_members.runDeleter();
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(bsl::nullptr_t, bsl::nullptr_t)
{
    this->clear();
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TYPE  *ptr,
                                        void        *cookie,
                                        DeleterFunc  deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    d_members.runDeleter();
    d_members.set(stripBasePointerType(ptr),
                  cookie,
                  deleter);
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TARGET_TYPE *ptr,
                                        void              *cookie,
                                        DeleterFunc        deleter)
{
    BSLMF_ASSERT(( bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                       BDEMA_TYPE *>::VALUE));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    d_members.runDeleter();
    d_members.set(stripCompletePointerType(ptr),
                  cookie,
                  deleter);
    d_members.setAliasPtr(stripBasePointerType(ptr));
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE, typename BDEMA_COOKIE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TARGET_TYPE *ptr,
                                        BDEMA_COOKIE      *cookie,
                                        DeleterFunc        deleter)
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TARGET_TYPE *, BDEMA_TYPE *>
                                                                     ::VALUE));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    d_members.runDeleter();
    d_members.set(stripCompletePointerType(ptr),
                  cookie,
                  deleter);
    d_members.setAliasPtr(stripBasePointerType(ptr));
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TARGET_TYPE *ptr)
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TARGET_TYPE *, BDEMA_TYPE *>
                                                                     ::VALUE));
    typedef
    bdema_ManagedPtr_FactoryDeleter<BDEMA_TARGET_TYPE,bslma_Allocator>
                                                                DeleterFactory;
    this->load(ptr,
               static_cast<void *>(bslma_Default::allocator()),
               &DeleterFactory::deleter
              );
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE, class BDEMA_FACTORY>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TARGET_TYPE *ptr,
                                        BDEMA_FACTORY *factory)
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TARGET_TYPE *, BDEMA_TYPE *>
                                                                     ::VALUE));
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);

    typedef
    typename bdema_ManagedPtr_FactoryDeleterType<BDEMA_TARGET_TYPE,
                                                 BDEMA_FACTORY>::Type
                                                                DeleterFactory;
    this->load(ptr,
               static_cast<void *>(factory),
               &DeleterFactory::deleter);
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE, class BDEMA_TARGET_BASE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(BDEMA_TARGET_TYPE *ptr,
                                   void                   *cookie,
                                   void (*deleter)(BDEMA_TARGET_BASE *, void*))
{
    BSLMF_ASSERT((!bslmf_IsVoid<BDEMA_TARGET_BASE>::VALUE ));
    BSLMF_ASSERT(( bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                       BDEMA_TYPE *>::VALUE));
    BSLMF_ASSERT(( bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                       BDEMA_TARGET_BASE *>::VALUE ));
    BSLS_ASSERT_SAFE(0 == cookie);
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    this->load(ptr,
               static_cast<void *>(0),
               reinterpret_cast<DeleterFunc>(deleter));
}

template <class BDEMA_TYPE>
template <class BDEMA_TARGET_TYPE,
          class BDEMA_TARGET_BASE,
          class BDEMA_COOKIE,
          class BDEMA_COOKIE_BASE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::load(
          BDEMA_TARGET_TYPE *ptr,
          BDEMA_COOKIE      *cookie,
          void             (*deleter)(BDEMA_TARGET_BASE*, BDEMA_COOKIE_BASE *))
{
    BSLMF_ASSERT(( bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                       BDEMA_TYPE *>::VALUE));
    BSLMF_ASSERT(( bslmf_IsConvertible<BDEMA_TARGET_TYPE *,
                                       BDEMA_TARGET_BASE *>::VALUE ));
    BSLMF_ASSERT(( bslmf_IsConvertible<BDEMA_COOKIE *,
                                       BDEMA_COOKIE_BASE *>::VALUE ));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    this->load(ptr,
               static_cast<void *>(static_cast<BDEMA_COOKIE_BASE *>(cookie)),
               reinterpret_cast<DeleterFunc>(deleter));
}

template <class BDEMA_TYPE>
template <class BDEMA_OTHER_TYPE>
void bdema_ManagedPtr<BDEMA_TYPE>::loadAlias(
                                     bdema_ManagedPtr<BDEMA_OTHER_TYPE>& alias,
                                     BDEMA_TYPE                         *ptr)
{
    BSLS_ASSERT_SAFE(  (0 == ptr && 0 == alias.ptr())
                    || (0 != ptr && 0 != alias.ptr()) );

    if (ptr && alias.d_members.pointer()) {
        if (&d_members != &alias.d_members) {
            d_members.runDeleter();
            d_members.move(alias.d_members);
        }
        d_members.setAliasPtr(stripBasePointerType(ptr));
    }
    else {
        d_members.runDeleter();
        d_members.clear();
    }
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::clear()
{
    d_members.runDeleter();
    d_members.clear();
}

template <typename BDEMA_TYPE>
bsl::pair<BDEMA_TYPE*, bdema_ManagedPtrDeleter>
bdema_ManagedPtr<BDEMA_TYPE>::release()
{
    BDEMA_TYPE *p = ptr();
    if (!p) {
        // undefined behavior to call d_members.deleter() if 'p' is null.
        return bsl::pair<BDEMA_TYPE*,bdema_ManagedPtrDeleter>();
    }
    bsl::pair<BDEMA_TYPE*,bdema_ManagedPtrDeleter> ret(p, d_members.deleter());
    d_members.clear();
    return ret;
}

template <class BDEMA_TYPE>
inline
void bdema_ManagedPtr<BDEMA_TYPE>::swap(bdema_ManagedPtr& other)
{
    d_members.swap(other.d_members);
}

template <class BDEMA_TYPE>
bdema_ManagedPtr<BDEMA_TYPE>&
bdema_ManagedPtr<BDEMA_TYPE>::operator=(bdema_ManagedPtr& rhs)
{   // Must protect against self-assignment due to destructive move
    if (&d_members != &rhs.d_members) {
        d_members.runDeleter();
        d_members.move(rhs.d_members);
    }

    return *this;
}

template <class BDEMA_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>&
bdema_ManagedPtr<BDEMA_TYPE>::operator=(bdema_ManagedPtr_Ref<BDEMA_TYPE> ref)
{   // Must protect against self-assignment due to destructive move
    if (&d_members != ref.base()) {
        d_members.runDeleter();
        d_members.move(*ref.base());
    }

    return *this;
}

template <class BDEMA_TYPE>
template <class BDEMA_OTHER_TYPE>
inline
bdema_ManagedPtr<BDEMA_TYPE>::operator bdema_ManagedPtr_Ref<BDEMA_OTHER_TYPE>()
{
    BSLMF_ASSERT((bslmf_IsConvertible<BDEMA_TYPE *,
                                      BDEMA_OTHER_TYPE *>::VALUE));

    return bdema_ManagedPtr_Ref<BDEMA_OTHER_TYPE>(&d_members);
}

// ACCESSORS
template <class BDEMA_TYPE>
inline
#if defined(BSLS_PLATFORM__CMP_IBM)
bdema_ManagedPtr<BDEMA_TYPE>::operator
                                    typename bdema_ManagedPtr::BoolType() const
#else
bdema_ManagedPtr<BDEMA_TYPE>::operator BoolType() const
#endif
{
    return d_members.pointer()
         ? bsls_UnspecifiedBool<bdema_ManagedPtr>::trueValue()
         : bsls_UnspecifiedBool<bdema_ManagedPtr>::falseValue();
}

template <class BDEMA_TYPE>
inline
typename bslmf_AddReference<BDEMA_TYPE>::Type
bdema_ManagedPtr<BDEMA_TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(d_members.pointer());

    return *static_cast<BDEMA_TYPE*>(d_members.pointer());
}

template <class BDEMA_TYPE>
inline
BDEMA_TYPE *bdema_ManagedPtr<BDEMA_TYPE>::operator->() const
{
    return static_cast<BDEMA_TYPE*>(d_members.pointer());
}

template <class BDEMA_TYPE>
inline
const bdema_ManagedPtrDeleter& bdema_ManagedPtr<BDEMA_TYPE>::deleter() const
{
    BSLS_ASSERT_SAFE(d_members.pointer());

    return d_members.deleter();
}

template <class BDEMA_TYPE>
inline
BDEMA_TYPE *bdema_ManagedPtr<BDEMA_TYPE>::ptr() const
{
    return static_cast<BDEMA_TYPE*>(d_members.pointer());
}

template <class BDEMA_TYPE>
inline
void swap(bdema_ManagedPtr<BDEMA_TYPE>& a, bdema_ManagedPtr<BDEMA_TYPE>& b)
{
    a.swap(b);
}

                      // ----------------------------------
                      // private class bdema_ManagedPtr_Ref
                      // ----------------------------------

// CREATOR
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr_Ref<BDEMA_TYPE>::bdema_ManagedPtr_Ref(
                                                bdema_ManagedPtr_Members *base)
: d_base_p(base)
{
    BSLS_ASSERT_SAFE(0 != base);
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr_Ref<BDEMA_TYPE>::~bdema_ManagedPtr_Ref()
{
    BSLS_ASSERT_SAFE(0 != d_base_p);
}
#endif

// ACCESSORS
template <class BDEMA_TYPE>
inline
bdema_ManagedPtr_Members *bdema_ManagedPtr_Ref<BDEMA_TYPE>::base() const
{
    return d_base_p;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
