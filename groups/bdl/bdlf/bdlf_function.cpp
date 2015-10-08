// bdlf_function.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlf_function.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlf_function_cpp,"$Id$ $CSID$")
//
// IMPLEMENTATIONS NOTES:  This very interesting component uses several known
// techniques to limit template bloat and curb memory allocations.  We detail
// them below.  In addition, the 'bdlf::Function_Rep' constructors initialize
// their manager *after* the invocable creation (in-place or out-of-place)
// because when an exception is thrown by the 'bdlf::Function_Rep' ctor in the
// *copy-assignment* operator, the object must still be in a destroyable state
// (and if the manager pointer was set, it would attempt to destroy a 'FUNC'
// object that was never constructed).
//
///SHORT SUMMARY OF TECHNIQUES USED IN 'bdlf_function'
///---------------------------------------------------
///Short-object optimization:
///--------------------------
// Also used in some 'bsl::string' implementations, this technique allows to
// store an object of variable size in-place when the size is smaller than some
// threshold.  Thus an object can have two representations, in-place and
// out-of-place.  Here it is used in the 'bdlf::Function_Rep' class, where the
// 'StorageArena' can store an object (in-place) or a pointer to an object
// (out-of-place).  This also allows us to have two managers, one for each
// representation, with only one of them actually being instantiated per each
// 'FUNC' type.
//
///Bitwise-copyable/moveable trait:
///--------------------------------
// In addition to potential performance gains (for small bitwise-copyable
// objects 'memcpy' is not necessarily faster than a copy ctor, however for
// bitwise-moveable types, it may elide a copy-construction followed by a
// destruction), we also use the trait to factor out the type in the manager,
// by providing a single manager for a functor size, since all implementations
// are equivalent to a bitwise copy.  This technique is known as "template
// hoisting" (removing -- or lessening, as in here -- the dependence of a type
// on its template parameter by factoring several types on a single
// implementation).
//
///Type erasure (a.k.a. static polymorphism):
///------------------------------------------
// Also used in 'bcema_sharedptr' (for aliasing and for the deleter type), this
// technique allows to provide polymorphic representations for the same target
// type without making the actual type (stored in the representation) a
// template parameter of the target type.  Thus a 'bdlf::Function' object can
// store many invocables of a parameterized 'FUNC' type, but 'FUNC' is not part
// of the 'bdlf::Function' or 'bdlf::Function_Rep' types.  The downside of this
// erasure of the 'FUNC' type is that it is impossible to retrieve the
// invocable from a 'bdlf::Function' object unless one knows the 'FUNC' type a
// priori.  (The C++0x standard offers a member function 'target_type'
// returning 'bsl::type_info' as well as a member function template
// 'FUNC *target<FUNC>' for its 'bsl::function' implementation for that
// purpose; note that 'FUNC' is then provided by the caller, and that if the
// invocable actually stored in the representation differs, 'target' returns
// 0i, i.e., a null pointer.)
//
// This technique is used twice in 'bdlf_function': once for the representation
// and once for the invoker.  A notable feature of the 'bdlf::Function_Rep'
// class is that it does not know the actual type of the embedded function
// object, hence the use of a manager.  The actual parameterized 'FUNC' type of
// the actual function object instance is known only to the manager which takes
// 'FUNC' objects by 'void *' address and provides the basic manipulation for
// the 'FUNC' type.  In this way, 'bdlf::Function_Rep' instances can manipulate
// their stored invocables at the cost of an extra indirection (manager
// function call).  Note that all manager functions have the same signature,
// which does *not* depend on 'FUNC', and although there are only four
// implementation templates (parameterized by 'FUNC'), the instantiation of
// these templates can generate any number of manager functions (a different
// one for each 'FUNC' type).  Note that taking the address of the manager in
// the 'bdlf::Function_Rep' constructors triggers the instantiation of these
// manager functions.
//
// Likewise, the invoker function pointer in the 'bdlf::Function' class points
// to a function whose type does not depend on 'FUNC', but the actual function
// that it points to is a static member function template of a class
// parameterized by 'FUNC'.  Thus, the invocable stored in the
// 'bdlf::Function_Rep' representation (of the 'FUNC' type) can be retrieved
// and invoked safely.  This mechanism relies on the fact that the function
// object representation 'd_rep' of the 'bdlf::Function_Rep' type was
// initialized with a constructor parameterized by the same 'FUNC' type as was
// used to retrieve the invoker function pointer.
//
///Static dispatching:
///-------------------
// A lot of the dispatching (calling a different function based on whether the
// 'FUNC' type can be stored in-place or requires out-of-place representation,
// on whether it has pointer semantics or not, is an allocator or not) is
// performed at compile-time, which is necessary one, to avoid runtime checks,
// and two, to avoid instantiating code that would trigger compilation errors.
// (For instance, a runtime test:
//..
//      if (isAllocator<FUNC>::VALUE) {
//          // Code that would be valid only if 'FUNC' is convertible to
//          // 'bslma::Allocator *'.
//      } else {
//          // Code that would be valid only if 'FUNC' is invocable in a way
//          // compatible with the 'PROTOTYPE'.
//      }
//..
// is sure to trigger to compilation error because both branches need to be
// instantiated.)
//
// Special care was taken to ensure that multiple dispatch was avoided, by
// collapsing all the compile-time decisions into a single (enumerated) list of
// possibilities.  Branching was performed by using an extra
// 'bslmf::Tag<VALUE>' argument for overload resolution.
//
///Const-correctness:
///------------------
// 'const'-correctness is broken for 'bdlf_function', as it is also in the
// C++0x standard for 'bsl::function'.  Namely, it is possible to invoke a
// non-modifiable function object containing an in-place representation of an
// invocable that modifies its state upon invocation.  Thus the state of the
// function object changes even though the invocation references a
// non-modifiable instance.  This is a direct result of the decision to have
// invocations as accessors only.  Note that having them as both accessors and
// manipulators would resolve this issue (along with constructors that take
// 'FUNC&' as well as 'const FUNC&', and various other minor adjustments), but
// would also (without extra discipline by the programmer, such as adding casts
// to 'const&' whenever appropriate to strictly match the type system) increase
// the amount of code instantiated.  As a result, we follow the existing
// (incorrect) practice and add either a 'mutable' to the 'd_arena' member of
// 'bdlf::Function_Rep' or a 'const_cast' in the 'invocable' member function
// for the 'IN_PLACE_WITH/WITHOUT_POINTER_SEMANTICS' tags.  In conversation to
// the author of 'bsl::function', Doug Gregor says:
//..
// As with a pointer, the constness of the wrapper does not necessarily
// have to affect the constness of the wrapped object.  Invocation of the
// function object does not modify the wrapper, so we invoke the function
// object as non-'const' [*] even though 'bsl::function::operator()' is
// const.  It's like dereferencing an 'int *' const and getting a
// (non-'const') int.
// [*] You can actually end up invoking a "const" target object by using
// 'bsl::reference_wrapper<const F>'.
//
// With the 'target' functions, we decided that they were much closer to
// getting at actually state within the wrapper (since we have a pointer
// into its stored target object), so the const-ness of the wrapper does
// affect the const-ness of the result.
//
// [In addition,] the small object optimization is an implementation detail
// that is not (strictly!) necessary, so we don't worry that it would
// require 'mutable'.
//..
//
///Value-semantic for 'bdlf::Function':
///-----------------------------------
// Unfortunately, comparison between 'bdlf::Function' objects cannot be
// implemented "well", and therefore in not implemented.  The typical
// semantics requested for comparing two 'bdlf::Function' objects 'f' and 'g'
// are:
//
// - If 'f' and 'g' store function objects of the same type, use that type's
//   equality comparison operator to compare them.
//
// - If 'f' and 'g' store function objects of different types, return 'false'.
//
// The problem occurs when the type of the function objects stored by both f
// and g does not have an equality comparison operator: we would like the
// expression 'f == g' to fail to compile, as occurs with, e.g., the standard
// containers.  However, this is not implementable for 'bdlf::Function' because
// it necessarily "erases" some type information after it has been assigned a
// function object, so it cannot try to call 'operator==' later: it must either
// find a way to call 'operator==' now, or it will never be able to call it
// later.  Note, for instance, what happens if you try to put a float value
// into a 'bdlf::Function' object: you will get an error at the assignment
// operator or constructor, not in 'operator()', because the function-call
// expression must be bound in the constructor or assignment operator.
//
///Assignment optimization:
///------------------------
// When assigning a 'FUNC' instance to a 'bdlf::Function' object, if the
// invocable object currently contained is also of type 'FUNC', it would be
// more efficient to use 'FUNC' assignment rather than destruction/creation, as
// is done.  The problem is that not all 'FUNC' types can be assumed to be
// assignable (user-defined types containing 'const' or reference members are
// not, for instance), and for those types the manager using an OpCode
// 'COPY_ASSIGN' would not compile.
//
// Note: with a 'bslmf_enableif' component, we could provide a manager that
// optimizes copy-assignment for assignable 'FUNC' types, and uses
// destroy/copy-construct otherwise.  In addition, using a 'COPY_ASSIGN'
// op-code in the manager and in the code below would also optimize the
// deallocation/reallocation that would still be done when the 'FUNC' type is
// not-assignable and not in-place.

#include <bdlf_memfn.h>            // for testing only

#include <bsls_assert.h>

#include <bsl_algorithm.h>

namespace BloombergLP {

namespace bdlf {                          // ------------------------
                          // struct Function_Rep
                          // ------------------------

// CREATORS
Function_Rep::Function_Rep(const Function_Rep&  original,
                           bslma::Allocator    *allocator)
: d_manager_p(original.d_manager_p)
, d_allocator_p(bslma::Default::allocator(allocator))
{
    if (d_manager_p) {
        (void)(*d_manager_p)(this,
                             (const void *)&original,
                             e_COPY_CONSTRUCT);
    }
    else {
        d_arena.d_func_p = original.d_arena.d_func_p;
    }
}

Function_Rep::~Function_Rep()
{
    if (d_manager_p) {
        (void)(*d_manager_p)(this, (const void *)0, e_DESTROY);
        d_manager_p = 0;
    }
}

// MANIPULATORS
Function_Rep&
Function_Rep::operator=(const Function_Rep& rhs)
{
    Function_Rep(rhs, d_allocator_p).swap(*this);
    return *this;
}

void Function_Rep::clear()
{
    if (d_manager_p) {
        (void)(*d_manager_p)(this, (const void *)0, e_DESTROY);
    }
    else {
        d_arena.d_func_p = 0;
    }
    d_manager_p = 0;
}

void Function_Rep::swap(Function_Rep& other)
{
    // Note: we systematically eliminate all opportunities for optimization.
    // It's better to do it here, instead of in the manager (which is a
    // template) when the optimization does not depend on the actual 'FUNC'
    // type.  This will limit template bloat.  Note that despite complicated
    // structure, maximum test depth is four and calls to the manager are after
    // at least three tests always.  At most one call to the manager is made
    // for 'SWAP' or 'TRANSFER', and prior to that at most two calls for
    // 'IN_PLACE_DETECTION' (and only when that has a chance of bringing
    // significant gains).

    if (!d_manager_p) {
        if (!other.d_manager_p) {
            // In the simplest case (two function pointers), two tests and a
            // pointer swap.

            using bsl::swap;
            swap(other.d_arena.d_func_p, d_arena.d_func_p);
            return;                                                   // RETURN
        }

        // Else 'this' is empty, transfer 'other' to this.

        void (*func)() = d_arena.d_func_p;
        if (other.d_allocator_p == d_allocator_p) {
            // Potential for huge gain (if large object, no need to copy).

            (void)(*other.d_manager_p)(this,
                                       (const void *)&other,
                                       e_MOVE_CONSTRUCT);
            d_manager_p            = other.d_manager_p;
            other.d_arena.d_func_p = func;
            other.d_manager_p      = 0;
            return;                                                   // RETURN
        }

        // Else need to know 'FUNC' type for the transfer, invoke manager.

        (void)(*other.d_manager_p)(this,
                                   (const void *)&other,
                                   e_COPY_CONSTRUCT);
        d_manager_p            = other.d_manager_p;

        (void)(*other.d_manager_p)(&other, (const void *)0, e_DESTROY);
        other.d_arena.d_func_p = func;
        other.d_manager_p      = 0;
        return;                                                       // RETURN
    }

    // Else 'this' is not empty.

    if (!other.d_manager_p) {
        // If 'other' is empty, transfer 'this' to 'other'.

        void (*func)() = other.d_arena.d_func_p;
        if (other.d_allocator_p == d_allocator_p) {
            // Same potential for huge gain (symmetric to above).

            (void)(*d_manager_p)(&other,
                                 (const void *)this,
                                 e_MOVE_CONSTRUCT);
            other.d_manager_p = d_manager_p;
            d_arena.d_func_p   = func;
            d_manager_p        = 0;
            return;                                                   // RETURN
        }

        // Need to know 'FUNC' type for the transfer, invoke manager.

        (void)(*d_manager_p)(&other, (const void *)this, e_COPY_CONSTRUCT);
        other.d_manager_p = d_manager_p;

        (void)(*d_manager_p)(this, (const void *)0, e_DESTROY);
        d_arena.d_func_p   = func;
        d_manager_p        = 0;
        return;                                                       // RETURN
    }

    // Else neither 'this' nor 'other' are empty (nor function pointers).

    if (d_allocator_p == other.d_allocator_p) {
        bool repInplace = (*d_manager_p)(0, 0, e_IN_PLACE_DETECTION);
        bool srcInplace = (*other.d_manager_p)(0, 0, e_IN_PLACE_DETECTION);

        if (!repInplace && !srcInplace) {
            // Quickswap: exchange object pointers and managers.

            using bsl::swap;
            swap(d_arena.d_object_p, other.d_arena.d_object_p);
            swap(d_manager_p, other.d_manager_p);
            return;                                                   // RETURN
        }
    }
    else {
        // Non-equal allocators, swap with temporary copies.

        Function_Rep tempThis(*this, other.d_allocator_p);
        Function_Rep tempOther(other, d_allocator_p);

        tempThis.swap(other);
        tempOther.swap(*this);
        return;                                                       // RETURN
    }

    // Exhausted all possibilities for optimizations, do a three-way swap.
    // Make sure that 'temp' uses the 'other' allocator so that move-construct
    // can be optimized if out-of-place.  Note that 'temp' will not need
    // destruction so we could wrap it into an object buffer, but it's peanuts
    // at this point.

    // need equal allocators to make moves no-throw

    BSLS_ASSERT(d_allocator_p == other.d_allocator_p);

    Function_Rep temp(other.d_allocator_p);

    (*d_manager_p)(&temp, (const void *)this, e_MOVE_CONSTRUCT);
    (*other.d_manager_p)(this, (const void *)&other, e_MOVE_CONSTRUCT);
    (*d_manager_p)(&other, (const void *)&temp, e_MOVE_CONSTRUCT);

    temp.d_manager_p = 0;  // clear the temp object

    using bsl::swap;
    swap(d_manager_p, other.d_manager_p);
}

void Function_Rep::transferTo(Function_Rep *target)
{
    BSLS_ASSERT(this != target);

    // As in 'swap', we systematically eliminate all opportunities for
    // optimization, to avoid template bloat.

    // In any case, need to clean up 'target' prior to transfer.

    if (target->d_manager_p) {
        (void)(*target->d_manager_p)(target, (const void *)0, e_DESTROY);
    }

    // For function pointers, only need to clean up 'target' and assign.

    if (!d_manager_p) {
        target->d_arena.d_func_p = d_arena.d_func_p;
        target->d_manager_p      = 0;
        d_arena.d_func_p         = 0;
        return;                                                       // RETURN
    }

    // Else do transfer.  Move-construct will optimize if possible, or simply
    // copy-construct and destroy the source if not possible.

    if (d_allocator_p == target->d_allocator_p) {
        (void)(*d_manager_p)(target, (const void *)this, e_MOVE_CONSTRUCT);
    }
    else {
        (void)(*d_manager_p)(target, (const void *)this, e_COPY_CONSTRUCT);
        (void)(*d_manager_p)(this, (const void *)0, e_DESTROY);
    }

    target->d_manager_p = d_manager_p;
    d_arena.d_func_p = 0;
    d_manager_p      = 0;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
