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
#error "include <bsl_function.h> instead of <bslmf_function.h> in \
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

#ifndef INCLUDED_BSLMF_ISEMPTY
#include <bslmf_isempty.h>
#endif

#ifndef INCLUDED_BSLMF_SELECTTRAIT
#include <bslmf_selecttrait.h>
#endif

#ifndef INCLUDED_UTILITY
#include <utility>
#define INCLUDED_UTILITY
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
      , CONSTRUCT
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

    typedef void *(*Manager)(ManagerOpCode  opCode,
                             Function_Rep  *rep,
                             const void    *source);
        // 'Manager' is an alias for a pointer to a function that manages a
        // specific object type (i.e., it copies, moves, or destroyes it).  It
        // implements a kind of hand-coded virtual-function dispatch.  The
        // specified 'opCode' is used to choose the "virtual function" to
        // invoke, where the specified 'rep' and 'source' are arguments to
        // that function.  Internally, a 'Manager' function uses a 'switch'
        // statement rather than preforming a virtual-table lookup.  This
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

        void                *d_object_p;                // pointer to external
                                                        // representation

        void               (*d_func_p)();               // pointer to function

        void (Function_Rep::*d_memfnPtr_p)();           // pointer to member
                                                        // function

        MaxAlignedType       d_align;                   // force align
        char                 d_minbuf[4*sizeof(void*)]; // force minimum size
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
    static Manager getFuncManager(true_type /* inplace */, FUNC*);
    template <class FUNC>
    static Manager getFuncManager(false_type /* inplace */, FUNC*);

    template <class FUNC>
    void *inplaceFuncManager(ManagerOpCode  opCode,
                             Function_Rep  *rep,
                             const void    *source);

    template <class FUNC>
    void *outofplaceFuncManager(ManagerOpCode  opCode,
                                Function_Rep  *rep,
                                const void    *source);

    template <class FUNC>
    void *pairedAllocManager(ManagerOpCode  opCode,
                             Function_Rep  *rep,
                             const void    *source);

    template <class FUNC>
    void *separateAllocManager(ManagerOpCode  opCode,
                               Function_Rep  *rep,
                               const void    *source);

#if defined(BSLS_PLATFORM_CMP_MSVC)
    __declspec(noreturn)
#endif

  private:
    // DATA
    InplaceBuffer     d_objbuf;      // in-place representation (if fits, as
                                     // indicated by the manager), or
                                     // pointer to external representation

    Manager           d_funcManager_p;
                                     // pointer to manager function used to
                                     // operate on function object instance
                                     // (which knows about the erased type
                                     // 'FUNC' of the function object), or null
                                     // for raw function pointers.

    bslma::Allocator *d_allocator_p; // allocator (held, not owned)

    Manager           d_allocManager_p;
                                     // pointer to manager function used to
                                     // operate on allocator instance (which
                                     // knows about the erased type 'ALLOC' of
                                     // the allocator object), or null for if
                                     // 'ALLOC' is a 'bslma::Allocator*'
};


                    // =======================
                    // class template function
                    // =======================

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

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

    typedef RET (*Invoker)(Function_Rep* rep, ARGS...);
    typedef Function_Rep::FuncType FuncType;

    Invoker d_invoker_p;

    template <class FUNC>
    static Invoker getInvoker(FUNC*,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>);
        // Return the invoker for a pointer to (non-member) function.

    template <class FUNC>
    static Invoker getInvoker(FUNC*,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>);
        // Return the invoker for a pointer to member function.

    template <class FUNC>
    static Invoker getInvoker(FUNC*, bslmf::SelectTraitCase<FitsInplace>);
        // Return the invoker for an in-place functor.

    template <class FUNC>
    static Invoker getInvoker(FUNC*, bslmf::SelectTraitCase<>);
        // Return the invoker for an out-of-place functor.

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

    explicit operator bool() const BSLS_NOTHROW_SPEC;

    RET operator()(ARGS...) const;

    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class T> T* target() BSLS_NOTHROW_SPEC;
    template<class T> const T* target() const BSLS_NOTHROW_SPEC;
};

// FREE FUNCTIONS
template <class R, class... ARGS>
bool operator==(const function<R(ARGS...)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator==(nullptr_t, const function<R(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator!=(const function<R(ARGS...)>&, nullptr_t) BSLS_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator!=(nullptr_t, const function<R(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class R, class... ARGS>
void swap(function<R(ARGS...)>&, function<R(ARGS...)>&);

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
    }

    // Construct function object in designated location
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    ::new(function_p) FUNC(std::move(func));
#else
    ::new(function_p) FUNC(func);
#endif

    d_funcManager_p = getFuncManager(IsInplaceFunc(), &func);

    d_allocator_p = alloc;
    d_allocManager_p = nullptr;
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

    d_funcManager_p = getFuncManager(IsInplaceFunc(), &func);

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

                        // ----------------------------
                        // class template bsl::function
                        // ----------------------------

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

// CREATORS
template <class R, class... ARGS>
bsl::function<R(ARGS...)>::function() BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = nullptr;
    d_funcManager_p   = nullptr;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = nullptr;
    d_invoker_p       = nullptr;
}

template <class R, class... ARGS>
bsl::function<R(ARGS...)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_objbuf.d_func_p = nullptr;
    d_funcManager_p   = nullptr;
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = nullptr;
    d_invoker_p       = nullptr;
}

template <class R, class... ARGS>
bsl::function<R(ARGS...)>::function(const function&)
{
    // TBD
}

template <class R, class... ARGS>
template<class FUNC>
bsl::function<R(ARGS...)>::function(FUNC func)
{
    initRep(func, bslma::Default::defaultAllocator(),
            integral_constant<AllocType, BSLMA_ALLOC_PTR>());

    typedef typename bslmf::SelectTrait<FUNC,
                                       bslmf::IsFunctionPointer,
                                       bslmf::IsMemberFunctionPointer,
                                       FitsInplace>::type TraitSelection;

    d_invoker_p = getInvoker(&func, TraitSelection());
}

template <class R, class... ARGS>
template<class ALLOC>
bsl::function<R(ARGS...)>::function(allocator_arg_t, const ALLOC& alloc)
{
    R (*nullfunc_p)(ARGS...) = nullptr;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = nullptr;
}

template <class R, class... ARGS>
template<class ALLOC>
bsl::function<R(ARGS...)>::function(allocator_arg_t, const ALLOC& alloc,
                                    nullptr_t)
{
    R (*nullfunc_p)(ARGS...) = nullptr;
    initRep(nullfunc_p, alloc, typename bsl::is_pointer<ALLOC>::type());

    d_invoker_p = nullptr;
}

template <class R, class... ARGS>
template<class ALLOC>
bsl::function<R(ARGS...)>::function(allocator_arg_t, const ALLOC&,
                                    const function&)
{
}

template <class R, class... ARGS>
template<class FUNC, class ALLOC>
bsl::function<R(ARGS...)>::function(allocator_arg_t, const ALLOC&, FUNC)
{
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class R, class... ARGS>
bsl::function<R(ARGS...)>::function(function&&)
{
}

template <class R, class... ARGS>
template<class ALLOC>
bsl::function<R(ARGS...)>::function(allocator_arg_t, const ALLOC&, function&&)
{
}

#endif //  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class R, class... ARGS>
bsl::function<R(ARGS...)>::~function()
{
}

template <class R, class... ARGS>
template <class FUNC>
typename bsl::function<R(ARGS...)>::Invoker
bsl::function<R(ARGS...)>::getInvoker(FUNC*,
                              bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
{
    // TBD
    return nullptr;
}

template <class R, class... ARGS>
template <class FUNC>
typename bsl::function<R(ARGS...)>::Invoker
bsl::function<R(ARGS...)>::getInvoker(FUNC*,
                        bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
{
    // TBD
    return nullptr;
}

template <class R, class... ARGS>
template <class FUNC>
typename bsl::function<R(ARGS...)>::Invoker
bsl::function<R(ARGS...)>::getInvoker(FUNC*,
                                      bslmf::SelectTraitCase<FitsInplace>)
{
    // TBD
    return nullptr;
}

template <class R, class... ARGS>
template <class FUNC>
typename bsl::function<R(ARGS...)>::Invoker
bsl::function<R(ARGS...)>::getInvoker(FUNC*, bslmf::SelectTraitCase<>)
{
    // TBD
    return nullptr;
}

// MANIPULATORS
template <class R, class... ARGS>
bsl::function<R(ARGS...)>&
bsl::function<R(ARGS...)>::operator=(const function&)
{
    // TBD
    return *this;
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class R, class... ARGS>
bsl::function<R(ARGS...)>&
bsl::function<R(ARGS...)>::operator=(function&&)
{
    // TBD
    return *this;
}

#endif

template <class R, class... ARGS>
bsl::function<R(ARGS...)>&
bsl::function<R(ARGS...)>::operator=(nullptr_t)
{
    // TBD
    return *this;
}

template <class R, class... ARGS>
template<class FUNC>
bsl::function<R(ARGS...)>&
bsl::function<R(ARGS...)>::operator=(FUNC&&)
{
    // TBD
    return *this;
}

// TBD: Need to implement reference_wrapper.
// template <class R, class... ARGS>
// template<class FUNC>
// function& bsl::function<R(ARGS...)>::operator=(reference_wrapper<FUNC>)
//     BSLS_NOTHROW_SPEC

template <class R, class... ARGS>
void bsl::function<R(ARGS...)>::swap(function&) BSLS_NOTHROW_SPEC
{
}

template <class R, class... ARGS>
template<class FUNC, class ALLOC>
void bsl::function<R(ARGS...)>::assign(FUNC&&, const ALLOC&)
{
}

template <class R, class... ARGS>
bsl::function<R(ARGS...)>::operator bool() const BSLS_NOTHROW_SPEC
{
    // TBD
    return (! d_invoker_p) && (! d_objbuf.d_func_p);
}

template <class R, class... ARGS>
R bsl::function<R(ARGS...)>::operator()(ARGS... args) const
{
    if (d_invoker_p) {
        return d_invoker(this, args...);
    }
    else if (d_objbuf.d_func_p) {
        return d_objbuf.d_func_p(args...);
    }
    else {
        BSLS_THROW(bad_function_call());
    }
}

template <class R, class... ARGS>
const std::type_info&
bsl::function<R(ARGS...)>::target_type() const BSLS_NOTHROW_SPEC
{
    const std::type_info *info_p =
        static_cast<const std::type_info*>(d_manager(GET_TYPE_ID, this,
                                                     nullptr));
    return *info_p;
}

template <class R, class... ARGS>
template<class T>
T* bsl::function<R(ARGS...)>::target() BSLS_NOTHROW_SPEC
{
    const function *constThis = this;
    return const_cast<T*>(constThis->target());
}

template <class R, class... ARGS>
template<class T>
const T* bsl::function<R(ARGS...)>::target() const BSLS_NOTHROW_SPEC
{
    BDES_ASSERT(target_type() == typeid(T));
    const void *target_p = d_manager(GET_TARGET, this, nullptr);
    return static_cast<const T*>(target_p);
}

// FREE FUNCTIONS
template <class R, class... ARGS>
bool operator==(const bsl::function<R(ARGS...)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator==(bsl::nullptr_t,
                const bsl::function<R(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator!=(const bsl::function<R(ARGS...)>&,
                bsl::nullptr_t) BSLS_NOTHROW_SPEC;

template <class R, class... ARGS>
bool operator!=(bsl::nullptr_t,
                const bsl::function<R(ARGS...)>&) BSLS_NOTHROW_SPEC;

template <class R, class... ARGS>
inline
void swap(bsl::function<R(ARGS...)>& a, bsl::function<R(ARGS...)>& b)
{
    a.swap(b);
}

#endif // BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

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
