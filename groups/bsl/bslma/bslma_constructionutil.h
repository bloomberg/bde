// bslma_constructionutil.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_CONSTRUCTIONUTIL
#define INCLUDED_BSLMA_CONSTRUCTIONUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide methods to construct arbitrarily-typed objects uniformily.
//
//@CLASSES:
//  bslma::ConstructionUtil: namespace for methods to construct objects
//
//@SEE_ALSO: bslma_destructionutil
//
//@AUTHOR:
//
//@DESCRIPTION: This component provides routines to construct objects of an
// arbitrary (template parameter) type, given an arbitrary number of arguments.
// These routines are useful in implementing 'allocator_trait' classes which,
// in turn, are used in implementing generic components such as containers.
//
// The traits under consideration by this component are:
//..
//  Trait                                         Description
//  --------------------------------------------  -----------------------------
//  bsl::is_trivially_default_constructible       "TYPE has the trivial default
//                                                constructor trait", or
//                                                "TYPE has a trivial default
//                                                constructor"
//
//  bslma::UsesBslmaAllocator                     "the 'TYPE' constructor takes
//                                                an allocator argument", or
//                                                "'TYPE' supports 'bslma'-
//                                                style allocators"
//
//  bslmf::UsesAllocatorArgT                      "the 'TYPE' constructor takes
//                                                an allocator argument", and
//                                                optionally passes allocators
//                                                as the first two arguments to
//                                                each constructor, where the
//                                                tag type 'allocator_arg_t' is
//                                                first, and the allocator type
//                                                is second
//
//  bsl::is_trivially_copyable                    "TYPE has the bit-wise
//                                                copyable trait", or
//                                                "TYPE is bit-wise copyable"
//                                                (implies that it has a
//                                                trivial destructor too)
//
// TBD: remove this?
//  bslmf::IsBitwiseMoveable                      "TYPE has the bit-wise
//                                                moveable trait", or
//                                                "TYPE is bit-wise moveable"
//
//  bslmf::IsPair                                 "TYPE has the pair trait"
//                                                or "TYPE is a pair-like"
//..
//
///Usage
///-----
// TBD: add this
//

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_DESTRUCTORPROCTOR
#include <bslma_destructorproctor.h>
#endif

#ifndef INCLUDED_BSLMA_DESTRUCTIONUTIL
#include <bslma_destructionutil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_USESALLOCATORARGT
#include <bslmf_usesallocatorargt.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_ALLOCATORARGT
#include <bslmf_allocatorargt.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISPAIR
#include <bslmf_ispair.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#include <bslmf_istriviallydefaultconstructible.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_UTIL
#include <bslmf_util.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_UTIL
#include <bsls_util.h>
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_CSTDDEF
#endif

#ifndef INCLUDED_STRING_H
#include <string.h>
#define INCLUDED_STRING_H
#endif

#ifndef INCLUDED_NEW
#include <new>          // placement 'new'
#define INCLUDED_NEW
#endif

namespace BloombergLP {
namespace bslma {

// TBD: this is temporary - find a routine in 'bsl' that can do this (or
//      similar) and get rid of this
namespace tmp {

struct ImpUtil {
    template <class TYPE>
    static void *voidify(TYPE *address);
        // Return the specified 'address' cast as a pointer to 'void', even if
        // (the template parameter) 'TYPE' is cv-qualified.
};

template <class TYPE>
inline
void *ImpUtil::voidify(TYPE *address) {
    return static_cast<void *>(
            const_cast<typename bsl::remove_cv<TYPE>::type *>(address));
}

} // close namespace 'tmp'

// Workaround for optimization issue in xlC that mishandles pointer aliasing.
//   IV56864: ALIASING BEHAVIOUR FOR PLACEMENT NEW
//   http://www-01.ibm.com/support/docview.wss?uid=swg1IV56864
// Place this macro following each use of placement new.  Alternatively,
// compile with xlC_r -qalias=noansi, which reduces optimization opportunities
// across entire translation unit instead of simply across optimization fence.
// Update: issue is fixed in xlC 13.1 (__xlC__ >= 0x0d01).

#if defined(BSLS_PLATFORM_CMP_IBM) && BSLS_PLATFORM_CMP_VERSION < 0x0d01
    #define BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX                     \
                             BSLS_PERFORMANCEHINT_OPTIMIZATION_FENCE
#else
    #define BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX
#endif

struct ConstructionUtil_Imp;

                          // =======================
                          // struct ConstructionUtil
                          // =======================

struct ConstructionUtil {
    // This 'struct' provides a namespace for utility functions that construct
    // elements of (a template parameter) 'TARGET_TYPE'.

  private:

    typedef ConstructionUtil_Imp Imp;

  public:
    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(TARGET_TYPE *address,
                                ALLOCATOR   *allocator,
                                TARGET_TYPE *original);
        // TBD: comment this

    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE *address, bslma::Allocator *allocator);
    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE *address, void *allocator);
        // Create a default-constructed object of (template parameter) type
        // 'TARGET_TYPE' at the specified (uninitialized memory) 'address'.  If
        // 'allocator' is of type 'bslma::Allocator' and 'TARGET_TYPE' supports
        // 'bslma'-style allocation, 'allocator' is passed to the default
        // constructor.  If the constructor throws, 'address' is left in an
        // uninitialized state.  Note that this operation may bypass the call
        // to the default constructor of a user-defined type entirely if
        // 'TARGET_TYPE' 1) does not use 'bslma'-style allocators and 2) has a
        // trivial default constructor.

    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE        *address,
                          bslma::Allocator   *allocator,
                          const TARGET_TYPE&  original);
    template <class TARGET_TYPE>
    static void
    construct(TARGET_TYPE *address, void *, const TARGET_TYPE& original);
    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE        *address,
                          bslma::Allocator   *allocator,
                          TARGET_TYPE&  original);
    template <class TARGET_TYPE>
    static void
    construct(TARGET_TYPE *address, void *, TARGET_TYPE& original);
        // Create an object of (template parameter) type 'TARGET_TYPE', having
        // the same value as the specified 'original' object, at the specified
        // (uninitialized memory) 'address'.  If 'allocator' is of type
        // 'bslma::Allocator' and 'TARGET_TYPE' supports 'bslma'-style
        // allocation, 'allocator' is propagated to the newly created object.
        // If a constructor throws, 'address' is left in an uninitialized
        // state.  Note that this operation may elide the call to the copy
        // constructor entirely if 'TARGET_TYPE' 1) does not use 'bslma'-style
        // allocators and 2) is trivially copyable.  Further note that we
        // provide (unconventional) overloads for modifiable lvalues because
        // these may match more generic overloads (below) taking a variable
        // number of deduced template parameters.

    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE                    *address,
                          bslma::Allocator               *allocator,
                          bslmf::MovableRef<TARGET_TYPE>  original);
    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE                    *address,
                          void                           *allocator,
                          bslmf::MovableRef<TARGET_TYPE>  original);
        // Create an object of (template parameter) type 'TARGET_TYPE', having
        // the same value as the specified 'original' object, at the specified
        // (uninitialized memory) 'address'.  'original' is left in a valid but
        // unspecified state.  If 'allocator' is of type 'bslma::Allocator' and
        // 'TARGET_TYPE' supports 'bslma'-style allocation, 'allocator' is
        // propagated to the newly created object.  If a constructor throws,
        // 'address' is left in an uninitialized state.  Note that this
        // operation may elide the call to the copy constructor entirely if
        // 'TARGET_TYPE' 1) does not use 'bslma'-style allocators and 2) is
        // trivially copyable.  Further note that we provide (unconventional)
        // overloads for modifiable lvalues because these may match more
        // generic overloads (below) taking a variable number of deduced
        // template parameters.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=15
    template <class TARGET_TYPE, class Arg1, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          Arg1&&            argument,
                          Args&&...         arguments);
    template <class TARGET_TYPE, class Arg1, class... Args>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          Arg1&&       argument,
                          Args&&...    arguments);
        // Create an object of (template parameter) type 'TARGET_TYPE' at
        // the specified (uninitialized memory) 'address', constructed by
        // forwarding the specified 'argument' and the (variable number of)
        // additional 'arguments' to the corresponding constructor of
        // 'TARGET_TYPE'.  If 'allocator' is of type 'bslma::Allocator' and
        // 'TARGET_TYPE' supports 'bslma'-style allocation, the allocator is
        // passed to the constructor (as the last argument).  If the
        // constructor throws, 'address' is left in an uninitialized state.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslma_constructionutil.h
    template <class TARGET_TYPE, class Arg1>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1);

    template <class TARGET_TYPE, class Arg1, class Args_01>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10,
                                             class Args_11>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10,
                                             class Args_11,
                                             class Args_12>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10,
                                             class Args_11,
                                             class Args_12,
                                             class Args_13>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10,
                                             class Args_11,
                                             class Args_12,
                                             class Args_13,
                                             class Args_14>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10,
                                             class Args_11,
                                             class Args_12,
                                             class Args_13,
                                             class Args_14,
                                             class Args_15>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);

    template <class TARGET_TYPE, class Arg1>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1);

    template <class TARGET_TYPE, class Arg1, class Args_01>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10,
                                             class Args_11>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10,
                                             class Args_11,
                                             class Args_12>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10,
                                             class Args_11,
                                             class Args_12,
                                             class Args_13>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10,
                                             class Args_11,
                                             class Args_12,
                                             class Args_13,
                                             class Args_14>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);

    template <class TARGET_TYPE, class Arg1, class Args_01,
                                             class Args_02,
                                             class Args_03,
                                             class Args_04,
                                             class Args_05,
                                             class Args_06,
                                             class Args_07,
                                             class Args_08,
                                             class Args_09,
                                             class Args_10,
                                             class Args_11,
                                             class Args_12,
                                             class Args_13,
                                             class Args_14,
                                             class Args_15>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class TARGET_TYPE, class Arg1, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
    template <class TARGET_TYPE, class Arg1, class... Args>
    static void construct(TARGET_TYPE *address,
                          void        *allocator,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
// }}} END GENERATED CODE
#endif
};

                        // ===========================
                        // struct ConstructionUtil_Imp
                        // ===========================

struct ConstructionUtil_Imp {
    // This 'struct' provides a namespace for a suite of utility functions that
    // are used to implement functions in 'ConstructionUtil'.  In particular,
    // they provide overloads, resolved at compile-time, for various features
    // (e.g., passing down the allocator to sub-elements of 'pair'-like types)
    // and optimizations (e.g., bypassing the call to the constructor for
    // classes with trivial default and copy constructors).  These functions
    // should not be used outside this component.

  private:
    template <class TARGET_TYPE>
    static TARGET_TYPE *unconst(const TARGET_TYPE *address)
    {
        return const_cast<TARGET_TYPE *>(address);
    }

  public:

    enum {
        // These constants are used in the overloads below, when the last
        // argument is of type 'bsl::integral_constant<int, N> *', indicating
        // that 'TARGET_TYPE' has the traits for which the enumerator equal to
        // 'N' is named.

        e_USES_ALLOCATOR_ARG_T_TRAITS     = 6, // Implies USES_BSLMA_ALLOCATOR
        e_USES_BSLMA_ALLOCATOR_TRAITS     = 5,
        e_PAIR_TRAITS                     = 4,
        e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS = 3,
        e_BITWISE_COPYABLE_TRAITS         = 2,
        e_BITWISE_MOVABLE_TRAITS          = 1,
        e_NIL_TRAITS                      = 0
    };

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(
                     TARGET_TYPE *address,
                     ALLOCATOR   *allocator,
                     bsl::integral_constant<int, e_BITWISE_MOVABLE_TRAITS> *,
                     TARGET_TYPE *original);
        // TBD: comment this

    template <class TARGET_TYPE, class ALLOCATOR>
    static void destructiveMove(TARGET_TYPE *address,
                                ALLOCATOR   *allocator,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                                TARGET_TYPE *original);
        // TBD: comment this

    // In order to implement 'allocator_traits<A>::construct(m, p)'
    //                       if 'PAIR_TRAITS'
    //                       or 'HAS_TRIVIAL_DEFAULT_CTOR_TRAITS'

    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_PAIR_TRAITS> *);
        // Create a default-constructed 'pair'-like object of (template
        // parameter) type 'TARGET_TYPE' at the specified (uninitialized
        // memory) address.  If 'allocator' is of type 'bslma::Allocator' and
        // either of the 'first' and 'second' sub-elements of 'TARGET_TYPE'
        // supports 'bslma'-style allocation, the 'allocator' is passed to the
        // default constructor when creating the sub-element.  If the
        // constructor throws, the 'address' is left in an uninitialized state.
        // Note that the behavior is undefined if 'TARGET_TYPE' directly
        // supports 'bslma'-style allocators.

    template <class TARGET_TYPE>
    static void construct(
             TARGET_TYPE      *address,
             bslma::Allocator *allocator,
             bsl::integral_constant<int, e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *);
    template <class TARGET_TYPE>
    static void construct(
             TARGET_TYPE *address,
             bsl::integral_constant<int, e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *);
        // Construct a default instance of (template parameter) type
        // 'TARGET_TYPE' that has a trivial default constructor, at the
        // specified (uninitialized memory) address.  If the constructor
        // throws, 'address' is left in an uninitialized state.  Note that the
        // behavior is undefined if 'TARGET_TYPE' supports 'bslma'-style
        // allocators.

    // In order to implement 'allocator_traits<A>::construct(m, p, rv)'
    //                       'allocator_traits<A>::construct(m, p,  v)'
    //                  if 'BITWISE_COPYABLE_TRAITS'

    template <class TARGET_TYPE>
    static void
    construct(TARGET_TYPE      *address,
              bslma::Allocator *allocator,
              bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
              const TARGET_TYPE& original);
    template <class TARGET_TYPE>
    static void
    construct(TARGET_TYPE *address,
              bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
              const TARGET_TYPE& original);
        // Create an object of a bit-wise copyable (template parameter) type
        // 'TARGET_TYPE' at the specified (uninitialized memory) 'address',
        // with the same value as the specified 'original' object.  If the
        // constructor throws, 'address' is left in an uninitialized state.
        // Note that the behavior is undefined if 'TARGET_TYPE' supports
        // 'bslma'-style allocators.

    template <class TARGET_TYPE>
    static void
    construct(TARGET_TYPE      *address,
              bslma::Allocator *allocator,
              bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
              bslmf::MovableRef<TARGET_TYPE> original);
    template <class TARGET_TYPE>
    static void
    construct(TARGET_TYPE *address,
              bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
              bslmf::MovableRef<TARGET_TYPE> original);
        // Create an object of a bit-wise copyable (template parameter) type
        // 'TARGET_TYPE' at the specified (uninitialized memory) 'address',
        // with the same value as the specified 'original' object.  If the
        // constructor throws, 'address' is left in an uninitialized state.
        // Note that the behavior is undefined if 'TARGET_TYPE' supports
        // 'bslma'-style allocators.

    // In order to implement 'allocator_traits<A>::construct(m, p, rv)'
    //                       'allocator_traits<A>::construct(m, p,  v)'
    //                  if 'PAIR_TRAITS'

#if defined(BSLS_PLATFORM_CMP_SUN)
    template <class TARGET_TYPE, class Arg1>
    static void construct(TARGET_TYPE                            *address,
                          bslma::Allocator                       *allocator,
                          bsl::integral_constant<int, e_PAIR_TRAITS> *,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) original);
#else
    template <template <class, class> class TARGET_TYPE,
              template <class, class> class SOURCE_TYPE,
              class T1,
              class T2,
              class U1,
              class U2>
    static void construct(TARGET_TYPE<T1, T2> *address,
                          bslma::Allocator    *allocator,
                          bsl::integral_constant<int, e_PAIR_TRAITS> *,
                          const SOURCE_TYPE<U1, U2>& original);
        // Construct an object of 'pair'-like (template parameter) type
        // 'TARGET_TYPE' at the specified (uninitialized memory) 'address'
        // with the same value as the specified 'original' object, by invoking
        // either the move (if 'Arg' is a modifiable rvalue) or copy (if 'Arg'
        // is an lvalue or non-modifiable rvalue) constructor.  If 'allocator'
        // is of type 'bslma::Allocator' and any of the 'first' and 'second'
        // sub-elements of 'TARGET_TYPE' support 'bslma'-style allocators,
        // the 'allocator' is passed to the (move or copy) constructor when
        // creating the sub-element.  If the constructor throws, 'address' is
        // left in an uninitialized state.  Note that the behavior is undefined
        // if 'TARGET_TYPE' directly supports 'bslma'-style allocators.  Note
        // that this function is enabled only if the decayed type of the
        // argument to the constructor is the same as 'TARGET_TYPE'.
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
    template <class TARGET_TYPE, class Arg1>
    static void construct(TARGET_TYPE            *address,
                          bslma::Allocator       *allocator,
                          bsl::integral_constant<int, e_PAIR_TRAITS> *,
                          bslmf::MovableRef<Arg1> original);
#else
#if defined (BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    template <template <class, class> class TARGET_TYPE,
              template <class, class> class SOURCE_TYPE,
              class T1,
              class T2,
              class U1,
              class U2>
    static void construct(TARGET_TYPE<T1, T2> *address,
                          bslma::Allocator    *allocator,
                          bsl::integral_constant<int, e_PAIR_TRAITS> *,
                          SOURCE_TYPE<U1, U2>&& original);
#else
    template <template<class, class> class TARGET_TYPE,
              template<class, class> class SOURCE_TYPE,
              class T1,
              class T2,
              class U1,
              class U2>
    static void construct(TARGET_TYPE<T1, T2> *address,
                          bslma::Allocator    *allocator,
                          bsl::integral_constant<int, e_PAIR_TRAITS> *,
                          bslmf::MovableRef<SOURCE_TYPE<U1, U2> > original);
#endif
#endif
        // TBD: add comment

    template <class TARGET_TYPE, class Arg1, class Arg2>
    static void construct(TARGET_TYPE                            *address,
                          bslma::Allocator                       *allocator,
                          bsl::integral_constant<int, e_PAIR_TRAITS> *,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) first,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Arg2) second);
        // Construct an object of 'pair'-like (template parameter) type
        // 'TARGET_TYPE' at the specified (uninitialized memory) 'address'
        // initialized to the specified 'first' and 'second' sub-elements.
        // If 'allocator' is of type 'bslma::Allocator' and any of the 'first'
        // and 'second' sub-elements of 'TARGET_TYPE' support 'bslma'-style
        // allocators, the 'allocator' is passed to the constructor when
        // creating the sub-element.  If the constructor throws, 'address' is
        // left in an uninitialized state.  Note that the behavior is undefined
        // if 'TARGET_TYPE' directly supports 'bslma'-style allocators.  Note
        // that this function is enabled only if the decayed type of the
        // argument to the constructor is the same as 'TARGET_TYPE'.

#if defined(BSLS_LIBRARYFEATURES_SUPPORT_PIECEWISE_CONSTRUCT)
    template <class TARGET_TYPE, class Piecewise, class Tuple1, class Tuple2>
    static void construct(
             TARGET_TYPE                                 *address,
             bslma::Allocator                            *allocator,
             bsl::integral_constant<int, e_PAIR_TRAITS> *,
             BSLS_COMPILERFEATURES_FORWARD_REF(Piecewise) piecewise,
             BSLS_COMPILERFEATURES_FORWARD_REF(Tuple1)    first,
             BSLS_COMPILERFEATURES_FORWARD_REF(Tuple2)    second);
        // TBD: comment this
#endif

    // In order to implement
    //             'allocator_traits<A>::construct(m, p    )'
    //             'allocator_traits<A>::construct(m, p, rv)'
    //             'allocator_traits<A>::construct(m, p,  v)'
    //
    //                       if 'USES_BSLMA_ALLOCATOR_TRAITS'
    //      and
    //             'allocator_traits<A>::construct(m, p, args)'

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=15
    template <class TARGET_TYPE, class... Args>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  Args&&...         arguments);
    template <class TARGET_TYPE, class... Args>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  Args&&...         arguments);
        // TBD: fix comment
        // Construct an object of (template parameter) type 'TARGET_TYPE',
        // that supports 'bslma'-style allocators, at the specified
        // (uninitialized memory) 'address', invoking the constructor of
        // 'TARGET_TYPE' corresponding to the specified 'arguments'.  If
        // 'allocator' is of type 'bslma::Allocator', the allocator is passed
        // to the constructor.  If the constructor throws, 'address' is left
        // in an uninitialized state.

    template <class TARGET_TYPE, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                          Args&&...         arguments);

    template <class TARGET_TYPE, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                          Args&&...         arguments);
        // Construct an object of (template parameter) type 'TARGET_TYPE',
        // that does not support 'bslma'-style allocators, at the specified
        // (uninitialized memory) 'address', invoking the constructor of
        // 'TARGET_TYPE' corresponding to the specified 'arguments'.  If the
        // constructor throws, 'address' is left in an uninitialized state.
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslma_constructionutil.h
    template <class TARGET_TYPE>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *);

    template <class TARGET_TYPE, class Args_01>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13,
                                 class Args_14>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13,
                                 class Args_14,
                                 class Args_15>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);

    template <class TARGET_TYPE>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *);

    template <class TARGET_TYPE, class Args_01>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13,
                                 class Args_14>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13,
                                 class Args_14,
                                 class Args_15>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);


    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *);

    template <class TARGET_TYPE, class Args_01>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13,
                                 class Args_14>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13,
                                 class Args_14,
                                 class Args_15>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);


    template <class TARGET_TYPE>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *);

    template <class TARGET_TYPE, class Args_01>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13,
                                 class Args_14>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14);

    template <class TARGET_TYPE, class Args_01,
                                 class Args_02,
                                 class Args_03,
                                 class Args_04,
                                 class Args_05,
                                 class Args_06,
                                 class Args_07,
                                 class Args_08,
                                 class Args_09,
                                 class Args_10,
                                 class Args_11,
                                 class Args_12,
                                 class Args_13,
                                 class Args_14,
                                 class Args_15>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                      BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
    template <class TARGET_TYPE, class... Args>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
    template <class TARGET_TYPE, class... Args>
    static void construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);

    template <class TARGET_TYPE, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bslma::Allocator *allocator,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);

    template <class TARGET_TYPE, class... Args>
    static void construct(TARGET_TYPE      *address,
                          bsl::integral_constant<int, e_NIL_TRAITS> *,
                         BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments);
// }}} END GENERATED CODE
#endif
};


// ============================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ============================================================================


                          // -----------------------
                          // struct ConstructionUtil
                          // -----------------------

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil::destructiveMove(TARGET_TYPE *address,
                                  ALLOCATOR   *allocator,
                                  TARGET_TYPE *original)
{
    BSLS_ASSERT_SAFE(address);
    BSLS_ASSERT_SAFE(original);

    enum {
       k_VALUE = bslmf::IsBitwiseMoveable<TARGET_TYPE>::value
                 ? Imp::e_BITWISE_MOVABLE_TRAITS
                 : Imp::e_NIL_TRAITS
    };

    Imp::destructiveMove(address,
                         allocator,
                         (bsl::integral_constant<int, k_VALUE>*)0,
                         original);
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            bslma::Allocator *allocator)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bsl::is_trivially_default_constructible<TARGET_TYPE>::value
                    ? Imp::e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS
                    : bslmf::IsPair<TARGET_TYPE>::value
                        ? Imp::e_PAIR_TRAITS
                        : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *)
{
    enum {
        k_VALUE = bsl::is_trivially_default_constructible<TARGET_TYPE>::value
                ? Imp::e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address, (bsl::integral_constant<int, k_VALUE>*)0);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE        *address,
                                 bslma::Allocator   *allocator,
                                 const TARGET_TYPE&  original)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bsl::is_trivially_copyable<TARGET_TYPE>::value
                    ? Imp::e_BITWISE_COPYABLE_TRAITS
                    : bslmf::IsPair<TARGET_TYPE>::value
                        ? Imp::e_PAIR_TRAITS
                        : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE> *)0,
                   original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE        *address,
                                 bslma::Allocator   *allocator,
                                 TARGET_TYPE&  original)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bsl::is_trivially_copyable<TARGET_TYPE>::value
                    ? Imp::e_BITWISE_COPYABLE_TRAITS
                    : bslmf::IsPair<TARGET_TYPE>::value
                        ? Imp::e_PAIR_TRAITS
                        : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE> *)0,
                   original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE       *address,
                                 void              *,
                                 const TARGET_TYPE& original)
{
    enum {
        k_VALUE = bsl::is_trivially_copyable<TARGET_TYPE>::value
                ? Imp::e_BITWISE_COPYABLE_TRAITS
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE  *address,
                                 void         *,
                                 TARGET_TYPE&  original)
{
    enum {
        k_VALUE = bsl::is_trivially_copyable<TARGET_TYPE>::value
                ? Imp::e_BITWISE_COPYABLE_TRAITS
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE                    *address,
                                 bslma::Allocator               *allocator,
                                 bslmf::MovableRef<TARGET_TYPE>  original)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bsl::is_trivially_copyable<TARGET_TYPE>::value
                    ? Imp::e_BITWISE_COPYABLE_TRAITS
                    : bslmf::IsPair<TARGET_TYPE>::value
                        ? Imp::e_PAIR_TRAITS
                        : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE> *)0,
                   BSLS_COMPILERFEATURES_FORWARD(TARGET_TYPE,original));
}

template <class TARGET_TYPE>
inline
void ConstructionUtil::construct(TARGET_TYPE                    *address,
                                 void                           *,
                                 bslmf::MovableRef<TARGET_TYPE>  original)
{
    enum {
        k_VALUE = bsl::is_trivially_copyable<TARGET_TYPE>::value
                ? Imp::e_BITWISE_COPYABLE_TRAITS
                : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(TARGET_TYPE,original));
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=15
template <class TARGET_TYPE, class Arg1, class... Args>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            Arg1&&            argument1,
                            Args&&...         arguments)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}

template <class TARGET_TYPE, class Arg1, class... Args>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            Arg1&&       argument1,
                            Args&&...    arguments)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslma_constructionutil.h
template <class TARGET_TYPE, class Arg1>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1));
}

template <class TARGET_TYPE, class Arg1, class Args_01>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                   BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                   BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                   BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                   BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                   BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                   BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10,
                                         class Args_11>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                   BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                   BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                   BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                   BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10,
                                         class Args_11,
                                         class Args_12>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                   BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                   BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                   BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                   BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                   BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10,
                                         class Args_11,
                                         class Args_12,
                                         class Args_13>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                   BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                   BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                   BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                   BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                   BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                   BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10,
                                         class Args_11,
                                         class Args_12,
                                         class Args_13,
                                         class Args_14>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                   BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                   BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                   BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                   BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                   BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                   BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                   BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14));
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10,
                                         class Args_11,
                                         class Args_12,
                                         class Args_13,
                                         class Args_14,
                                         class Args_15>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                   BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                   BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                   BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                   BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                   BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                   BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                   BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                   BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                   BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                   BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                   BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                   BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                   BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                   BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15));
}


template <class TARGET_TYPE, class Arg1>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10,
                                         class Args_11>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10,
                                         class Args_11,
                                         class Args_12>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10,
                                         class Args_11,
                                         class Args_12,
                                         class Args_13>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10,
                                         class Args_11,
                                         class Args_12,
                                         class Args_13,
                                         class Args_14>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                          BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Arg1, class Args_01,
                                         class Args_02,
                                         class Args_03,
                                         class Args_04,
                                         class Args_05,
                                         class Args_06,
                                         class Args_07,
                                         class Args_08,
                                         class Args_09,
                                         class Args_10,
                                         class Args_11,
                                         class Args_12,
                                         class Args_13,
                                         class Args_14,
                                         class Args_15>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                          BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                          BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class TARGET_TYPE, class Arg1, class... Args>
inline
void
ConstructionUtil::construct(TARGET_TYPE      *address,
                            bslma::Allocator *allocator,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    enum {
        k_VALUE = bslma::UsesBslmaAllocator<TARGET_TYPE>::value
                ? (bslmf::UsesAllocatorArgT<TARGET_TYPE>::value
                 ? Imp::e_USES_ALLOCATOR_ARG_T_TRAITS
                 : Imp::e_USES_BSLMA_ALLOCATOR_TRAITS)
                : bslmf::IsPair<TARGET_TYPE>::value
                    ? Imp::e_PAIR_TRAITS
                    : Imp::e_NIL_TRAITS
    };
    Imp::construct(address,
                   allocator,
                   (bsl::integral_constant<int, k_VALUE>*)0,
                   BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                   BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}

template <class TARGET_TYPE, class Arg1, class... Args>
inline
void
ConstructionUtil::construct(TARGET_TYPE *address,
                            void        *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) argument1,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Arg1,argument1),
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
// }}} END GENERATED CODE
#endif

                       // ---------------------------
                       // struct ConstructionUtil_Imp
                       // ---------------------------

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil_Imp::destructiveMove(TARGET_TYPE *address,
                                      ALLOCATOR   *,
                      bsl::integral_constant<int, e_BITWISE_MOVABLE_TRAITS> *,
                                      TARGET_TYPE *original)
{
    if (bsl::is_fundamental<TARGET_TYPE>::value
     || bsl::is_pointer<TARGET_TYPE>::value) {
         ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(*original);
         BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
     }
     else {
         memcpy(address, original, sizeof *original);
     }
}

template <class TARGET_TYPE, class ALLOCATOR>
inline
void
ConstructionUtil_Imp::destructiveMove(TARGET_TYPE *address,
                                      ALLOCATOR   *allocator,
                                  bsl::integral_constant<int, e_NIL_TRAITS> *,
                                      TARGET_TYPE *original)
{
    // TBD: should be ok with C++03 as well, but need to test edge cases first
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    ConstructionUtil::construct(address,
                                allocator,
                                bslmf::MovableRefUtil::move(*original));
#else
    ConstructionUtil::construct(address,
                                allocator,
                                *original);
#endif
    DestructionUtil::destroy(original);
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *allocator,
                                bsl::integral_constant<int, e_PAIR_TRAITS> *)
{
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->first)),
                                allocator);
    DestructorProctor<typename bsl::remove_cv<
                                   typename TARGET_TYPE::first_type>::type>
                         proctor(unconst(BSLS_UTIL_ADDRESSOF(address->first)));
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->second)),
                                allocator);
    proctor.release();
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(
              TARGET_TYPE *address,
              bslma::Allocator *,
              bsl::integral_constant<int, e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *)
{
    construct(address,
              (bsl::integral_constant<int,
                                      e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *) 0);
}

template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(
              TARGET_TYPE *address,
              bsl::integral_constant<int, e_HAS_TRIVIAL_DEFAULT_CTOR_TRAITS> *)
{
    if (bsl::is_fundamental<TARGET_TYPE>::value
     || bsl::is_pointer<TARGET_TYPE>::value) {

        ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE();
        BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
    }
    else {
        memset((char *) address, 0, sizeof *address);
    }
}

template <class TARGET_TYPE>
inline
void ConstructionUtil_Imp::construct(TARGET_TYPE       *address,
                                     bslma::Allocator *,
                      bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
                                     const TARGET_TYPE& original)
{
    construct(address,
              (bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS>*)0,
               original);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil_Imp::construct(TARGET_TYPE       *address,
                      bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
                                     const TARGET_TYPE& original)
{
    if (bsl::is_fundamental<TARGET_TYPE>::value
     || bsl::is_pointer<TARGET_TYPE>::value) {

        ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(original);
        BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
    }
    else {
        memcpy(address,
               BSLS_UTIL_ADDRESSOF(original),
               sizeof original);
    }
}

template <class TARGET_TYPE>
inline
void ConstructionUtil_Imp::construct(TARGET_TYPE       *address,
                                     bslma::Allocator *,
                      bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
                                     bslmf::MovableRef<TARGET_TYPE> original)
{
    const TARGET_TYPE& lvalue = original;
    construct(address,
              (bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS>*)0,
               lvalue);
}

template <class TARGET_TYPE>
inline
void ConstructionUtil_Imp::construct(TARGET_TYPE       *address,
                      bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS> *,
                                     bslmf::MovableRef<TARGET_TYPE> original)
{
    const TARGET_TYPE& lvalue = original;
    construct(address,
              (bsl::integral_constant<int, e_BITWISE_COPYABLE_TRAITS>*)0,
               lvalue);
}

#if defined(BSLS_PLATFORM_CMP_SUN)
template <class TARGET_TYPE, class Arg1>
inline
void ConstructionUtil_Imp::construct(
                            TARGET_TYPE                            *address,
                            bslma::Allocator                       *allocator,
                            bsl::integral_constant<int, e_PAIR_TRAITS> *,
                            BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) original)
{
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->first)),
                                allocator,
                                original.first);
    DestructorProctor<typename bsl::remove_cv<
                                      typename TARGET_TYPE::first_type>::type>
                         proctor(unconst(BSLS_UTIL_ADDRESSOF(address->first)));
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->second)),
                                allocator,
                                original.second);
    proctor.release();
}
#else
template <template <class, class> class TARGET_TYPE,
          template <class, class> class SOURCE_TYPE,
          class T1,
          class T2,
          class U1,
          class U2>
inline
void ConstructionUtil_Imp::construct(
                                  TARGET_TYPE<T1, T2> *address,
                                  bslma::Allocator    *allocator,
                                  bsl::integral_constant<int, e_PAIR_TRAITS> *,
                                  const SOURCE_TYPE<U1, U2>& original)
{
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->first)),
                                allocator,
                                original.first);
    DestructorProctor<typename bsl::remove_cv<T1>::type>
                         proctor(unconst(BSLS_UTIL_ADDRESSOF(address->first)));
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->second)),
                                allocator,
                                original.second);
    proctor.release();
}
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
template <class TARGET_TYPE, class Arg1>
inline
void ConstructionUtil_Imp::construct(TARGET_TYPE            *address,
                               bslma::Allocator       *allocator,
                               bsl::integral_constant<int, e_PAIR_TRAITS> *,
                               bslmf::MovableRef<Arg1> original)
{
    Arg1& lvalue = original;

    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->first)),
                                allocator,
                                bslmf::MovableRefUtil::move(lvalue.first));
    DestructorProctor<typename bsl::remove_cv<
                                      typename TARGET_TYPE::first_type>::type>
                         proctor(unconst(BSLS_UTIL_ADDRESSOF(address->first)));
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->second)),
                                allocator,
                                bslmf::MovableRefUtil::move(lvalue.second));
    proctor.release();
}
#else
#if defined (BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <template <class, class> class TARGET_TYPE,
          template <class, class> class SOURCE_TYPE,
          class T1,
          class T2,
          class U1,
          class U2>
inline
void ConstructionUtil_Imp::construct(
                                  TARGET_TYPE<T1, T2> *address,
                                  bslma::Allocator    *allocator,
                                  bsl::integral_constant<int, e_PAIR_TRAITS> *,
                                  SOURCE_TYPE<U1, U2>&& original)
{
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->first)),
                                allocator,
                                bslmf::MovableRefUtil::move(original.first));
    DestructorProctor<typename bsl::remove_cv<T1>::type>
                         proctor(unconst(BSLS_UTIL_ADDRESSOF(address->first)));
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->second)),
                                allocator,
                                bslmf::MovableRefUtil::move(original.second));
    proctor.release();
}
#else
template <template <class, class> class TARGET_TYPE,
          template <class, class> class SOURCE_TYPE,
          class T1,
          class T2,
          class U1,
          class U2>
inline
void ConstructionUtil_Imp::construct(
                                  TARGET_TYPE<T1, T2> *address,
                                  bslma::Allocator    *allocator,
                                  bsl::integral_constant<int, e_PAIR_TRAITS> *,
                              bslmf::MovableRef<SOURCE_TYPE<U1, U2> > original)
{
    SOURCE_TYPE<U1, U2>& lvalue = original;
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->first)),
                                allocator,
                                bslmf::MovableRefUtil::move(lvalue.first));
    DestructorProctor<typename bsl::remove_cv<T1>::type> proctor(
                                 unconst(BSLS_UTIL_ADDRESSOF(address->first)));
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->second)),
                                allocator,
                                bslmf::MovableRefUtil::move(lvalue.second));
    proctor.release();
}
#endif
#endif

template <class TARGET_TYPE, class Arg1, class Arg2>
inline
void ConstructionUtil_Imp::construct(
                             TARGET_TYPE                            *address,
                             bslma::Allocator                       *allocator,
                             bsl::integral_constant<int, e_PAIR_TRAITS> *,
                             BSLS_COMPILERFEATURES_FORWARD_REF(Arg1) first,
                             BSLS_COMPILERFEATURES_FORWARD_REF(Arg2) second)
{
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->first)),
                                allocator,
                                BSLS_COMPILERFEATURES_FORWARD(Arg1,first));
    DestructorProctor<typename bsl::remove_cv<
                                  typename TARGET_TYPE::first_type>::type>
                         proctor(unconst(BSLS_UTIL_ADDRESSOF(address->first)));
    ConstructionUtil::construct(unconst(BSLS_UTIL_ADDRESSOF(address->second)),
                                allocator,
                                BSLS_COMPILERFEATURES_FORWARD(Arg2,second));
    proctor.release();
}

#if defined(BSLS_LIBRARYFEATURES_SUPPORT_PIECEWISE_CONSTRUCT)
template <class TARGET_TYPE, class Piecewise, class Tuple1, class Tuple2>
inline
void ConstructionUtil_Imp::construct(
             TARGET_TYPE                                 *address,
             bslma::Allocator                            *allocator,
             bsl::integral_constant<int, e_PAIR_TRAITS> *,
             BSLS_COMPILERFEATURES_FORWARD_REF(Piecewise) piecewise,
             BSLS_COMPILERFEATURES_FORWARD_REF(Tuple1)    first,
             BSLS_COMPILERFEATURES_FORWARD_REF(Tuple2)    second)
{
    construct(address, allocator,
              (bsl::integral_constant<int, e_NIL_TRAITS> *) 0,
              BSLS_COMPILERFEATURES_FORWARD(Piecewise, piecewise),
              BSLS_COMPILERFEATURES_FORWARD(Tuple1,    first),
              BSLS_COMPILERFEATURES_FORWARD(Tuple2,    second));
}
#endif

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=15
template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  Args&&...         arguments)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args,arguments)..., allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  Args&&...         arguments)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                                Args&&...         arguments)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                                Args&&...         arguments)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslma_constructionutil.h
template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                  BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                  BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                  BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                  BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                  BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                  BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                  BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                  BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                  BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                  BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                  BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                  BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                  BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                  BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                  BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13,
                             class Args_14>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                  BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                  BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                  BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                  BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                  BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                  BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13,
                             class Args_14,
                             class Args_15>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                  BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                  BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                  BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                  BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                  BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                  BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                  BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                  BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                  BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                  BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                  BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                  BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                  BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                  BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15),
                  allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}


template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13,
                             class Args_14>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                          BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13,
                             class Args_14,
                             class Args_15>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                          BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                          BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}


template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0);
}

template <class TARGET_TYPE, class Args_01>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
              BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
              BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
              BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
              BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
              BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
              BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
              BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
              BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
              BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
              BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
              BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
              BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
              BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
              BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
              BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
              BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
              BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
              BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
              BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
              BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
              BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13,
                             class Args_14>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
              BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
              BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
              BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
              BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
              BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
              BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
              BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14));
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13,
                             class Args_14,
                             class Args_15>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
              BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
              BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
              BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
              BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
              BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
              BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
              BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
              BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
              BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
              BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
              BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
              BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
              BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
              BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15));
}


template <class TARGET_TYPE>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             );
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13,
                             class Args_14>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                          BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class Args_01,
                             class Args_02,
                             class Args_03,
                             class Args_04,
                             class Args_05,
                             class Args_06,
                             class Args_07,
                             class Args_08,
                             class Args_09,
                             class Args_10,
                             class Args_11,
                             class Args_12,
                             class Args_13,
                             class Args_14,
                             class Args_15>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_01) arguments_01,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_02) arguments_02,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_03) arguments_03,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_04) arguments_04,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_05) arguments_05,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_06) arguments_06,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_07) arguments_07,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_08) arguments_08,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_09) arguments_09,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_10) arguments_10,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_11) arguments_11,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_12) arguments_12,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_13) arguments_13,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_14) arguments_14,
                       BSLS_COMPILERFEATURES_FORWARD_REF(Args_15) arguments_15)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                          BSLS_COMPILERFEATURES_FORWARD(Args_01,arguments_01),
                          BSLS_COMPILERFEATURES_FORWARD(Args_02,arguments_02),
                          BSLS_COMPILERFEATURES_FORWARD(Args_03,arguments_03),
                          BSLS_COMPILERFEATURES_FORWARD(Args_04,arguments_04),
                          BSLS_COMPILERFEATURES_FORWARD(Args_05,arguments_05),
                          BSLS_COMPILERFEATURES_FORWARD(Args_06,arguments_06),
                          BSLS_COMPILERFEATURES_FORWARD(Args_07,arguments_07),
                          BSLS_COMPILERFEATURES_FORWARD(Args_08,arguments_08),
                          BSLS_COMPILERFEATURES_FORWARD(Args_09,arguments_09),
                          BSLS_COMPILERFEATURES_FORWARD(Args_10,arguments_10),
                          BSLS_COMPILERFEATURES_FORWARD(Args_11,arguments_11),
                          BSLS_COMPILERFEATURES_FORWARD(Args_12,arguments_12),
                          BSLS_COMPILERFEATURES_FORWARD(Args_13,arguments_13),
                          BSLS_COMPILERFEATURES_FORWARD(Args_14,arguments_14),
                          BSLS_COMPILERFEATURES_FORWARD(Args_15,arguments_15));
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.
template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_BSLMA_ALLOCATOR_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                  BSLS_COMPILERFEATURES_FORWARD(Args,arguments)..., allocator);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(
                  TARGET_TYPE      *address,
                  bslma::Allocator *allocator,
                  bsl::integral_constant<int, e_USES_ALLOCATOR_ARG_T_TRAITS> *,
                  BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             bsl::allocator_arg,
                             allocator,
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bslma::Allocator *,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    construct(address,
              (bsl::integral_constant<int, e_NIL_TRAITS> *)0,
              BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
}

template <class TARGET_TYPE, class... Args>
inline
void
ConstructionUtil_Imp::construct(TARGET_TYPE      *address,
                                bsl::integral_constant<int, e_NIL_TRAITS> *,
                          BSLS_COMPILERFEATURES_FORWARD_REF(Args)... arguments)
{
    ::new (tmp::ImpUtil::voidify(address)) TARGET_TYPE(
                             BSLS_COMPILERFEATURES_FORWARD(Args,arguments)...);
    BSLMA_CONSTRUCTIONUTIL_XLC_PLACEMENT_NEW_FIX;
}
// }}} END GENERATED CODE
#endif

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
