// bdef_bind.h                                                        -*-C++-*-
#ifndef INCLUDED_BDEF_BIND
#define INCLUDED_BDEF_BIND

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a signature-specific function object (functor).
//
//@CLASSES:
//      bdef_Bind: select the proper implementation for the binder
//  bdef_BindUtil: factory methods for creating the binder
//
//@AUTHOR: Ilougino Rocha (irocha), Herve Bronnimann (hbronnimann)
//
//@SEE_ALSO: bdef_memfn, bdef_function, bdef_placeholder
//
//@DESCRIPTION: This component provides a parameterized binder mechanism,
// 'bdef_Bind', that is a functor object that binds an invokable object or
// function to a list of arguments.  This component also defines factory
// methods in the 'bdef_BindUtil' namespace for creating 'bdef_Bind' objects
// (e.g., 'bind', 'bindA', and 'bindR').  The 'bdef_Bind' functor (called
// henceforth a "binder") is an object that can hold any invocable object (the
// "bound functor") and a number of parameters (the "bound arguments", some of
// which can be place-holders of type 'bdef_PlaceHolder').  When the binder is
// later invoked (with optional additional arguments called the "invocation
// arguments" used to compute the value of bound arguments that use
// place-holders), it returns the result of calling the bound functor with the
// bound arguments and invocation arguments.  The section "Supported
// functionality and limitations" details which kind of bound functors and
// bound arguments can be used in conjunction with this component.
//
// The main use of 'bdef_Bind' is to invoke bound functors with additional data
// that is not available prior to the point the functor is invoked (as is often
// the case with callback functions).  The binding is accomplished by passing
// place-holders instead of literal values as the bound arguments.  When the
// binder is invoked, these place-holders are replaced by their respective
// invocation arguments.  The section "Elementary construction and usage of
// 'bdef_Bind' objects" shows an elementary (but not realistic) usage of the
// 'bdef_BindUtil::bind' factory methods with free functions, that should be
// enough for most users to grasp the basic usage of this component.  The
// section "Binding data" offers more details and should enable a user to make
// a more advanced usage of this component.  The usage example presents many
// uses of 'bdef_Bind' and 'bdef_BindUtil' in a somewhat realistic (but a bit
// more involved) situation.
//
// Note that 'bdef_Bind' functors are typically used with standard algorithms,
// or with 'bdef_Function'.  This mechanism is similar to, but much more
// powerful than 'bsl::binder1st' or 'bsl::binder2nd'.
//
///Supported functionality
///-----------------------
// An invocable object is any object that can be invoked in syntactically the
// same manner as a function.  Invocable objects include function pointers, and
// objects that provide an 'operator()' method.  This component supports bound
// objects that can be function pointers, member function pointers (the first
// bound argument must evaluate to an instance of the class of which the
// function is a member), or function objects passed by address or by value.
// In addition, there is a limitation on the number of parameters that such an
// objet can take (currently no more than 14).
//
// A 'bdef_Bind' functor can be constructed, usually by one of the
// 'bdef_BindUtil' factory methods, from a bound functor and from 0 up to 14
// bound arguments that can be either literal values, place-holders of type
// 'bdef_PlaceHolder', or further 'bdef_Bind' functors.  The type of a binder
// object is a complicated expression, which is why a binder is typically a
// short-lived object that is returned or passed as parameter to a function
// template.  That is also why the 'bdef_BindUtil' factory methods are the
// preferred way to create a binder.  When a binder is later invoked with some
// arguments, literal values are passed to the bound functor and place-holders
// are substituted by their respective invocation arguments.  In addition, any
// argument that is a binder itself is invoked recursively with the same
// invocation arguments and the result of that invocation is passed to the
// parent binder.  The section "Elementary construction and usage of
// 'bdef_Bind' objects" below details the whole mechanism and offers some
// examples.
//
// The 'bdef_Bind' functors support 'bdema_allocator*' arguments and, when
// constructed with an allocator via the 'bdef_BindUtil::bindA' factory
// methods, allocate all the memory for holding copies of the bound functor and
// arguments through that allocator.  See the section "Binding with allocators"
// below for a more detailed discussion.
//
///Elementary construction and usage of 'bdef_Bind' objects
///--------------------------------------------------------
// Bound functors are generally constructed by invoking the 'bdef_BindUtil'
// with an "invocation template".  An invocation template is a series of one or
// more arguments that describe how to invoke the bound functor.  Each argument
// can be either a place-holder or a literal value.  Literal values are stored
// by value in the binder and directly forwarded to the bound functor when
// invoked.  Place-holders are substituted with the respective argument
// provided at invocation of the binder.  For example, given the following
// 'invocable' (here a free function for simplicity):
//..
//  void invocable(int i, int j, const char *str)
//  {
//      // Do something with 'i', 'j' and 'str'.
//      printf("Invoked with: %d %d %s\n", i, j, str);
//  }
//..
// and the following (global) string:
//..
//  const char *someString = "p3"; // for third parameter to 'invocable'
//..
// we can bind the parameters of 'invocable' to the following arguments:
//..
//  void bindTest() {
//      bdef_BindUtil::bind(&invocable,         // bound functor and
//                          10, 14, someString) // bound arguments
//..
// and the binder declared above can be passed invocation arguments directly,
// as follows (here we specify zero invocation arguments since all the bound
// arguments are fully specified):
//..
//                                          (); // invocation
//  }
//..
// In the function call above, the 'invocable' will be bound with the
// arguments '10', '14', and '"p3"' respectively, then invoked with those bound
// arguments.  In the next example, place-holders are used to forward
// user-provided arguments to the bound functor.  We separate the invocation of
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
//  void bindTest1() {
//      callBinder(bdef_BindUtil::bind(&invocable,
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
//  void bindTest2() {
//      callBinder(bdef_BindUtil::bind(&invocable, _2, _1, someString));
//  }
//..
// When called within the 'callBinder' function, 'invocable' will be invoked as
// follows:
//..
//  invocable(14, 10, "p3");
//..
// The following snippet of code illustrates a number of ways to call
// 'bdef_BindUtil' and their respective output:
//..
//  int test1(int i, int j)
//  {
//      return i + j;
//  }
//
//  int abs(int x)
//  {
//      return (x > 0) ? x : -x;
//  }
//
//  void bindTest3()
//  {
//      using namespace bdef_PlaceHolders;
//      assert(24 == bdef_BindUtil::bind(&test1, _1, _2)(10, 14));
//      assert(24 == bdef_BindUtil::bind(&test1, _1, 14)(10));
//      assert(24 == bdef_BindUtil::bind(&test1, 10, _1)(14));
//      assert(24 == bdef_BindUtil::bind(&test1, 10, 14)());
//      assert(24 == bdef_BindUtil::bind(&test1,
//                                     bdef_BindUtil::bind(&abs,_1), 14)(-10));
//  }
//..
// The usage example below provides a more comprehensive series of calling
// sequences.
//
///Binding Data
///------------
// The main use of 'bdef_Bind' is to invoke bound functors with additional data
// that is not available prior to the point the functor is invoked (as is often
// the case with callback functions).  For that purpose, place-holders are key.
// There are a couple of issues to understand in order to properly use this
// component.  The bound arguments must be of a value-semantic type (unless
// they are place-holders or 'bdef_Bind' objects).  They are evaluated at
// binding time once and only once and their value copied into the binder
// (using the default allocator to supply memory unless an allocator is
// specified).  A 'bdef_Bind' object always invokes its bound functor with only
// the arguments listed as bound arguments, regardless of how many arguments
// are specified to the binder at invocation time.  Invocation arguments that
// are not referenced through a place-holder are simply discarded.  Invocation
// arguments that are duplicated (by using the same place-holder several times)
// are simply copied several times.  The following examples should make things
// perfectly clear.
//
///Ignoring parameters
///- - - - - - - - - -
// It is possible to pass more invocation arguments to a binder than were
// specified in the signature by the number of bound arguments.  Invocation
// arguments not referenced by any placeholder, as well as extra invocation
// arguments, will be ignored.  Note that they will nevertheless be evaluated
// even though their value will be unused.  Consider, for example, the
// following snippet of code:
//..
//  int singleArgumentFunction(int x) {
//      bsl::cout << "singleArgumentFunction(" << x << ")" << bsl::endl;
//      return x;
//  }
//
//  void bindTest4() {
//      int ret = bdef_BindUtil::bind(&singleArgumentFunction, _2)(
//                                                 singleArgumentFunction(5),
//                                                 singleArgumentFunction(10),
//                                                 singleArgumentFunction(15));
//      assert(10 == marker);
//  }
//..
// In the above snippet of code, 'singleArgumentFunction' will be invoked 4
// times, even though the results of the invocation with '5' and '15' are
// ignored (as the place holder specifies that only the second invocation
// argument is used).  The above invocation will produce the following on
// 'stdout':
//..
//  singleArgumentFunction(5)
//  singleArgumentFunction(10)
//  singleArgumentFunction(15)
//  singleArgumentFunction(10)
//..
// Note that "singleArgumentFunction" is printed twice - once for evaluating
// the value used to pass to the bound functor, and once for invoking the
// bound functor itself.
//
///Duplicating parameters
///- - - - - - - - - - -
// A place-holder can be specified multiple times, effectively passing the same
// value to different arguments of the function.  The value will be evaluated
// only once.  To illustrate this, consider another example that reuses the
// 'singleArgumentFunction' of the previous example:
//..
//  int doubleArgumentFunction(int x, int y) {
//      return x+y;
//  }
//
//  void bindTest5() {
//      marker = 0;
//      int ret = bdef_BindUtil::bind(&doubleArgumentFunction, _1, _1)(
//                                                  singleArgumentFunction(5));
//      assert(10 == marker);
//  }
//..
// In the above snippet of code, 'doubleArgumentFunction' will be called with
// the result of 'singleArgumentFunction'.  The value, however, is evaluated
// only once.  The above invocation will produce the following single line
// output on 'stdout':
//..
//  singleArgumentFunction(5)
//..
// The return value of 'doubleArgumentFunction' is 10, as 5 is passed to both
// 'x' and 'y'.
//
///Bound functors
///--------------
// There are a few issues to be aware of concerning the kind of bound functors
// that can successfully be used with this component.
//
///Binding to member functions
///- - - - - - - - - - - - - -
// Although member function pointers are not invoked in syntactically the same
// manner as free functions, they can still be used directly in conjunction
// with 'bdef_Bind'.  When the binder detects that a member function pointer
// was specified, it automatically wraps it in a 'bdef_MemFn' object.  In this
// case a pointer to the object must be passed as the first argument to bind,
// followed by the remaining arguments.  See the usage example "Binding to
// Member Functions" below.
//
///Binding to functions with an ellipsis
///- - - - - - - - - - - - - - - - - - -
// It is possible to create a binder with a *free* function (pointer or
// reference) that takes an ellipsis (e.g., 'int printf(const char*, ...').
// This component does *not* support ellipsis in *member* function pointers,
// however.  See the 'bindTest7' example function at the end of the usage
// example below.
//
///Binding to function objects by value or by address
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Although function objects are invoked in syntactically the same manner as
// free functions, they can be used by value or by address in conjunction with
// 'bdef_Bind'.  When the binder detects that a pointer to a function object
// was specified, it automatically dereferences that pointer prior to invoking
// the function object.  The difference between the two usages is that the
// binder object holds a copy of the whole object or of its address only.  In
// particular, when passing by value an object that takes an allocator, the
// copy held by the binder uses the same allocator as the binder object (if
// constructed by 'bdef_BindUtil::bindA') or the default allocator (if
// constructed by 'bdef_BindUtil::bind' or 'bdef_BindUtil::bindR'), but *not*
// the allocator of the original object.  For keeping the same allocator, pass
// the object by address to the binder instead.  See the section "Binding with
// allocators" and the usage example sections "Binding to Function Objects" and
// "Binding to Function Objects by Reference" below.
//
// CAVEAT:  When passing a function object by value, only the (non-modifiable)
// copy held by the binder will be invoked.  Prior to this version, it was
// possible to modifiably invoke this copy (hence a non-const 'operator()')
// with the intent to modify the state of the function object.  However, only
// the copy held by the binder was modified and the original function object
// passed to the binder was not, but this usage error went undetected.  In this
// version, a binder cannot modifiably invoke functors held by value, and
// attempting to do so will trigger a compilation error.  If it is desired that
// an invocation modifies the state of the function object, then this function
// object must be passed to the binder *by* *address*.
//
///Inferring the signature of the bound functor
/// - - - - - - - - - - - - - - - - - - - - - -
// A 'bdef_Bind' object will strive to properly and automatically resolve the
// signature of its bound functor between different overloads of that
// invocable.  The signature of the bound functor is inferred from that of the
// bound functor and the type of the arguments either at binding or invocation
// time.  The signature of invocables that are not function objects (i.e., free
// functions with C++ linkage and member functions) must be determined at
// binding time (in particular, overloads must be disambiguated when obtaining
// the address of the function).  In those cases, the bound arguments will be
// of the corresponding type and any values passed as bound arguments (except
// placeholders) will be cast to the corresponding type at binding time and
// stored by value unless the argument type is a reference type.
//
// Invocation arguments will be cast in place of their corresponding
// place-holder(s) to the corresponding type only at invocation time.  If the
// signature of the bound functor is known, the invocation argument will be
// forwarded using the most efficient type (in particular, unnecessary copies
// will be avoided for non fundamental types).
//
// Some invocable objects, however, may not allow the binder to detect their
// signature until invocation.  This is the case for function objects, for free
// functions with C linkage (e.g., 'printf'), if a bound argument is a nested
// binder, or if a placeholder is used in two positions in the bound arguments.
// In that case, the bound arguments are stored in their own types, and cast to
// the corresponding argument type of the signature only when the signature is
// determined at invocation time.  Place-holders, likewise, are not typed and
// will acquire the type of their corresponding invocation argument when
// invoked, which will be cast to the corresponding argument type of the
// signature.  In particular, the same binder constructed with a functor and
// place-holders in place of the bound arguments can invoke several overloads
// of the 'operator()' of the functor, depending on the type of the invocation
// arguments.
//
// Although function objects are invoked in syntactically the same manner as
// free functions, their return type cannot always be inferred.  The same
// limitation applies to free functions with 'extern "C"' linkage.  In that
// case, the return type has to be given explicitly to the binder.  This can be
// done by using the 'bdef_BindUtil::bindR' function.  Note that all
// 'bdef_Function' objects have a standard public type 'ResultType' to assist
// the deduction of return type and can be used with 'bdef_BindUtil::bind'.
// See the usage example "Binding to a Function Object with Explicit Return
// Type" below.
//
///Binding with constants and temporaries
/// - - - - - - - - - - - - - - - - - - -
// Due to a technical restriction of the C++ language known as the "forwarding
// problem", it is not possible to match the signature of a function object
// exactly when passing a mix of non-'const' lvalues and rvalues as invocation
// arguments.  Nevertheless, this component supports passing literal values and
// temporaries as invocation arguments to a 'bdef_Bind' object.  There is
// however one limitation: if *any* of the arguments in the signature of the
// bound functor should be of a modifiable reference type, then *all* the
// invocation arguments need to be modifiable references.  That is, it is not
// possible to pass a literal (const) value to some argument of a bound functor
// when another argument expects a modifiable reference.  Note that a direct
// call to the bound functor (without the binder) would accept such an
// argument.  This is not a severe limitation, and the workaround is to pass
// instead a local modifiable variable initialized to the literal value.
//
///Binding with allocators
///-----------------------
// The bound functor and bound arguments are created as members of the
// 'bdef_Bind' object, so no memory is allocated for storing them.  However, if
// the bound functor or bound argument's copy constructor requires memory
// allocation, that memory is supplied by the default allocator unless
// 'bdef_BindUtil::bindA' is used to specify the allocator to be passed to the
// copy constructors of the bound functor and arguments.  Note that the
// invocation arguments, passed to the binder at invocation time, are passed
// "as is" to the bound functor, and are not copied if the bound functor takes
// them by modifiable or non-modifiable reference.
//
// In order to make clear where the allocation occurs, we will wrap "p3" (the
// third parameter for the function object 'invocable' into a type that takes
// an allocator, e.g., a class 'MyString' (kept minimal here for the purpose of
// exposition), and automatically converts to 'const char *':
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
//      , d_string_p((char*)d_allocator_p->allocate(1 + strlen(str)))
//      {
//          strcpy(d_string_p, str);
//      }
//
//      MyString(MyString const& rhs, bslma_Allocator *allocator = 0)
//      : d_allocator_p(bslma_Default::allocator(allocator))
//      , d_string_p((char*)d_allocator_p->allocate(1 + strlen(rhs)))
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
//      assert(0 == defaultAllocator.numAllocation());
//..
// We now create a binder object with allocator using 'bindA'.  'allocator'
// *will* be used to make the copy of 'myString' held by the binder:
//..
//      callBinder(bdef_BindUtil::bindA(&allocator, &invocable,
//                                      _1, _2, myString));
//..
// We now check that memory was allocated from the test allocator, and none
// from the default allocator:
//..
//      assert(NUM_ALLOCS < allocator.numAllocation());
//      assert(0 == defaultAllocator.numAllocation());
//  }
//..
//
///Usage example
///-------------
// What follows is a series of code snippets illustrating detailed aspects of
// typical usage of 'bdef_Bind' and 'bdef_BindUtil'.  For these examples, we
// will use a typical pair of event and event dispatcher classes, where the
// event is defined as:
//..
//  struct MyEvent {
//      // Event data, for illustration purpose here:
//      int d_value;
//  };
//..
// and the event dispatcher is defined as follows:
//..
//  class MyEventDispatcher {
//      // This class owns a callback function object that takes an 'int' and
//      // an instance of 'MyEvent'.  When the 'dispatch' method is called, it
//      // invokes the callback with a series of events that it obtains using
//      // its own stream of events.
//
//      // PRIVATE INSTANCE DATA
//      bdef_Function<void (*)(int, MyEvent)>  d_callback;
//
//      // PRIVATE MANIPULATORS
//      int getNextEvent(MyEvent *eventBuffer) {
//          // Create a copy of the next event in the specified 'eventBuffer'
//          // Return 0 on success, and non-zero if no event is available.
//
//          // Implementation elided
//          // ...
//      }
//..
// A dispatcher is created with a callback function object as follows:
//..
//    public:
//      // CREATORS
//      MyEventDispatcher(bdef_Function<void(*)(int, MyEvent)> const& callback)
//      : d_callback(callback)
//      {
//      }
//..
// and its main function is to invoke the callback on the series of events as
// obtained by 'getNextEvent':
//..
//      // MANIPULATORS
//      void dispatch(int id)
//      {
//          MyEvent e;
//          while (!getNextEvent(&e)) {
//              d_callback(id, e);
//          }
//      }
//  };
//..
//
///Binding to Free Functions
/// - -  - - - - - - - - - -
// We illustrate how to use the dispatcher with free callback functions that
// have various signatures by passing a binder as the callback function of the
// dispatcher, and how to use the binder to match the signature of the callback
// function.  Note that at the point of invocation in 'dispatch' the binder
// will be invoked with two invocation arguments, thus we may use only
// place-holders '_1' and '_2'.  In the following snippet of code, the binder
// passes its invocation arguments straight through to the callback:
//..
//  void myCallback(int id, MyEvent const& event)
//  {
//      // Do something ...
//  }
//
//  void myMainLoop()
//  {
//      MyEventDispatcher ed(bdef_BindUtil::bind(&myCallback, _1, _2));
//      ed.dispatch(10);
//  }
//..
// Next we show how to bind some of the callback arguments at binding time,
// while letting the invocation arguments straight through to the callback as
// the first two arguments:
//..
//  void myCallbackWithUserArgs(int            id,
//                              MyEvent const& event,
//                              int            userArg1,
//                              double         userArg2)
//  {
//      // Do something ...
//  }
//
//  void myMainLoop2()
//  {
//      MyEventDispatcher ed(bdef_BindUtil::bind(&myCallbackWithUserArgs,
//                                                        _1, _2, 360, 3.14));
//      ed.dispatch(10);
//  }
//..
// In the next snippet of code, we show how to reorder the invocation arguments
// before they are passed to the callback:
//..
//  void myCallbackWithUserArgsReordered(int            id,
//                                       int            userArg1,
//                                       double         userArg2,
//                                       MyEvent const& event)
//  {
//      // Do something ...
//  }
//
//  void myMainLoop3()
//  {
//      MyEventDispatcher ed(bdef_BindUtil::bind(
//                      &myCallbackWithUserArgsReordered, _1, 360, 3.14, _2));
//      ed.dispatch(10);
//  }
//..
// And finally, we illustrate that the signature of the callback can be
// *smaller* than expected by the dispatcher by letting the binder ignore its
// first argument:
//..
//  void myCallbackThatDiscardsResult(MyEvent const& event)
//  {
//      // Do something ...
//  }
//
//  void myMainLoop4()
//  {
//      MyEventDispatcher ed(bdef_BindUtil::bind(
//                                        &myCallbackThatDiscardsResult, _2));
//      ed.dispatch(10);
//  }
//..
//
///Binding to Function Objects
///- - - - - - - - - - - - - -
// In the next example, we wrap the callback function into a function object
// which is bound by value.  For brevity, we only present the basic example of
// passing the arguments straight through to the actual callback 'operator()',
// but all the variations of the previous example could be given as well.
//..
//  struct MyCallbackObject {
//      typedef void ResultType;
//      void operator() (int id, MyEvent const& event)
//      {
//          myCallback(id, event);
//      }
//  };
//
//  void myMainLoop5()
//  {
//      MyCallbackObject obj;
//      MyEventDispatcher ed(bdef_BindUtil::bind(obj, _1, _2));
//      ed.dispatch(10);
//  }
//..
//
///Binding to Function Objects by Reference
/// - - - - - - - - - - - - - - - - - - - -
// The following example reuses the 'MyCallbackObject' of the previous example,
// but illustrates that it can be passed by reference as well as by value:
//..
//  void myMainLoop6()
//  {
//      MyCallbackObject obj;
//      MyEventDispatcher ed(bdef_BindUtil::bind(&obj, _1, _2));
//      ed.dispatch(10);
//  }
//..
// When passed by reference, only the address of the function object is copied.
// Hence the function object must remain valid throughout the lifetime of the
// binder.
//
///Binding to Member Functions
///- - - - - - - - - - - - - -
// In the next example, we show that the callback function can be a member
// function, in which case there are three, not two, bound arguments.  The
// first bound argument must be a pointer to an instance of the class owning
// the member function.
//..
//  struct MyStatefulObject {
//
//      // DATA
//      // ...
//
//    public:
//      void callback(int, MyEvent const& event)
//      {
//          // Do something that may modify the state info...
//      }
//  };
//
//  void myMainLoop7()
//  {
//      MyStatefulObject obj;
//      MyEventDispatcher ed(bdef_BindUtil::bind(
//                                 &MyStatefulObject::callback, &obj, _1, _2));
//      ed.dispatch(10);
//  }
//..
//
///Nesting Bindings
/// - - - - - - - -
// We now show that it is possible to provide a binder as an argument to
// 'bdef_BindUtil'.  Upon invocation, the invocation arguments are forwarded to
// the nested binder.
//..
//  MyEvent annotateEvent(int, MyEvent const& event) {
//      // Do something to 'event' ...
//      return event;
//  }
//
//  void myMainLoop8()
//  {
//      MyCallbackObject obj;
//      MyEventDispatcher ed(
//              bdef_BindUtil::bind(&obj, _1,
//                              bdef_BindUtil::bind(&annotateEvent, _1, _2)));
//      ed.dispatch(10);
//  }
//..
///Binding to a Function Object with Explicit Return Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When the return type cannot be inferred from the bound functor (using
// 'FUNC::ResultType'), the binder needs an explicitly specification.  This is
// done by using the 'bdef_BindUtil::bindR' function template as exemplified
// below:
//..
//  typedef void GlobalResultType;
//  struct MyCallbackObjectWithoutResultType {
//      GlobalResultType operator() (int id, MyEvent const& event)
//      {
//          myCallback(id, event);
//      }
//  };
//
//  void myMainLoop9()
//  {
//      MyCallbackObjectWithoutResultType obj;
//      MyEventDispatcher ed(bdef_BindUtil::
//                                      bindR<GlobalResultType>(obj, _1, _2));
//      ed.dispatch(10);
//  }
//..
// Another situation where the return type (in fact, the whole signature)
// cannot be inferred from the bound functor is the use of the free function
// with C linkage and variable number of arguments 'printf(const char*, ...)'.
// In the following code snippet, we show how the argument to the 'callBinder'
// function (redefined below for the reader's convenience) of section
// "Elementary construction and usage of 'bdef_Bind' objects" above can be
// bound to 'printf':
//..
//  template <class BINDER>
//  void callBinder(BINDER const& b)
//  {
//      b(10, 14);
//  }
//
//  void bindTest7()
//  {
//      const char *formatString = "Here it is: %d %d\n";
//      callBinder(bdef_BindUtil::bindR<int>(&printf, formatString, _1, _2));
//  }
//..
// When called, 'bindTest7' will create a binder, pass it to 'callBinder' which
// will invoke it with arguments '10' and '14', and the output will be:
//..
//  Here it is: 10 14
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEF_MEMFN
#include <bdef_memfn.h>
#endif

#ifndef INCLUDED_BDEF_PLACEHOLDER
#include <bdef_placeholder.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS
#include <bslalg_typetraithaspointersemantics.h>
#endif

#ifndef INCLUDED_BSLMF_MATCHANYTYPE
#include <bslmf_matchanytype.h>
#endif

#ifndef INCLUDED_BSLMF_ARRAYTOPOINTER
#include <bslmf_arraytopointer.h>
#endif

#ifndef INCLUDED_BSLMF_FORWARDINGTYPE
#include <bslmf_forwardingtype.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_IF
#include <bslmf_if.h>
#endif

#ifndef INCLUDED_BSLMF_MEMBERFUNCTIONPOINTERTRAITS
#include <bslmf_memberfunctionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLMF_TYPELIST
#include <bslmf_typelist.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

template <class RET, class FUNC> struct bdef_Bind_FuncTraits;
template <class RET, int NUMARGS> struct bdef_Bind_Invoker;
template <class BINDER, class ARGS> struct bdef_Bind_Evaluator;

    struct bdef_Bind_BoundTuple0;
template <class A1>
    struct bdef_Bind_BoundTuple1;
template <class A1, class A2>
    struct bdef_Bind_BoundTuple2;
template <class A1, class A2, class A3>
    struct bdef_Bind_BoundTuple3;
template <class A1, class A2, class A3, class A4>
    struct bdef_Bind_BoundTuple4;
template <class A1, class A2, class A3, class A4, class A5>
    struct bdef_Bind_BoundTuple5;
template <class A1, class A2, class A3, class A4, class A5, class A6>
    struct bdef_Bind_BoundTuple6;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    struct bdef_Bind_BoundTuple7;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
    struct bdef_Bind_BoundTuple8;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
    struct bdef_Bind_BoundTuple9;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
    struct bdef_Bind_BoundTuple10;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11>
    struct bdef_Bind_BoundTuple11;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12>
    struct bdef_Bind_BoundTuple12;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13>
    struct bdef_Bind_BoundTuple13;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14>
    struct bdef_Bind_BoundTuple14;

    struct bdef_Bind_Tuple0;
template <class A1>
    struct bdef_Bind_Tuple1;
template <class A1, class A2>
    struct bdef_Bind_Tuple2;
template <class A1, class A2, class A3>
    struct bdef_Bind_Tuple3;
template <class A1, class A2, class A3, class A4>
    struct bdef_Bind_Tuple4;
template <class A1, class A2, class A3, class A4, class A5>
    struct bdef_Bind_Tuple5;
template <class A1, class A2, class A3, class A4, class A5, class A6>
    struct bdef_Bind_Tuple6;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    struct bdef_Bind_Tuple7;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
    struct bdef_Bind_Tuple8;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
    struct bdef_Bind_Tuple9;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
    struct bdef_Bind_Tuple10;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11>
    struct bdef_Bind_Tuple11;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12>
    struct bdef_Bind_Tuple12;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13>
    struct bdef_Bind_Tuple13;
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14>
    struct bdef_Bind_Tuple14;

template <class LIST> struct bdef_Bind_CalcParameterMask;
template <class FUNC, class ARGS, int INDEX, int OFFSET>
                                             struct bdef_Bind_MapParameter;
template <class RET, class FUNC, class LIST> struct bdef_Bind_ImplSelector;

                          // ===============================
                          // class bdef_Bind_BoundTupleValue
                          // ===============================

// IMPLEMENTATION NOTE: This class template, as well as the
// 'bdef_bind_BoundTuple[0-14]' class templates, are always instantiated with
// template argument 'TYPE'.  'TYPE' is one of the 'A[0-14]' template
// parameters for 'bdef_bind_BoundTuple[0-14]'.  Since 'TYPE' is *not* a
// reference or const types, it is always appropriate to take any value of
// these types by 'const&' to avoid unnecessary copies until the only one and
// final copy is done in the constructor proxy.

template <class TYPE>
class bdef_Bind_BoundTupleValue {
    // This local class provides storage for a value of the specified 'TYPE'
    // suitable for storing an argument value in one of the 'bdef_Bind_Tuple*'
    // local classes.  This general template definition ensures that the
    // allocator passed to the creators is passed through to the value if it
    // uses an allocator, using the 'bslalg_ConstructorProxy' mechanism.

    // PRIVATE TYPES
    typedef typename bslmf_ArrayToConstPointer<TYPE>::Type STORAGE_TYPE;

    // PRIVATE INSTANCE DATA
    bslalg_ConstructorProxy<STORAGE_TYPE> d_value;

  public:
    // CREATORS
    bdef_Bind_BoundTupleValue(
                        const bdef_Bind_BoundTupleValue<TYPE>&  original,
                        bslma_Allocator                        *basicAllocator)
        // Create a 'bdef_Bind_BoundTupleValue' object holding a copy of the
        // specified 'original' value, using 'basicAllocator' to supply any
        // memory.
    : d_value(original.d_value, basicAllocator)
    {
    }

    bdef_Bind_BoundTupleValue(const TYPE&      value,
                              bslma_Allocator *basicAllocator)
        // Create a 'bdef_Bind_BoundTupleValue' object holding a copy of the
        // specified 'value', using 'basicAllocator' to supply any memory.
    : d_value(value, basicAllocator)
    {
    }

    template <class BDE_OTHER_TYPE>
    bdef_Bind_BoundTupleValue(const BDE_OTHER_TYPE&  value,
                              bslma_Allocator       *basicAllocator)
        // Create a 'bdef_Bind_BoundTupleValue' object holding a copy of the
        // specified 'value', using 'basicAllocator' to supply any memory.
    : d_value(value, basicAllocator)
    {
    }

    // MANIPULATORS
    STORAGE_TYPE& value() { return d_value.object(); }
        // Return a reference to the modifiable object held by this proxy.

    // ACCESSORS
    const STORAGE_TYPE& value() const { return d_value.object(); }
        // Return a reference to the non-modifiable object held by this proxy.
};

                           // ===========================
                           // class bdef_Bind_BoundTuple*
                           // ===========================

struct bdef_Bind_BoundTuple0 : public bslmf_TypeList0 {
    // This 'struct' provides the creators for a list of zero arguments.

    // CREATORS
    bdef_Bind_BoundTuple0()
    {
    }

    bdef_Bind_BoundTuple0(const bdef_Bind_BoundTuple0&  original,
                          bslma_Allocator              *allocator = 0)
    {
        (void) original;
        (void) allocator;
    }
};

template <class A1>
struct bdef_Bind_BoundTuple1 : public bslmf_TypeList1<A1>
{
    // This 'struct' stores a list of one argument.  It does *not* use the
    // const-forwarding type of its argument, unlike 'bdef_Bind_Tuple1'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1> d_a1;

    // CREATORS
    bdef_Bind_BoundTuple1(const bdef_Bind_BoundTuple1<A1>& orig,
                          bslma_Allocator                *allocator = 0)
    : d_a1(orig.d_a1, allocator)
    {
    }

    bdef_Bind_BoundTuple1(A1 const& a1, bslma_Allocator *allocator = 0)
    : d_a1(a1, allocator)
    {
    }
};

                           // ===============
                           // class bdef_Bind
                           // ===============

template <class RET, class FUNC, class LIST>
class bdef_Bind : public bdef_Bind_ImplSelector<RET,
                                                FUNC,
                                                LIST>::Type {
    // This bind class select the implementation for the given template
    // arguments.  Note that instances of this class should not be
    // created explicitly, instead use the 'bdef_BindUtil' factory methods.

    // PRIVATE TYPES
    typedef typename bdef_Bind_ImplSelector<RET, FUNC, LIST>::Type Base;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdef_Bind,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    bdef_Bind(typename bslmf_ForwardingType<FUNC>::Type  func,
              LIST const&                                list,
              bslma_Allocator                           *allocator = 0)
        // Create a 'bdef_Bind' object that is bound to the specified 'func'
        // invocable object, optionally using the 'allocator' to supply memory.
    : Base(func, list, allocator)
    {
    }

    bdef_Bind(const bdef_Bind& other, bslma_Allocator *allocator = 0)
        // Create a 'bdef_Bind' object that is bound to the same invocable
        // object with the same bound parameters as 'other', optionally using
        // the 'allocator' to supply memory.
    : Base(other, allocator)
    {
    }
};

                           // ===================
                           // class bdef_BindUtil
                           // ===================

struct bdef_BindUtil {
    // This 'struct' provides a namespace for utility functions used to
    // construct 'bdef_Bind' objects.  Three families of factory methods are
    // provided: 'bind', 'bindA' and 'bindR'.  All factory methods accept an
    // invocable object, optionally followed by up to fourteen additional
    // arguments.  Each argument can be either a literal value, a place-holder,
    // or another 'bdef_Bind' object.
    //
    // The 'bdef_BindUtil' class methods compute the signature of the function
    // object automatically, including the return type, and return a binder
    // object with the specified bound functor and bound arguments.  Memory for
    // copying the bound functor and bound arguments is supplied by the user
    // specified allocator if 'bindA' is used, or the currently installed
    // default allocator.  The return type is inferred by using
    // 'bslmf_FunctionPointerTraits' for free functions references and
    // pointers, 'bslmf_MemberFunctionPointerTraits' for member function
    // pointers, and 'FUNC::ResultType' for a functor of type 'FUNC'.
    //
    // The 'bindA' variations are used when a specified allocator must be used
    // to supply memory for copying the bound functor and bound arguments.  The
    // 'bindR' variation must be used when binding to an object for which a
    // result type cannot be automatically determined.

    // CLASS METHODS
    template <class FUNC>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple0>
    bind(FUNC func)
        // Return a 'bdef_Bind' object that is bound to the specified
        // 'func' invocable object, which can be invoked with no parameters.
    {
        return bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple0>
                   (func, bdef_Bind_BoundTuple0());
    }

    template <class FUNC, class P1>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple1<P1> >
    bind(FUNC func, P1 const&p1)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with one parameter.
    {
        return bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple1<P1> >
                   (func, bdef_Bind_BoundTuple1<P1>(p1));
    }

    template <class FUNC, class P1, class P2>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple2<P1, P2> >
    bind(FUNC func, P1 const&p1, P2 const&p2)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with two parameters.
    {
        typedef bdef_Bind_BoundTuple2<P1, P2> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1, p2));
    }

    template <class FUNC, class P1, class P2, class P3>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple3<P1,P2,P3> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with three
        // parameters.
    {
        typedef bdef_Bind_BoundTuple3<P1,P2,P3> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3));
    }

    template <class FUNC, class P1, class P2, class P3, class P4>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple4<P1,P2,P3,P4> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with four parameters.
    {
        typedef bdef_Bind_BoundTuple4<P1,P2,P3,P4> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4));
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple5<P1,P2,P3,P4,P5> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
         P5 const&p5)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with five parameters.
    {
        typedef bdef_Bind_BoundTuple5<P1,P2,P3,P4,P5> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4,p5));
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
         P5 const&p5, P6 const&p6)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with six parameters.
    {
        typedef bdef_Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4,p5,p6));
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,P7> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
         P5 const&p5, P6 const&p6, P7 const&p7)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with seven parameters.
    {
        typedef bdef_Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,P7> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4,p5,p6,p7));
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,P8> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
         P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eight
        // parameters.
    {
        typedef bdef_Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,P8> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8));
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9>
    static
    bdef_Bind<bslmf_Nil, FUNC,
              bdef_Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,P8,P9> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
         P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with nine parameters.
    {
        typedef bdef_Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,P8,P9> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9));
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple10<
                                              P1,P2,P3,P4,P5,P6,P7,P8,P9,P10> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
         P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9,
         P10 const&p10)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with ten parameters.
    {
        typedef bdef_Bind_BoundTuple10<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10>
            ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10));
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple11<
                                          P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
         P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9,
         P10 const&p10, P11 const&p11)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eleven
        // parameters.
    {
        typedef bdef_Bind_BoundTuple11<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11>
            ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11));
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple12<
                                      P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
         P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9,
         P10 const&p10, P11 const&p11, P12 const&p12)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with twelve
        // parameters.
    {
        typedef bdef_Bind_BoundTuple12<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12>
            ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12));
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12, class P13>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple13<
                                  P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
         P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9,
         P10 const&p10, P11 const&p11, P12 const&p12, P13 const&p13)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with thirteen
        // parameters.
    {
        typedef bdef_Bind_BoundTuple13<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,
                                       P13> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13));
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12, class P13, class P14>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple14<P1,P2,P3,P4,P5,P6,P7,
                                              P8,P9,P10,P11,P12,P13,P14> >
    bind(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
         P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9,
         P10 const&p10, P11 const&p11, P12 const&p12, P13 const&p13,
         P14 const&p14)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with fourteen
        // parameters.
    {
        typedef bdef_Bind_BoundTuple14<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,
                                       P13,P14> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14));
    }

    template <class FUNC>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple0>
    bindA(bslma_Allocator *allocator, FUNC func)
        // Return a 'bdef_Bind' object that is bound to the specified
        // 'func' invocable object, which can be invoked with no parameters.
    {
        return bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple0>
                   (func, bdef_Bind_BoundTuple0(), allocator);
    }

    template <class FUNC, class P1>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple1<P1> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with one parameter.
    {
        return bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple1<P1> >
            (func, bdef_Bind_BoundTuple1<P1>(p1, allocator), allocator);
    }

    template <class FUNC, class P1, class P2>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple2<P1, P2> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with two parameters.
    {
        typedef bdef_Bind_BoundTuple2<P1,P2> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1, p2, allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple3<P1,P2,P3> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2, P3 const&p3)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with three
        // parameters.
    {
        typedef bdef_Bind_BoundTuple3<P1,P2,P3> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                    (func, ListType(p1,p2,p3, allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple4<P1,P2,P3,P4> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2, P3 const&p3, P4 const&p4)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with four parameters.
    {
        typedef bdef_Bind_BoundTuple4<P1,P2,P3,P4> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4, allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple5<P1,P2,P3,P4,P5> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2, P3 const&p3, P4 const&p4, P5 const&p5)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with five parameters.
    {
        typedef bdef_Bind_BoundTuple5<P1,P2,P3,P4,P5> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4,p5, allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2, P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with six parameters.
    {
        typedef bdef_Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                   (func, ListType(p1,p2,p3,p4,p5,p6, allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,P7> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2, P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6,
          P7 const&p7)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with seven
        // parameters.
    {
        typedef bdef_Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,P7> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
                  (func, ListType(p1,p2,p3,p4,p5,p6,p7, allocator), allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,P8> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2, P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6,
          P7 const&p7, P8 const&p8)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eight
        // parameters.
    {
        typedef bdef_Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,P8> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,allocator),allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9>
    static
    bdef_Bind<bslmf_Nil, FUNC,
              bdef_Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,P8,P9> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
         P2 const&p2, P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6,
         P7 const&p7, P8 const&p8, P9 const&p9)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with nine parameters.
    {
        typedef bdef_Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,P8,P9> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,allocator),allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple10<
                                              P1,P2,P3,P4,P5,P6,P7,P8,P9,P10> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2, P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6,
          P7 const&p7, P8 const&p8, P9 const&p9, P10 const&p10)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with ten parameters.
    {
        typedef bdef_Bind_BoundTuple10<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10>
            ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,allocator),
             allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple11<
                                          P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2, P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6,
          P7 const&p7, P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eleven
        // parameters.
    {
        typedef bdef_Bind_BoundTuple11<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11>
            ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,allocator),
             allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple12<
                                      P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2, P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6,
          P7 const&p7, P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11,
          P12 const&p12)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with twelve
        // parameters.
    {
        typedef bdef_Bind_BoundTuple12<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12>
            ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,allocator),
             allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12, class P13>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple13<
                                  P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13> >
    bindA(bslma_Allocator *allocator,FUNC func, P1 const&p1, P2 const&p2,
          P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6, P7 const&p7,
          P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11,
          P12 const&p12, P13 const&p13)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with thirteen
        // parameters.
    {
        typedef bdef_Bind_BoundTuple13<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,
                                       P13> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,
                            allocator),allocator);
    }

    template <class FUNC, class P1, class P2, class P3, class P4, class P5,
              class P6, class P7, class P8, class P9, class P10, class P11,
              class P12, class P13, class P14>
    static
    bdef_Bind<bslmf_Nil, FUNC, bdef_Bind_BoundTuple14<
                              P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14> >
    bindA(bslma_Allocator *allocator, FUNC func, P1 const&p1,
          P2 const&p2, P3 const&p3, P4 const&p4, P5 const&p5, P6 const&p6,
          P7 const&p7, P8 const&p8, P9 const&p9, P10 const&p10, P11 const&p11,
          P12 const&p12, P13 const&p13, P14 const&p14)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with fourteen
        // parameters.
    {
        typedef bdef_Bind_BoundTuple14<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,
                                       P13,P14> ListType;
        return bdef_Bind<bslmf_Nil, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,
                            allocator),allocator);
    }

    template <class RET, class FUNC>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple0>
    bindR(FUNC func)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with no parameters
        // and returns a value of type 'RET'.
    {
        return bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple0>
            (func, bdef_Bind_BoundTuple0());
    }

    template <class RET, class FUNC, class P1>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple1<P1> >
    bindR(FUNC func, P1 const&p1)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with one parameter
        // and returns a value of type 'RET'.
    {
        return bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple1<P1> >
            (func, bdef_Bind_BoundTuple1<P1>(p1));
    }

    template <class RET, class FUNC, class P1, class P2>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple2<P1,P2> >
    bindR(FUNC func, P1 const&p1, P2 const&p2)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with two parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple2<P1,P2> ListType;
        return bdef_Bind<RET, FUNC, ListType>(func, ListType(p1,p2));
    }

    template <class RET, class FUNC, class P1, class P2, class P3>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple3<P1,P2,P3> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with three parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple3<P1,P2,P3> ListType;
        return bdef_Bind<RET, FUNC, ListType>(func, ListType(p1,p2,p3));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple4<P1,P2,P3,P4> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with four parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple4<P1,P2,P3,P4> ListType;
        return bdef_Bind<RET,FUNC,ListType>(func,ListType(p1,p2,p3,p4));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple5<P1,P2,P3,P4,P5> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
          P5 const&p5)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with five parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple5<P1,P2,P3,P4,P5> ListType;
        return bdef_Bind<RET,FUNC,ListType>(func, ListType(p1,p2,p3,p4,p5));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
          P5 const&p5, P6 const&p6)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with six parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple6<P1,P2,P3,P4,P5,P6> ListType;
        return
            bdef_Bind<RET, FUNC, ListType>(func, ListType(p1,p2,p3,p4,p5,p6));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,P7> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
          P5 const&p5, P6 const&p6, P7 const&p7)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with seven parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple7<P1,P2,P3,P4,P5,P6,P7> ListType;
        return bdef_Bind<RET, FUNC,
            ListType>(func, ListType(p1,p2,p3,p4,p5,p6,p7));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,P8> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
          P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eight parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple8<P1,P2,P3,P4,P5,P6,P7,P8> ListType;
        return bdef_Bind<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,P8,P9> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
          P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with nine parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple9<P1,P2,P3,P4,P5,P6,P7,P8,P9> ListType;
        return bdef_Bind<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10>
    static
    bdef_Bind<RET, FUNC,
              bdef_Bind_BoundTuple10<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
          P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9,
          P10 const&p10)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with ten parameters
        // and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple10<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10>
            ListType;
        return bdef_Bind<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple11<
                                          P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
          P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9,
          P10 const&p10, P11 const&p11)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with eleven
        // parameters and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple11<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11>
            ListType;
        return bdef_Bind<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11, class P12>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple12<
                                      P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
          P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9,
          P10 const&p10, P11 const&p11, P12 const&p12)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with twelve
        // parameters and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple12<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12>
            ListType;
        return bdef_Bind<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11, class P12, class P13>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple13<
                                  P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
          P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9,
          P10 const&p10, P11 const&p11, P12 const&p12, P13 const&p13)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with thirteen
        // parameters and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple13<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,
                                       P13> ListType;
        return bdef_Bind<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13));
    }

    template <class RET, class FUNC, class P1, class P2, class P3, class P4,
              class P5, class P6, class P7, class P8, class P9, class P10,
              class P11, class P12, class P13, class P14>
    static
    bdef_Bind<RET, FUNC, bdef_Bind_BoundTuple14<
                              P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,P13,P14> >
    bindR(FUNC func, P1 const&p1, P2 const&p2, P3 const&p3, P4 const&p4,
          P5 const&p5, P6 const&p6, P7 const&p7, P8 const&p8, P9 const&p9,
          P10 const&p10, P11 const&p11, P12 const&p12, P13 const&p13,
          P14 const&p14)
        // Return a 'bdef_Bind' object that is bound to the specified
        // invocable object 'func', which can be invoked with fourteen
        // parameters and returns a value of type 'RET'.
    {
        typedef bdef_Bind_BoundTuple14<P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12,
                                       P13,P14> ListType;
        return bdef_Bind<RET, FUNC, ListType>
            (func, ListType(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14));
    }
};

// ---- Anything below this line is implementation specific.  Do not use.  ----

                          // ==========================
                          // class bdef_Bind_TupleValue
                          // ==========================

template <class TYPE>
class bdef_Bind_TupleValue {
    // This local class provides storage for a value of the specified 'TYPE'
    // suitable for storing an argument value in one of the
    // 'bdef_Bind_Tuple[0-14]' local classes.  'TYPE' must already be a
    // 'bslmf_ForwardingType', meaning no extra copies will be made (unless the
    // type is a fundamental type, which is meant to be copied for efficiency).

    // PRIVATE TYPES
    typedef typename bslmf_ArrayToConstPointer<TYPE>::Type STORAGE_TYPE;

    // PRIVATE INSTANCE DATA
    STORAGE_TYPE d_value;

  public:
    // CREATORS
    bdef_Bind_TupleValue(const bdef_Bind_TupleValue<TYPE>&  original)
        // Create a 'bdef_Bind_TupleValue' object holding a copy of the
        // specified 'original' value.
    : d_value(original.d_value)
    {
    }

    bdef_Bind_TupleValue(TYPE value)
        // Create a 'bdef_Bind_TupleValue' object holding a copy of the
        // specified 'value'.
    : d_value(value)
    {
    }

    // MANIPULATORS
    STORAGE_TYPE& value() { return d_value; }
        // Return a reference to the modifiable object held by this proxy.

    // ACCESSORS
    const STORAGE_TYPE& value() const { return d_value; }
        // Return a reference to the non-modifiable object held by this proxy.
};

template <class TYPE>
class bdef_Bind_TupleValue<TYPE&> {
    // This local class provides storage for a value of the specified 'TYPE'
    // suitable for storing an argument value in one of the 'bdef_Bind_Tuple*'
    // local classes.  This full specialization for reference types simply
    // stores the address of the argument value.

    // PRIVATE INSTANCE DATA
    TYPE *d_value;

  public:
    // CREATORS
    bdef_Bind_TupleValue(const bdef_Bind_TupleValue<TYPE&>& original)
        // Create a 'bdef_Bind_TupleValue' object holding a copy of the
        // specified 'original' reference.
    : d_value(original.d_value)
    {
    }

    bdef_Bind_TupleValue(TYPE& value)
        // Create a 'bdef_Bind_TupleValue' object holding the address of the
        // specified 'value'.
    : d_value(&value)
    {
    }

    // MANIPULATORS
    TYPE& value() { return *d_value; }
        // Return a reference to the modifiable object held by this proxy.

    // ACCESSORS
    const TYPE& value() const { return *d_value; }
        // Return a reference to the non-modifiable object held by this proxy.
};

template <class TYPE>
class bdef_Bind_TupleValue<TYPE const&> {
    // This local class provides storage for a value of the specified 'TYPE'
    // suitable for storing an argument value in one of the 'bdef_Bind_Tuple*'
    // local classes.  This full specialization for const reference types
    // simply stores the address of the argument value.

    // PRIVATE INSTANCE DATA
    const TYPE *d_value;

  public:
    // CREATORS
    bdef_Bind_TupleValue(const bdef_Bind_TupleValue<TYPE const&>& original)
        // Create a 'bdef_Bind_TupleValue' object holding a copy of the
        // specified 'original' reference.
    : d_value(original.d_value)
    {
    }

    bdef_Bind_TupleValue(const TYPE& value)
        // Create a 'bdef_Bind_TupleValue' object holding the address of the
        // specified 'value'.
    : d_value(&value)
    {
    }

    // MANIPULATORS
    const TYPE& value() { return *d_value; }
        // Return a reference to the non-modifiable object held by this proxy.

    // ACCESSORS
    const TYPE& value() const { return *d_value; }
        // Return a reference to the non-modifiable object held by this proxy.
};

                           // ======================
                           // class bdef_Bind_Tuple*
                           // ======================

struct bdef_Bind_Tuple0 : public bslmf_TypeList0
{
    // This 'struct' provides the creators for a list of zero arguments.

    // CREATORS
    bdef_Bind_Tuple0()
    {}

    bdef_Bind_Tuple0(const bdef_Bind_Tuple0&)
    {
    }
};

template <class A1>
struct bdef_Bind_Tuple1 : public bslmf_TypeList1<A1>
{
    // This 'struct' stores a list of one argument.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type FA1;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1> d_a1;

    // CREATORS
    bdef_Bind_Tuple1(const bdef_Bind_Tuple1<A1>&  orig)
    : d_a1(orig.d_a1)
    {
    }

    bdef_Bind_Tuple1(FA1 a1)
    : d_a1(a1)
    {
    }
};

template <class A1, class A2>
struct bdef_Bind_Tuple2 : public bslmf_TypeList2<A1,A2>
{
    // This 'struct' stores a list of two arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type FA2;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1> d_a1;
    bdef_Bind_TupleValue<FA2> d_a2;

    // CREATORS
    bdef_Bind_Tuple2(const bdef_Bind_Tuple2<A1,A2>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    {
    }

    bdef_Bind_Tuple2(FA1 a1, FA2 a2)
    : d_a1(a1)
    , d_a2(a2)
    {
    }
};

template <class A1, class A2, class A3>
struct bdef_Bind_Tuple3 : public bslmf_TypeList3<A1,A2,A3>
{
    // This 'struct' stores a list of three arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type FA3;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1> d_a1;
    bdef_Bind_TupleValue<FA2> d_a2;
    bdef_Bind_TupleValue<FA3> d_a3;

    // CREATORS
    bdef_Bind_Tuple3(const bdef_Bind_Tuple3<A1,A2,A3>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    {
    }

    bdef_Bind_Tuple3(FA1 a1, FA2 a2, FA3 a3)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    {
    }
};

template <class A1, class A2, class A3, class A4>
struct bdef_Bind_Tuple4 : public bslmf_TypeList4<A1,A2,A3,A4>
{
    // This 'struct' stores a list of four arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type FA4;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1> d_a1;
    bdef_Bind_TupleValue<FA2> d_a2;
    bdef_Bind_TupleValue<FA3> d_a3;
    bdef_Bind_TupleValue<FA4> d_a4;

    // CREATORS
    bdef_Bind_Tuple4(const bdef_Bind_Tuple4<A1,A2,A3,A4>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    {
    }

    bdef_Bind_Tuple4(FA1 a1, FA2 a2, FA3 a3, FA4 a4)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5>
struct bdef_Bind_Tuple5 : public bslmf_TypeList5<A1,A2,A3,A4,A5>
{
    // This 'struct' stores a list of five arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type FA4;
    typedef typename bslmf_ConstForwardingType<A5>::Type FA5;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1> d_a1;
    bdef_Bind_TupleValue<FA2> d_a2;
    bdef_Bind_TupleValue<FA3> d_a3;
    bdef_Bind_TupleValue<FA4> d_a4;
    bdef_Bind_TupleValue<FA5> d_a5;

    // CREATORS
    bdef_Bind_Tuple5(const bdef_Bind_Tuple5<A1,A2,A3,A4,A5>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    , d_a5(orig.d_a5)
    {
    }

    bdef_Bind_Tuple5(FA1 a1, FA2 a2, FA3 a3, FA4 a4, FA5 a5)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    , d_a5(a5)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6>
struct bdef_Bind_Tuple6 : public bslmf_TypeList6<A1,A2,A3,A4,A5,A6>
{
    // This 'struct' stores a list of six arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type FA4;
    typedef typename bslmf_ConstForwardingType<A5>::Type FA5;
    typedef typename bslmf_ConstForwardingType<A6>::Type FA6;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1> d_a1;
    bdef_Bind_TupleValue<FA2> d_a2;
    bdef_Bind_TupleValue<FA3> d_a3;
    bdef_Bind_TupleValue<FA4> d_a4;
    bdef_Bind_TupleValue<FA5> d_a5;
    bdef_Bind_TupleValue<FA6> d_a6;

    // CREATORS
    bdef_Bind_Tuple6(const bdef_Bind_Tuple6<A1,A2,A3,A4,A5,A6>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    , d_a5(orig.d_a5)
    , d_a6(orig.d_a6)
    {
    }

    bdef_Bind_Tuple6(FA1 a1, FA2 a2, FA3 a3, FA4 a4, FA5 a5, FA6 a6)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    , d_a5(a5)
    , d_a6(a6)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
struct bdef_Bind_Tuple7 : public bslmf_TypeList7<A1,A2,A3,A4,A5,A6,A7>
{
    // This 'struct' stores a list of seven arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type FA4;
    typedef typename bslmf_ConstForwardingType<A5>::Type FA5;
    typedef typename bslmf_ConstForwardingType<A6>::Type FA6;
    typedef typename bslmf_ConstForwardingType<A7>::Type FA7;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1> d_a1;
    bdef_Bind_TupleValue<FA2> d_a2;
    bdef_Bind_TupleValue<FA3> d_a3;
    bdef_Bind_TupleValue<FA4> d_a4;
    bdef_Bind_TupleValue<FA5> d_a5;
    bdef_Bind_TupleValue<FA6> d_a6;
    bdef_Bind_TupleValue<FA7> d_a7;

    // CREATORS
    inline
    bdef_Bind_Tuple7(const bdef_Bind_Tuple7<A1,A2,A3,A4,A5,A6,A7>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    , d_a5(orig.d_a5)
    , d_a6(orig.d_a6)
    , d_a7(orig.d_a7)
    {
    }

    bdef_Bind_Tuple7(FA1 a1, FA2 a2, FA3 a3, FA4 a4, FA5 a5, FA6 a6,
                     FA7 a7)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    , d_a5(a5)
    , d_a6(a6)
    , d_a7(a7)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
struct bdef_Bind_Tuple8 : public bslmf_TypeList8<A1,A2,A3,A4,A5,A6,A7,A8>
{
    // This 'struct' stores a list of eight arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type FA4;
    typedef typename bslmf_ConstForwardingType<A5>::Type FA5;
    typedef typename bslmf_ConstForwardingType<A6>::Type FA6;
    typedef typename bslmf_ConstForwardingType<A7>::Type FA7;
    typedef typename bslmf_ConstForwardingType<A8>::Type FA8;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1> d_a1;
    bdef_Bind_TupleValue<FA2> d_a2;
    bdef_Bind_TupleValue<FA3> d_a3;
    bdef_Bind_TupleValue<FA4> d_a4;
    bdef_Bind_TupleValue<FA5> d_a5;
    bdef_Bind_TupleValue<FA6> d_a6;
    bdef_Bind_TupleValue<FA7> d_a7;
    bdef_Bind_TupleValue<FA8> d_a8;

    // CREATORS
    inline
    bdef_Bind_Tuple8(const bdef_Bind_Tuple8<A1,A2,A3,A4,A5,A6,A7,A8>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    , d_a5(orig.d_a5)
    , d_a6(orig.d_a6)
    , d_a7(orig.d_a7)
    , d_a8(orig.d_a8)
    {
    }

    bdef_Bind_Tuple8(FA1 a1, FA2 a2, FA3 a3, FA4 a4, FA5 a5, FA6 a6,
                     FA7 a7, FA8 a8)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    , d_a5(a5)
    , d_a6(a6)
    , d_a7(a7)
    , d_a8(a8)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
struct bdef_Bind_Tuple9 : public bslmf_TypeList9<A1,A2,A3,A4,A5,A6,A7,A8,A9>
{
    // This 'struct' stores a list of nine arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type FA4;
    typedef typename bslmf_ConstForwardingType<A5>::Type FA5;
    typedef typename bslmf_ConstForwardingType<A6>::Type FA6;
    typedef typename bslmf_ConstForwardingType<A7>::Type FA7;
    typedef typename bslmf_ConstForwardingType<A8>::Type FA8;
    typedef typename bslmf_ConstForwardingType<A9>::Type FA9;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1> d_a1;
    bdef_Bind_TupleValue<FA2> d_a2;
    bdef_Bind_TupleValue<FA3> d_a3;
    bdef_Bind_TupleValue<FA4> d_a4;
    bdef_Bind_TupleValue<FA5> d_a5;
    bdef_Bind_TupleValue<FA6> d_a6;
    bdef_Bind_TupleValue<FA7> d_a7;
    bdef_Bind_TupleValue<FA8> d_a8;
    bdef_Bind_TupleValue<FA9> d_a9;

    // CREATORS
    inline
    bdef_Bind_Tuple9(const bdef_Bind_Tuple9<A1,A2,A3,A4,A5,A6,A7,A8,A9>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    , d_a5(orig.d_a5)
    , d_a6(orig.d_a6)
    , d_a7(orig.d_a7)
    , d_a8(orig.d_a8)
    , d_a9(orig.d_a9)
    {
    }

    bdef_Bind_Tuple9(FA1 a1, FA2 a2, FA3 a3, FA4 a4, FA5 a5, FA6 a6,
                     FA7 a7, FA8 a8, FA9 a9)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    , d_a5(a5)
    , d_a6(a6)
    , d_a7(a7)
    , d_a8(a8)
    , d_a9(a9)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
struct bdef_Bind_Tuple10 : public bslmf_TypeList10<A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                   A10>
{
    // This 'struct' stores a list of ten arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type  FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type  FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type  FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type  FA4;
    typedef typename bslmf_ConstForwardingType<A5>::Type  FA5;
    typedef typename bslmf_ConstForwardingType<A6>::Type  FA6;
    typedef typename bslmf_ConstForwardingType<A7>::Type  FA7;
    typedef typename bslmf_ConstForwardingType<A8>::Type  FA8;
    typedef typename bslmf_ConstForwardingType<A9>::Type  FA9;
    typedef typename bslmf_ConstForwardingType<A10>::Type FA10;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1>  d_a1;
    bdef_Bind_TupleValue<FA2>  d_a2;
    bdef_Bind_TupleValue<FA3>  d_a3;
    bdef_Bind_TupleValue<FA4>  d_a4;
    bdef_Bind_TupleValue<FA5>  d_a5;
    bdef_Bind_TupleValue<FA6>  d_a6;
    bdef_Bind_TupleValue<FA7>  d_a7;
    bdef_Bind_TupleValue<FA8>  d_a8;
    bdef_Bind_TupleValue<FA9>  d_a9;
    bdef_Bind_TupleValue<FA10> d_a10;

    // CREATORS
    bdef_Bind_Tuple10(const bdef_Bind_Tuple10<A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                             A10>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    , d_a5(orig.d_a5)
    , d_a6(orig.d_a6)
    , d_a7(orig.d_a7)
    , d_a8(orig.d_a8)
    , d_a9(orig.d_a9)
    , d_a10(orig.d_a10)
    {
    }

    bdef_Bind_Tuple10(FA1 a1, FA2 a2, FA3 a3, FA4 a4, FA5 a5, FA6 a6,
                      FA7 a7, FA8 a8, FA9 a9, FA10 a10)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    , d_a5(a5)
    , d_a6(a6)
    , d_a7(a7)
    , d_a8(a8)
    , d_a9(a9)
    , d_a10(a10)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11>
struct bdef_Bind_Tuple11 : public bslmf_TypeList11<A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                   A10,A11>
{
    // This 'struct' stores a list of eleven arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type  FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type  FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type  FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type  FA4;
    typedef typename bslmf_ConstForwardingType<A5>::Type  FA5;
    typedef typename bslmf_ConstForwardingType<A6>::Type  FA6;
    typedef typename bslmf_ConstForwardingType<A7>::Type  FA7;
    typedef typename bslmf_ConstForwardingType<A8>::Type  FA8;
    typedef typename bslmf_ConstForwardingType<A9>::Type  FA9;
    typedef typename bslmf_ConstForwardingType<A10>::Type FA10;
    typedef typename bslmf_ConstForwardingType<A11>::Type FA11;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1>  d_a1;
    bdef_Bind_TupleValue<FA2>  d_a2;
    bdef_Bind_TupleValue<FA3>  d_a3;
    bdef_Bind_TupleValue<FA4>  d_a4;
    bdef_Bind_TupleValue<FA5>  d_a5;
    bdef_Bind_TupleValue<FA6>  d_a6;
    bdef_Bind_TupleValue<FA7>  d_a7;
    bdef_Bind_TupleValue<FA8>  d_a8;
    bdef_Bind_TupleValue<FA9>  d_a9;
    bdef_Bind_TupleValue<FA10> d_a10;
    bdef_Bind_TupleValue<FA11> d_a11;

    // CREATORS
    bdef_Bind_Tuple11(const bdef_Bind_Tuple11<A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                              A10,A11>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    , d_a5(orig.d_a5)
    , d_a6(orig.d_a6)
    , d_a7(orig.d_a7)
    , d_a8(orig.d_a8)
    , d_a9(orig.d_a9)
    , d_a10(orig.d_a10)
    , d_a11(orig.d_a11)
    {
    }

    bdef_Bind_Tuple11(FA1 a1, FA2 a2, FA3 a3, FA4 a4, FA5 a5, FA6 a6,
                      FA7 a7, FA8 a8, FA9 a9, FA10 a10, FA11 a11)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    , d_a5(a5)
    , d_a6(a6)
    , d_a7(a7)
    , d_a8(a8)
    , d_a9(a9)
    , d_a10(a10)
    , d_a11(a11)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12>
struct bdef_Bind_Tuple12 : public bslmf_TypeList12<A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                   A10,A11,A12>
{
    // This 'struct' stores a list of twelve arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type  FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type  FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type  FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type  FA4;
    typedef typename bslmf_ConstForwardingType<A5>::Type  FA5;
    typedef typename bslmf_ConstForwardingType<A6>::Type  FA6;
    typedef typename bslmf_ConstForwardingType<A7>::Type  FA7;
    typedef typename bslmf_ConstForwardingType<A8>::Type  FA8;
    typedef typename bslmf_ConstForwardingType<A9>::Type  FA9;
    typedef typename bslmf_ConstForwardingType<A10>::Type FA10;
    typedef typename bslmf_ConstForwardingType<A11>::Type FA11;
    typedef typename bslmf_ConstForwardingType<A12>::Type FA12;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1>  d_a1;
    bdef_Bind_TupleValue<FA2>  d_a2;
    bdef_Bind_TupleValue<FA3>  d_a3;
    bdef_Bind_TupleValue<FA4>  d_a4;
    bdef_Bind_TupleValue<FA5>  d_a5;
    bdef_Bind_TupleValue<FA6>  d_a6;
    bdef_Bind_TupleValue<FA7>  d_a7;
    bdef_Bind_TupleValue<FA8>  d_a8;
    bdef_Bind_TupleValue<FA9>  d_a9;
    bdef_Bind_TupleValue<FA10> d_a10;
    bdef_Bind_TupleValue<FA11> d_a11;
    bdef_Bind_TupleValue<FA12> d_a12;

    // CREATORS
    bdef_Bind_Tuple12(const bdef_Bind_Tuple12<A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                              A10,A11,A12>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    , d_a5(orig.d_a5)
    , d_a6(orig.d_a6)
    , d_a7(orig.d_a7)
    , d_a8(orig.d_a8)
    , d_a9(orig.d_a9)
    , d_a10(orig.d_a10)
    , d_a11(orig.d_a11)
    , d_a12(orig.d_a12)
    {
    }

    bdef_Bind_Tuple12(FA1 a1, FA2 a2, FA3 a3, FA4 a4, FA5 a5, FA6 a6,
                      FA7 a7, FA8 a8, FA9 a9, FA10 a10, FA11 a11,
                      FA12 a12)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    , d_a5(a5)
    , d_a6(a6)
    , d_a7(a7)
    , d_a8(a8)
    , d_a9(a9)
    , d_a10(a10)
    , d_a11(a11)
    , d_a12(a12)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13>
struct bdef_Bind_Tuple13 : public bslmf_TypeList13<A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                   A10,A11,A12,A13>
{
    // This 'struct' stores a list of thirteen arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type  FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type  FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type  FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type  FA4;
    typedef typename bslmf_ConstForwardingType<A5>::Type  FA5;
    typedef typename bslmf_ConstForwardingType<A6>::Type  FA6;
    typedef typename bslmf_ConstForwardingType<A7>::Type  FA7;
    typedef typename bslmf_ConstForwardingType<A8>::Type  FA8;
    typedef typename bslmf_ConstForwardingType<A9>::Type  FA9;
    typedef typename bslmf_ConstForwardingType<A10>::Type FA10;
    typedef typename bslmf_ConstForwardingType<A11>::Type FA11;
    typedef typename bslmf_ConstForwardingType<A12>::Type FA12;
    typedef typename bslmf_ConstForwardingType<A13>::Type FA13;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1>  d_a1;
    bdef_Bind_TupleValue<FA2>  d_a2;
    bdef_Bind_TupleValue<FA3>  d_a3;
    bdef_Bind_TupleValue<FA4>  d_a4;
    bdef_Bind_TupleValue<FA5>  d_a5;
    bdef_Bind_TupleValue<FA6>  d_a6;
    bdef_Bind_TupleValue<FA7>  d_a7;
    bdef_Bind_TupleValue<FA8>  d_a8;
    bdef_Bind_TupleValue<FA9>  d_a9;
    bdef_Bind_TupleValue<FA10> d_a10;
    bdef_Bind_TupleValue<FA11> d_a11;
    bdef_Bind_TupleValue<FA12> d_a12;
    bdef_Bind_TupleValue<FA13> d_a13;

    // CREATORS
    bdef_Bind_Tuple13(const bdef_Bind_Tuple13<A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                              A10,A11,A12,A13>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    , d_a5(orig.d_a5)
    , d_a6(orig.d_a6)
    , d_a7(orig.d_a7)
    , d_a8(orig.d_a8)
    , d_a9(orig.d_a9)
    , d_a10(orig.d_a10)
    , d_a11(orig.d_a11)
    , d_a12(orig.d_a12)
    , d_a13(orig.d_a13)
    {
    }

    bdef_Bind_Tuple13(FA1 a1, FA2 a2, FA3 a3, FA4 a4, FA5 a5, FA6 a6,
                      FA7 a7, FA8 a8, FA9 a9, FA10 a10, FA11 a11,
                      FA12 a12, FA13 a13)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    , d_a5(a5)
    , d_a6(a6)
    , d_a7(a7)
    , d_a8(a8)
    , d_a9(a9)
    , d_a10(a10)
    , d_a11(a11)
    , d_a12(a12)
    , d_a13(a13)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14>
struct bdef_Bind_Tuple14 : public bslmf_TypeList14<A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                   A10,A11,A12,A13,A14>
{
    // This 'struct' stores a list of fourteen arguments.

    // TYPES
    typedef typename bslmf_ConstForwardingType<A1>::Type  FA1;
    typedef typename bslmf_ConstForwardingType<A2>::Type  FA2;
    typedef typename bslmf_ConstForwardingType<A3>::Type  FA3;
    typedef typename bslmf_ConstForwardingType<A4>::Type  FA4;
    typedef typename bslmf_ConstForwardingType<A5>::Type  FA5;
    typedef typename bslmf_ConstForwardingType<A6>::Type  FA6;
    typedef typename bslmf_ConstForwardingType<A7>::Type  FA7;
    typedef typename bslmf_ConstForwardingType<A8>::Type  FA8;
    typedef typename bslmf_ConstForwardingType<A9>::Type  FA9;
    typedef typename bslmf_ConstForwardingType<A10>::Type FA10;
    typedef typename bslmf_ConstForwardingType<A11>::Type FA11;
    typedef typename bslmf_ConstForwardingType<A12>::Type FA12;
    typedef typename bslmf_ConstForwardingType<A13>::Type FA13;
    typedef typename bslmf_ConstForwardingType<A14>::Type FA14;

    // INSTANCE DATA
    bdef_Bind_TupleValue<FA1>  d_a1;
    bdef_Bind_TupleValue<FA2>  d_a2;
    bdef_Bind_TupleValue<FA3>  d_a3;
    bdef_Bind_TupleValue<FA4>  d_a4;
    bdef_Bind_TupleValue<FA5>  d_a5;
    bdef_Bind_TupleValue<FA6>  d_a6;
    bdef_Bind_TupleValue<FA7>  d_a7;
    bdef_Bind_TupleValue<FA8>  d_a8;
    bdef_Bind_TupleValue<FA9>  d_a9;
    bdef_Bind_TupleValue<FA10> d_a10;
    bdef_Bind_TupleValue<FA11> d_a11;
    bdef_Bind_TupleValue<FA12> d_a12;
    bdef_Bind_TupleValue<FA13> d_a13;
    bdef_Bind_TupleValue<FA14> d_a14;

    // CREATORS
    bdef_Bind_Tuple14(const bdef_Bind_Tuple14<A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                              A10,A11,A12,A13,A14>& orig)
    : d_a1(orig.d_a1)
    , d_a2(orig.d_a2)
    , d_a3(orig.d_a3)
    , d_a4(orig.d_a4)
    , d_a5(orig.d_a5)
    , d_a6(orig.d_a6)
    , d_a7(orig.d_a7)
    , d_a8(orig.d_a8)
    , d_a9(orig.d_a9)
    , d_a10(orig.d_a10)
    , d_a11(orig.d_a11)
    , d_a12(orig.d_a12)
    , d_a13(orig.d_a13)
    , d_a14(orig.d_a14)
    {
    }

    bdef_Bind_Tuple14(FA1 a1, FA2 a2, FA3 a3, FA4 a4, FA5 a5, FA6 a6,
                      FA7 a7, FA8 a8, FA9 a9, FA10 a10, FA11 a11,
                      FA12 a12, FA13 a13, FA14 a14)
    : d_a1(a1)
    , d_a2(a2)
    , d_a3(a3)
    , d_a4(a4)
    , d_a5(a5)
    , d_a6(a6)
    , d_a7(a7)
    , d_a8(a8)
    , d_a9(a9)
    , d_a10(a10)
    , d_a11(a11)
    , d_a12(a12)
    , d_a13(a13)
    , d_a14(a14)
    {
    }
};

                          // ====================
                          // class bdef_Bind_Impl
                          // ====================

template <class RET, class FUNC, class LIST>
class bdef_Bind_Impl {
    // This class is an implementation detail of 'bdef_Bind'.  Do not use.
    // This class implements the storage and functionality required for a
    // binder that invokes an object of type 'FUNC' with a list of invocation
    // parameters of type 'LIST'.  The return type of the invocation is
    // determined by a combination of type 'RET' and 'FUNC'.  Note that this
    // class is a generic binder implementation; this component provides more
    // type safe implementation 'bdef_Bind_ImplExplicit' that will be used
    // under certain conditions.

    // PRIVATE TYPES
    typedef bdef_Bind_FuncTraits<RET,FUNC>                 Traits;
    typedef bslmf_Tag<Traits::HAS_POINTER_SEMANTICS>       HasPointerSemantics;
    typedef bdef_Bind_Invoker<typename Traits::ResultType,
                              LIST::LENGTH>                Invoker;
    typedef typename Traits::Type                          FuncType;

    // PRIVATE INSTANCE DATA
    bslalg_ConstructorProxy<typename Traits::WrapperType>  d_func;
    LIST                                                   d_list;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdef_Bind_Impl,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // PUBLIC TYPES
    typedef typename Traits::ResultType                    ResultType;
        // The return type of this binder object.

  private:
    // PRIVATE ACCESSORS
    template <class ARGS>
    inline ResultType invokeImpl(ARGS& arguments, bslmf_Tag<0>) const
        // Invoke the bound functor using the invocation parameters provided at
        // construction of this 'bdef_Bind' object.  Substituting place-holders
        // for their respective values in the specified 'arguments'.  The
        // 'bslmf_Tag' is only used for overloading resolution - indicating
        // whether 'd_func' has pointer semantics or not.
    {
        Invoker invoker;
        return invoker.invoke(&d_func.object(), &d_list, arguments);
    }

    template <class ARGS>
    inline ResultType invokeImpl(ARGS& arguments, bslmf_Tag<1>) const
        // Invoke the bound functor using the invocation parameters provided at
        // construction of this 'bdef_Bind' object.  Substituting place-holders
        // for their respective values in the specified 'arguments'.  The
        // 'bslmf_Tag' is only used for overloading resolution - indicating
        // whether 'd_func' has pointer semantics or not.
    {
        Invoker invoker;
        return invoker.invoke(&(*d_func.object()), &d_list, arguments);
    }

  public:
    // CREATORS
    bdef_Bind_Impl(typename bslmf_ForwardingType<FUNC>::Type  func,
                   LIST const&                                list,
                   bslma_Allocator                           *basicAllocator=0)
        // Construct a 'bdef_Bind_Impl' object bound to the specified
        // invocable object 'func' using the invocation parameters specified
        // in 'list'.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.
    : d_func(func, basicAllocator)
    , d_list(list, basicAllocator)
    {
    }

    bdef_Bind_Impl(const bdef_Bind_Impl&  other,
                   bslma_Allocator       *basicAllocator = 0)
        // Construct a 'bdef_Bind_Impl' object bound to the same invocable
        // object 'func' and using the same invocation parameters as the
        // specified 'other' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.
    : d_func(other.d_func, basicAllocator)
    , d_list(other.d_list, basicAllocator)
    {
    }

    // ACCESSORS
    template <class ARGS>
    ResultType invoke(ARGS& arguments) const
        // Invoke the bound functor using the invocation parameters provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for their respective values in the specified 'arguments'.
    {
        return invokeImpl(arguments, HasPointerSemantics());
    }

    ResultType operator()() const
        // Invoke the bound functor using only the invocation parameters
        // provided at construction of this 'bdef_Bind' object and return
        // the result.
    {
        typedef bdef_Bind_Tuple0 ARGS;
        ARGS args;
        return invoke(args);
    }

    template <class P1>
    ResultType operator()(P1& p1)  const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for argument 1 with the value of the argument 'p1'.  Return the
        // result.
    {
        typedef bdef_Bind_Tuple1<P1&> ARGS;
        ARGS args(p1);
        return invoke(args);
    }

    template <class P1>
    ResultType operator()(P1 const& p1)  const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for argument 1 with the value of the argument 'p1'.  Return the
        // result.
    {
        typedef bdef_Bind_Tuple1<P1 const&> ARGS;
        ARGS args(p1);
        return invoke(args);
    }

    template <class P1, class P2>
    ResultType operator()(P1& p1, P2& p2) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 and 2 with the value of the arguments 'p1', and 'p2'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple2<P1&, P2&> ARGS;
        ARGS args(p1, p2);
        return invoke(args);
    }

    template <class P1, class P2>
    ResultType operator()(P1 const& p1, P2 const& p2) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 and 2 with the value of the arguments 'p1', and 'p2'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple2<P1 const&, P2 const&> ARGS;
        ARGS args(p1, p2);
        return invoke(args);
    }

    template <class P1, class P2, class P3>
    ResultType operator()(P1& p1, P2& p2, P3& p3) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1, 2, and 3 with the values of the arguments 'p1',
        // 'p2' and 'p3' respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple3<P1&, P2&, P3&> ARGS;
        ARGS args(p1, p2, p3);
        return invoke(args);
    }

    template <class P1, class P2, class P3>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3)
    const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1, 2, and 3 with the values of the arguments 'p1',
        // 'p2' and 'p3' respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple3<P1 const&, P2 const&, P3 const&> ARGS;
        ARGS args(p1, p2, p3);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 4 with the values of the arguments 'p1' - 'p4'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple4<P1&, P2&, P3&, P4&> ARGS;
        ARGS args(p1, p2, p3, p4);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 4 with the values of the arguments 'p1' - 'p4'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple4<P1 const&, P2 const&, P3 const&, P4 const&>
                                 ARGS;
        ARGS args(p1, p2, p3, p4);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 5 with the values of the arguments 'p1' - 'p5'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple5<P1&, P2&, P3&, P4&, P5&> ARGS;
        ARGS args(p1, p2, p3, p4, p5);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                                 P4 const& p4, P5 const& p5) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 5 with the values of the arguments 'p1' - 'p5'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple5<P1 const&, P2 const&, P3 const&, P4 const&,
                                 P5 const&> ARGS;
        ARGS args(p1, p2, p3, p4, p5);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                                 P6& p6) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 6 with the values of the arguments 'p1' - 'p7'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple6<P1&, P2&, P3&, P4&, P5&, P6&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                          P4 const& p4, P5 const& p5, P6 const& p6) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 6 with the values of the arguments 'p1' - 'p7'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple6<P1 const&, P2 const&, P3 const&, P4 const&,
                                 P5 const&, P6 const&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                          P6& p6, P7& p7) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 7 with the values of the arguments 'p1' - 'p7'
        // respectively.  Return the result.

    {
        typedef bdef_Bind_Tuple7<P1&, P2&, P3&, P4&, P5&, P6&, P7&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                          P4 const& p4, P5 const& p5, P6 const& p6,
                          P7 const& p7) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 7 with the values of the arguments 'p1' - 'p7'
        // respectively.  Return the result.

    {
        typedef bdef_Bind_Tuple7<P1 const&, P2 const&, P3 const&, P4 const&,
                                 P5 const&, P6 const&, P7 const&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                          P6& p6, P7& p7, P8& p8) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 8 with the values of the arguments 'p1' - 'p8'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple8<P1&, P2&, P3&, P4&, P5&, P6&, P7&, P8&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                          P4 const& p4, P5 const& p5, P6 const& p6,
                          P7 const& p7, P8 const& p8) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 8 with the values of the arguments 'p1' - 'p8'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple8<P1 const&, P2 const&, P3 const&, P4 const&,
                                 P5 const&, P6 const&, P7 const&, P8 const&>
                                 ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                          P6& p6, P7& p7, P8& p8, P9& p9) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 9 with the values of the arguments 'p1' - 'p9'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple9<P1&, P2&, P3&, P4&, P5&, P6&, P7&, P8&,
                                 P9&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                          P4 const& p4, P5 const& p5, P6 const& p6,
                          P7 const& p7, P8 const& p8, P9 const& p9)
    const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 9 with the values of the arguments 'p1' - 'p9'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple9<P1 const&, P2 const&, P3 const&, P4 const&,
                                 P5 const&, P6 const&, P7 const&, P8 const&,
                                 P9 const&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                          P6& p6, P7& p7, P8& p8, P9& p9,
                          P10& p10) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 10 with the values of the arguments 'p1' - 'p10'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple10<P1&, P2&, P3&, P4&, P5&, P6&, P7&, P8&,
                                  P9&, P10&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                          P4 const& p4, P5 const& p5, P6 const& p6,
                          P7 const& p7, P8 const& p8, P9 const& p9,
                          P10 const& p10) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 10 with the values of the arguments 'p1' - 'p10'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple10<P1 const&, P2 const&, P3 const&, P4 const&,
                                  P5 const&, P6 const&, P7 const&, P8 const&,
                                  P9 const&, P10 const&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                          P6& p6, P7& p7, P8& p8, P9& p9, P10& p10,
                          P11& p11) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 11 with the values of the arguments 'p1' - 'p11'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple11<P1&, P2&, P3&, P4&, P5&, P6&, P7&, P8&,
                                  P9&, P10&, P11&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                          P4 const& p4, P5 const& p5, P6 const& p6,
                          P7 const& p7, P8 const& p8, P9 const& p9,
                                 P10 const& p10, P11 const& p11) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 11 with the values of the arguments 'p1' - 'p11'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple11<P1 const&, P2 const&, P3 const&, P4 const&,
                                  P5 const&, P6 const&, P7 const&, P8 const&,
                                  P9 const&, P10 const&, P11 const&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                          P6& p6, P7& p7, P8& p8, P9& p9, P10& p10,
                          P11& p11, P12& p12) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 12 with the values of the arguments 'p1' - 'p12'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple12<P1&, P2&, P3&, P4&, P5&, P6&, P7&, P8&,
                                  P9&, P10&, P11&, P12&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                          P4 const& p4, P5 const& p5, P6 const& p6,
                          P7 const& p7, P8 const& p8, P9 const& p9,
                          P10 const& p10, P11 const& p11,
                          P12 const& p12) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 12 with the values of the arguments 'p1' - 'p12'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple12<P1 const&, P2 const&, P3 const&, P4 const&,
                                  P5 const&, P6 const&, P7 const&, P8 const&,
                                  P9 const&, P10 const&, P11 const&,
                                  P12 const&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12,
              class P13>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                          P6& p6, P7& p7, P8& p8, P9& p9, P10& p10,
                          P11& p11, P12& p12, P13& p13) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 13 with the values of the arguments 'p1' - 'p13'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple13<P1&, P2&, P3&, P4&, P5&, P6&, P7&, P8&,
                                  P9&, P10&, P11&, P12&, P13&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12,
              class P13>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                          P4 const& p4, P5 const& p5, P6 const& p6,
                          P7 const& p7, P8 const& p8, P9 const& p9,
                          P10 const& p10, P11 const& p11,
                          P12 const& p12, P13 const& p13) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 13 with the values of the arguments 'p1' - 'p13'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple13<P1 const&, P2 const&, P3 const&, P4 const&,
                                  P5 const&, P6 const&, P7 const&, P8 const&,
                                  P9 const&,  P10 const&, P11 const&,
                                  P12 const&, P13 const&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12,
              class P13, class P14>
    ResultType operator()(P1& p1, P2& p2, P3& p3, P4& p4, P5& p5,
                          P6& p6, P7& p7, P8& p8, P9& p9, P10& p10,
                          P11& p11, P12& p12, P13& p13, P14& p14) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 14 with the values of the arguments 'p1' - 'p14'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple14<P1&, P2&, P3&, P4&, P5&, P6&, P7&, P8&,
                                  P9&, P10&, P11&, P12&, P13&, P14&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
        return invoke(args);
    }

    template <class P1, class P2, class P3, class P4, class P5, class P6,
              class P7, class P8, class P9, class P10, class P11, class P12,
              class P13, class P14>
    ResultType operator()(P1 const& p1, P2 const& p2, P3 const& p3,
                          P4 const& p4, P5 const& p5, P6 const& p6,
                          P7 const& p7, P8 const& p8, P9 const& p9,
                          P10 const& p10, P11 const& p11,
                          P12 const& p12, P13 const& p13,
                          P14 const& p14) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 14 with the values of the arguments 'p1' - 'p14'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple14<P1 const&, P2 const&, P3 const&, P4 const&,
                                  P5 const&, P6 const&, P7 const&, P8 const&,
                                  P9 const&,  P10 const&, P11 const&,
                                  P12 const&, P13 const&, P14 const&> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
        return invoke(args);
    }
};

                          // ============================
                          // class bdef_Bind_ImplExplicit
                          // ============================

template <class RET, class FUNC, class LIST>
class bdef_Bind_ImplExplicit {
    // This class implements the storage and functionality required for a
    // binder that invokes an object of type 'FUNC' with a list of invocation
    // parameters of type 'LIST'.  The return type of the invocation is
    // determined by a combination of type 'RET' and 'FUNC'.  This
    // implementation is preferred to the more generic 'bdef_Bind_Impl' above,
    // because it allows to match the signature more closely.  However, this
    // implementation can only be used when the type of binder is "explicit"
    // (see below).  Note that it does not yet allow to cast the bound
    // arguments to their respective argument type at binding time; the bound
    // arguments are still converted to the signature at invocation time.
    //
    // A binder is explicit if:
    //
    //: 1 The function signature can be determined completely (return and
    //:   argument types, e.g., free or member function pointers).
    //: 2 There are no duplicate references to the same placeholder nor any
    //:   nested 'bdef_Bind' objects in the bound arguments.
    //: 3 There are no ellipsis argument in the signature of the function.
    //
    // Note that this class is an implementation detail of 'bdef_Bind'.  Do not
    // use this class outside this component.

    // PRIVATE TYPES
    typedef bdef_Bind_FuncTraits<RET,FUNC>                 Traits;
    typedef typename Traits::Type                          FuncType;
    typedef bdef_Bind_Invoker<typename Traits::ResultType,
                              LIST::LENGTH>                Invoker;
    typedef bslmf_Tag<Traits::HAS_POINTER_SEMANTICS>       HasPointerSemantics;
    typedef typename Traits::ArgumentList                  Args;
    typedef bdef_Bind_CalcParameterMask<LIST>              ParamMask;

    enum {
        OFFSET = (int)Traits::PARAM_OFFSET    // 1 for member functions, 0 else
    };

    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX1, OFFSET>::Type   P1;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX2, OFFSET>::Type   P2;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX3, OFFSET>::Type   P3;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX4, OFFSET>::Type   P4;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX5, OFFSET>::Type   P5;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX6, OFFSET>::Type   P6;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX7, OFFSET>::Type   P7;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX8, OFFSET>::Type   P8;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX9, OFFSET>::Type   P9;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX10, OFFSET>::Type  P10;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX11, OFFSET>::Type  P11;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX12, OFFSET>::Type  P12;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX13, OFFSET>::Type  P13;
    typedef typename bdef_Bind_MapParameter<FUNC, Args,
              (int)ParamMask::PARAMINDEX14, OFFSET>::Type  P14;

    // PRIVATE INSTANCE DATA
    bslalg_ConstructorProxy<typename Traits::WrapperType>  d_func;
    LIST                                                   d_list;

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdef_Bind_ImplExplicit,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // PUBLIC TYPES
    typedef typename Traits::ResultType                    ResultType;
        // The return type of this binder object.

  private:
    // PRIVATE ACCESSORS
    template <class ARGS>
    ResultType invokeImpl(ARGS& arguments, bslmf_Tag<0>) const
        // Invoke the bound functor using the invocation parameters provided at
        // construction of this 'bdef_Bind' object.  Substituting place-holders
        // for their respective values in the specified 'arguments'.  The
        // 'bslmf_Tag' is only used for overloading resolution - indicating
        // whether 'd_func' has pointer semantics or not.
    {
        Invoker invoker;
        return invoker.invoke(&d_func.object(), &d_list, arguments);
    }

    template <class ARGS>
    inline ResultType invokeImpl(ARGS& arguments, bslmf_Tag<1>) const
        // Invoke the bound functor using the invocation parameters provided at
        // construction of this 'bdef_Bind' object.  Substituting place-holders
        // for their respective values in the specified 'arguments'.  The
        // 'bslmf_Tag' is only used for overloading resolution - indicating
        // whether 'd_func' has pointer semantics or not.
    {
        Invoker invoker;
        return invoker.invoke(&(*d_func.object()), &d_list, arguments);
    }

  public:
    // CREATORS
    bdef_Bind_ImplExplicit(typename bslmf_ForwardingType<FUNC>::Type func,
                           LIST const&                               list,
                           bslma_Allocator                          *allocator)
        // Construct a 'bdef_Bind_Impl' object bound to the specified
        // invocable object 'func' using the invocation parameters specified
        // in 'list'.
    : d_func(func, allocator)
    , d_list(list, allocator)
    {
    }

    bdef_Bind_ImplExplicit(const bdef_Bind_ImplExplicit&  other,
                           bslma_Allocator               *allocator)
    : d_func(other.d_func, allocator)
    , d_list(other.d_list, allocator)
    {
    }

    // ACCESSORS
    template <class ARGS>
    ResultType invoke(ARGS& arguments) const
        // Invoke the bound functor using the invocation parameters provided at
        // construction of this 'bdef_Bind' object.  Substituting place-holders
        // for their respective values in the specified 'arguments'.
    {
        return invokeImpl(arguments, HasPointerSemantics());
    }

    ResultType operator()() const
        // Invoke the bound functor using only the invocation parameters
        // provided at construction of this 'bdef_Bind' object and return
        // the result.
    {
        typedef bdef_Bind_Tuple0 ARGS; ARGS args;
        return invoke(args);
    }

    ResultType operator()(P1 p1)  const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for argument 1 with the value of the argument 'p1'.  Return the
        // result.
    {

        typedef bdef_Bind_Tuple1<P1> ARGS;  ARGS args(p1);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 and 2 with the value of the arguments 'p1', and 'p2'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple2<P1, P2> ARGS;
        ARGS args(p1, p2);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2  p2, P3  p3) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1, 2, and 3 with the values of the arguments 'p1',
        // 'p2' and 'p3' respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple3<P1, P2, P3> ARGS;
        ARGS args(p1, p2, p3);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2  p2, P3 p3, P4 p4) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 4 with the values of the arguments 'p1' - 'p4'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple4<P1, P2, P3, P4> ARGS;
        ARGS args(p1, p2, p3, p4);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 5 with the values of the arguments 'p1' - 'p5'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple5<P1, P2, P3, P4, P5> ARGS;
        ARGS args(p1, p2, p3, p4, p5);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 6 with the values of the arguments 'p1' - 'p7'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple6<P1, P2, P3, P4, P5, P6> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                          P7 p7) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 7 with the values of the arguments 'p1' - 'p7'
        // respectively.  Return the result.

    {
        typedef bdef_Bind_Tuple7<P1, P2, P3, P4, P5, P6, P7> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                          P7 p7, P8 p8) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 8 with the values of the arguments 'p1' - 'p8'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple8<P1, P2, P3, P4, P5, P6, P7, P8> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                          P7 p7, P8 p8, P9 p9) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 9 with the values of the arguments 'p1' - 'p9'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple9<P1, P2, P3, P4, P5, P6, P7, P8, P9> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                          P7 p7, P8 p8, P9 p9, P10 p10) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 10 with the values of the arguments 'p1' - 'p10'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple10<P1, P2, P3, P4, P5, P6, P7, P8, P9,
                                  P10> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                          P7 p7, P8 p8, P9 p9, P10 p10, P11 p11) const
        // invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_bind' object, substituting place-holders
        // for arguments 1 - 11 with the values of the arguments 'p1' - 'p11'
        // respectively.  return the result.
    {
        typedef bdef_Bind_Tuple11<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10,
                                  P11> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                          P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,
                          P12 p12) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 12 with the values of the arguments 'p1' - 'p12'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple12<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10,
                                  P11, P12> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                          P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,
                          P12 p12, P13 p13) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 13 with the values of the arguments 'p1' - 'p13'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple13<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10,
                                  P11, P12, P13> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
        return invoke(args);
    }

    ResultType operator()(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,
                          P7 p7, P8 p8, P9 p9, P10 p10, P11 p11,
                          P12 p12, P13 p13, P14 p14) const
        // Invoke the bound functor using the invocation template provided at
        // construction of this 'bdef_Bind' object, substituting place-holders
        // for arguments 1 - 14 with the values of the arguments 'p1' - 'p14'
        // respectively.  Return the result.
    {
        typedef bdef_Bind_Tuple14<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11,
                                  P12, P13, P14> ARGS;
        ARGS args(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
        return invoke(args);
    }
};

                          // ============================
                          // class bdef_Bind_ImplSelector
                          // ============================

template <class RET, class FUNC, class LIST>
struct bdef_Bind_ImplSelector {
    // This utility is used to select the best bind implementation for the
    // given function and invocation template.  'bdef_Bind_ImplExplicit' is
    // selected if the binder is explicit, else 'bdef_Bind_Impl' is selected.
    // A binder is explicit when:
    //
    //: 1 The function signature can be determined completely (return and
    //:   argument types, e.g., free or member function pointers).
    //: 2 There are no duplicate references to the same placeholder nor any
    //:   nested 'bdef_Bind' objects in the bound arguments.
    //: 3 There are no ellipsis argument in the signature of the function.

    enum {
        IS_EXPLICIT = (bdef_Bind_CalcParameterMask<LIST>::IS_EXPLICIT &&
                       bdef_Bind_FuncTraits<RET,FUNC>::IS_EXPLICIT) ? 1 : 0
    };

    typedef typename
        bslmf_If<IS_EXPLICIT,
                 bdef_Bind_ImplExplicit<RET,FUNC,LIST>,
                 bdef_Bind_Impl<RET,FUNC,LIST> >::Type Type;
};

// Implementation note: The following three classes, 'bdef_Bind_FuncTraits',
// 'bdef_Bind_FuncTraitsImp', and 'bdef_Bind_FuncTraitsHasNoEllipsis' are
// presented in the reverse order to eliminate an AIX failure due to the order
// of templates, even if those have been forward-declared.

                          // =======================================
                          // class bdef_Bind_FuncTraitsHasNoEllipsis
                          // =======================================

template <class FUNC>
struct bdef_Bind_FuncTraitsHasNoEllipsis {
    // Determines whether a function type has the ellipsis as an argument.
    // This is needed for making the binder non-explicit in case of a function
    // pointer, reference, or member function pointer.  By default, a function
    // does not take an ellipsis unless specialized below.  This traits class
    // is used in the 'bdef_Bind_FuncTraitsImp' below.  Note that this meta
    // function is not supported on sun studio 8.

    enum {
        VALUE = 1
    };
};

// PARTIAL SPECIALIZATIONS
template <class RET>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(...)> {
    // Specialization for function pointers that return 'RET' and accept one
    // argument which is an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,...)> {
    // Specialization for function pointers that return 'RET' and accept one
    // argument and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,...)> {
    // Specialization for function pointers that return 'RET' and accept two
    // arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,...)> {
    // Specialization for function pointers that return 'RET' and accept three
    // arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,...)> {
    // Specialization for function pointers that return 'RET' and accept four
    // arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4, class A5>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,A5,...)> {
    // Specialization for function pointers that return 'RET' and accept five
    // arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,A5,A6,...)> {
    // Specialization for function pointers that return 'RET' and accept six
    // arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,A5,A6, A7,...)> {
    // Specialization for function pointers that return 'RET' and accept seven
    // arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,A5,A6,A7,
                                                 A8,...)> {
    // Specialization for function pointers that return 'RET' and accept eight
    // arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,
                                                 A9,...)> {
    // Specialization for function pointers that return 'RET' and accept nine
    // arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9, class A10>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                 A10,...)> {
    // Specialization for function pointers that return 'RET' and accept ten
    // arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9, class A10,class A11>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                 A10,A11,...)> {
    // Specialization for function pointers that return 'RET' and accept eleven
    // arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9, class A10, class A11,
          class A12>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                 A10,A11,A12,...)> {
    // Specialization for function pointers that return 'RET' and accept
    // twelve arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9, class A10, class A11,
          class A12, class A13>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                 A10,A11,A12,A13,...)> {
    // Specialization for function pointers that return 'RET' and accept
    // thirteen arguments and an ellipsis.
    enum { VALUE = 0 };
};

template <class RET, class A1, class A2, class A3, class A4, class A5,
          class A6, class A7, class A8, class A9, class A10, class A11,
          class A12, class A13, class A14>
struct bdef_Bind_FuncTraitsHasNoEllipsis<RET (*)(A1,A2,A3,A4,A5,A6,A7,A8,A9,
                                                 A10,A11,A12,A13,A14...)> {
    // Specialization for function pointers that return 'RET' and accept
    // fourteen arguments and an ellipsis.
    enum { VALUE = 0 };
};

                          // ========================
                          // class bdef_FuncTraitsImp
                          // ========================

template <class RET, class FUNC,
          int IS_FUNCTION,
          int IS_FUNCTION_POINTER,
          int IS_MEMBER_FUNCTION_POINTER>
struct bdef_Bind_FuncTraitsImp;
    // This 'struct' provides a mechanism for inferring various traits of the
    // function object type 'FUNC'.  The return type is given by 'RET' unless
    // it is 'bslmf_Nil' in which case it is inferred from 'FUNC'.  The last
    // three parameters 'IS_FUNCTION', 'IS_FUNCTION_POINTER' and
    // 'IS_MEMBER_FUNCTION_POINTER' specify whether 'FUNC' is a free function
    // type (either a reference-to-function or plain function type), a pointer
    // to function type, or a pointer to member function type, respectively.
    //
    // Only specializations of this class should be used (given below).  All
    // specializations define the types and enumerations documented in
    // the 'bdef_Bind_FuncTraits' below.

// PARTIAL SPECIALIZATIONS
template <class RET, class FUNC>
struct bdef_Bind_FuncTraitsImp<RET,FUNC,1,0,0> {
    // Function traits for non-member function types (references or not, but
    // not pointers to function), with explicit return type specification.

    // ENUMERATIONS
    enum {
        IS_EXPLICIT           = bdef_Bind_FuncTraitsHasNoEllipsis<FUNC*>::VALUE
      , PARAM_OFFSET          = 0
      , HAS_POINTER_SEMANTICS = 0
    };

    // PUBLIC TYPES
    typedef FUNC*                                             Type;
    typedef FUNC*                                             WrapperType;
    typedef RET                                               ResultType;
    typedef typename bslmf_FunctionPointerTraits<FUNC*>::ArgumentList
                                                              ArgumentList;
};

template <class RET, class FUNC>
struct bdef_Bind_FuncTraitsImp<RET,FUNC,0,1,0> {
    // Function traits for non-member function pointers, with explicit return
    // type specification.

    // ENUMERATIONS
    enum {
        IS_EXPLICIT           = bdef_Bind_FuncTraitsHasNoEllipsis<FUNC>::VALUE
      , PARAM_OFFSET          = 0
      , HAS_POINTER_SEMANTICS = 0
    };

    // PUBLIC TYPES
    typedef FUNC              Type;
    typedef FUNC              WrapperType;
    typedef RET               ResultType;
    typedef typename bslmf_FunctionPointerTraits<FUNC>::ArgumentList
                              ArgumentList;
};

template <class RET, class FUNC>
struct bdef_Bind_FuncTraitsImp<RET,FUNC,0,0,1> {
    // Function traits for member function pointers, with explicit return type
    // specification.

    // ENUMERATIONS
    enum {
        IS_EXPLICIT           = 1 // we do not support the ellipsis
      , PARAM_OFFSET          = 1
      , HAS_POINTER_SEMANTICS = 0
    };

    // PUBLIC TYPES
    typedef FUNC             Type;
    typedef bdef_MemFn<FUNC> WrapperType;
    typedef RET              ResultType;
    typedef typename bslmf_MemberFunctionPointerTraits<FUNC>::ArgumentList
                             ArgumentList;
};

template <class RET, class FUNC>
struct bdef_Bind_FuncTraitsImp<RET,FUNC,0,0,0> {
    // Function traits for function objects that are passed by value with
    // explicit return type specification.

    // ENUMERATIONS
    enum {
        IS_EXPLICIT           = 0
      , HAS_POINTER_SEMANTICS = bslalg_HasTrait<FUNC,
                                    bslalg_TypeTraitHasPointerSemantics>::VALUE
    };

    // PUBLIC TYPES
    typedef FUNC Type;
    typedef FUNC WrapperType;
    typedef RET  ResultType;
};

template <class RET, class FUNC>
struct bdef_Bind_FuncTraitsImp<RET,FUNC*,0,0,0> {
    // Function traits for objects passed by pointer with explicit return type.

    // ENUMERATIONS
    enum {
        IS_EXPLICIT           = 0
      , HAS_POINTER_SEMANTICS = 1
    };

    // PUBLIC TYPES
    typedef FUNC  Type;
    typedef FUNC *WrapperType;
    typedef RET   ResultType;
};

template <class FUNC>
struct bdef_Bind_FuncTraitsImp<bslmf_Nil,FUNC,1,0,0> {
    // Function traits for non-member function types (references or not, but
    // not pointers to function).  The result type is determined from the
    // function pointer traits.

    // ENUMERATIONS
    enum {
        IS_EXPLICIT           = bdef_Bind_FuncTraitsHasNoEllipsis<FUNC*>::VALUE
      , PARAM_OFFSET          = 0
      , HAS_POINTER_SEMANTICS = 0
    };

    // PUBLIC TYPES
    typedef FUNC                                              Type;
    typedef FUNC*                                             WrapperType;
    typedef typename bslmf_FunctionPointerTraits<FUNC*>::ResultType
                                                              ResultType;
    typedef typename bslmf_FunctionPointerTraits<FUNC*>::ArgumentList
                                                              ArgumentList;
};

template <class FUNC>
struct bdef_Bind_FuncTraitsImp<bslmf_Nil,FUNC,0,1,0> {
    // Function traits for non-member function pointers.  The result type is
    // determined from the function pointer traits.

    // ENUMERATIONS
    enum {
        IS_EXPLICIT           = bdef_Bind_FuncTraitsHasNoEllipsis<FUNC>::VALUE
      , PARAM_OFFSET          = 0
      , HAS_POINTER_SEMANTICS = 0
    };

    // PUBLIC TYPES
    typedef FUNC Type;
    typedef FUNC WrapperType;
    typedef typename bslmf_FunctionPointerTraits<FUNC>::ResultType
                 ResultType;
    typedef typename bslmf_FunctionPointerTraits<FUNC>::ArgumentList
                 ArgumentList;
};

template <class FUNC>
struct bdef_Bind_FuncTraitsImp<bslmf_Nil,FUNC,0,0,1> {
    // Function traits for member function pointers.  The result type is
    // determined from the function pointer traits.

    // ENUMERATIONS
    enum {
        IS_EXPLICIT           = 1 // we do not support the ellipsis
      , PARAM_OFFSET          = 1
      , HAS_POINTER_SEMANTICS = 0
    };

    // PUBLIC TYPES
    typedef FUNC             Type;
    typedef bdef_MemFn<FUNC> WrapperType;
    typedef typename bslmf_MemberFunctionPointerTraits<FUNC>::ResultType
                             ResultType;
    typedef typename bslmf_MemberFunctionPointerTraits<FUNC>::ArgumentList
                             ArgumentList;
};

template <class FUNC>
struct bdef_Bind_FuncTraitsImp<bslmf_Nil,FUNC,0,0,0> {
    // Function traits for function objects that are passed by value without
    // explicit result type specification.  The result type is determined
    // to the 'FUNC::ResultType'.

    // ENUMERATIONS
    enum {
        IS_EXPLICIT           = 0
      , HAS_POINTER_SEMANTICS = bslalg_HasTrait<FUNC,
                                    bslalg_TypeTraitHasPointerSemantics>::VALUE
    };

    // PUBLIC TYPES
    typedef FUNC                      Type;
    typedef FUNC                      WrapperType;
    typedef typename FUNC::ResultType ResultType;
};

template <class FUNC>
struct bdef_Bind_FuncTraitsImp<bslmf_Nil,FUNC*,0,0,0> {
    // Function traits for objects passed by pointer with no explicit
    // return type.  The object is assumed to have a 'ResultType' type
    // definition.

    // ENUMERATIONS
    enum {
        IS_EXPLICIT           = 0
      , HAS_POINTER_SEMANTICS = 1
    };

    // PUBLIC TYPES
    typedef FUNC                      Type;
    typedef FUNC*                     WrapperType;
    typedef typename FUNC::ResultType ResultType;
};

                          // =====================
                          // class bdef_FuncTraits
                          // =====================

template <class RET, class FUNC>
struct bdef_Bind_FuncTraits
: public bdef_Bind_FuncTraitsImp<RET,FUNC,
                             (int)bslmf_IsFunctionPointer<FUNC*>::VALUE,
                             (int)bslmf_IsFunctionPointer<FUNC>::VALUE,
                             (int)bslmf_IsMemberFunctionPointer<FUNC>::VALUE>
{
    // This 'struct' provides various traits of the functor type 'FUNC'
    // documented below.  If 'RET' is 'bslmf_Nil', then the return type is
    // inferred by using either 'bslmf_FunctionPointerTraits',
    // 'bslmf_MemberFunctionPointerTraits', or 'FUNC::ResultType' as
    // appropriate.
    //..
    // // ENUMERATIONS
    // enum {
    //     IS_EXPLICIT           // An invocable object is explicit if ...
    //                           // (see 'bdef_Bind_ImplExplicit')
    //
    //   , PARAM_OFFSET          // Offset for calculating evaluation of
    //                           // placeholder values at invocation.
    //                           // Will be 1 for member function pointers,
    //                           // and 0 otherwise.
    //
    //   , HAS_POINTER_SEMANTICS // Whether the bound functor should be invoked
    //                           // by address or by value.
    // };
    //
    // // PUBLIC TYPES
    // typedef ... Type;         // type of the bound function object
    // typedef ... WrapperType;  // type of the bound object wrapper
    // typedef ... ResultType;   // return type of the bound function object
    // typedef ... ArgumentList; // signature of the bound function object
    //..
    // The table below gives a summary of the values set by the specializations
    // of 'bdef_Bind_FuncTraitsImp', where 'X' represents a class type and 'T'
    // represents the return type (given explicitly by 'RET', or deduced by the
    // nested 'ResultType' of 'X' or of the appropriate traits when 'RET' is
    // specified as 'bslmf_Nil').
    //
    // RET        FUNC     IS _FN IS_FNP IS_MEM IS_EXPLICIT OFFSET PTR_SEM
    // ---------  -------  ------ ------ ------ ----------- ------ -------
    // T          T(...)        1      0      0          1^      0       0
    // T          T(*)(...)     0      1      0          1^      0       0
    // T          T(X::*)(...)  0      0      1          1       1       0
    // T          X             0      0      0          0       A       B
    // T          X*            0      0      0          0       A       1
    // bslmf_Nil  T(...)        1      0      0          1^      0       0
    // bslmf_Nil  T(*)(..)      0      1      0          1^      0       0
    // bslmf_Nil  T(X::*)(...)  0      0      1          1       1       0
    // bslmf_Nil  X             0      0      0          0       A       B
    // bslmf_Nil  X*            0      0      0          0       A       1
    //
    // A: undefined, since unused by 'bdef_Bin_Impl', which is used for
    //    non-explicit binders.
    // B: as determined by the value of
    //    'bslalg_TypeTraits<X, bslalg_TypeTraitHasPointerSemantics>'.
    // ^: in the (rare) case where the 'FUNC' type has an ellipsis argument,
    //    the IS_EXPLICIT will be set to 0.  Note that we *only* support the
    //    ellipsis in non-member functions.
};

                           // ============================
                           // class bdef_Bind_ArgumentMask
                           // ============================

template <class TYPE>
struct bdef_Bind_ArgumentMask {
    // This utility is used to calculate an integer mask for bound arguments.
    // The bit position of the mask corresponding to the index position of the
    // place holder is set to 1.  The general definition does not define a
    // mask.

    enum {
        VALUE = 0
    };
};

template <int INDEX>
struct bdef_Bind_ArgumentMask<bdef_PlaceHolder<INDEX> > {
    // This specialization of 'bdef_Bind_ArgumentMask' defines a mask for the
    // place-holder at the specified 'INDEX' of bound arguments.  The bit
    // position at 'INDEX' is set to 1.

    enum {
        VALUE = 1 << INDEX
    };
};

template <class RET, class FUNC, class LIST>
struct bdef_Bind_ArgumentMask<bdef_Bind<RET, FUNC, LIST> > {
    // This specialization of 'bdef_Bind_ArgumentMask' defines a mask for a
    // 'bdef_Bind' object passed recursively as a bound argument.  The value is
    // not important, as long as it is out of range.  Note that '1 << 30' would
    // be equally valid, but can lead to an overflow in constant expression
    // with 'bdef_Bind_CalcParameterMask::PARAM_MASK2' below (obtained by
    // adding the masks of the bound arguments together) when there are more
    // than 3 nested binders (this is unfortunately an error with the GNU
    // compiler).

    enum {
        VALUE = 1 << 24
    };
};

                          // ==============================
                          // class bdef_Bind_ArgumentNumber
                          // ==============================

template <class TYPE>
struct bdef_Bind_ArgumentNumber {
    // This template is used to calculate a compound numeric value of which
    // parameters must be forwarded to a bound functor.  The general definition
    // defines a numeric value of zero.

    enum {
        VALUE = 0
    };
};

template <int INDEX>
struct bdef_Bind_ArgumentNumber<bdef_PlaceHolder<INDEX> > {
    // This specialization of 'bdef_Bind_ArgumentNumber' defines a numeric
    // value for the place-holder at the specified 'INDEX' of the bound
    // arguments.

    enum {
        VALUE = INDEX
    };
};

                       // ==================================
                       // class bdef_Bind_MemFnObjectWrapper
                       // ==================================

template <class TYPE>
class bdef_Bind_MemFnObjectWrapper {
    // This local class is used to wrap an object used to invoke a member
    // functions bound to a 'bdef_Bind' binder.  In explicit binding, the
    // argument types of operator() are determined during 'bdef_Bind'
    // instantiation.  If a member function is bound to the binder, and the
    // object to invoke the member function on is passed as an invocation
    // argument (instead of a bound argument), the exact type of the object -
    // whether it is a pointer or reference - cannot be determined based on the
    // method signature.  This wrapper serve as an intermediate type to wrap
    // around the object pointer or reference.  This class is declared to have
    // pointer semantics, such that 'bdef_MemFn' will properly dereference the
    // object to invoke the member function.

    // DATA
    TYPE *d_object;  // held, not owned

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(bdef_Bind_MemFnObjectWrapper,
                                  bslalg_TypeTraitHasPointerSemantics,
                                  bslalg_TypeTraitBitwiseMoveable);
  public:
    // CREATORS
    bdef_Bind_MemFnObjectWrapper(TYPE  *object) : d_object(object) {}
    bdef_Bind_MemFnObjectWrapper(TYPE&  object) : d_object(&object) {}
        // Implicitly converts the specified object (of parameterized 'TYPE')
        // to a 'bdef_Bind_MemFnObjectWrapper'.

    // ACCESSORS
    TYPE& operator*() const { return *d_object; }
        // Returns a reference to the object (of parameterized 'TYPE') pointed
        // to by this 'bdef_Bind_MemFnObjectWrapper'.

};

                           // ============================
                           // class bdef_Bind_MapParameter
                           // ============================

template <class FUNC, class ARGS, int INDEX, int OFFSET>
struct bdef_Bind_MapParameter {
    // This meta-function is used to select the type at 'OFFSET' positions of
    // the 'INDEX' position in an argument list type 'ARGS', which is of type
    // 'bslmf_TypeList'.  Note that this class is used by
    // 'bdef_Bind_ImplExplicit'.

    // PUBLIC TYPES
    typedef typename
        bslmf_ForwardingType<
            typename bslmf_TypeListTypeOf<INDEX - OFFSET,
                        ARGS, const bslmf_AnyType&>::TypeOrDefault>::Type Type;
};

template <class FUNC, class ARGS>
struct bdef_Bind_MapParameter<FUNC, ARGS, 1, 1> {
    // This partial specialization of 'bdef_Bind_MapParameter' is intended for
    // use with member functions.  The 'bdef_Bind_MemFnObjectWrapper' will
    // properly wrap around the class object reference or pointer, such that
    // either can be used to invoke the member function.

    // PUBLIC TYPES
    typedef bslmf_MemberFunctionPointerTraits<FUNC> Traits;
    typedef typename Traits::ClassType              ObjectType;
    typedef typename
        bslmf_ForwardingType<const bdef_Bind_MemFnObjectWrapper<ObjectType>& >
                                                                   ::Type Type;
};

template <class FUNC, class ARGS, int OFFSET>
struct bdef_Bind_MapParameter<FUNC, ARGS, 0, OFFSET> {
    // This partial specialization of 'bdef_Bind_MapParameter' does not specify
    // any type in the 'ARGS' list.  It is used for parameters that should be
    // ignored.

    // PUBLIC TYPES
    typedef const bslmf_AnyType& Type;
};

                           // =================================
                           // class bdef_Bind_CalcParameterMask
                           // =================================

// The following macro is used to compute the index of place holders within a
// parameter list and its corresponding mapping to the place holder values.
// For example:
//..
//  bdef_BindUtil::bind(&foo, _1, "abc", _2, _5);
//..
// Will have the following values:
//..
//  bind time index    invocation time index
//  ---------------    ---------------------
//      PARAM1                   1
//      PARAM2                   0
//      PARAM3                   2
//      PARAM4                   5
//..
// The 'BDEF_BIND_PARAMINDEX' macro will compute the following:
//..
//  BDEF_BIND_PARAMINDEX(1) = 1
//  BDEF_BIND_PARAMINDEX(2) = 3
//  BDEF_BIND_PARAMINDEX(3) = 0
//  BDEF_BIND_PARAMINDEX(4) = 0
//  BDEF_BIND_PARAMINDEX(5) = 4
//..
// The index is used to deduce the appropriate invocation argument type for the
// explicit binder.

#define BDEF_BIND_PARAMINDEX(N) ( \
(PARAM1  == N ? 1  : 0) + \
(PARAM2  == N ? 2  : 0) + \
(PARAM3  == N ? 3  : 0) + \
(PARAM4  == N ? 4  : 0) + \
(PARAM5  == N ? 5  : 0) + \
(PARAM6  == N ? 6  : 0) + \
(PARAM7  == N ? 7  : 0) + \
(PARAM8  == N ? 8  : 0) + \
(PARAM9  == N ? 9  : 0) + \
(PARAM10 == N ? 10 : 0) + \
(PARAM11 == N ? 11 : 0) + \
(PARAM12 == N ? 12 : 0) + \
(PARAM13 == N ? 13 : 0) + \
(PARAM14 == N ? 14 : 0))

template <class LIST>
struct bdef_Bind_CalcParameterMask {
    // This meta-function is used to calculate a mask that indicates which
    // parameters in 'LIST' are place-holders and which ones are literal
    // values.  It is used by 'bdef_Bind_ImplExplicit'.

    typedef typename LIST::template TypeOf< 1>::TypeOrDefault  Type1;
    typedef typename LIST::template TypeOf< 2>::TypeOrDefault  Type2;
    typedef typename LIST::template TypeOf< 3>::TypeOrDefault  Type3;
    typedef typename LIST::template TypeOf< 4>::TypeOrDefault  Type4;
    typedef typename LIST::template TypeOf< 5>::TypeOrDefault  Type5;
    typedef typename LIST::template TypeOf< 6>::TypeOrDefault  Type6;
    typedef typename LIST::template TypeOf< 7>::TypeOrDefault  Type7;
    typedef typename LIST::template TypeOf< 8>::TypeOrDefault  Type8;
    typedef typename LIST::template TypeOf< 9>::TypeOrDefault  Type9;
    typedef typename LIST::template TypeOf<10>::TypeOrDefault  Type10;
    typedef typename LIST::template TypeOf<11>::TypeOrDefault  Type11;
    typedef typename LIST::template TypeOf<12>::TypeOrDefault  Type12;
    typedef typename LIST::template TypeOf<13>::TypeOrDefault  Type13;
    typedef typename LIST::template TypeOf<14>::TypeOrDefault  Type14;

    enum {
        PARAM1  = bdef_Bind_ArgumentNumber<Type1 >::VALUE
      , PARAM2  = bdef_Bind_ArgumentNumber<Type2 >::VALUE
      , PARAM3  = bdef_Bind_ArgumentNumber<Type3 >::VALUE
      , PARAM4  = bdef_Bind_ArgumentNumber<Type4 >::VALUE
      , PARAM5  = bdef_Bind_ArgumentNumber<Type5 >::VALUE
      , PARAM6  = bdef_Bind_ArgumentNumber<Type6 >::VALUE
      , PARAM7  = bdef_Bind_ArgumentNumber<Type7 >::VALUE
      , PARAM8  = bdef_Bind_ArgumentNumber<Type8 >::VALUE
      , PARAM9  = bdef_Bind_ArgumentNumber<Type9 >::VALUE
      , PARAM10 = bdef_Bind_ArgumentNumber<Type10>::VALUE
      , PARAM11 = bdef_Bind_ArgumentNumber<Type11>::VALUE
      , PARAM12 = bdef_Bind_ArgumentNumber<Type12>::VALUE
      , PARAM13 = bdef_Bind_ArgumentNumber<Type13>::VALUE
      , PARAM14 = bdef_Bind_ArgumentNumber<Type14>::VALUE
        // Individual place-holder indices.  For each type that is a valid
        // place-holder, 'PARAM<N>' will be set to the index of the
        // corresponding argument.  For non-place-holder types, 'PARAM<N>' will
        // be 0.  For nested 'bdef_Bind' types, the out-of-range value 31 will
        // be used.

      , PARAM_MASK = bdef_Bind_ArgumentMask<Type1 >::VALUE |
                     bdef_Bind_ArgumentMask<Type2 >::VALUE |
                     bdef_Bind_ArgumentMask<Type3 >::VALUE |
                     bdef_Bind_ArgumentMask<Type4 >::VALUE |
                     bdef_Bind_ArgumentMask<Type5 >::VALUE |
                     bdef_Bind_ArgumentMask<Type6 >::VALUE |
                     bdef_Bind_ArgumentMask<Type7 >::VALUE |
                     bdef_Bind_ArgumentMask<Type8 >::VALUE |
                     bdef_Bind_ArgumentMask<Type9 >::VALUE |
                     bdef_Bind_ArgumentMask<Type10>::VALUE |
                     bdef_Bind_ArgumentMask<Type11>::VALUE |
                     bdef_Bind_ArgumentMask<Type12>::VALUE |
                     bdef_Bind_ArgumentMask<Type13>::VALUE |
                     bdef_Bind_ArgumentMask<Type14>::VALUE
         // Mask of which parameters are place-holders.

       , PARAM_MASK2 = bdef_Bind_ArgumentMask<Type1 >::VALUE +
                       bdef_Bind_ArgumentMask<Type2 >::VALUE +
                       bdef_Bind_ArgumentMask<Type3 >::VALUE +
                       bdef_Bind_ArgumentMask<Type4 >::VALUE +
                       bdef_Bind_ArgumentMask<Type5 >::VALUE +
                       bdef_Bind_ArgumentMask<Type6 >::VALUE +
                       bdef_Bind_ArgumentMask<Type7 >::VALUE +
                       bdef_Bind_ArgumentMask<Type8 >::VALUE +
                       bdef_Bind_ArgumentMask<Type9 >::VALUE +
                       bdef_Bind_ArgumentMask<Type10>::VALUE +
                       bdef_Bind_ArgumentMask<Type11>::VALUE +
                       bdef_Bind_ArgumentMask<Type12>::VALUE +
                       bdef_Bind_ArgumentMask<Type13>::VALUE +
                       bdef_Bind_ArgumentMask<Type14>::VALUE
        // Mask of which parameters are place-holder calculated by
        // addition rather the by ORing.  If the given place-holder
        // is used for multiple arguments, the result of mask will be
        // different from the ORed value above.

      , IS_EXPLICIT = (PARAM_MASK == PARAM_MASK2 ?
                            (PARAM_MASK < (1<<15) ? 1 : 0) : 0)
        // Indicates if the types defined in 'LIST' are explicit.
        // The LIST is said to be explicit if there are no duplicate
        // references to the same place-holder, and if there is no
        // nested 'bdef_Bind' objects.  Note that even if 'LIST' is
        // explicit, the binding may still not be explicit (e.g., for function
        // objects that could have multiple signatures, or for functions that
        // take an ellipsis).  See the documentation for this component for
        // more information.

      , PARAMINDEX1  = BDEF_BIND_PARAMINDEX( 1)
      , PARAMINDEX2  = BDEF_BIND_PARAMINDEX( 2)
      , PARAMINDEX3  = BDEF_BIND_PARAMINDEX( 3)
      , PARAMINDEX4  = BDEF_BIND_PARAMINDEX( 4)
      , PARAMINDEX5  = BDEF_BIND_PARAMINDEX( 5)
      , PARAMINDEX6  = BDEF_BIND_PARAMINDEX( 6)
      , PARAMINDEX7  = BDEF_BIND_PARAMINDEX( 7)
      , PARAMINDEX8  = BDEF_BIND_PARAMINDEX( 8)
      , PARAMINDEX9  = BDEF_BIND_PARAMINDEX( 9)
      , PARAMINDEX10 = BDEF_BIND_PARAMINDEX(10)
      , PARAMINDEX11 = BDEF_BIND_PARAMINDEX(11)
      , PARAMINDEX12 = BDEF_BIND_PARAMINDEX(12)
      , PARAMINDEX13 = BDEF_BIND_PARAMINDEX(13)
      , PARAMINDEX14 = BDEF_BIND_PARAMINDEX(14)
    };
};

#undef BDEF_BIND_PARAMINDEX

                           // ===========================
                           // class bdef_Bind_BoundTuple*
                           // ===========================

template <class A1, class A2>
struct bdef_Bind_BoundTuple2 : public bslmf_TypeList2<A1,A2>
{
    // This 'struct' stores a list of two arguments.  It does *not* use the
    // const-forwarding type of its argument, unlike 'bdef_Bind_Tuple2'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1> d_a1;
    bdef_Bind_BoundTupleValue<A2> d_a2;

    // CREATORS
    bdef_Bind_BoundTuple2(const bdef_Bind_BoundTuple2<A1,A2>& orig,
                          bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    {
    }

    bdef_Bind_BoundTuple2(A1 const& a1, A2 const& a2,
                          bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    {
    }
};

template <class A1, class A2, class A3>
struct bdef_Bind_BoundTuple3 : public bslmf_TypeList3<A1,A2,A3>
{
    // This 'struct' stores a list of three arguments.  It does *not* use the
    // const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple3'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1> d_a1;
    bdef_Bind_BoundTupleValue<A2> d_a2;
    bdef_Bind_BoundTupleValue<A3> d_a3;

    // CREATORS
    bdef_Bind_BoundTuple3(const bdef_Bind_BoundTuple3<A1,A2,A3>& orig,
                          bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    {
    }

    bdef_Bind_BoundTuple3(A1 const& a1, A2 const& a2, A3 const& a3,
                          bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4>
struct bdef_Bind_BoundTuple4 : public bslmf_TypeList4<A1,A2,A3,A4>
{
    // This 'struct' stores a list of four arguments.  It does *not* use the
    // const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple4'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1> d_a1;
    bdef_Bind_BoundTupleValue<A2> d_a2;
    bdef_Bind_BoundTupleValue<A3> d_a3;
    bdef_Bind_BoundTupleValue<A4> d_a4;

    // CREATORS
    bdef_Bind_BoundTuple4(const bdef_Bind_BoundTuple4<A1,A2,A3,A4>& orig,
                          bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    {
    }

    bdef_Bind_BoundTuple4(A1 const& a1, A2 const& a2, A3 const& a3,
                          A4 const& a4, bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5>
struct bdef_Bind_BoundTuple5 : public bslmf_TypeList5<A1,A2,A3,A4,A5>
{
    // This 'struct' stores a list of five arguments.  It does *not* use the
    // const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple5'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1> d_a1;
    bdef_Bind_BoundTupleValue<A2> d_a2;
    bdef_Bind_BoundTupleValue<A3> d_a3;
    bdef_Bind_BoundTupleValue<A4> d_a4;
    bdef_Bind_BoundTupleValue<A5> d_a5;

    // CREATORS
    bdef_Bind_BoundTuple5(const bdef_Bind_BoundTuple5<A1,A2,A3,A4,A5>& orig,
                          bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    , d_a5(orig.d_a5,allocator)
    {
    }

    bdef_Bind_BoundTuple5(A1 const& a1, A2 const& a2, A3 const& a3,
                          A4 const& a4, A5 const& a5,
                          bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    , d_a5(a5,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6>
struct bdef_Bind_BoundTuple6 : public bslmf_TypeList6<A1,A2,A3,A4,A5,A6>
{
    // This 'struct' stores a list of six arguments.  It does *not* use the
    // const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple6'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1> d_a1;
    bdef_Bind_BoundTupleValue<A2> d_a2;
    bdef_Bind_BoundTupleValue<A3> d_a3;
    bdef_Bind_BoundTupleValue<A4> d_a4;
    bdef_Bind_BoundTupleValue<A5> d_a5;
    bdef_Bind_BoundTupleValue<A6> d_a6;

    // CREATORS
    bdef_Bind_BoundTuple6(const bdef_Bind_BoundTuple6<A1,A2,A3,A4,A5,A6>&
                                           orig,
                          bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    , d_a5(orig.d_a5,allocator)
    , d_a6(orig.d_a6,allocator)
    {
    }

    bdef_Bind_BoundTuple6(A1 const& a1, A2 const& a2, A3 const& a3,
                          A4 const& a4, A5 const& a5, A6 const& a6,
                          bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    , d_a5(a5,allocator)
    , d_a6(a6,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
struct bdef_Bind_BoundTuple7 : public bslmf_TypeList7<A1,A2,A3,A4,A5,A6,A7>
{
    // This 'struct' stores a list of seven arguments.  It does *not* use the
    // const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple7'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1> d_a1;
    bdef_Bind_BoundTupleValue<A2> d_a2;
    bdef_Bind_BoundTupleValue<A3> d_a3;
    bdef_Bind_BoundTupleValue<A4> d_a4;
    bdef_Bind_BoundTupleValue<A5> d_a5;
    bdef_Bind_BoundTupleValue<A6> d_a6;
    bdef_Bind_BoundTupleValue<A7> d_a7;

    // CREATORS
    bdef_Bind_BoundTuple7(const bdef_Bind_BoundTuple7<A1,A2,A3,A4,A5,A6,A7>&
                                           orig,
                          bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    , d_a5(orig.d_a5,allocator)
    , d_a6(orig.d_a6,allocator)
    , d_a7(orig.d_a7,allocator)
    {
    }

    bdef_Bind_BoundTuple7(A1 const& a1, A2 const& a2, A3 const& a3,
                          A4 const& a4, A5 const& a5, A6 const& a6,
                          A7 const& a7, bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    , d_a5(a5,allocator)
    , d_a6(a6,allocator)
    , d_a7(a7,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8>
struct bdef_Bind_BoundTuple8 : public bslmf_TypeList8<A1,A2,A3,A4,A5,A6,A7,A8>
{
    // This 'struct' stores a list of eight arguments.  It does *not* use the
    // const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple8'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1> d_a1;
    bdef_Bind_BoundTupleValue<A2> d_a2;
    bdef_Bind_BoundTupleValue<A3> d_a3;
    bdef_Bind_BoundTupleValue<A4> d_a4;
    bdef_Bind_BoundTupleValue<A5> d_a5;
    bdef_Bind_BoundTupleValue<A6> d_a6;
    bdef_Bind_BoundTupleValue<A7> d_a7;
    bdef_Bind_BoundTupleValue<A8> d_a8;

    // CREATORS
    bdef_Bind_BoundTuple8(const bdef_Bind_BoundTuple8<A1,A2,A3,A4,A5,A6,A7,A8>&
                                           orig,
                          bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    , d_a5(orig.d_a5,allocator)
    , d_a6(orig.d_a6,allocator)
    , d_a7(orig.d_a7,allocator)
    , d_a8(orig.d_a8,allocator)
    {
    }

    bdef_Bind_BoundTuple8(A1 const& a1, A2 const& a2, A3 const& a3,
                          A4 const& a4, A5 const& a5, A6 const& a6,
                          A7 const& a7, A8 const& a8,
                          bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    , d_a5(a5,allocator)
    , d_a6(a6,allocator)
    , d_a7(a7,allocator)
    , d_a8(a8,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9>
struct bdef_Bind_BoundTuple9 : public bslmf_TypeList9<A1,A2,A3,A4,A5,A6,A7,
                                                      A8,A9>
{
    // This 'struct' stores a list of nine arguments.  It does *not* use the
    // const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple9'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1> d_a1;
    bdef_Bind_BoundTupleValue<A2> d_a2;
    bdef_Bind_BoundTupleValue<A3> d_a3;
    bdef_Bind_BoundTupleValue<A4> d_a4;
    bdef_Bind_BoundTupleValue<A5> d_a5;
    bdef_Bind_BoundTupleValue<A6> d_a6;
    bdef_Bind_BoundTupleValue<A7> d_a7;
    bdef_Bind_BoundTupleValue<A8> d_a8;
    bdef_Bind_BoundTupleValue<A9> d_a9;

    // CREATORS
    bdef_Bind_BoundTuple9(const bdef_Bind_BoundTuple9<A1,A2,A3,A4,A5,A6,A7,
                                                      A8,A9>& orig,
                          bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    , d_a5(orig.d_a5,allocator)
    , d_a6(orig.d_a6,allocator)
    , d_a7(orig.d_a7,allocator)
    , d_a8(orig.d_a8,allocator)
    , d_a9(orig.d_a9,allocator)
    {
    }

    bdef_Bind_BoundTuple9(A1 const& a1, A2 const& a2, A3 const& a3,
                          A4 const& a4, A5 const& a5, A6 const& a6,
                          A7 const& a7, A8 const& a8, A9 const& a9,
                          bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    , d_a5(a5,allocator)
    , d_a6(a6,allocator)
    , d_a7(a7,allocator)
    , d_a8(a8,allocator)
    , d_a9(a9,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10>
struct bdef_Bind_BoundTuple10 : public bslmf_TypeList10<A1,A2,A3,A4,A5,A6,A7,
                                                        A8,A9,A10>
{
    // This 'struct' stores a list of ten arguments.  It does *not* use the
    // const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple10'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1>  d_a1;
    bdef_Bind_BoundTupleValue<A2>  d_a2;
    bdef_Bind_BoundTupleValue<A3>  d_a3;
    bdef_Bind_BoundTupleValue<A4>  d_a4;
    bdef_Bind_BoundTupleValue<A5>  d_a5;
    bdef_Bind_BoundTupleValue<A6>  d_a6;
    bdef_Bind_BoundTupleValue<A7>  d_a7;
    bdef_Bind_BoundTupleValue<A8>  d_a8;
    bdef_Bind_BoundTupleValue<A9>  d_a9;
    bdef_Bind_BoundTupleValue<A10> d_a10;

    // CREATORS
    bdef_Bind_BoundTuple10(const bdef_Bind_BoundTuple10<A1,A2,A3,A4,A5,A6,A7,
                                                        A8,A9,A10>& orig,
                           bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    , d_a5(orig.d_a5,allocator)
    , d_a6(orig.d_a6,allocator)
    , d_a7(orig.d_a7,allocator)
    , d_a8(orig.d_a8,allocator)
    , d_a9(orig.d_a9,allocator)
    , d_a10(orig.d_a10,allocator)
    {
    }

    bdef_Bind_BoundTuple10(A1 const& a1, A2 const& a2, A3 const& a3,
                           A4 const& a4, A5 const& a5, A6 const& a6,
                           A7 const& a7, A8 const& a8, A9 const& a9,
                           A10 const& a10,
                           bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    , d_a5(a5,allocator)
    , d_a6(a6,allocator)
    , d_a7(a7,allocator)
    , d_a8(a8,allocator)
    , d_a9(a9,allocator)
    , d_a10(a10,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11>
struct bdef_Bind_BoundTuple11 : public bslmf_TypeList11<A1,A2,A3,A4,A5,A6,A7,
                                                        A8,A9,A10,A11>
{
    // This 'struct' stores a list of eleven arguments.  It does *not* use the
    // const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple11'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1>  d_a1;
    bdef_Bind_BoundTupleValue<A2>  d_a2;
    bdef_Bind_BoundTupleValue<A3>  d_a3;
    bdef_Bind_BoundTupleValue<A4>  d_a4;
    bdef_Bind_BoundTupleValue<A5>  d_a5;
    bdef_Bind_BoundTupleValue<A6>  d_a6;
    bdef_Bind_BoundTupleValue<A7>  d_a7;
    bdef_Bind_BoundTupleValue<A8>  d_a8;
    bdef_Bind_BoundTupleValue<A9>  d_a9;
    bdef_Bind_BoundTupleValue<A10> d_a10;
    bdef_Bind_BoundTupleValue<A11> d_a11;

    // CREATORS
    bdef_Bind_BoundTuple11(const bdef_Bind_BoundTuple11<A1,A2,A3,A4,A5,A6,A7,
                                                        A8,A9,A10,A11>& orig,
                           bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    , d_a5(orig.d_a5,allocator)
    , d_a6(orig.d_a6,allocator)
    , d_a7(orig.d_a7,allocator)
    , d_a8(orig.d_a8,allocator)
    , d_a9(orig.d_a9,allocator)
    , d_a10(orig.d_a10,allocator)
    , d_a11(orig.d_a11,allocator)
    {
    }

    bdef_Bind_BoundTuple11(A1 const& a1, A2 const& a2, A3 const& a3,
                           A4 const& a4, A5 const& a5, A6 const& a6,
                           A7 const& a7, A8 const& a8, A9 const& a9,
                           A10 const& a10, A11 const& a11,
                           bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    , d_a5(a5,allocator)
    , d_a6(a6,allocator)
    , d_a7(a7,allocator)
    , d_a8(a8,allocator)
    , d_a9(a9,allocator)
    , d_a10(a10,allocator)
    , d_a11(a11,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12>
struct bdef_Bind_BoundTuple12 : public bslmf_TypeList12<A1,A2,A3,A4,A5,A6,A7,
                                                        A8,A9,A10,A11,A12>
{
    // This 'struct' stores a list of twelve arguments.  It does *not* use the
    // const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple12'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1>  d_a1;
    bdef_Bind_BoundTupleValue<A2>  d_a2;
    bdef_Bind_BoundTupleValue<A3>  d_a3;
    bdef_Bind_BoundTupleValue<A4>  d_a4;
    bdef_Bind_BoundTupleValue<A5>  d_a5;
    bdef_Bind_BoundTupleValue<A6>  d_a6;
    bdef_Bind_BoundTupleValue<A7>  d_a7;
    bdef_Bind_BoundTupleValue<A8>  d_a8;
    bdef_Bind_BoundTupleValue<A9>  d_a9;
    bdef_Bind_BoundTupleValue<A10> d_a10;
    bdef_Bind_BoundTupleValue<A11> d_a11;
    bdef_Bind_BoundTupleValue<A12> d_a12;

    // CREATORS
    bdef_Bind_BoundTuple12(const bdef_Bind_BoundTuple12<A1,A2,A3,A4,A5,A6,A7,
                                                      A8,A9,A10,A11,A12>& orig,
                           bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    , d_a5(orig.d_a5,allocator)
    , d_a6(orig.d_a6,allocator)
    , d_a7(orig.d_a7,allocator)
    , d_a8(orig.d_a8,allocator)
    , d_a9(orig.d_a9,allocator)
    , d_a10(orig.d_a10,allocator)
    , d_a11(orig.d_a11,allocator)
    , d_a12(orig.d_a12,allocator)
    {
    }

    bdef_Bind_BoundTuple12(A1 const& a1, A2 const& a2, A3 const& a3,
                           A4 const& a4, A5 const& a5, A6 const& a6,
                           A7 const& a7, A8 const& a8, A9 const& a9,
                           A10 const& a10, A11 const& a11,
                           A12 const& a12, bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    , d_a5(a5,allocator)
    , d_a6(a6,allocator)
    , d_a7(a7,allocator)
    , d_a8(a8,allocator)
    , d_a9(a9,allocator)
    , d_a10(a10,allocator)
    , d_a11(a11,allocator)
    , d_a12(a12,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13>
struct bdef_Bind_BoundTuple13
: public bslmf_TypeList13<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13>
{
    // This 'struct' stores a list of thirteen arguments.  It does *not* use
    // the const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple13'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1>  d_a1;
    bdef_Bind_BoundTupleValue<A2>  d_a2;
    bdef_Bind_BoundTupleValue<A3>  d_a3;
    bdef_Bind_BoundTupleValue<A4>  d_a4;
    bdef_Bind_BoundTupleValue<A5>  d_a5;
    bdef_Bind_BoundTupleValue<A6>  d_a6;
    bdef_Bind_BoundTupleValue<A7>  d_a7;
    bdef_Bind_BoundTupleValue<A8>  d_a8;
    bdef_Bind_BoundTupleValue<A9>  d_a9;
    bdef_Bind_BoundTupleValue<A10> d_a10;
    bdef_Bind_BoundTupleValue<A11> d_a11;
    bdef_Bind_BoundTupleValue<A12> d_a12;
    bdef_Bind_BoundTupleValue<A13> d_a13;

    // CREATORS
    bdef_Bind_BoundTuple13(const bdef_Bind_BoundTuple13<A1,A2,A3,A4,A5,A6,A7,
                                                  A8,A9,A10,A11,A12,A13>& orig,
                           bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    , d_a5(orig.d_a5,allocator)
    , d_a6(orig.d_a6,allocator)
    , d_a7(orig.d_a7,allocator)
    , d_a8(orig.d_a8,allocator)
    , d_a9(orig.d_a9,allocator)
    , d_a10(orig.d_a10,allocator)
    , d_a11(orig.d_a11,allocator)
    , d_a12(orig.d_a12,allocator)
    , d_a13(orig.d_a13,allocator)
    {
    }

    bdef_Bind_BoundTuple13(A1 const& a1, A2 const& a2, A3 const& a3,
                           A4 const& a4, A5 const& a5, A6 const& a6,
                           A7 const& a7, A8 const& a8, A9 const& a9,
                           A10 const& a10, A11 const& a11,
                           A12 const& a12, A13 const& a13,
                           bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    , d_a5(a5,allocator)
    , d_a6(a6,allocator)
    , d_a7(a7,allocator)
    , d_a8(a8,allocator)
    , d_a9(a9,allocator)
    , d_a10(a10,allocator)
    , d_a11(a11,allocator)
    , d_a12(a12,allocator)
    , d_a13(a13,allocator)
    {
    }
};

template <class A1, class A2, class A3, class A4, class A5, class A6, class A7,
          class A8, class A9, class A10, class A11, class A12, class A13,
          class A14>
struct bdef_Bind_BoundTuple14 : public bslmf_TypeList14<A1,A2,A3,A4,A5,A6,A7,
                                                     A8,A9,A10,A11,A12,A13,A14>
{
    // This 'struct' stores a list of fourteen arguments.  It does *not* use
    // the const-forwarding type of its arguments, unlike 'bdef_Bind_Tuple14'
    // which applies that optimization to avoid unnecessary copying.

    // INSTANCE DATA
    bdef_Bind_BoundTupleValue<A1>  d_a1;
    bdef_Bind_BoundTupleValue<A2>  d_a2;
    bdef_Bind_BoundTupleValue<A3>  d_a3;
    bdef_Bind_BoundTupleValue<A4>  d_a4;
    bdef_Bind_BoundTupleValue<A5>  d_a5;
    bdef_Bind_BoundTupleValue<A6>  d_a6;
    bdef_Bind_BoundTupleValue<A7>  d_a7;
    bdef_Bind_BoundTupleValue<A8>  d_a8;
    bdef_Bind_BoundTupleValue<A9>  d_a9;
    bdef_Bind_BoundTupleValue<A10> d_a10;
    bdef_Bind_BoundTupleValue<A11> d_a11;
    bdef_Bind_BoundTupleValue<A12> d_a12;
    bdef_Bind_BoundTupleValue<A13> d_a13;
    bdef_Bind_BoundTupleValue<A14> d_a14;

    // CREATORS
    bdef_Bind_BoundTuple14(const bdef_Bind_BoundTuple14<A1,A2,A3,A4,A5,A6,A7,
                                              A8,A9,A10,A11,A12,A13,A14>& orig,
                           bslma_Allocator *allocator = 0)
    : d_a1(orig.d_a1,allocator)
    , d_a2(orig.d_a2,allocator)
    , d_a3(orig.d_a3,allocator)
    , d_a4(orig.d_a4,allocator)
    , d_a5(orig.d_a5,allocator)
    , d_a6(orig.d_a6,allocator)
    , d_a7(orig.d_a7,allocator)
    , d_a8(orig.d_a8,allocator)
    , d_a9(orig.d_a9,allocator)
    , d_a10(orig.d_a10,allocator)
    , d_a11(orig.d_a11,allocator)
    , d_a12(orig.d_a12,allocator)
    , d_a13(orig.d_a13,allocator)
    , d_a14(orig.d_a14,allocator)
    {
    }

    bdef_Bind_BoundTuple14(A1 const& a1, A2 const& a2, A3 const& a3,
                           A4 const& a4, A5 const& a5, A6 const& a6,
                           A7 const& a7, A8 const& a8, A9 const& a9,
                           A10 const& a10, A11 const& a11,
                           A12 const& a12, A13 const& a13, A14 const& a14,
                           bslma_Allocator *allocator = 0)
    : d_a1(a1,allocator)
    , d_a2(a2,allocator)
    , d_a3(a3,allocator)
    , d_a4(a4,allocator)
    , d_a5(a5,allocator)
    , d_a6(a6,allocator)
    , d_a7(a7,allocator)
    , d_a8(a8,allocator)
    , d_a9(a9,allocator)
    , d_a10(a10,allocator)
    , d_a11(a11,allocator)
    , d_a12(a12,allocator)
    , d_a13(a13,allocator)
    , d_a14(a14,allocator)
    {
    }
};

                          // =======================
                          // class bdef_Bind_Invoker
                          // =======================

#define BDEF_BIND_EVAL(N) \
    bdef_Bind_Evaluator<typename bslmf_TypeListTypeOf<N,LIST>::Type, ARGS>\
           ::eval(args, (list->d_a##N).value())

template <class RET>
struct bdef_Bind_Invoker<RET,0> {
    // Invoker for functions that take zero arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *, ARGS&) const
    {
        return (*func)();
    }
};

template <>
struct bdef_Bind_Invoker<void,0> {
    // Invoker for functions that take zero arguments.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *, ARGS&) const
    {
        (*func)();
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET,1> {
    // Invoker for functions that take one argument.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1));
    }
};

template <>
struct bdef_Bind_Invoker<void,1> {
    // Invoker for functions that take one argument.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET,2> {
    // Invoker for function that take two arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1),
                       BDEF_BIND_EVAL(2));
    }
};

template <>
struct bdef_Bind_Invoker<void,2> {
    // Invoker for functions that take two arguments.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET,3> {
    // Invoker for functions that take three arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2),
                       BDEF_BIND_EVAL(3));
    }
};

template <>
struct bdef_Bind_Invoker<void,3> {
    // Invoker for functions that take three arguments.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 4> {
    // Invoker for functions that take four arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2),
                       BDEF_BIND_EVAL(3), BDEF_BIND_EVAL(4));
    }
};

template <>
struct bdef_Bind_Invoker<void, 4> {
    // Invoker for functions that take four arguments.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                BDEF_BIND_EVAL(4));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 5> {
    // Invoker for functions that take five arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2),
                       BDEF_BIND_EVAL(3), BDEF_BIND_EVAL(4),
                       BDEF_BIND_EVAL(5));
    }
};

template <>
struct bdef_Bind_Invoker<void, 5> {
    // Invoker for functions that take five arguments.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 6> {
    // Invoker for functions that take six arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2),
                       BDEF_BIND_EVAL(3), BDEF_BIND_EVAL(4),
                       BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6));
    }
};

template <>
struct bdef_Bind_Invoker<void, 6> {
    // Invoker for functions that take six arguments.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 7> {
    // Invoker for functions that take seven arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                       BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                       BDEF_BIND_EVAL(7));
    }
};

template <>
struct bdef_Bind_Invoker<void, 7> {
    // Invoker for functions that take seven arguments.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                BDEF_BIND_EVAL(7));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 8> {
    // Invoker for functions that take eight arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2),
                       BDEF_BIND_EVAL(3), BDEF_BIND_EVAL(4),
                       BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                       BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8));
    }
};

template <>
struct bdef_Bind_Invoker<void, 8> {
    // Invoker for functions that take eight arguments.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 9> {
    // Invoker for functions that take nine arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                       BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                       BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8), BDEF_BIND_EVAL(9)
                      );
    }
};

template <>
struct bdef_Bind_Invoker<void, 9> {
    // Invoker for functions that take nine arguments.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8), BDEF_BIND_EVAL(9));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 10> {
    // Invoker for functions that take nine arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                       BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                       BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8), BDEF_BIND_EVAL(9),
                       BDEF_BIND_EVAL(10));
    }
};

template <>
struct bdef_Bind_Invoker<void, 10> {
    // Invoker for functions that take ten arguments and return void.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8), BDEF_BIND_EVAL(9),
                BDEF_BIND_EVAL(10));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 11> {
    // Invoker for functions that take eleven arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2),
                    BDEF_BIND_EVAL(3), BDEF_BIND_EVAL(4),
                    BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                    BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8),
                    BDEF_BIND_EVAL(9), BDEF_BIND_EVAL(10),
                    BDEF_BIND_EVAL(11));
    }
};

template <>
struct bdef_Bind_Invoker<void, 11> {
    // Invoker for functions that take eleven arguments and return void.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2),BDEF_BIND_EVAL(3),
                BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8), BDEF_BIND_EVAL(9),
                BDEF_BIND_EVAL(10), BDEF_BIND_EVAL(11));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 12> {
    // Invoker for functions that take nine arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2),
                    BDEF_BIND_EVAL(3), BDEF_BIND_EVAL(4),
                    BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                    BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8),
                    BDEF_BIND_EVAL(9), BDEF_BIND_EVAL(10),
                    BDEF_BIND_EVAL(11), BDEF_BIND_EVAL(12));
    }
};

template <>
struct bdef_Bind_Invoker<void, 12> {
    // Invoker for functions that take twelve arguments and return void.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
             BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
             BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8), BDEF_BIND_EVAL(9),
             BDEF_BIND_EVAL(10), BDEF_BIND_EVAL(11), BDEF_BIND_EVAL(12));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 13> {
    // Invoker for functions that take thirteen arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2),
                    BDEF_BIND_EVAL(3), BDEF_BIND_EVAL(4),
                    BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                    BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8),
                    BDEF_BIND_EVAL(9), BDEF_BIND_EVAL(10),
                    BDEF_BIND_EVAL(11), BDEF_BIND_EVAL(12),
                    BDEF_BIND_EVAL(13));
    }
};

template <>
struct bdef_Bind_Invoker<void, 13> {
    // Invoker for functions that take thirteen arguments and return void.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                BDEF_BIND_EVAL(4), BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8), BDEF_BIND_EVAL(9),
                BDEF_BIND_EVAL(10),BDEF_BIND_EVAL(11),BDEF_BIND_EVAL(12),
                BDEF_BIND_EVAL(13));
    }
};

template <class RET>
struct bdef_Bind_Invoker<RET, 14> {
    // Invoker for functions that take fourteen arguments.
    template <class FUNC, class LIST, class ARGS>
    RET invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        return (*func)(BDEF_BIND_EVAL(1), BDEF_BIND_EVAL(2),
                       BDEF_BIND_EVAL(3), BDEF_BIND_EVAL(4),
                       BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                       BDEF_BIND_EVAL(7), BDEF_BIND_EVAL(8),
                       BDEF_BIND_EVAL(9), BDEF_BIND_EVAL(10),
                       BDEF_BIND_EVAL(11),BDEF_BIND_EVAL(12),
                       BDEF_BIND_EVAL(13), BDEF_BIND_EVAL(14));
    }
};

template <>
struct bdef_Bind_Invoker<void, 14> {
    // Invoker for functions that take fourteen arguments and return void.
    template <class FUNC, class LIST, class ARGS>
    void invoke(FUNC *func, LIST *list, ARGS& args) const
    {
        (*func)(BDEF_BIND_EVAL(1),  BDEF_BIND_EVAL(2), BDEF_BIND_EVAL(3),
                BDEF_BIND_EVAL(4),  BDEF_BIND_EVAL(5), BDEF_BIND_EVAL(6),
                BDEF_BIND_EVAL(7),  BDEF_BIND_EVAL(8), BDEF_BIND_EVAL(9),
                BDEF_BIND_EVAL(10), BDEF_BIND_EVAL(11),BDEF_BIND_EVAL(12),
                BDEF_BIND_EVAL(13), BDEF_BIND_EVAL(14));
    }
};

#undef BDEF_BIND_EVAL

                          // =========================
                          // class bdef_Bind_Evaluator
                          // =========================

template <class ARG, class LIST>
struct bdef_Bind_Evaluator {
    // This utility provides a default argument evaluator that simply
    // returns whatever value is passed.

    typedef typename  bslmf_ForwardingType<
        typename bslmf_ArrayToConstPointer<ARG>::Type>::Type ArgType;

    static
    ArgType eval(LIST&, ArgType value)
        // Return the specified 'value'.
    {
        return value;
    }
};

template <int INDEX, class LIST>
struct bdef_Bind_Evaluator<bdef_PlaceHolder<INDEX>, LIST> {
    // This partial specialization of 'bdef_Bind_Evaluator' provides an
    // argument evaluator that substitutes the place-holder at position 'INDEX'
    // with the corresponding element in the specified 'LIST'.

    // PUBLIC TYPES
    typedef typename
       bslmf_ForwardingType<typename bslmf_ArrayToConstPointer<
               typename bslmf_TypeListTypeOf<INDEX,LIST>::TypeOrDefault>::Type
       >::Type  ReturnType;

    // CLASS METHODS
    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<1>&)
        // Return the first value from the specified 'list'.
    {
        return *(&list.d_a1.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<2>&)
        // Return the second value from the specified 'list'.
    {
        return *(&list.d_a2.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<3>&)
        // Return the third value from the specified 'list'.
    {
        return *(&list.d_a3.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<4>&)
        // Return the fourth value from the specified 'list'.
    {
        return *(&list.d_a4.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<5>&)
        // Return the fifth value from the specified 'list'.
    {
        return *(&list.d_a5.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<6>&)
        // Return the sixth value from the specified 'list'.
    {
        return *(&list.d_a6.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<7>&)
        // Return the seventh value from the specified 'list'.
    {
        return *(&list.d_a7.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<8>&)
        // Return the eighth value from the specified 'list'.
    {
        return *(&list.d_a8.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<9>&)
        // Return the ninth value from the specified 'list'.
    {
        return *(&list.d_a9.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<10>&)
        // Return the tenth value from the specified 'list'.
    {
        return *(&list.d_a10.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<11>&)
        // Return the eleventh value from the specified 'list'.
    {
        return *(&list.d_a11.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<12>&)
        // Return the twelfth value from the specified 'list'.
    {
        return *(&list.d_a12.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<13>&)
        // Return the thirteenth value from the specified 'list'.
    {
        return *(&list.d_a13.value());
    }

    static
    ReturnType eval(LIST& list, const bdef_PlaceHolder<14>&)
        // Return the fourteenth value from the specified 'list'.
    {
        return *(&list.d_a14.value());
    }
};

template <class RET, class FUNC, class BINDLIST, class LIST>
struct bdef_Bind_Evaluator<bdef_Bind<RET,FUNC,BINDLIST>, LIST> {
    // This utility provides an evaluator for nested 'bdef_Bind' arguments.
    // The 'bdef_Bind' function object is invoked using the provided
    // argument list and the result is returned.

    static
    typename bdef_Bind<RET,FUNC,BINDLIST>::ResultType
    eval(LIST& args, const bdef_Bind<RET,FUNC,BINDLIST>& func)
        // Call the specified 'func' functor with the specified 'args'
        // arguments and return the result.
    {
        return func.invoke(args);
    }
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
