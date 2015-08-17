// bdlfxxxu_vfunc4.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLFXXXU_VFUNC4
#define INCLUDED_BDLFXXXU_VFUNC4

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide factory methods for populating 4-arg void function objects.
//
//@CLASSES:
//   bdlfxxxu::Vfunc4: namespace for factory methods loading 4-arg void functors
//
//@AUTHOR: John Lakos (jlakos)
//
//@DEPRECATED: This component should not be used in new code, and will be
// deleted in the near future.  Please see 'bdlf_function', 'bdlf_bind', etc.
// for alternatives that should be used for all future development.
//
//@DESCRIPTION: This component contains a suite of template functions used to
// configure an existing function object (functor) characterized by a
// function-call operator taking four arguments and returning void.  Four
// families of populators are supported.
//..
//   bdlfxxxu::Vfunc4::makeF:    Initialize the functor by embedding a function
//                           (or another functor) and any additional trailing
//                           arguments.
//
//   bdlfxxxu::Vfunc4::makeC:    Initialize the functor by embedding an object, one
//                           of that object's 'const' methods, and any
//                           additional trailing arguments for that method.
//
//   bdlfxxxu::Vfunc4::makeM:    Initialize the functor by embedding THE ADDRESS OF
//                           an object, one of that object's ('const' or
//                           non-'const') methods, and any additional trailing
//                           argments for that method.  Note that it is the
//                           client's responsibility to ensure that the object
//                           remains valid throughout the productive lifetime
//                           of a functor object configured in this way.
//
//   bdlfxxxu::Vfunc4::makeNull: Initialize the functor with an empty function.
//..
// In each of the first three cases, the underlying procedure (i.e., function,
// functor, or method) will have four parameter (matching those of the host
// functor's function-call operator) and perhaps additional parameters, whose
// arguments are specified when the host functor is configured.  Currently,
// underlying procedures with up to nine parameters are supported.
//
// Note, that the argument types are converted to match parameter types at
// functor invocation (and not at initialization); This delayed conversion
// allows for greater flexibility in chosing mandatory function parameter
// types.  The conversion will take place every time the functor is invoked,
// therefore, functors where argument types match exactly will operate more
// efficiently.
//
// Each function comes in two flavors: one that requires the user to specify a
// memory allocator, and one that uses the currently installed default
// allocator.  In the version that requires an allocator, the address of the
// allocator is the first argument specified.
//
///USAGE 1
///-------

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLFXXX_VFUNC4
#include <bdlfxxx_vfunc4.h>
#endif

#ifndef INCLUDED_BDLFXXXI_VFUNC4
#include <bdlfxxxi_vfunc4.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

namespace BloombergLP {

namespace bdlfxxxu {
                        // ==================
                        // class Vfunc4
                        // ==================

struct Vfunc4 {
    // This class provides a namespace for a suite of member template
    // procedures used to populate existing function objects (functors) taking
    // four required argument.

    template <class A1, class A2, class A3, class A4>
    static void makeNull(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle)
        // Initialize the specified four-argument empty functor 'handle'.
        // Optionally specify, as the *first* argument, a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0 or unspecified,
        // the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4Null<A1, A2, A3, A4>(ba));
    }

    template <class A1, class A2, class A3, class A4>
    static void makeNull(bslma::Allocator *basicAllocator,
                         bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle)
        // Initialize the specified four-argument empty functor 'handle'.
        // Optionally specify, as the *first* argument, a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0 or unspecified,
        // the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
             bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4Null<A1, A2, A3, A4>(ba));
    }

    template <class F, class A1, class A2, class A3, class A4>
    static void makeF(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle, F function)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F4<F, A1, A2, A3, A4>(function, ba));
    }

    template <class F, class A1, class A2, class A3, class A4>
    static void makeF(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma::Allocator *ba =
             bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
             new(*ba) bdlfxxxi::Vfunc4F4<F, A1, A2, A3, A4>(function, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class D1>
    static void makeF(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function,
                      const D1&        embeddedArg1)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and the value corresponding to this
        // argument.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F5<F, A1, A2, A3, A4, D1> (
                function, embeddedArg1, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class D1>
    static void makeF(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function,
                      const D1&        embeddedArg1)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and the value corresponding to this
        // argument.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F5<F, A1, A2, A3, A4, D1> (
                function, embeddedArg1, ba));
    }

    template <class F, class A1, class A2, class A3, class A4,
              class D1, class D2>
    static void makeF(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();

        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F6<F, A1, A2, A3, A4, D1, D2>
                (function, embeddedArg1, embeddedArg2, ba));
    }

    template <class F, class A1, class A2, class A3, class A4,
              class D1, class D2>
    static void makeF(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);

        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F6<F, A1, A2, A3, A4, D1, D2>
                (function, embeddedArg1, embeddedArg2, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class D1,
              class D2, class D3>
    static void makeF(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F7<F, A1, A2, A3, A4, D1, D2, D3>
                (function, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class D1,
              class D2, class D3>
    static void makeF(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F7<F, A1, A2, A3, A4, D1, D2, D3>
                (function, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class D1,
              class D2, class D3, class D4>
    static void makeF(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking four
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F8<F, A1, A2, A3, A4, D1, D2, D3, D4>
                (function, embeddedArg1, embeddedArg2, embeddedArg3,
                 embeddedArg4, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class D1,
              class D2, class D3, class D4>
    static void makeF(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking four
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F8<F, A1, A2, A3, A4, D1, D2, D3, D4>
                (function, embeddedArg1, embeddedArg2, embeddedArg3,
                 embeddedArg4, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class D1,
              class D2, class D3, class D4, class D5>
    static void makeF(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4,
                      const D5&        embeddedArg5)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking five
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F9<F, A1, A2, A3, A4, D1, D2, D3, D4, D5>
                (function, embeddedArg1, embeddedArg2, embeddedArg3,
                 embeddedArg4, embeddedArg5, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class D1,
              class D2, class D3, class D4, class D5>
    static void makeF(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4,
                      const D5&        embeddedArg5)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking five
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4F9<F, A1, A2, A3, A4, D1, D2, D3, D4, D5>
                (function, embeddedArg1, embeddedArg2, embeddedArg3,
                 embeddedArg4, embeddedArg5, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4>
    static void makeC(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle, const OBJ& object,
                      C                method)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4C4<OBJ, C, A1, A2, A3, A4>(
                object, method, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4>
    static void makeC(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4C4<OBJ, C, A1, A2, A3, A4>(
                object, method, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class D1>
    static void makeC(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4C5<OBJ, C, A1, A2, A3, A4, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class D1>
    static void makeC(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4C5<OBJ, C, A1, A2, A3, A4, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class D1, class D2>
    static void makeC(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4C6<OBJ, C, A1, A2, A3, A4, D1, D2>(
                object, method, embeddedArg1, embeddedArg2, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class D1, class D2>
    static void makeC(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4C6<OBJ, C, A1, A2, A3, A4, D1, D2>(
                object, method, embeddedArg1, embeddedArg2, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3>
    static void makeC(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4C7<OBJ, C, A1, A2, A3, A4, D1, D2, D3>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3>
    static void makeC(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4C7<OBJ, C, A1, A2, A3, A4, D1, D2, D3>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3, class D4>
    static void makeC(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking four
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4C8<OBJ, C, A1, A2, A3, A4, D1, D2, D3, D4>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3,
                embeddedArg4, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3, class D4>
    static void makeC(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking four
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4C8<OBJ, C, A1, A2, A3, A4, D1, D2, D3, D4>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3,
                embeddedArg4, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3, class D4, class D5>
    static void makeC(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4,
                      const D5&        embeddedArg5)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking five
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
           new(*ba) bdlfxxxi::Vfunc4C9<OBJ, C, A1, A2, A3, A4, D1, D2, D3, D4, D5>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3,
                embeddedArg4, embeddedArg5, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3, class D4, class D5>
    static void makeC(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4,
                      const D5&        embeddedArg5)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking five
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
           new(*ba) bdlfxxxi::Vfunc4C9<OBJ, C, A1, A2, A3, A4, D1, D2, D3, D4, D5>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3,
                embeddedArg4, embeddedArg5, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4>
    static void makeM(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle, OBJ *object,
                      M                method)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4M4<OBJ, M, A1, A2, A3, A4>(
                object, method, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4>
    static void makeM(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4M4<OBJ, M, A1, A2, A3, A4>(
                object, method, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class D1>
    static void makeM(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4M5<OBJ, M, A1, A2, A3, A4, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class D1>
    static void makeM(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4M5<OBJ, M, A1, A2, A3, A4, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class D1, class D2>
    static void makeM(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4M6<OBJ, M, A1, A2, A3, A4, D1, D2>(
                object, method, embeddedArg1, embeddedArg2, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class D1, class D2>
    static void makeM(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
        new(*ba) bdlfxxxi::Vfunc4M6<OBJ, M, A1, A2, A3, A4, D1, D2>(
            object, method, embeddedArg1, embeddedArg2, ba));
}

template <class OBJ, class M, class A1, class A2, class A3, class A4,
          class D1, class D2, class D3>
static void makeM(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4M7<OBJ, M, A1, A2, A3, A4, D1, D2, D3>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3>
    static void makeM(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4M7<OBJ, M, A1, A2, A3, A4, D1, D2, D3>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3, class D4>
    static void makeM(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking four
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4M8<OBJ, M, A1, A2, A3, A4, D1, D2, D3, D4>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3,
                embeddedArg4, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3, class D4>
    static void makeM(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking four
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
            new(*ba) bdlfxxxi::Vfunc4M8<OBJ, M, A1, A2, A3, A4, D1, D2, D3, D4>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3,
                embeddedArg4, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3, class D4, class D5>
    static void makeM(bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4,
                      const D5&        embeddedArg5)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking five
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba = bslma::Default::defaultAllocator();
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
           new(*ba) bdlfxxxi::Vfunc4M9<OBJ, M, A1, A2, A3, A4, D1, D2, D3, D4, D5>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3,
                embeddedArg4, embeddedArg5, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class D1, class D2, class D3, class D4, class D5>
    static void makeM(bslma::Allocator *basicAllocator,
                      bdlfxxx::Vfunc4<A1, A2, A3, A4> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3,
                      const D4&        embeddedArg4,
                      const D5&        embeddedArg5)
        // Initialize the specified four-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking five
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma::Allocator *ba =
                           bslma::Default::allocator(basicAllocator);
        *handle = bdlfxxx::Vfunc4<A1, A2, A3, A4>(
           new(*ba) bdlfxxxi::Vfunc4M9<OBJ, M, A1, A2, A3, A4, D1, D2, D3, D4, D5>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3,
                embeddedArg4, embeddedArg5, ba));
    }
};
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
