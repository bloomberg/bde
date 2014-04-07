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

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
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
        // Constructs this exception object.

    const char* what() const BSLS_NOTHROW_SPEC;
        // Returns "bad_function_call".
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

      , e_DESTRUCTIVE_MOVE
            // move-construct the object in 'rep' from the object pointed to
            // by 'input' and destroy the object at 'input'.  Return nothing.
            // This operation is guaranteed not to throw.  Uses bitwise move
            // where possible.

      , e_GET_SIZE
            // Return the size of the object.  For function objects, the
            // computed size is modified by the SOO convention described in
            // the 'SooFuncSize' metafunction.  ('rep' and 'input' are
            // ignored.)

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

    static const std::size_t k_NON_SOO_SMALL_SIZE = ~sizeof(InplaceBuffer);
        // This value is added to the size of a small stateful functor to
        // indicate that, despite being small, it should not be allocated
        // inplace using the small object optimization (SOO), e.g., because it
        // does not have a nothrow move constructor and cannot, therefore, be
        // swapped safely.  When a size larger than this constant is seen, the
        // actual object size can be determined by subtracting this constant.
        // A useful quality of this encoding is that if
        // 'SZ <= sizeof(InplaceBuffer)' for some object size 'SZ', then
        // 'SZ + k_NON_SOO_SMALL_SIZE > sizeof(InplaceBuffer)', so th

    BSLMF_ASSERT(k_NON_SOO_SMALL_SIZE > 0);  // Assert unsigned size_t

    template <class TP>
    struct SooFuncSize
    {
        // Metafunction to determine the size of an object for the purposes of
        // the small object optimization (SOO).  The 'VALUE' member is encoded
        // as follows:
        //
        //:  o If 'TP' is not larger than 'InplaceBuffer' but should
        //:    not be allocated inplace for other reasons, then
        //:    'VALUE == sizeof(TP) + k_NON_SOO_SMALL_SIZE'.
        //:  o Otherwise, if 'TP' is an empty class, 'VALUE == 0'.
        //:  o Otherwise, 'VALUE == sizeof(TP)'.
        //
        // Note that the 'Soo' prefix is used to indicate that an identifier
        // uses the above protocol.  Thus a variable called 'SooSize' is
        // assumed to be encoded as above, whereas a variable called 'size'
        // can generally be assumed not to be encoded that way.

    private:
        // The actual memory footprint used of the object, which is zero for
        // empty classes.
        static const std::size_t FOOTPRINT = (bsl::is_empty<TP>::value ? 0 :
                                              sizeof(TP));

    public:
        static const std::size_t VALUE =
            (FOOTPRINT > sizeof(InplaceBuffer)                 ? FOOTPRINT :
             bslmf::IsBitwiseMoveable<TP>::value               ? FOOTPRINT :
#ifdef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
             // Check if nothrow move constructible.  The use of '::new' lets
             // us check the constructor without also checking the destructor.
             // This is especially important in gcc 4.7 and before because
             // destructors are not implicitly noexcept in those compilers.
             noexcept(::new((void*) 0) TP(std::declval<TP>())) ? FOOTPRINT :
#endif
             // If not nonthrow or bitwise moveable, then use the real size
             // (as computed by sizeof), not the adjusted size in 'FOOTPRINT'
             sizeof(TP) + k_NON_SOO_SMALL_SIZE);
    };

    template <class FN>
    struct InplaceFunc :
        integral_constant<bool,SooFuncSize<FN>::VALUE <= sizeof(InplaceBuffer)>
    {
        // Metafunction to determine whether the specified 'FN' template
        // parameter should be allocated within the 'InplaceBuffer'.

        // TBD: 'InplaceFunc' should also take alignment into account, but
        // since we don't (yet) have the ability to specify alignment when
        // allocating memory, there's nothing we can do at this point.
    };

    template <class FUNC>
    friend class bsl::function;

    template <class ALLOC>
    friend struct Function_AllocTraits;

    bool moveInit(Function_Rep& other);
        // Move-initialize this rep from the specified 'other' rep.  Returns
        // 'true' if the move took was inplace (i.e., function was
        // move-constructed from one 'd_objbuf' to another) and false
        // otherwise.

    void *initRep(std::size_t sooFuncSize, bslma::Allocator* alloc,
                  integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>);
    template <class TP>
    void *initRep(std::size_t sooFuncSize, const bsl::allocator<TP>& alloc,
                  integral_constant<AllocCategory, e_BSL_ALLOCATOR>);
    template <class ALLOC>
    void *initRep(std::size_t sooFuncSize, const ALLOC& alloc,
                  integral_constant<AllocCategory, e_ERASED_STATEFUL_ALLOC>);
    template <class ALLOC>
    void *initRep(std::size_t sooFuncSize, const ALLOC& alloc,
                  integral_constant<AllocCategory, e_ERASED_STATELESS_ALLOC>);
        // Initialize this object's 'd_objbuf', 'd_allocator_p', and
        // 'd_allocManager_p' fields, allocating (if necessary) enough storage
        // to hold a function object of the specified 'sooFuncSize' and
        // holding a copy of 'alloc'.  If the function and allocator fit
        // within 'd_objbuf', then no memory is allocated.  The actual wrapped
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

    static void destructiveMove(Function_Rep *to,
                                Function_Rep *from) BSLS_NOTHROW_SPEC;
        // Move the state from the specified 'from' location to the specified
        // 'to' location, where 'to' points to uninitialized storage.  After
        // the move, 'from' points to uninitialized storage.  The move
        // is performed using only non-throwing operations.

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
    // CREATORS
    ~Function_Rep();

    // MANIPULATORS
    void swap(Function_Rep& other) BSLS_NOTHROW_SPEC;

    // ACCESSORS
    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class TP> TP* target() BSLS_NOTHROW_SPEC;
    template<class TP> const TP* target() const BSLS_NOTHROW_SPEC;
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
                               bslmf::SelectTraitCase<InplaceFunc>);
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

    void swap(function& other) BSLS_NOTHROW_SPEC;
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
void swap(function<RET(ARGS...)>& a, function<RET(ARGS...)>& b);

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

                        // --------------------------------
                        // class Function_PairBufDesc
                        // --------------------------------

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

template <class TP>
const std::size_t bsl::Function_Rep::SooFuncSize<TP>::FOOTPRINT;

template <class TP>
const std::size_t bsl::Function_Rep::SooFuncSize<TP>::VALUE;

template <class FUNC>
bsl::Function_Rep::PtrOrSize_t
bsl::Function_Rep::functionManager(ManagerOpCode  opCode,
                                   Function_Rep  *rep,
                                   PtrOrSize_t    input)
{
    static const std::size_t k_SOO_FUNC_SIZE = SooFuncSize<FUNC>::VALUE;
    static const std::size_t k_IS_INPLACE =
        k_SOO_FUNC_SIZE <= sizeof(InplaceBuffer);

    // If functor is empty, empty it should have a one-byte footprint.
    BSLMF_ASSERT(0 != k_SOO_FUNC_SIZE || 1 == sizeof(FUNC));

    // If wrapped function fits in 'd_objbuf', then it is inplace; otherwise,
    // its heap-allocated address is found in 'd_objbuf.d_object_p'.  There
    // is no need to computed this using metaprogramming; the compiler will
    // optimize away the conditional test anyway.
    void *wrappedFunc_p = (k_IS_INPLACE ?
                           &rep->d_objbuf : rep->d_objbuf.d_object_p);

    char savedFuncByte;
    if (0 == k_SOO_FUNC_SIZE) {
        // We are allocating zero bytes for an empty functor.  Save the memory
        // contents of the (one-byte) functor footprint in case an operation
        // does something silly like zero the footprint or memcpy into it.
        savedFuncByte = static_cast<char*>(wrappedFunc_p)[0];
    }

    switch (opCode) {

      case e_MOVE_CONSTRUCT: {
          // Move-construct function object.  There is no point to optimizing
          // this operation for bitwise moveable types.  If the type is
          // trivially moveable, then the move operation below will do it
          // trivially.  

          FUNC &srcFunc =
              *static_cast<FUNC*>(const_cast<void*>(input.asPtr()));

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
          ::new (wrappedFunc_p) FUNC(std::move(srcFunc));
#else
          ::new (wrappedFunc_p) FUNC(srcFunc);
#endif
          if (0 == k_SOO_FUNC_SIZE) {
              // Restore the footprint of an empty functor in case the
              // constructor did something silly like zero it or memcpy into
              // it.
              static_cast<char*>(wrappedFunc_p)[0] = savedFuncByte;
          }
          return wrappedFunc_p;
      } break;

      case e_COPY_CONSTRUCT: {

          // Copy-construct function object.  There is no point to optimizing
          // this operation for bitwise copyable types.  If the type is
          // trivially copyiable, then the copy operation below will do it
          // trivially.
          const FUNC &srcFunc = *static_cast<const FUNC*>(input.asPtr());
          ::new (wrappedFunc_p) FUNC(srcFunc);
          if (0 == k_SOO_FUNC_SIZE) {
              // Restore the footprint of an empty functor in case the
              // constructor did something silly like zero it or memcpy into
              // it.
              static_cast<char*>(wrappedFunc_p)[0] = savedFuncByte;
          }
          return wrappedFunc_p;
      } break;

      case e_DESTROY: {

          // Call destructor for functor.
          static_cast<FUNC*>(wrappedFunc_p)->~FUNC();

          if (0 == k_SOO_FUNC_SIZE) {
              // Restore the footprint of an empty functor in case the
              // destructor did something silly like zero it or memcpy into
              // it.
              static_cast<char*>(wrappedFunc_p)[0] = savedFuncByte;
          }

          // Return size of destroyed function object
          return k_SOO_FUNC_SIZE;
      } break;

      case e_DESTRUCTIVE_MOVE: {
        void *input_p = const_cast<void*>(input.asPtr());
        char savedSrcByte = static_cast<char*>(input_p)[0];
        if (bslmf::IsBitwiseMoveable<FUNC>::value) {
            *static_cast<bsls::ObjectBuffer<FUNC>*>(wrappedFunc_p) =
                *static_cast<bsls::ObjectBuffer<FUNC>*>(input_p);
        }
        else {
            FUNC &srcFunc = *static_cast<FUNC*>(input_p);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            ::new (wrappedFunc_p) FUNC(std::move(srcFunc));
#else
            ::new (wrappedFunc_p) FUNC(srcFunc);
#endif
            srcFunc.~FUNC();
        }

        if (0 == k_SOO_FUNC_SIZE) {
            // Restore the footprint of an empty functor in case the
            // constructor or destructor did something silly like zero it or
            // memcpy into it.
            static_cast<char*>(wrappedFunc_p)[0] = savedFuncByte;
            static_cast<char*>(input_p)[0]       = savedSrcByte;
        }
      } break;

      case e_GET_SIZE:     return k_SOO_FUNC_SIZE;
      case e_GET_TARGET:   return wrappedFunc_p;
      case e_GET_TYPE_ID:  return &typeid(FUNC);

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
        std::size_t sooFuncSize = input.asSize_t();

        rep->d_allocator_p->~Allocator();  // destroy allocator

        if (sooFuncSize > sizeof(InplaceBuffer)) {
            // Deallocate memory holding function and allocator
            allocCopy.deallocate(rep->d_objbuf.d_object_p);
        }
        else if (sooFuncSize + sizeof(Adaptor) > sizeof(InplaceBuffer)) {
            // Function is inplace but allocator is not.
            // Deallocate space used by allocator.
            allocCopy.deallocate(rep->d_allocator_p);
        }
        // else everything is inplace.  Nothing to deallocate.

        // Return size allocator adaptor.
        return sizeof(Adaptor);
      }

      case e_DESTRUCTIVE_MOVE: {
        const Adaptor& other = *static_cast<const Adaptor*>(input.asPtr());
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        ::new ((void*) rep->d_allocator_p) Adaptor(std::move(other));
#else
        ::new ((void*) rep->d_allocator_p) Adaptor(other);
#endif
        other.~Adaptor();  
      } break;

      case e_GET_SIZE: {
        // The SOO size of the adaptor is always the same as the true size of
        // the adaptor because it is always nothrow moveable.
        return sizeof(Adaptor);
      }

      case e_GET_TARGET:  return rep->d_allocator_p;
      case e_GET_TYPE_ID: return &typeid(Adaptor);

      case e_INIT_REP: {
        const bslma::Allocator *inputAlloc =
            static_cast<const bslma::Allocator *>(input.asPtr());
        const Adaptor *inputAdaptor =
            dynamic_cast<const Adaptor*>(inputAlloc);
        BSLS_ASSERT(inputAdaptor);

        std::size_t sooFuncSize = rep->d_funcManager_p ?
            rep->d_funcManager_p(e_GET_SIZE, rep,
                                 PtrOrSize_t()).asSize_t() : 0;

        rep->initRep(sooFuncSize, inputAdaptor->adaptedAllocator(),
                  integral_constant<AllocCategory, e_ERASED_STATEFUL_ALLOC>());

      } break;
    } // end switch

    return PtrOrSize_t();
}

inline
void *bsl::Function_Rep::initRep(std::size_t       sooFuncSize,
                                 bslma::Allocator *alloc,
                                 integral_constant<AllocCategory,
                                                   e_BSLMA_ALLOC_PTR>)
{
    void *function_p;
    if (sooFuncSize <= sizeof(InplaceBuffer)) {
        function_p = &d_objbuf;
    }
    else {
        std::size_t funcSize = (sooFuncSize >= k_NON_SOO_SMALL_SIZE ?
                                sooFuncSize - k_NON_SOO_SMALL_SIZE :
                                sooFuncSize);
        function_p = alloc->allocate(funcSize);
        d_objbuf.d_object_p = function_p;
    }

    d_allocator_p = alloc;
    d_allocManager_p = &unownedAllocManager;

    return function_p;
}

template <class TP>
inline
void *bsl::Function_Rep::initRep(std::size_t               sooFuncSize,
                                 const bsl::allocator<TP>& alloc,
                                 integral_constant<AllocCategory,
                                                   e_BSL_ALLOCATOR>)
{
    return initRep(sooFuncSize, alloc.mechanism(),
                   integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>());
}

template <class ALLOC>
inline
void *bsl::Function_Rep::initRep(std::size_t  sooFuncSize,
                                 const ALLOC& alloc,
                                 integral_constant<AllocCategory,
                                                   e_ERASED_STATEFUL_ALLOC>)
{
    typedef bslma::AllocatorAdaptor<ALLOC> Adaptor;

    static const std::size_t allocSize = sizeof(Adaptor);

    std::size_t funcSize = (sooFuncSize >= k_NON_SOO_SMALL_SIZE ?
                            sooFuncSize - k_NON_SOO_SMALL_SIZE :
                            sooFuncSize);

    bool isInplaceFunc = sooFuncSize <= sizeof(InplaceBuffer);
    bool isInplacePair = (isInplaceFunc &&
                          (funcSize + allocSize) <= sizeof(InplaceBuffer));

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
    // optimize away the conditional when 'sooFuncSize' is known at compile time
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
void *bsl::Function_Rep::initRep(std::size_t sooFuncSize,
                                 const ALLOC& alloc,
                                 integral_constant<AllocCategory,
                                                   e_ERASED_STATELESS_ALLOC>)
{
    // Since ALLOC is an empty type, we need only one instance of it.
    // This single instance is wrapped in an adaptor.
    static bslma::AllocatorAdaptor<ALLOC> allocInstance(alloc);

    return initRep(sooFuncSize, &allocInstance,
                   integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>());
}

inline
bool bsl::Function_Rep::equalAlloc(bslma::Allocator* alloc,
                     integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>) const
{
    return alloc == d_allocator_p;
}

template <class TP>
inline
bool bsl::Function_Rep::equalAlloc(const bsl::allocator<TP>& alloc,
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
    std::size_t sooFuncSize =
        other.d_funcManager_p(e_GET_SIZE, this, PtrOrSize_t()).asSize_t();

    initRep(sooFuncSize, alloc, atp);

    void *otherFunction_p = (sooFuncSize <= sizeof(InplaceBuffer) ?
                             &other.d_objbuf : other.d_objbuf.d_object_p);

    d_funcManager_p = other.d_funcManager_p;

    // Construct a copy of the function int its correct location
    d_funcManager_p(e_COPY_CONSTRUCT, this, otherFunction_p);
}

template<class TP>
TP* bsl::Function_Rep::target() BSLS_NOTHROW_SPEC
{
    if ((! d_funcManager_p) || target_type() != typeid(TP)) {
        return NULL;
    }

    PtrOrSize_t target = d_funcManager_p(e_GET_TARGET, this, PtrOrSize_t());
    return static_cast<TP*>(const_cast<void*>(target.asPtr()));
}

template<class TP>
inline
const TP* bsl::Function_Rep::target() const BSLS_NOTHROW_SPEC
{
    return const_cast<Function_Rep*>(this)->target<TP>();
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
                                        bslmf::SelectTraitCase<InplaceFunc>)
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

    std::size_t sooFuncSize = d_funcManager_p ?
        d_funcManager_p(e_GET_SIZE, this, PtrOrSize_t()).asSize_t() : 0;

    typedef Function_AllocTraits<ALLOC> Traits;
    initRep(sooFuncSize,
            typename Traits::Type(alloc), typename Traits::Category());

    if (d_funcManager_p) {
        PtrOrSize_t source = d_funcManager_p(e_GET_TARGET,
                                             const_cast<function*>(&other),
                                             PtrOrSize_t());
        d_funcManager_p(e_COPY_CONSTRUCT, this, source);
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
                                        InplaceFunc>::Type FuncSelection;

    d_invoker_p = getInvoker(func, FuncSelection());

    std::size_t sooFuncSize = d_invoker_p ? SooFuncSize<FUNC>::VALUE : 0;

    initRep(sooFuncSize, bslma::Default::defaultAllocator(),
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
                                        InplaceFunc>::Type FuncSelection;

    d_invoker_p = getInvoker(func, FuncSelection());

    std::size_t sooFuncSize = d_invoker_p ? SooFuncSize<FUNC>::VALUE : 0;
    initRep(sooFuncSize, typename AllocTraits::Type(alloc),
            typename AllocTraits::Category());

    if (d_invoker_p) {
        d_funcManager_p = &functionManager<FUNC>;
        d_funcManager_p(e_MOVE_CONSTRUCT, this, &func);
    }
   else {
        // Empty 'function' object
        d_funcManager_p = NULL;
    }
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function(function&& other)
{
    d_invoker_p = other.d_invoker_p;
    if (! moveInit(other)) {
        other.d_invoker_p = NULL;
    }
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t,
                                      const ALLOC& alloc,
                                      function&&   other)
{
    typedef Function_AllocTraits<ALLOC> AllocTraits;

    if (other.equalAlloc(alloc, typename AllocTraits::Category())) {
        d_invoker_p = other.d_invoker_p;
        if (! moveInit(other)) {
            other.d_invoker_p = NULL;
        }
    }
    else
        copyInit(typename AllocTraits::Type(alloc), other);
}

#endif //  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
inline
bsl::function<RET(ARGS...)>::~function()
{
    // Assert class invariants
    BSLS_ASSERT(d_invoker_p || ! d_funcManager_p);
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
void bsl::function<RET(ARGS...)>::swap(function& other) BSLS_NOTHROW_SPEC
{
    Invoker *thisInvoker  = this->d_invoker_p;
    Invoker *otherInvoker = other.d_invoker_p;

    Function_Rep::swap(other);
    this->d_invoker_p = otherInvoker;
    other.d_invoker_p = thisInvoker;
}

template <class RET, class... ARGS>
template<class FUNC, class ALLOC>
void bsl::function<RET(ARGS...)>::assign(FUNC&&, const ALLOC&)
{
    // TBD
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
bool bsl::operator==(const bsl::function<RET(ARGS...)>&,
                     bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool bsl::operator==(bsl::nullptr_t,
                     const bsl::function<RET(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool bsl::operator!=(const bsl::function<RET(ARGS...)>&,
                     bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
bool bsl::operator!=(bsl::nullptr_t,
                     const bsl::function<RET(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class RET, class... ARGS>
inline
void bsl::swap(bsl::function<RET(ARGS...)>& a, bsl::function<RET(ARGS...)>& b)
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
