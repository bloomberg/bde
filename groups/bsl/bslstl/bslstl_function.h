// bslstl_function.h                  -*-C++-*-
#ifndef INCLUDED_BSLSTL_FUNCTION
#define INCLUDED_BSLSTL_FUNCTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a polymorphic function object with a specific prototype.
//
//@CLASSES:
// bsl::function: polymorphic function object with a specific prototype.
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a polymorphic function object
// (functor) that can be invoked like a function and that wraps a run-time
// invokable object such as a function pointer, member function pointer, or
// functor.  The return type and the number and type of arguments at
// invocation are specified by the template parameter, which is a function
// prototype.  A "functor" is similar to a C/C++ function pointer, but unlike
// function pointers, functors can be used to invoke any object that can be
// syntactically invoked as a function.  Objects of type 'bsl::function' are
// generally used as callback functions to avoid templatizing a function or
// class.
//
///Usage
///-----

// Prevent this header from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_function.h> instead of <bslmf_function.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

// Forward declarations
namespace bsl {

template <class FUNC>
class function;  // Primary template declared but not defined.

}

namespace BloombergLP {

namespace bslstl {

                        // ================================
                        // class template Function_ArgTypes
                        // ================================

template <class FUNC>
class Function_ArgTypes {
    // This is a component-private class template.  Do not use.  This template
    // provides the following nested typedefs:
    //..
    //  argument_type        -- Only if FUNC takes exactly one argument
    //  first_argument_type  -- Only if FUNC takes exactly two arguments
    //  second_argument_type -- Only if FUNC takes exactly two arguments

public:
    // No typedefs for the unspecialized case
};

template <class R, class ARG>
class Function_ArgTypes<R(ARG)> {
    // Specialization for functions that take exactly one argument.

public:
    typedef ARG argument_type;
};

template <class R, class ARG1, class ARG2>
class Function_ArgTypes<R(ARG1, ARG2)> {
    // Specialization for functions that take exactly two arguments.

public:
    typedef ARG1 first_argument_type;
    typedef ARG2 second_argument_type;
};

                        // ==================
                        // class Function_Rep
                        // ==================

class Function_Rep {
    // This is a component-private class.  Do not use.  This class provides a
    // non-template representation for a 'bsl::function' instance.  For
    // technical reasons, this class must be defined before 'bsl::function'
    // (although a mere forward declaration would be all right with most
    // compilers, the Gnu compiler emits an error when trying to do syntactic
    // checking on template code even though it does not instantiate
    // template).

    // TYPES
    enum DispatcherOpCode {
        // This enumeration provide values to identify operations to be
        // performed by the dispatcher function (below).

        MOVE_CONSTRUCT    = 0
      , COPY_CONSTRUCT    = 1
      , CONSTRUCT         = 2
      , DESTROY           = 3
      , INPLACE_DETECTION = 4
    };

    typedef bool (*Dispatcher)(DispatcherOpCode  opCode,
                               Function_Rep     *rep,
                               const void       *source);
        // 'Dispatcher' is an alias for a pointer to a function that
        // implements a kind of hand-coded virtual-function dispatch.  The
        // specified 'opCode' is used to choose the "virtual function" to
        // invoke, where the specified 'rep' and 'source' are arguments to
        // that function.  Internally, a 'Dispatcher' function will likely use
        // a 'switch' statement rather than preforming a virtual-table
        // lookup.  This mechanism is chosen because it saves a significant
        // amount of space over the C++ virtual-function mechanism, especially
        // when the number of different instantiations of 'bsl::function' is
        // large.

    union InplaceBuffer {
        // This 'struct' defines the storage area for a functor
        // representation.  Its design uses the "small-object optimization" in
        // an attempt to avoid allocations for objects that are no larger than
        // 'InplaceBuffer'.  When using the in-place representation, the
        // invocable, whether a function pointer or function object (if it
        // should fit in the size of 'InplaceBuffer') is stored directly in
        // the 'InplaceBuffer'.  Anything bigger than 'sizeof(InplaceBuffer)'
        // will be stored out-of-place and its address will be stored in
        // 'd_object_p'.  Discriminating between the two representations can
        // be done by the dispatcher with the opcode 'INPLACE_DETECTION'.
        //
        // Note that union members other than 'd_object_p' are just fillers to
        // make sure that a function or member function pointer can fit
        // without allocation and that alignment is respected.  The 'd_minbuf'
        // member ensures that 'InplaceBuffer' is at large enough so that
        // modestly-complex functors (e.g., functors that own embedded
        // arguments, such as 'bdef_Bind' objects) to be constructed in-place
        // without triggering further allocation.  The benefit of avoiding
        // allocation for those function objects is balanced against the waste
        // of space when used with smaller function objects.

        void                                *d_object_p;  // pointer to
                                                          // external
                                                          // representation

        void                               (*d_func_p)(); // pointer to
                                                          // function

        void (bslma::Allocator::*            d_memfnPtr_p)();
                                                          // pointer to
                                                          // member
                                                          // function

        bsls::AlignmentUtil::MaxAlignedType  d_align;     // force align
        char                                 d_minbuf[4*sizeof(void*)];
                                                          // force minimum size
    };

    template <class FUNC>
    friend class bsl::function;

  private:
    // DATA
    InplaceBuffer     d_objbuf;      // in-place representation (if raw
                                     // function pointer or as indicated by
                                     // the dispatcher), or pointer to
                                     // external representation

    Dispatcher        d_dispatcher_p;// pointer to dispatcher function used to
                                     // operate on function object instance
                                     // (which knows about the actual type
                                     // 'FUNC' of the function object), or 0
                                     // for raw function pointers

    bslma::Allocator *d_allocator_p; // allocator (held, not owned)

};

}  // close package namespace

}  // close enterprise namespace

namespace bsl {

                    // =======================
                    // class template function
                    // =======================

template <class FUNC>
class function;  // Primary template is not defined.

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

template <class RET, class... ARGS>
class function<RET(ARGS...)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS...)>
{
public:
    typedef RET result_type;
};

#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_function.h bslstl_function.t.cpp

template <class RET>
class function<RET()> :
        public BloombergLP::bslstl::Function_ArgTypes<RET()>
{
public:
    typedef RET result_type;
};

template <class RET, class ARGS_01>
class function<RET(ARGS_01)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS_01)>
{
public:
    typedef RET result_type;
};

template <class RET, class ARGS_01,
                     class ARGS_02>
class function<RET(ARGS_01,
                   ARGS_02)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS_01,
                                                          ARGS_02)>
{
public:
    typedef RET result_type;
};

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
class function<RET(ARGS_01,
                   ARGS_02,
                   ARGS_03)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS_01,
                                                          ARGS_02,
                                                          ARGS_03)>
{
public:
    typedef RET result_type;
};

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
class function<RET(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS_01,
                                                          ARGS_02,
                                                          ARGS_03,
                                                          ARGS_04)>
{
public:
    typedef RET result_type;
};

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
class function<RET(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS_01,
                                                          ARGS_02,
                                                          ARGS_03,
                                                          ARGS_04,
                                                          ARGS_05)>
{
public:
    typedef RET result_type;
};

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
class function<RET(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05,
                   ARGS_06)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS_01,
                                                          ARGS_02,
                                                          ARGS_03,
                                                          ARGS_04,
                                                          ARGS_05,
                                                          ARGS_06)>
{
public:
    typedef RET result_type;
};

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
class function<RET(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05,
                   ARGS_06,
                   ARGS_07)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS_01,
                                                          ARGS_02,
                                                          ARGS_03,
                                                          ARGS_04,
                                                          ARGS_05,
                                                          ARGS_06,
                                                          ARGS_07)>
{
public:
    typedef RET result_type;
};

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
class function<RET(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05,
                   ARGS_06,
                   ARGS_07,
                   ARGS_08)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS_01,
                                                          ARGS_02,
                                                          ARGS_03,
                                                          ARGS_04,
                                                          ARGS_05,
                                                          ARGS_06,
                                                          ARGS_07,
                                                          ARGS_08)>
{
public:
    typedef RET result_type;
};

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
class function<RET(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05,
                   ARGS_06,
                   ARGS_07,
                   ARGS_08,
                   ARGS_09)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS_01,
                                                          ARGS_02,
                                                          ARGS_03,
                                                          ARGS_04,
                                                          ARGS_05,
                                                          ARGS_06,
                                                          ARGS_07,
                                                          ARGS_08,
                                                          ARGS_09)>
{
public:
    typedef RET result_type;
};

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09,
                     class ARGS_10>
class function<RET(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05,
                   ARGS_06,
                   ARGS_07,
                   ARGS_08,
                   ARGS_09,
                   ARGS_10)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS_01,
                                                          ARGS_02,
                                                          ARGS_03,
                                                          ARGS_04,
                                                          ARGS_05,
                                                          ARGS_06,
                                                          ARGS_07,
                                                          ARGS_08,
                                                          ARGS_09,
                                                          ARGS_10)>
{
public:
    typedef RET result_type;
};

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.

template <class RET, class... ARGS>
class function<RET(ARGS...)> :
        public BloombergLP::bslstl::Function_ArgTypes<RET(ARGS...)>
{
public:
    typedef RET result_type;
};

// }}} END GENERATED CODE
#endif

}  // close namespace bsl

// ===========================================================================
//                      TEMPLATE DEFINITIONS
// ===========================================================================

namespace BloombergLP {

namespace bslstl {

// CLASS METHODS

// CREATORS

// MANIPULATORS

// ACCESSORS

// FREE OPERATORS

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLSTL_FUNCTION)

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
