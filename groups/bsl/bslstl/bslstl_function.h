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
#error "include <bsl_functional.h> instead of <bslstl_function.h> in \
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

namespace bsl {

// TBD: Move into its own component (or into uses_allocator component)
struct allocator_arg_t { };
static const allocator_arg_t allocator_arg;

// Forward declarations
template <class FUNC>
class function;  // Primary template declared but not defined.

template <class MEM_FUNC_PTR, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke; // Primary template declared but not defined.

template <class ALLOC>
struct Function_AllocTraits;

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
    // This is a component-private class template.  Do not use.
    //
    // The standard requires that 'function' define certain typedefs for
    // compatibility with one- and two-argument legacy functor adaptors. This
    // template provides the following nested typedefs:
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
    // This is a component-private class template.  Do not use.
    //
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
    // This is a component-private class template.  Do not use.
    //
    // Metafunction to return the alignment needed for TYPE.  If 'TYPE' is
    // 'void', then the 'VALUE' member will be 1; otherwise the 'VALUE' member
    // will be bsls::AlignmentFromType<TYPE>::VALUE.

    static const std::size_t VALUE = bsls::AlignmentFromType<TYPE>::VALUE;
                                     
};

template <>
struct Function_ObjAlignment<void> {
    static const std::size_t VALUE = 1;
};

                        // ==================
                        // class Function_Rep
                        // ==================

class Function_Rep {
    // This is a component-private class.  Do not use.
    //
    // This class provides a non-template representation for a 'bsl::function'
    // instance.  It handles all of the object-management parts of
    // 'bsl::function' that are not specific to the argument list or return
    // type: storing, copying, and moving the function object and allocator,
    // but not invoking the function (which would require knowledge of the
    // function prototype). These management functions are run-time
    // polymorphic, and therefore do not require that this class be templated
    // (although several of the member functions are templated). For technical
    // reasons, this class must be defined before 'bsl::function' (although a
    // mere forward declaration would be all right with most compilers, the
    // Gnu compiler emits an error when trying to do syntactic checking on
    // template code even though it does not instantiate template).

    // TYPES

    // Short aliases for type with maximum platform alignment
    typedef bsls::AlignmentUtil::MaxAlignedType MaxAlignedType;

    union PtrOrSize_t {
        // This union stores either a pointer to const void or a size_t.  It
        // is used as the input argument and return type for manager functions
        // (below).
    private:
        std::size_t  d_asSize_t;
        const void  *d_asPtr;

    public:
        PtrOrSize_t(std::size_t s) : d_asSize_t(s) { }
        PtrOrSize_t(const void *p) : d_asPtr(p) { }
        PtrOrSize_t() : d_asPtr(0) { }

        std::size_t asSize_t() const { return d_asSize_t; }
        const void *asPtr() const { return d_asPtr; }
    };

    enum ManagerOpCode {
        // This enumeration provide values to identify operations to be
        // performed by an object manager function (below).  The object being
        // managed is either the function object in the case of the function
        // manager or the allocator object in the case of the allocator
        // manager.

        e_MOVE_CONSTRUCT
            // move-construct the object in 'rep' from the object pointed to
            // by 'input'.  Return the number of bytes needed to hold the
            // object.

      , e_COPY_CONSTRUCT
            // copy-construct the object in 'rep' from the object pointed to
            // by 'input'.  Return the number of bytes needed to hold the
            // object.

      , e_DESTROY
            // Call the destructor on the object in 'rep'.  Return the number
            // of bytes needed to hold the destroyed object.  Some managers
            // also deallocate memory.

      , e_GET_SIZE
            // Return the size of the object.  ('rep' and 'input' are ignored.)

      , e_GET_TARGET
            // Return a pointer to the object in 'rep'.

      , e_GET_TYPE_ID
            // Return a pointer to the 'type_info' for the object in 'rep'.

      , e_INIT_REP
            // (Allocator manager only) initialize 'rep' using the 'input'
            // allocator.  Requires that 'input' point to an object of the
            // manager's 'ALLOC' type and that 'rep->d_funcManager_p' is
            // already set.
    };

    enum AllocCategory {
        // Category of allocator supplied to a constructor.

        e_BSLMA_ALLOC_PTR        // Ptr to type derived from 'bslma::Allocator'
      , e_BSL_ALLOCATOR          // Instantiation of 'bsl::allocator'
      , e_ERASED_STATEFUL_ALLOC  // C++03 STL-style stateful allocator
      , e_ERASED_STATELESS_ALLOC // C++03 STL-style stateless allocator
    };

    typedef PtrOrSize_t (*Manager)(ManagerOpCode  opCode,
                                   Function_Rep  *rep,
                                   PtrOrSize_t    input);
        // 'Manager' is an alias for a pointer to a function that manages a
        // specific object type (i.e., it copies, moves, or destroyes it).  It
        // implements a kind of hand-coded virtual-function dispatch.  The
        // specified 'opCode' is used to choose the "virtual function" to
        // invoke, where the specified 'rep' and 'input' are arguments to
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
        // be done by the manager with the opcode 'e_GET_SIZE'.
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

    template <class ALLOC>
    friend struct Function_AllocTraits;

    void *initRep(std::size_t funcSize, bslma::Allocator* alloc,
                  integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>);
    template <class T>
    void *initRep(std::size_t funcSize, const bsl::allocator<T>& alloc,
                  integral_constant<AllocCategory, e_BSL_ALLOCATOR>);
    template <class ALLOC>
    void *initRep(std::size_t funcSize, const ALLOC& alloc,
                  integral_constant<AllocCategory, e_ERASED_STATEFUL_ALLOC>);
    template <class ALLOC>
    void *initRep(std::size_t funcSize, const ALLOC& alloc,
                  integral_constant<AllocCategory, e_ERASED_STATELESS_ALLOC>);
        // Initialize this object's 'd_objbuf', 'd_allocator_p', and
        // 'd_allocManager_p' fields, allocating (if necessary) enough storage
        // to hold a function object of the specified 'funcSize' and holding a
        // copy of 'alloc'.  If the function and allocator fit within
        // 'd_objbuf', then no memory is allocated.  The actual wrapped
        // function object is not initialized, nor is 'd_funcManager_p' set.

    bool equalAlloc(bslma::Allocator* alloc,
                    integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>) const;
    template <class T>
    bool equalAlloc(const bsl::allocator<T>& alloc,
                    integral_constant<AllocCategory, e_BSL_ALLOCATOR>) const;
    template <class ALLOC>
    bool equalAlloc(const ALLOC& alloc,
              integral_constant<AllocCategory, e_ERASED_STATEFUL_ALLOC>) const;
    template <class ALLOC>
    bool equalAlloc(const ALLOC& alloc,
              integral_constant<AllocCategory,e_ERASED_STATELESS_ALLOC>) const;
        // Return true if the specified 'alloc' can be used to free memory
        // from the allocator stored in this object and vice versa, and false
        // otherwise.

    template <class ALLOC, AllocCategory ATP>
    void copyRep(const Function_Rep&                   other,
                 const ALLOC&                          alloc,
                 integral_constant<AllocCategory, ATP> atp);

    template <class FUNC>
    static PtrOrSize_t functionManager(ManagerOpCode  opCode,
                                       Function_Rep  *rep,
                                       PtrOrSize_t    input);

    static PtrOrSize_t unownedAllocManager(ManagerOpCode  opCode,
                                           Function_Rep  *rep,
                                           PtrOrSize_t    input);
        // Note that this function is not a template.  It is defined in the
        // component .cpp file.

    template <class ALLOC>
    static PtrOrSize_t ownedAllocManager(ManagerOpCode  opCode,
                                         Function_Rep  *rep,
                                         PtrOrSize_t    input);

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
    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
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
        // reference back to the original type.  The 'const_cast', below,
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

    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS>::Type... args);

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

    template <class ALLOC>
    void copyInit(const ALLOC& alloc, const function& other);

    void moveInit(function& other);

public:
    // PUBLIC TYPES
    typedef RET result_type;

    // CREATORS
    function() BSLS_NOTHROW_SPEC;
    function(nullptr_t) BSLS_NOTHROW_SPEC;
    function(const function& other);
    template<class FUNC> function(FUNC func);
    template<class ALLOC> function(allocator_arg_t, const ALLOC& alloc);
    template<class ALLOC> function(allocator_arg_t, const ALLOC& alloc,
                                   nullptr_t);
    template<class ALLOC> function(allocator_arg_t,
                                   const ALLOC&    alloc,
                                   const function& other);
    template<class FUNC, class ALLOC> function(allocator_arg_t,
                                               const ALLOC& alloc,
                                               FUNC         func);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    function(function&& other);
    template<class ALLOC> function(allocator_arg_t,
                                   const ALLOC& alloc,
                                   function&&   other);
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

    RET operator()(ARGS...) const;

    // ACCESSORS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit  // Explicit conversion available only with C++11
    operator bool() const BSLS_NOTHROW_SPEC;
#else
    operator bool() const BSLS_NOTHROW_SPEC;
#endif

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
                        // class Function_PairBufDesc
                        // --------------------------------

namespace bsl {

class Function_PairBufDesc {
    // Descriptor for a maximally-aligned memory buffer that can hold two
    // objects.  Given a pointer to a maximally-aligned memory buffer, it
    // provides sufficient information to access the first and second objects
    // stored within that memory buffer, as if the memory buffer were a pair
    // struct.  In order to generate a descriptor, we need only the size of
    // the two objects being stored, thus allowing a descriptor to be created
    // dynamically for objects whose types are not known until runtime.  Note
    // that no attempt is made to match the layout of a 'std::pair' containing
    // the members of the same data types and, in practice, the layout will
    // often differ from 'std::pair'.

    std::size_t d_totalSize;    // Total size of the buffer
    std::size_t d_secondOffset; // Offset within the buffer of the 2nd object

public:
    // CREATORS
    Function_PairBufDesc(std::size_t t1Size, std::size_t t2Size);
        // Construct a descriptor for a maximally-aligned pair of objects with
        // the specified sizes 't1Size' and 't2Size'.

    //! Function_PairBufDesc(const Function_PairBufDesc&);
    //! ~Function_PairBufDesc();
    //! Function_PairBufDesc& operator=(const Function_PairBufDesc&);

    // ACCESSORS
    std::size_t totalSize() const;
        // Return the total size of the maximally-aligned buffer needed to
        // hold two objects whose sizes were specified in the constructor.

    void       *first(void       *buffer) const;
    void const *first(void const *buffer) const;
        // Treat the specified 'buffer' as a pointer to memory intended to
        // hold two objects whose sizes were specified in the constructor of
        // this 'Function_PairBufDesc' and return the address of the first of
        // those two objects.  The behavior is undefined unless 'buffer' is
        // the address of a maximally-aligned block of memory of at least
        // 'totalSize()' bytes.

    void       *second(void       *buffer) const;
    void const *second(void const *buffer) const;
        // Treat the specified 'buffer' as a pointer to memory intended to
        // hold two objects whose sizes were specified in the constructor of
        // this 'Function_PairBufDesc' and return the address of the second of
        // those two objects.  The behavior is undefined unless 'buffer' is
        // the address of a maximally-aligned block of memory of at least
        // 'totalSize()' bytes.
};

template <class ALLOC>
struct Function_AllocTraits
{
    // Determine the category of an allocator and compute a normalized type
    // for allocators that belong to the same family.

private:
    typedef Function_Rep::AllocCategory         AllocCategory;

public:
    static const AllocCategory k_CATEGORY =
        (bsl::is_empty<ALLOC>::value ?
         Function_Rep::e_ERASED_STATELESS_ALLOC :
         Function_Rep::e_ERASED_STATEFUL_ALLOC);

    typedef bsl::integral_constant<AllocCategory, k_CATEGORY> Category;
    typedef typename ALLOC::template rebind<char>::other      Type;
};

template <class ALLOC>
struct Function_AllocTraits<ALLOC *>
{
    // Specialization for pointer to type derived from 'bslma::Allocator'.

private:
    BSLMF_ASSERT((bsl::is_convertible<ALLOC *, bslma::Allocator*>::value));

    typedef Function_Rep::AllocCategory AllocCategory;

public:
    static const AllocCategory k_CATEGORY = Function_Rep::e_BSLMA_ALLOC_PTR;

    typedef bsl::integral_constant<AllocCategory, k_CATEGORY>  Category;
    typedef bslma::Allocator                                  *Type;
};

template <class TYPE>
struct Function_AllocTraits<bsl::allocator<TYPE> >
{
    // Specialization for 'bsl::allocator<TYPE>'

private:
    typedef Function_Rep::AllocCategory AllocCategory;

public:
    static const AllocCategory k_CATEGORY = Function_Rep::e_BSL_ALLOCATOR;

    typedef bsl::integral_constant<AllocCategory, k_CATEGORY> Category;
    typedef bsl::allocator<char>                              Type;
};

} // close namespace bsl

// CREATORS
inline
bsl::Function_PairBufDesc::Function_PairBufDesc(std::size_t t1Size,
                                                std::size_t t2Size)
{
    static const std::size_t k_MAX_ALIGNMENT =
        bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

    d_totalSize = ((t1Size + t2Size + k_MAX_ALIGNMENT - 1) &
                   ~(k_MAX_ALIGNMENT - 1));

    d_secondOffset = d_totalSize - t2Size;
}

// ACCESSORS
inline std::size_t bsl::Function_PairBufDesc::totalSize() const
{
    return d_totalSize;
}

inline void       *bsl::Function_PairBufDesc::first(void       *buffer) const
{
    return buffer;
}

inline void const *bsl::Function_PairBufDesc::first(void const *buffer) const
{
    return buffer;
}

inline void       *bsl::Function_PairBufDesc::second(void       *buffer) const
{
    return d_secondOffset + static_cast<char*>(buffer);
}

inline void const *bsl::Function_PairBufDesc::second(void const *buffer) const
{
    return d_secondOffset + static_cast<const char*>(buffer);
}

                        // -----------------------
                        // class bsl::Function_Rep
                        // -----------------------

template <class FUNC>
bsl::Function_Rep::PtrOrSize_t
bsl::Function_Rep::functionManager(ManagerOpCode  opCode,
                                   Function_Rep  *rep,
                                   PtrOrSize_t    input)
{
    static const std::size_t k_FUNC_SIZE = Function_ObjSize<FUNC>::VALUE;

    // If wrapped function fits in 'd_objbuf', then it is inplace; otherwise,
    // its heap-allocated address is found in 'd_objbuf.d_object_p'.  There
    // is no need to computed this using metaprogramming; the compiler will
    // optimize away the conditional test anyway.
    void *wrappedFunc_p = (k_FUNC_SIZE <= sizeof(InplaceBuffer) ?
                           &rep->d_objbuf : rep->d_objbuf.d_object_p);

    switch (opCode) {

      case e_MOVE_CONSTRUCT: {
          // Move-construct function object.  There is no point to optimizing
          // this operation for bitwise moveable types.  If the type is
          // trivially moveable, then the move operation below will do it
          // trivially.  Do not call constructor if functor is an empty class,
          // as it might try to do something silly like zeroing itself out.
          if (k_FUNC_SIZE) {
              FUNC &srcFunc =
                  *static_cast<FUNC*>(const_cast<void*>(input.asPtr()));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
              return ::new (wrappedFunc_p) FUNC(std::move(srcFunc));
#else
              return ::new (wrappedFunc_p) FUNC(srcFunc);
#endif
          }
      } break;

      case e_COPY_CONSTRUCT: {
          // Copy-construct function object.  There is no point to optimizing
          // this operation for bitwise copyable types.  If the type is
          // trivially copyiable, then the copy operation below will do it
          // trivially.  Do not call constructor if functor is an empty class,
          // as it might try to do something silly like zeroing itself out.
          if (k_FUNC_SIZE) {
              const FUNC &srcFunc = *static_cast<const FUNC*>(input.asPtr());
              return ::new (wrappedFunc_p) FUNC(srcFunc);
          }
      } break;

      case e_DESTROY: {
          // Call destructor for functor.  Do not call destructor if
          // functor is an empty class, as it might try to do something
          // silly like zeroing itself out.
          if (k_FUNC_SIZE) {
              reinterpret_cast<FUNC*>(wrappedFunc_p)->~FUNC();
          }

          // Return size of destroyed function object
          return k_FUNC_SIZE;
      } break;

      case e_GET_SIZE:    return k_FUNC_SIZE;
      case e_GET_TARGET:  return wrappedFunc_p;
      case e_GET_TYPE_ID: return &typeid(FUNC);

      case e_INIT_REP: {
          BSLS_ASSERT(0 && "Opcode not implemented for function manager");
      } break;

    } // end switch

    return PtrOrSize_t();
}

template <class ALLOC>
bsl::Function_Rep::PtrOrSize_t
bsl::Function_Rep::ownedAllocManager(ManagerOpCode  opCode,
                                     Function_Rep  *rep,
                                     PtrOrSize_t    input)
{
    typedef bslma::AllocatorAdaptor<ALLOC> Adaptor;

    switch (opCode) {
      case e_MOVE_CONSTRUCT: // Fall through: allocators are always copied
      case e_COPY_CONSTRUCT: {
        const Adaptor& other = *static_cast<const Adaptor*>(input.asPtr());
        return ::new ((void*) rep->d_allocator_p) Adaptor(other);
      } break;

      case e_DESTROY: {
        // Allocator cannot deallocate itself, so make a copy of the allocator
        // on the stack and use the copy for deallocation.
        Adaptor allocCopy(*static_cast<Adaptor*>(rep->d_allocator_p));
        std::size_t funcSize = input.asSize_t();

        rep->d_allocator_p->~Allocator();  // destroy allocator

        if (funcSize > sizeof(InplaceBuffer)) {
            // Deallocate memory holding function and allocator
            allocCopy.deallocate(rep->d_objbuf.d_object_p);
        }
        else if (funcSize + sizeof(Adaptor) > sizeof(InplaceBuffer)) {
            // Function is inplace but allocator is not.
            // Deallocate space used by allocator.
            allocCopy.deallocate(rep->d_allocator_p);
        }
        // else everything is inplace.  Nothing to deallocate.

        // Return size allocator adaptor.
        return sizeof(Adaptor);
      }

      case e_GET_SIZE:    return sizeof(Adaptor);
      case e_GET_TARGET:  return rep->d_allocator_p;
      case e_GET_TYPE_ID: return &typeid(Adaptor);

      case e_INIT_REP: {
          const bslma::Allocator *inputAlloc =
              static_cast<const bslma::Allocator *>(input.asPtr());
          const Adaptor *inputAdaptor =
              dynamic_cast<const Adaptor*>(inputAlloc);
          BSLS_ASSERT(inputAdaptor);

          std::size_t funcSize = rep->d_funcManager_p ?
              rep->d_funcManager_p(e_GET_SIZE, rep, PtrOrSize_t()).asSize_t() :
              0;

          rep->initRep(funcSize, inputAdaptor->adaptedAllocator(),
                  integral_constant<AllocCategory, e_ERASED_STATEFUL_ALLOC>());

      } break;
    } // end switch

    return PtrOrSize_t();
}

inline
void *bsl::Function_Rep::initRep(std::size_t funcSize, bslma::Allocator *alloc,
                           integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>)
{
    void *function_p;
    if (funcSize <= sizeof(InplaceBuffer)) {
        function_p = &d_objbuf;
    }
    else {
        function_p = alloc->allocate(funcSize);
        d_objbuf.d_object_p = function_p;
    }

    d_allocator_p = alloc;
    d_allocManager_p = &unownedAllocManager;

    return function_p;
}

template <class T>
inline
void *bsl::Function_Rep::initRep(std::size_t          funcSize,
                             const bsl::allocator<T>& alloc,
                             integral_constant<AllocCategory, e_BSL_ALLOCATOR>)
{
    return initRep(funcSize, alloc.mechanism(),
                   integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>());
}

template <class ALLOC>
inline
void *bsl::Function_Rep::initRep(std::size_t funcSize, const ALLOC& alloc,
                     integral_constant<AllocCategory, e_ERASED_STATEFUL_ALLOC>)
{
    typedef bslma::AllocatorAdaptor<ALLOC> Adaptor;

    static const std::size_t allocSize = sizeof(Adaptor);

    bool isInplaceFunc = funcSize               <= sizeof(InplaceBuffer);
    bool isInplacePair = (funcSize + allocSize) <= sizeof(InplaceBuffer);

    void *function_p;
    void *allocator_p;

    // Whether or not the function object is in-place is not dependent on
    // whether the allocator also fits in the object buffer.  This design
    // avoids the function manager having to handle the cross product of all
    // possible allocators and all possible function types.  If the function
    // object is in-place and the allocator happens to fit as well, then it
    // will be squeezed in as well, otherwise the allocator will be allocated
    // out-of-place.
    //
    // Although this is a run-time 'if' statement, the compiler will usually
    // optimize away the conditional when 'funcSize' is known at compile time
    // and this function is inlined. (Besides, it's cheap even if not
    // optimized away).
    if (isInplacePair) {
        // Both Function object and allocator fit in-place.
        Function_PairBufDesc pairDesc(funcSize, allocSize);
        function_p = pairDesc.first(&d_objbuf);
        allocator_p = pairDesc.second(&d_objbuf);
    }
    else if (isInplaceFunc) {
        // Function object fits in-place, but allocator is out-of-place
        function_p = &d_objbuf;
        // Allocate allocator adaptor from allocator itself
        allocator_p = Adaptor(alloc).allocate(allocSize);
    }
    else {
        // Not in-place.
        // Allocate function and allocator adaptor from the allocator.
        Function_PairBufDesc pairDesc(funcSize, allocSize);
        void *pair_p = Adaptor(alloc).allocate(pairDesc.totalSize());
        d_objbuf.d_object_p = pair_p;
        function_p = pairDesc.first(pair_p);
        allocator_p = pairDesc.second(pair_p);
    }

    // Construct allocator adaptor in its correct location
    d_allocator_p = ::new((void*) allocator_p) Adaptor(alloc);
    d_allocManager_p = &ownedAllocManager<ALLOC>;

    return function_p;
}

template <class ALLOC>
inline
void *bsl::Function_Rep::initRep(std::size_t funcSize, const ALLOC& alloc,
                    integral_constant<AllocCategory, e_ERASED_STATELESS_ALLOC>)
{
    // Since ALLOC is an empty type, we need only one instance of it.
    // This single instance is wrapped in an adaptor.
    static bslma::AllocatorAdaptor<ALLOC> allocInstance(alloc);

    return initRep(funcSize, &allocInstance,
                   integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>());
}

inline
bool bsl::Function_Rep::equalAlloc(bslma::Allocator* alloc,
                     integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>) const
{
    return alloc == d_allocator_p;
}

template <class T>
inline
bool bsl::Function_Rep::equalAlloc(const bsl::allocator<T>& alloc,
                       integral_constant<AllocCategory, e_BSL_ALLOCATOR>) const
{
    return alloc.mechanism() == d_allocator_p;
}

template <class ALLOC>
bool bsl::Function_Rep::equalAlloc(const ALLOC&,
              integral_constant<AllocCategory,e_ERASED_STATELESS_ALLOC>)  const
{
    BSLMF_ASSERT((is_same<typename ALLOC::value_type, char>::value));

    typedef bslma::AllocatorAdaptor<ALLOC> Adaptor;

    // If the our allocator has the same type as the adapted stateless
    // allocator, then they are assumed equal.
    return NULL != dynamic_cast<Adaptor*>(d_allocator_p);
}

template <class ALLOC>
bool bsl::Function_Rep::equalAlloc(const ALLOC& alloc,
               integral_constant<AllocCategory, e_ERASED_STATEFUL_ALLOC>) const
{
    BSLMF_ASSERT((is_same<typename ALLOC::value_type, char>::value));

    typedef bslma::AllocatorAdaptor<ALLOC> Adaptor;

    // Try to cast our allocator into the same type as the adapted 'ALLOC'.
    Adaptor *thisAdaptor = dynamic_cast<Adaptor*>(d_allocator_p);

    if (! thisAdaptor) {
        // Different type.  Cannot compare equal.
        return false;
    }

    // Compare the wrapped STL allocator to 'alloc'.
    return thisAdaptor->adaptedAllocator() == alloc;
}

template <class ALLOC, bsl::Function_Rep::AllocCategory ATP>
void bsl::Function_Rep::copyRep(const Function_Rep&                   other,
                                const ALLOC&                          alloc,
                                integral_constant<AllocCategory, ATP> atp)
{
    typedef bslma::AllocatorAdaptor<ALLOC> Adaptor;

    // Compute function size.
    std::size_t funcSize =
        other.d_funcManager_p(e_GET_SIZE, this, PtrOrSize_t()).asSize_t();

    initRep(funcSize, alloc, atp);

    void *otherFunction_p = (funcSize <= sizeof(InplaceBuffer) ?
                             &other.d_objbuf : other.d_objbuf.d_object_p);

    d_funcManager_p = other.d_funcManager_p;

    // Construct a copy of the function int its correct location
    d_funcManager_p(e_COPY_CONSTRUCT, this, otherFunction_p);
}

template<class T>
T* bsl::Function_Rep::target() BSLS_NOTHROW_SPEC
{
    if ((! d_funcManager_p) || target_type() != typeid(T)) {
        return NULL;
    }

    PtrOrSize_t target = d_funcManager_p(e_GET_TARGET, this, PtrOrSize_t());
    return static_cast<T*>(const_cast<void*>(target.asPtr()));
}

template<class T>
inline
const T* bsl::Function_Rep::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<Function_Rep*>(this)->target<T>();
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
// TBD: Correct forwarding
//     typedef bslmf::ForwardingTypeUtil<ARGS> FTUtil;
//     return static_cast<RET>(f(FTUtil::forwardToTarget(args)...));
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

    // Cast to 'RET' is needed to avoid compilation error if 'RET' is void and
    // 'f' returns non-void.
    return static_cast<RET>(f(args...));
}

template <class RET, class... ARGS>
template <class FUNC>
RET
bsl::function<RET(ARGS...)>::outofplaceFunctorInvoker(const Function_Rep *rep, 
                            typename bslmf::ForwardingType<ARGS>::Type... args)
{
    FUNC& f = *reinterpret_cast<FUNC*>(rep->d_objbuf.d_object_p);
    // Cast to 'RET' is needed to avoid compilation error if 'RET' is void and
    // 'f' returns non-void.
    return static_cast<RET>(f(args...));
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
template <class ALLOC>
void bsl::function<RET(ARGS...)>::copyInit(const ALLOC&    alloc,
                                           const function& other)
{
    d_funcManager_p = other.d_funcManager_p;
    d_invoker_p     = other.d_invoker_p;

    std::size_t funcSize = d_funcManager_p ?
        d_funcManager_p(e_GET_SIZE, this, PtrOrSize_t()).asSize_t() : 0;

    typedef Function_AllocTraits<ALLOC> Traits;
    initRep(funcSize,
            typename Traits::Type(alloc), typename Traits::Category());

    if (d_funcManager_p) {
        PtrOrSize_t source = d_funcManager_p(e_GET_TARGET,
                                             const_cast<function*>(&other),
                                             PtrOrSize_t());
        d_funcManager_p(e_COPY_CONSTRUCT, this, source);
    }
}

template <class RET, class... ARGS>
void bsl::function<RET(ARGS...)>::moveInit(function& other)
{
    // This function is called only when it is known that '*this' will get
    // its allocator from 'other'.

    d_funcManager_p = other.d_funcManager_p;
    d_invoker_p     = other.d_invoker_p;

    if (d_funcManager_p) {
        std::size_t funcSize = d_funcManager_p(e_GET_SIZE, &other,
                                               PtrOrSize_t()).asSize_t();

        if (funcSize <= sizeof(InplaceBuffer)) {
            // Function is inplace.

            // Initialize the rep using other's allocator.
            other.d_allocManager_p(e_INIT_REP, this, other.d_allocator_p);

            // Move-construct function
            PtrOrSize_t source =
                d_funcManager_p(e_GET_TARGET,
                                const_cast<function*>(&other),
                                PtrOrSize_t());
            d_funcManager_p(e_MOVE_CONSTRUCT, this, source);
        }
        else {
            // Function is not inplace.
            // Just move the pointers from other.
            d_objbuf.d_object_p = other.d_objbuf.d_object_p;
            d_allocManager_p    = other.d_allocManager_p;
            d_allocator_p       = other.d_allocator_p;

            // Now re-initialize 'other' as an empty object
            other.d_funcManager_p = NULL;
            other.d_invoker_p     = NULL;
            d_allocManager_p(e_INIT_REP, &other, d_allocator_p);
        }
    }
    else {
        // Moving an empty 'function' object.
        // Initialize just the allocator portion of the result
        other.d_allocManager_p(e_INIT_REP, this, other.d_allocator_p);
    }
}

// CREATORS
template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = &unownedAllocManager;
    d_invoker_p       = NULL;
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = &unownedAllocManager;
    d_invoker_p       = NULL;
}

template <class RET, class... ARGS>
inline
bsl::function<RET(ARGS...)>::function(const function& other)
{
    copyInit(bslma::Default::defaultAllocator(), other);
}

template <class RET, class... ARGS>
template<class FUNC>
bsl::function<RET(ARGS...)>::function(FUNC func)
{
    typedef typename bslmf::SelectTrait<FUNC,
                                        bslmf::IsFunctionPointer,
                                        bslmf::IsMemberFunctionPointer,
                                        FitsInplace>::Type FuncSelection;

    d_invoker_p = getInvoker(func, FuncSelection());

    std::size_t funcSize = d_invoker_p ? Function_ObjSize<FUNC>::VALUE : 0;

    initRep(funcSize, bslma::Default::defaultAllocator(),
            integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>());

    if (d_invoker_p) {
        d_funcManager_p = &functionManager<FUNC>;
        d_funcManager_p(e_MOVE_CONSTRUCT, this, &func);
    }
    else {
        // Empty 'func' object
        d_funcManager_p = NULL;
    }
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC& alloc)
{
    d_funcManager_p = NULL;
    d_invoker_p     = NULL;

    typedef Function_AllocTraits<ALLOC> Traits;
    initRep(0, typename Traits::Type(alloc), typename Traits::Category());
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    d_funcManager_p = NULL;
    d_invoker_p     = NULL;

    typedef Function_AllocTraits<ALLOC> Traits;
    initRep(0, typename Traits::Type(alloc), typename Traits::Category());
}

template <class RET, class... ARGS>
template<class ALLOC>
inline
bsl::function<RET(ARGS...)>::function(allocator_arg_t,
                                      const ALLOC&    alloc,
                                      const function& other)
{
    copyInit(alloc, other);
}

template <class RET, class... ARGS>
template<class FUNC, class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t,
                                      const ALLOC& alloc,
                                      FUNC         func)
{
    typedef Function_AllocTraits<ALLOC> AllocTraits;
    typedef typename bslmf::SelectTrait<FUNC,
                                        bslmf::IsFunctionPointer,
                                        bslmf::IsMemberFunctionPointer,
                                        FitsInplace>::Type FuncSelection;

    d_invoker_p = getInvoker(func, FuncSelection());

    std::size_t funcSize = d_invoker_p ? Function_ObjSize<FUNC>::VALUE : 0;
    initRep(funcSize, typename AllocTraits::Type(alloc),
            typename AllocTraits::Category());

    if (d_invoker_p) {
        d_funcManager_p = &functionManager<FUNC>;
        d_funcManager_p(e_MOVE_CONSTRUCT, this, &func);
    }
    else {
        // Empty 'func' object
        d_funcManager_p = NULL;
    }
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function(function&& other)
{
    moveInit(other);
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t,
                                      const ALLOC& alloc,
                                      function&&   other)
{
    typedef Function_AllocTraits<ALLOC> AllocTraits;

    if (other.equalAlloc(alloc, typename AllocTraits::Category())) {
        moveInit(other);
    }
    else
        copyInit(typename AllocTraits::Type(alloc), other);
}

#endif //  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::~function()
{
    // Assert class invariants
    BSLS_ASSERT(d_allocator_p);
    BSLS_ASSERT(d_allocManager_p);
    BSLS_ASSERT(d_invoker_p || ! d_funcManager_p);

    // Integral function size cast to pointer type.
    PtrOrSize_t funcSize;

    if (d_funcManager_p) {
        // Destroy returns the size of the object that was destroyed.
        funcSize = d_funcManager_p(e_DESTROY, this, PtrOrSize_t());
    }

    d_allocManager_p(e_DESTROY, this, funcSize);
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
RET bsl::function<RET(ARGS...)>::operator()(ARGS... args) const
{
    if (d_invoker_p) {
        return d_invoker_p(this, args...);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

// ACCESSORS

template <class RET, class... ARGS>
inline
bsl::function<RET(ARGS...)>::operator bool() const BSLS_NOTHROW_SPEC
{
    // If there is an invoker, then this function is non-empty (return true);
    // otherwise it is empty (return false).
    return d_invoker_p;
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
