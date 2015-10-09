// bslstl_function.cpp                                                -*-C++-*-
#include <bslstl_function.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_platform.h>

#ifdef BDE_BUILD_TARGET_EXC

const char* bsl::bad_function_call::what() const BSLS_NOTHROW_SPEC
{
    return "bad_function_call";
}

#endif

#ifndef BSLS_PLATFORM_CMP_MSVC
const std::size_t bsl::Function_SmallObjectOptimization::k_NON_SOO_SMALL_SIZE;

const std::size_t bsl::Function_Rep::k_NON_SOO_SMALL_SIZE;
#endif

void *bsl::Function_Rep::initRep(std::size_t       sooFuncSize,
                                 Allocator        *alloc,
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

void bsl::Function_Rep::moveInit(Function_Rep& other)
{
    // This function is called only when it is known that '*this' will get its
    // allocator from 'other'.

    Manager allocManager_p = other.d_allocManager_p;

    // Initialize an empty function object with other's allocator.
    Function_Rep emptyRep;
    allocManager_p(e_INIT_REP, &emptyRep, other.d_allocator_p);

    destructiveMove(this, &other);
    destructiveMove(&other, &emptyRep);
}

void bsl::Function_Rep::makeEmpty()
{
    if (! d_funcManager_p) {
        // Already empty.  Nothing to do.
        return;                                                       // RETURN
    }

    // Call destructor on wrapped functor.
    std::size_t sooFuncSize = d_funcManager_p(e_DESTROY, this,
                                              PtrOrSize_t()).asSize_t();
    d_funcManager_p = NULL;

    if (sooFuncSize <= sizeof(InplaceBuffer)) {
        return;                                                       // RETURN
    }

    Allocator *fromAlloc = d_allocator_p;
    if (d_allocManager_p == &unownedAllocManager) {
        // Was out-of-place functor with unowned allocator.
        // Deallocate functor.  (Allocator is unaffected.)
        d_allocator_p->deallocate(d_objbuf.d_object_p);
        return;                                                       // RETURN
    }

    // Allocator is type-erased and stored in the same allocated memory block
    // as the (destructed) functor
    void *memoryBlock = d_objbuf.d_object_p;

    // Allocator is at some offset into 'd_object_p' memory block.  Move
    // allocator to front (offset 0) of memory block.  Do not try to allocate
    // a new memory block (could throw) nor deallocate existing memory block
    // (because it's still in use).  Note that the allocator manager ensures
    // that destructive move works correctly even if the old an new locations
    // overlap.
    d_allocator_p = static_cast<Allocator*>(memoryBlock);
    d_allocManager_p(e_DESTRUCTIVE_MOVE, this, fromAlloc);
}

bsl::Function_Rep::PtrOrSize_t
bsl::Function_Rep::unownedAllocManager(ManagerOpCode  opCode,
                                       Function_Rep  *rep,
                                       PtrOrSize_t    input)
{
    switch (opCode) {
      case e_MOVE_CONSTRUCT: // Fall through: allocators are always copied
      case e_COPY_CONSTRUCT: {
        // Allocator pointer was already copied.
        // No copy/move construction needed.
        BSLS_ASSERT(rep->d_allocator_p == input.asPtr());
        return rep->d_allocator_p;                                    // RETURN
      } break;

      case e_DESTROY: {
        // Allocator is unowned, so don't destroy it.
        // Deallocate storage used by functor, if any.
        std::size_t sooFuncSize = input.asSize_t();
        if (sooFuncSize > sizeof(InplaceBuffer)) {
            rep->d_allocator_p->deallocate(rep->d_objbuf.d_object_p);
        }
        return PtrOrSize_t();                                         // RETURN
      } break;

      case e_DESTRUCTIVE_MOVE: {
        // Move just the pointer for unowned allocator.
        rep->d_allocator_p = static_cast<Allocator*>(input.asPtr());
      } break;

      case e_GET_SIZE:     return PtrOrSize_t();                      // RETURN
      case e_GET_TARGET:   return rep->d_allocator_p;                 // RETURN
      case e_GET_TYPE_ID:
        return const_cast<std::type_info*>(&typeid(Allocator));       // RETURN

      case e_IS_EQUAL: {
        const Allocator *inputAlloc =
            static_cast<const Allocator *>(input.asPtr());
        return inputAlloc == rep->d_allocator_p;                      // RETURN
      } break;

      case e_INIT_REP: {
        Allocator *inputAlloc = static_cast<Allocator *>(input.asPtr());

        std::size_t sooFuncSize = rep->d_funcManager_p ?
              rep->d_funcManager_p(e_GET_SIZE, rep,
                                   PtrOrSize_t()).asSize_t() : 0;

        rep->initRep(sooFuncSize, inputAlloc,
                     integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>());

      } break;

    } // end switch

    return PtrOrSize_t();
}

void bsl::Function_Rep::assign(Function_Rep *rhs_p, ManagerOpCode moveOrCopy)
{
    Function_Rep tempRep;

    tempRep.d_funcManager_p = rhs_p->d_funcManager_p;
    tempRep.d_invoker_p     = rhs_p->d_invoker_p;

    // Initialize tempRep using allocator from 'this'
    this->d_allocManager_p(e_INIT_REP, &tempRep, this->d_allocator_p);

    // Move function into initialized tempRep.
    if (tempRep.d_funcManager_p) {
        PtrOrSize_t source = rhs_p->d_funcManager_p(e_GET_TARGET, rhs_p,
                                                    PtrOrSize_t());
        tempRep.d_funcManager_p(moveOrCopy, &tempRep, source);
    }

    // If functor construction into 'tempRep' was successful (no exceptions
    // thrown), swap 'tempRep' into '*this'.
    this->swap(tempRep);

    if (tempRep.d_funcManager_p) {
        // Destroy the functor in 'tempRep' before 'tempRep' goes out of scope
        tempRep.d_funcManager_p(e_DESTROY, &tempRep, PtrOrSize_t());
    }
}

void bsl::Function_Rep::destructiveMove(Function_Rep *to,
                                        Function_Rep *from) BSLS_NOTHROW_SPEC
{
    Manager funcManager_p  = from->d_funcManager_p;
    Manager allocManager_p = from->d_allocManager_p;
    to->d_funcManager_p  = funcManager_p;
    to->d_allocManager_p = allocManager_p;
    to->d_allocator_p    = from->d_allocator_p;
    to->d_invoker_p      = from->d_invoker_p;

    if (funcManager_p) {
        // '*from' is not empty.
        // Move the wrapped functor.
        if (from->isInplace()) {
            // '*from' uses the small-object optimization.
            // Destructively move inplace wrapped functor.
            funcManager_p(e_DESTRUCTIVE_MOVE, to, &from->d_objbuf);
        }
        else {
            // '*from' does not use the small-object optimization.
            // Just move pointer for out-of-place wrapped functor.
            to->d_objbuf.d_object_p = from->d_objbuf.d_object_p;
        }
    }

    // Make 'from' destructible.
    from->d_invoker_p = 0;
    from->d_allocManager_p = 0;
}

bsl::Function_Rep::~Function_Rep()
{
    // Wrapped functor must already have been destroyed.  It cannot be
    // destroyed in this destructor because a 'Function_Rep' may exist with a
    // wrapped function in either a constructed or uninitialized state.  Only
    // the 'function<F>' derived class knows whether the wrapped function
    // needs to be destroyed, especially during exception unwinding.

    if (d_allocManager_p) {
        BSLS_ASSERT(d_allocator_p);
        PtrOrSize_t sooFuncSize;  // Defaults to zero

        if (d_funcManager_p) {
            sooFuncSize = d_funcManager_p(e_GET_SIZE, this, PtrOrSize_t());
        }

        d_allocManager_p(e_DESTROY, this, sooFuncSize);
    }
}

void bsl::Function_Rep::swap(Function_Rep& other) BSLS_NOTHROW_SPEC
{
    // Swap will fail if allocators are not compatible
    // TBD: Should this be a 'BSLS_ASSERT_SAFE'?
    BSLS_ASSERT(d_allocManager_p(e_IS_EQUAL, this,
                                 other.d_allocator_p).asSize_t());

    BloombergLP::bsls::ObjectBuffer<Function_Rep> temp;
    destructiveMove(&temp.object(), &other);
    destructiveMove(&other, this);
    destructiveMove(this, &temp.object());
}

const std::type_info&
bsl::Function_Rep::target_type() const BSLS_NOTHROW_SPEC
{
    if (! d_funcManager_p) {
        return typeid(void);                                          // RETURN
    }

    PtrOrSize_t ret = d_funcManager_p(e_GET_TYPE_ID,
                                      const_cast<Function_Rep*>(this),
                                      PtrOrSize_t());
    return *static_cast<const std::type_info*>(ret.asPtr());
}

bool bsl::Function_Rep::isInplace() const BSLS_NOTHROW_SPEC
{
    if (d_funcManager_p) {
        std::size_t sooFuncSize =
            d_funcManager_p(e_GET_SIZE, const_cast<Function_Rep*>(this),
                            PtrOrSize_t()).asSize_t();
        return sooFuncSize <= sizeof(InplaceBuffer);                  // RETURN
    }
    else {
        // Always return true when invoked on an empty object.
        return true;                                                  // RETURN
    }
}

void bsl::Function_Rep::nothing(...)
{
}

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
