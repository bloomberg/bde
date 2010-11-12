// bcef_bindutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BCEF_BINDUTIL
#define INCLUDED_BCEF_BINDUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a signature-specific function object (functor).
//
//@CLASSES:
//   bcef_BindWrapper: Reference counted function binder
//      bcef_BindUtil: Utility for constructing 'bcef_BindWrapper' objects
//
//@AUTHOR: Ilougino Rocha (irocha), Herve Bronnimann (hbronnimann)
//
//@DESCRIPTION: This component provides several factory methods for creating a
// parameterized binder mechanism ('bcef_Bind') similar to those of the
// component 'bdef_bind'.  The difference between a binder created by this
// component rather than by the factory methods in 'bdef_bind' is that the
// binder is returned by reference rather than by value, with shared ownership
// semantics.  Hence its main use is for creating binders that hold a
// non-trivial amount of storage (i.e., the bound arguments) and will be
// copied, possibly several times, such as jobs enqueued in a threadpool.
//
///Migrating from 'bdef_BindUtil::bind...' to bcef_BindUtil::bind...'
///------------------------------------------------------------------
// A call to 'bdef_BindUtil::bind' or 'bindR' can easily be replaced to another
// call to 'bcef_BindUtil::bind' or 'bindR', with almost no change in syntax.
// A common mistake however is to forget the allocator passed as an extra
// *first* *argument*.  A call to 'bdef_BindUtil::bindA', however, needs no
// change of syntax since the binder's allocator is already present as first
// argument.
//
// For further information about binder mechanisms, usage examples, etc., see
// the 'bdef_bind' component.  For completeness and compatibility with this
// component's test driver, we translate the documentation in terms of
// 'bcef_bindutil' (that is, replacing calls to 'bdef_BindUtil::bind' by
// 'bcef_BindUtil::bind', and adding the allocator argument where appropriate).
//
///Elementary construction and usage of 'bcef_BindWrapper' objects
///---------------------------------------------------------------
// Bound objects are generally constructed by invoking the 'bcef_BindUtil' with
// an "invocation template".  An invocation template is a series of one or
// more arguments that describe how to invoke the bound object.  Each argument
// can be either a place-holder or a literal value.  Literal values are stored
// by value in the binder and directly forwarded to the bound object when
// invoked.  Place-holders are substituted with the respective argument
// provided at invocation of the binder.  For example, given the following
// 'invocable' (here a free function for simplicity):
//..
//  void invocable(int i, int j, const char *str) {
//      // Do something with 'i', 'j' and 'str' ... e.g.:
//      printf("Invoked with: %d %d %s\n", i, j, str);
//  }
//..
// and the following (global) string:
//..
//  const char *someString = "p3"; // for third parameter to 'invocable'
//..
// we can bind the parameters of 'invocable' to the following arguments:
//..
//  void bindTest(bslma_Allocator *allocator = 0) {
//      bcef_BindUtil::bind(allocator,                 // allocator,
//                          &invocable,                // bound object and
//                          10, 14, (const char*)"p3") // bound arguments
//..
// and the binder declared above can be passed invocation arguments directly,
// as follows (here we specify zero invocation arguments since all the bound
// arguments are fully specified):
//..
//                                                 (); // invocation
//   }
//..
// In the function call above, the 'invocable' will be bound with the
// arguments '10', '14', and '"p3"' respectively, then invoked with those bound
// arguments.  In the next example, place-holders are used to forward
// user-provided arguments to the bound object.  We separate the invocation of
// the binder into a function template to avoid having to declare the type of
// the binder:
//..
//  template <class BINDER>
//  void callBinder(BINDER const& b)
//  {
//      b(10, 14);
//  }
//..
// The creation of the binder is as follows:
//..
//  void bindTest1(bslma_Allocator *allocator = 0) {
//      callBinder(bcef_BindUtil::bind(allocator,
//                                     &invocable,
//                                     _1, _2, someString));
//  }
//..
// In this code snippet, the 'callBinder' template function is invoked with a
// binder bound to the specified 'invocable' and having the invocation
// template '_1', '_2', and '"p3"' respectively.  The two special parameters
// '_1' and '_2' are place-holders for arguments one and two, respectively,
// which will be specified to the binder at invocation time.  Each place-holder
// will be substituted with the corresponding positional argument when invoked.
// When called within the 'callBinder' function, 'invocable' will be invoked as
// follows:
//..
//  invocable(10, 14, "p3");
//..
// Place-holders can appear anywhere in the invocation template, and in any
// order.  The same place-holder can appear multiple times.  Each instance will
// be substituted with the same value.  For example, in the following snippet
// of code, the 'callBinder' function, is invoked with a binder such
// that argument one (10) of the binder is passed as parameter two
// and argument two (14) is passed as (i.e., bound to) parameter one:
//..
//  void bindTest2(bslma_Allocator *allocator = 0) {
//      callBinder(bcef_BindUtil::bind(allocator,
//                                     &invocable,
//                                     _2, _1, someString));
//  }
//..
// When called within the 'callBinder' function, 'invocable' will be invoked as
// follows:
//..
//  invocable(14, 10, "p3");
//..
// The following snippet of code illustrates a number of ways to call
// 'bcef_BindUtil' and their respective output:
//..
//  int test1(int i, int j) {
//      return i + j;
//  }
//
//  int abs(int x) {
//      return (x > 0) ? x : -x;
//  }
//
//  void bindTest3(bslma_Allocator *allocator = 0) {
//      using namespace bdef_PlaceHolders;
//      ASSERT( 24 == bcef_BindUtil::bind(allocator, &test1, _1, _2)(10, 14) );
//      ASSERT( 24 == bcef_BindUtil::bind(allocator, &test1, _1, 14)(10) );
//      ASSERT( 24 == bcef_BindUtil::bind(allocator, &test1, 10, _1 )(14) );
//      ASSERT( 24 == bcef_BindUtil::bind(allocator, &test1, 10, 14)() );
//      ASSERT( 24 == bcef_BindUtil::bind(allocator, &test1,
//                        bcef_BindUtil::bind(allocator, &abs, _1), 14)(-10) );
//  }
//..
// The usage example below provides a more comprehensive series of calling
// sequences.
//
///Binding Data
///------------
// The main use of 'bdef_Bind' is to invoke bound objects with additional data
// that is not specified by the caller.  For that purpose, place-holders are
// key.  There are a couple of issues to understand in order to properly use
// this component.  The bound arguments must be of a value-semantic type
// (unless they are place-holders or 'bdef_Bind' objects).  They are evaluated
// at binding time once and only once and their value copied into the binder
// (using the default allocator to supply memory unless an allocator is
// specified).  A 'bdef_Bind' object always invokes its bound object with only
// the arguments listed as bound arguments, regardless of how many arguments
// are specified to the binder at invocation time.  Invocation arguments that
// are not referenced through a place-holder are simply discarded.  Invocation
// arguments that are duplicated (by using the same place-holder several times)
// are simply copied several times.  The following examples should make things
// perfectly clear.
//
///Ignoring parameters
///- - - - - - - - - -
// It is possible to pass more invocation arguments to a binder than was
// specified in the signature by the number of bound arguments.  Invocation
// arguments not referenced by any placeholder, as well as extra invocation
// arguments, will be ignored.  Note that they will nevertheless be evaluated
// even though their value will be unused.  Consider, for example, the
// following snippet of code:
//..
//  int marker = 0;
//  int singleArgumentFunction(int x) {
//      return x;
//  }
//
//  int identityFunctionWithSideEffects(int x)
//  {
//      printf("Calling 'identityFunctionWithSideEffects' with %d\n", x);
//      marker += x;
//      return x;
//  }
//
//  template <class BINDER>
//  void callBinderWithSideEffects1(BINDER const& binder)
//  {
//      ASSERT(14 == binder(identityFunctionWithSideEffects(10), 14));
//  }
//
//  void bindTest4(bslma_Allocator *allocator = 0) {
//      marker = 0;
//      callBinderWithSideEffects1(bcef_BindUtil::bind(allocator,
//                                                     &singleArgumentFunction,
//                                                     _2));
//..
// In the above snippet of code, 'singleArgumentFunction' will be called with
// only the second argument (14) specified to the binder at invocation time in
// the 'callBinderWithSideEffects1' function.  Thus the return value of the
// invocation must be 14.  The 'identityFunctionWithSideEffects(10)' will be
// evaluated, even though its return value (10) will be discarded.  We can
// check this as follows:
//..
//      LOOP_ASSERT(marker, 10 == marker);
//  }
//..
///Duplicating parameters
///- - - - - - - - - - -
// Consider another example that reuses the 'identityFunctionWithSideEffects'
// of the previous example:
//..
//  int doubleArgumentFunction(int x, int y) {
//      return x+y;
//  }
//
//  template <class BINDER>
//  void callBinderWithSideEffects2(BINDER const& binder)
//  {
//      const int RET1 = binder(10);
//      ASSERT(20 == RET1);
//      const int RET2 = binder(identityFunctionWithSideEffects(10));
//      ASSERT(20 == RET2);
//  }
//
//  void bindTest5(bslma_Allocator *allocator = 0) {
//      marker = 0;
//      callBinderWithSideEffects2(bcef_BindUtil::bind(allocator,
//                                                     &doubleArgumentFunction,
//                                                     _1, _1));
//..
// In the above snippet of code, 'doubleArgumentFunction' will be called with
// the first argument ('identityFunctionWithSideEffects(10)') specified to
// the binder, computed only once at invocation time.  We can check this as
// follows:
//..
//      LOOP_ASSERT(marker, 10 == marker);
//  }
//..
// The return value (10) will, however, be copied twice before being passed to
// the 'doubleArgumentFunction' which will return their sum (20).
//
///Bound objects
///-------------
// There are a few issues to be aware of concerning the kind of bound objects
// that can successfully be used with this component.  The issues are identical
// to 'bdef_bind', in fact the invocable object is forwarded in the same manner
// to 'bdef_bind', and so there no additional issues and we refer the reader to
// the 'bdef_bind' component for a treatment of bound objects.
//
///Binding with allocators
///-----------------------
// The copy of the bound object and arguments are created as members of the
// 'bdef_Bind' object, so no memory is allocated for those.  When the bound
// object or arguments use memory allocation, however, that memory is supplied
// by the default allocator unless 'bcef_BindUtil::bindA' is used to specify
// the allocator to be passed to the copy constructors of the bound object and
// arguments.  Note that the invocation arguments, passed to the binder at
// invocation time, are passed "as is" to the bound object, and are not copied
// if the bound object takes them by modifiable or non-modifiable reference.
//
// In order to make clear where the allocation occurs, we will wrap "p3" into a
// type that takes an allocator, e.g., a class 'MyString' (kept minimal here
// for the purpose of exposition):
//..
//  class MyString {
//      // PRIVATE INSTANCE DATA
//      bslma_Allocator *d_allocator_p;
//      char            *d_string_p;
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(MyString,
//                                         bslalg_TypeTraitUsesBslmaAllocator);
//
//      //CREATORS
//      MyString(const char *str, bslma_Allocator *allocator = 0)
//      : d_allocator_p(bslma_Default::allocator(allocator))
//      , d_string_p((char*)d_allocator_p->allocate(strlen(str)))
//      {
//          strcpy(d_string_p, str);
//      }
//
//      MyString(MyString const& rhs, bslma_Allocator *allocator = 0)
//      : d_allocator_p(bslma_Default::allocator(allocator))
//      , d_string_p((char*)d_allocator_p->allocate(strlen(rhs)))
//      {
//          strcpy(d_string_p, rhs);
//      }
//
//      ~MyString() {
//          d_allocator_p->deallocate(d_string_p);
//      }
//
//      // ACCESSORS
//      operator const char*() const { return d_string_p; }
//  };
//..
// We will also use a 'bslma_TestAllocator' to keep track of the memory
// allocated:
//..
//  void bindTest6() {
//      bslma_TestAllocator allocator;
//      MyString myString((const char*)"p3", &allocator);
//      const int NUM_ALLOCS = allocator.numAllocation();
//..
// To expose that the default allocator is not used, we will use a default
// allocator guard, which will re-route any default allocation to the
// 'defaultAllocator':
//..
//      bslma_TestAllocator defaultAllocator;
//      bslma_DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);
//      const int NUM_DEFAULT_ALLOCS = defaultAllocator.numAllocation();
//..
// We now create a binder object with allocator using 'bindA'.  If the bound
// object were an instance of a class taking an allocator, then 'allocator'
// would be passed to its copy constructor; in this case, 'allocator' will be
// ignored.  But 'allocator' *will* be used to make the copy of 'myString' held
// by the binder:
//..
//      callBinder(bcef_BindUtil::bind(&allocator,
//                                     &invocable,
//                                     _1, _2, myString));
//..
// We now check that memory was allocated from the test allocator, and none
// from the default allocator:
//..
//      ASSERT(NUM_ALLOCS != allocator.numAllocation());
//      ASSERT(NUM_DEFAULT_ALLOCS == defaultAllocator.numAllocation());
//  }
//..
//
///Usage
///-----
// What follows is a series of code snippets illustrating detailed aspects of
// typical usage of 'bcef_BindUtil'.  For these examples, we will use a typical
// pair of event and scheduler classes, where the event is defined as:
//..
//  struct MyEvent {
//      // Event data, for illustration purpose here:
//      int d_value;
//  };
//..
// and the scheduler is defined as follows:
//..
//  class MyEventScheduler {
//      // This class owns a callback function object that takes an 'int' and
//      // an instance of 'MyEvent'.  When the 'run' method is called, it
//      //invokes the callback with a series of events that it obtains using
//      //its own stream of events.
//
//      // PRIVATE INSTANCE DATA
//      bdef_Function<void (*)(int, MyEvent)>  d_callback;
//..
// For illustration purposes, we give this scheduler a dummy stream of events:
//..
//      int d_count;
//
//      // PRIVATE MANIPULATORS
//      int getNextEvent(MyEvent *eventBuffer) {
//          // Create a copy of the next event in the specified 'eventBuffer'
//          // Return 0 on success, and non-zero if no event is available.
//
//          if (--d_count) {
//              eventBuffer->d_value = d_count;
//          }
//          return d_count;
//      }
//..
// A scheduler is created with a callback function object as follows:
//..
//    public:
//      // CREATORS
//      MyEventScheduler(bdef_Function<void(*)(int, MyEvent)> const& callback)
//      : d_callback(callback)
//      {
//      }
//..
// and its main function is to invoke the callback on the series of events as
// obtained by 'getNextEvent':
//..
//      // MANIPULATORS
//      void run(int n)
//      {
//          MyEvent e;
//          d_count = n;
//          while (!getNextEvent(&e)) {
//              d_callback(0, e);
//          }
//      }
//  };
//..
///Binding to Free Functions
/// - -  - - - - - - - - - -
// We illustrate how to use a scheduler with free callback functions that have
// various signatures by passing a binder as the callback function of the
// scheduler, and how to use the binder to match the signature of the callback
// function.  Note that at the point of invocation in 'run' the binder will be
// invoked with two invocation arguments, thus we may only use place-holders
// '_1' and '_2'.  In the following snippet of code, the binder passes its
// invocation arguments straight through to the callback:
//..
//  void myCallback(int result, MyEvent const& event)
//  {
//      // Do something ...
//  }
//
//  void myMainLoop(bslma_Allocator *allocator = 0)
//  {
//      MyEventScheduler sched(bcef_BindUtil::bind(allocator,
//                                                 &myCallback, _1, _2));
//      sched.run(10);
//  }
//..
// Next we show how to bind some of the callback arguments at binding time,
// while letting the invocation arguments straight through to the callback as
// the first two arguments:
//..
//  void myCallbackWithUserArgs(int            result,
//                              MyEvent const& event,
//                              int            userArg1,
//                              double         userArg2)
//  {
//      // Do something ...
//  }
//
//  void myMainLoop2(bslma_Allocator *allocator = 0)
//  {
//      MyEventScheduler sched(bcef_BindUtil::bind(allocator,
//                                                 &myCallbackWithUserArgs,
//                                                 _1, _2, 360, 3.14));
//      sched.run(10);
//  }
//..
// In the next snippet of code, we show how to reorder the invocation arguments
// before they are passed to the callback:
//..
//  void myCallbackWithUserArgsReordered(int            result,
//                                       int            userArg1,
//                                       double         userArg2,
//                                       MyEvent const& event)
//  {
//      // Do something ...
//  }
//
//
//  void myMainLoop3(bslma_Allocator *allocator = 0)
//  {
//      MyEventScheduler sched(bcef_BindUtil::bind(allocator,
//                      &myCallbackWithUserArgsReordered, _1, 360, 3.14, _2));
//      sched.run(10);
//  }
//..
// And finally, we illustrate that the signature of the callback can be
// *smaller* than expected by the scheduler by letting the binder ignore its
// first argument:
//..
//  void myCallbackThatDiscardsResult(MyEvent const& event)
//  {
//      // Do something ...
//  }
//
//  void myMainLoop4(bslma_Allocator *allocator = 0)
//  {
//      MyEventScheduler sched(bcef_BindUtil::bind(allocator,
//                                        &myCallbackThatDiscardsResult, _2));
//      sched.run(10);
//  }
//..
///Binding to Function Objects
///- - - - - - - - - - - - - -
// In the next example, we wrap the callback function into a function object
// which is bound by value.  For brevity, we only present the basic example of
// passing the arguments straight through to the actual callback 'operator()',
// but all the variations of the previous example could be given as well.
//..
//  struct MyCallbackObject {
//      typedef void ResultType;
//      MyCallbackObject()
//      {
//      }
//      void operator() (int result, MyEvent const& event) const
//      {
//          myCallback(result, event);
//      }
//  };
//
//  void myMainLoop5(bslma_Allocator *allocator = 0)
//  {
//      MyCallbackObject obj;
//      MyEventScheduler sched(bcef_BindUtil::bind(allocator, obj, _1, _2));
//      sched.run(10);
//  }
//..
///Binding to Function Objects by Reference
/// - - - - - - - - - - - - - - - - - - - -
// The following example reuses the 'MyCallbackObject' of the previous example,
// but illustrates that it can be passed by reference as well as by value:
//..
//  void myMainLoop6(bslma_Allocator *allocator = 0)
//  {
//      MyCallbackObject obj;
//      MyEventScheduler sched(bcef_BindUtil::bind(allocator, &obj, _1, _2));
//      sched.run(10);
//  }
//..
///Binding to Member Functions
///- - - - - - - - - - - - - -
// In the next example, we show that the callback function can be a member
// function, in which case there are three, not two, bound arguments.  The
// first bound argument must be a pointer to an instance of the class owning
// the member function.
//..
//  struct MyStatefulObject {
//    // State info
//    public:
//      void callback(int, MyEvent const& event)
//      {
//          // Do something that may modify the state info...
//      }
//  };
//
//  void myMainLoop7(bslma_Allocator *allocator = 0)
//  {
//      MyStatefulObject obj;
//      MyEventScheduler sched(bcef_BindUtil::bind(allocator,
//                                &MyStatefulObject::callback, &obj, _1, _2));
//      sched.run(10);
//  }
//..
///Nesting Bindings
/// - - - - - - - -
// We now show that it is possible to provide a binder as an argument to
// 'bcef_BindUtil'.  Upon invocation, the invocation arguments are forwarded to
// the nested binder.
//..
//  MyEvent annotateEvent(int, MyEvent const& event) {
//      // Do something to 'event' ...
//      return event;
//  }
//
//  void myMainLoop8(bslma_Allocator *allocator = 0)
//  {
//      MyCallbackObject obj;
//      MyEventScheduler sched(
//              bcef_BindUtil::bind(allocator, &obj, _1,
//                    bcef_BindUtil::bind(allocator, &annotateEvent, _1, _2)));
//      sched.run(10);
//  }
//..
///Binding to a Function Object with Explicit Return Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When the return type cannot be inferred from the bound object (using
// 'FUNC::ResultType'), the binder needs an explicitly specification.  This is
// done by using the 'bcef_BindUtil::bindR' function template as exemplified
// below:
//..
//  typedef void GlobalResultType;
//  struct MyCallbackObjectWithoutResultType {
//      MyCallbackObjectWithoutResultType()
//      {
//      }
//      GlobalResultType operator() (int result, MyEvent const& event) const
//      {
//          myCallback(result, event);
//      }
//  };
//
//  void myMainLoop9(bslma_Allocator *allocator = 0)
//  {
//      MyCallbackObjectWithoutResultType obj;
//      MyEventScheduler sched(bcef_BindUtil::bindR<GlobalResultType>(
//                                                    allocator, obj, _1, _2));
//      sched.run(10);
//  }
//..
// Another situation where the return type (in fact, the whole signature)
// cannot be inferred from the bound object is the use of the free function
// with C linkage and variable number of arguments 'printf(const char*, ...)'.
// In the following code snippet, we show how the argument to the 'callBinder'
// function of section "Elementary construction and usage of 'bdef_Bind'
// objects" above can be bound to 'printf':
//..
//  void bindTest7(bslma_Allocator *allocator = 0)
//  {
//      const char* formatString = "Here it is: %d %d\n";
//      callBinder(bcef_BindUtil::bindR<int>(allocator,
//                                           &printf, formatString, _1, _2));
//  }
//..
// When called, 'bindTest7' will create a binder, pass it to 'callBinder' which
// will invoke it with arguments '10' and '14', and the output will be:
//..
//  Here it is: 10 14
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_BIND
#include <bdef_bind.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

namespace BloombergLP {

                        // ======================
                        // class bcef_BindWrapper
                        // ======================

template <class RET, class FUNC, class TUPLE>
class bcef_BindWrapper {
    // This wrapper class provides a binder with shared pointer semantics to a
    // non-modifiable binder (both 'operator*' and 'operator->' accessors) and
    // forward its construction arguments to the underlying binder.  Note that
    // instances of this class should not be created explicitly, instead use
    // the 'bcef_BindUtil' factory methods.  Note also that instances of this
    // class can be invoked directly, in order to meet user expectations that
    // the result of 'bcef_BindUtil::bind' be an invocable just as the result
    // of 'bdef_BindUtil::bind' is; in that case, the invocation parameters are
    // simply forwarded to the shared binder.  Note finally that even though
    // this wrapper has pointer semantics for the 'operator*' and 'operator->',
    // it has value semantics for the invocation: invoking a non-modifiable
    // wrapper will forward the invocation to the non-modifiable binder shared
    // by that wrapper, even though pointer semantics have it .

    // DATA
    bcema_SharedPtr<const bdef_Bind<RET,FUNC,TUPLE> > d_impl;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcef_BindWrapper,
                                 bslalg_TypeTraitHasPointerSemantics);

    // PUBLIC TYPES
    typedef typename bdef_Bind<RET,FUNC,TUPLE>::ResultType ResultType;
        // The return type of this binder object.

    //CREATORS
    bcef_BindWrapper(typename bslmf_ForwardingType<FUNC>::Type  func,
                     const TUPLE&                               tuple,
                     bslma_Allocator                           *allocator = 0);
        // Create a wrapper with shared pointer semantics around a binder
        // constructed with the specified 'func' invocable and specified
        // 'tuple' bound arguments.

    bcef_BindWrapper(const bcef_BindWrapper<RET,FUNC,TUPLE>& original);
        // Create a wrapper that shares ownership of the binder of the
        // specified 'original'.

    //ACCESSORS
    template <class ARGS>
    inline ResultType invoke(ARGS& arguments) const
        // Invoke the bound object using the invocation parameters provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for their respective values in the specified 'arguments'.
    {
        return d_impl->invoke(arguments);
    }

    const bdef_Bind<RET,FUNC,TUPLE>& operator*() const;
        // Return a reference to the non-modifiable binder shared by this
        // wrapper.

    const bdef_Bind<RET,FUNC,TUPLE>* operator->() const;
        // Return the address of the non-modifiable binder shared by this
        // wrapper.

    inline ResultType operator()() const
        // Invoke the bound object using only the invocation parameters
        // provided at construction of this 'bdef_Bind' object and return
        // the result.
    {
        return (*d_impl)();
    }

    template <class P1>
    inline ResultType operator()(P1& p1)  const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for argument 1 with the value of the argument 'p1'.  Return the
        // result.
    {
        return (*d_impl)(p1);
    }

    template <class P1>
    inline ResultType operator()(P1 const& p1)  const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for argument 1 with the value of the argument 'p1'.  Return the
        // result.
    {
        return (*d_impl)(p1);
    }

    template <class P1, class P2>
    inline ResultType operator()(P1& p1, P2& p2) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 and 2 with the value of the arguments 'p1', and 'p2'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2);
    }

    template <class P1, class P2>
    inline ResultType operator()(P1 const& p1, P2 const& p2) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 and 2 with the value of the arguments 'p1', and 'p2'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2);
    }

    template <class P1, class P2, class P3>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1, 2, and 3 with the values of the arguments 'p1',
        // 'p2' and 'p3' respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3);
    }

    template <class P1, class P2, class P3>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3)
    const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1, 2, and 3 with the values of the arguments 'p1',
        // 'p2' and 'p3' respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3);
    }

    template <class P1, class P2, class P3, class P4>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 4 with the values of the arguments 'p1' - 'p4'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4);
    }

    template <class P1, class P2, class P3, class P4>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 4 with the values of the arguments 'p1' - 'p4'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4);
    }

    template <class P1, class P2, class P3, class P4, class P5>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 5 with the values of the arguments 'p1' - 'p5'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5);
    }

    template <class P1, class P2, class P3, class P4, class P5>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 5 with the values of the arguments 'p1' - 'p5'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                                 P6& p6) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 6 with the values of the arguments 'p1' - 'p7'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5, P6 const& p6)
    const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 6 with the values of the arguments 'p1' - 'p7'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                                 P6& p6, P7& p7) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 7 with the values of the arguments 'p1' - 'p7'
        // respectively.  Return the result.

    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5, P6 const& p6,
                                 P7 const& p7) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 7 with the values of the arguments 'p1' - 'p7'
        // respectively.  Return the result.

    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                                 P6& p6, P7& p7, P8& p8) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 8 with the values of the arguments 'p1' - 'p8'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5, P6 const& p6,
                                 P7 const& p7, P8 const& p8) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 8 with the values of the arguments 'p1' - 'p8'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                                 P6& p6, P7& p7, P8& p8, P9& p9) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 9 with the values of the arguments 'p1' - 'p9'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5, P6 const& p6,
                                 P7 const& p7, P8 const& p8, P9 const& p9)
    const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 9 with the values of the arguments 'p1' - 'p9'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                                 P6& p6, P7& p7, P8& p8, P9& p9,
                                 P10& p10) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 10 with the values of the arguments 'p1' - 'p10'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5, P6 const& p6,
                                 P7 const& p7, P8 const& p8, P9 const& p9,
                                 P10 const& p10) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 10 with the values of the arguments 'p1' - 'p10'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                                 P6& p6, P7& p7, P8& p8, P9& p9, P10& p10,
                                 P11& p11) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 11 with the values of the arguments 'p1' - 'p11'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5, P6 const& p6,
                                 P7 const& p7, P8 const& p8, P9 const& p9,
                                 P10 const& p10, P11 const& p11) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 11 with the values of the arguments 'p1' - 'p11'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                                 P6& p6, P7& p7, P8& p8, P9& p9, P10& p10,
                                 P11& p11, P12& p12) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 12 with the values of the arguments 'p1' - 'p12'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5, P6 const& p6,
                                 P7 const& p7, P8 const& p8, P9 const& p9,
                                 P10 const& p10, P11 const& p11,
                                 P12 const& p12) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 12 with the values of the arguments 'p1' - 'p12'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p12);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12,
              class P13>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                                 P6& p6, P7& p7, P8& p8, P9& p9, P10& p10,
                                 P11& p11, P12& p12, P13& p13) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 13 with the values of the arguments 'p1' - 'p13'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,
                         p13);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12,
              class P13>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5, P6 const& p6,
                                 P7 const& p7, P8 const& p8, P9 const& p9,
                                 P10 const& p10, P11 const& p11,
                                 P12 const& p12, P13 const& p13) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 13 with the values of the arguments 'p1' - 'p13'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,
                         p13);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12,
              class P13, class P14>
    inline ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                                 P6& p6, P7& p7, P8& p8, P9& p9, P10& p10,
                                 P11& p11, P12& p12, P13& p13, P14& p14) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 14 with the values of the arguments 'p1' - 'p14'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,
                         p13, p14);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12,
              class P13, class P14>
    inline ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5, P6 const& p6,
                                 P7 const& p7, P8 const& p8, P9 const& p9,
                                 P10 const& p10, P11 const& p11,
                                 P12 const& p12, P13 const& p13,
                                 P14 const& p14) const
        // Invoke the bound object using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 14 with the values of the arguments 'p1' - 'p14'
        // respectively.  Return the result.
    {
        return (*d_impl)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12,
                         p13, p14);
    }
};

                        // ===================
                        // class bcef_BindUtil
                        // ===================

struct bcef_BindUtil {
    // This 'struct' provides a namespace for utility functions used to
    // construct 'bdef_Bind' objects and return references with shared
    // ownership to them.  Two variations are provided; the 'bind' and the
    // 'bindR' variations.  Both variations accept an invocable object,
    // optionally followed by up to fourteen additional arguments.  Each
    // argument can be either a literal value, a place holder, or another
    // 'bdef_Bind' object.  The 'bindR' variation must be used when binding to
    // object for which a result type cannot be automatically determined.
    //
    //  See the 'bdef_bind' component documentation for an introduction to
    //  binders and to their creation using their factory methods.

    // CLASS METHODS
    template <class FUNC>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple0 >
    bind(bslma_Allocator *allocator, FUNC func)
        // Return a 'bdef_Bind' object that is bound to the specified
        // 'func' invocable object which can be invoked with no parameters.
    {
        return bcef_BindWrapper<bslmf_Nil, FUNC, bdef_Bind_BoundTuple0>
                   (func, bdef_Bind_BoundTuple0(),allocator);
    }

    template <class FUNC, class P1>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple1<P1> >
    bind(bslma_Allocator *allocator, FUNC func,P1 const&p1)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with one parameters.
    {
        return bcef_BindWrapper<bslmf_Nil, FUNC, bdef_Bind_BoundTuple1<P1> >
                   (func, bdef_Bind_BoundTuple1<P1>(p1,allocator), allocator);
    }

    template <class FUNC, class P1, class P2>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple2<P1,P2> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const &p1, P2 const &p2)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with two parameters.
    {
        typedef bdef_Bind_BoundTuple2<P1,P2> ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple3<P1,P2,P3> >
    bind(bslma_Allocator *allocator, FUNC  func, P1 const&p1, P2 const&p2,
            P3 const&p3)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with three
        // parameters.
    {
        typedef bdef_Bind_BoundTuple3<P1,P2,P3> ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3,allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple4<P1,P2,P3,P4> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
            P3 const&p3, P4 const&p4)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with four parameters.
    {
        typedef bdef_Bind_BoundTuple4<P1,P2,P3,P4> ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple5<P1,P2,P3,P4,P5> >
    bind(bslma_Allocator* allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with five parameters.
    {
        typedef bdef_Bind_BoundTuple5<P1,P2,P3,P4,P5> ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4,p5,allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with six parameters.
    {
        typedef bdef_Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4,p5,p6,allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7>
    static inline bcef_BindWrapper<
                                  bslmf_Nil,
                                  FUNC,
                                  bdef_Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,P7> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with seven
        // parameters.
    {
        typedef bdef_Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,P7> ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4,p5,p6,p7,allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,
                                                         P8> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eight
        // parameters.
    {
        typedef bdef_Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,P8> ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
                (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,
                                                         P8,P9> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4,P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with nine parameters.
    {
        typedef bdef_Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,P8,P9> ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
             (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple10<P1,P2,P3,P4,P5,P6,P7,
                                                            P8,P9,P10> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9, P10 const&p10)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with ten parameters.
    {
        typedef bdef_Bind_BoundTuple10<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10>
                                                                      ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,allocator),
             allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple11<P1,P2,P3,P4,P5,P6,P7,
                                                            P8,P9,P10,P11> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eleven
        // parameters.
    {
        typedef bdef_Bind_BoundTuple11<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11>
            ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,allocator),
             allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple12<P1,P2,P3,P4,P5,P6,P7,
                                              P8,P9,P10,P11,P12> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11, P12 const&p12)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with twelve
        // parameters.
    {
        typedef bdef_Bind_BoundTuple12<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12>
            ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,allocator),
             allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12, class P13>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple13<P1,P2,P3,P4,P5,P6,P7,
                                              P8,P9,P10,P11,P12,P13> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11, P12 const&p12,
         P13 const&p13)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with thirteen
        // parameters.
    {
        typedef bdef_Bind_BoundTuple13<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,
                                       P13> ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,
                            allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12, class P13, class P14>
    static inline bcef_BindWrapper<bslmf_Nil, FUNC,
                                   bdef_Bind_BoundTuple14<P1,P2,P3,P4,P5,P6,P7,
                                              P8,P9,P10,P11,P12,P13,P14> >
    bind(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11, P12 const&p12,
         P13 const&p13, P14 const&p14)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with fourteen
        // parameters.
    {
        typedef bdef_Bind_BoundTuple14<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,
                                       P13,P14> ListType;
        return bcef_BindWrapper<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,
                            allocator), allocator);
    }

    template <class RET, class FUNC>
    static inline bcef_BindWrapper<RET, FUNC, bdef_Bind_BoundTuple0 >
    bindR(bslma_Allocator *allocator, FUNC func)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with no parameters
        // and returns a value of type 'RET'.
    {
        return bcef_BindWrapper<RET, FUNC, bdef_Bind_BoundTuple0>
                   (func, bdef_Bind_BoundTuple0(), allocator);
    }

    template <class RET, class FUNC, class P1>
    static inline bcef_BindWrapper<RET, FUNC, bdef_Bind_BoundTuple1<P1> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with one parameter
        // and returns a value of type 'RET'.
    {
        return bcef_BindWrapper<RET, FUNC, bdef_Bind_BoundTuple1<P1> >
                   (func, bdef_Bind_BoundTuple1<P1>(p1, allocator), allocator);
    }

    template <class RET, class FUNC, class P1, class P2>
    static inline bcef_BindWrapper<RET, FUNC, bdef_Bind_BoundTuple2<P1,P2> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with two parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple2<P1,P2> ListType;
        return bcef_BindWrapper<RET,FUNC,ListType>
                   (func, ListType(p1,p2,allocator), allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3>
    static inline bcef_BindWrapper<RET, FUNC, bdef_Bind_BoundTuple3<P1,P2,P3> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with three parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple3<P1,P2,P3> ListType;
        return bcef_BindWrapper<RET,FUNC,ListType>
                   (func, ListType(p1,p2,p3,allocator), allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple4<P1,P2,P3,P4> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with four parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple4<P1,P2,P3,P4> ListType;
        return bcef_BindWrapper<RET,FUNC,ListType>
                   (func, ListType(p1,p2,p3,p4,allocator), allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple5<P1,P2,P3,P4,P5> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with five parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple5<P1,P2,P3,P4,P5> ListType;
        return bcef_BindWrapper<RET,FUNC,ListType>
                   (func, ListType(p1,p2,p3,p4,p5,allocator), allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with six parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> ListType;
        return bcef_BindWrapper<RET, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4,p5,p6,allocator), allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,
                                                   P7> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with seven parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,P7> ListType;
        return bcef_BindWrapper<RET, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4,p5,p6,p7,allocator), allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,
                                                         P8> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eight parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,P8> ListType;
        return bcef_BindWrapper<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,allocator), allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,
                                                         P8,P9> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with nine parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,P8,P9> ListType;
        return bcef_BindWrapper<RET, FUNC, ListType>
             (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,allocator), allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple10<P1,P2,P3,P4,P5,P6,P7,
                                                          P8,P9,P10> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with ten parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple10<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10>
                                                                      ListType;
        return bcef_BindWrapper<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,allocator),
             allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple11<P1,P2,P3,P4,P5,P6,P7,
                                                          P8,P9,P10,P11> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eleven
        // parameters and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple11<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11>
            ListType;
        return bcef_BindWrapper<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,allocator),
             allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11, class P12>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple12<P1,P2,P3,P4,P5,P6,P7,
                                                          P8,P9,P10,P11,P12> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11,
          P12 const&p12)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with twelve
        // parameters and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple12<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12>
            ListType;
        return bcef_BindWrapper<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,allocator),
             allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11, class P12, class P13>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple13<P1,P2,P3,P4,P5,P6,P7,
                                                       P8,P9,P10,P11,P12,P13> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11,
          P12 const&p12, P13 const&p13)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with thirteen
        // parameters and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple13<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,
                                       P13> ListType;
        return bcef_BindWrapper<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,
                            allocator), allocator);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11, class P12, class P13, class P14>
    static inline bcef_BindWrapper<RET, FUNC,
                                   bdef_Bind_BoundTuple14<P1,P2,P3,P4,P5,P6,P7,
                                                   P8,P9,P10,P11,P12,P13,P14> >
    bindR(bslma_Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11,
          P12 const&p12, P13 const&p13, P14 const&p14)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with fourteen
        // parameters and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple14<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,
                                       P13,P14> ListType;
        return bcef_BindWrapper<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,
                            allocator), allocator);
    }

};

// ---- Anything below this line is implementation specific.  Do not use.  ----

                        // ============================
                        // class bcef_Bind_ArgumentMask
                        // ============================

template <class RET, class FUNC, class LIST>
struct bdef_Bind_ArgumentMask<bcef_BindWrapper<RET, FUNC, LIST> > {
    // This specialization of 'bdef_Bind_ArgumentMask' defines a mask for a
    // 'bcef_BindWrapper' object passed recursively as a bound argument.  The
    // value is not important, as long as it is out of range.  This makes sure
    // that a binder with a nested 'bcef_BindWrapper' object is treated as
    // non-explicit, in the same way as if the the nested binder was of type
    // 'bdef_Bind'.

    enum {
        VALUE = 1 << 24
    };
};

                        // =========================
                        // class bcef_Bind_Evaluator
                        // =========================

template <class RET, class FUNC, class BINDLIST, class LIST>
struct bdef_Bind_Evaluator<bcef_BindWrapper<RET,FUNC,BINDLIST>, LIST> {
    // This utility provides an evaluator for nested 'bcef_BindWrapper'
    // arguments.  It is a specialization of the 'bdef_Bind_Evaluator' declared
    // in the 'bdef_bind' component to enable nested 'bcef_BindWrapper' objects
    // in the same fashion as nested 'bdef_Bind' objects.
    // The underlying 'bdef_Bind' function object is invoked using the provided
    // argument list and the result is returned.

    static inline typename bdef_Bind<RET,FUNC,BINDLIST>::ResultType
    eval(LIST& args, const bcef_BindWrapper<RET,FUNC,BINDLIST>& func)
        // Call the specified 'func' functor with the specified 'args'
        // arguments and return the result.
    {
        return func.invoke(args);
    }
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

template <class RET, class FUNC, class TUPLE>
inline
bcef_BindWrapper<RET,FUNC,TUPLE>::bcef_BindWrapper(
    typename BloombergLP::bslmf_ForwardingType<FUNC>::Type  func,
    TUPLE const&                                            tuple,
    bslma_Allocator                                        *allocator)
{
    this->d_impl.createInplace(allocator, func, tuple, allocator);
}

template <class RET, class FUNC, class TUPLE>
inline
bcef_BindWrapper<RET,FUNC,TUPLE>::bcef_BindWrapper(
    bcef_BindWrapper<RET,FUNC,TUPLE> const&original)
: d_impl(original.d_impl)
{
}

template <class RET, class FUNC, class TUPLE>
inline
const bdef_Bind<RET,FUNC,TUPLE>&
bcef_BindWrapper<RET,FUNC,TUPLE>::operator*() const
{
    return *(this->d_impl);
}

template <class RET, class FUNC, class TUPLE>
inline
const bdef_Bind<RET,FUNC,TUPLE>*
bcef_BindWrapper<RET,FUNC,TUPLE>::operator->() const
{
    return this->d_impl.ptr();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
