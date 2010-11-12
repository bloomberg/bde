// bcefu_vfunc9.h                  -*-C++-*-
#ifndef INCLUDED_BCEFU_VFUNC9
#define INCLUDED_BCEFU_VFUNC9

//@PURPOSE: Provide factory methods for populating 9-arg void function objects.
//
//@CLASSES:
//   bcefu_Vfunc9: namespace for factory methods loading 9-arg void functors
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component contains a suite of template functions used to
// configure an existing function object (functor) characterized by a
// function-call operator taking nine arguments and returning void.  Four
// families of populators are supported.
//..
//   bcefu_Vfunc9::makeF:    Initialize the functor by embedding a function.
//
//   bcefu_Vfunc9::makeC:    Initialize the functor by embedding an object, and
//                           one of that object's 'const' methods.
//
//   bcefu_Vfunc9::makeM:    Initialize the functor by embedding THE ADDRESS OF
//                           an object, and one of that object's ('const' or
//                           non-'const') methods.  Note that it is the
//                           client's responsibility to ensure that the object
//                           remains valid throughout the productive lifetime
//                           of a functor object configured in this way.
//
//   bcefu_Vfunc9::makeNull: Initialize the functor with an empty function.
//..
// In each of the first three cases, the underlying procedure (i.e., function,
// functor, or method) will have nine parameter (matching those of the host
// functor's function-call operator) with no additional parameters.
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

#ifndef INCLUDED_BCEF_VFUNC9
#include <bcef_vfunc9.h>
#endif

#ifndef INCLUDED_BCEFI_VFUNC9
#include <bcefi_vfunc9.h>
#endif

#ifndef INCLUDED_BDEMA_DEFAULT
#include <bdema_default.h>
#endif

                        // ==================
                        // class bcefu_Vfunc9
                        // ==================

namespace BloombergLP {

struct bcefu_Vfunc9 {
    // This class provides a namespace for a suite of member template
    // procedures used to populate existing function objects (functors) taking
    // nine required argument.

    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    static void makeNull(
        bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> *handle)
        // Initialize the specified nine-argument empty functor 'handle'.
        // Optionally specify, as the *first* argument, a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0 or unspecified, the
        // currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(
            new(*ba) bcefi_Vfunc9Null<A1, A2, A3, A4, A5, A6, A7, A8, A9>(ba));
    }

    template <class A1, class A2, class A3, class A4, class A5, class A6,
              class A7, class A8, class A9>
    static void makeNull(bdema_Allocator *basicAllocator,
        bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> *handle)
        // Initialize the specified nine-argument empty functor 'handle'.
        // Optionally specify, as the *first* argument, a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0 or unspecified, the
        // currently installed default allocator is used.
    {
        bdema_Allocator *ba =
             bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(
            new(*ba) bcefi_Vfunc9Null<A1, A2, A3, A4, A5, A6, A7, A8, A9>(ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6, class A7, class A8, class A9>
    static void makeF(bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> *handle,
                      F function)
        // Initialize the specified nine-argument functor 'handle' by embedding
        // the specified 'function' (or functor) taking no additional trailing
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(
            new(*ba) bcefi_Vfunc9F9<F, A1, A2, A3, A4, A5, A6, A7, A8, A9>(
                function, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6, class A7, class A8, class A9>
    static void makeF(bdema_Allocator *basicAllocator,
                      bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> *handle,
                      F                function)
        // Initialize the specified nine-argument functor 'handle' by embedding
        // the specified 'function' (or functor) taking no additional trailing
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
             bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(
             new(*ba) bcefi_Vfunc9F9<F, A1, A2, A3, A4, A5, A6, A7, A8, A9>(
                 function, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8, class A9>
    static void makeC(bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> *handle,
                      const OBJ& object, C method)
        // Initialize the specified nine-argument functor 'handle' by embedding
        // the specified 'function' (or functor) taking no additional trailing
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(
           new(*ba) bcefi_Vfunc9C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8, A9>(
                object, method, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8, class A9>
    static void makeC(bdema_Allocator *basicAllocator,
                      bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> *handle,
                      const OBJ&       object,
                      C                method)
        // Initialize the specified nine-argument functor 'handle' by embedding
        // the specified 'function' (or functor) taking no additional trailing
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(
           new(*ba) bcefi_Vfunc9C9<OBJ, C, A1, A2, A3, A4, A5, A6, A7, A8, A9>(
                object, method, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8, class A9>
    static void makeM(bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> *handle,
                      OBJ                                     *object,
                      M                                        method)
        // Initialize the specified nine-argument functor 'handle' by embedding
        // the specified 'method' (or functor) taking no additional trailing
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba = bdema_Default::defaultAllocator();
        *handle = bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(
           new(*ba) bcefi_Vfunc9M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8, A9>(
                object, method, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class A7, class A8, class A9>
    static void makeM(bdema_Allocator *basicAllocator,
                      bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9> *handle,
                      OBJ             *object,
                      M                method)
        // Initialize the specified nine-argument functor 'handle' by embedding
        // the specified 'method' (or functor) taking no additional trailing
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0 or
        // unspecified, the currently installed default allocator is used.
    {
        bdema_Allocator *ba =
                           bdema_Default::allocator(basicAllocator);
        *handle = bcef_Vfunc9<A1, A2, A3, A4, A5, A6, A7, A8, A9>(
           new(*ba) bcefi_Vfunc9M9<OBJ, M, A1, A2, A3, A4, A5, A6, A7, A8, A9>(
                object, method, ba));
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
