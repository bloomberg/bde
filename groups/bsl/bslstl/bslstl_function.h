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
// bsl::bad_function_call: exception object thrown when invoking null function
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
#error "include <bsl_function.h> instead of <bslstl_function.h> in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_PAIR
#include <bslstl_pair.h>
#endif

#ifndef INCLUDED_BSLS_NULLPTR
#include <bsls_nullptr.h>
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

#ifndef INCLUDED_BSLMA_ALLOCATORADAPTOR
#include <bslma_allocatoradaptor.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_MEMBERFUNCTIONPOINTERTRAITS
#include <bslmf_memberfunctionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_ADDLVALUEREFERENCE
#include <bslmf_addlvaluereference.h>
#endif

#ifndef INCLUDED_BSLMF_NTHPARAMETER
#include <bslmf_nthparameter.h>
#endif

#ifndef INCLUDED_BSLMF_ISEMPTY
#include <bslmf_isempty.h>
#endif

#ifndef INCLUDED_BSLMF_SELECTTRAIT
#include <bslmf_selecttrait.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef INCLUDED_BSLMF_FORWARDINGTYPE
#include <bslmf_forwardingtype.h>
#endif

#ifndef INCLUDED_UTILITY
#include <utility>
#define INCLUDED_UTILITY
#endif

#ifndef INCLUDED_TYPEINFO
#include <typeinfo>
#define INCLUDED_TYPEINFO
#endif

// TBD: Remove this when BloombergLP is removed from bsl package group
using namespace BloombergLP;

// TBD: Move into its own component (or into uses_allocator component)
struct allocator_arg_t { };
static const allocator_arg_t allocator_arg;

// Forward declarations
namespace bsl {

template <class FUNC>
class function;  // Primary template declared but not defined.

template <class MEM_FUNC_PTR, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke; // Primary template declared but not defined.

                        // =======================
                        // class bad_function_call
                        // =======================

class bad_function_call : public std::exception {
    // Standard exception object thrown when attempting to invoke a null
    // function object.

public:
    bad_function_call() BSLS_NOTHROW_SPEC;
};

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
    //..

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

                        // ======================
                        // class Function_ObjSize
                        // ======================

template <class TYPE>
struct Function_ObjSize {
    // Metafunction to return the amount of memory needed hold TYPE.  If
    // 'TYPE' is 'void' or an empty class, then the 'VALUE' member will be 0;
    // otherwise the 'VALUE' member will be sizeof(TYPE).

    static const std::size_t VALUE = (bsl::is_empty<TYPE>::value ? 0 :
                                      sizeof(TYPE));
};

template <>
struct Function_ObjSize<void> {
    static const std::size_t VALUE = 0;
};

                        // ===========================
                        // class Function_ObjAlignment
                        // ===========================

template <class TYPE>
struct Function_ObjAlignment {
    // Metafunction to return the alignment needed for TYPE.  If 'TYPE' is
    // 'void', then the 'VALUE' member will be 1; otherwise the 'VALUE' member
    // will be bsls::AlignmentFromType<TYPE>::VALUE.

    static const std::size_t VALUE = bsls::AlignmentFromType<TYPE>::VALUE;
                                     
};

template <>
struct Function_ObjAlignment<void> {
    static const std::size_t VALUE = 1;
};

                        // ================================
                        // class Function_ObjPairBufferDesc
                        // ================================

class Function_ObjPairBufferDesc {
    // Descriptor for a memory buffer that can hold two objects.
    // TBD: Needs more description.
    // TBD: Is this worthy of being in its own component?

    std::size_t d_firstSize;
    std::size_t d_firstAlign;
    std::size_t d_secondSize;
    std::size_t d_secondAlign;
    std::size_t d_secondOffset;
    std::size_t d_totalSize;
    std::size_t d_totalAlign;

public:
    // MANIPULATORS
    template <class TYPE1, class TYPE2>
    void setDescriptor();

    // ACCESSORS
    void       *first(void       *buffer);
    void const *first(void const *buffer);
    void       *second(void       *buffer);
    void const *second(void const *buffer);

    std::size_t totalSize() const;
    std::size_t totalAlign() const;
    std::size_t firstSize() const;
//    std::size_t firstAlign() const;
    std::size_t secondSize() const;
//    std::size_t secondAlign() const;
//    std::size_t secondOffset() const;
};

                        // ==================
                        // class Function_Rep
                        // ==================

class Function_Rep {
    // This is a component-private class.  Do not use.  This class provides a
    // non-template representation for a 'bsl::function' instance.  It handles
    // all of the object-management parts of 'bsl::function' that are not
    // specific to the argument list or return type: storing, copying, and
    // moving the function object and allocator, but not invoking the function
    // (which would require knowledge of the function prototype). These
    // management functions are run-time polymorphic, and therefore do not
    // require that this class be templated (although several of the member
    // functions are templated). For technical reasons, this class must be
    // defined before 'bsl::function' (although a mere forward declaration
    // would be all right with most compilers, the Gnu compiler emits an error
    // when trying to do syntactic checking on template code even though it
    // does not instantiate template).

    // TYPES

    // Short aliases for type with maximum platform alignment
    typedef bsls::AlignmentUtil::MaxAlignedType MaxAlignedType;

    enum ManagerOpCode {
        // This enumeration provide values to identify operations to be
        // performed by an object manager function (below).

        MOVE_CONSTRUCT
      , COPY_CONSTRUCT
      , DESTROY
      , INPLACE_DETECTION
      , GET_TARGET
      , GET_TYPE_ID
    };

    enum FuncType {
        EMPTY_FUNC
      , FUNCTION_POINTER
      , MEMBER_FUNCTION_POINTER
      , IN_PLACE_FUNCTOR
      , OUT_OF_PLACE_FUNCTOR
    };

    enum AllocType {
        BSLMA_ALLOC_PTR
      , BSL_ALLOCATOR
      , ERASED_ALLOC
      , ERASED_EMPTY_ALLOC
    };

    typedef const void *(*Manager)(ManagerOpCode  opCode,
                                   const void    *source,
                                   Function_Rep  *rep);
        // 'Manager' is an alias for a pointer to a function that manages a
        // specific object type (i.e., it copies, moves, or destroyes it).  It
        // implements a kind of hand-coded virtual-function dispatch.  The
        // specified 'opCode' is used to choose the "virtual function" to
        // invoke, where the specified 'rep' and 'target' are arguments to
        // that function.  Internally, a 'Manager' function uses a 'switch'
        // statement rather than performing a virtual-table lookup.  This
        // mechanism is chosen because it saves a significant amount of space
        // over the C++ virtual-function mechanism, especially when the number
        // of different instantiations of 'bsl::function' is large.

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
        // be done by the manager with the opcode 'INPLACE_DETECTION'.
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

        void                *d_object_p;     // pointer to external rep
        void               (*d_func_p)();    // pointer to function
        void (Function_Rep::*d_memFunc_p)(); // pointer to member function
        MaxAlignedType       d_align;        // force align
        void                *d_minbuf[4];    // force minimum size
    };

    template <class T>
    struct FitsInplace : integral_constant<bool,
                                           sizeof(T) <= sizeof(InplaceBuffer)>
    {
        // Metafunction to determine whether the specified 'T' template
        // parameter fits within the 'InplaceBuffer'.

        // TBD: 'FitsInplace' should also take alignment into account, but
        // since we don't (yet) have the ability to specify alignment when
        // allocating memory, there's nothing we can do at this point.
    };

    template <class FUNC>
    friend class bsl::function;

    template <class FUNC>
    void initRep(FUNC& func, bslma::Allocator* alloc,
                 integral_constant<AllocType, BSLMA_ALLOC_PTR>);
    template <class FUNC, class T>
    void initRep(FUNC& func, const bsl::allocator<T>& alloc,
                 integral_constant<AllocType, BSL_ALLOCATOR>);
    template <class FUNC, class ALLOC>
    void initRep(FUNC& func, const ALLOC& alloc,
                 integral_constant<AllocType, ERASED_ALLOC>);
    template <class FUNC, class ALLOC>
    void initRep(FUNC& func, const ALLOC& alloc,
                 integral_constant<AllocType, ERASED_EMPTY_ALLOC>);

    template <class FUNC>
    static Manager getFuncManager(const FUNC&, true_type /* inplace */);
    template <class FUNC>
    static Manager getFuncManager(const FUNC&, false_type /* inplace */);

    template <class FUNC>
    static const void *inplaceFuncManager(ManagerOpCode  opCode,
                                          const void    *source,
                                          Function_Rep  *rep);

    template <class FUNC>
    static const void *outofplaceFuncManager(ManagerOpCode  opCode,
                                             const void    *source,
                                             Function_Rep  *rep);

    template <class FUNC>
    static const void *pairedAllocManager(ManagerOpCode  opCode,
                                          const void    *source,
                                          Function_Rep  *rep);

    template <class FUNC>
    static const void *separateAllocManager(ManagerOpCode  opCode,
                                            const void    *source,
                                            Function_Rep  *rep);

#if defined(BSLS_PLATFORM_CMP_MSVC)
    __declspec(noreturn)
#endif

  private:
    // DATA
    mutable InplaceBuffer d_objbuf;  // in-place representation (if fits, as
                                     // indicated by the manager), or
                                     // pointer to external representation

    Manager               d_funcManager_p;
                                     // pointer to manager function used to
                                     // operate on function object instance
                                     // (which knows about the erased type
                                     // 'FUNC' of the function object), or null
                                     // for raw function pointers.

    bslma::Allocator     *d_allocator_p; // allocator (held, not owned)

    Manager               d_allocManager_p;
                                     // pointer to manager function used to
                                     // operate on allocator instance (which
                                     // knows about the erased type 'ALLOC' of
                                     // the allocator object), or null if
                                     // 'ALLOC' is 'bslma::Allocator*'

public:
    // ACCESSORS
    bslma::Allocator *allocator() const;
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

                    // =====================================
                    // class template Function_MemFuncInvoke
                    // =====================================

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE,
          class RET, class... ARGS>
struct Function_MemFuncInvokeImp {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS = sizeof...(ARGS) };

    static
    RET invoke_imp(true_type /* DirectInvoke */, FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS>::Type... args) {
        // If 'OBJ_ARG_TYPE' is a non-const rvalue, then it will have been
        // forwarded as a const reference, instead.  In order to call a
        // potentially non-const member function on it, we must cast the
        // reference back to a the original type.  The 'const_cast', below,
        // will have no effect unless 'OBJ_ARG_TYPE' is a non-const rvalue.
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args...);
    }

    static
    RET invoke_imp(false_type /* DirectInvoke */, FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS>::Type... args) {
        // If 'OBJ_ARG_TYPE' is a non-const rvalue, then it will have been
        // forwarded as a const reference, instead.  In order to call a
        // potentially non-const member function on it, we must cast the
        // reference back to a the original type.  The 'const_cast', below,
        // will have no effect unless 'OBJ_ARG_TYPE' is a non-const rvalue.
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args...);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS>::Type... args)
        { return invoke_imp(DirectInvoke(), f, obj, args...); }

};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS...>
{
};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS...>
{
};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS...>
{
};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS...>
{
};

                    // =======================
                    // class template function
                    // =======================

template <class RET, class... ARGS>
class function<RET(ARGS...)> :
        public Function_ArgTypes<RET(ARGS...)>,
        public Function_Rep
{
    // An instantiation of this class template generalizes the notion of a
    // pointer to a function taking the specified 'ARGS' types and returning
    // the specified 'RET' type, i.e., a function pointer of type
    // 'RET(*)(ARGS)'.  An object of this class wraps a run-time invokable
    // object specified at construction, such as a function pointer, member
    // function pointer, or functor.  Note that 'function' is defined only for
    // template parameters that specify a function prototype; the primary
    // template (taking an arbitrary template parameter) is not defined.

    // Invoker Variants:
    //   empty
    //   function pointer
    //   member-function pointer
    //   functor

    // Manager quantities:
    //   Size of function rep (Used during cloning)
    //   Alignment of function rep (Used during cloning)
    //   Size of allocator (0 for unowned) (NOT Used during cloning)
    //   Alignment of allocator (1 for unowned) (NOT Used during cloning)
    // Manager variants
    //   Dimension 0: Invoker variant (Template parameter)
    //     empty
    //     function pointer
    //     member-function pointer
    //     functor
    //   Dimension 1: Allocator ownership
    //     Does not own allocator (treat allocator size as 0)
    //     Owns allocator (type-erased allocator)
    //   Dimension 2: Inplace allocation
    //     Function and allocator fit in-place
    //     Function and allocator do not fit in-place
    //

    // Cloning of functor and allocator:
    //   During initial construction:
    //     no cloning
    //   During move construction
    //     no cloning
    //   During extended move construction
    //     Clone rhs functor
    //     No allocator clone
    //   During copy construction
    //     Clone rhs functor
    //     No allocator clone
    //   During extended copy construction
    //     Clone rhs functor
    //     No allocator clone
    //   During move assignment
    //     Clone rhs functor
    //     Clone lhs allocator (into new block)
    //   During copy assignment
    //     Clone rhs functor
    //     Clone lhs allocator (into new block)
    //

    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS>::Type... args);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);
        // Return the invoker for a pointer to (non-member) function.

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);
        // Return the invoker for a pointer to member function.

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);
        // Return the invoker for an in-place functor.

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);
        // Return the invoker for an out-of-place functor.

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS>::Type...);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS>::Type...);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS>::Type...);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS>::Type...);

public:
    // PUBLIC TYPES
    typedef RET result_type;

    // CREATORS
    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    // MANIPULATORS
    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(FUNC&&);

    // TBD: Need to implement reference_wrapper.
    // template<class FUNC>
    // function& operator=(reference_wrapper<FUNC>) BSLS_NOTHROW_SPEC;

    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(FUNC&&, const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit  // Explicit conversion available only with C++11
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS...) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
};

// FREE FUNCTIONS
template <class RET, class... ARGS>
bool operator==(const function<RET(ARGS...)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator==(nullptr_t, const function<RET(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator!=(const function<RET(ARGS...)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator!=(nullptr_t, const function<RET(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
void swap(function<RET(ARGS...)>&, function<RET(ARGS...)>&);

#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_function.h


template <class FUNC,
          class OBJ_TYPE,
          class OBJ_ARG_TYPE,
          class RET,
          class ARGS_0 = BSLS_COMPILERFEATURES_NILT,
          class ARGS_1 = BSLS_COMPILERFEATURES_NILT,
          class ARGS_2 = BSLS_COMPILERFEATURES_NILT,
          class ARGS_3 = BSLS_COMPILERFEATURES_NILT,
          class ARGS_4 = BSLS_COMPILERFEATURES_NILT,
          class ARGS_5 = BSLS_COMPILERFEATURES_NILT,
          class ARGS_6 = BSLS_COMPILERFEATURES_NILT,
          class ARGS_7 = BSLS_COMPILERFEATURES_NILT,
          class ARGS_8 = BSLS_COMPILERFEATURES_NILT,
          class ARGS_9 = BSLS_COMPILERFEATURES_NILT,
          class = BSLS_COMPILERFEATURES_NILT>
struct Function_MemFuncInvokeImp;

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS =  0u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)();
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)();
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj)
        { return invoke_imp(DirectInvoke(), f, obj); }

};

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET,
          class ARGS_01>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS =  1u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args_01);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args_01);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS_01>::Type args_01)
        { return invoke_imp(DirectInvoke(), f, obj, args_01); }

};

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET,
          class ARGS_01,
          class ARGS_02>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                    ARGS_02> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS =  2u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args_01,
                                           args_02);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args_01,
                                              args_02);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS_01>::Type args_01,
               typename bslmf::ForwardingType<ARGS_02>::Type args_02)
        { return invoke_imp(DirectInvoke(), f, obj, args_01,
                                                    args_02); }

};

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET,
          class ARGS_01,
          class ARGS_02,
          class ARGS_03>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                    ARGS_02,
                                                                    ARGS_03> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS =  3u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args_01,
                                           args_02,
                                           args_03);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args_01,
                                              args_02,
                                              args_03);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS_01>::Type args_01,
               typename bslmf::ForwardingType<ARGS_02>::Type args_02,
               typename bslmf::ForwardingType<ARGS_03>::Type args_03)
        { return invoke_imp(DirectInvoke(), f, obj, args_01,
                                                    args_02,
                                                    args_03); }

};

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET,
          class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                    ARGS_02,
                                                                    ARGS_03,
                                                                    ARGS_04> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS =  4u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args_01,
                                           args_02,
                                           args_03,
                                           args_04);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args_01,
                                              args_02,
                                              args_03,
                                              args_04);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS_01>::Type args_01,
               typename bslmf::ForwardingType<ARGS_02>::Type args_02,
               typename bslmf::ForwardingType<ARGS_03>::Type args_03,
               typename bslmf::ForwardingType<ARGS_04>::Type args_04)
        { return invoke_imp(DirectInvoke(), f, obj, args_01,
                                                    args_02,
                                                    args_03,
                                                    args_04); }

};

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET,
          class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                    ARGS_02,
                                                                    ARGS_03,
                                                                    ARGS_04,
                                                                    ARGS_05> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS =  5u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args_01,
                                           args_02,
                                           args_03,
                                           args_04,
                                           args_05);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args_01,
                                              args_02,
                                              args_03,
                                              args_04,
                                              args_05);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS_01>::Type args_01,
               typename bslmf::ForwardingType<ARGS_02>::Type args_02,
               typename bslmf::ForwardingType<ARGS_03>::Type args_03,
               typename bslmf::ForwardingType<ARGS_04>::Type args_04,
               typename bslmf::ForwardingType<ARGS_05>::Type args_05)
        { return invoke_imp(DirectInvoke(), f, obj, args_01,
                                                    args_02,
                                                    args_03,
                                                    args_04,
                                                    args_05); }

};

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET,
          class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                    ARGS_02,
                                                                    ARGS_03,
                                                                    ARGS_04,
                                                                    ARGS_05,
                                                                    ARGS_06> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS =  6u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                   typename bslmf::ForwardingType<ARGS_06>::Type args_06) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args_01,
                                           args_02,
                                           args_03,
                                           args_04,
                                           args_05,
                                           args_06);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                   typename bslmf::ForwardingType<ARGS_06>::Type args_06) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args_01,
                                              args_02,
                                              args_03,
                                              args_04,
                                              args_05,
                                              args_06);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS_01>::Type args_01,
               typename bslmf::ForwardingType<ARGS_02>::Type args_02,
               typename bslmf::ForwardingType<ARGS_03>::Type args_03,
               typename bslmf::ForwardingType<ARGS_04>::Type args_04,
               typename bslmf::ForwardingType<ARGS_05>::Type args_05,
               typename bslmf::ForwardingType<ARGS_06>::Type args_06)
        { return invoke_imp(DirectInvoke(), f, obj, args_01,
                                                    args_02,
                                                    args_03,
                                                    args_04,
                                                    args_05,
                                                    args_06); }

};

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET,
          class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                    ARGS_02,
                                                                    ARGS_03,
                                                                    ARGS_04,
                                                                    ARGS_05,
                                                                    ARGS_06,
                                                                    ARGS_07> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS =  7u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                   typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                   typename bslmf::ForwardingType<ARGS_07>::Type args_07) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args_01,
                                           args_02,
                                           args_03,
                                           args_04,
                                           args_05,
                                           args_06,
                                           args_07);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                   typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                   typename bslmf::ForwardingType<ARGS_07>::Type args_07) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args_01,
                                              args_02,
                                              args_03,
                                              args_04,
                                              args_05,
                                              args_06,
                                              args_07);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS_01>::Type args_01,
               typename bslmf::ForwardingType<ARGS_02>::Type args_02,
               typename bslmf::ForwardingType<ARGS_03>::Type args_03,
               typename bslmf::ForwardingType<ARGS_04>::Type args_04,
               typename bslmf::ForwardingType<ARGS_05>::Type args_05,
               typename bslmf::ForwardingType<ARGS_06>::Type args_06,
               typename bslmf::ForwardingType<ARGS_07>::Type args_07)
        { return invoke_imp(DirectInvoke(), f, obj, args_01,
                                                    args_02,
                                                    args_03,
                                                    args_04,
                                                    args_05,
                                                    args_06,
                                                    args_07); }

};

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET,
          class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                    ARGS_02,
                                                                    ARGS_03,
                                                                    ARGS_04,
                                                                    ARGS_05,
                                                                    ARGS_06,
                                                                    ARGS_07,
                                                                    ARGS_08> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS =  8u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                   typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                   typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                   typename bslmf::ForwardingType<ARGS_08>::Type args_08) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args_01,
                                           args_02,
                                           args_03,
                                           args_04,
                                           args_05,
                                           args_06,
                                           args_07,
                                           args_08);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                   typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                   typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                   typename bslmf::ForwardingType<ARGS_08>::Type args_08) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args_01,
                                              args_02,
                                              args_03,
                                              args_04,
                                              args_05,
                                              args_06,
                                              args_07,
                                              args_08);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS_01>::Type args_01,
               typename bslmf::ForwardingType<ARGS_02>::Type args_02,
               typename bslmf::ForwardingType<ARGS_03>::Type args_03,
               typename bslmf::ForwardingType<ARGS_04>::Type args_04,
               typename bslmf::ForwardingType<ARGS_05>::Type args_05,
               typename bslmf::ForwardingType<ARGS_06>::Type args_06,
               typename bslmf::ForwardingType<ARGS_07>::Type args_07,
               typename bslmf::ForwardingType<ARGS_08>::Type args_08)
        { return invoke_imp(DirectInvoke(), f, obj, args_01,
                                                    args_02,
                                                    args_03,
                                                    args_04,
                                                    args_05,
                                                    args_06,
                                                    args_07,
                                                    args_08); }

};

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET,
          class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08,
          class ARGS_09>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                    ARGS_02,
                                                                    ARGS_03,
                                                                    ARGS_04,
                                                                    ARGS_05,
                                                                    ARGS_06,
                                                                    ARGS_07,
                                                                    ARGS_08,
                                                                    ARGS_09> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS =  9u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                   typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                   typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                   typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                   typename bslmf::ForwardingType<ARGS_09>::Type args_09) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args_01,
                                           args_02,
                                           args_03,
                                           args_04,
                                           args_05,
                                           args_06,
                                           args_07,
                                           args_08,
                                           args_09);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                   typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                   typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                   typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                   typename bslmf::ForwardingType<ARGS_09>::Type args_09) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args_01,
                                              args_02,
                                              args_03,
                                              args_04,
                                              args_05,
                                              args_06,
                                              args_07,
                                              args_08,
                                              args_09);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS_01>::Type args_01,
               typename bslmf::ForwardingType<ARGS_02>::Type args_02,
               typename bslmf::ForwardingType<ARGS_03>::Type args_03,
               typename bslmf::ForwardingType<ARGS_04>::Type args_04,
               typename bslmf::ForwardingType<ARGS_05>::Type args_05,
               typename bslmf::ForwardingType<ARGS_06>::Type args_06,
               typename bslmf::ForwardingType<ARGS_07>::Type args_07,
               typename bslmf::ForwardingType<ARGS_08>::Type args_08,
               typename bslmf::ForwardingType<ARGS_09>::Type args_09)
        { return invoke_imp(DirectInvoke(), f, obj, args_01,
                                                    args_02,
                                                    args_03,
                                                    args_04,
                                                    args_05,
                                                    args_06,
                                                    args_07,
                                                    args_08,
                                                    args_09); }

};

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE, class RET,
          class ARGS_01,
          class ARGS_02,
          class ARGS_03,
          class ARGS_04,
          class ARGS_05,
          class ARGS_06,
          class ARGS_07,
          class ARGS_08,
          class ARGS_09,
          class ARGS_10>
struct Function_MemFuncInvokeImp<FUNC, OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                    ARGS_02,
                                                                    ARGS_03,
                                                                    ARGS_04,
                                                                    ARGS_05,
                                                                    ARGS_06,
                                                                    ARGS_07,
                                                                    ARGS_08,
                                                                    ARGS_09,
                                                                    ARGS_10> {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS = 10u };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                   typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                   typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                   typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                   typename bslmf::ForwardingType<ARGS_09>::Type args_09,
                   typename bslmf::ForwardingType<ARGS_10>::Type args_10) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args_01,
                                           args_02,
                                           args_03,
                                           args_04,
                                           args_05,
                                           args_06,
                                           args_07,
                                           args_08,
                                           args_09,
                                           args_10);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                   typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                   typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                   typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                   typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                   typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                   typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                   typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                   typename bslmf::ForwardingType<ARGS_09>::Type args_09,
                   typename bslmf::ForwardingType<ARGS_10>::Type args_10) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args_01,
                                              args_02,
                                              args_03,
                                              args_04,
                                              args_05,
                                              args_06,
                                              args_07,
                                              args_08,
                                              args_09,
                                              args_10);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS_01>::Type args_01,
               typename bslmf::ForwardingType<ARGS_02>::Type args_02,
               typename bslmf::ForwardingType<ARGS_03>::Type args_03,
               typename bslmf::ForwardingType<ARGS_04>::Type args_04,
               typename bslmf::ForwardingType<ARGS_05>::Type args_05,
               typename bslmf::ForwardingType<ARGS_06>::Type args_06,
               typename bslmf::ForwardingType<ARGS_07>::Type args_07,
               typename bslmf::ForwardingType<ARGS_08>::Type args_08,
               typename bslmf::ForwardingType<ARGS_09>::Type args_09,
               typename bslmf::ForwardingType<ARGS_10>::Type args_10)
        { return invoke_imp(DirectInvoke(), f, obj, args_01,
                                                    args_02,
                                                    args_03,
                                                    args_04,
                                                    args_05,
                                                    args_06,
                                                    args_07,
                                                    args_08,
                                                    args_09,
                                                    args_10); }

};


template <class RET, class OBJ_TYPE, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05,
                                                   ARGS_06>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05,
                                                   ARGS_06,
                                                   ARGS_07>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05,
                                                   ARGS_06,
                                                   ARGS_07,
                                                   ARGS_08>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08,
                                     class ARGS_09, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08,
                                                ARGS_09), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05,
                                                   ARGS_06,
                                                   ARGS_07,
                                                   ARGS_08,
                                                   ARGS_09>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08,
                                     class ARGS_09,
                                     class ARGS_10, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08,
                                                ARGS_09,
                                                ARGS_10), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09,
                                                  ARGS_10), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05,
                                                   ARGS_06,
                                                   ARGS_07,
                                                   ARGS_08,
                                                   ARGS_09,
                                                   ARGS_10>
{
};


template <class RET, class OBJ_TYPE, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)() const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)() const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                   ARGS_02>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                   ARGS_02,
                                                                   ARGS_03>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                   ARGS_02,
                                                                   ARGS_03,
                                                                   ARGS_04>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                   ARGS_02,
                                                                   ARGS_03,
                                                                   ARGS_04,
                                                                   ARGS_05>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                   ARGS_02,
                                                                   ARGS_03,
                                                                   ARGS_04,
                                                                   ARGS_05,
                                                                   ARGS_06>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                   ARGS_02,
                                                                   ARGS_03,
                                                                   ARGS_04,
                                                                   ARGS_05,
                                                                   ARGS_06,
                                                                   ARGS_07>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                   ARGS_02,
                                                                   ARGS_03,
                                                                   ARGS_04,
                                                                   ARGS_05,
                                                                   ARGS_06,
                                                                   ARGS_07,
                                                                   ARGS_08>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08,
                                     class ARGS_09, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08,
                                                ARGS_09) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                   ARGS_02,
                                                                   ARGS_03,
                                                                   ARGS_04,
                                                                   ARGS_05,
                                                                   ARGS_06,
                                                                   ARGS_07,
                                                                   ARGS_08,
                                                                   ARGS_09>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08,
                                     class ARGS_09,
                                     class ARGS_10, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08,
                                                ARGS_09,
                                                ARGS_10) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09,
                                                  ARGS_10) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                   ARGS_02,
                                                                   ARGS_03,
                                                                   ARGS_04,
                                                                   ARGS_05,
                                                                   ARGS_06,
                                                                   ARGS_07,
                                                                   ARGS_08,
                                                                   ARGS_09,
                                                                   ARGS_10>
{
};


template <class RET, class OBJ_TYPE, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)() volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)() volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01
                                                ) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02
                                                ) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                      ARGS_02>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03
                                                ) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                      ARGS_02,
                                                                      ARGS_03>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04
                                                ) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                      ARGS_02,
                                                                      ARGS_03,
                                                                      ARGS_04>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05
                                                ) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                      ARGS_02,
                                                                      ARGS_03,
                                                                      ARGS_04,
                                                                      ARGS_05>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06
                                                ) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                      ARGS_02,
                                                                      ARGS_03,
                                                                      ARGS_04,
                                                                      ARGS_05,
                                                                      ARGS_06>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07
                                                ) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                      ARGS_02,
                                                                      ARGS_03,
                                                                      ARGS_04,
                                                                      ARGS_05,
                                                                      ARGS_06,
                                                                      ARGS_07>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08
                                                ) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                      ARGS_02,
                                                                      ARGS_03,
                                                                      ARGS_04,
                                                                      ARGS_05,
                                                                      ARGS_06,
                                                                      ARGS_07,
                                                                      ARGS_08>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08,
                                     class ARGS_09, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08,
                                                ARGS_09
                                                ) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                      ARGS_02,
                                                                      ARGS_03,
                                                                      ARGS_04,
                                                                      ARGS_05,
                                                                      ARGS_06,
                                                                      ARGS_07,
                                                                      ARGS_08,
                                                                      ARGS_09>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08,
                                     class ARGS_09,
                                     class ARGS_10, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08,
                                                ARGS_09,
                                                ARGS_10
                                                ) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09,
                                                  ARGS_10) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS_01,
                                                                      ARGS_02,
                                                                      ARGS_03,
                                                                      ARGS_04,
                                                                      ARGS_05,
                                                                      ARGS_06,
                                                                      ARGS_07,
                                                                      ARGS_08,
                                                                      ARGS_09,
                                                                      ARGS_10>
{
};


template <class RET, class OBJ_TYPE, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)() const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)() const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05,
                                                   ARGS_06>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05,
                                                   ARGS_06,
                                                   ARGS_07>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05,
                                                   ARGS_06,
                                                   ARGS_07,
                                                   ARGS_08>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08,
                                     class ARGS_09, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08,
                                                ARGS_09) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05,
                                                   ARGS_06,
                                                   ARGS_07,
                                                   ARGS_08,
                                                   ARGS_09>
{
};

template <class RET, class OBJ_TYPE, class ARGS_01,
                                     class ARGS_02,
                                     class ARGS_03,
                                     class ARGS_04,
                                     class ARGS_05,
                                     class ARGS_06,
                                     class ARGS_07,
                                     class ARGS_08,
                                     class ARGS_09,
                                     class ARGS_10, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08,
                                                ARGS_09,
                                                ARGS_10) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS_01,
                                                  ARGS_02,
                                                  ARGS_03,
                                                  ARGS_04,
                                                  ARGS_05,
                                                  ARGS_06,
                                                  ARGS_07,
                                                  ARGS_08,
                                                  ARGS_09,
                                                  ARGS_10) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS_01,
                                                   ARGS_02,
                                                   ARGS_03,
                                                   ARGS_04,
                                                   ARGS_05,
                                                   ARGS_06,
                                                   ARGS_07,
                                                   ARGS_08,
                                                   ARGS_09,
                                                   ARGS_10>
{
};



template <class RET>
class function<RET()> :
        public Function_ArgTypes<RET()>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()() const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
};

template <class RET, class ARGS_01>
class function<RET(ARGS_01)> :
        public Function_ArgTypes<RET(ARGS_01)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS_01>::Type args_01);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS_01) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
};

template <class RET, class ARGS_01,
                     class ARGS_02>
class function<RET(ARGS_01,
                   ARGS_02)> :
        public Function_ArgTypes<RET(ARGS_01,
                                     ARGS_02)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                        typename bslmf::ForwardingType<ARGS_02>::Type args_02);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS_01,
                   ARGS_02) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
};

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
class function<RET(ARGS_01,
                   ARGS_02,
                   ARGS_03)> :
        public Function_ArgTypes<RET(ARGS_01,
                                     ARGS_02,
                                     ARGS_03)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                        typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                        typename bslmf::ForwardingType<ARGS_03>::Type args_03);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS_01,
                   ARGS_02,
                   ARGS_03) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
};

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
class function<RET(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04)> :
        public Function_ArgTypes<RET(ARGS_01,
                                     ARGS_02,
                                     ARGS_03,
                                     ARGS_04)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                        typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                        typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                        typename bslmf::ForwardingType<ARGS_04>::Type args_04);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
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
        public Function_ArgTypes<RET(ARGS_01,
                                     ARGS_02,
                                     ARGS_03,
                                     ARGS_04,
                                     ARGS_05)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                        typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                        typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                        typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                        typename bslmf::ForwardingType<ARGS_05>::Type args_05);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
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
        public Function_ArgTypes<RET(ARGS_01,
                                     ARGS_02,
                                     ARGS_03,
                                     ARGS_04,
                                     ARGS_05,
                                     ARGS_06)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                        typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                        typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                        typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                        typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                        typename bslmf::ForwardingType<ARGS_06>::Type args_06);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05,
                   ARGS_06) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
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
        public Function_ArgTypes<RET(ARGS_01,
                                     ARGS_02,
                                     ARGS_03,
                                     ARGS_04,
                                     ARGS_05,
                                     ARGS_06,
                                     ARGS_07)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                        typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                        typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                        typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                        typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                        typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                        typename bslmf::ForwardingType<ARGS_07>::Type args_07);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05,
                   ARGS_06,
                   ARGS_07) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
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
        public Function_ArgTypes<RET(ARGS_01,
                                     ARGS_02,
                                     ARGS_03,
                                     ARGS_04,
                                     ARGS_05,
                                     ARGS_06,
                                     ARGS_07,
                                     ARGS_08)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                        typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                        typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                        typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                        typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                        typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                        typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                        typename bslmf::ForwardingType<ARGS_08>::Type args_08);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05,
                   ARGS_06,
                   ARGS_07,
                   ARGS_08) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
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
        public Function_ArgTypes<RET(ARGS_01,
                                     ARGS_02,
                                     ARGS_03,
                                     ARGS_04,
                                     ARGS_05,
                                     ARGS_06,
                                     ARGS_07,
                                     ARGS_08,
                                     ARGS_09)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                        typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                        typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                        typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                        typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                        typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                        typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                        typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                        typename bslmf::ForwardingType<ARGS_09>::Type args_09);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type,
                                typename bslmf::ForwardingType<ARGS_09>::Type);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type,
                                typename bslmf::ForwardingType<ARGS_09>::Type);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type,
                                typename bslmf::ForwardingType<ARGS_09>::Type);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type,
                                typename bslmf::ForwardingType<ARGS_09>::Type);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05,
                   ARGS_06,
                   ARGS_07,
                   ARGS_08,
                   ARGS_09) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
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
        public Function_ArgTypes<RET(ARGS_01,
                                     ARGS_02,
                                     ARGS_03,
                                     ARGS_04,
                                     ARGS_05,
                                     ARGS_06,
                                     ARGS_07,
                                     ARGS_08,
                                     ARGS_09,
                                     ARGS_10)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                        typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                        typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                        typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                        typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                        typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                        typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                        typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                        typename bslmf::ForwardingType<ARGS_09>::Type args_09,
                        typename bslmf::ForwardingType<ARGS_10>::Type args_10);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type,
                                typename bslmf::ForwardingType<ARGS_09>::Type,
                                typename bslmf::ForwardingType<ARGS_10>::Type);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type,
                                typename bslmf::ForwardingType<ARGS_09>::Type,
                                typename bslmf::ForwardingType<ARGS_10>::Type);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type,
                                typename bslmf::ForwardingType<ARGS_09>::Type,
                                typename bslmf::ForwardingType<ARGS_10>::Type);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS_01>::Type,
                                typename bslmf::ForwardingType<ARGS_02>::Type,
                                typename bslmf::ForwardingType<ARGS_03>::Type,
                                typename bslmf::ForwardingType<ARGS_04>::Type,
                                typename bslmf::ForwardingType<ARGS_05>::Type,
                                typename bslmf::ForwardingType<ARGS_06>::Type,
                                typename bslmf::ForwardingType<ARGS_07>::Type,
                                typename bslmf::ForwardingType<ARGS_08>::Type,
                                typename bslmf::ForwardingType<ARGS_09>::Type,
                                typename bslmf::ForwardingType<ARGS_10>::Type);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC));


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS_01,
                   ARGS_02,
                   ARGS_03,
                   ARGS_04,
                   ARGS_05,
                   ARGS_06,
                   ARGS_07,
                   ARGS_08,
                   ARGS_09,
                   ARGS_10) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
};


template <class RET>
bool operator==(const function<RET()>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01>
bool operator==(const function<RET(ARGS_01)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02>
bool operator==(const function<RET(ARGS_01,
                                   ARGS_02)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bool operator==(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bool operator==(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bool operator==(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bool operator==(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05,
                                   ARGS_06)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bool operator==(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05,
                                   ARGS_06,
                                   ARGS_07)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bool operator==(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05,
                                   ARGS_06,
                                   ARGS_07,
                                   ARGS_08)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bool operator==(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05,
                                   ARGS_06,
                                   ARGS_07,
                                   ARGS_08,
                                   ARGS_09)>&, nullptr_t) BSLS_NOTHROW_SPEC;

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
bool operator==(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05,
                                   ARGS_06,
                                   ARGS_07,
                                   ARGS_08,
                                   ARGS_09,
                                   ARGS_10)>&, nullptr_t) BSLS_NOTHROW_SPEC;


template <class RET>
bool operator==(nullptr_t, const function<RET()>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01>
bool operator==(nullptr_t, const function<RET(ARGS_01)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02>
bool operator==(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bool operator==(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bool operator==(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bool operator==(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bool operator==(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bool operator==(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bool operator==(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bool operator==(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09)>&) BSLS_NOTHROW_SPEC;

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
bool operator==(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10)>&) BSLS_NOTHROW_SPEC;


template <class RET>
bool operator!=(const function<RET()>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01>
bool operator!=(const function<RET(ARGS_01)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02>
bool operator!=(const function<RET(ARGS_01,
                                   ARGS_02)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bool operator!=(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bool operator!=(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bool operator!=(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bool operator!=(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05,
                                   ARGS_06)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bool operator!=(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05,
                                   ARGS_06,
                                   ARGS_07)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bool operator!=(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05,
                                   ARGS_06,
                                   ARGS_07,
                                   ARGS_08)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bool operator!=(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05,
                                   ARGS_06,
                                   ARGS_07,
                                   ARGS_08,
                                   ARGS_09)>&, nullptr_t) BSLS_NOTHROW_SPEC;

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
bool operator!=(const function<RET(ARGS_01,
                                   ARGS_02,
                                   ARGS_03,
                                   ARGS_04,
                                   ARGS_05,
                                   ARGS_06,
                                   ARGS_07,
                                   ARGS_08,
                                   ARGS_09,
                                   ARGS_10)>&, nullptr_t) BSLS_NOTHROW_SPEC;


template <class RET>
bool operator!=(nullptr_t, const function<RET()>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01>
bool operator!=(nullptr_t, const function<RET(ARGS_01)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02>
bool operator!=(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bool operator!=(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bool operator!=(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bool operator!=(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bool operator!=(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bool operator!=(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bool operator!=(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bool operator!=(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09)>&) BSLS_NOTHROW_SPEC;

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
bool operator!=(nullptr_t, const function<RET(ARGS_01,
                                              ARGS_02,
                                              ARGS_03,
                                              ARGS_04,
                                              ARGS_05,
                                              ARGS_06,
                                              ARGS_07,
                                              ARGS_08,
                                              ARGS_09,
                                              ARGS_10)>&) BSLS_NOTHROW_SPEC;


template <class RET>
void swap(function<RET()>&, function<RET()>&);

template <class RET, class ARGS_01>
void swap(function<RET(ARGS_01)>&, function<RET(ARGS_01)>&);

template <class RET, class ARGS_01,
                     class ARGS_02>
void swap(function<RET(ARGS_01,
                       ARGS_02)>&, function<RET(ARGS_01,
                                                ARGS_02)>&);

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
void swap(function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03)>&, function<RET(ARGS_01,
                                                ARGS_02,
                                                ARGS_03)>&);

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
void swap(function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04)>&, function<RET(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04)>&);

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
void swap(function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05)>&, function<RET(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05)>&);

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
void swap(function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06)>&, function<RET(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06)>&);

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
void swap(function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07)>&, function<RET(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07)>&);

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
void swap(function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07,
                       ARGS_08)>&, function<RET(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08)>&);

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
void swap(function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07,
                       ARGS_08,
                       ARGS_09)>&, function<RET(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08,
                                                ARGS_09)>&);

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
void swap(function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07,
                       ARGS_08,
                       ARGS_09,
                       ARGS_10)>&, function<RET(ARGS_01,
                                                ARGS_02,
                                                ARGS_03,
                                                ARGS_04,
                                                ARGS_05,
                                                ARGS_06,
                                                ARGS_07,
                                                ARGS_08,
                                                ARGS_09,
                                                ARGS_10)>&);

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.


template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE,
          class RET, class... ARGS>
struct Function_MemFuncInvokeImp {

    typedef typename is_convertible<
            typename remove_reference<OBJ_ARG_TYPE>::type*,
            OBJ_TYPE*
        >::type DirectInvoke;

    enum { NUM_ARGS = sizeof...(ARGS) };

    static
    RET invoke_imp(true_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS>::Type... args) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return (const_cast<ObjTp>(obj).*f)(args...);
    }

    static
    RET invoke_imp(false_type , FUNC f,
                   typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
                   typename bslmf::ForwardingType<ARGS>::Type... args) {
        typedef typename bsl::add_lvalue_reference<OBJ_ARG_TYPE>::type ObjTp;
        return ((*const_cast<ObjTp>(obj)).*f)(args...);
    }

public:
    static
    RET invoke(FUNC f,
               typename bslmf::ForwardingType<OBJ_ARG_TYPE>::Type obj,
               typename bslmf::ForwardingType<ARGS>::Type... args)
        { return invoke_imp(DirectInvoke(), f, obj, args...); }

};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...), OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...), OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS...>
{
};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS...>
{
};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...) volatile, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS...>
{
};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS...>
{
};


template <class RET, class... ARGS>
class function<RET(ARGS...)> :
        public Function_ArgTypes<RET(ARGS...)>,
        public Function_Rep
{




    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS>::Type... args);

    typedef Function_Rep::FuncType FuncType;

    Invoker *d_invoker_p;

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&,
                              bslmf::SelectTraitCase<FitsInplace>);

    template <class FUNC>
    static Invoker *getInvoker(const FUNC&, bslmf::SelectTraitCase<>);

    template <class FUNC>
    static RET functionPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS>::Type...);

    template <class FUNC>
    static RET memFuncPtrInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS>::Type...);

    template <class FUNC>
    static RET inplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS>::Type...);

    template <class FUNC>
    static RET outofplaceFunctorInvoker(const Function_Rep *rep, 
                                typename bslmf::ForwardingType<ARGS>::Type...);

public:
    typedef RET result_type;

    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function&);
    template<class FUNC> function(FUNC);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, nullptr_t);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&,
                                   const function&);
    template<class FUNC, class ALLOC> function(allocator_arg_t, const ALLOC&,
                                               FUNC);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&&);
    template<class ALLOC> function(allocator_arg_t, const ALLOC&, function&&);
#endif

    ~function();

    function& operator=(const function&);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function& operator=(function&&);
#endif
    function& operator=(nullptr_t);
    template<class FUNC>
    function& operator=(FUNC&&);


    void swap(function&) BSLS_NOTHROW_SPEC;
    template<class FUNC, class ALLOC> void assign(FUNC&&, const ALLOC&);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit
#endif
    operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS...) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
};

template <class RET, class... ARGS>
bool operator==(const function<RET(ARGS...)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator==(nullptr_t, const function<RET(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator!=(const function<RET(ARGS...)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator!=(nullptr_t, const function<RET(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
void swap(function<RET(ARGS...)>&, function<RET(ARGS...)>&);

// }}} END GENERATED CODE
#endif

}  // close namespace bsl

// ===========================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ===========================================================================


                        // -----------------------
                        // class bad_function_call
                        // -----------------------

inline
bsl::bad_function_call::bad_function_call() BSLS_NOTHROW_SPEC
    : std::exception()
{
}

                        // --------------------------------
                        // class Function_ObjPairBufferDesc
                        // --------------------------------

// MANIPULATORS
template <class TYPE1, class TYPE2>
void bsl::Function_ObjPairBufferDesc::setDescriptor()
{
    d_firstSize   = Function_ObjSize<TYPE1>::VALUE;
    d_firstAlign  = Function_ObjAlignment<TYPE1>::VALUE;
    d_secondSize  = Function_ObjSize<TYPE2>::VALUE;
    d_secondAlign = Function_ObjAlignment<TYPE2>::VALUE;

    d_secondOffset = d_firstSize;
    d_secondOffset += d_secondAlign;
    d_secondOffset &= ~(d_secondAlign - 1);

    d_totalSize = d_secondOffset + d_secondSize;
    d_totalSize += d_firstAlign;
    d_totalSize &= ~(d_firstAlign - 1);

    d_totalAlign = (d_firstAlign < d_secondAlign ?
                    d_secondAlign : d_firstAlign);
}

// ACCESSORS
inline void       *bsl::Function_ObjPairBufferDesc::first(void       *buffer)
{
    return buffer;
}

inline void const *bsl::Function_ObjPairBufferDesc::first(void const *buffer)
{
    return buffer;
}

inline void       *bsl::Function_ObjPairBufferDesc::second(void       *buffer)
{
    return d_secondOffset + static_cast<char*>(buffer);
}

inline void const *bsl::Function_ObjPairBufferDesc::second(void const *buffer)
{
    return d_secondOffset + static_cast<const char*>(buffer);
}

inline std::size_t bsl::Function_ObjPairBufferDesc::totalSize() const
{
    return d_totalSize;
}

inline std::size_t bsl::Function_ObjPairBufferDesc::totalAlign() const
{
    return d_totalAlign;
}

inline std::size_t bsl::Function_ObjPairBufferDesc::firstSize() const
{
    return d_firstSize;
}

// inline std::size_t bsl::Function_ObjPairBufferDesc::firstAlign() const
// {
//     return d_firstAlign;
// }

inline std::size_t bsl::Function_ObjPairBufferDesc::secondSize() const
{
    return d_secondSize;
}

// inline std::size_t bsl::Function_ObjPairBufferDesc::secondAlign() const
// {
//     return d_secondAlign;
// }

// inline std::size_t bsl::Function_ObjPairBufferDesc::secondOffset() const
// {
//     return d_secondOffset;
// }

                        // -----------------------
                        // class bsl::Function_Rep
                        // -----------------------

template <class FUNC>
inline
bsl::Function_Rep::Manager
bsl::Function_Rep::getFuncManager(const FUNC&, true_type /* inplace */)
{
    return &inplaceFuncManager<FUNC>;
}

template <class FUNC>
inline
bsl::Function_Rep::Manager
bsl::Function_Rep::getFuncManager(const FUNC&, false_type /* inplace */)
{
    return &outofplaceFuncManager<FUNC>;
}

template <class FUNC>
const void *bsl::Function_Rep::inplaceFuncManager(ManagerOpCode  opCode,
                                                  const void    *source,
                                                  Function_Rep  *rep)
{
    switch (opCode) {
      case MOVE_CONSTRUCT: {
          // There is no point to optimizing this operation for bitwise
          // moveable types.  If the type is trivially moveable, then the move
          // operation below will do it trivially.
          FUNC &srcFunc = *static_cast<FUNC*>(const_cast<void*>(source));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
          ::new (&rep->d_objbuf) FUNC(std::move(srcFunc));
#else
          ::new (&rep->d_objbuf) FUNC(srcFunc);
#endif
      } break;

      case COPY_CONSTRUCT: {
          // There is no point to optimizing this operation for bitwise
          // copyable types.  If the type is trivially moveable, then the copy
          // operation below will do it trivially.
          FUNC &srcFunc = *static_cast<FUNC*>(const_cast<void*>(source));
          ::new (&rep->d_objbuf) FUNC(srcFunc);
      } break;

      case DESTROY: {
          reinterpret_cast<FUNC&>(rep->d_objbuf).~FUNC();
      } break;

      case INPLACE_DETECTION: {
          return rep;  // Evaluates true in a boolean context
      } break;

      case GET_TARGET: {
          return &rep->d_objbuf;
      } break;

      case GET_TYPE_ID: {
          return &typeid(FUNC);
      } break;

    } // end switch

    return NULL;
}

template <class FUNC>
const void *bsl::Function_Rep::outofplaceFuncManager(ManagerOpCode  opCode,
                                                     const void    *source,
                                                     Function_Rep  *rep)
{
    switch (opCode) {

      case MOVE_CONSTRUCT: {
          // There is no point to optimizing this operation for bitwise
          // moveable types.  If the type is trivially moveable, then the move
          // operation below will do it trivially.
          FUNC &srcFunc = *static_cast<FUNC*>(const_cast<void*>(source));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
          ::new (rep->d_objbuf.d_object_p) FUNC(std::move(srcFunc));
#else
          ::new (rep->d_objbuf.d_object_p) FUNC(srcFunc);
#endif
      } break;

      case COPY_CONSTRUCT: {
          // There is no point to optimizing this operation for bitwise
          // copyable types.  If the type is trivially moveable, then the copy
          // operation below will do it trivially.
          FUNC &srcFunc = *static_cast<FUNC*>(const_cast<void*>(source));
          ::new (rep->d_objbuf.d_object_p) FUNC(srcFunc);
      } break;

      case DESTROY: {
          reinterpret_cast<FUNC&>(rep->d_objbuf.d_object_p).~FUNC();
          rep->d_allocator_p->deallocate(rep->d_objbuf.d_object_p);
      } break;

      case INPLACE_DETECTION: {
          return NULL;  // Evaluates false in a boolean context
      } break;

      case GET_TARGET: {
          return rep->d_objbuf.d_object_p;
      } break;

      case GET_TYPE_ID: {
          return &typeid(FUNC);
      } break;

    } // end switch

    return NULL;
}


template <class FUNC>
void bsl::Function_Rep::initRep(FUNC& func, bslma::Allocator* alloc,
                                integral_constant<AllocType, BSLMA_ALLOC_PTR>)
{
    typedef FitsInplace<FUNC> IsInplaceFunc;

    void *function_p;
    if (IsInplaceFunc::value) {
        function_p = &d_objbuf;
    }
    else {
        function_p = alloc->allocate(sizeof(FUNC));
        d_objbuf.d_object_p = function_p;
    }

    // Construct function object in designated location
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    ::new(function_p) FUNC(std::move(func));
#else
    ::new(function_p) FUNC(func);
#endif

    d_funcManager_p = getFuncManager(func, IsInplaceFunc());

    d_allocator_p = alloc;
    d_allocManager_p = NULL;
}

template <class FUNC, class T>
void bsl::Function_Rep::initRep(FUNC& func, const bsl::allocator<T>& alloc,
                                integral_constant<AllocType, BSL_ALLOCATOR>)
{
    initRep(func, alloc.mechanism(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());
}

template <class FUNC, class ALLOC>
void bsl::Function_Rep::initRep(FUNC& func, const ALLOC& alloc,
                                integral_constant<AllocType, ERASED_ALLOC>)
{
    typedef bslma::AllocatorAdaptor<ALLOC> Adaptor;

    // Object big enough to hold both function and adapted allocator
    typedef bsl::pair<FUNC, Adaptor> Pair;

    typedef FitsInplace<FUNC> IsInplaceFunc;
    typedef FitsInplace<Pair> IsInplacePair;

    void *function_p;
    void *allocator_p;

    // Whether or not the function object is in-place is not dependent on
    // whether the allocator also fits object buffer.  This design avoids the
    // function manager having to handle the cross product of all possible
    // allocators and all possible function types.  If the function object is
    // in-place and the allocator happens to fit as well, then it will be
    // squeezed in as well, otherwise the allocator will be allocated
    // out-of-place.
    //
    // Although this is a run-time 'if' statement, the compiler will usually
    // optimize away the conditional (and it's cheap even if not optimized
    // away).  This does mean that both the paired and separate allocator
    // managers will be instantiated, but that's OK because they are both
    // needed in the assignment operators, where size determinations really
    // are made at run time.
    if (IsInplaceFunc::value) {
        // Function object will be allocated in-place.
        if (IsInplacePair::value) {
            // Both Function object and allocator fit in-place.
            Pair *pair_p = reinterpret_cast<Pair*>(&d_objbuf);
            function_p = &pair_p->first;
            allocator_p = &pair_p->second;
            d_allocManager_p = &pairedAllocManager<ALLOC>;
        }
        else {
            // Function object fits in-place, but allocator is out-of-place
            function_p = &d_objbuf;
            // Allocate allocator adaptor from allocator itself
            allocator_p = Adaptor(alloc).allocate(sizeof(ALLOC));
            d_allocManager_p = &separateAllocManager<ALLOC>;
        }
    }
    else {
        // Not in-place

        // Allocate function and allocator adaptor from the allocator
        Pair *pair_p =
            static_cast<Pair*>(Adaptor(alloc).allocate(sizeof(Pair)));
        function_p = &pair_p->first;
        allocator_p = &pair_p->second;
        d_allocManager_p = &pairedAllocManager<ALLOC>;
        d_objbuf.d_func_p = function_p;
    }

    // Construct Function int its correct location
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    ::new(function_p) FUNC(std::move(func));
#else
    ::new(function_p) FUNC(func);
#endif

    d_funcManager_p = getFuncManager(func, IsInplaceFunc());

    // Construct allocator adaptor in its correct location
    d_allocator_p = ::new(allocator_p) Adaptor(alloc);
}

template <class FUNC, class ALLOC>
void bsl::Function_Rep::initRep(FUNC& func, const ALLOC& alloc,
                              integral_constant<AllocType, ERASED_EMPTY_ALLOC>)
{
    // Since ALLOC is an empty type, we need only one instance of it.
    // This single instance is wrapped in an adaptor
    static bslma::AllocatorAdaptor<ALLOC> allocInstance(alloc);

    initRep(func, &allocInstance,
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());
}

inline
bslma::Allocator *bsl::Function_Rep::allocator() const
{
    return d_allocator_p;
}

                        // ----------------------------
                        // class template bsl::function
                        // ----------------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

// PRIVATE STATIC MEMBER FUNCTIONS
template <class RET, class... ARGS>
template <class FUNC>
RET bsl::function<RET(ARGS...)>::functionPtrInvoker(const Function_Rep *rep,
                            typename bslmf::ForwardingType<ARGS>::Type... args)
{
    // Note that 'FUNC' might be different than 'RET(*)(ARGS...)'. All that is
    // required is that it be Callable with 'ARGS...' and return something
    // convertible to 'RET'.
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    // Cast to 'RET' is needed to avoid compilation error if 'RET' is void and
    // 'f' returns non-void.
    return static_cast<RET>(f(args...));
}

template <class RET, class... ARGS>
template <class FUNC>
RET bsl::function<RET(ARGS...)>::memFuncPtrInvoker(const Function_Rep *rep, 
                            typename bslmf::ForwardingType<ARGS>::Type... args)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS...>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT(sizeof...(ARGS) == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args...);
}

template <class RET, class... ARGS>
template <class FUNC>
RET bsl::function<RET(ARGS...)>::inplaceFunctorInvoker(const Function_Rep *rep, 
                            typename bslmf::ForwardingType<ARGS>::Type... args)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args...);
}

template <class RET, class... ARGS>
template <class FUNC>
RET
bsl::function<RET(ARGS...)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                            typename bslmf::ForwardingType<ARGS>::Type... args)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args...);
}

template <class RET, class... ARGS>
template <class FUNC>
inline
typename bsl::function<RET(ARGS...)>::Invoker *
bsl::function<RET(ARGS...)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class... ARGS>
template <class FUNC>
typename bsl::function<RET(ARGS...)>::Invoker *
bsl::function<RET(ARGS...)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class... ARGS>
template <class FUNC>
typename bsl::function<RET(ARGS...)>::Invoker *
bsl::function<RET(ARGS...)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class... ARGS>
template <class FUNC>
typename bsl::function<RET(ARGS...)>::Invoker *
bsl::function<RET(ARGS...)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

// CREATORS
template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function(const function&)
{
    // TBD
}

template <class RET, class... ARGS>
template<class FUNC>
bsl::function<RET(ARGS...)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS...) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS...) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class... ARGS>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function(function&&)
{
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

#endif //  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

// MANIPULATORS
template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(const function&)
{
    // TBD
    return *this;
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(function&&)
{
    // TBD
    return *this;
}

#endif

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(nullptr_t)
{
    // TBD
    return *this;
}

template <class RET, class... ARGS>
template<class FUNC>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(FUNC&&)
{
    // TBD
    return *this;
}

// TBD: Need to implement reference_wrapper.
// template <class RET, class... ARGS>
// template<class FUNC>
// function& bsl::function<RET(ARGS...)>::operator=(reference_wrapper<FUNC>)
//     BSLS_NOTHROW_SPEC

template <class RET, class... ARGS>
void bsl::function<RET(ARGS...)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class... ARGS>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS...)>::assign(FUNC&&, const ALLOC&)
{
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::operator bool() const BSLS_NOTHROW_SPEC
{
    // If there is an invoker, then this function is non-empty (return true);
    // otherwise it is empty (return false).
    return d_invoker_p;
}

template <class RET, class... ARGS>
RET bsl::function<RET(ARGS...)>::operator()(ARGS... args) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args...);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class... ARGS>
const std::type_info&
bsl::function<RET(ARGS...)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class... ARGS>
template<class T>
T* bsl::function<RET(ARGS...)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class... ARGS>
template<class T>
const T* bsl::function<RET(ARGS...)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

// FREE FUNCTIONS
template <class RET, class... ARGS>
bool operator==(const bsl::function<RET(ARGS...)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator!=(const bsl::function<RET(ARGS...)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
inline
void swap(bsl::function<RET(ARGS...)>& a, bsl::function<RET(ARGS...)>& b)
{
    a.swap(b);
}

#elif BSLS_COMPILERFEATURES_SIMULATE_VARIADIC_TEMPLATES
// {{{ BEGIN GENERATED CODE
// The following section is automatically generated.  **DO NOT EDIT**
// Generator command line: sim_cpp11_features.pl bslstl_function.h

template <class RET>
template <class FUNC>
RET bsl::function<RET()>::functionPtrInvoker(const Function_Rep *rep)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f());
}

template <class RET, class ARGS_01>
template <class FUNC>
RET bsl::function<RET(ARGS_01)>::functionPtrInvoker(const Function_Rep *rep,
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args_01));
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02)>::functionPtrInvoker(const Function_Rep *rep,
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args_01,
                              args_02));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03)>::functionPtrInvoker(const Function_Rep *rep,
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args_01,
                              args_02,
                              args_03));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04)>::functionPtrInvoker(const Function_Rep *rep,
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args_01,
                              args_02,
                              args_03,
                              args_04));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05)>::functionPtrInvoker(const Function_Rep *rep,
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args_01,
                              args_02,
                              args_03,
                              args_04,
                              args_05));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06)>::functionPtrInvoker(const Function_Rep *rep,
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args_01,
                              args_02,
                              args_03,
                              args_04,
                              args_05,
                              args_06));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07)>::functionPtrInvoker(const Function_Rep *rep,
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args_01,
                              args_02,
                              args_03,
                              args_04,
                              args_05,
                              args_06,
                              args_07));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08)>::functionPtrInvoker(const Function_Rep *rep,
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args_01,
                              args_02,
                              args_03,
                              args_04,
                              args_05,
                              args_06,
                              args_07,
                              args_08));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09)>::functionPtrInvoker(const Function_Rep *rep,
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                         typename bslmf::ForwardingType<ARGS_09>::Type args_09)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args_01,
                              args_02,
                              args_03,
                              args_04,
                              args_05,
                              args_06,
                              args_07,
                              args_08,
                              args_09));
}

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
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09,
                      ARGS_10)>::functionPtrInvoker(const Function_Rep *rep,
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                         typename bslmf::ForwardingType<ARGS_09>::Type args_09,
                         typename bslmf::ForwardingType<ARGS_10>::Type args_10)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args_01,
                              args_02,
                              args_03,
                              args_04,
                              args_05,
                              args_06,
                              args_07,
                              args_08,
                              args_09,
                              args_10));
}


template <class RET>
template <class FUNC>
RET bsl::function<RET()>::memFuncPtrInvoker(const Function_Rep *rep)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT( 0u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f);
}

template <class RET, class ARGS_01>
template <class FUNC>
RET bsl::function<RET(ARGS_01)>::memFuncPtrInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS_01>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT( 1u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args_01);
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02)>::memFuncPtrInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS_01,
                                            ARGS_02>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT( 2u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args_01,
                                       args_02);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03)>::memFuncPtrInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS_01,
                                            ARGS_02,
                                            ARGS_03>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT( 3u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args_01,
                                       args_02,
                                       args_03);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04)>::memFuncPtrInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS_01,
                                            ARGS_02,
                                            ARGS_03,
                                            ARGS_04>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT( 4u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args_01,
                                       args_02,
                                       args_03,
                                       args_04);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05)>::memFuncPtrInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS_01,
                                            ARGS_02,
                                            ARGS_03,
                                            ARGS_04,
                                            ARGS_05>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT( 5u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args_01,
                                       args_02,
                                       args_03,
                                       args_04,
                                       args_05);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06)>::memFuncPtrInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS_01,
                                            ARGS_02,
                                            ARGS_03,
                                            ARGS_04,
                                            ARGS_05,
                                            ARGS_06>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT( 6u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args_01,
                                       args_02,
                                       args_03,
                                       args_04,
                                       args_05,
                                       args_06);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07)>::memFuncPtrInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS_01,
                                            ARGS_02,
                                            ARGS_03,
                                            ARGS_04,
                                            ARGS_05,
                                            ARGS_06,
                                            ARGS_07>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT( 7u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args_01,
                                       args_02,
                                       args_03,
                                       args_04,
                                       args_05,
                                       args_06,
                                       args_07);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08)>::memFuncPtrInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS_01,
                                            ARGS_02,
                                            ARGS_03,
                                            ARGS_04,
                                            ARGS_05,
                                            ARGS_06,
                                            ARGS_07,
                                            ARGS_08>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT( 8u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args_01,
                                       args_02,
                                       args_03,
                                       args_04,
                                       args_05,
                                       args_06,
                                       args_07,
                                       args_08);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09)>::memFuncPtrInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                         typename bslmf::ForwardingType<ARGS_09>::Type args_09)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS_01,
                                            ARGS_02,
                                            ARGS_03,
                                            ARGS_04,
                                            ARGS_05,
                                            ARGS_06,
                                            ARGS_07,
                                            ARGS_08,
                                            ARGS_09>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT( 9u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args_01,
                                       args_02,
                                       args_03,
                                       args_04,
                                       args_05,
                                       args_06,
                                       args_07,
                                       args_08,
                                       args_09);
}

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
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09,
                      ARGS_10)>::memFuncPtrInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                         typename bslmf::ForwardingType<ARGS_09>::Type args_09,
                         typename bslmf::ForwardingType<ARGS_10>::Type args_10)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS_01,
                                            ARGS_02,
                                            ARGS_03,
                                            ARGS_04,
                                            ARGS_05,
                                            ARGS_06,
                                            ARGS_07,
                                            ARGS_08,
                                            ARGS_09,
                                            ARGS_10>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT(10u == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args_01,
                                       args_02,
                                       args_03,
                                       args_04,
                                       args_05,
                                       args_06,
                                       args_07,
                                       args_08,
                                       args_09,
                                       args_10);
}


template <class RET>
template <class FUNC>
RET bsl::function<RET()>::inplaceFunctorInvoker(const Function_Rep *rep)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f();
}

template <class RET, class ARGS_01>
template <class FUNC>
RET bsl::function<RET(ARGS_01
                      )>::inplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args_01);
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02
                      )>::inplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args_01,
             args_02);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03
                      )>::inplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args_01,
             args_02,
             args_03);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04
                      )>::inplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args_01,
             args_02,
             args_03,
             args_04);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05
                      )>::inplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06
                      )>::inplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05,
             args_06);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07
                      )>::inplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05,
             args_06,
             args_07);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08
                      )>::inplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05,
             args_06,
             args_07,
             args_08);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09
                      )>::inplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                         typename bslmf::ForwardingType<ARGS_09>::Type args_09)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05,
             args_06,
             args_07,
             args_08,
             args_09);
}

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
template <class FUNC>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09,
                      ARGS_10
                      )>::inplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                         typename bslmf::ForwardingType<ARGS_09>::Type args_09,
                         typename bslmf::ForwardingType<ARGS_10>::Type args_10)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05,
             args_06,
             args_07,
             args_08,
             args_09,
             args_10);
}


template <class RET>
template <class FUNC>
RET
bsl::function<RET()>::outofplaceFunctorInvoker(const Function_Rep *rep)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f();
}

template <class RET, class ARGS_01>
template <class FUNC>
RET
bsl::function<RET(ARGS_01)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args_01);
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template <class FUNC>
RET
bsl::function<RET(ARGS_01,
                  ARGS_02)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args_01,
             args_02);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template <class FUNC>
RET
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args_01,
             args_02,
             args_03);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template <class FUNC>
RET
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args_01,
             args_02,
             args_03,
             args_04);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template <class FUNC>
RET
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template <class FUNC>
RET
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05,
             args_06);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template <class FUNC>
RET
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05,
             args_06,
             args_07);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template <class FUNC>
RET
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05,
             args_06,
             args_07,
             args_08);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template <class FUNC>
RET
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                         typename bslmf::ForwardingType<ARGS_09>::Type args_09)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05,
             args_06,
             args_07,
             args_08,
             args_09);
}

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
template <class FUNC>
RET
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                         typename bslmf::ForwardingType<ARGS_01>::Type args_01,
                         typename bslmf::ForwardingType<ARGS_02>::Type args_02,
                         typename bslmf::ForwardingType<ARGS_03>::Type args_03,
                         typename bslmf::ForwardingType<ARGS_04>::Type args_04,
                         typename bslmf::ForwardingType<ARGS_05>::Type args_05,
                         typename bslmf::ForwardingType<ARGS_06>::Type args_06,
                         typename bslmf::ForwardingType<ARGS_07>::Type args_07,
                         typename bslmf::ForwardingType<ARGS_08>::Type args_08,
                         typename bslmf::ForwardingType<ARGS_09>::Type args_09,
                         typename bslmf::ForwardingType<ARGS_10>::Type args_10)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args_01,
             args_02,
             args_03,
             args_04,
             args_05,
             args_06,
             args_07,
             args_08,
             args_09,
             args_10);
}


template <class RET>
template <class FUNC>
inline
typename bsl::function<RET()>::Invoker *
bsl::function<RET()>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01>
template <class FUNC>
inline
typename bsl::function<RET(ARGS_01)>::Invoker *
bsl::function<RET(ARGS_01)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template <class FUNC>
inline
typename bsl::function<RET(ARGS_01,
                           ARGS_02)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template <class FUNC>
inline
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template <class FUNC>
inline
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template <class FUNC>
inline
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template <class FUNC>
inline
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template <class FUNC>
inline
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template <class FUNC>
inline
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template <class FUNC>
inline
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08,
                           ARGS_09)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

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
template <class FUNC>
inline
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08,
                           ARGS_09,
                           ARGS_10)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}


template <class RET>
template <class FUNC>
typename bsl::function<RET()>::Invoker *
bsl::function<RET()>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01>
template <class FUNC>
typename bsl::function<RET(ARGS_01)>::Invoker *
bsl::function<RET(ARGS_01)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08,
                           ARGS_09)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

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
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08,
                           ARGS_09,
                           ARGS_10)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}


template <class RET>
template <class FUNC>
typename bsl::function<RET()>::Invoker *
bsl::function<RET()>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01>
template <class FUNC>
typename bsl::function<RET(ARGS_01)>::Invoker *
bsl::function<RET(ARGS_01)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08,
                           ARGS_09)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

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
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08,
                           ARGS_09,
                           ARGS_10)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}


template <class RET>
template <class FUNC>
typename bsl::function<RET()>::Invoker *
bsl::function<RET()>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01>
template <class FUNC>
typename bsl::function<RET(ARGS_01)>::Invoker *
bsl::function<RET(ARGS_01)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08,
                           ARGS_09)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

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
template <class FUNC>
typename bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08,
                           ARGS_09,
                           ARGS_10)>::Invoker *
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}


template <class RET>
bsl::function<RET()>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01>
bsl::function<RET(ARGS_01)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

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
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}


template <class RET>
bsl::function<RET()>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01>
bsl::function<RET(ARGS_01)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

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
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}


template <class RET>
bsl::function<RET()>::function(const function&)
{
}

template <class RET, class ARGS_01>
bsl::function<RET(ARGS_01)>::function(const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::function(const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::function(const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::function(const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::function(const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::function(const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::function(const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::function(const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::function(const function&)
{
}

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
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::function(const function&)
{
}


template <class RET>
template<class FUNC>
bsl::function<RET()>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class ARGS_01>
template<class FUNC>
bsl::function<RET(ARGS_01)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

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
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}


template <class RET>
template<class ALLOC>
bsl::function<RET()>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)() = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01>
template<class ALLOC>
bsl::function<RET(ARGS_01)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS_01) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

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
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09,
                      ARGS_10) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}


template <class RET>
template<class ALLOC>
bsl::function<RET()>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)() = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01>
template<class ALLOC>
bsl::function<RET(ARGS_01)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS_01) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

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
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09,
                      ARGS_10) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}


template <class RET>
template<class ALLOC>
bsl::function<RET()>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class ARGS_01>
template<class ALLOC>
bsl::function<RET(ARGS_01)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

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
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}


template <class RET>
template<class FUNC, class ALLOC>
bsl::function<RET()>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

template <class RET, class ARGS_01>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS_01)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

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
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}


#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET>
bsl::function<RET()>::function(function&&)
{
}

template <class RET, class ARGS_01>
bsl::function<RET(ARGS_01)>::function(function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::function(function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::function(function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::function(function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::function(function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::function(function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::function(function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::function(function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::function(function&&)
{
}

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
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::function(function&&)
{
}


template <class RET>
template<class ALLOC>
bsl::function<RET()>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

template <class RET, class ARGS_01>
template<class ALLOC>
bsl::function<RET(ARGS_01
                  )>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02
                  )>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03
                  )>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04
                  )>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05
                  )>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06
                  )>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07
                  )>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08
                  )>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09
                  )>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

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
template<class ALLOC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10
                  )>::function(allocator_arg_t, const ALLOC&, function&&)
{
}


#endif

template <class RET>
bsl::function<RET()>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

template <class RET, class ARGS_01>
bsl::function<RET(ARGS_01)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

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
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}


template <class RET>
bsl::function<RET()>&
bsl::function<RET()>::operator=(const function&)
{
    return *this;
}

template <class RET, class ARGS_01>
bsl::function<RET(ARGS_01)>&
bsl::function<RET(ARGS_01)>::operator=(const function&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
bsl::function<RET(ARGS_01,
                  ARGS_02)>&
bsl::function<RET(ARGS_01,
                  ARGS_02)>::operator=(const function&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::operator=(const function&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::operator=(const function&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::operator=(const function&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::operator=(const function&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::operator=(const function&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::operator=(const function&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::operator=(const function&)
{
    return *this;
}

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
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::operator=(const function&)
{
    return *this;
}


#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET>
bsl::function<RET()>&
bsl::function<RET()>::operator=(function&&)
{
    return *this;
}

template <class RET, class ARGS_01>
bsl::function<RET(ARGS_01)>&
bsl::function<RET(ARGS_01)>::operator=(function&&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
bsl::function<RET(ARGS_01,
                  ARGS_02)>&
bsl::function<RET(ARGS_01,
                  ARGS_02)>::operator=(function&&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::operator=(function&&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::operator=(function&&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::operator=(function&&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::operator=(function&&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::operator=(function&&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::operator=(function&&)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::operator=(function&&)
{
    return *this;
}

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
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::operator=(function&&)
{
    return *this;
}


#endif

template <class RET>
bsl::function<RET()>&
bsl::function<RET()>::operator=(nullptr_t)
{
    return *this;
}

template <class RET, class ARGS_01>
bsl::function<RET(ARGS_01)>&
bsl::function<RET(ARGS_01)>::operator=(nullptr_t)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
bsl::function<RET(ARGS_01,
                  ARGS_02)>&
bsl::function<RET(ARGS_01,
                  ARGS_02)>::operator=(nullptr_t)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::operator=(nullptr_t)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::operator=(nullptr_t)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::operator=(nullptr_t)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::operator=(nullptr_t)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::operator=(nullptr_t)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::operator=(nullptr_t)
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::operator=(nullptr_t)
{
    return *this;
}

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
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::operator=(nullptr_t)
{
    return *this;
}


template <class RET>
template<class FUNC>
bsl::function<RET()>&
bsl::function<RET()>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}

template <class RET, class ARGS_01>
template<class FUNC>
bsl::function<RET(ARGS_01)>&
bsl::function<RET(ARGS_01)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02)>&
bsl::function<RET(ARGS_01,
                  ARGS_02)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}

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
template<class FUNC>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}



template <class RET>
void bsl::function<RET()>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class ARGS_01>
void bsl::function<RET(ARGS_01)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class ARGS_01,
                     class ARGS_02>
void bsl::function<RET(ARGS_01,
                       ARGS_02)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07,
                       ARGS_08)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07,
                       ARGS_08,
                       ARGS_09)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

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
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07,
                       ARGS_08,
                       ARGS_09,
                       ARGS_10)>::swap(function&) BSLS_NOTHROW_SPEC
{
}


template <class RET>
template<class FUNC, class ALLOC>
void bsl::function<RET()>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

template <class RET, class ARGS_01>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS_01)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS_01,
                       ARGS_02)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07,
                       ARGS_08)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07,
                       ARGS_08,
                       ARGS_09)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

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
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS_01,
                       ARGS_02,
                       ARGS_03,
                       ARGS_04,
                       ARGS_05,
                       ARGS_06,
                       ARGS_07,
                       ARGS_08,
                       ARGS_09,
                       ARGS_10)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}


template <class RET>
bsl::function<RET()>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

template <class RET, class ARGS_01>
bsl::function<RET(ARGS_01)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

template <class RET, class ARGS_01,
                     class ARGS_02>
bsl::function<RET(ARGS_01,
                  ARGS_02)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

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
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}


template <class RET>
RET bsl::function<RET()>::operator()() const
{
    if (d_invoker_p) {
        return d_invoker_p(this);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class ARGS_01>
RET bsl::function<RET(ARGS_01)>::operator()(ARGS_01 args_01) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args_01);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02>
RET bsl::function<RET(ARGS_01,
                      ARGS_02)>::operator()(ARGS_01 args_01,
                                            ARGS_02 args_02) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args_01,
                                 args_02);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03)>::operator()(ARGS_01 args_01,
                                            ARGS_02 args_02,
                                            ARGS_03 args_03) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args_01,
                                 args_02,
                                 args_03);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04)>::operator()(ARGS_01 args_01,
                                            ARGS_02 args_02,
                                            ARGS_03 args_03,
                                            ARGS_04 args_04) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args_01,
                                 args_02,
                                 args_03,
                                 args_04);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05)>::operator()(ARGS_01 args_01,
                                            ARGS_02 args_02,
                                            ARGS_03 args_03,
                                            ARGS_04 args_04,
                                            ARGS_05 args_05) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args_01,
                                 args_02,
                                 args_03,
                                 args_04,
                                 args_05);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06)>::operator()(ARGS_01 args_01,
                                            ARGS_02 args_02,
                                            ARGS_03 args_03,
                                            ARGS_04 args_04,
                                            ARGS_05 args_05,
                                            ARGS_06 args_06) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args_01,
                                 args_02,
                                 args_03,
                                 args_04,
                                 args_05,
                                 args_06);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07)>::operator()(ARGS_01 args_01,
                                            ARGS_02 args_02,
                                            ARGS_03 args_03,
                                            ARGS_04 args_04,
                                            ARGS_05 args_05,
                                            ARGS_06 args_06,
                                            ARGS_07 args_07) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args_01,
                                 args_02,
                                 args_03,
                                 args_04,
                                 args_05,
                                 args_06,
                                 args_07);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08)>::operator()(ARGS_01 args_01,
                                            ARGS_02 args_02,
                                            ARGS_03 args_03,
                                            ARGS_04 args_04,
                                            ARGS_05 args_05,
                                            ARGS_06 args_06,
                                            ARGS_07 args_07,
                                            ARGS_08 args_08) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args_01,
                                 args_02,
                                 args_03,
                                 args_04,
                                 args_05,
                                 args_06,
                                 args_07,
                                 args_08);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09)>::operator()(ARGS_01 args_01,
                                            ARGS_02 args_02,
                                            ARGS_03 args_03,
                                            ARGS_04 args_04,
                                            ARGS_05 args_05,
                                            ARGS_06 args_06,
                                            ARGS_07 args_07,
                                            ARGS_08 args_08,
                                            ARGS_09 args_09) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args_01,
                                 args_02,
                                 args_03,
                                 args_04,
                                 args_05,
                                 args_06,
                                 args_07,
                                 args_08,
                                 args_09);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

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
RET bsl::function<RET(ARGS_01,
                      ARGS_02,
                      ARGS_03,
                      ARGS_04,
                      ARGS_05,
                      ARGS_06,
                      ARGS_07,
                      ARGS_08,
                      ARGS_09,
                      ARGS_10)>::operator()(ARGS_01 args_01,
                                            ARGS_02 args_02,
                                            ARGS_03 args_03,
                                            ARGS_04 args_04,
                                            ARGS_05 args_05,
                                            ARGS_06 args_06,
                                            ARGS_07 args_07,
                                            ARGS_08 args_08,
                                            ARGS_09 args_09,
                                            ARGS_10 args_10) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args_01,
                                 args_02,
                                 args_03,
                                 args_04,
                                 args_05,
                                 args_06,
                                 args_07,
                                 args_08,
                                 args_09,
                                 args_10);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}


template <class RET>
const std::type_info&
bsl::function<RET()>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class ARGS_01>
const std::type_info&
bsl::function<RET(ARGS_01)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class ARGS_01,
                     class ARGS_02>
const std::type_info&
bsl::function<RET(ARGS_01,
                  ARGS_02)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
const std::type_info&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
const std::type_info&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
const std::type_info&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
const std::type_info&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
const std::type_info&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
const std::type_info&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
const std::type_info&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

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
const std::type_info&
bsl::function<RET(ARGS_01,
                  ARGS_02,
                  ARGS_03,
                  ARGS_04,
                  ARGS_05,
                  ARGS_06,
                  ARGS_07,
                  ARGS_08,
                  ARGS_09,
                  ARGS_10)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}


template <class RET>
template<class T>
T* bsl::function<RET()>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class ARGS_01>
template<class T>
T* bsl::function<RET(ARGS_01)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template<class T>
T* bsl::function<RET(ARGS_01,
                     ARGS_02)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template<class T>
T* bsl::function<RET(ARGS_01,
                     ARGS_02,
                     ARGS_03)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template<class T>
T* bsl::function<RET(ARGS_01,
                     ARGS_02,
                     ARGS_03,
                     ARGS_04)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template<class T>
T* bsl::function<RET(ARGS_01,
                     ARGS_02,
                     ARGS_03,
                     ARGS_04,
                     ARGS_05)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template<class T>
T* bsl::function<RET(ARGS_01,
                     ARGS_02,
                     ARGS_03,
                     ARGS_04,
                     ARGS_05,
                     ARGS_06)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template<class T>
T* bsl::function<RET(ARGS_01,
                     ARGS_02,
                     ARGS_03,
                     ARGS_04,
                     ARGS_05,
                     ARGS_06,
                     ARGS_07)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template<class T>
T* bsl::function<RET(ARGS_01,
                     ARGS_02,
                     ARGS_03,
                     ARGS_04,
                     ARGS_05,
                     ARGS_06,
                     ARGS_07,
                     ARGS_08)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template<class T>
T* bsl::function<RET(ARGS_01,
                     ARGS_02,
                     ARGS_03,
                     ARGS_04,
                     ARGS_05,
                     ARGS_06,
                     ARGS_07,
                     ARGS_08,
                     ARGS_09)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

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
template<class T>
T* bsl::function<RET(ARGS_01,
                     ARGS_02,
                     ARGS_03,
                     ARGS_04,
                     ARGS_05,
                     ARGS_06,
                     ARGS_07,
                     ARGS_08,
                     ARGS_09,
                     ARGS_10)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}


template <class RET>
template<class T>
const T* bsl::function<RET()>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

template <class RET, class ARGS_01>
template<class T>
const T* bsl::function<RET(ARGS_01)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

template <class RET, class ARGS_01,
                     class ARGS_02>
template<class T>
const T* bsl::function<RET(ARGS_01,
                           ARGS_02)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
template<class T>
const T* bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
template<class T>
const T* bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
template<class T>
const T* bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
template<class T>
const T* bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
template<class T>
const T* bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
template<class T>
const T* bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
template<class T>
const T* bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08,
                           ARGS_09)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

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
template<class T>
const T* bsl::function<RET(ARGS_01,
                           ARGS_02,
                           ARGS_03,
                           ARGS_04,
                           ARGS_05,
                           ARGS_06,
                           ARGS_07,
                           ARGS_08,
                           ARGS_09,
                           ARGS_10)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}


template <class RET>
bool operator==(const bsl::function<RET()>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01>
bool operator==(const bsl::function<RET(ARGS_01)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02>
bool operator==(const bsl::function<RET(ARGS_01,
                                        ARGS_02)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bool operator==(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bool operator==(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bool operator==(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bool operator==(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bool operator==(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bool operator==(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bool operator==(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08,
                                        ARGS_09)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

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
bool operator==(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08,
                                        ARGS_09,
                                        ARGS_10)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;


template <class RET>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET()>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08,
                                        ARGS_09)>&) BSLS_NOTHROW_SPEC;

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
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08,
                                        ARGS_09,
                                        ARGS_10)>&) BSLS_NOTHROW_SPEC;


template <class RET>
bool operator!=(const bsl::function<RET()>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01>
bool operator!=(const bsl::function<RET(ARGS_01)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02>
bool operator!=(const bsl::function<RET(ARGS_01,
                                        ARGS_02)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bool operator!=(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bool operator!=(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bool operator!=(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bool operator!=(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bool operator!=(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bool operator!=(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bool operator!=(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08,
                                        ARGS_09)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

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
bool operator!=(const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08,
                                        ARGS_09,
                                        ARGS_10)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;


template <class RET>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET()>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08)>&) BSLS_NOTHROW_SPEC;

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08,
                                        ARGS_09)>&) BSLS_NOTHROW_SPEC;

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
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS_01,
                                        ARGS_02,
                                        ARGS_03,
                                        ARGS_04,
                                        ARGS_05,
                                        ARGS_06,
                                        ARGS_07,
                                        ARGS_08,
                                        ARGS_09,
                                        ARGS_10)>&) BSLS_NOTHROW_SPEC;


template <class RET>
inline
void swap(bsl::function<RET()>& a, bsl::function<RET()>& b)
{
    a.swap(b);
}

template <class RET, class ARGS_01>
inline
void swap(bsl::function<RET(ARGS_01)>& a, bsl::function<RET(ARGS_01)>& b)
{
    a.swap(b);
}

template <class RET, class ARGS_01,
                     class ARGS_02>
inline
void swap(bsl::function<RET(ARGS_01,
                            ARGS_02)>& a, bsl::function<RET(ARGS_01,
                                                            ARGS_02)>& b)
{
    a.swap(b);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03>
inline
void swap(bsl::function<RET(ARGS_01,
                            ARGS_02,
                            ARGS_03)>& a, bsl::function<RET(ARGS_01,
                                                            ARGS_02,
                                                            ARGS_03)>& b)
{
    a.swap(b);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04>
inline
void swap(bsl::function<RET(ARGS_01,
                            ARGS_02,
                            ARGS_03,
                            ARGS_04)>& a, bsl::function<RET(ARGS_01,
                                                            ARGS_02,
                                                            ARGS_03,
                                                            ARGS_04)>& b)
{
    a.swap(b);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05>
inline
void swap(bsl::function<RET(ARGS_01,
                            ARGS_02,
                            ARGS_03,
                            ARGS_04,
                            ARGS_05)>& a, bsl::function<RET(ARGS_01,
                                                            ARGS_02,
                                                            ARGS_03,
                                                            ARGS_04,
                                                            ARGS_05)>& b)
{
    a.swap(b);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06>
inline
void swap(bsl::function<RET(ARGS_01,
                            ARGS_02,
                            ARGS_03,
                            ARGS_04,
                            ARGS_05,
                            ARGS_06)>& a, bsl::function<RET(ARGS_01,
                                                            ARGS_02,
                                                            ARGS_03,
                                                            ARGS_04,
                                                            ARGS_05,
                                                            ARGS_06)>& b)
{
    a.swap(b);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07>
inline
void swap(bsl::function<RET(ARGS_01,
                            ARGS_02,
                            ARGS_03,
                            ARGS_04,
                            ARGS_05,
                            ARGS_06,
                            ARGS_07)>& a, bsl::function<RET(ARGS_01,
                                                            ARGS_02,
                                                            ARGS_03,
                                                            ARGS_04,
                                                            ARGS_05,
                                                            ARGS_06,
                                                            ARGS_07)>& b)
{
    a.swap(b);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08>
inline
void swap(bsl::function<RET(ARGS_01,
                            ARGS_02,
                            ARGS_03,
                            ARGS_04,
                            ARGS_05,
                            ARGS_06,
                            ARGS_07,
                            ARGS_08)>& a, bsl::function<RET(ARGS_01,
                                                            ARGS_02,
                                                            ARGS_03,
                                                            ARGS_04,
                                                            ARGS_05,
                                                            ARGS_06,
                                                            ARGS_07,
                                                            ARGS_08)>& b)
{
    a.swap(b);
}

template <class RET, class ARGS_01,
                     class ARGS_02,
                     class ARGS_03,
                     class ARGS_04,
                     class ARGS_05,
                     class ARGS_06,
                     class ARGS_07,
                     class ARGS_08,
                     class ARGS_09>
inline
void swap(bsl::function<RET(ARGS_01,
                            ARGS_02,
                            ARGS_03,
                            ARGS_04,
                            ARGS_05,
                            ARGS_06,
                            ARGS_07,
                            ARGS_08,
                            ARGS_09)>& a, bsl::function<RET(ARGS_01,
                                                            ARGS_02,
                                                            ARGS_03,
                                                            ARGS_04,
                                                            ARGS_05,
                                                            ARGS_06,
                                                            ARGS_07,
                                                            ARGS_08,
                                                            ARGS_09)>& b)
{
    a.swap(b);
}

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
inline
void swap(bsl::function<RET(ARGS_01,
                            ARGS_02,
                            ARGS_03,
                            ARGS_04,
                            ARGS_05,
                            ARGS_06,
                            ARGS_07,
                            ARGS_08,
                            ARGS_09,
                            ARGS_10)>& a, bsl::function<RET(ARGS_01,
                                                            ARGS_02,
                                                            ARGS_03,
                                                            ARGS_04,
                                                            ARGS_05,
                                                            ARGS_06,
                                                            ARGS_07,
                                                            ARGS_08,
                                                            ARGS_09,
                                                            ARGS_10)>& b)
{
    a.swap(b);
}

#else
// The generated code below is a workaround for the absence of perfect
// forwarding in some compilers.

template <class RET, class... ARGS>
template <class FUNC>
RET bsl::function<RET(ARGS...)>::functionPtrInvoker(const Function_Rep *rep,
                            typename bslmf::ForwardingType<ARGS>::Type... args)
{
    FUNC f = reinterpret_cast<FUNC>(rep->d_objbuf.d_func_p);

    return static_cast<RET>(f(args...));
}

template <class RET, class... ARGS>
template <class FUNC>
RET bsl::function<RET(ARGS...)>::memFuncPtrInvoker(const Function_Rep *rep, 
                            typename bslmf::ForwardingType<ARGS>::Type... args)
{
    FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    typedef typename bslmf::NthParameter<0, ARGS...>::Type ObjType;
    typedef Function_MemFuncInvoke<FUNC, ObjType> InvokeType;
    BSLMF_ASSERT(sizeof...(ARGS) == InvokeType::NUM_ARGS + 1);
    return (RET) InvokeType::invoke(f, args...);
}

template <class RET, class... ARGS>
template <class FUNC>
RET bsl::function<RET(ARGS...)>::inplaceFunctorInvoker(const Function_Rep *rep, 
                            typename bslmf::ForwardingType<ARGS>::Type... args)
{
    FUNC& f = reinterpret_cast<FUNC&>(rep->d_objbuf);
    return f(args...);
}

template <class RET, class... ARGS>
template <class FUNC>
RET
bsl::function<RET(ARGS...)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                            typename bslmf::ForwardingType<ARGS>::Type... args)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    return f(args...);
}

template <class RET, class... ARGS>
template <class FUNC>
inline
typename bsl::function<RET(ARGS...)>::Invoker *
bsl::function<RET(ARGS...)>::getInvoker(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    if (f) {
        return &functionPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class... ARGS>
template <class FUNC>
typename bsl::function<RET(ARGS...)>::Invoker *
bsl::function<RET(ARGS...)>::getInvoker(const FUNC& f,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    if (f) {
        return &memFuncPtrInvoker<FUNC>;
    }
    else {
        return NULL;
    }
}

template <class RET, class... ARGS>
template <class FUNC>
typename bsl::function<RET(ARGS...)>::Invoker *
bsl::function<RET(ARGS...)>::getInvoker(const FUNC&,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    return &inplaceFunctorInvoker<FUNC>;
}

template <class RET, class... ARGS>
template <class FUNC>
typename bsl::function<RET(ARGS...)>::Invoker *
bsl::function<RET(ARGS...)>::getInvoker(const FUNC&, bslmf::SelectTraitCase<>)
{
    return &outofplaceFunctorInvoker<FUNC>;
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function(const function&)
{
}

template <class RET, class... ARGS>
template<class FUNC>
bsl::function<RET(ARGS...)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::Type TraitSelection;

    d_invoker_p = getInvoker(func, TraitSelection());
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC& alloc)
{
    RET (*nullfunc_p)(ARGS...) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    RET (*nullfunc_p)(ARGS...) = NULL;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = NULL;
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class RET, class... ARGS>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function(function&&)
{
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

#endif

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::~function()
{
    if (d_funcManager_p) {
        d_funcManager_p(DESTROY, NULL, this);
    }

    if (d_allocManager_p) {
        d_allocManager_p(DESTROY, NULL, this);
    }
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(const function&)
{
    return *this;
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(function&&)
{
    return *this;
}

#endif

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(nullptr_t)
{
    return *this;
}

template <class RET, class... ARGS>
template<class FUNC>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC))
{
    return *this;
}


template <class RET, class... ARGS>
void bsl::function<RET(ARGS...)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class RET, class... ARGS>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS...)>::assign(
                                       BSLS_COMPILERFEATURES_FORWARD_REF(FUNC),
                                       const ALLOC&)
{
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::operator bool() const BSLS_NOTHROW_SPEC
{
    return d_invoker_p;
}

template <class RET, class... ARGS>
RET bsl::function<RET(ARGS...)>::operator()(ARGS... args) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args...);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class RET, class... ARGS>
const std::type_info&
bsl::function<RET(ARGS...)>::target_type() const BSLS_NOTHROW_SPEC
{
    if (! *this) {
        return typeid(void);
    }

    const void *ret = d_funcManager_p(GET_TYPE_ID, this, NULL);
    return *static_cast<const std::type_info*>(ret);
}

template <class RET, class... ARGS>
template<class T>
T* bsl::function<RET(ARGS...)>::target() BSLS_NOTHROW_SPEC
{
    if ((! *this) || target_type() != typeid(T)) {
        return NULL;
    }

    const void *target_p = d_funcManager_p(GET_TARGET, NULL, this);
    return static_cast<T*>(const_cast<void*>(target_p));
}

template <class RET, class... ARGS>
template<class T>
const T* bsl::function<RET(ARGS...)>::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<function*>(this)->target<T>();
}

template <class RET, class... ARGS>
bool operator==(const bsl::function<RET(ARGS...)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator==(bsl::nullptr_t,
                const bsl::function<RET(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator!=(const bsl::function<RET(ARGS...)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool operator!=(bsl::nullptr_t,
                const bsl::function<RET(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
inline
void swap(bsl::function<RET(ARGS...)>& a, bsl::function<RET(ARGS...)>& b)
{
    a.swap(b);
}

// }}} END GENERATED CODE
#endif

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
