// bdlcf_bindutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLCF_BINDUTIL
#define INCLUDED_BDLCF_BINDUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a signature-specific function object (functor).
//
//@CLASSES:
//   bdlcf::BindWrapper: Reference counted function binder
//      bdlcf::BindUtil: Utility for constructing 'bdlcf::BindWrapper' objects
//
//@AUTHOR: Ilougino Rocha (irocha), Herve Bronnimann (hbronnimann)
//
//@DESCRIPTION: This component provides several factory methods for creating a
// parameterized binder mechanism ('bcef::Bind') similar to those of the
// component 'bdlf_bind'.  The difference between a binder created by this
// component rather than by the factory methods in 'bdlf_bind' is that the
// binder is returned by reference rather than by value, with shared ownership
// semantics.  Hence its main use is for creating binders that hold a
// non-trivial amount of storage (i.e., the bound arguments) and will be
// copied, possibly several times, such as jobs enqueued in a threadpool.
//
///Migrating from 'bdlf::BindUtil::bind...' to bdlcf::BindUtil::bind...'
///------------------------------------------------------------------
// A call to 'bdlf::BindUtil::bind' or 'bindR' can easily be replaced to another
// call to 'bdlcf::BindUtil::bind' or 'bindR', with almost no change in syntax.
// A common mistake however is to forget the allocator passed as an extra
// *first* *argument*.  A call to 'bdlf::BindUtil::bindA', however, needs no
// change of syntax since the binder's allocator is already present as first
// argument.
//
// For further information about binder mechanisms, usage examples, etc., see
// the 'bdlf_bind' component.  For completeness and compatibility with this
// component's test driver, we translate the documentation in terms of
// 'bdlcf_bindutil' (that is, replacing calls to 'bdlf::BindUtil::bind' by
// 'bdlcf::BindUtil::bind', and adding the allocator argument where appropriate).
//
///Elementary construction and usage of 'bdlcf::BindWrapper' objects
///---------------------------------------------------------------
// Bound objects are generally constructed by invoking the 'bdlcf::BindUtil' with
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
//  void bindTest(bslma::Allocator *allocator = 0) {
//      bdlcf::BindUtil::bind(allocator,                 // allocator,
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
//  void bindTest1(bslma::Allocator *allocator = 0) {
//      callBinder(bdlcf::BindUtil::bind(allocator,
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
//  void bindTest2(bslma::Allocator *allocator = 0) {
//      callBinder(bdlcf::BindUtil::bind(allocator,
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
// 'bdlcf::BindUtil' and their respective output:
//..
//  int test1(int i, int j) {
//      return i + j;
//  }
//
//  int abs(int x) {
//      return (x > 0) ? x : -x;
//  }
//
//  void bindTest3(bslma::Allocator *allocator = 0) {
//      using namespace bdlf::PlaceHolders;
//      ASSERT( 24 == bdlcf::BindUtil::bind(allocator, &test1, _1, _2)(10, 14) );
//      ASSERT( 24 == bdlcf::BindUtil::bind(allocator, &test1, _1, 14)(10) );
//      ASSERT( 24 == bdlcf::BindUtil::bind(allocator, &test1, 10, _1 )(14) );
//      ASSERT( 24 == bdlcf::BindUtil::bind(allocator, &test1, 10, 14)() );
//      ASSERT( 24 == bdlcf::BindUtil::bind(allocator, &test1,
//                        bdlcf::BindUtil::bind(allocator, &abs, _1), 14)(-10) );
//  }
//..
// The usage example below provides a more comprehensive series of calling
// sequences.
//
///Binding Data
///------------
// The main use of 'bdlf::Bind' is to invoke bound objects with additional data
// that is not specified by the caller.  For that purpose, place-holders are
// key.  There are a couple of issues to understand in order to properly use
// this component.  The bound arguments must be of a value-semantic type
// (unless they are place-holders or 'bdlf::Bind' objects).  They are evaluated
// at binding time once and only once and their value copied into the binder
// (using the default allocator to supply memory unless an allocator is
// specified).  A 'bdlf::Bind' object always invokes its bound object with only
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
//  void bindTest4(bslma::Allocator *allocator = 0) {
//      marker = 0;
//      callBinderWithSideEffects1(bdlcf::BindUtil::bind(allocator,
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
//  void bindTest5(bslma::Allocator *allocator = 0) {
//      marker = 0;
//      callBinderWithSideEffects2(bdlcf::BindUtil::bind(allocator,
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
// to 'bdlf_bind', in fact the invocable object is forwarded in the same manner
// to 'bdlf_bind', and so there no additional issues and we refer the reader to
// the 'bdlf_bind' component for a treatment of bound objects.
//
///Binding with allocators
///-----------------------
// The copy of the bound object and arguments are created as members of the
// 'bdlf::Bind' object, so no memory is allocated for those.  When the bound
// object or arguments use memory allocation, however, that memory is supplied
// by the default allocator unless 'bdlcf::BindUtil::bindA' is used to specify
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
//      bslma::Allocator *d_allocator_p;
//      char             *d_string_p;
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(MyString,
//                                   bslalg::TypeTraitUsesBslmaAllocator);
//
//      //CREATORS
//      MyString(const char *str, bslma::Allocator *allocator = 0)
//      : d_allocator_p(bslma::Default::allocator(allocator))
//      , d_string_p((char*)d_allocator_p->allocate(strlen(str)))
//      {
//          strcpy(d_string_p, str);
//      }
//
//      MyString(MyString const& rhs, bslma::Allocator *allocator = 0)
//      : d_allocator_p(bslma::Default::allocator(allocator))
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
// We will also use a 'bslma::TestAllocator' to keep track of the memory
// allocated:
//..
//  void bindTest6() {
//      bslma::TestAllocator allocator;
//      MyString myString((const char*)"p3", &allocator);
//      const int NUM_ALLOCS = allocator.numAllocations();
//..
// To expose that the default allocator is not used, we will use a default
// allocator guard, which will re-route any default allocation to the
// 'defaultAllocator':
//..
//      bslma::TestAllocator defaultAllocator;
//      bslma::DefaultAllocatorGuard defaultAllocatorGuard(&defaultAllocator);
//      const int NUM_DEFAULT_ALLOCS = defaultAllocator.numAllocations();
//..
// We now create a binder object with allocator using 'bindA'.  If the bound
// object were an instance of a class taking an allocator, then 'allocator'
// would be passed to its copy constructor; in this case, 'allocator' will be
// ignored.  But 'allocator' *will* be used to make the copy of 'myString' held
// by the binder:
//..
//      callBinder(bdlcf::BindUtil::bind(&allocator,
//                                     &invocable,
//                                     _1, _2, myString));
//..
// We now check that memory was allocated from the test allocator, and none
// from the default allocator:
//..
//      ASSERT(NUM_ALLOCS != allocator.numAllocations());
//      ASSERT(NUM_DEFAULT_ALLOCS == defaultAllocator.numAllocations());
//  }
//..
//
///Usage
///-----
// What follows is a series of code snippets illustrating detailed aspects of
// typical usage of 'bdlcf::BindUtil'.  For these examples, we will use a typical
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
//      bdlf::Function<void (*)(int, MyEvent)>  d_callback;
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
//      MyEventScheduler(bdlf::Function<void(*)(int, MyEvent)> const& callback)
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
//  void myMainLoop(bslma::Allocator *allocator = 0)
//  {
//      MyEventScheduler sched(bdlcf::BindUtil::bind(allocator,
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
//  void myMainLoop2(bslma::Allocator *allocator = 0)
//  {
//      MyEventScheduler sched(bdlcf::BindUtil::bind(allocator,
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
//  void myMainLoop3(bslma::Allocator *allocator = 0)
//  {
//      MyEventScheduler sched(bdlcf::BindUtil::bind(allocator,
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
//  void myMainLoop4(bslma::Allocator *allocator = 0)
//  {
//      MyEventScheduler sched(bdlcf::BindUtil::bind(allocator,
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
//  void myMainLoop5(bslma::Allocator *allocator = 0)
//  {
//      MyCallbackObject obj;
//      MyEventScheduler sched(bdlcf::BindUtil::bind(allocator, obj, _1, _2));
//      sched.run(10);
//  }
//..
///Binding to Function Objects by Reference
/// - - - - - - - - - - - - - - - - - - - -
// The following example reuses the 'MyCallbackObject' of the previous example,
// but illustrates that it can be passed by reference as well as by value:
//..
//  void myMainLoop6(bslma::Allocator *allocator = 0)
//  {
//      MyCallbackObject obj;
//      MyEventScheduler sched(bdlcf::BindUtil::bind(allocator, &obj, _1, _2));
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
//  void myMainLoop7(bslma::Allocator *allocator = 0)
//  {
//      MyStatefulObject obj;
//      MyEventScheduler sched(bdlcf::BindUtil::bind(allocator,
//                                &MyStatefulObject::callback, &obj, _1, _2));
//      sched.run(10);
//  }
//..
///Nesting Bindings
/// - - - - - - - -
// We now show that it is possible to provide a binder as an argument to
// 'bdlcf::BindUtil'.  Upon invocation, the invocation arguments are forwarded to
// the nested binder.
//..
//  MyEvent annotateEvent(int, MyEvent const& event) {
//      // Do something to 'event' ...
//      return event;
//  }
//
//  void myMainLoop8(bslma::Allocator *allocator = 0)
//  {
//      MyCallbackObject obj;
//      MyEventScheduler sched(
//              bdlcf::BindUtil::bind(allocator, &obj, _1,
//                    bdlcf::BindUtil::bind(allocator, &annotateEvent, _1, _2)));
//      sched.run(10);
//  }
//..
///Binding to a Function Object with Explicit Return Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When the return type cannot be inferred from the bound object (using
// 'FUNC::ResultType'), the binder needs an explicitly specification.  This is
// done by using the 'bdlcf::BindUtil::bindR' function template as exemplified
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
//  void myMainLoop9(bslma::Allocator *allocator = 0)
//  {
//      MyCallbackObjectWithoutResultType obj;
//      MyEventScheduler sched(bdlcf::BindUtil::bindR<GlobalResultType>(
//                                                    allocator, obj, _1, _2));
//      sched.run(10);
//  }
//..
// Another situation where the return type (in fact, the whole signature)
// cannot be inferred from the bound object is the use of the free function
// with C linkage and variable number of arguments 'printf(const char*, ...)'.
// In the following code snippet, we show how the argument to the 'callBinder'
// function of section "Elementary construction and usage of 'bdlf::Bind'
// objects" above can be bound to 'printf':
//..
//  void bindTest7(bslma::Allocator *allocator = 0)
//  {
//      const char* formatString = "Here it is: %d %d\n";
//      callBinder(bdlcf::BindUtil::bindR<int>(allocator,
//                                           &printf, formatString, _1, _2));
//  }
//..
// When called, 'bindTest7' will create a binder, pass it to 'callBinder' which
// will invoke it with arguments '10' and '14', and the output will be:
//..
//  Here it is: 10 14
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLF_BIND
#include <bdlf_bind.h>
#endif

namespace BloombergLP {

namespace bdlf {
    struct BindUtil;
}

namespace bdlcf {

typedef bdlf::BindUtil BindUtil;

#if 0
                        // ===================
                        // class BindUtil
                        // ===================

struct BindUtil {
    // This 'struct' provides a namespace for utility functions used to
    // construct 'bdlf::Bind' objects and return references with shared
    // ownership to them.  Two variations are provided; the 'bind' and the
    // 'bindR' variations.  Both variations accept an invocable object,
    // optionally followed by up to fourteen additional arguments.  Each
    // argument can be either a literal value, a place holder, or another
    // 'bdlf::Bind' object.  The 'bindR' variation must be used when binding to
    // object for which a result type cannot be automatically determined.
    //
    //  See the 'bdlf_bind' component documentation for an introduction to
    //  binders and to their creation using their factory methods.

    // CLASS METHODS
    template <class FUNC>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple0 >
    bind(bslma::Allocator *allocator, FUNC func)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // 'func' invocable object which can be invoked with no parameters.
    {
        return bdlf::BindUtil::bind(allocator, func);
    }

    template <class FUNC, class P1>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple1<P1> >
    bind(bslma::Allocator *allocator, FUNC func,P1 const&p1)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with one parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1);
    }

    template <class FUNC, class P1, class P2>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple2<P1,P2> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const &p1, P2 const &p2)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with two parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2);
    }

    template <class FUNC, class P1, class P2, class P3>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple3<P1,P2,P3> >
    bind(bslma::Allocator *allocator, FUNC  func, P1 const&p1, P2 const&p2,
            P3 const&p3)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with three
        // parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3);
    }

    template <class FUNC, class P1, class P2, class P3, class P4>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple4<P1,P2,P3,P4> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
            P3 const&p3, P4 const&p4)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with four parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple5<P1,P2,P3,P4,P5> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with five parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with six parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5,
                                     p6);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7>
    static inline bdlf::BindWrapper<
                                  bslmf::Nil,
                                  FUNC,
                                  bdlf::Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,P7> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with seven
        // parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5,
                                     p6, p7);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,
                                                         P8> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eight
        // parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5,
                                     p6, p7, p8);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,
                                                         P8,P9> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4,P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with nine parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5,
                                     p6, p7, p8, p9);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple10<P1,P2,P3,P4,P5,P6,P7,
                                                            P8,P9,P10> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9, P10 const&p10)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with ten parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5,
                                     p6, p7, p8, p9, p10);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple11<P1,P2,P3,P4,P5,P6,P7,
                                                            P8,P9,P10,P11> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eleven
        // parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5,
                                     p6, p7, p8, p9, p10, p11);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple12<P1,P2,P3,P4,P5,P6,P7,
                                              P8,P9,P10,P11,P12> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11, P12 const&p12)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with twelve
        // parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5,
                                     p6, p7, p8, p9, p10, p11, p12);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12, class P13>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple13<P1,P2,P3,P4,P5,P6,P7,
                                              P8,P9,P10,P11,P12,P13> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11, P12 const&p12,
         P13 const&p13)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with thirteen
        // parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5,
                                     p6, p7, p8, p9, p10, p11, p12, p13);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12, class P13, class P14>
    static inline bdlf::BindWrapper<bslmf::Nil, FUNC,
                                   bdlf::Bind_BoundTuple14<P1,P2,P3,P4,P5,P6,P7,
                                              P8,P9,P10,P11,P12,P13,P14> >
    bind(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
         P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
         P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11, P12 const&p12,
         P13 const&p13, P14 const&p14)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with fourteen
        // parameters.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5,
                                     p6, p7, p8, p9, p10, p11, p12, p13, p14);
    }

    template <class RET, class FUNC>
    static inline bdlf::BindWrapper<RET, FUNC, bdlf::Bind_BoundTuple0 >
    bindR(bslma::Allocator *allocator, FUNC func)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with no parameters
        // and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func);
    }

    template <class RET, class FUNC, class P1>
    static inline bdlf::BindWrapper<RET, FUNC, bdlf::Bind_BoundTuple1<P1> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with one parameter
        // and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1);
    }

    template <class RET, class FUNC, class P1, class P2>
    static inline bdlf::BindWrapper<RET, FUNC, bdlf::Bind_BoundTuple2<P1,P2> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with two parameters
        // and returns a value of type 'RET'.
    {
        typedef bdlf::Bind_BoundTuple2<P1,P2> ListType;
        return bdlf::BindWrapper<RET,FUNC,ListType>
                   (func, ListType(p1,p2,allocator), allocator);
        // TBD:
//         return bdlf::BindUtil::bind(allocator, func, p1, p2);
    }

    template <class RET, class FUNC, class P1, class P2, class P3>
    static inline bdlf::BindWrapper<RET, FUNC, bdlf::Bind_BoundTuple3<P1,P2,P3> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with three parameters
        // and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple4<P1,P2,P3,P4> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with four parameters
        // and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple5<P1,P2,P3,P4,P5> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with five parameters
        // and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with six parameters
        // and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5, p6);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,
                                                   P7> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with seven parameters
        // and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5, p6,
                                      p7);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,
                                                         P8> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eight parameters
        // and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5, p6,
                                      p7, p8);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,
                                                         P8,P9> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with nine parameters
        // and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5, p6,
                                      p7, p8, p9);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple10<P1,P2,P3,P4,P5,P6,P7,
                                                          P8,P9,P10> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with ten parameters
        // and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5, p6,
                                      p7, p8, p9, p10);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple11<P1,P2,P3,P4,P5,P6,P7,
                                                          P8,P9,P10,P11> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eleven
        // parameters and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5, p6,
                                      p7, p8, p9, p10, p11);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11, class P12>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple12<P1,P2,P3,P4,P5,P6,P7,
                                                          P8,P9,P10,P11,P12> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11,
          P12 const&p12)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with twelve
        // parameters and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5, p6,
                                      p7, p8, p9, p10, p11, p12);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11, class P12, class P13>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple13<P1,P2,P3,P4,P5,P6,P7,
                                                       P8,P9,P10,P11,P12,P13> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11,
          P12 const&p12, P13 const&p13)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with thirteen
        // parameters and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5, p6,
                                      p7, p8, p9, p10, p11, p12, p13);
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11, class P12, class P13, class P14>
    static inline bdlf::BindWrapper<RET, FUNC,
                                   bdlf::Bind_BoundTuple14<P1,P2,P3,P4,P5,P6,P7,
                                                   P8,P9,P10,P11,P12,P13,P14> >
    bindR(bslma::Allocator *allocator, FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11,
          P12 const&p12, P13 const&p13, P14 const&p14)
        // Return a 'bdlf::Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with fourteen
        // parameters and returns a value of type 'RET'.
    {
        return bdlf::BindUtil::bind(allocator, func, p1, p2, p3, p4, p5, p6,
                                      p7, p8, p9, p10, p11, p12, p13, p14);
    }

};

#endif

}  // close package namespace

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
