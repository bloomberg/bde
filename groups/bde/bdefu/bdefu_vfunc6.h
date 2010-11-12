// bdefu_vfunc6.h                  -*-C++-*-
#ifndef INCLUDED_BDEFU_VFUNC6
#define INCLUDED_BDEFU_VFUNC6

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide factory methods for populating 6-arg void function objects.
//
//@CLASSES:
//   bdefu_Vfunc6: namespace for factory methods loading 6-arg void functors
//
//@AUTHOR: John Lakos (jlakos)
//
//@DEPRECATED: This component should not be used in new code, and will be
// deleted in the near future.  Please see 'bdef_function', 'bdef_bind', etc.
// for alternatives that should be used for all future development.
//
//@DESCRIPTION: This component contains a suite of template functions used to
// configure an existing function object (functor) characterized by a
// function-call operator taking six arguments and returning void.  Four
// families of populators are supported.
//..
//   bdefu_Vfunc6::makeF:    Initialize the functor by embedding a function
//                           (or another functor) and any additional trailing
//                           arguments.
//
//   bdefu_Vfunc6::makeC:    Initialize the functor by embedding an object, one
//                           of that object's 'const' methods, and any
//                           additional trailing arguments for that method.
//
//   bdefu_Vfunc6::makeM:    Initialize the functor by embedding THE ADDRESS OF
//                           an object, one of that object's ('const' or
//                           non-'const') methods, and any additional trailing
//                           argments for that method.  Note that it is the
//                           client's responsibility to ensure that the object
//                           remains valid throughout the productive lifetime
//                           of a functor object configured in this way.
//
//   bdefu_Vfunc6::makeNull: Initialize the functor with an empty function.
//..
// In each of the first three cases, the underlying procedure (i.e., function,
// functor, or method) will have six parameter (matching those of the host
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEF_VFUNC6
#include <bdef_vfunc6.h>
#endif

#ifndef INCLUDED_BDEFI_VFUNC6
#include <bdefi_vfunc6.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

namespace BloombergLP {

                        // ==================
                        // class bdefu_Vfunc6
                        // ==================
struct bdefu_Vfunc6 {
    // This class provides a namespace for a suite of member template
    // procedures used to populate existing function objects (functors) taking
    // six required argument.

    template <class A1, class A2, class A3, class A4, class A5, class A6>
    static void makeNull(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle)
        // Initialize the specified six-argument empty functor 'handle'.
        // Optionally specify, as the *first* argument, a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0 or unspecified,
        // the currently installed default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6Null<A1, A2, A3, A4, A5, A6>(ba));
    }

    template <class A1, class A2, class A3, class A4, class A5, class A6>
    static void makeNull(bslma_Allocator *basicAllocator,
                         bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle)
        // Initialize the specified six-argument empty functor 'handle'.
        // Optionally specify, as the *first* argument, a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0 or unspecified,
        // the currently installed default allocator is used.
    {
        bslma_Allocator *ba =
             bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6Null<A1, A2, A3, A4, A5, A6>(ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6>
    static void makeF(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle, F function)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6F6<F, A1, A2, A3, A4, A5, A6>(function, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6>
    static void makeF(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      F                function)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma_Allocator *ba =
             bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
             new(*ba) bdefi_Vfunc6F6<F, A1, A2, A3, A4, A5, A6>(function, ba));
    }

    template <class F, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1>
    static void makeF(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      F                function,
                      const D1&        embeddedArg1)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and the value corresponding to this
        // argument.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6F7<F, A1, A2, A3, A4, A5, A6, D1> (
                function, embeddedArg1, ba));
    }

    template <class F, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1>
    static void makeF(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      F                function,
                      const D1&        embeddedArg1)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and the value corresponding to this
        // argument.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6F7<F, A1, A2, A3, A4, A5, A6, D1> (
                function, embeddedArg1, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6, class D1, class D2>
    static void makeF(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();

        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6F8<F, A1, A2, A3, A4, A5, A6, D1, D2>
                (function, embeddedArg1, embeddedArg2, ba));
    }

    template <class F, class A1, class A2, class A3, class A4, class A5,
              class A6, class D1, class D2>
    static void makeF(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);

        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6F8<F, A1, A2, A3, A4, A5, A6, D1, D2>
                (function, embeddedArg1, embeddedArg2, ba));
    }

    template <class F, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1, class D2, class D3>
    static void makeF(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6F9<F, A1, A2, A3, A4, A5, A6, D1, D2, D3>
                (function, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class F, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1, class D2, class D3>
    static void makeF(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      F                function,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6F9<F, A1, A2, A3, A4, A5, A6, D1, D2, D3>
                (function, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6>
    static void makeC(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      const OBJ& object, C method)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6C6<OBJ, C, A1, A2, A3, A4, A5, A6>(
                object, method, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6>
    static void makeC(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      const OBJ&       object,
                      C                method)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6C6<OBJ, C, A1, A2, A3, A4, A5, A6>(
                object, method, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1>
    static void makeC(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6C7<OBJ, C, A1, A2, A3, A4, A5, A6, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1>
    static void makeC(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6C7<OBJ, C, A1, A2, A3, A4, A5, A6, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1, class D2>
    static void makeC(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6C8<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2>(
                object, method, embeddedArg1, embeddedArg2, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1, class D2>
    static void makeC(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6C8<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2>(
                object, method, embeddedArg1, embeddedArg2, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1, class D2, class D3>
    static void makeC(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
           new(*ba) bdefi_Vfunc6C9<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2, D3>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class OBJ, class C, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1, class D2, class D3>
    static void makeC(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      const OBJ&       object,
                      C                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'function' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
           new(*ba) bdefi_Vfunc6C9<OBJ, C, A1, A2, A3, A4, A5, A6, D1, D2, D3>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6>
    static void makeM(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle, OBJ *object,
                      M                method)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6M6<OBJ, M, A1, A2, A3, A4, A5, A6>(
                object, method, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6>
    static void makeM(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      OBJ             *object,
                      M                method)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking no
        // additional trailing arguments.  Optionally specify, as the *first*
        // argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6M6<OBJ, M, A1, A2, A3, A4, A5, A6>(
                object, method, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1>
    static void makeM(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6M7<OBJ, M, A1, A2, A3, A4, A5, A6, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1>
    static void makeM(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking one
        // additional trailing argument and its value.  Optionally specify, as
        // the *first* argument, a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0 or unspecified, the currently installed
        // default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6M7<OBJ, M, A1, A2, A3, A4, A5, A6, D1>(
                object, method, embeddedArg1, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1, class D2>
    static void makeM(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
            new(*ba) bdefi_Vfunc6M8<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2>(
                object, method, embeddedArg1, embeddedArg2, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1, class D2>
    static void makeM(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking two
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
        new(*ba) bdefi_Vfunc6M8<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2>(
            object, method, embeddedArg1, embeddedArg2, ba));
}

template <class OBJ, class M, class A1, class A2, class A3, class A4,
          class A5, class A6, class D1, class D2, class D3>
static void makeM(bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba = bslma_Default::defaultAllocator();
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
           new(*ba) bdefi_Vfunc6M9<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2, D3>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }

    template <class OBJ, class M, class A1, class A2, class A3, class A4,
              class A5, class A6, class D1, class D2, class D3>
    static void makeM(bslma_Allocator *basicAllocator,
                      bdef_Vfunc6<A1, A2, A3, A4, A5, A6> *handle,
                      OBJ             *object,
                      M                method,
                      const D1&        embeddedArg1,
                      const D2&        embeddedArg2,
                      const D3&        embeddedArg3)
        // Initialize the specified six-argument functor 'handle' by
        // embedding the specified 'method' (or functor) taking three
        // additional trailing arguments and the values corresponding to those
        // arguments.  Optionally specify, as the *first* argument, a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0
        // or unspecified, the currently installed default allocator is used.
    {
        bslma_Allocator *ba =
                           bslma_Default::allocator(basicAllocator);
        *handle = bdef_Vfunc6<A1, A2, A3, A4, A5, A6>(
           new(*ba) bdefi_Vfunc6M9<OBJ, M, A1, A2, A3, A4, A5, A6, D1, D2, D3>(
                object, method, embeddedArg1, embeddedArg2, embeddedArg3, ba));
    }
};

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
