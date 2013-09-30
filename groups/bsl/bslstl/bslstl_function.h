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

#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#include <bsls_exceptionutil.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

// Forward declarations
namespace bsl {

template <class FUNC>
class function;  // Primary template declared but not defined.

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
        // representation.  The design uses the "small-object optimization" in
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
        // member ensures that 'InplaceBuffer' is at least large enough so that
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


                    // =======================
                    // class template function
                    // =======================

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

template <class RET, class... ARGS>
class function<RET(ARGS...)> : public Function_ArgTypes<RET(ARGS...)>
{
    // An instantiation of this class template generalizes the notion of a
    // pointer to a function taking the specified 'ARGS' types and returning
    // the specified 'RET' type, i.e., a function pointer of type
    // 'RET(*)(ARGS)'.  An object of this class wraps a run-time invokable
    // object specified at construction, such as a function pointer, member
    // function pointer, or functor.  Note that 'function' is defined only for
    // template parameters that specify a function prototype; the primary
    // template (taking an arbitrary template parameter) is not defined.

public:
    // PUBLIC TYPES
    typedef RET result_type;

    // CREATORS
    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class F> function(F);
    template<class A> function(allocator_arg_t, const A&) BSLS_NOTHROW_SPEC;
    template<class A> function(allocator_arg_t, const A&,
                               nullptr_t) BSLS_NOTHROW_SPEC;
    template<class A> function(allocator_arg_t, const A&,
                               const function&);
    template<class F, class A> function(allocator_arg_t, const A&, F);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class A> function(allocator_arg_t, const A&, function&&);
#endif

    ~function();

    // MANIPULATORS
    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class F>
    function& operator=(F&&);

    // TBD: Need to implement reference_wrapper.
    // template<class F>
    // function& operator=(reference_wrapper<F>) BSLS_NOTHROW_SPEC;

    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class F, class A> void assign(F&&, const A&);

    explicit operator bool() const BSLS_NOTHROW_SPEC;

    R operator()(ArgTypes...) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
};

// FREE FUNCTIONS
template <class R, class... ARGS>
bool operator==(const function<R(ARGS...)>&, nullptr_t) BSL_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator==(nullptr_t, const function<R(ARGS...)>&) BSL_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator!=(const function<R(ARGS...)>&, nullptr_t) BSL_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator!=(nullptr_t, const function<R(ARGS...)>&) BSL_NOTHROW_SPEC;

template <class R, class... ARGS>
void swap(function<R(ARGS...)>&, function<R(ARGS...)>&);

#endif

}  // close namespace bsl

// ===========================================================================
//                      TEMPLATE DEFINITIONS
// ===========================================================================

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

// CREATORS
template <class R, class... ARGS>
bsl::function<R(ARGS...)>::function() BSLS_NOTHROW_SPEC
{
}

template <class R, class... ARGS>
bsl::function<R(ARGS...)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
}

template <class R, class... ARGS>
bsl::function<R(ARGS...)>::function(const function&)
{
}

template <class R, class... ARGS>
template<class F>
bsl::function<R(ARGS...)>::function(F)
{
}

template <class R, class... ARGS>
template<class A>
bsl::function<R(ARGS...)>::function(allocator_arg_t, const A&)
    BSLS_NOTHROW_SPEC
{
}

template <class R, class... ARGS>
template<class A>
bsl::function<R(ARGS...)>::function(allocator_arg_t, const A&,
                                    nullptr_t) BSLS_NOTHROW_SPEC
{
}

template <class R, class... ARGS>
template<class A>
bsl::function<R(ARGS...)>::function(allocator_arg_t, const A&,
                                    const function&)
{
}

template <class R, class... ARGS>
template<class F, class A>
bsl::function<R(ARGS...)>::function(allocator_arg_t, const A&, F)
{
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class R, class... ARGS>
bsl::function<R(ARGS...)>::function(function&&)
{
}

template <class R, class... ARGS>
template<class A>
bsl::function<R(ARGS...)>::function(allocator_arg_t, const A&, function&&)
{
}

#endif //  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class R, class... ARGS>
bsl::function<R(ARGS...)>::~function()
{
}

// MANIPULATORS
template <class R, class... ARGS>
function& bsl::function<R(ARGS...)>::operator=(const function&)
{
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class R, class... ARGS>
function& bsl::function<R(ARGS...)>::operator=(function&&)
{
}

#endif

template <class R, class... ARGS>
function& bsl::function<R(ARGS...)>::operator=(nullptr_t)
{
}

template <class R, class... ARGS>
template<class F>
function& bsl::function<R(ARGS...)>::operator=(F&&)
{
}

// TBD: Need to implement reference_wrapper.
// template <class R, class... ARGS>
// template<class F>
// function& bsl::function<R(ARGS...)>::operator=(reference_wrapper<F>)
//     BSLS_NOTHROW_SPEC

template <class R, class... ARGS>
void bsl::function<R(ARGS...)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class R, class... ARGS>
template<class F, class A>
void bsl::function<R(ARGS...)>::assign(F&&, const A&)
{
}

template <class R, class... ARGS>
bsl::function<R(ARGS...)>::operator bool() const BSLS_NOTHROW_SPEC
{
}

template <class R, class... ARGS>
R bsl::function<R(ARGS...)>::operator()(ArgTypes...) const
{
}

template <class R, class... ARGS>
const std::type_info&
bsl::function<R(ARGS...)>::target_type() const BSLS_NOTHROW_SPEC
{
}

template <class R, class... ARGS>
template<class T>
T* bsl::function<R(ARGS...)>::target() BSLS_NOTHROW_SPEC
{
}

template <class R, class... ARGS>
template<class T>
const T* bsl::function<R(ARGS...)>::target() const BSLS_NOTHROW_SPEC
{
}

// FREE FUNCTIONS
template <class R, class... ARGS>
bool operator==(const function<R(ARGS...)>&, nullptr_t) BSL_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator==(nullptr_t, const function<R(ARGS...)>&) BSL_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator!=(const function<R(ARGS...)>&, nullptr_t) BSL_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator!=(nullptr_t, const function<R(ARGS...)>&) BSL_NOTHROW_SPEC;

template <class R, class... ARGS>
void swap(function<R(ARGS...)>&, function<R(ARGS...)>&);

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
