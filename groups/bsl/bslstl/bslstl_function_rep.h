// bslstl_function_rep.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_FUNCTION_REP
#define INCLUDED_BSLSTL_FUNCTION_REP

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a non-template, common implementation for 'bsl::function'.
//
//@CLASSES:
// bslstl::Function_Rep: Representation of a 'bsl::function' object
//
//@SEE_ALSO: bslstl_function
//
//@DESCRIPTION: This private, subordinate component to 'bslstl_function'
// provides a non-template class, 'Function_Rep', that is the data
// representation of 'bsl::function' (see {'bslstl_function'}).  The
// 'bsl::function' class template uses 'bslstl::Function_Rep' to store the
// callable held by the 'function' (its *target*), the allocator, and a pointer
// to the function it uses to indirectly invoke the target (the *invoker*
// function).
//
// The client of this component, 'bsl::function', is a complex class that is
// templated in two ways:
//
//: 1 The class itself has a template parameter representing the prototype
//:   (argument and return types) of its call operator.  E.g.,
//:   type 'bsl::function<int(char*)>' has member 'int operator()(char*);'.
//:
//: 2 Several of its constructors are templated on a callable type and wrap an
//:   object of that type.  By using type erasure, the type of the wrapped
//:   target is not part of the type of the 'bsl::function'.
//
// The 'Function_Rep' class takes care of the runtime polymorphism required by
// (2), above.  It stores the target object (which can be of any size), copy-
// or move-constructs it, destroys it, and returns its runtime type, size, and
// address.  Nothing in 'Function_Rep' is concerned with the call prototype.
//
// 'Function_Rep' is a quasi-value-semantic type: It doesn't provide copy and
// move constructors or assignment operators, but it does have the abstract
// notion of an in-memory value (the target object, if not empty) and it
// provides methods for copying, moving, swapping, and destroying that value.
// There is no ability to provide equality comparison because the wrapped
// object is not required to provide equality comparison operations.
//
// 'Function_Rep' has only one constructor, which creates an empty object (one
// with no target) using a specified allocator.  The methods of 'Function_Rep'
// are a collection of primitive operations for setting, getting, copy
// constructing, move constructing, or swapping the target object, as well as
// accessing the allocator and invoker function pointer.  The methods to set
// and get the invoker pointer represent it as a generic function pointer (the
// closest we could get to 'void *' for function pointers), so it is up to the
// caller to cast the pointer back to a specific function pointer type before
// invoking it.

#include <bslscm_version.h>

#include <bslalg_nothrowmovableutil.h>

#include <bslma_constructionutil.h>
#include <bslma_stdallocator.h>

#include <bslmf_assert.h>
#include <bslmf_decay.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <bslstl_function_smallobjectoptimization.h>

#include <cstddef>
#include <cstdlib>
#include <typeinfo>

namespace BloombergLP {
namespace bslstl {

                        // ==================
                        // class Function_Rep
                        // ==================

class Function_Rep {
    // This is a component-private class.  Do not use.
    //
    // This class provides a non-template representation for a 'bsl::function'
    // instance.  It handles all of the object-management parts of
    // 'bsl::function' that are not specific to the prototype (argument list
    // and return type), e.g., storing, copying, and moving the 'function'
    // object, but not invoking the 'function' (which requires knowledge of the
    // prototype).  These management methods are run-time polymorphic, and
    // therefore do not require that this class be a template (although several
    // of the member functions are templates).

    // PRIVATE CONSTANTS
#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
  public:
    // Not really public: made public to work around a Sun compiler bug.
#endif
    static const std::size_t k_NON_SOO_SMALL_SIZE =
                bslstl::Function_SmallObjectOptimization::k_NON_SOO_SMALL_SIZE;

  private:
    // PRIVATE TYPES
    enum ManagerOpCode {
        // Function manager opcode enumerators.  See documentation for the
        // 'functionManager' function template (below).

        e_MOVE_CONSTRUCT  ,
        e_COPY_CONSTRUCT  ,
        e_DESTROY         ,
        e_DESTRUCTIVE_MOVE,
        e_GET_SIZE        ,
        e_GET_TARGET      ,
        e_GET_TYPE_ID
    };

    union ManagerRet {
        // This union stores either a pointer or a 'size_t'.  It is used as
        // the return type for the manager function (below).

      private:
        // DATA
        std::size_t  d_asSize_t;
        void        *d_asPtr_p;

      public:
        // CREATORS
        ManagerRet(std::size_t s);                                  // IMPLICIT
            // Create a union holding the specified 's' 'size_t'.

        ManagerRet(void *p);                                        // IMPLICIT
            // Create a union holding the specified 'p' pointer.

        // ACCESSORS
        operator std::size_t() const;
            // Return the 'size_t' stored in this union.  The behavior is
            // undefined unless this object was constructed with a 'size_t'.

        template <class TP>
        operator TP *() const;
            // Return the pointer stored in this union.  The behavior is
            // undefined unless this object was constructed with a 'TP *'.
    };

    template <class TYPE>
    struct Decay
    : bsl::decay<typename bslmf::MovableRefUtil::RemoveReference<TYPE>::type> {
        // Abbreviation for metafunction used to provide a C++03-compatible
        // implementation of 'std::decay' that treats 'bslmf::MovableReference'
        // as an rvalue reference.
    };

    typedef bslstl::Function_SmallObjectOptimization Soo;
    typedef Soo::InplaceBuffer                       InplaceBuffer;
    typedef bslma::Allocator                         Allocator;
        // Type aliases for convenience.

    // DATA
    mutable InplaceBuffer d_objbuf;
        // When wrapping a target object that qualifies for the small-object
        // optimization (as described in the
        // 'bslstl_function_smallobjectoptimization' component), this buffer
        // stores the in-place representation of the target; otherwise it
        // stores a pointer to allocated storage holding the target.

    bsl::allocator<char>  d_allocator;
        // Allocator used to supply memory

    ManagerRet          (*d_funcManager_p)(ManagerOpCode  opCode,
                                           Function_Rep  *rep_p,
                                           void          *srcFunc_vp);
        // Pointer to a specialization of the 'functionManager' function
        // template (below) used to manage the current target, or null for
        // empty objects.

    void                (*d_invoker_p)();
        // Pointer to the function used to invoke the current target, or null
        // for empty objects.  Note that this pointer is always set *after*
        // 'd_funcManager_p' (see state transition table, below).

    // The table below shows progression of states of the a 'Function_Rep' from
    // empty through having a fully constructed target.  The progression goes
    // in the reverse direction when setting the 'Function_Rep' back to empty.
    // The "target allocated" state is transient and occurs only while a target
    // is being installed; unless otherwise documented, all member functions
    // assume as a class invariant that an object is not in the "target
    // allocated" state.  This state *can* exist at destruction during
    // exception unwinding and is specifically handled correctly by the
    // destructor.
    //
    // d_funcManager_p d_invoker_p  Rep (d_objbuf) state
    // =============== ===========  ======================================
    // NULL            NULL         Initial (Empty)
    // non-NULL        NULL         Target allocated (transient)
    // non-NULL        non-NULL     Target constructed

    // PRIVATE CLASS METHODS
    template <class FUNC, bool INPLACE>
    static ManagerRet functionManager(ManagerOpCode  opCode,
                                      Function_Rep  *rep,
                                      void          *srcVoidPtr);
        // Apply the specified 'opCode' to the objects at the specified 'rep'
        // and 'srcVoidPtr' addresses and return a pointer or 'size_t' result.
        // If 'srcVoidPtr' is non-null, it should point to a callable object of
        // type indicated by template parameter 'FUNC'.  A pointer to an
        // instantiation of this function is stored in 'd_functionManager_p'
        // and is used to manage the target object wrapped in a 'Function_Rep'.
        // The 'FUNC' parameter must not be wrapped in a
        // 'NothrowMovableWrapper'.  The 'INPLACE' parameter should be 'true'
        // if and only if the target is allocated inplace within '*rep'.
        //
        // The following describes the behavior of each possible value for
        // 'opCode'.  In this description, *the* *target* refers to the object
        // wrapped within the representation at 'rep'.
        //
        //: 'e_MOVE_CONSTRUCT':
        //:   Move construct the target from the callable object at
        //:   'srcVoidPtr'.  Return the number of bytes needed to hold the
        //:   object.  The behavior is undefined unless memory has been
        //:   allocated for the target.
        //:
        //: 'e_COPY_CONSTRUCT':
        //:   Copy construct the target from the callable object at
        //:   'srcVoidPtr'.  Return the number of bytes needed to hold the
        //:   object.  The behavior is undefined unless memory has been
        //:   allocated for the target.
        //:
        //: 'e_DESTROY':
        //:   Call the destructor for the target object but do not deallocate
        //:   it.  Return the number of bytes needed to hold the destroyed
        //:   object.  The behavior is undefined unless '*rep' holds a target
        //:   of type 'FUNC'.
        //:
        //: 'e_DESTRUCTIVE_MOVE':
        //:   Move the object at 'srcVoidPtr' to the memory allocated for the
        //:   target and destroy the object at 'srcVoidPtr'.  Return the number
        //:   of bytes needed to hold the target.  This operation is
        //:   guaranteed not to throw.  If 'FUNC' is bitwise movable, perform
        //:   this move using 'memcpy'; otherwise invoke the move constructor
        //:   followed by the destructor.  The behavior is undefined unless
        //:   memory has been allocated for the target.  Note that this
        //:   operation is never invoked unless 'FUNC' has either the
        //:   'bslmf::BitwiseMoveable' or 'bsl::is_nothrow_move_constructible'
        //:   trait.  If 'bsl::is_nothrow_move_constructible<FUNC>' is true but
        //:   the move constructor throws anyway, the program is likely to
        //:   terminate.
        //:
        //: 'e_GET_SIZE':
        //:   Return the size of a target object of type 'FUNC', encoded using
        //:   the rules of the 'Soo::SooFuncSize' metafunction (see
        //:   {'bslstl_function_smallobjectoptimization'}).  The arguments
        //:   'rep' and 'srcVoidPtr' are not used.
        //:
        //: 'e_GET_TARGET':
        //:   If the 'srcVoidPtr' argument points to 'typeid(FUNC)' return a
        //:   pointer to the target object; otherwise return a null pointer.
        //:   The behavior is undefined unless '*rep' holds a target of type
        //:   'FUNC' and 'srcVoidPtr' points to a valid 'type_info' object.
        //:
        //: 'e_GET_TYPE_ID':
        //:   Return a pointer to the 'type_info' for a target object of type
        //:   'FUNC'.  The 'srcVoidPtr' argument is not used.
        //
        // Implementation note: Instantiations of this function implement a
        // kind of hand-coded virtual-function dispatch.  Internally, a
        // 'Manager' function uses a 'switch' statement rather than performing
        // a virtual-table lookup.  This mechanism was chosen because testing
        // showed that it saves a significant amount of generated code space
        // over the C++ virtual-function mechanism, especially when the number
        // of different instantiations of 'bsl::function' is large.

    // PRIVATE MANIPULATORS
    void allocateBuf(std::size_t sooFuncSize);
        // Initialize this object's 'd_objbuf' field, allocating enough storage
        // to hold a target of the specified 'sooFuncSize', which is encoded as
        // per the 'Soo::SooFuncSize' metafunction.  If the function qualifies
        // for the small-object optimization, then the storage comes from the
        // small-object buffer in 'd_objbuf'; otherwise it is obtained from
        // 'd_allocator' and 'd_objbuf.d_object_p' is set to the address of the
        // allocated block.  The target object is not initialized, nor is
        // 'd_funcManager_p' modified.

    template <class FUNC>
    void constructTarget(FUNC& func);
    template <class FUNC>
    void constructTarget(BSLMF_MOVABLEREF_DEDUCE(const FUNC) func);
        // Copy the specified callable object 'func' into this object's target
        // storage (either in-place within this object's small-object buffer or
        // out-of-place from the allocator).  The behavior is undefined unless
        // this object is in the target-allocated state for the 'func'; which
        // is when 'd_invoker_p == 0' and 'd_funcManager_p != 0'.

    template <class FUNC>
    void constructTarget(BSLMF_MOVABLEREF_DEDUCE(FUNC) func);
        // Move the specified callable object 'func' into this object's target
        // storage (either in-place within this object's small-object buffer or
        // out-of-place from the allocator).  The behavior is undefined unless
        // this object is in the target-allocated state for the 'func'; which
        // is when 'd_invoker_p == 0' and 'd_funcManager_p != 0'.

    // PRIVATE ACCESSORS
    std::size_t calcSooFuncSize() const BSLS_KEYWORD_NOEXCEPT;
        // Return the size of the target, encoded as per the 'Soo::SooFuncSize'
        // metafunction, or zero if this 'function' is empty.

    // NOT IMPLEMENTED
    Function_Rep(const Function_Rep&);
    Function_Rep& operator=(const Function_Rep&);

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;
        // This class does not conform to any specific interface so is not
        // allocator-aware in the strict sense.  However, this type does hold
        // an allocator for its AA client and therefore uses the type name
        // for the allocator preferred by AA types.

    typedef void                 GenericInvoker();
        // A "generic" function type analogous to the data type 'void' (though
        // without the language support provided by 'void').

    // CREATORS
    explicit Function_Rep(const allocator_type& allocator)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create an empty object using the specified 'allocator' to supply
        // memory.

    ~Function_Rep();
        // Destroy this object and its target object (if any) and deallocate
        // memory for the target object (if not in-place).  This destructor
        // is implemented to correctly deallocate a target object that has been
        // allocated but not constructed (e.g., if an exception is thrown
        // while constructing the target).

    // MANIPULATORS
    void copyInit(const Function_Rep& original);
        // Copy-initialize this rep from the specified 'original' rep.  If an
        // exception is thrown by the copy, the only valid subsequent operation
        // on this object is destruction.  The behavior is undefined unless
        // this object is empty before the call.

    template <class FUNC>
    void installFunc(BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) func,
                     GenericInvoker                          invoker);
        // Do nothing if the specified 'func' is a null pointer, otherwise
        // allocate storage (either in-place within this object's small-object
        // buffer or out-of-place from the allocator) to hold a target of
        // (template parameter) type 'FUNC', forward the 'func' to the
        // constructor of the new target, set 'd_funcManager_p' to manage the
        // new target, and set 'd_invoker_p' to the specified 'invoker'
        // address.  The behavior is undefined unless this object is empty on
        // entry.  Note that 'FUNC' will not qualify for the small-object
        // optimization unless
        // 'bsl::is_nothrow_move_constructible<FUNC>::value' is 'true'.

    void makeEmpty();
        // Change this object to be an empty object without changing its
        // allocator.  Any previous target is destroyed and deallocated.  Note
        // that value returned by 'get_allocator().mechanism()' might change,
        // but will point to an allocator with the same type managing the same
        // memory resource.

    void moveInit(Function_Rep *from);
        // Move-initialize this rep from the rep at the specified 'from'
        // address, leaving the latter empty.  If 'this->get_allocator() !=
        // from->get_allocator()', this function degenerates to a call to
        // 'copyInit(*from)'.  The behavior is undefined unless this rep is
        // empty before the call.

    void swap(Function_Rep& other) BSLS_KEYWORD_NOEXCEPT;
        // Exchange this object's target object, manager function, and invoker
        // with those of the specified 'other' object.  The behavior is
        // undefined unless 'this->get_allocator() == other->get_allocator()'.

    template<class TP> TP* target() const BSLS_KEYWORD_NOEXCEPT;
        // If 'typeid(TP) == this->target_type()', return a pointer offering
        // modifiable access to this object's target; otherwise return a null
        // pointer.  Note that this function is 'const' but returns a
        // non-'const' pointer because, according to the C++ Standard,
        // 'function' (and therefore this representation) does not adhere to
        // logical constness conventions.

    template<class TP, bool INPLACE> TP* targetRaw() const
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Return a pointer offering modifiable access to this object's target.
        // If 'INPLACE' is true, then the object is assumed to be allocated
        // inplace in the small object buffer.  Note that this function is
        // 'const' but returns a non-'const' pointer because this type does not
        // adhere to logical constness conventions.  The behavior is undefined
        // unless 'typeid(TP) == this->target_type()' and 'INPLACE' correctly
        // identifies whether the target is inplace.

    // ACCESSORS
    allocator_type get_allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return the allocator used to supply memory for this object.

    GenericInvoker *invoker() const BSLS_KEYWORD_NOEXCEPT;
        // Return a pointer the invoker function set using 'installFunc' or a
        // null pointer if this object is empty.

    bool isEmpty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if 'invoker()' is a null pointer, indicating that this
        // object has no target object.

    // The 'isInplace function is public in BDE legacy mode and private
    // otherwise.
#ifdef BDE_OMIT_INTERNAL_DEPRECATED
  private:
#endif
    bool isInplace() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if the target is allocated in place within the
        // small-object buffer of this object; otherwise return 'false'.

  public:
    const std::type_info& target_type() const BSLS_KEYWORD_NOEXCEPT;
        // Return a reference to the 'type_info' for the type of the current
        // target object or 'typeid(void)' if this object is empty.  If the
        // target type is a specialization of 'bslalg::NothrowMovableWrapper',
        // then the returned 'type_info' is for the unwrapped type.
};

}  // close package namespace

// ============================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // --------------------------------------
                        // class bslstl::Function_Rep::ManagerRet
                        // --------------------------------------

// CREATORS
inline
bslstl::Function_Rep::ManagerRet::ManagerRet(std::size_t s)
    : d_asSize_t(s)
{
}

inline
bslstl::Function_Rep::ManagerRet::ManagerRet(void *p)
    : d_asPtr_p(p)
{
}

// ACCESSORS
inline
bslstl::Function_Rep::ManagerRet::operator std::size_t() const
{
    return d_asSize_t;
}

template <class TP>
inline
bslstl::Function_Rep::ManagerRet::operator TP *() const
{
    return static_cast<TP*>(d_asPtr_p);
}

                        // --------------------------
                        // class bslstl::Function_Rep
                        // --------------------------

// PRIVATE CLASS METHODS
template <class FUNC, bool INPLACE>
bslstl::Function_Rep::ManagerRet
bslstl::Function_Rep::functionManager(ManagerOpCode  opCode,
                                      Function_Rep  *rep,
                                      void          *srcVoidPtr)
{
    using bslma::ConstructionUtil;

    // Assert that 'FUNC' is not wrapped.  It should have been unwrapped before
    // instantiating this template.
    BSLMF_ASSERT(! bslalg::NothrowMovableUtil::IsWrapped<FUNC>::value);

    // If 'FUNC' was allocated inplace despite having a throwing move
    // constructor, then 'INPLACE' will disagree with 'Soo::IsInplaceFunc'.  In
    // this case, use the raw size of 'FUNC' rather than the encoded size from
    // 'Soo'.
    static const std::size_t k_SOO_FUNC_SIZE =
        (INPLACE && ! Soo::IsInplaceFunc<FUNC>::value) ?
        sizeof(FUNC) : Soo::SooFuncSize<FUNC>::value;

    // If a function manager exists, then target must have non-zero size.
    BSLMF_ASSERT(0 != k_SOO_FUNC_SIZE);

    FUNC *target = rep->targetRaw<FUNC, INPLACE>();

    switch (opCode) {

      case e_MOVE_CONSTRUCT: {
        // Move-construct function object.  There is no point to optimizing
        // this operation for trivially movable types.  If the type is
        // trivially moveable, then the 'construct' operation below will do it
        // trivially.
        FUNC& original = *static_cast<FUNC *>(srcVoidPtr);
        ConstructionUtil::construct(target,
                                    rep->d_allocator.mechanism(),
                                    bslmf::MovableRefUtil::move(original));
      } break;

      case e_COPY_CONSTRUCT: {
        // Copy-construct function object.  There is no point to optimizing
        // this operation for bitwise copyable types.  If the type is trivially
        // copyable, then the 'construct' operation below will do it trivially.
        const FUNC& original = *static_cast<FUNC *>(srcVoidPtr);
        ConstructionUtil::construct(target,
                                    rep->d_allocator.mechanism(),
                                    original);
      } break;

      case e_DESTROY: {
        target->~FUNC();
      } break;

      case e_DESTRUCTIVE_MOVE: {
        FUNC *fromPtr = static_cast<FUNC*>(srcVoidPtr);
        ConstructionUtil::destructiveMove(target,
                                          rep->d_allocator.mechanism(),
                                          fromPtr);
      } break;

      case e_GET_SIZE: {
        return k_SOO_FUNC_SIZE;                                       // RETURN
      }

      case e_GET_TARGET: {
        std::type_info *expType = static_cast<std::type_info *>(srcVoidPtr);
        if (*expType != typeid(FUNC)) {
            // Wrapped type does not match expected type.
            return static_cast<FUNC*>(0);                             // RETURN
        }
        return target;                                                // RETURN
      }

      case e_GET_TYPE_ID: {
        // 'const_cast' needed for conversion to 'ManagerRet'.
        return const_cast<std::type_info*>(&typeid(FUNC));            // RETURN
      }
    } // end switch

    // Any case that doesn't return something explicitly, returns the size of
    // the target object by default.
    return k_SOO_FUNC_SIZE;
}

// PRIVATE MANIPULATORS
template <class FUNC>
void bslstl::Function_Rep::constructTarget(FUNC& func)
{
    BSLS_ASSERT_SAFE(0 != d_funcManager_p);
    BSLS_ASSERT_SAFE(0 == d_invoker_p);

    typedef typename Decay<FUNC>::type DecayedFunc;
    const DecayedFunc&                 decayedFunc = func;

    typedef bslalg::NothrowMovableUtil                        NMUtil;
    typedef typename NMUtil::UnwrappedType<DecayedFunc>::type UnwrappedFunc;
    const UnwrappedFunc& unwrappedFunc = NMUtil::unwrap(decayedFunc);

    d_funcManager_p(
        e_COPY_CONSTRUCT, this, &const_cast<UnwrappedFunc&>(unwrappedFunc));
}

template <class FUNC>
void bslstl::Function_Rep::constructTarget(BSLMF_MOVABLEREF_DEDUCE(const FUNC)
                                               func)
{
    BSLS_ASSERT_SAFE(0 != d_funcManager_p);
    BSLS_ASSERT_SAFE(0 == d_invoker_p);

    constructTarget(bslmf::MovableRefUtil::access(func));
}

template <class FUNC>
void bslstl::Function_Rep::constructTarget(BSLMF_MOVABLEREF_DEDUCE(FUNC) func)
{
    BSLS_ASSERT_SAFE(0 != d_funcManager_p);
    BSLS_ASSERT_SAFE(0 == d_invoker_p);

    typedef typename Decay<FUNC>::type DecayedFunc;
    DecayedFunc&                       decayedFunc = func;

    typedef bslalg::NothrowMovableUtil                        NMUtil;
    typedef typename NMUtil::UnwrappedType<DecayedFunc>::type UnwrappedFunc;
    UnwrappedFunc& unwrappedFunc = NMUtil::unwrap(decayedFunc);

    d_funcManager_p(e_MOVE_CONSTRUCT, this, &unwrappedFunc);
}

// PRIVATE ACCESSORS
inline
std::size_t bslstl::Function_Rep::calcSooFuncSize() const BSLS_KEYWORD_NOEXCEPT
{
    std::size_t ret = 0;

    if (d_funcManager_p) {
        ret = d_funcManager_p(e_GET_SIZE,
                              const_cast<Function_Rep*>(this), 0);
    }

    return ret;
}

// CREATORS
inline
bslstl::Function_Rep::Function_Rep(const allocator_type& allocator)
                                                          BSLS_KEYWORD_NOEXCEPT
    : d_allocator(allocator)
    , d_funcManager_p(0)
    , d_invoker_p(0)
{
}

// MANIPULATORS
template <class FUNC>
void bslstl::Function_Rep::installFunc(
                               BSLS_COMPILERFEATURES_FORWARD_REF(FUNC) func,
                               GenericInvoker                          invoker)
{
    if (! invoker) {
        // Leave this object in the empty state.
        return;                                                       // RETURN
    }

    typedef typename Decay<FUNC>::type DecayedFunc;

    // If 'FUNC' is wrapped in a 'bslalg::NothrowMovableWrapper', then the SOO
    // size calculation works as though 'FUNC' were nothrow movable.
    static const std::size_t k_SOO_FUNC_SIZE =
        Soo::SooFuncSize<DecayedFunc>::value;
    static const bool k_INPLACE = Soo::IsInplaceFunc<DecayedFunc>::value;

    allocateBuf(k_SOO_FUNC_SIZE);  // Might throw

    // Target rep was successfully allocated, set 'd_funcManager_p'.  If 'FUNC'
    // is wrapped in a 'NothrowMovableWrapper', then unwrap it first, but use
    // 'k_INPLACE' value for the original (potentially-wrapped) 'FUNC' so that
    // the function manager knows whether to expect the object to be inplace or
    // not.
    typedef
        typename bslalg::NothrowMovableUtil::UnwrappedType<DecayedFunc>::type
            UnwrappedFunc;
    d_funcManager_p = &functionManager<UnwrappedFunc, k_INPLACE>;

    // Copy or move the function argument into '*this' object.  Note that this
    // operation might throw.
    constructTarget(BSLS_COMPILERFEATURES_FORWARD(FUNC, func));

    // Exception danger has passed.  Setting the invoker makes the
    // 'Function_Rep' non-empty.
    d_invoker_p = invoker;
}

template <class TP>
inline
TP *bslstl::Function_Rep::target() const BSLS_KEYWORD_NOEXCEPT
{
    if (! d_funcManager_p) {
        return 0;                                                     // RETURN
    }

    const std::type_info& tpTypeInfo = typeid(TP);

    void *ret = d_funcManager_p(e_GET_TARGET,
                                const_cast<Function_Rep *>(this),
                                const_cast<std::type_info*>(&tpTypeInfo));

    // If 'TP' is a function (not pointer-to-function) type, 'ret' will be
    // null, but we still must use a C-style cast to avoid the compiler error
    // produced from converting a data pointer to a function pointer.
    return (TP *) ret;
}

template <class TP, bool INPLACE>
inline
TP *bslstl::Function_Rep::targetRaw() const BSLS_KEYWORD_NOEXCEPT
{
    // If target fits in 'd_objbuf', then it is inplace; otherwise, its
    // heap-allocated address is found in 'd_objbuf.d_object_p'.  There is no
    // need to dispatch using metaprogramming because the compiler will
    // optimize away the compile-time conditional test.
    return static_cast<TP*>(INPLACE ? &d_objbuf : d_objbuf.d_object_p);
}

// ACCESSORS
inline
bslstl::Function_Rep::allocator_type
bslstl::Function_Rep::get_allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return d_allocator;
}

inline
bslstl::Function_Rep::GenericInvoker *
bslstl::Function_Rep::invoker() const BSLS_KEYWORD_NOEXCEPT
{
    return d_invoker_p;
}

inline
bool bslstl::Function_Rep::isEmpty() const BSLS_KEYWORD_NOEXCEPT
{
    return 0 == d_invoker_p;
}

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLSTL_FUNCTION_REP)

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
