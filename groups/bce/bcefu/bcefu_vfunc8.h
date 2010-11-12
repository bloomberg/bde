// bcefu_vfunc8.h                  -*-C++-*-
#ifndef INCLUDED_BCEFU_VFUNC8
#define INCLUDED_BCEFU_VFUNC8

//@PURPOSE: Provide factory methods for populating 8-arg void function objects.
//
//@CLASSES:
//   bcefu_Vfunc8: namespace for factory methods loading 8-arg void functors
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component contains a suite of template functions used to
// configure an existing function object (functor) characterized by a
// function-call operator taking eight arguments and returning void.  Four
// families of populators are supported.
//..
//   bcefu_Vfunc8::makeF:    Initialize the functor by embedding a function
//                           (or another functor) and any additional trailing
//                           arguments.
//
//   bcefu_Vfunc8::makeC:    Initialize the functor by embedding an object, one
//                           of that object's 'const' methods, and any
//                           additional trailing arguments for that method.
//
//   bcefu_Vfunc8::makeM:    Initialize the functor by embedding THE ADDRESS OF
//                           an object, one of that object's ('const' or
//                           non-'const') methods, and any additional trailing
//                           argments for that method.  Note that it is the
//                           client's responsibility to ensure that the object
//                           remains valid throughout the productive lifetime
//                           of a functor object configured in this way.
//
//   bcefu_Vfunc8::makeNull: Initialize the functor with an empty function.
//..
// In each of the first three cases, the underlying procedure (i.e., function,
// functor, or method) will have eight parameter (matching those of the host
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

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEF_VFUNC8
#include <bcef_vfunc8.h>
#endif

#ifndef INCLUDED_BCEFI_VFUNC8
#include <bcefi_vfunc8.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

                        // ==================
                        // class bcefu_Vfunc8
                        // ==================

namespace BloombergLP {

struct bcefu_Vfunc8 {
    // This class provides a namespace for a suite of member template
    // procedures used to populate existing function objects (functors) taking
    // eight required argument.

    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    static void makeNull(bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle)
        // Initialize the specified eight-argument empty functor 'handle'.
        // Optionally specify, as the *first* argument, a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0 or unspecified, the
        // currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
            new(*ba) bcefi_Vfunc8Null<A1, A2, A3, A4, A5, A6, A7, A8>(ba));
    }

    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8>
    static void makeNull(bdema_Allocator *basicAllocator,
                         bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle)
        // Initialize the specified eight-argument empty functor 'handle'.
        // Optionally specify, as the *first* argument, a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0 or unspecified, the
        // currently installed default allocator is used.
    {
        bdema_Allocator *ba =
             bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
            new(*ba) bcefi_Vfunc8Null<A1, A2, A3, A4, A5, A6, A7, A8>(ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6, class A7, class A8>
    static void makeF(bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      F function)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
            new(*ba) bcefi_Vfunc8F8<F, A1, A2, A3, A4, A5, A6, A7, A8>(
                function, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6, class A7, class A8>
    static void makeF(bdema_Allocator *basicAllocator,
                      bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      F                function)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
             bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
             new(*ba) bcefi_Vfunc8F8<F, A1, A2, A3, A4, A5, A6, A7, A8>(
                 function, ba));
    }

    template <class F, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8, class D1>
    static void makeF(bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      F                function,
                      const D1&        embeddedArg1)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and the value corresponding to this
        // argument.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
            new(*ba) bcefi_Vfunc8F9<F, A1, A2, A3, A4, A5, A6, A7, A8, D1> (
                function, embeddedArg1, ba));
    }

    template <class F, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8, class D1>
    static void makeF(bdema_Allocator *basicAllocator,
                      bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      F                function,
                      const D1&        embeddedArg1)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and the value corresponding to this
        // argument.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
            new(*ba) bcefi_Vfunc8F9<F, A1, A2, A3, A4, A5, A6, A7, A8, D1> (
                function, embeddedArg1, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8>
    static void makeC(bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      const OBJ& object, C method)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
            new(*ba) bcefi_Vfunc8C8<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8>(
                object, method, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8>
    static void makeC(bdema_Allocator *basicAllocator,
                      bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      const OBJ&       object,
                      C                method)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
            new(*ba) bcefi_Vfunc8C8<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8>(
                object, method, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8, class D1>
    static void makeC(bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
           new(*ba) bcefi_Vfunc8C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8, class D1>
    static void makeC(bdema_Allocator *basicAllocator,
                      bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
           new(*ba) bcefi_Vfunc8C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8>
    static void makeM(bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      OBJ                                     *object,
                      M                                        method)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
            new(*ba) bcefi_Vfunc8M8<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8>(
                object, method, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8>
    static void makeM(bdema_Allocator *basicAllocator,
                      bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      OBJ             *object,
                      M                method)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
            new(*ba) bcefi_Vfunc8M8<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8>(
                object, method, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8, class D1>
    static void makeM(bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking one additional
        // trailing argument and its value.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
           new(*ba) bcefi_Vfunc8M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8, class D1>
    static void makeM(bdema_Allocator *basicAllocator,
                      bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1)
        // Initialize the specified eight-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking one additional
        // trailing argument and its value.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc8<A1, A2, A3, A4, A5, A6, A7, A8>(
           new(*ba) bcefi_Vfunc8M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8, D1>(
                object, method, embeddedArg1, ba));
    }
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
