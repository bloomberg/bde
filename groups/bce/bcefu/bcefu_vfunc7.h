// bcefu_vfunc7.h                  -*-C++-*-
#ifndef INCLUDED_BCEFU_VFUNC7
#define INCLUDED_BCEFU_VFUNC7

//@PURPOSE: Provide factory methods for populating 7-arg void function objects.
//
//@CLASSES:
//   bcefu_Vfunc7: namespace for factory methods loading 7-arg void functors
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component contains a suite of template functions used to
// configure an existing function object (functor) characterized by a
// function-call operator taking seven arguments and returning void.  Four
// families of populators are supported.
//..
//   bcefu_Vfunc7::makeF:    Initialize the functor by embedding a function
//                           (or another functor) and any additional trailing
//                           arguments.
//
//   bcefu_Vfunc7::makeC:    Initialize the functor by embedding an object, one
//                           of that object's 'const' methods, and any
//                           additional trailing arguments for that method.
//
//   bcefu_Vfunc7::makeM:    Initialize the functor by embedding THE ADDRESS OF
//                           an object, one of that object's ('const' or
//                           non-'const') methods, and any additional trailing
//                           argments for that method.  Note that it is the
//                           client's responsibility to ensure that the object
//                           remains valid throughout the productive lifetime
//                           of a functor object configured in this way.
//
//   bcefu_Vfunc7::makeNull: Initialize the functor with an empty function.
//..
// In each of the first three cases, the underlying procedure (i.e., function,
// functor, or method) will have seven parameter (matching those of the host
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

#ifndef INCLUDED_BCEF_VFUNC7
#include <bcef_vfunc7.h>
#endif

#ifndef INCLUDED_BCEFI_VFUNC7
#include <bcefi_vfunc7.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

                        // ==================
                        // class bcefu_Vfunc7
                        // ==================

namespace BloombergLP {

struct bcefu_Vfunc7 {
    // This class provides a namespace for a suite of member template
    // procedures used to populate existing function objects (functors) taking
    // seven required argument.

    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7>
    static void makeNull(bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle)
        // Initialize the specified seven-argument empty functor 'handle'.
        // Optionally specify, as the *first* argument, a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0 or unspecified, the
        // currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7Null<A1, A2, A3, A4, A5, A6, A7>(ba));
    }

    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7>
    static void makeNull(bdema_Allocator *basicAllocator,
                         bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle)
        // Initialize the specified seven-argument empty functor 'handle'.
        // Optionally specify, as the *first* argument, a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0 or unspecified, the
        // currently installed default allocator is used.
    {
        bdema_Allocator *ba =
             bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7Null<A1, A2, A3, A4, A5, A6, A7>(ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6, class A7>
    static void makeF(bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      F function)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7F7<F, A1, A2, A3, A4, A5, A6, A7>(
                function, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6, class A7>
    static void makeF(bdema_Allocator *basicAllocator,
                      bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      F                function)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
             bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
             new(*ba) bcefi_Vfunc7F7<F, A1, A2, A3, A4, A5, A6, A7>(
                 function, ba));
    }

    template <class F, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class D1>
    static void makeF(bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      F                function,
                      const D1&        embeddedArg1)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and the value corresponding to this
        // argument.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7F8<F, A1, A2, A3, A4, A5, A6, A7, D1> (
                function, embeddedArg1, ba));
    }

    template <class F, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class D1>
    static void makeF(bdema_Allocator *basicAllocator,
                      bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      F                function,
                      const D1&        embeddedArg1)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and the value corresponding to this
        // argument.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7F8<F, A1, A2, A3, A4, A5, A6, A7, D1> (
                function, embeddedArg1, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6, class A7, class D1, class D2>
    static void makeF(bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();

        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7F9<F, A1, A2, A3, A4, A5, A6, A7, D1, D2>
                (function, embeddedArg1, embeddedArg2, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6, class A7, class D1, class D2>
    static void makeF(bdema_Allocator *basicAllocator,
                      bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);

        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7F9<F, A1, A2, A3, A4, A5, A6, A7, D1, D2>
                (function, embeddedArg1, embeddedArg2, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7>
    static void makeC(bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      const OBJ& object, C method)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7C7<OBJ, C, A1, A2, A3, A4, A5, A6, A7>(
                object, method, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7>
    static void makeC(bdema_Allocator *basicAllocator,
                      bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      const OBJ&       object,
                      C                method)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7C7<OBJ, C, A1, A2, A3, A4, A5, A6, A7>(
                object, method, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class D1>
    static void makeC(bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7C8<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class D1>
    static void makeC(bdema_Allocator *basicAllocator,
                      bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7C8<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class D1, class D2>
    static void makeC(bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
           new(*ba) bcefi_Vfunc7C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1, D2>(
                object, method, embeddedArg1, embeddedArg2, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class D1, class D2>
    static void makeC(bdema_Allocator *basicAllocator,
                      bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
           new(*ba) bcefi_Vfunc7C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, D1, D2>(
                object, method, embeddedArg1, embeddedArg2, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7>
    static void makeM(bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      OBJ                                     *object,
                      M                                        method)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7M7<OBJ, M, A1, A2, A3, A4, A5, A6, A7>(
                object, method, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7>
    static void makeM(bdema_Allocator *basicAllocator,
                      bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      OBJ             *object,
                      M                method)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking no additional
        // trailing arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7M7<OBJ, M, A1, A2, A3, A4, A5, A6, A7>(
                object, method, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class D1>
    static void makeM(bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking one additional
        // trailing argument and its value.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7M8<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class D1>
    static void makeM(bdema_Allocator *basicAllocator,
                      bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking one additional
        // trailing argument and its value.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
            new(*ba) bcefi_Vfunc7M8<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class D1, class D2>
    static void makeM(bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking two additional
        // trailing arguments and the values corresponding to those arguments.
        // Optionally specify, as the *first* argument, a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0 or unspecified, the
        // currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
           new(*ba) bcefi_Vfunc7M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1, D2>(
                object, method, embeddedArg1, embeddedArg2, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class D1, class D2>
    static void makeM(bdema_Allocator *basicAllocator,
                      bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified seven-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking two additional
        // trailing arguments and the values corresponding to those arguments.
        // Optionally specify, as the *first* argument, a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0 or unspecified, the
        // currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc7<A1, A2, A3, A4, A5, A6, A7>(
        new(*ba) bcefi_Vfunc7M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, D1, D2>(
            object, method, embeddedArg1, embeddedArg2, ba));
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
