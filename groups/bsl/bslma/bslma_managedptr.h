// bslma_managedptr.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDPTR
#define INCLUDED_BSLMA_MANAGEDPTR

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a managed pointer class.
//
//@CLASSES:
//  bslma::ManagedPtr: proctor for automatic memory management
//  bslma::ManagedPtrUtil: namespace for 'ManagedPtr'-related utility functions
//
//@SEE_ALSO: bslmf_ispolymporphic
//
//@DESCRIPTION: This component provides a proctor, 'bslma::ManagedPtr', similar
// to 'bsl::auto_ptr', that supports user-specified deleters.  The proctor is
// responsible for the automatic destruction of the object referenced by the
// managed pointer.  As a "smart pointer", this object offers an interface
// similar to a native pointer, supporting dereference operators (*, ->),
// (in)equality comparison and testing as if it were a boolean value.  However,
// like 'bsl::auto_ptr' it has unusual "copy-semantics" that transfer ownership
// of the managed object, rather than making a copy.  It should be noted that
// this signature does not satisfy the requirements for an element-type stored
// in any of the standard library containers.  Note that this component will
// fail to compile when instantiated for a class that gives a false-positive
// for the type trait 'bslmf::IsPolymorphic'.  See the 'bslmf_ispolymporphic'
// component for more details.
//
// This component also provides the 'bslma::ManagedPtrUtil' 'struct', which
// defines a namespace for utility functions that facilitate working with
// 'ManagedPtr' objects.  Of particular note are the 'allocateManaged' and
// 'makeManaged' class methods that can be used to create a managed object as
// well as a 'ManagedPtr' to manage it, with the latter being returned.
// 'allocateManaged' takes a 'bslma::Allocator *' argument that is both (1)
// used to allocate the footprint of the managed object and (2) used by the
// managed object itself if it defines the 'bslma::UsesBslmaAllocator' trait.
// 'makeManaged' does not take a 'bslma::Allocator *' argument and uses the
// default allocator to allocate the footprint of the managed object instead.
//
///Factories
///---------
// An object that will be managed by a 'ManagedPtr' object is typically
// dynamically allocated and destroyed by a factory.  For the purposes of this,
// component, a factory is any class that provides a 'deleteObject' function
// taking a single argument of the (pointer) type of the managed pointer.  The
// following is an example of a factory deleter:
//..
//  class my_Factory {
//
//     // . . .
//
//     // MANIPULATORS
//     my_Type *createObject(bslma::Allocator *basicAllocator = 0);
//         // Create a 'my_Type' object.  Optionally specify a
//         // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//         // 0, the currently installed default allocator is used.
//
//     void deleteObject(my_Type *object);
//         // Delete the specified 'object'.
//  };
//
//  class my_Allocator : public bslma::Allocator { /* ... */ };
//..
// Note that 'deleteObject' is provided by all 'bslma' allocators and by any
// object that implements the 'bdlma::Deleter' protocol.  Thus, any of these
// objects can be used as a factory deleter.  The purpose of this design is to
// allow 'bslma' allocators and factories to be used seamlessly as deleters.
//
// Note that when the 'ManagedPtr(MANAGED_TYPE *)' constructor is used, the
// managed object will be destroyed with a built-in deleter that calls
// 'delete ptr', but when the 'ManagedPtr(MANAGED_TYPE *, FACTORY_TYPE *)'
// constructor is called with '0 == factory', the currently installed default
// allocator will be used as the factory.
//
///Deleters
///--------
// When a managed pointer is destroyed, the managed object is destroyed using
// the user supplied "deleter".  A deleter is simply a function that is invoked
// with two 'void *' arguments: a pointer to the object to be destroyed, and a
// pointer to a 'cookie' that is supplied at the same time as the 'deleter' and
// managed object.
//..
//  typedef void (*DeleterFunc)(void *managedObject, void *cookie);
//..
// The meaning of the 'cookie' depends on the specific deleter.  Typically a
// deleter function will accept the two 'void *' pointers and internally cast
// them to the appropriate types for pointers to the managed object and
// 'cookie'.  Note that there are no methods taking just a deleter, as the user
// must always supply a 'cookie' to be passed when the deleter is actually
// invoked.
//
// Note that this component still supports (deprecated) legacy deleters that
// expect to be passed pointers to the specific 'cookie' and managed object
// types in use.  This latter form of deleter was deprecated as it relies on
// undefined behavior, casting such function pointers to the correct form
// (taking two 'void *' arguments) and invoking the function with two 'void *'
// pointer arguments.  While this is undefined behavior, it is known to have
// the desired effect on all platforms currently in use.
//
///Aliasing
///--------
// In a managed pointer, the pointer value (the value returned by the 'get'
// method) and the pointer to the managed object need not have the same value.
// The 'loadAlias' method allows a managed pointer to be created as an "alias"
// to another managed pointer (possibly of a different type), which we'll call
// the "original" managed pointer.  When 'get' is invoked on the alias, the
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
// 'ManagedPtr' objects can be implicitly and explicitly cast to different
// types in the same way that native pointers can.
//
///Explicit Casting
/// - - - - - - - -
// Through "aliasing", a managed pointer of any type can be explicitly cast to
// a managed pointer of any other type using any legal cast expression.  See
// example 4 on 'type casting' below for more details.
//
///Implicit Casting
/// - - - - - - - -
// As with native pointers, a managed pointer of the type 'B' that is derived
// from the type 'A', can be directly assigned to a 'ManagedPtr' of 'A'.
// Likewise a managed pointer of type 'B' can be directly assigned to a
// 'ManagedPtr' of 'const B'.  However, the rules for construction are a little
// more subtle, and apply when passing a 'bslma::ManagedPtr' by value into a
// function, or returning as the result of a function.
//..
//  class A {};
//
//  class B : public A {};
//
//  void test()
//  {
//      B *b_p = 0;
//      A *a_p = b_p;
//
//      bslma::ManagedPtr<B> b_mp1;
//      bslma::ManagedPtr<A> a_mp1(b_mp1);   // direct-initialization is valid
//      bslma::ManagedPtr<A> a_mp2 = b_mp1;  // copy-initialization should fail
//  }
//..
// Note that 'std::auto_ptr' has the same restriction, and this failure will
// occur only on compilers that strictly conform to the C++ standard, such as
// recent gcc compilers or (in this case) IBM xlC.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Implementing a Protocol
/// - - - - - - - - - - - - - - - - -
// We demonstrate using 'ManagedPtr' to configure and return a managed object
// implementing an abstract protocol.
//
// First we define our protocol, 'Shape', a type of object that knows how to
// compute its 'area'.  Note that for expository reasons only, we do *not* give
// 'Shape' a virtual destructor.
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
//      explicit Circle(double radius);
//          // Create a 'Circle' object having the specified 'radius'.
//
//      // ACCESSORS
//      virtual double area() const;
//          // Return the area of this Circle, given by the formula pi*r*r.
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
//          // Return the area of this Square, given by the formula side*side
//  };
//..
// Next we implement the methods for 'Circle' and 'Square'.
//..
//  Circle::Circle(double radius)
//  : d_radius(radius)
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
//  bslma::ManagedPtr<Shape> makeShape(Shapes::VALUES kind, double dimension)
//  {
//      bslma::Allocator *alloc = bslma::Default::defaultAllocator();
//      bslma::ManagedPtr<Shape> result;
//      switch (kind) {
//        case Shapes::SHAPE_CIRCLE: {
//          Circle *circ = new(*alloc)Circle(dimension);
//          result.load(circ);
//        } break;
//        case Shapes::SHAPE_SQUARE: {
//          Square *sqr = new(*alloc)Square(dimension);
//          result.load(sqr);
//        } break;
//      }
//      return result;
//  }
//..
// Then, we can use our function to create shapes of different kinds, and check
// that they report the correct area.  Note that we are using a radius of '1.0'
// for the 'Circle' and integral side-length for the 'Square' to support an
// accurate 'operator==' with floating-point quantities.  Also note that,
// despite the destructor for 'Shape' being non-virtual, the correct destructor
// for the appropriate concrete 'Shape' type is called.  This is because the
// destructor is captured when the 'ManagedPtr' constructor is called, and has
// access to the complete type of each shape object.
//..
//  void testShapes()
//  {
//      bslma::ManagedPtr<Shape> shape = makeShape(Shapes::SHAPE_CIRCLE, 1.0);
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
// allocator to the 'ManagedPtr' object as the "factory".
//..
//  bslma::ManagedPtr<Shape> makeShape(Shapes::VALUES    kind,
//                                     double            dimension,
//                                     bslma::Allocator *allocator)
//  {
//      bslma::Allocator *alloc = bslma::Default::allocator(allocator);
//      bslma::ManagedPtr<Shape> result;
//      switch (kind) {
//        case Shapes::SHAPE_CIRCLE: {
//          Circle *circ = new(*alloc)Circle(dimension);
//          result.load(circ, alloc);
//        } break;
//        case Shapes::SHAPE_SQUARE: {
//          Square *sqr = new(*alloc)Square(dimension);
//          result.load(sqr, alloc);
//        } break;
//      }
//      return result;
//  }
//..
// Finally we repeat the earlier test, additionally passing a test allocator:
//..
//  void testShapesToo()
//  {
//      bslma::TestAllocator ta("object");
//
//      bslma::ManagedPtr<Shape> shape =
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
// Suppose that we wish to give access to an item in a temporary array via a
// pointer, which we will call the "finger".  The finger is the only pointer to
// the array or any part of the array, but the entire array must be valid until
// the finger is destroyed, at which time the entire array must be deleted.  We
// handle this situation by first creating a managed pointer to the entire
// array, then creating an alias of that pointer for the finger.  The finger
// takes ownership of the array instance, and when the finger is destroyed, it
// is the array's address, rather than the finger, that is passed to the
// deleter.
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
// for an array of quotes (the threshold and its neighbors).  It thus returns a
// managed pointer to the desired value:
//..
//  const double END_QUOTE = -1;
//
//  bslma::ManagedPtr<double>
//  getFirstQuoteLargerThan(double threshold, bslma::Allocator *allocator)
//  {
//      assert(END_QUOTE < 0 && 0 <= threshold);
//..
// Next, we allocate our array with extra room to mark the beginning and end
// with a special 'END_QUOTE' value:
//..
//      const int MAX_QUOTES = 100;
//      int numBytes = (MAX_QUOTES + 2) * sizeof(double);
//      double *quotes = (double *) allocator->allocate(numBytes);
//      quotes[0] = quotes[MAX_QUOTES + 1] = END_QUOTE;
//..
// Then, we create a managed pointer to the entire array:
//..
//      bslma::ManagedPtr<double> managedQuotes(quotes, allocator);
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
// Now, we use the alias constructor to create a managed pointer that points to
// the desired value (the finger) but manages the entire array:
//..
//      return bslma::ManagedPtr<double>(managedQuotes, finger);
//  }
//..
// Then, our main program calls 'getFirstQuoteLargerThan' like this:
//..
//  int aliasExample()
//  {
//      bslma::TestAllocator ta;
//      bslma::ManagedPtr<double> result = getFirstQuoteLargerThan(16.00, &ta);
//      assert(*result > 16.00);
//      assert(1 == ta.numBlocksInUse());
//      if (g_verbose) bsl::cout << "Found quote: " << *result << bsl::endl;
//..
// Next, we also print the preceding 5 quotes in last-to-first order:
//..
//      if (g_verbose) bsl::cout << "Preceded by:";
//      int i;
//      for (i = -1; i >= -5; --i) {
//          double quote = result.get()[i];
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
// 'result.load(result.get()-i)' would be an error, because it would first
// compute the pointer value 'result.get()-i' of the argument, then release the
// entire array before starting to manage what has now become an invalid
// pointer.  Instead, 'result' must retain its ownership to the entire array,
// which can be attained by:
//..
//      result.loadAlias(result, result.get()-i);
//..
// Finally, if we reset the result pointer, the entire array is deallocated:
//..
//      result.reset();
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
// 'ManagedPtr' objects.
//
// First we define a factory type that holds an allocator and a usage-counter.
// Note that such a type cannot sensibly be copied, as the notion 'count'
// becomes confused.
//..
//  class CountedFactory {
//      // DATA
//      int               d_count;
//      bslma::Allocator *d_allocator_p;
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
// signature has the form required by 'bslma::ManagedPtr' for a factory.
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
//          // Destroy the object pointed to by 'target' and reclaim the
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
//  , d_allocator_p(bslma::Default::allocator(alloc))
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
//      TYPE *result = new(*d_allocator_p)TYPE;
//      ++d_count;
//      return result;
//  }
//
//  template <class TYPE>
//  void CountedFactory::deleteObject(const TYPE *object)
//  {
//      d_allocator_p->deleteObject(object);
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
// be used with 'ManagedPtr'.
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
//          bslma::ManagedPtr<int> pData[4];
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
// Then, we 'reset' the contents of a single managed pointer in the array, and
// assert that the factory 'count' is appropriately reduced.
//..
//          pData[1].reset();
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
// 'ManagedPtr' objects can be implicitly and explicitly cast to different
// types in the same way that native pointers can.
//
///Implicit Conversion
/// -  -  -  -  -  - -
// As with native pointers, a pointer of the type 'B' that is publicly derived
// from the type 'A', can be directly assigned a 'ManagedPtr' of 'A'.
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
//          bslma::ManagedPtr<A> a_mp1;
//          bslma::ManagedPtr<B> b_mp1;
//
//          assert(!a_mp1 && !b_mp1);
//
//          a_mp1 = b_mp1;      // conversion assignment of nil ptr to nil
//          assert(!a_mp1 && !b_mp1);
//
//          B *b_p2 = new (localDefaultTa) B(&numdels);
//          bslma::ManagedPtr<B> b_mp2(b_p2);    // default allocator
//          assert(!a_mp1 && b_mp2);
//
//          a_mp1 = b_mp2;      // conversion assignment of non-nil ptr to nil
//          assert(a_mp1 && !b_mp2);
//
//          B *b_p3 = new (localTa) B(&numdels);
//          bslma::ManagedPtr<B> b_mp3(b_p3, &localTa);
//          assert(a_mp1 && b_mp3);
//
//          a_mp1 = b_mp3;      // conversion assignment of non-nil to non-nil
//          assert(a_mp1 && !b_mp3);
//
//          a_mp1 = b_mp3;      // conversion assignment of nil to non-nil
//          assert(!a_mp1 && !b_mp3);
//
//          // constructor conversion init with nil
//          bslma::ManagedPtr<A> a_mp4(b_mp3, b_mp3.get());
//          assert(!a_mp4 && !b_mp3);
//
//          // constructor conversion init with non-nil
//          B *p_b5 = new (localTa) B(&numdels);
//          bslma::ManagedPtr<B> b_mp5(p_b5, &localTa);
//          bslma::ManagedPtr<A> a_mp5(b_mp5, b_mp5.get());
//          assert(a_mp5 && !b_mp5);
//          assert(a_mp5.get() == p_b5);
//
//          // constructor conversion init with non-nil
//          B *p_b6 = new (localTa) B(&numdels);
//          bslma::ManagedPtr<B> b_mp6(p_b6, &localTa);
//          bslma::ManagedPtr<A> a_mp6(b_mp6);
//          assert(a_mp6 && !b_mp6);
//          assert(a_mp6.get() == p_b6);
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
// managed pointer of type B, one can simply do the following:
//..
//  void explicitCastingExample() {
//
//      bslma::ManagedPtr<A> a_mp;
//      bslma::ManagedPtr<B> b_mp1(a_mp, static_cast<B *>(a_mp.get()));
//..
// or even use the less safe "C"-style casts:
//..
//      bslma::ManagedPtr<B> b_mp2(a_mp, (B *)(a_mp.get()));
//
//  } // explicitCastingExample()
//..
// Note that when using dynamic cast, if the cast fails, the target managed
// pointer will be reset to an unset state, and the source will not be
// modified.  Consider for example the following snippet of code:
//..
//  void processPolymorphicObject(bslma::ManagedPtr<A> aPtr,
//                                bool *castSucceeded)
//  {
//      bslma::ManagedPtr<B> bPtr(aPtr, dynamic_cast<B *>(aPtr.get()));
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
// If the value of 'aPtr' can be dynamically cast to 'B *' then ownership is
// transferred to 'bPtr'; otherwise, 'aPtr' is to be modified.  As previously
// stated, the managed object will be destroyed correctly regardless of how it
// is cast.
//
///Example 5: Inplace Object Creation
/// - - - - - - - - - - - - - - - - -
// Suppose we want to allocate memory for an object, construct it in place, and
// obtain a managed pointer referring to this object.  This can be done in one
// step using two free functions provided in 'bslma::ManagedPtrUtil'.
//
// First, we create a simple class clearly showing the features of these
// functions.  Note that this class does not define the
// 'bslma::UsesBslmaAllocator' trait.  It is done intentionally for
// illustration purposes only, and definitely is *not* *recommended* in
// production code.  The class has an elided interface (i.e., copy constructor
// and copy-assignment operator are not included for brevity):
//..
//  class String {
//      // Simple class that stores a copy of a null-terminated C-style string.
//
//    private:
//      // DATA
//      char             *d_str_p;    // stored value (owned)
//      bslma::Allocator *d_alloc_p;  // allocator to allocate any dynamic
//                                    // memory (held, not owned)
//
//    public:
//      // CREATORS
//      String(const char *str, bslma::Allocator *alloc)
//          // Create an object having the same value as the specified 'str'
//          // using the specified 'alloc' to supply memory.
//      : d_alloc_p(alloc)
//      {
//          assert(str);
//          assert(alloc);
//
//          std::size_t length = std::strlen(str);
//
//          d_str_p = static_cast<char *>(d_alloc_p->allocate(length + 1));
//          std::strncpy(d_str_p, str, length + 1);
//      }
//
//      ~String()
//          // Destroy this object.
//      {
//          d_alloc_p->deallocate(d_str_p);
//      }
//
//      // ACCESSORS
//      bslma::Allocator *allocator() const
//          // Return a pointer providing modifiable access to the allocator
//          // associated with this 'String'.
//      {
//          return d_alloc_p;
//      }
//  };
//..
// Next, we create a code fragment that will construct a managed 'String'
// object using the default allocator to supply memory:
//..
//  void testInplaceCreation()
//  {
//..
// Suppose we want to have a different allocator supply memory allocated by the
// object:
//..
//      bslma::TestAllocator ta;
//      bsls::Types::Int64   testBytesInUse = ta.numBytesInUse();
//
//      assert(0 == testBytesInUse);
//
//      bslma::TestAllocator         da;
//      bslma::DefaultAllocatorGuard dag(&da);
//      bsls::Types::Int64           defaultBytesInUse = da.numBytesInUse();
//
//      assert(0 == defaultBytesInUse);
//..
// Then, create a string to copy:
//..
//      const char *STR        = "Test string";
//      const int   STR_LENGTH = static_cast<int>(std::strlen(STR));
//..
// Next, dynamically create an object and obtain the managed pointer referring
// to it using the 'bslma::ManagedPtrUtil::makeManaged' function:
//..
//      {
//          bslma::ManagedPtr<String> stringManagedPtr =
//                        bslma::ManagedPtrUtil::makeManaged<String>(STR, &ta);
//..
// Note that memory for the object itself is supplied by the default allocator,
// while memory for the copy of the passed string is supplied by another
// allocator:
//..
//          assert(static_cast<int>(sizeof(String)) <= da.numBytesInUse());
//          assert(&ta == stringManagedPtr->allocator());
//          assert(STR_LENGTH + 1 == ta.numBytesInUse());
//      }
//..
// Then, make sure that all allocated memory is successfully released after
// managed pointer destruction:
//..
//      assert(0 == da.numBytesInUse());
//      assert(0 == ta.numBytesInUse());
//..
// If you want to use an allocator other than the default allocator, then the
// 'allocateManaged' function should be used instead:
//..
//      bslma::TestAllocator oa;
//      bsls::Types::Int64   objectBytesInUse = oa.numBytesInUse();
//      assert(0 == objectBytesInUse);
//
//      {
//          bslma::ManagedPtr<String> stringManagedPtr =
//               bslma::ManagedPtrUtil::allocateManaged<String>(&oa, STR, &ta);
//
//          assert(static_cast<int>(sizeof(String)) <= oa.numBytesInUse());
//          assert(&ta == stringManagedPtr->allocator());
//          assert(STR_LENGTH + 1 == ta.numBytesInUse());
//          assert(0 == da.numBytesInUse());
//      }
//
//      assert(0 == da.numBytesInUse());
//      assert(0 == ta.numBytesInUse());
//      assert(0 == oa.numBytesInUse());
//  }
//..
// Next, let's look at a more common scenario where the object's type uses
// 'bslma' allocators.  In that case 'allocateManaged' implicitly passes the
// supplied allocator to the object's constructor as an extra argument in the
// final position.
//
// The second example class almost completely repeats the first one, except
// that it explicitly defines the 'bslma::UsesBslmaAllocator' trait:
//..
//  class StringAlloc {
//      // Simple class that stores a copy of a null-terminated C-style string
//      // and explicitly claims to use 'bslma' allocators.
//
//    private:
//      // DATA
//      char             *d_str_p;    // stored value (owned)
//      bslma::Allocator *d_alloc_p;  // allocator to allocate any dynamic
//                                    // memory (held, not owned)
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(StringAlloc, bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      StringAlloc(const char *str, bslma::Allocator *basicAllocator = 0)
//          // Create an object having the same value as the specified 'str'.
//          // Optionally specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//      : d_alloc_p(bslma::Default::allocator(basicAllocator))
//      {
//          assert(str);
//
//          std::size_t length = std::strlen(str);
//
//          d_str_p = static_cast<char *>(d_alloc_p->allocate(length + 1));
//          std::strncpy(d_str_p, str, length + 1);
//      }
//
//      ~StringAlloc()
//          // Destroy this object.
//      {
//          d_alloc_p->deallocate(d_str_p);
//      }
//
//      // ACCESSORS
//      bslma::Allocator *allocator() const
//          // Return a pointer providing modifiable access to the allocator
//          // associated with this 'StringAlloc'.
//      {
//          return d_alloc_p;
//      }
//  };
//..
// Then, let's create two managed objects using both 'makeManaged' and
// 'allocateManaged':
//..
//  void testUsesAllocatorInplaceCreation()
//  {
//      bslma::TestAllocator ta;
//      bsls::Types::Int64   testBytesInUse = ta.numBytesInUse();
//
//      assert(0 == testBytesInUse);
//
//      bslma::TestAllocator         da;
//      bslma::DefaultAllocatorGuard dag(&da);
//      bsls::Types::Int64           defaultBytesInUse = da.numBytesInUse();
//
//      assert(0 == defaultBytesInUse);
//
//      const char *STR        = "Test string";
//      const int   STR_LENGTH = static_cast<int>(std::strlen(STR));
//
//..
// Note that we need to explicitly supply the allocator's address to
// 'makeManaged' to be passed to the object's constructor:
//..
//      {
//          bslma::ManagedPtr<StringAlloc> stringManagedPtr =
//                   bslma::ManagedPtrUtil::makeManaged<StringAlloc>(STR, &ta);
//
//          assert(static_cast<int>(sizeof(String)) <= da.numBytesInUse());
//          assert(&ta == stringManagedPtr->allocator());
//          assert(STR_LENGTH + 1 == ta.numBytesInUse());
//      }
//
//..
// But the supplied allocator is implicitly passed to the constructor by
// 'allocateManaged':
//..
//      {
//          bslma::ManagedPtr<StringAlloc> stringManagedPtr =
//               bslma::ManagedPtrUtil::allocateManaged<StringAlloc>(&ta, STR);
//
//          assert(static_cast<int>(sizeof(String)) + STR_LENGTH + 1 <=
//                                                         ta.numBytesInUse());
//          assert(&ta == stringManagedPtr->allocator());
//          assert(0 == da.numBytesInUse());
//      }
//..
// Finally, make sure that all allocated memory is successfully released after
// the managed pointers (and the objects they manage) are destroyed:
//..
//      assert(0 == da.numBytesInUse());
//      assert(0 == ta.numBytesInUse());
//  }
//..

#include <bslscm_version.h>

#include <bslma_allocator.h>
#include <bslma_constructionutil.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_default.h>
#include <bslma_managedptr_factorydeleter.h>
#include <bslma_managedptr_members.h>
#include <bslma_managedptr_pairproxy.h>
#include <bslma_managedptrdeleter.h>

#include <bslmf_addreference.h>
#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_enableif.h>
#include <bslmf_haspointersemantics.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_isvoid.h>
#include <bslmf_movableref.h>
#include <bslmf_removecv.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_nullptr.h>
#include <bsls_platform.h>
#include <bsls_unspecifiedbool.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Oct 21 10:11:37 2021
// Command line: sim_cpp11_features.pl bslma_managedptr.h
# define COMPILING_BSLMA_MANAGEDPTR_H
# include <bslma_managedptr_cpp03.h>
# undef COMPILING_BSLMA_MANAGEDPTR_H
#else

namespace BloombergLP {
namespace bslma {

                  // =================================
                  // private struct ManagedPtr_ImpUtil
                  // =================================

struct ManagedPtr_ImpUtil {
    // This 'struct' provides a namespace for utility functions used to obtain
    // the necessary types of pointers.

    // CLASS METHODS
    template <class TYPE>
    static void *voidify(TYPE *address) BSLS_KEYWORD_NOEXCEPT;
        // Return the specified 'address' cast as a pointer to 'void', even if
        // (the template parameter) 'TYPE' is cv-qualified.

    template <class TYPE>
    static TYPE *unqualify(const volatile TYPE *address) BSLS_KEYWORD_NOEXCEPT;
        // Return the specified 'address' of a potentially cv-qualified object
        // of the given (template parameter) 'TYPE', cast as a pointer to
        // non-cv-qualified 'TYPE'.
};
                    // ============================
                    // private class ManagedPtr_Ref
                    // ============================

template <class TARGET_TYPE>
class ManagedPtr_Ref {
    // This class holds a managed pointer reference, returned by the implicit
    // conversion operator in the class 'ManagedPtr'.  This class is used to
    // allow the construction of managed pointers from temporary managed
    // pointer objects, since temporaries cannot bind to the reference to a
    // modifiable object used in the copy constructor and copy-assignment
    // operator for 'ManagedPtr'.  Note that while no members or methods of
    // this class template depend on the specified 'TARGET_TYPE', it is
    // important to carry this type into conversions to support passing
    // ownership of 'ManagedPtr_Members' pointers when assigning or
    // constructing 'ManagedPtr' objects.

    // DATA
    ManagedPtr_Members *d_base_p;  // non-null pointer to the managed state of
                                   // a 'ManagedPtr' object

    TARGET_TYPE        *d_cast_p;  // safely-cast pointer to the referenced
                                   // object

  public:
    // CREATORS
    ManagedPtr_Ref(ManagedPtr_Members *base, TARGET_TYPE *target);
        // Create a 'ManagedPtr_Ref' object having the specified 'base' value
        // for its 'base' attribute, and the specified 'target' for its
        // 'target' attribute.  Note that 'target' (but not 'base') may be
        // null.

    //! ManagedPtr_Ref(const ManagedPtr_Ref& original) = default;
        // Create a 'ManagedPtr_Ref' object having the same 'd_base_p' value as
        // the specified 'original'.  Note that this trivial constructor's
        // definition is compiler generated.

    ~ManagedPtr_Ref();
        // Destroy this object.  Note that the referenced managed object is
        // *not* destroyed.

    // MANIPULATORS
    //! ManagedPtr_Ref& operator=(const ManagedPtr_Ref& original) = default;
        // Create a 'ManagedPtr_Ref' object having the same 'd_base_p' as the
        // specified 'original'.  Note that this trivial copy-assignment
        // operator's definition is compiler generated.

    // ACCESSORS
    ManagedPtr_Members *base() const;
        // Return a pointer to the managed state of a 'ManagedPtr' object.

    TARGET_TYPE *target() const;
        // Return a pointer to the referenced object.
};

                    // =========================================
                    // private struct ManagedPtr_TraitConstraint
                    // =========================================

struct ManagedPtr_TraitConstraint {
    // This 'struct' is an empty type that exists solely to enable constructor
    // access to be constrained by type trait.
};
                           // ================
                           // class ManagedPtr
                           // ================

template <class TARGET_TYPE>
class ManagedPtr {
    // This class is a "smart pointer" that refers to a *target* object
    // accessed via a pointer to the specified parameter type, 'TARGET_TYPE',
    // and that supports sole ownership of a *managed* object that is
    // potentially of a different type, and may be an entirely different object
    // from the target object.  A managed pointer ensures that the object it
    // manages is destroyed when the managed pointer is destroyed (or
    // re-assigned), using the "deleter" supplied along with the managed
    // object.  The target object referenced by a managed pointer may be
    // accessed using either the '->' operator, or the dereference operator
    // ('operator *').  The specified 'TARGET_TYPE' may be 'const'-qualified,
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
    // INTERFACE TYPES
    typedef ManagedPtrDeleter::Deleter DeleterFunc;
        // Alias for a function-pointer type for functions used to destroy the
        // object managed by a 'ManagedPtr' object.

    typedef TARGET_TYPE element_type;
        // Alias to the 'TARGET_TYPE' template parameter.

  private:
    // PRIVATE TYPES
    typedef typename bsls::UnspecifiedBool<ManagedPtr>::BoolType BoolType;
        // 'BoolType' is an alias for an unspecified type that is implicitly
        // convertible to 'bool', but will not promote to 'int'.  This (opaque)
        // type can be used as an "unspecified boolean type" for converting a
        // managed pointer to 'bool' in contexts such as 'if (mp) { ... }'
        // without actually having a conversion to 'bool' or being less-than
        // comparable (either of which would also enable undesirable implicit
        // comparisons of managed pointers to 'int' and less-than comparisons).

    typedef bslmf::MovableRefUtil                                MoveUtil;
        // This 'typedef' is a convenient alias for the utility associated with
        // movable references.

    // DATA
    ManagedPtr_Members d_members;  // state managed by this object

    // PRIVATE CLASS METHODS
    static void *stripBasePointerType(TARGET_TYPE *ptr);
        // Return the value of the specified 'ptr' as a 'void *', after
        // stripping all 'const' and 'volatile' qualifiers from 'TARGET_TYPE'.
        // This function avoids accidental type-safety errors when performing
        // the necessary sequence of casts.  Note that calling this function
        // implies a conversion of the calling pointer to 'TARGET_TYPE *',
        // which, in rare cases, may involve some adjustment of the pointer
        // value, e.g., in the case of multiple inheritance where 'TARGET_TYPE'
        // is not a left-most base of the complete object type.

    template <class MANAGED_TYPE>
    static void *stripCompletePointerType(MANAGED_TYPE *ptr);
        // Return the value of the specified 'ptr' as a 'void *', after
        // stripping all 'const' and 'volatile' qualifiers from 'MANAGED_TYPE'.
        // This function avoids accidental type-safety errors when performing
        // the necessary sequence of casts.

    // PRIVATE MANIPULATORS
    template <class MANAGED_TYPE>
    void loadImp(MANAGED_TYPE *ptr, void *cookie, DeleterFunc deleter);
        // Destroy the currently managed object, if any.  Then, set the target
        // object of this managed pointer to be that referenced by the
        // specified 'ptr', take ownership of '*ptr' as the currently managed
        // object, and set a deleter that will invoke the specified 'deleter'
        // with the address of the currently managed object, and with the
        // specified 'cookie' (that the deleter can use for its own purposes),
        // unless '0 == ptr', in which case reset this managed pointer to
        // empty.  The behavior is undefined if 'ptr' is already managed by
        // another object, or if '0 == deleter && 0 != ptr'.

  private:
    // NOT IMPLEMENTED
    template <class MANAGED_TYPE>
    ManagedPtr(MANAGED_TYPE *, bsl::nullptr_t);
        // It is never defined behavior to pass a null pointer literal as a
        // factory, unless the specified 'ptr' is also a null pointer literal.

  private:
    // NOT IMPLEMENTED
    template <class MANAGED_TYPE, class COOKIE_TYPE>
    ManagedPtr(MANAGED_TYPE *, COOKIE_TYPE *, bsl::nullptr_t);
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
    void operator==(const ManagedPtr&) const;
    void operator!=(const ManagedPtr&) const;
        // These two operator overloads are declared as 'private' but never
        // defined in order to eliminate accidental equality comparisons that
        // would occur through the implicit conversion to 'BoolType'.  Note
        // that the return type of 'void' is chosen as it will often produce a
        // clearer error message than relying on the 'private' control failure.
        // Also note that these private operators will not be needed with
        // C++11, where an 'explicit operator bool()' conversion operator would
        // be preferred.

    // FRIENDS
    template <class ALIASED_TYPE>
    friend class ManagedPtr;  // required only for alias support

  public:
    // CREATORS
    ManagedPtr();
        // Create an empty managed pointer.

    template <class MANAGED_TYPE>
    explicit ManagedPtr(MANAGED_TYPE *ptr);
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that will call 'delete ptr' to destroy the managed object
        // when invoked (e.g., when this managed pointer object is destroyed),
        // unless '0 == ptr', in which case create an empty managed pointer.
        // The deleter will invoke the destructor of 'MANAGED_TYPE' rather than
        // the destructor of 'TARGET_TYPE'.  This constructor will not compile
        // unless 'MANAGED_TYPE *' is convertible to 'TARGET_TYPE *'.  Note
        // that this behavior allows 'ManagedPtr' to be defined for 'void'
        // pointers, and to call the correct destructor for the managed object,
        // even if the destructor for 'TARGET_TYPE' is not declared as
        // 'virtual'.  The behavior is undefined unless the managed object (if
        // any) can be destroyed by 'delete', or if the lifetime of the managed
        // object is already managed by another object.

    ManagedPtr(ManagedPtr_Ref<TARGET_TYPE> ref)
                                             BSLS_KEYWORD_NOEXCEPT; // IMPLICIT
        // Create a managed pointer having the same target object as the
        // managed pointer referenced by the specified 'ref', transfer
        // ownership of the managed object owned by the managed pointer
        // referenced by 'ref', and reset the managed pointer referenced by
        // 'ref' to empty.  This constructor is used to create a managed
        // pointer from a managed pointer rvalue, or from a managed pointer to
        // a "compatible" type, where "compatible" means a built-in conversion
        // from 'COMPATIBLE_TYPE *' to 'TARGET_TYPE *' is defined, e.g.,
        // 'derived *' to 'base *', 'T *' to 'const T *', or 'T *' to 'void *'.

    ManagedPtr(ManagedPtr& original)                   BSLS_KEYWORD_NOEXCEPT;
    ManagedPtr(bslmf::MovableRef<ManagedPtr> original) BSLS_KEYWORD_NOEXCEPT;
        // Create a managed pointer having the same target object as the
        // specified 'original', transfer ownership of the object managed by
        // 'original' (if any) to this managed pointer, and reset 'original' to
        // empty.

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    template <class BDE_OTHER_TYPE>
    ManagedPtr(ManagedPtr<BDE_OTHER_TYPE>&& original,
               typename bsl::enable_if<
                   bsl::is_convertible<BDE_OTHER_TYPE *, TARGET_TYPE *>::value,
                   ManagedPtr_TraitConstraint>::type =
                       ManagedPtr_TraitConstraint())
        BSLS_KEYWORD_NOEXCEPT;
#elif defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    // sun compiler version 12.3 and earlier
    template <class BDE_OTHER_TYPE>
    ManagedPtr(bslmf::MovableRef<ManagedPtr<BDE_OTHER_TYPE> > original)
        BSLS_KEYWORD_NOEXCEPT;
#else // c++03 except old (version <= 12.3) sun compilers
    template <class BDE_OTHER_TYPE>
    ManagedPtr(bslmf::MovableRef<ManagedPtr<BDE_OTHER_TYPE> > original,
               typename bsl::enable_if<
                 bsl::is_convertible<BDE_OTHER_TYPE *, TARGET_TYPE *>::value,
                 ManagedPtr_TraitConstraint>::type =
                     ManagedPtr_TraitConstraint())
        BSLS_KEYWORD_NOEXCEPT;
#endif
        // Create a managed pointer having the same target object as the
        // specified 'original', transfer ownership of the object managed by
        // 'original' (if any) to this managed pointer, and reset 'original' to
        // empty.  'TARGET_TYPE' must be an accessible and unambiguous base of
        // 'BDE_OTHER_TYPE'

    template <class ALIASED_TYPE>
    ManagedPtr(ManagedPtr<ALIASED_TYPE>& alias, TARGET_TYPE *ptr);
    template <class ALIASED_TYPE>
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    ManagedPtr(ManagedPtr<ALIASED_TYPE>&&                    alias,
#else
    ManagedPtr(bslmf::MovableRef<ManagedPtr<ALIASED_TYPE> >  alias,
#endif
               TARGET_TYPE                                  *ptr);
        // Create a managed pointer that takes ownership of the object managed
        // by the specified 'alias', but which uses the specified 'ptr' to
        // refer to its target object, unless '0 == ptr', in which case create
        // an empty managed pointer.  Reset 'alias' to empty if ownership of
        // its managed object is transferred.  The behavior is undefined if
        // 'alias' is empty, but '0 != ptr'.  Note that destroying or
        // re-assigning a managed pointer created with this constructor will
        // destroy the object originally managed by 'alias' (unless 'release'
        // is called first); the destructor for '*ptr' is not called directly.

    template <class MANAGED_TYPE, class FACTORY_TYPE>
    ManagedPtr(MANAGED_TYPE *ptr, FACTORY_TYPE *factory);
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
        // '0 == factory && 0 != ptr', or if the lifetime of the managed object
        // is already managed by another object.  Note that 'bslma::Allocator',
        // and any class publicly and unambiguously derived from
        // 'bslma::Allocator', meets the requirements for 'FACTORY_TYPE'.

    explicit ManagedPtr(bsl::nullptr_t, bsl::nullptr_t = 0);
        // Create an empty managed pointer.  Note that this constructor is
        // necessary to match null-pointer literal arguments, in order to break
        // ambiguities and provide valid type deduction with the other
        // constructor templates in this class.

    template <class FACTORY_TYPE>
    ManagedPtr(bsl::nullptr_t, FACTORY_TYPE *factory);
        // Create an empty managed pointer.  Note that the specified 'factory'
        // is ignored, as an empty managed pointer does not call its deleter.

    ManagedPtr(TARGET_TYPE *ptr, void *cookie, DeleterFunc deleter);
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that will invoke the specified 'deleter' with the address of
        // the currently managed object, and with the specified 'cookie' (that
        // the deleter can use for its own purposes), unless '0 == ptr', in
        // which case create an empty managed pointer.  The behavior is
        // undefined if 'ptr' is already managed by another object, or if
        // '0 == deleter && 0 != ptr'.  Note that this constructor is required
        // only because the deprecated overloads cause an ambiguity in its
        // absence; it should be removed when the deprecated overloads are
        // removed.

    template <class MANAGED_TYPE>
    ManagedPtr(MANAGED_TYPE *ptr, void *cookie, DeleterFunc deleter);
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that will invoke the specified 'deleter' with the address of
        // the currently managed object, and with the specified 'cookie' (that
        // the deleter can use for its own purposes), unless '0 == ptr', in
        // which case create an empty managed pointer.  This constructor will
        // not compile unless 'MANAGED_TYPE *' is convertible to
        // 'TARGET_TYPE *'.  The deleter will invoke the destructor of
        // 'MANAGED_TYPE' rather than the destructor of 'TARGET_TYPE'.  The
        // behavior is undefined if 'ptr' is already managed by another object,
        // or if '0 == deleter && 0 != ptr'.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    template <class MANAGED_TYPE, class MANAGED_BASE>
    ManagedPtr(MANAGED_TYPE *ptr,
               void         *cookie,
               void        (*deleter)(MANAGED_BASE *, void *));
        // [!DEPRECATED!]: Instead, use:
        //..
        //  template <class MANAGED_TYPE>
        //  ManagedPtr(MANAGED_TYPE *ptr, void *cookie, DeleterFunc deleter);
        //..
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that will invoke the specified 'deleter' with the address of
        // the currently managed object, and with the specified 'cookie' (that
        // the deleter can use for its own purposes), unless '0 == ptr', in
        // which case create an empty managed pointer.  This constructor will
        // not compile unless 'MANAGED_TYPE *' is convertible to
        // 'TARGET_TYPE *', and 'MANAGED_TYPE *' is convertible to
        // 'MANAGED_BASE *'.  The deleter will invoke the destructor of
        // 'MANAGED_TYPE' rather than the destructor of 'TARGET_TYPE'.  The
        // behavior is undefined if 'ptr' is already managed by another object,
        // or if '0 == deleter && 0 != ptr'.  Note that this constructor is
        // needed only to avoid ambiguous type deductions when passing a null
        // pointer literal as the 'cookie' when the user passes a deleter
        // taking a type other than 'void *' for its object type.  Also note
        // that this function is *deprecated* as it relies on undefined
        // compiler behavior for its implementation (that luckily performs as
        // required on every platform supported by BDE).

    template <class MANAGED_TYPE,
              class MANAGED_BASE,
              class COOKIE_TYPE,
              class COOKIE_BASE>
    ManagedPtr(MANAGED_TYPE *ptr,
               COOKIE_TYPE  *cookie,
               void        (*deleter)(MANAGED_BASE *, COOKIE_BASE *));
        // [!DEPRECATED!]: Instead, use:
        //..
        //  template <class MANAGED_TYPE>
        //  ManagedPtr(MANAGED_TYPE *ptr, void *cookie, DeleterFunc deleter);
        //..
        // Create a managed pointer having a target object referenced by the
        // specified 'ptr', owning the managed object '*ptr', and having a
        // deleter that will invoke the specified 'deleter' with the address of
        // the currently managed object, and with the specified 'cookie' (that
        // the deleter can use for its own purposes), unless '0 == ptr', in
        // which case create an empty managed pointer.  This constructor will
        // not compile unless 'MANAGED_TYPE *' is convertible to
        // 'TARGET_TYPE *', and 'MANAGED_TYPE *' is convertible to
        // 'MANAGED_BASE *'.  The deleter will invoke the destructor of
        // 'MANAGED_TYPE' rather than the destructor of 'TARGET_TYPE'.  The
        // behavior is undefined if 'ptr' is already managed by another object,
        // or if '0 == deleter && 0 != ptr'.  Note that this function is
        // *deprecated* as it relies on undefined compiler behavior for its
        // implementation (that luckily performs as required on every platform
        // supported by BDE).
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    ~ManagedPtr();
        // Destroy this managed pointer object.  Destroy the object managed by
        // this managed pointer by invoking the user-supplied deleter, unless
        // this managed pointer is empty, in which case the deleter will *not*
        // be called.

    // MANIPULATORS
    ManagedPtr& operator=(ManagedPtr& rhs)               BSLS_KEYWORD_NOEXCEPT;
    ManagedPtr& operator=(bslmf::MovableRef<ManagedPtr> rhs)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // If this object and the specified 'rhs' manage the same object,
        // return a reference to this managed pointer; otherwise, destroy the
        // managed object owned by this managed pointer, transfer ownership of
        // the managed object owned by 'rhs' to this managed pointer, set this
        // managed pointer to point to the target object referenced by 'rhs',
        // reset 'rhs' to empty, and return a reference to this managed
        // pointer.

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    template <class BDE_OTHER_TYPE>
    typename bsl::enable_if<
        bsl::is_convertible<BDE_OTHER_TYPE *, TARGET_TYPE *>::value,
        ManagedPtr<TARGET_TYPE> >::type&
    operator=(ManagedPtr<BDE_OTHER_TYPE>&& rhs) BSLS_KEYWORD_NOEXCEPT;
#elif defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    // sun compiler version 12.3 and earlier
    template <class BDE_OTHER_TYPE>
    ManagedPtr<TARGET_TYPE>&
    operator=(bslmf::MovableRef<ManagedPtr<BDE_OTHER_TYPE> > rhs)
        BSLS_KEYWORD_NOEXCEPT;
#else // c++03 except old (version <= 12.3) sun compilers
    template <class BDE_OTHER_TYPE>
    typename bsl::enable_if<
        bsl::is_convertible<BDE_OTHER_TYPE *, TARGET_TYPE *>::value,
        ManagedPtr<TARGET_TYPE> >::type&
    operator=(bslmf::MovableRef<ManagedPtr<BDE_OTHER_TYPE> > rhs)
        BSLS_KEYWORD_NOEXCEPT;
#endif
        // If this object and the specified 'rhs' manage the same object,
        // return a reference to this managed pointer; otherwise, destroy the
        // managed object owned by this managed pointer, transfer ownership of
        // the managed object owned by 'rhs' to this managed pointer, set this
        // managed pointer to point to the target object referenced by 'rhs',
        // reset 'rhs' to empty, and return a reference to this managed
        // pointer.  'TARGET_TYPE' must be an accessible and unambiguous base
        // of 'BDE_OTHER_TYPE'

    ManagedPtr& operator=(ManagedPtr_Ref<TARGET_TYPE> ref)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // If this object and the managed pointer reference by the specified
        // 'ref' manage the same object, return a reference to this managed
        // pointer; otherwise, destroy the managed object owned by this managed
        // pointer, transfer ownership of the managed object owned by the
        // managed pointer referenced by 'ref', and set this managed pointer to
        // point to the target object currently referenced the managed pointer
        // referenced by 'ref'; then reset the managed pointer referenced by
        // 'ref' to empty, and return a reference to this managed pointer.
        // This operator is (implicitly) used to assign from a managed pointer
        // rvalue, or from a managed pointer to a "compatible" type, where
        // "compatible" means a built-in conversion from 'MANAGED_TYPE *' to
        // 'TARGET_TYPE *' is defined, e.g., 'derived *' to 'base *', 'T *' to
        // 'const T *', or 'T *' to 'void *'.

    ManagedPtr& operator=(bsl::nullptr_t);
        // Destroy the current managed object (if any) and reset this managed
        // pointer to empty.

    template <class REFERENCED_TYPE>
    operator ManagedPtr_Ref<REFERENCED_TYPE>();
        // Return a managed pointer reference, referring to this object.  Note
        // that this conversion operator is used implicitly to allow the
        // construction of managed pointers from rvalues because temporaries
        // cannot be passed by references offering modifiable access.

    void clear();
        // [!DEPRECATED!] Use 'reset' instead.
        //
        // Destroy the current managed object (if any) and reset this managed
        // pointer to empty.

    template <class MANAGED_TYPE>
    void load(MANAGED_TYPE *ptr);
        // Destroy the currently managed object, if any.  Then, set the target
        // object of this managed pointer to be that referenced by the
        // specified 'ptr', take ownership of '*ptr' as the currently managed
        // object, and set a deleter that uses the currently installed default
        // allocator to destroy the managed object when invoked (e.g., when
        // this managed pointer object is destroyed), unless '0 == ptr', in
        // which case reset this managed pointer to empty.  The deleter will
        // invoke the destructor of 'MANAGED_TYPE' rather than the destructor
        // of 'TARGET_TYPE'.  This function will not compile unless
        // 'MANAGED_TYPE *' is convertible to 'TARGET_TYPE *'.  The behavior is
        // undefined unless the managed object (if any) can be destroyed by the
        // currently installed default allocator, or if the lifetime of the
        // managed object is already managed by another object.

    template <class MANAGED_TYPE, class FACTORY_TYPE>
    void load(MANAGED_TYPE *ptr, FACTORY_TYPE *factory);
        // Destroy the currently managed object, if any.  Then, set the target
        // object of this managed pointer to be that referenced by the
        // specified 'ptr', take ownership of '*ptr' as the currently managed
        // object, and set a deleter that calls 'factory->deleteObject(ptr)' to
        // destroy the managed object when invoked (e.g., when this managed
        // pointer object is destroyed), unless '0 == ptr', in which case reset
        // this managed pointer to empty.  The deleter will invoke the
        // destructor of 'MANAGED_TYPE' rather than the destructor of
        // 'TARGET_TYPE'.  This function will not compile unless
        // 'MANAGED_TYPE *' is convertible to 'TARGET_TYPE *'.  The behavior is
        // undefined unless the managed object (if any) can be destroyed by the
        // specified 'factory',  or if '0 == factory && 0 != ptr', or if the
        // the lifetime of the managed object is already managed by another
        // object.  Note that 'bslma::Allocator', and any class publicly and
        // unambiguously derived from 'bslma::Allocator', meets the
        // requirements for 'FACTORY_TYPE'.

    template <class MANAGED_TYPE>
    void load(MANAGED_TYPE *ptr, void *cookie, DeleterFunc deleter);
        // Destroy the currently managed object, if any.  Then, set the target
        // object of this managed pointer to be that referenced by the
        // specified 'ptr', take ownership of '*ptr' as the currently managed
        // object, and set a deleter that will invoke the specified 'deleter'
        // with the address of the currently managed object, and with the
        // specified 'cookie' (that the deleter can use for its own purposes),
        // unless '0 == ptr', in which case reset this managed pointer to
        // empty.  The behavior is undefined if 'ptr' is already managed by
        // another object, or if '0 == deleter && 0 != ptr'.  Note that GCC 3.4
        // and earlier versions have a bug in template type deduction/overload
        // resolution that causes ambiguities if this signature is available.
        // This function will be restored on that platform once the deprecated
        // signatures are finally removed.

    void load(bsl::nullptr_t = 0, void *cookie = 0, DeleterFunc deleter = 0);
        // Destroy the current managed object (if any) and reset this managed
        // pointer to empty.  Note that the optionally specified 'cookie' and
        // 'deleter' will be ignored, as empty managed pointers do not invoke a
        // deleter.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    template <class MANAGED_TYPE, class COOKIE_TYPE>
    void load(MANAGED_TYPE *ptr, COOKIE_TYPE *cookie, DeleterFunc deleter);

    template <class MANAGED_TYPE, class MANAGED_BASE>
    void load(MANAGED_TYPE *ptr,
              void         *cookie,
              void        (*deleter)(MANAGED_BASE *, void *));

    template <class MANAGED_TYPE,
              class MANAGED_BASE,
              class COOKIE_TYPE,
              class COOKIE_BASE>
    void load(MANAGED_TYPE *ptr,
              COOKIE_TYPE  *cookie,
              void        (*deleter)(MANAGED_BASE *, COOKIE_BASE *));
        // [!DEPRECATED!]: Instead, use:
        //..
        //  template <class MANAGED_TYPE>
        //  void load(MANAGED_TYPE *ptr, void *cookie, DeleterFunc deleter);
        //..
        // Destroy the currently managed object, if any.  Then, set the target
        // object of this managed pointer to be that referenced by the
        // specified 'ptr', take ownership of '*ptr' as the currently managed
        // object, and set a deleter that will invoke the specified 'deleter'
        // with the address of the currently managed object, and with the
        // specified 'cookie' (that the deleter can use for its own purposes),
        // unless '0 == ptr', in which case reset this managed pointer to
        // empty.  The behavior is undefined if 'ptr' is already managed by
        // another object, or if '0 == deleter && 0 != ptr'.  Note that this
        // function is *deprecated* as it relies on undefined compiler behavior
        // for its implementation, but luckily perform as required for all
        // currently supported platforms; on platforms where the non-deprecated
        // overload is not available (e.g., GCC 3.4) code should be written as
        // if it were available, as an appropriate (deprecated) overload will
        // be selected with the correct (non-deprecated) behavior.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    template <class ALIASED_TYPE>
    void loadAlias(ManagedPtr<ALIASED_TYPE>& alias, TARGET_TYPE *ptr);
        // If the specified 'alias' manages the same object as this managed
        // pointer, set the target object of this managed pointer to be that
        // referenced by the specified 'ptr'; otherwise, destroy the currently
        // managed object (if any), and if 'alias' is empty, reset this managed
        // pointer to empty; otherwise, transfer ownership (and the deleter) of
        // the object managed by 'alias', and set the target object of this
        // managed pointer to be that referenced by 'ptr'.  The behavior is
        // undefined if '0 == ptr' and 'alias' is not empty, or if '0 != ptr'
        // and 'alias' is empty, or if 'ptr' is already managed by a managed
        // pointer other than 'alias'.  Note that this establishes a managed
        // pointer where 'ptr' aliases 'alias'.  The managed object for 'alias'
        // will ultimately be destroyed, and the destructor for 'ptr' is not
        // called directly.

    ManagedPtr_PairProxy<TARGET_TYPE, ManagedPtrDeleter> release();
        // Return a raw pointer to the current target object (if any) and the
        // deleter for the currently managed object, and reset this managed
        // pointer to empty.  It is undefined behavior to run the returned
        // deleter unless the returned pointer to target object is not null.

    TARGET_TYPE *release(ManagedPtrDeleter *deleter);
        // Load the specified 'deleter' for the currently managed object and
        // reset this managed pointer to empty.  Return a raw pointer to the
        // target object (if any) managed by this pointer.  It is undefined
        // behavior to run the returned deleter unless the returned pointer to
        // target object is not null.

    void reset();
        // Destroy the current managed object (if any) and reset this managed
        // pointer to empty.

    void swap(ManagedPtr& other);
        // Exchange the value and ownership of this managed pointer with the
        // specified 'other' managed pointer.

    // ACCESSORS
    operator BoolType() const;
        // Return a value of "unspecified bool" type that evaluates to 'false'
        // if this managed pointer is empty, and 'true' otherwise.  Note that
        // this conversion operator allows a managed pointer to be used within
        // a conditional context, such as within an 'if' or 'while' statement,
        // but does *not* allow managed pointers to be compared (e.g., via '<'
        // or '>').  Also note that a superior solution is available in C++11
        // using the 'explicit operator bool()' syntax, that removes the need
        // for a special boolean-like type and private equality-comparison
        // operators.

    typename bslmf::AddReference<TARGET_TYPE>::Type operator*() const;
        // Return a reference to the target object.  The behavior is undefined
        // if this managed pointer is empty, or if 'TARGET_TYPE' is 'void' or
        // 'const void'.

    TARGET_TYPE *operator->() const;
        // Return the address of the target object, or 0 if this managed
        // pointer is empty.

    const ManagedPtrDeleter& deleter() const;
        // Return a reference to the non-modifiable deleter information
        // associated with this managed pointer.  The behavior is undefined if
        // this managed pointer is empty.

    TARGET_TYPE *get() const;
        // Return the address of the target object, or 0 if this managed
        // pointer is empty.

    TARGET_TYPE *ptr() const;
        // [!DEPRECATED!]: Use 'get' instead.
        //
        // Return the address of the target object, or 0 if this managed
        // pointer is empty.
};

// FREE FUNCTIONS
template <class TARGET_TYPE>
void swap(ManagedPtr<TARGET_TYPE>& a, ManagedPtr<TARGET_TYPE>& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.

                        // =====================
                        // struct ManagedPtrUtil
                        // =====================

struct ManagedPtrUtil {
    // This utility class provides a general no-op deleter, which is useful
    // when creating managed pointers to stack-allocated objects.

    // CLASS METHODS
    static void noOpDeleter(void *, void *);
        // Deleter function that does nothing.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14

    template <class ELEMENT_TYPE, class... ARGS>
    static ManagedPtr<ELEMENT_TYPE> allocateManaged(
                                              bslma::Allocator *basicAllocator,
                                              ARGS&&...         args);
        // Create an object of the (template parameter) 'ELEMENT_TYPE' from the
        // specified 'args...' arguments, and return a 'ManagedPtr' to manage
        // the new object.  Use the specified 'basicAllocator' to supply memory
        // for the footprint of the new object and implicitly pass
        // 'basicAllocator' as the last argument of its constructor if
        // 'bslma::UsesBslmaAllocator<ELEMENT_TYPE>::value' is 'true'.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    template <class ELEMENT_TYPE, class... ARGS>
    static ManagedPtr<ELEMENT_TYPE> makeManaged(ARGS&&... args);
        // Create an object of the (template parameter) 'ELEMENT_TYPE' from the
        // specified 'args...' arguments, and return a 'ManagedPtr' to manage
        // the new object.  Use the currently installed default allocator to
        // supply memory for the footprint of the new object but do *not*
        // implicitly pass the default allocator as the last argument of its
        // constructor even if 'bslma::UsesBslmaAllocator<ELEMENT_TYPE>::value'
        // is 'true'.  Note that an allocator may be included in 'args' but see
        // 'allocateManaged' for an alternative function that is better suited
        // to creating managed pointers to objects of allocator-aware type.

#endif
};

                     // ===========================
                     // struct ManagedPtrNilDeleter
                     // ===========================

template <class TARGET_TYPE>
struct ManagedPtrNilDeleter {
    // [!DEPRECATED!]: Use 'ManagedPtrUtil::noOpDeleter' instead.
    //
    // This utility class provides a general no-op deleter, which is useful
    // when creating managed pointers to stack-allocated objects.  Note that
    // the non-template class 'ManagedPtrUtil' should be used in preference to
    // this deprecated class, avoiding both template bloat and undefined
    // behavior.

    // CLASS METHODS
    static void deleter(void *, void *);
        // Deleter function that does nothing.
};

             // ===========================================
             // private class ManagedPtr_FactoryDeleterType
             // ===========================================

template <class TARGET_TYPE, class FACTORY_TYPE>
struct ManagedPtr_FactoryDeleterType
    : bsl::conditional<
                 bsl::is_convertible<FACTORY_TYPE *, Allocator *>::value,
                 ManagedPtr_FactoryDeleter<TARGET_TYPE, Allocator>,
                 ManagedPtr_FactoryDeleter<TARGET_TYPE, FACTORY_TYPE> > {
    // This metafunction class-template provides a means to compute the
    // preferred deleter function for a factory class for those methods of
    // 'ManagedPtr' that supply only a factory, and no additional deleter
    // function.  The intent is to use a common deleter function for all
    // allocators that implement the 'bslma::Allocator' protocol, rather than
    // create a special deleter function based on the complete type of each
    // allocator, each doing the same thing (invoking the virtual function
    // 'deleteObject').
};

              // ========================================
              // private struct ManagedPtr_DefaultDeleter
              // ========================================

template <class MANAGED_TYPE>
struct ManagedPtr_DefaultDeleter {
    // This 'struct' provides a function-like managed pointer deleter that
    // invokes 'delete' with the passed pointer.

    // CLASS METHODS
    static void deleter(void *ptr, void *);
        // Cast the specified 'ptr' to (template parameter) type
        // 'MANAGED_TYPE *', and then call 'delete' with the cast pointer.
};


// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                      // ---------------------------------
                      // private struct ManagedPtr_ImpUtil
                      // ---------------------------------

// CLASS METHODS
template <class TYPE>
inline
void *ManagedPtr_ImpUtil::voidify(TYPE *address) BSLS_KEYWORD_NOEXCEPT
{
    return static_cast<void *>(
            const_cast<typename bsl::remove_cv<TYPE>::type *>(address));
}

template <class TYPE>
inline
TYPE *ManagedPtr_ImpUtil::unqualify(const volatile TYPE *address)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    return const_cast<TYPE *>(address);
}

                      // ----------------------------
                      // private class ManagedPtr_Ref
                      // ----------------------------

// CREATORS
template <class TARGET_TYPE>
inline
ManagedPtr_Ref<TARGET_TYPE>::ManagedPtr_Ref(ManagedPtr_Members *base,
                                            TARGET_TYPE        *target)
: d_base_p(base)
, d_cast_p(target)
{
    BSLS_ASSERT_SAFE(0 != base);
}

template <class TARGET_TYPE>
inline
ManagedPtr_Ref<TARGET_TYPE>::~ManagedPtr_Ref()
{
    BSLS_ASSERT_SAFE(0 != d_base_p);
}

// ACCESSORS
template <class TARGET_TYPE>
inline
ManagedPtr_Members *ManagedPtr_Ref<TARGET_TYPE>::base() const
{
    return d_base_p;
}

template <class TARGET_TYPE>
inline
TARGET_TYPE *ManagedPtr_Ref<TARGET_TYPE>::target() const
{
    return d_cast_p;
}

                           // ----------------
                           // class ManagedPtr
                           // ----------------

template <class TARGET_TYPE>
class ManagedPtr<volatile TARGET_TYPE>;
    // This specialization is declared but not defined, in order to provide an
    // early compile-fail check to catch misuse of managed pointer to
    // 'volatile' types, which is explicitly called out as not supported in the
    // primary class template contract.

template <class TARGET_TYPE>
class ManagedPtr<TARGET_TYPE&>;
    // This specialization is declared but not defined, in order to provide an
    // early compile-fail check to catch misuse of managed pointer to reference
    // types, which is explicitly called out as not supported in the primary
    // class template contract.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TARGET_TYPE>
class ManagedPtr<TARGET_TYPE&&>;
    // This specialization is declared but not defined, in order to provide an
    // early compile-fail check to catch misuse of managed pointer to reference
    // types, which is explicitly called out as not supported in the primary
    // class template contract.
#endif

// PRIVATE CLASS METHODS
template <class TARGET_TYPE>
inline
void *ManagedPtr<TARGET_TYPE>::stripBasePointerType(TARGET_TYPE *ptr)
{
    return const_cast<void *>(static_cast<const void *>(ptr));
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE>
inline
void *
ManagedPtr<TARGET_TYPE>::stripCompletePointerType(MANAGED_TYPE *ptr)
{
    return const_cast<void *>(static_cast<const void *>(ptr));
}

// PRIVATE MANIPULATORS
template <class TARGET_TYPE>
template <class MANAGED_TYPE>
inline
void ManagedPtr<TARGET_TYPE>::loadImp(MANAGED_TYPE *ptr,
                                      void         *cookie,
                                      DeleterFunc   deleter)
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::value));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    d_members.runDeleter();
    d_members.set(stripCompletePointerType(ptr), cookie, deleter);
    d_members.setAliasPtr(stripBasePointerType(ptr));
}

// CREATORS
template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr()
: d_members()
{
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(MANAGED_TYPE *ptr)
: d_members(stripCompletePointerType(ptr),
            0,
            &ManagedPtr_DefaultDeleter<MANAGED_TYPE>::deleter,
            stripBasePointerType(ptr))
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
}

template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(ManagedPtr_Ref<TARGET_TYPE> ref)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_members(*ref.base())
{
    d_members.setAliasPtr(stripBasePointerType(ref.target()));
}

template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(ManagedPtr& original) BSLS_KEYWORD_NOEXCEPT
: d_members(original.d_members)
{
}

template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(bslmf::MovableRef<ManagedPtr> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_members(MoveUtil::access(original).d_members)
{
}

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    template <class TARGET_TYPE>
    template <class BDE_OTHER_TYPE>
    inline
    ManagedPtr<TARGET_TYPE>::ManagedPtr(
            ManagedPtr<BDE_OTHER_TYPE> &&original,
            typename bsl::enable_if<bsl::is_convertible<BDE_OTHER_TYPE *,
                                                        TARGET_TYPE *>::value,
                                    ManagedPtr_TraitConstraint>::type)
        BSLS_KEYWORD_NOEXCEPT
    : d_members(original.d_members)
#elif defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    // sun compiler version 12.3 and earlier
    template <class TARGET_TYPE>
    template <class BDE_OTHER_TYPE>
    inline
    ManagedPtr<TARGET_TYPE>::ManagedPtr(
            bslmf::MovableRef<ManagedPtr<BDE_OTHER_TYPE> > original)
        BSLS_KEYWORD_NOEXCEPT
    : d_members(MoveUtil::access(original).d_members)
#else // c++03 except old (version <= 12.3) sun compilers
    template <class TARGET_TYPE>
    template <class BDE_OTHER_TYPE>
    inline
    ManagedPtr<TARGET_TYPE>::ManagedPtr(
            bslmf::MovableRef<ManagedPtr<BDE_OTHER_TYPE> > original,
            typename bsl::enable_if<bsl::is_convertible<BDE_OTHER_TYPE *,
                                                        TARGET_TYPE *>::value,
                                    ManagedPtr_TraitConstraint>::type)
        BSLS_KEYWORD_NOEXCEPT
    : d_members(MoveUtil::access(original).d_members)
#endif
{
    // This constructor cannot be constrained using a type trait on old Sun
    // compilers (version <= 12.3), so we need the check here.
    #if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
        BSLMF_ASSERT((bsl::is_convertible<BDE_OTHER_TYPE *,
                                          TARGET_TYPE *>::VALUE));
    #endif

    // To deal with the possibility of multiple inheritance, we need to
    // "correct" the target pointer.
    d_members.setAliasPtr(
        stripBasePointerType(
            static_cast<TARGET_TYPE *>(
                static_cast<BDE_OTHER_TYPE *>(
                    d_members.pointer()))));
}


template <class TARGET_TYPE>
template <class ALIASED_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(ManagedPtr<ALIASED_TYPE>&  alias,
                                    TARGET_TYPE               *ptr)
: d_members()
{
    BSLS_ASSERT_SAFE(0 != alias.get() || 0 == ptr);

    if (0 != ptr) {
        d_members.move(&alias.d_members);
        d_members.setAliasPtr(stripBasePointerType(ptr));
    }
}

template <class TARGET_TYPE>
template <class ALIASED_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(
#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
                           ManagedPtr<ALIASED_TYPE>&&                    alias,
#else
                           bslmf::MovableRef<ManagedPtr<ALIASED_TYPE> >  alias,
#endif
                           TARGET_TYPE                                  *ptr)
: d_members()
{
    ManagedPtr<ALIASED_TYPE>& lvalue = alias;

    BSLS_ASSERT_SAFE(0 != lvalue.get() || 0 == ptr);

    if (0 != ptr) {
        d_members.move(&lvalue.d_members);
        d_members.setAliasPtr(stripBasePointerType(ptr));
    }
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE, class FACTORY_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(MANAGED_TYPE *ptr, FACTORY_TYPE *factory)
: d_members(stripCompletePointerType(ptr),
            factory,
            &ManagedPtr_FactoryDeleterType<MANAGED_TYPE,
                                           FACTORY_TYPE>::type::deleter,
            stripBasePointerType(ptr))
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::value));
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);
}

template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(bsl::nullptr_t, bsl::nullptr_t)
: d_members()
{
}

template <class TARGET_TYPE>
template <class FACTORY_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(bsl::nullptr_t, FACTORY_TYPE *)
: d_members()
{
}

template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(TARGET_TYPE *ptr,
                                    void        *cookie,
                                    DeleterFunc  deleter)
: d_members(stripBasePointerType(ptr), cookie, deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(MANAGED_TYPE *ptr,
                                    void         *cookie,
                                    DeleterFunc   deleter)
: d_members(stripCompletePointerType(ptr),
            cookie,
            deleter,
            stripBasePointerType(ptr))
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::value));

    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class TARGET_TYPE>
template <class MANAGED_TYPE, class MANAGED_BASE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(
                                MANAGED_TYPE *ptr,
                                void         *cookie,
                                void        (*deleter)(MANAGED_BASE *, void *))
: d_members(stripCompletePointerType(ptr),
            cookie,
            reinterpret_cast<DeleterFunc>(deleter),
            stripBasePointerType(ptr))
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::value));
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *,
                                      const MANAGED_BASE *>::value));

    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE,
          class MANAGED_BASE,
          class COOKIE_TYPE,
          class COOKIE_BASE>
inline
ManagedPtr<TARGET_TYPE>::ManagedPtr(
                         MANAGED_TYPE *ptr,
                         COOKIE_TYPE  *cookie,
                         void        (*deleter)(MANAGED_BASE *, COOKIE_BASE *))
: d_members(stripCompletePointerType(ptr),
            static_cast<COOKIE_BASE *>(cookie),
            reinterpret_cast<DeleterFunc>(deleter),
            stripBasePointerType(ptr))
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::value));
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *,
                                      const MANAGED_BASE *>::value));
    BSLMF_ASSERT((bsl::is_convertible<COOKIE_TYPE *, COOKIE_BASE *>::value));

    // Note that the undefined behavior embodied in the 'reinterpret_cast'
    // above could be removed by inserting an additional forwarding function
    // truly of type 'DeleterFunc' which 'reinterpret_cast's each pointer
    // argument as part of its forwarding behavior.  We choose not to do this
    // on the grounds of simple efficiency, and there is currently no known
    // supported compiler that we use where this does not work as desired.

    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>::~ManagedPtr()
{
    d_members.runDeleter();
}

// MANIPULATORS
template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>&
ManagedPtr<TARGET_TYPE>::operator=(ManagedPtr& rhs)       BSLS_KEYWORD_NOEXCEPT
{
    d_members.moveAssign(&rhs.d_members);
    return *this;
}

template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>&
ManagedPtr<TARGET_TYPE>::operator=(bslmf::MovableRef<ManagedPtr> rhs)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    ManagedPtr& lvalue = rhs;
    d_members.moveAssign(&lvalue.d_members);
    return *this;
}

#if defined(BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES)
    template <class TARGET_TYPE>
    template <class BDE_OTHER_TYPE>
    inline
    typename bsl::enable_if<
        bsl::is_convertible<BDE_OTHER_TYPE*, TARGET_TYPE*>::value,
        ManagedPtr<TARGET_TYPE> >::type&
    ManagedPtr<TARGET_TYPE>::operator =(
            ManagedPtr<BDE_OTHER_TYPE>&& rhs) BSLS_KEYWORD_NOEXCEPT
#elif defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    // sun compiler version 12.3 and earlier
    template <class TARGET_TYPE>
    template <class BDE_OTHER_TYPE>
    inline
    ManagedPtr<TARGET_TYPE>&
    ManagedPtr<TARGET_TYPE>::operator =(
          bslmf::MovableRef<ManagedPtr<BDE_OTHER_TYPE> > rhs)
        BSLS_KEYWORD_NOEXCEPT
#else // c++03 except old (version <= 12.3) sun compilers
    template <class TARGET_TYPE>
    template <class BDE_OTHER_TYPE>
    inline
    typename bsl::enable_if<
        bsl::is_convertible<BDE_OTHER_TYPE *, TARGET_TYPE *>::value,
        ManagedPtr<TARGET_TYPE> >::type&
    ManagedPtr<TARGET_TYPE>::operator =(
          bslmf::MovableRef<ManagedPtr<BDE_OTHER_TYPE> > rhs)
        BSLS_KEYWORD_NOEXCEPT
#endif
{
    // This operator cannot be constrained using a type trait on Sun, so we
    // need the check here.
    #if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
        BSLMF_ASSERT((bsl::is_convertible<BDE_OTHER_TYPE *,
                                          TARGET_TYPE *>::VALUE));
    #endif

    ManagedPtr<BDE_OTHER_TYPE>& lvalue = rhs;
    d_members.moveAssign(&lvalue.d_members);

    // To deal with the possibility of multiple inheritance, we need to
    // "correct" the target pointer.
    d_members.setAliasPtr(
        stripBasePointerType(
            static_cast<TARGET_TYPE *>(
                static_cast<BDE_OTHER_TYPE *>(
                    d_members.pointer()))));

    return *this;
}

template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>&
ManagedPtr<TARGET_TYPE>::operator=(ManagedPtr_Ref<TARGET_TYPE> ref)
                                                          BSLS_KEYWORD_NOEXCEPT
{
    d_members.moveAssign(ref.base());
    d_members.setAliasPtr(stripBasePointerType(ref.target()));
    return *this;
}

template <class TARGET_TYPE>
inline
ManagedPtr<TARGET_TYPE>&
ManagedPtr<TARGET_TYPE>::operator=(bsl::nullptr_t)
{
    this->reset();
    return *this;
}

template <class TARGET_TYPE>
template <class REFERENCED_TYPE>
inline
ManagedPtr<TARGET_TYPE>::operator ManagedPtr_Ref<REFERENCED_TYPE>()
{
    BSLMF_ASSERT((bsl::is_convertible<TARGET_TYPE *,
                                      REFERENCED_TYPE *>::VALUE));

    return ManagedPtr_Ref<REFERENCED_TYPE>(&d_members,
                             static_cast<REFERENCED_TYPE *>(
                             static_cast<TARGET_TYPE *>(d_members.pointer())));
}

template <class TARGET_TYPE>
inline
void ManagedPtr<TARGET_TYPE>::clear()
{
    reset();
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE>
inline
void ManagedPtr<TARGET_TYPE>::load(MANAGED_TYPE *ptr)
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::value));

    typedef ManagedPtr_FactoryDeleter<MANAGED_TYPE, Allocator> DeleterFactory;
    this->loadImp(ptr,
                  static_cast<void *>(Default::allocator()),
                  &DeleterFactory::deleter);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE, class FACTORY_TYPE>
inline
void ManagedPtr<TARGET_TYPE>::load(MANAGED_TYPE *ptr, FACTORY_TYPE *factory)
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::value));
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);

    typedef typename
    ManagedPtr_FactoryDeleterType<MANAGED_TYPE, FACTORY_TYPE>::type
                                                                DeleterFactory;

    this->loadImp(ptr, static_cast<void *>(factory), &DeleterFactory::deleter);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE>
inline
void ManagedPtr<TARGET_TYPE>::load(MANAGED_TYPE *ptr,
                                   void         *cookie,
                                   DeleterFunc   deleter)
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::value));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    this->loadImp(ptr, cookie, deleter);
}

template <class TARGET_TYPE>
inline
void ManagedPtr<TARGET_TYPE>::load(bsl::nullptr_t, void *, DeleterFunc)
{
    this->reset();
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class TARGET_TYPE>
template <class MANAGED_TYPE, class COOKIE_TYPE>
inline
void ManagedPtr<TARGET_TYPE>::load(MANAGED_TYPE *ptr,
                                   COOKIE_TYPE  *cookie,
                                   DeleterFunc   deleter)
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::value));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    this->loadImp(ptr, static_cast<void *>(cookie), deleter);
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE, class MANAGED_BASE>
inline
void ManagedPtr<TARGET_TYPE>::load(
                                MANAGED_TYPE *ptr,
                                void         *cookie,
                                void        (*deleter)(MANAGED_BASE *, void *))
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
    BSLMF_ASSERT((!bsl::is_void<MANAGED_BASE>::value));
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, MANAGED_BASE *>::value));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    this->loadImp(ptr, cookie, reinterpret_cast<DeleterFunc>(deleter));
}

template <class TARGET_TYPE>
template <class MANAGED_TYPE,
          class MANAGED_BASE,
          class COOKIE_TYPE,
          class COOKIE_BASE>
inline
void ManagedPtr<TARGET_TYPE>::load(
                         MANAGED_TYPE *ptr,
                         COOKIE_TYPE  *cookie,
                         void        (*deleter)(MANAGED_BASE *, COOKIE_BASE *))
{
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, TARGET_TYPE *>::VALUE));
    BSLMF_ASSERT((bsl::is_convertible<MANAGED_TYPE *, MANAGED_BASE *>::VALUE));
    BSLMF_ASSERT((bsl::is_convertible<COOKIE_TYPE *, COOKIE_BASE *>::VALUE));
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    this->loadImp(ptr,
                  static_cast<void *>(static_cast<COOKIE_BASE *>(cookie)),
                  reinterpret_cast<DeleterFunc>(deleter));
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED

template <class TARGET_TYPE>
template <class ALIASED_TYPE>
void ManagedPtr<TARGET_TYPE>::loadAlias(ManagedPtr<ALIASED_TYPE>&  alias,
                                        TARGET_TYPE               *ptr)
{
    BSLS_ASSERT_SAFE(!ptr == !alias.get());  // both null or both non-null

    if (ptr && alias.d_members.pointer()) {
        d_members.moveAssign(&alias.d_members);
        d_members.setAliasPtr(stripBasePointerType(ptr));
    }
    else {
        d_members.runDeleter();
        d_members.clear();
    }
}

template <class TARGET_TYPE>
ManagedPtr_PairProxy<TARGET_TYPE, ManagedPtrDeleter>
ManagedPtr<TARGET_TYPE>::release()
{
    typedef ManagedPtr_PairProxy<TARGET_TYPE, ManagedPtrDeleter> ResultType;

    TARGET_TYPE *p = get();

    // The behavior would be undefined if 'd_members.deleter()' were called
    // when 'p' is null.

    if (p) {
        ResultType result = { p, d_members.deleter() };
        d_members.clear();
        return result;                                                // RETURN
    }
    ResultType result = { p, ManagedPtrDeleter() };
    return result;
}

template <class TARGET_TYPE>
TARGET_TYPE *ManagedPtr<TARGET_TYPE>::release(ManagedPtrDeleter *deleter)
{
    BSLS_ASSERT_SAFE(deleter);

    TARGET_TYPE *result = get();

    // The behavior is undefined if 'd_members.deleter()' is called when
    // 'result' is null.

    if (result) {
        *deleter = d_members.deleter();
        d_members.clear();
    }
    return result;
}

template <class TARGET_TYPE>
inline
void ManagedPtr<TARGET_TYPE>::reset()
{
    d_members.runDeleter();
    d_members.clear();
}

template <class TARGET_TYPE>
inline
void ManagedPtr<TARGET_TYPE>::swap(ManagedPtr& other)
{
    d_members.swap(other.d_members);
}

// ACCESSORS
template <class TARGET_TYPE>
inline
#if defined(BSLS_PLATFORM_CMP_IBM)      // last confirmed with xlC 12.1
ManagedPtr<TARGET_TYPE>::operator typename ManagedPtr::BoolType() const
#else
ManagedPtr<TARGET_TYPE>::operator BoolType() const
#endif
{
    return d_members.pointer()
           ? bsls::UnspecifiedBool<ManagedPtr>::trueValue()
           : bsls::UnspecifiedBool<ManagedPtr>::falseValue();
}

template <class TARGET_TYPE>
inline
typename bslmf::AddReference<TARGET_TYPE>::Type
ManagedPtr<TARGET_TYPE>::operator*() const
{
    BSLS_ASSERT_SAFE(d_members.pointer());

    return *static_cast<TARGET_TYPE *>(d_members.pointer());
}

template <class TARGET_TYPE>
inline
TARGET_TYPE *ManagedPtr<TARGET_TYPE>::operator->() const
{
    return static_cast<TARGET_TYPE *>(d_members.pointer());
}

template <class TARGET_TYPE>
inline
const ManagedPtrDeleter& ManagedPtr<TARGET_TYPE>::deleter() const
{
    BSLS_ASSERT_SAFE(d_members.pointer());

    return d_members.deleter();
}

template <class TARGET_TYPE>
inline
TARGET_TYPE *ManagedPtr<TARGET_TYPE>::get() const
{
    return static_cast<TARGET_TYPE *>(d_members.pointer());
}

template <class TARGET_TYPE>
inline
TARGET_TYPE *ManagedPtr<TARGET_TYPE>::ptr() const
{
    return get();
}

// FREE FUNCTIONS
template <class TARGET_TYPE>
inline
void swap(ManagedPtr<TARGET_TYPE>& a, ManagedPtr<TARGET_TYPE>& b)
{
    a.swap(b);
}

                      // --------------------
                      // class ManagedPtrUtil
                      // --------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14

template <class ELEMENT_TYPE, class... ARGS>
inline
ManagedPtr<ELEMENT_TYPE> ManagedPtrUtil::allocateManaged(
                                              bslma::Allocator *basicAllocator,
                                              ARGS&&...         args)
{
    bslma::Allocator *allocator = bslma::Default::allocator(basicAllocator);

    ELEMENT_TYPE *objPtr = static_cast<ELEMENT_TYPE *>(
                                    allocator->allocate(sizeof(ELEMENT_TYPE)));

    bslma::DeallocatorProctor<bslma::Allocator> proctor(
                                           ManagedPtr_ImpUtil::voidify(objPtr),
                                           allocator);
    bslma::ConstructionUtil::construct(
                                 ManagedPtr_ImpUtil::unqualify(objPtr),
                                 allocator,
                                 BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    proctor.release();

    return ManagedPtr<ELEMENT_TYPE>(objPtr, allocator);
}

template <class ELEMENT_TYPE, class... ARGS>
inline
ManagedPtr<ELEMENT_TYPE> ManagedPtrUtil::makeManaged(ARGS&&... args)
{
    bslma::Allocator *defaultAllocator = bslma::Default::defaultAllocator();

    ELEMENT_TYPE *objPtr = static_cast<ELEMENT_TYPE *>(
                             defaultAllocator->allocate(sizeof(ELEMENT_TYPE)));

    bslma::DeallocatorProctor<bslma::Allocator> proctor(
                                           ManagedPtr_ImpUtil::voidify(objPtr),
                                           defaultAllocator);
    ::new (ManagedPtr_ImpUtil::voidify(objPtr)) ELEMENT_TYPE(
                                 BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);
    proctor.release();

    return ManagedPtr<ELEMENT_TYPE>(objPtr, defaultAllocator);
}

#endif

                      // --------------------------
                      // class ManagedPtrNilDeleter
                      // --------------------------

// CLASS METHODS
template <class TARGET_TYPE>
inline
void ManagedPtrNilDeleter<TARGET_TYPE>::deleter(void *, void *)
{
}

              // ----------------------------------------
              // private struct ManagedPtr_DefaultDeleter
              // ----------------------------------------

// CLASS METHODS
template <class MANAGED_TYPE>
inline
void ManagedPtr_DefaultDeleter<MANAGED_TYPE>::deleter(void *ptr, void *)
{
    delete reinterpret_cast<MANAGED_TYPE *>(ptr);
}

}  // close package namespace

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bslmf {

template <class TARGET_TYPE>
struct HasPointerSemantics<bslma::ManagedPtr<TARGET_TYPE> > : bsl::true_type
{
};

template <class TARGET_TYPE>
struct IsBitwiseMoveable<bslma::ManagedPtr<TARGET_TYPE> > : bsl::true_type
{
};

}  // close namespace bslmf
}  // close enterprise namespace

namespace bsl {

template <class TARGET_TYPE>
struct is_nothrow_move_constructible<
    BloombergLP::bslma::ManagedPtr<TARGET_TYPE> > : bsl::true_type
{
};

}  // close namespace bsl

#endif // End C++11 code

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
