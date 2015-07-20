// bslstl_function.h                                                  -*-C++-*-
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

#ifndef INCLUDED_BSLALG_SCALARPRIMITIVES
#include <bslalg_scalarprimitives.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATORADAPTOR
#include <bslma_allocatoradaptor.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMF_ADDLVALUEREFERENCE
#include <bslmf_addlvaluereference.h>
#endif

#ifndef INCLUDED_BSLMF_ADDRVALUEREFERENCE
#include <bslmf_addrvaluereference.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_CONDITIONAL
#include <bslmf_conditional.h>
#endif

#ifndef INCLUDED_BSLMF_FORWARDINGTYPE
#include <bslmf_forwardingtype.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_ISEMPTY
#include <bslmf_isempty.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISRVALUEREFERENCE
#include <bslmf_isrvaluereference.h>
#endif

#ifndef INCLUDED_BSLMF_MEMBERFUNCTIONPOINTERTRAITS
#include <bslmf_memberfunctionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_MOVABLEREF
#include <bslmf_movableref.h>
#endif

#ifndef INCLUDED_BSLMF_NTHPARAMETER
#include <bslmf_nthparameter.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECONST
#include <bslmf_removeconst.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

#ifndef INCLUDED_BSLMF_SELECTTRAIT
#include <bslmf_selecttrait.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#include <bsls_exceptionutil.h>
#endif

#ifndef INCLUDED_BSLS_NULLPTR
#include <bsls_nullptr.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

#ifndef INCLUDED_BSLS_UNSPECIFIEDBOOL
#include <bsls_unspecifiedbool.h>
#endif

#ifndef INCLUDED_TYPEINFO
#include <typeinfo>
#define INCLUDED_TYPEINFO
#endif

#ifndef INCLUDED_UTILITY
#include <utility>
#define INCLUDED_UTILITY
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_STDDEF_H
#endif

#ifndef INCLUDED_STDLIB_H
#include <stdlib.h>
#define INCLUDED_STDLIB_H
#endif

// TBD: Remove this when BloombergLP is removed from bsl package group
using namespace BloombergLP;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

// In order to allow inter-converibility between 'bsl::function' and
// 'bdef_function', we need to bend our leveling rules a bit and
// forward-declare 'bdef_function'.  The use of this incomplete type assumes,
// that the structure of 'bdef_function' is identical to that of
// 'bsl::function', though with a slightly different public interface.  Note,
// that this is a by-name reference only.  No long-distance friendship is
// used.
namespace BloombergLP {
template <class PROTOTYPE>
class bdef_Function;
}  // close enterprise namespace

#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace bsl {

// TBD: Move into its own component (or into uses_allocator component)
struct allocator_arg_t { };
static const allocator_arg_t allocator_arg = {};

// Forward declarations
template <class FUNC>
class function;  // Primary template declared but not defined.

template <class MEM_FUNC_PTR, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke; // Primary template declared but not defined.

template <class ALLOC>
struct Function_AllocTraits;

template <class FUNC>
struct Function_ArgTypes;

template <class FUNC>
struct Function_NothrowWrapperUtil;

                        // =======================
                        // class bad_function_call
                        // =======================

#ifdef BDE_BUILD_TARGET_EXC

class bad_function_call : public native_std::exception {
    // Standard exception object thrown when attempting to invoke a null
    // function object.

  public:
    bad_function_call() BSLS_NOTHROW_SPEC;
        // Constructs this exception object.

    const char* what() const BSLS_NOTHROW_SPEC;
        // Returns "bad_function_call".
};

#endif // BDE_BUILD_TARGET_EXC

                        // ======================================
                        // class template Function_NothrowWrapper
                        // ======================================

template <class FUNC>
class Function_NothrowWrapper
{
    // If a functor can throw on move, 'bsl::function' will always allocate it
    // out-of-place so that move and swap will always be nothrow operations, as
    // is required by the standard.  Thus, many small functors will fail to
    // take advantage of the small-object optimization because they might throw
    // on move, no matter how unlikely that may be.  A function object wrapped
    // in 'Function_NothrowWrapper', however, will be treated by
    // 'bsl::function' as though it were a function object with a 'noexcept'
    // move constructor (even though it does not have the interface of a
    // function object).  This wrapper is especially useful in C++03 mode,
    // where 'noexcept' does not exist, it that even non-throwing operations
    // are assumed to throw unless they delcare the bitwise movable trait.
    // Note that, in the unlikely event that moving the wrapped object *does*
    // throw at runtime, the result will likely be a call to 'terminate()'.

    FUNC d_func;

  public:
    typedef FUNC UnwrappedType;

    Function_NothrowWrapper(const FUNC& other) : d_func(other) { }

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    Function_NothrowWrapper(FUNC&& other)
      : d_func(bslmf::MovableRefUtil::move(other))
    {
    }
#endif

    //! Function_NothrowWrapper(const Function_NothrowWrapper&) = default;
    //! Function_NothrowWrapper& operator=(const Function_NothrowWrapper&)
    //!    = default;
    //! ~Function_NothrowWrapper() = default;

    FUNC&       unwrap()       { return d_func; }
    FUNC const& unwrap() const { return d_func; }
};

                        // ======================================
                        // class Function_SmallObjectOptimization
                        // ======================================

class Function_SmallObjectOptimization {
    // Namespace for several definitions related to use of the small object
    // optimization.

    class Dummy;  // Declared but not defined

    // Short aliases for type with maximum platform alignment
    typedef bsls::AlignmentUtil::MaxAlignedType MaxAlignedType;

  public:
    union InplaceBuffer {
        // This 'union' defines the storage area for a functor representation.
        // The design uses the "small-object optimization" in an attempt to
        // avoid allocations for objects that are no larger than
        // 'InplaceBuffer'.  When using the in-place representation, the
        // invocable, whether a function pointer or function object (if it
        // should fit in the size of 'InplaceBuffer') is stored directly in the
        // 'InplaceBuffer'.  Anything bigger than 'sizeof(InplaceBuffer)' will
        // be stored out-of-place and its address will be stored in
        // 'd_object_p'.  Discriminating between the two representations can be
        // done by the manager with the opcode 'e_GET_SIZE'.
        //
        // Note that union members other than 'd_object_p' are just fillers to
        // make sure that a function or member function pointer can fit without
        // allocation and that alignment is respected.  The 'd_minbuf' member
        // ensures that 'InplaceBuffer' is at least large enough so that
        // modestly-complex functors (e.g., functors that own embedded
        // arguments, such as 'bdef_Bind' objects) to be constructed in-place
        // without triggering further allocation.  The benefit of avoiding
        // allocation for those function objects is balanced against the waste
        // of space when used with smaller function objects.
        //
        // The size of this 'union' was chosen so that the inplace buffer will
        // be 6 pointers in size and the total footprint of a 'bsl::function'
        // object on most platforms will be 10 pointers, which matches the
        // sizes of previous implementations of 'bdef_Function'.

        void                *d_object_p;     // pointer to external rep
        void               (*d_func_p)();    // pointer to function
        void        (Dummy::*d_memFunc_p)(); // pointer to member function
        MaxAlignedType       d_align;        // force align
        void                *d_minbuf[6];    // force minimum size
    };

    static const std::size_t k_NON_SOO_SMALL_SIZE = ~sizeof(InplaceBuffer);
        // This value is added to the size of a small stateful functor to
        // indicate that, despite being small, it should not be allocated
        // inplace using the small object optimization (SOO), e.g., because it
        // does not have a nothrow move constructor and cannot, therefore, be
        // swapped safely.  When a size larger than this constant is seen, the
        // actual object size can be determined by subtracting this constant.
        // A useful quality of this encoding is that if 'SZ <=
        // sizeof(InplaceBuffer)' for some object size 'SZ', then 'SZ +
        // k_NON_SOO_SMALL_SIZE > sizeof(InplaceBuffer)', so the 'SooFuncSize'
        // (below) for any object that should not be allocated inplace is
        // larger than 'sizeof(InplaceBuffer)', and the 'SooFuncSize' for any
        // object that *should* be allocated inplace is smaller than or equal
        // to 'sizeof(InplaceBuffer)', making the test for "is inplace
        // function" simple.

    BSLMF_ASSERT(k_NON_SOO_SMALL_SIZE > 0);  // Assert unsigned size_t

    template <class TP>
    struct SooFuncSize
    {
        // Metafunction to determine the size of an object for the purposes of
        // the small object optimization (SOO).  The 'VALUE' member is encoded
        // as follows:
        //
        //:  o If 'TP' is not larger than 'InplaceBuffer' but has a throwing
        //:    destructive move operation (and therefore should not be
        //:    allocated inplace), then 'VALUE == sizeof(TP) +
        //:    k_NON_SOO_SMALL_SIZE'.
        //:  o Otherwise, 'VALUE == sizeof(TP)'.
        //
        // Note that the 'Soo' prefix is used to indicate that an identifier
        // uses the above protocol.  Thus a variable called 'SooSize' is
        // assumed to be encoded as above, whereas a variable called 'size'
        // can generally be assumed not to be encoded that way.

#if    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)           \
    && defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        template <class SOME_TYPE>
            static typename bsl::add_rvalue_reference<SOME_TYPE>::type
                myDeclVal() noexcept;
            // The older versions of clang on OS/X do not provide declval even
            // in c++11 mode.
#endif

        static const std::size_t VALUE =
            sizeof(TP) > sizeof(InplaceBuffer)                ? sizeof(TP) :
            bslmf::IsBitwiseMoveable<TP>::value               ? sizeof(TP) :
            Function_NothrowWrapperUtil<TP>::IS_WRAPPED       ? sizeof(TP) :
#if    defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)           \
    && defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
            // Check if nothrow move constructible.  The use of '::new' lets
            // us check the constructor without also checking the destructor.
            // This is especially important in gcc 4.7 and before because
            // destructors are not implicitly 'noexcept' in those compilers.
            noexcept(::new((void*) 0) TP(myDeclVal<TP>())) ? sizeof(TP) :
#endif
            // If not nonthrow or bitwise moveable, then add
            // 'k_NON_SOO_SMALL_SIZE' to the size indicate that we should not
            // use the small object optimization for this type.
            sizeof(TP) + k_NON_SOO_SMALL_SIZE;
    };

    template <class FN>
    struct IsInplaceFunc :
        integral_constant<bool,SooFuncSize<FN>::VALUE <= sizeof(InplaceBuffer)>
    {
        // Metafunction to determine whether the specified 'FN' template
        // parameter should be allocated within the 'InplaceBuffer'.

        // TBD: 'InplaceFunc' should also take alignment into account, but
        // since we don't (yet) have the ability to specify alignment when
        // allocating memory, there's nothing we can do at this point.
    };
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
    // function prototype).  These management functions are run-time
    // polymorphic, and therefore do not require that this class be templated
    // (although several of the member functions are templated).  For technical
    // reasons, this class must be defined before 'bsl::function' (although a
    // mere forward declaration would be all right with most compilers, the Gnu
    // compiler emits an error when trying to do syntactic checking on template
    // code even though it does not instantiate template).

    // TYPES

    union PtrOrSize_t {
        // This union stores either a pointer to const void or a size_t.  It
        // is used as the input argument and return type for manager functions
        // (below).
      private:
        std::size_t  d_asSize_t;
        void        *d_asPtr;

      public:
        PtrOrSize_t(std::size_t s) : d_asSize_t(s) { }
        PtrOrSize_t(void *p) : d_asPtr(p) { }
        PtrOrSize_t() : d_asPtr(0) { }

        std::size_t asSize_t() const { return d_asSize_t; }
        void *asPtr() const { return d_asPtr; }
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

      , e_IS_EQUAL
            // (Allocator manager only) Return whether the allocator stored in
            // '*rep' is equal to the allocator stored in 'input', where
            // 'input' is the 'd_allocator_p' member of another 'Function_Rep'
            // object.  For 'STL-style' allocators, the allocators will be
            // considered equal only if 'rep->d_allocator_p' and 'input' both
            // point to 'bslma::AllocatorAdaptor' objects of the same type
            // wrapping allocators that compare equal.  For all other
            // allocators types, the allocators are considered equal only if
            // 'rep->d_allocator_p' is equal to the pointer stored in 'input'.

      , e_INIT_REP
            // (Allocator manager only) Initialize 'rep' using the 'input'
            // allocator.  Requires that 'input' point to an object of the
            // manager's 'ALLOC' type and that 'rep->d_funcManager_p' is
            // already set.
    };

  public:

    // Not really public: made public to work around a Sun compiler bug.
    enum AllocCategory {
        // Category of allocator supplied to a constructor.

        e_BSLMA_ALLOC_PTR        // Ptr to type derived from 'bslma::Allocator'
      , e_BSL_ALLOCATOR          // Instantiation of 'bsl::allocator'
      , e_ERASED_STATEFUL_ALLOC  // C++03 STL-style stateful allocator
      , e_ERASED_STATELESS_ALLOC // C++03 STL-style stateless allocator
    };

  private:

    typedef PtrOrSize_t (*Manager)(ManagerOpCode  opCode,
                                   Function_Rep  *rep,
                                   PtrOrSize_t    input);
        // 'Manager' is an alias for a pointer to a function that manages a
        // specific object type (i.e., it copies, moves, or destroys it).  It
        // implements a kind of hand-coded virtual-function dispatch.  The
        // specified 'opCode' is used to choose the "virtual function" to
        // invoke, where the specified 'rep' and 'input' are arguments to that
        // function.  Internally, a 'Manager' function uses a 'switch'
        // statement rather than performing a virtual-table lookup.  This
        // mechanism is chosen because it saves a significant amount of space
        // over the C++ virtual-function mechanism, especially when the number
        // of different instantiations of 'bsl::function' is large.

    typedef Function_SmallObjectOptimization Soo;
        // Type alias for convenience.

    typedef Soo::InplaceBuffer InplaceBuffer;
        // Type alias for convenience.

    static const std::size_t k_NON_SOO_SMALL_SIZE = Soo::k_NON_SOO_SMALL_SIZE;
        // Constant alias for convenience.

    template <class FUNC>
    friend class bsl::function;

    template <class ALLOC>
    friend struct Function_AllocTraits;

    template <class ALLOC>
    void copyInit(const ALLOC& alloc, const Function_Rep& other);
        // Using the specified 'alloc', copy-initialize this rep from the
        // specified 'other' rep.

    void moveInit(Function_Rep& other);
        // Move-initialize this rep from the specified 'other' rep, leaving
        // the latter empty.

    void makeEmpty();
        // Change this object to be an empty object without changing its
        // allocator.  Note that value returned by 'allocator()' might change,
        // but will point to an allocator with the same type managing the same
        // memory resource.

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

    void assign(Function_Rep *from, ManagerOpCode moveOrCopy);
        // Move or copy the value of the specified '*from' object into
        // '*this', depending on the the value of the specified 'moveOrCopy'
        // argument.  The previous value of '*this' is discarded.  The
        // bahavior is undefined unless 'moveOrCopy' is either
        // 'e_MOVE_CONSTRUCT' or 'e_COPY_CONSTRUCT'.

    static void destructiveMove(Function_Rep *to,
                                Function_Rep *from) BSLS_NOTHROW_SPEC;
        // Move the state from the specified 'from' location to the specified
        // 'to' location, where 'to' points to uninitialized storage.  After
        // the move, 'from' points to uninitialized storage.  The move is
        // performed using only non-throwing operations.

    template <class FUNC, bool IS_INPLACE>
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

    template <class FUNC>
    static Manager getFunctionManager();

  private:
    // DATA
    mutable InplaceBuffer d_objbuf;  // in-place representation (if fits, as
                                     // indicated by the manager), or pointer
                                     // to external representation

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
                                     // 'ALLOC' is 'bslma::Allocator*'.
                                     // INVARIANT: Must be NULL unless
                                     // 'd_allocator_p' has been set to a
                                     // fully-constructed allocator.

    void (*d_invoker_p)();           // Pointer to invoker function

    // The 'isInplace function is public in BDE legacy mode and private
    // otherwise.
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
  public:
#endif

    bool isInplace() const BSLS_NOTHROW_SPEC;
        // Return true if the wrapped functor is allocated in place within the
        // footprint of this 'function' object; otherwise return false.

  public:
    // CREATORS
    Function_Rep();
    ~Function_Rep();

    // MANIPULATORS
    void swap(Function_Rep& other) BSLS_NOTHROW_SPEC;

    // ACCESSORS
    const std::type_info& target_type() const BSLS_NOTHROW_SPEC;
    template<class TP> TP*       target()       BSLS_NOTHROW_SPEC;
    template<class TP> const TP* target() const BSLS_NOTHROW_SPEC;
    bslma::Allocator *allocator() const;
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=14

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

    // PRIVATE TYPES
    typedef RET Invoker(const Function_Rep* rep,
                        typename bslmf::ForwardingType<ARGS>::Type... args);
        // A function of this type is used to invoke the wrapped functor.

    // PRIVATE MEMBER FUNCTIONS
    void setInvoker(Invoker *p);
        // Set the invoker to the address in the specified 'p' argument.

    Invoker *invoker() const;
        // Return the current invoker.

    template <class FUNC>
    static Invoker *invokerForFunc(const FUNC& f,
                             bslmf::SelectTraitCase<bslmf::IsFunctionPointer>)
        // Return the invoker for an invocable of
        // pointer-to-(non-member)function.  Defined inline to work around Sun
        // CC bug.
    {
        if (f) {
            return &functionPtrInvoker<FUNC>;
        }
        else {
            return NULL;
        }
    }

    template <class FUNC>
    static Invoker *invokerForFunc(const FUNC& f,
                       bslmf::SelectTraitCase<bslmf::IsMemberFunctionPointer>)
        // Return the invoker for an invocable of pointer-to-member-function
        // type.  Defined inline to work around Sun CC bug.
    {
        if (f) {
            return &memFuncPtrInvoker<FUNC>;                          // RETURN
        }
        else {
            return NULL;                                              // RETURN
        }
    }

    template <class FUNC>
    static Invoker *invokerForFunc(const FUNC&,
                                   bslmf::SelectTraitCase<Soo::IsInplaceFunc>)
        // Return the invoker for an invocable of in-place functor type.
        // Defined inline to work around Sun CC bug.
    {
        return &inplaceFunctorInvoker<FUNC>;
    }

    template <class FUNC>
    static Invoker *invokerForFunc(const FUNC&, bslmf::SelectTraitCase<>)
        // Return the invoker for an invocable of out-of-place functor type.
        // Defined inline to work around Sun CC bug.
    {
        return &outofplaceFunctorInvoker<FUNC>;
    }

    template <class FUNC>
    static Invoker *invokerForFunc(const FUNC& f)
        // Return the invoker for an invocable of the specified 'FUNC' type.
        // Defined inline to work around Sun CC bug.
    {
        typedef Function_SmallObjectOptimization Soo;

        // Unwrap FUNC type if it is a specialization of
        // 'Function_NothrowWrapper'.
        typedef typename
            Function_NothrowWrapperUtil<FUNC>::UnwrappedType UwFuncType;

        // Determine dispatch based on the traits of 'FuncType'.
        typedef typename
            bslmf::SelectTrait<UwFuncType,
            bslmf::IsFunctionPointer,
            bslmf::IsMemberFunctionPointer,
            Soo::IsInplaceFunc>::Type UwFuncSelection;

        const std::size_t kSOOSIZE       = Soo::SooFuncSize<FUNC>::VALUE;
        const std::size_t kUNWRAPPED_SOOSIZE
                                         = Soo::SooFuncSize<UwFuncType>::VALUE;

        // The only reason that the original and unwrapped 'FUNC' would result
        // in different 'SooFuncSize' is if 'FUNC' is wrapping a small object
        // that would otherwise have a throwing move.  In that case, we force
        // the dispatch to choose 'Soo::IsInplaceFunc', otherwise we dispatch
        // on the selection traits of the original 'FUNC' type.
        typedef typename
            bsl::conditional<kSOOSIZE != kUNWRAPPED_SOOSIZE,
            bslmf::SelectTraitCase<Soo::IsInplaceFunc>,
            UwFuncSelection>::type FuncSelection;

        // Dispatch to the correct variant of 'invokerForFunc'
        return invokerForFunc(Function_NothrowWrapperUtil<FUNC>::unwrap(f),
                FuncSelection());
    }

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

#ifndef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    // UNSPECIFIED BOOL

    // This type is needed only in C++03 mode, where 'explicit' conversion
    // operators are not supported.  A 'function' is implicitly converted to
    // 'UnspecifiedBool' when used in 'if' statements, but is not implicitly
    // convertible to 'bool'.
    typedef bsls::UnspecifiedBool<function>        UnspecifiedBoolUtil;
    typedef typename UnspecifiedBoolUtil::BoolType UnspecifiedBool;

    // Since 'function' does not support 'operator==' and 'operator!=', they
    // must be deliberately supressed; otherwise 'function' objects would be
    // implicitly comparable by implicit conversion to 'UnspecifiedBool'.
    bool operator==(const function&) const;  // Declared but not defined
    bool operator!=(const function&) const;  // Declared but not defined
#endif

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
    function& operator=(function&);
        // Copy constructor.  Needed to overload in preference to
        // 'operator=(FUNC&&)'.
    function& operator=(function&&);
#endif
    template<class FUNC>
    function& operator=(FUNC&&);
    function& operator=(nullptr_t);

    // TBD: Need to implement reference_wrapper.
    // template<class FUNC>
    // function& operator=(reference_wrapper<FUNC>) BSLS_NOTHROW_SPEC;

    // template<class FUNC, class ALLOC> void assign(FUNC&&, const ALLOC&);
    //     // We have filed an issue report and have elected not to support
    //     // this function because the arguments and definition in the
    //     // standard make no sense.  Replacing the allocator of an existing
    //     // object is inconsistent with the rest of the standard.

    RET operator()(ARGS...) const;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    operator       bdef_Function<RET(*)(ARGS...)>&()       BSLS_NOTHROW_SPEC;
        // Return a reference to a modifiable 'bdef_Function' that is an alias
        // of '*this'.  Note that no copy is made; modifying the object
        // through the returned object (e.g., by assigning to it) will modify
        // '*this'.  This operator depends on 'bdef_Function' being a thin
        // layer on top of 'bsl::function', having identical structure.
#endif

    // ACCESSORS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
    explicit  // Explicit conversion available only with C++11
    operator bool() const BSLS_NOTHROW_SPEC;
#else
    // Simulation of explicit conversion to bool.
    // Inlined to work around xlC bug when out-of-line.
    operator UnspecifiedBool() const BSLS_NOTHROW_SPEC
    {
        return UnspecifiedBoolUtil::makeValue(invoker());
    }
#endif

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    operator const bdef_Function<RET(*)(ARGS...)>&() const BSLS_NOTHROW_SPEC;
        // Return a reference to a 'bdef_Function' that is an alias of '*this'.
        // Note that no copy is made; modifying the object through the returned
        // object (e.g., by assigning to it) will modify '*this'.  This
        // operator depends on 'bdef_Function' being a thin layer on top of
        // 'bsl::function', having identical structure.
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

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ---------------------------------
                        // struct template Function_ArgTypes
                        // ---------------------------------

template <class FUNC>
struct Function_ArgTypes {
    // This is a component-private struct template.  Do not use.
    //
    // The standard requires that 'function' define certain typedefs for
    // compatibility with one- and two-argument legacy functor adaptors.  This
    // template provides the following nested typedefs:
    //..
    //  argument_type        -- Only if FUNC takes exactly one argument
    //  first_argument_type  -- Only if FUNC takes exactly two arguments
    //  second_argument_type -- Only if FUNC takes exactly two arguments
    //..

    // No typedefs for the unspecialized case
};

template <class R, class ARG>
struct Function_ArgTypes<R(ARG)> {
    // Specialization for functions that take exactly one argument.

    typedef ARG argument_type;
};

template <class R, class ARG1, class ARG2>
struct Function_ArgTypes<R(ARG1, ARG2)> {
    // Specialization for functions that take exactly two arguments.

    typedef ARG1 first_argument_type;
    typedef ARG2 second_argument_type;
};

                        // -------------------------------------------
                        // struct template Function_NothrowWrapperUtil
                        // -------------------------------------------

template <class FUNC>
struct Function_NothrowWrapperUtil {
    // This is a component-private struct template.  Do not use.
    //
    // Namesapace for 'Function_NothrowWrapper' traits and utilities.

    typedef FUNC UnwrappedType;

    enum { IS_WRAPPED = false };
         // True for specializations of 'Function_NothrowWrapper', else false.

    static FUNC&       unwrap(typename bsl::remove_cv<FUNC>::type&       f)
        { return f; }
    static FUNC const& unwrap(typename bsl::remove_cv<FUNC>::type const& f)
        { return f; }
};

template <class FUNC>
struct  Function_NothrowWrapperUtil<Function_NothrowWrapper<FUNC> > {
    // This is a component-private struct template.  Do not use.
    //
    // Namesapace for 'Function_NothrowWrapper' traits and utilities,
    // specialized for instantiations of 'Function_NothrowWrapper<FUNC>'.

    typedef Function_NothrowWrapper<FUNC> WrappedType;
    typedef FUNC                          UnwrappedType;

    enum { IS_WRAPPED = true };
         // True for specializations of 'Function_NothrowWrapper', else false.

    static FUNC&       unwrap(WrappedType&       f) { return f.unwrap(); }
    static FUNC const& unwrap(WrappedType const& f) { return f.unwrap(); }
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES

                    // -------------------------------------
                    // class template Function_MemFuncInvoke
                    // -------------------------------------

template <class FUNC, class OBJ_TYPE, class OBJ_ARG_TYPE,
          class RET, class... ARGS>
struct Function_MemFuncInvokeImp {
    // This is a component-private class template.  Do not use.
    //

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
        // reference back to the original type.  The 'const_cast', below, will
        // have no effect unless 'OBJ_ARG_TYPE' is a non-const rvalue.
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
    // This is a component-private struct template.  Do not use.
    //
};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...) const, OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...) const,
                                const OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS...>
{
    // This is a component-private struct template.  Do not use.
    //
};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...) volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...) volatile,
                                volatile OBJ_TYPE, OBJ_ARG_TYPE, RET, ARGS...>
{
    // This is a component-private struct template.  Do not use.
    //
};

template <class RET, class OBJ_TYPE, class... ARGS, class OBJ_ARG_TYPE>
struct Function_MemFuncInvoke<RET (OBJ_TYPE::*)(ARGS...) const volatile,
                              OBJ_ARG_TYPE>
    : Function_MemFuncInvokeImp<RET (OBJ_TYPE::*)(ARGS...) const volatile,
                                const volatile OBJ_TYPE,
                                OBJ_ARG_TYPE, RET, ARGS...>
{
    // This is a component-private struct template.  Do not use.
    //
};

#endif

class Function_PairBufDesc {
    // This is a component-private struct template.  Do not use.
    //
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
    // This is a component-private struct template.  Do not use.
    //
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

template <class ALLOC>
const Function_Rep::AllocCategory Function_AllocTraits<ALLOC>::k_CATEGORY;

template <class ALLOC>
const Function_Rep::AllocCategory Function_AllocTraits<ALLOC *>::k_CATEGORY;

template <class TYPE>
const Function_Rep::AllocCategory
    Function_AllocTraits<bsl::allocator<TYPE> >::k_CATEGORY;

}  // close namespace bsl

                        // --------------------------
                        // class Function_PairBufDesc
                        // --------------------------

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

                        // --------------------------------------
                        // class Function_SmallObjectOptimization
                        // --------------------------------------

template <class TP>
const std::size_t
bsl::Function_SmallObjectOptimization::SooFuncSize<TP>::VALUE;

                        // -----------------------
                        // class bad_function_call
                        // -----------------------

#ifdef BDE_BUILD_TARGET_EXC

inline
bsl::bad_function_call::bad_function_call() BSLS_NOTHROW_SPEC
    : std::exception()
{
}

#endif

                        // -----------------------
                        // class bsl::Function_Rep
                        // -----------------------

template <class ALLOC>
void bsl::Function_Rep::copyInit(const ALLOC& alloc, const Function_Rep& other)
{
    d_funcManager_p = other.d_funcManager_p;
    d_invoker_p = other.d_invoker_p;

    std::size_t sooFuncSize = d_funcManager_p ?
        d_funcManager_p(e_GET_SIZE, this, PtrOrSize_t()).asSize_t() : 0;

    typedef Function_AllocTraits<ALLOC> Traits;
    initRep(sooFuncSize,
            typename Traits::Type(alloc), typename Traits::Category());

    if (d_funcManager_p) {
        PtrOrSize_t source = d_funcManager_p(e_GET_TARGET,
                                             const_cast<Function_Rep*>(&other),
                                             PtrOrSize_t());
        d_funcManager_p(e_COPY_CONSTRUCT, this, source);
    }
}

template <class FUNC, bool IS_INPLACE>
bsl::Function_Rep::PtrOrSize_t
bsl::Function_Rep::functionManager(ManagerOpCode  opCode,
                                   Function_Rep  *rep,
                                   PtrOrSize_t    input)
{
    static const std::size_t k_SOO_FUNC_SIZE =
        IS_INPLACE ? sizeof(FUNC) : Soo::SooFuncSize<FUNC>::VALUE;
    static const bool k_IS_INPLACE = IS_INPLACE;

    // If a function manager exists, then functor must have non-zero size.
    BSLMF_ASSERT(0 != k_SOO_FUNC_SIZE);

    // If wrapped function fits in 'd_objbuf', then it is inplace; otherwise,
    // its heap-allocated address is found in 'd_objbuf.d_object_p'.  There is
    // no need to computed this using metaprogramming; the compiler will
    // optimize away the conditional test.
    char *wrappedFuncBuf_p = static_cast<char*>(
        k_IS_INPLACE ? &rep->d_objbuf : rep->d_objbuf.d_object_p);
    FUNC *wrappedFunc_p = reinterpret_cast<FUNC*>(wrappedFuncBuf_p);

    switch (opCode) {

      case e_MOVE_CONSTRUCT: {
        // Move-construct function object.  There is no point to optimizing
        // this operation for trivial types.  If the type is trivially
        // moveable or copyable, then the move or copy operation below will do
        // it trivially.

        FUNC &srcFunc = *static_cast<FUNC*>(input.asPtr());

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        bslalg::ScalarPrimitives::moveConstruct(wrappedFunc_p,
                                                srcFunc,
                                                rep->d_allocator_p);
#else
        bslalg::ScalarPrimitives::copyConstruct(wrappedFunc_p,
                                                srcFunc,
                                                rep->d_allocator_p);
#endif
        return wrappedFunc_p;                                         // RETURN
      } break;

      case e_COPY_CONSTRUCT: {

        // Copy-construct function object.  There is no point to optimizing
        // this operation for bitwise copyable types.  If the type is trivially
        // copyable, then the copy operation below will do it trivially.
        const FUNC &srcFunc = *static_cast<const FUNC*>(input.asPtr());
        bslalg::ScalarPrimitives::copyConstruct(wrappedFunc_p,
                                                srcFunc,
                                                rep->d_allocator_p);
        return wrappedFunc_p;                                         // RETURN
      } break;

      case e_DESTROY: {

        // Call destructor for functor.
        wrappedFunc_p->~FUNC();

        // Return size of destroyed function object
        return k_SOO_FUNC_SIZE;                                       // RETURN
      } break;

      case e_DESTRUCTIVE_MOVE: {
        void *input_p      = input.asPtr();
        FUNC *srcFunc_p    = static_cast<FUNC*>(input_p);

        bslalg::ScalarPrimitives::destructiveMove(wrappedFunc_p, srcFunc_p,
                                                  rep->d_allocator_p);
      } break;

      case e_GET_SIZE:     return k_SOO_FUNC_SIZE;                    // RETURN
      case e_GET_TARGET:   return wrappedFunc_p;                      // RETURN
      case e_GET_TYPE_ID:
        return const_cast<std::type_info*>(&typeid(FUNC));            // RETURN

      case e_IS_EQUAL:
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
    typedef typename bslma::AllocatorAdaptor<ALLOC>::Type Adaptor;

    switch (opCode) {
      case e_MOVE_CONSTRUCT: // Fall through: allocators are always copied
      case e_COPY_CONSTRUCT: {
        const Adaptor& other = *static_cast<const Adaptor*>(input.asPtr());
        return ::new ((void*) rep->d_allocator_p) Adaptor(other);     // RETURN
      } break;

      case e_DESTROY: {
        // Allocator cannot deallocate itself, so make a copy of the allocator
        // on the stack and use the copy for deallocation.
        Adaptor allocCopy(*static_cast<Adaptor*>(rep->d_allocator_p));
        std::size_t sooFuncSize = input.asSize_t();

        rep->d_allocator_p->~Allocator();  // destroy allocator

        if (sooFuncSize > sizeof(InplaceBuffer)) {
            // Deallocate memory holding both functor and allocator
            allocCopy.deallocate(rep->d_objbuf.d_object_p);
        }
        else {
            // Functor is inplace but allocator is not.
            // Deallocate space used by allocator.
            allocCopy.deallocate(rep->d_allocator_p);
        }

        // Return size allocator adaptor.
        return sizeof(Adaptor);                                       // RETURN
      }

      case e_DESTRUCTIVE_MOVE: {
        const Adaptor& other = *static_cast<const Adaptor*>(input.asPtr());

        // Compute the distance (in bytes) between the start of the source and
        // the start of the destination to see if they overlap.
        std::ptrdiff_t dist =
            abs(reinterpret_cast<const char*>(rep->d_allocator_p) -
                reinterpret_cast<const char*>(&other));

        if (dist >= sizeof(Adaptor)) {
            // Input and output don't overlap.
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            ::new ((void*) rep->d_allocator_p)
                                   Adaptor(bslmf::MovableRefUtil::move(other));
#else
            ::new ((void*) rep->d_allocator_p) Adaptor(other);
#endif
            other.~Adaptor();
        }
        else {
            // Input and output overlap so we need to move through a temporary
            // variable.
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
            Adaptor temp(bslmf::MovableRefUtil::move(other));
            other.~Adaptor();
            ::new ((void*) rep->d_allocator_p)
                                    Adaptor(bslmf::MovableRefUtil::move(temp));
#else
            Adaptor temp(other);
            other.~Adaptor();
            ::new ((void*) rep->d_allocator_p) Adaptor(temp);
#endif
        }
      } break;

      case e_GET_SIZE: {
        // The SOO size of the adaptor is always the same as the true size of
        // the adaptor because it is always nothrow moveable.
        return sizeof(Adaptor);                                       // RETURN
      }

      case e_GET_TARGET:  return rep->d_allocator_p;                  // RETURN
      case e_GET_TYPE_ID:
          return const_cast<std::type_info*>(&typeid(Adaptor));       // RETURN

      case e_IS_EQUAL: {
        const bslma::Allocator *inputAlloc =
            static_cast<const bslma::Allocator *>(input.asPtr());
        const Adaptor *inputAdaptor = dynamic_cast<const Adaptor*>(inputAlloc);
        Adaptor *thisAdaptor = static_cast<Adaptor*>(rep->d_allocator_p);
        return inputAdaptor ?
            inputAdaptor->adaptedAllocator() == thisAdaptor->adaptedAllocator()
            : false;                                                  // RETURN
      } break;

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

template <class FUNC>
inline
bsl::Function_Rep::Manager bsl::Function_Rep::getFunctionManager()
{
    static const bool k_IS_INPLACE = Soo::IsInplaceFunc<FUNC>::value;
    typedef typename
        Function_NothrowWrapperUtil<FUNC>::UnwrappedType UnwrappedFunc;
    return &functionManager<UnwrappedFunc, k_IS_INPLACE>;
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
void *bsl::Function_Rep::initRep(std::size_t  sooFuncSize,
                                 const ALLOC& alloc,
                                 integral_constant<AllocCategory,
                                                   e_ERASED_STATEFUL_ALLOC>)
{
    typedef typename bslma::AllocatorAdaptor<ALLOC>::Type Adaptor;

    static const std::size_t allocSize = sizeof(Adaptor);

    std::size_t funcSize = (sooFuncSize >= k_NON_SOO_SMALL_SIZE ?
                            sooFuncSize - k_NON_SOO_SMALL_SIZE :
                            sooFuncSize);

    bool isInplaceFunc = sooFuncSize <= sizeof(InplaceBuffer);

    void *function_p;
    void *allocator_p;

    // A type-erased allocator is never allocated inplace.  Allocating the
    // allocator out of place allows the allocator to be moved from one
    // function object to another without changing its address.  This pointer
    // stability is critical to ensuring that move-constructing an inplace
    // functor does not result in a functor holding an pointer to a different
    // 'function' object's allocator.  Not having the erased allocator inplace
    // also simplifies the logic in a number of places.
    //
    // If the functor is allocated out of place, then the functor and the
    // out-of-place allocator can be allocated in a single memory block, since
    // they will always move together as a unit and the allocator's address
    // will not change.
    //
    // Although this is a run-time 'if' statement, the compiler will usually
    // optimize away the conditional when 'sooFuncSize' is known at compile
    // time and this function is inlined.  (Besides, it's cheap even if not
    // optimized away).
    if (isInplaceFunc) {
        // Function object fits in-place, but allocator is out-of-place
        function_p = &d_objbuf;
        // Allocate allocator adaptor from allocator itself
        allocator_p = Adaptor(alloc).allocate(allocSize);
    }
    else {
        // Not in-place.  Allocate (from the allocator) a single block to hold
        // the function and allocator adaptor.
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
    static typename bslma::AllocatorAdaptor<ALLOC>::Type allocInstance(alloc);

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

    typedef typename bslma::AllocatorAdaptor<ALLOC>::Type Adaptor;

    // If the our allocator has the same type as the adapted stateless
    // allocator, then they are assumed equal.
    return NULL != dynamic_cast<Adaptor*>(d_allocator_p);
}

template <class ALLOC>
bool bsl::Function_Rep::equalAlloc(const ALLOC& alloc,
               integral_constant<AllocCategory, e_ERASED_STATEFUL_ALLOC>) const
{
    BSLMF_ASSERT((is_same<typename ALLOC::value_type, char>::value));

    typedef typename bslma::AllocatorAdaptor<ALLOC>::Type Adaptor;

    // Try to cast our allocator into the same type as the adapted 'ALLOC'.
    Adaptor *thisAdaptor = dynamic_cast<Adaptor*>(d_allocator_p);

    if (! thisAdaptor) {
        // Different type.  Cannot compare equal.
        return false;                                                 // RETURN
    }

    // Compare the wrapped STL allocator to 'alloc'.
    return thisAdaptor->adaptedAllocator() == alloc;
}

template <class ALLOC, bsl::Function_Rep::AllocCategory ATP>
void bsl::Function_Rep::copyRep(const Function_Rep&                   other,
                                const ALLOC&                          alloc,
                                integral_constant<AllocCategory, ATP> atp)
{
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

inline
bsl::Function_Rep::Function_Rep()
{
    d_objbuf.d_func_p = NULL;
    d_funcManager_p   = NULL;
    d_allocator_p     = NULL;
    d_allocManager_p  = NULL;
    d_invoker_p       = NULL;
}

template<class TP>
TP* bsl::Function_Rep::target() BSLS_NOTHROW_SPEC
{
    if ((! d_funcManager_p) || target_type() != typeid(TP)) {
        return NULL;                                                  // RETURN
    }

    PtrOrSize_t target = d_funcManager_p(e_GET_TARGET, this, PtrOrSize_t());
    return static_cast<TP*>(target.asPtr());
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
    return static_cast<RET>(f(bslmf::ForwardingTypeUtil<ARGS>::
                              forwardToTarget(args)...));
}

template <class RET, class... ARGS>
template <class FUNC>
RET bsl::function<RET(ARGS...)>::memFuncPtrInvoker(const Function_Rep *rep,
                            typename bslmf::ForwardingType<ARGS>::Type... args)
{
    // Workaround Sun compiler issue - it thinks we're trying to cast away
    // const or volatile if we use reinterpret_cast.
    // FUNC f = reinterpret_cast<const FUNC&>(rep->d_objbuf.d_memFunc_p);
    FUNC f = (const FUNC&)(rep->d_objbuf.d_memFunc_p);
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
inline
void bsl::function<RET(ARGS...)>::setInvoker(Invoker *p)
{
    // Verify the assumption that all function pointers are the same size.
    BSLMF_ASSERT(sizeof(Invoker*) == sizeof(d_invoker_p));

    typedef void (*VoidFn)();

    d_invoker_p = reinterpret_cast<VoidFn>(p);
}

template <class RET, class... ARGS>
inline
typename bsl::function<RET(ARGS...)>::Invoker *
bsl::function<RET(ARGS...)>::invoker() const
{
    return reinterpret_cast<Invoker*>(d_invoker_p);
}

// CREATORS
template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function() BSLS_NOTHROW_SPEC
{
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = &unownedAllocManager;
    setInvoker(NULL);
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>::function(nullptr_t) BSLS_NOTHROW_SPEC
{
    d_allocator_p     = bslma::Default::defaultAllocator();
    d_allocManager_p  = &unownedAllocManager;
    setInvoker(NULL);
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
    setInvoker(invokerForFunc(func));

    std::size_t sooFuncSize = invoker() ? Soo::SooFuncSize<FUNC>::VALUE : 0;

    initRep(sooFuncSize, bslma::Default::defaultAllocator(),
            integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>());

    if (invoker()) {
        d_funcManager_p = getFunctionManager<FUNC>();
        d_funcManager_p(e_MOVE_CONSTRUCT, this, &func);
    }
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC& alloc)
{
    setInvoker(NULL);

    typedef Function_AllocTraits<ALLOC> Traits;
    initRep(0, typename Traits::Type(alloc), typename Traits::Category());
}

template <class RET, class... ARGS>
template<class ALLOC>
bsl::function<RET(ARGS...)>::function(allocator_arg_t, const ALLOC& alloc,
                                      nullptr_t)
{
    setInvoker(NULL);

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

    setInvoker(invokerForFunc(func));

    std::size_t sooFuncSize = invoker() ? Soo::SooFuncSize<FUNC>::VALUE : 0;

    initRep(sooFuncSize, typename AllocTraits::Type(alloc),
            typename AllocTraits::Category());

    if (invoker()) {
        d_funcManager_p = getFunctionManager<FUNC>();
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
    else {
        copyInit(typename AllocTraits::Type(alloc), other);
    }
}

#endif //  BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
inline
bsl::function<RET(ARGS...)>::~function()
{
    // Assert class invariants
    BSLS_ASSERT(invoker() || ! d_funcManager_p);
    BSLS_ASSERT(d_allocator_p);

    // Destroying the functor is not done in the base class destructor because
    // the invariant of the functor being fully constructed is not maintained
    // in the base class.  Moving this destruction to the base class would
    // cause exception handling to break if an exception were thrown when
    // constructing the wrapped functor.
    if (d_funcManager_p) {
        d_funcManager_p(e_DESTROY, this, PtrOrSize_t());
    }
}

// MANIPULATORS
template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(const function& rhs)
{
    Function_Rep::assign(const_cast<function*>(&rhs), e_COPY_CONSTRUCT);

    return *this;
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

template <class RET, class... ARGS>
inline
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(function& rhs)
{
    // Delegate to the const version
    return operator=(const_cast<const function&>(rhs));
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(function&& rhs)
{
    if (d_allocManager_p(e_IS_EQUAL, this, rhs.d_allocator_p).asSize_t()) {
        // Equal allocators.  Just swap.
        this->swap(rhs);
    }
    else {
        Function_Rep::assign(&rhs, e_MOVE_CONSTRUCT);
    }

    return *this;
}

#endif

template <class RET, class... ARGS>
template<class FUNC>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(FUNC&& func)
{
    Function_Rep tempRep;

    // Remove reference and const from 'FUNC' to get underlying functor type.
    typedef typename bsl::remove_const<
            typename bsl::remove_reference<FUNC>::type
        >::type FuncType;

    Invoker *invoker_p = invokerForFunc(func);
    tempRep.d_funcManager_p = invoker_p ? getFunctionManager<FuncType>() :NULL;

    // Initialize tempRep using allocator from 'this'
    this->d_allocManager_p(e_INIT_REP, &tempRep, this->d_allocator_p);

    // Move 'func' into initialized 'tempRep'
    if (tempRep.d_funcManager_p) {
        // Get non-const pointer to 'func'
        FuncType *funcAddr = const_cast<FuncType*>(&func);
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        if (bsl::is_rvalue_reference<FUNC&&>::value) {
            tempRep.d_funcManager_p(e_MOVE_CONSTRUCT, &tempRep, funcAddr);
        }
        else
#endif
        {
            tempRep.d_funcManager_p(e_COPY_CONSTRUCT, &tempRep, funcAddr);
        }
    }

    // If successful (no exceptions thrown) swap 'tempRep' into '*this'.
    tempRep.swap(*this);
    if (tempRep.d_funcManager_p) {
        // Destroy the functor in 'tempRep' before 'tempRep' goes out of scope
        tempRep.d_funcManager_p(e_DESTROY, &tempRep, PtrOrSize_t());
    }

    setInvoker(invoker_p);

    return *this;
}

template <class RET, class... ARGS>
bsl::function<RET(ARGS...)>&
bsl::function<RET(ARGS...)>::operator=(nullptr_t)
{
    setInvoker(NULL);
    makeEmpty();
    return *this;
}

// TBD: Need to implement reference_wrapper.
// template <class RET, class... ARGS>
// template<class FUNC>
// function& bsl::function<RET(ARGS...)>::operator=(reference_wrapper<FUNC>)
//     BSLS_NOTHROW_SPEC

template <class RET, class... ARGS>
RET bsl::function<RET(ARGS...)>::operator()(ARGS... args) const
{
#ifdef BDE_BUILD_TARGET_EXC

    if (invoker()) {
        return invoker()(this, args...);                              // RETURN
    }
    else {
        BSLS_THROW(bad_function_call());
    }

#else
    // Non-exception build
    BSLS_ASSERT_OPT(invoker());
    return invoker()(this, args...);
#endif
}

// ACCESSORS

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE
template <class RET, class... ARGS>
inline
bsl::function<RET(ARGS...)>::operator bool() const BSLS_NOTHROW_SPEC
{
    // If there is an invoker, then this function is non-empty (return true);
    // otherwise it is empty (return false).
    return invoker();
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
template <class RET, class... ARGS>
inline
bsl::function<RET(ARGS...)>::
    operator bdef_Function<RET(*)(ARGS...)>&() BSLS_NOTHROW_SPEC
{
    return *reinterpret_cast<bdef_Function<RET(*)(ARGS...)>*>(this);
}

template <class RET, class... ARGS>
inline
bsl::function<RET(ARGS...)>::
    operator const bdef_Function<RET(*)(ARGS...)>&() const BSLS_NOTHROW_SPEC
{
    return *reinterpret_cast<const bdef_Function<RET(*)(ARGS...)>*>(this);
}
#endif // BDE_OMIT_INTERNAL_DEPRECATED


// FREE FUNCTIONS
template <class RET, class... ARGS>
inline
bool bsl::operator==(const bsl::function<RET(ARGS...)>& f,
                     bsl::nullptr_t) BSLS_NOTHROW_SPEC
{
    return !f;
}

template <class RET, class... ARGS>
inline
bool bsl::operator==(bsl::nullptr_t,
                     const bsl::function<RET(ARGS...)>& f) BSLS_NOTHROW_SPEC
{
    return !f;
}

template <class RET, class... ARGS>
inline
bool bsl::operator!=(const bsl::function<RET(ARGS...)>& f,
                     bsl::nullptr_t) BSLS_NOTHROW_SPEC
{
    return static_cast<bool>(f);
}

template <class RET, class... ARGS>
inline
bool bsl::operator!=(bsl::nullptr_t,
                     const bsl::function<RET(ARGS...)>& f) BSLS_NOTHROW_SPEC
{
    return static_cast<bool>(f);
}

template <class RET, class... ARGS>
inline
void bsl::swap(bsl::function<RET(ARGS...)>& a, bsl::function<RET(ARGS...)>& b)
{
    a.swap(b);
}

#endif

#endif // ! defined(INCLUDED_BSLSTL_FUNCTION)

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
