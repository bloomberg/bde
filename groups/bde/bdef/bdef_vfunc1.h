// bdef_vfunc1.h                -*-C++-*-
#ifndef INCLUDED_BDEF_VFUNC1
#define INCLUDED_BDEF_VFUNC1

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide a signature-specific function object (functor).
//
//@CLASSES:
//   bdef_Vfunc1: signature-specific function object (functor)
//
//@AUTHOR: John Lakos (jlakos)
//
//@DEPRECATED: This component should not be used in new code, and will be
// deleted in the near future.  Please see 'bdef_function', 'bdef_bind', etc.
// for alternatives that should be used for all future development.
//
//@DESCRIPTION: This component implements a function object (functor) that
// returns void and takes one argument of arbitrary type; it is invoked in a
// manner similar to that of following free function:
//..
//   void functionObject(arg1);
//..
// The functors in the 'bdef' package are implemented using the envelope-letter
// pattern (see Coplien, Sections 5.5-5.6, pp.  133-179).  Functors are
// objects that implement a function-call operator whose signature
// characterizes the particular functor's type.  A valid functor will contain
// a pointer to an underlying (member or free) function and is invoked in a
// manner similar to that of free functions.  Unlike a function pointer, a
// functor may be created with one or more arbitrary objects (sometimes called
// "user data") to be passed (typically as trailing arguments) to the
// underlying function.  By "pre-binding" particular arguments to the
// underlying function, a functor reduces the number of arguments a caller
// must supply when the functor is invoked.  In this way, function objects can
// be used to coerce functions with extra arguments of arbitrary type into a
// standard calling signature.  Even (non-'static') *member* functions can be
// encapsulated within functors and treated uniformly, simply by supplying the
// appropriate representation object at construction.
//
// The 'bdef_Vfunc1' functor (envelope) holds a polymorphic, reference-counted,
// dynamically-allocated representation (letter), which is loaded by the
// immediate client.  Typical users do not load the functor representation
// directly, but instead rely on a suite of template functions (provided by
// components in the higher-level 'bdefu' package) to allocate and install the
// representation appropriate to (1) the desired function-call-operator
// signature and (2) the supplied user data.  See the 'bdefu_vfunc1' component
// for more information on populating 'bdef_Vfunc1' function objects with
// specific representations.
//
// Note that because each required argument type 'T' is passed by ('const'
// 'T&'), a compiler will generate an error if the user declares a callback
// function taking an argument via a non-'const' reference parameter.  This
// serendipitous side effect is truly a feature, and specifically not a bug,
// as we use references (and occasionally pointers) to pass constant
// arguments, and pointers (exclusively) to pass modifiable arguments.  This
// implementation enforces that rule.
//
///USAGE
///-----
// 'bdef'-style functors provide a type-neutral, exception-safe (and
// *potentially*, but not yet, thread-safe) *properly* *managed* alternative
// to the traditional callback paradigm (where "client data" is placed in a
// single structure whose address is cast to type 'void *').  The following
// example illustrates how functor callbacks can be added to a graphical
// object.
//
// Let's suppose that the implementor of the 'MyGuiButton' class allows his
// client to specify a callback function that should be called whenever a
// button is pressed.  Furthermore, the implementor agrees to provide to the
// callback function one argument at its invocation: a modifiable object of
// type 'MyGuiContext', and a non-modifiable object of type 'MyGuiLocation'.
//..
//   class MyGuiContext {
//       int d_changedFlag;
//
//     public:
//       MyGuiContext():d_changedFlag(0) { }
//       int isChanged() { return d_changedFlag; }
//       void changeState() { ++d_changedFlag; }
//
//       // 'MyGuiContext' implementation
//   };
//
//   class MyGuiLocation {
//     public:
//       MyGuiLocation() { };
//
//       // 'MyGuiLocation' implementation
//   };
//..
// Here is the implementation of the 'MyGuiButton' class:
//..
//   class MyGuiButton  {
//       bdef_Vfunc1<MyGuiContext *, MyGuiLocation> d_callback;
//           // Functor to execute when button is pressed.
//
//     public:
//       MyGuiButton(const bdef_Vfunc1<MyGuiContext *,
//                   MyGuiLocation>& buttonPressCallback);
//           // Create a graphical button object that executes the
//           // specified callback when 'pressButton' is invoked.
//
//       void pressButton(MyGuiContext *context,
//                        const MyGuiLocation& location);
//           // Execute the callback owned by this button object.
//   };
//
//   MyGuiButton::MyGuiButton(const bdef_Vfunc1<MyGuiContext *,
//                            MyGuiLocation>& buttonPressCallback)
//   : d_callback(buttonPressCallback) // Retain a "copy" of the specified
//                                     // functor.
//   {
//   }
//
//   void MyGuiButton::pressButton(MyGuiContext *context,
//                                 const MyGuiLocation& location)
//   {
//       d_callback(context, location);
//           // Execute the contained callback object.
//   }
//..
// The 'context' and 'location' arguments are mandatory for every function
// called at the press of the button (the number '1' in the name of
// 'bdef_Vfunc1' class specifies the number of required arguments).  However,
// we also allow the user of 'MyGuiButton' class to invoke the function with a
// number of additional parameters, as in the 'buttonpressFunction' function
// presented below.
//..
//   static void buttonpressFunction(MyGuiContext *a,
//                                   const MyGuiLocation& b,
//                                   int *invocationCounter)
//       // This function will be invoked by a functor to increment the
//       // specified 'invocationCounter'.
//   {
//           a->changeState();
//           ++*invocationCounter;
//   }
//..
// The 'bdef_Vfunc1<MyGuiContext, MyGuiLocation>' class used in 'MyGuiButton'
// class has a private member of an abstract class 'bdefr_vfunc1', whose
// 'execute' method is called when the functor is being called.  The 'execute'
// method of the 'bdefr_vfunc1' class, the functor invocation operator of the
// 'bdef_Vfunc1' class, and the 'pressButton' method of 'MyGuiButton' class
// have one parameter.  The 'buttonpressFunction' callback function, however,
// has to be called with three arguments.  To achieve the proper calling
// signature the user implements a concrete class derived from 'bdefr_vfunc1'
// as follows:
//..
//   template <class F, class A1, class D1>
//   class FuncRep : public bdefr_Vfunc1<A1> {
//       // This class defines the representation for a function object
//       // (functor), characterized by a function-call operator taking one
//       // argument and returning 'void', that holds a pure procedure (i.e.,
//       // free function, static member function, or functor) taking one
//       // additional trailing argument, and this argument's corresponding
//       // value.
//
//       F  d_f;  // function pointer or function object (functor)
//       D1 d_d1; // first embedded argument
//
//     private:
//       // not implemented
//       FuncRep(const FuncRep<F, A1, D1>&);
//       FuncRep<F, A1, D1>& operator=(const FuncRep<F, A1, D1>&);
//
//     private:
//       ~FuncRep()
//           // Destroy this functor.  Note that destructor can be invoked only
//           // through the static 'deleteObject' method of the base class.
//       {
//       };
//
//     public:
//       // CREATORS
//       FuncRep(const F&         procedure,
//               const D1&        embeddedArg1,
//               bslma_Allocator *basicAllocator)
//           // Create a representation for a function object (functor) taking
//           // one argument and returning 'void', using the specified
//           // 'procedure' (i.e., free function, static member function, or
//           // functor) taking 1 additional trailing argument, and this
//           // argument's specified 'embeddedArg1' value.  Use
//           // the specified 'basicAllocator' to supply memory.
//       : bdefr_Vfunc1<A1>(basicAllocator)
//       , d_f(procedure)
//       , d_d1(embeddedArg1)
//       {
//       };
//
//       // ACCESSORS
//       void execute(const A1& argument1, const A2& argument2) const
//           // Invoke the underlying procedure (free function, static member
//           // function, or functor) with the specified 'argument1' and
//           // 'argument2' followed by the argument value specified at
//           // construction.
//       {
//           d_f(argument1, argument2, d_d1);
//       };
//   };
//..
// Note that the required arguments are passed in at the functor invocation,
// and optional arguments are passed in at the functor initialization.
// Conventionally, a function signature consists of output parameters, and
// then input parameters.  Since here we have two parameter lists (one passed
// in by the caller, and one passed in by the callee) we can follow this
// convention within each list only.  We chose to pass in first the required
// arguments (passed by the caller) and then the optional arguments (supplied
// by the callee).  This order allows a callee (more easily) add parameters at
// the end of the function parameter list.  The number of required parameters
// is a part of the 'MyGuiButton' interface, and hence will never change.
//
// The following code shows how we
//  (1) Create the functor representation-(letter).
//  (2) Create the functor initialized with the representation.
//  (3) Register the functor as a callback with an object of the 'MyGuiButton'
//      class.
//  (4) Invoke the functor.
//..
//   // (1) Create the representation.
//
//   typedef void (*BpFun)(const MyGuiContext *, const MyGuiLocation&, int *);
//   bslma_Allocator *myAllocator = bslma_Default::defaultAllocator();
//
//   int globalCounter = 0;
//
//   bdefr_Vfunc1<MyGuiContext *, MyGuiLocation>  *rep = new(myAllocator)
//       FuncRep<BpFun, MyGuiContext *, MyGuiLocation, int*>
//           (buttonpressFunction, &globalCounter, myAllocator);
//
//   // (2) Create the functor using the representation.
//
//   bdef_Vfunc1<MyGuiContext *, MyGuiLocation> callbackFunctor(rep);
//
//   // (3) Register the functor as a callback.
//
//   MyGuiButton button(callbackFunctor);
//
//   // (4) Use the object.
//
//   MyGuiContext gc;
//   const MyGuiLocation gl;             assert(0 == globalCounter);
//                                       assert(0 == gc.isChanged());
//   button.pressButton(&gc, gl);        assert(1 == globalCounter);
//                                       assert(1 == gc.isChanged());
//   button.pressButton(&gc, gl);        assert(2 == globalCounter);
//                                       assert(2 == gc.isChanged());
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEFR_VFUNC1
#include <bdefr_vfunc1.h>
#endif

namespace BloombergLP {

                        // =================
                        // class bdef_Vfunc1
                        // =================

template <class A1>
class bdef_Vfunc1 {
    // Provide an object that encapsulates a function or method along with zero
    // or more "extra" arguments of arbitrary type (collectively referred to
    // as "user data") to be passed as arguments to the underlying function on
    // invocation.  Clients invoke the underlying function by calling this
    // functor's function-call operator,
    //..
    //     (void) functionObject(arg1)    // functor calling signature
    //..
    // which takes one argument and returns void.  Note that pointer (but not
    // reference) types are supported.

    bdefr_Vfunc1<A1> *d_rep_p;  // polymorphic functor representation

    // NOT IMPLEMENTED
    bool operator==(const bdef_Vfunc1&) const;
    bool operator!=(const bdef_Vfunc1&) const;

    template<class B1>
    bool operator==(const bdef_Vfunc1<B1>&) const;
    template<class B1>
    bool operator!=(const bdef_Vfunc1<B1>&) const;

  public:
    // CREATORS
    bdef_Vfunc1();
        // Create an uninitialized functor.  The resulting functor cannot be
        // invoked, but may subsequently be assigned an invokable value.

    bdef_Vfunc1(bdefr_Vfunc1<A1> *representation);
        // Create a functor that assumes shared ownership of the specified,
        // dynamically allocated, reference-counted representation.  If 0 is
        // specified, the functor is created in a null state (i.e., cannot be
        // invoked), but may subsequently be assigned a value.
        //
        // Note that typical clients of this class do not use this constructor
        // directly, but instead use utilities provided in higher level
        // components (see 'bdefu_vfunc1') to create an appropriate
        // representation and load it into an existing functor.

    bdef_Vfunc1(const bdef_Vfunc1<A1>& functionObject);
        // Create a copy of the specified 'functionObject'.  If
        // 'functionObject' is null, an independent null functor is created.
        // Otherwise, both functors will share ownership of the same internal
        // representation.

    ~bdef_Vfunc1();
        // If this functor holds a non-null representation, decrement the
        // reference count of that internal representation object, and, if the
        // count is now 0, destroy and deallocate that representation (using
        // the static 'deleteObject' method of 'bdefr_Vfunc1' base class).

    // MANIPULATORS
    bdef_Vfunc1<A1>& operator=(const bdef_Vfunc1<A1>& functionObject);
        // Assign the value of the specified 'functionObject' to this object.
        // If 'functionObject' is not null, both functors will share ownership
        // of the same internal representation; otherwise, both functors will
        // be null.  If this object was not initially null (and did not
        // already share ownership of the same internal representation with
        // 'functionObject'), first decrement the reference count of this
        // functor's internal representation object, and, if the count is now
        // 0, destroy and deallocate the representation (using the static
        // 'deleteObject' method of 'bdefr_Vfunc1' base class.

    // ACCESSORS
    void operator()(const A1& argument1) const;
        // Invoke this functor.  The behavior is undefined if this functor is
        // null.

    operator const void *() const;
        // Return 0 if this functor is null, and non-zero otherwise.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

template <class A1>
inline bdef_Vfunc1<A1>::bdef_Vfunc1()
: d_rep_p(0)
{
}

template <class A1>
inline bdef_Vfunc1<A1>::bdef_Vfunc1(bdefr_Vfunc1<A1> *rep)
: d_rep_p(rep)
{
    if (d_rep_p) {
        d_rep_p->increment();
    }
}

template <class A1>
inline bdef_Vfunc1<A1>::bdef_Vfunc1(const bdef_Vfunc1& functor)
: d_rep_p(functor.d_rep_p)
{
    if (d_rep_p) {
        d_rep_p->increment();
    }
}

template <class A1>
inline bdef_Vfunc1<A1>::~bdef_Vfunc1()
{
    if (d_rep_p && 0 == d_rep_p->decrement()) {
        bdefr_Vfunc1<A1>::deleteObject(d_rep_p);
    }
}

template <class A1> inline
bdef_Vfunc1<A1>&
bdef_Vfunc1<A1>::operator=(const bdef_Vfunc1<A1>& rhs)
{
    if (d_rep_p != rhs.d_rep_p) {
        if (d_rep_p && 0 == d_rep_p->decrement()) {
            bdefr_Vfunc1<A1>::deleteObject(d_rep_p);
        }
        d_rep_p = rhs.d_rep_p;
        if (d_rep_p) {
            d_rep_p->increment();
        }
    }
    return *this;
}

template <class A1>
inline void bdef_Vfunc1<A1>::operator()(const A1& argument1) const
{
    d_rep_p->execute(argument1);
}

template <class A1>
inline bdef_Vfunc1<A1>::operator const void *() const
{
    return d_rep_p;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
