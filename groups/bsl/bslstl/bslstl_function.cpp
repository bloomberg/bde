// bslstl_function.cpp                  -*-C++-*-

#include <bslstl_function.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

const char* bsl::bad_function_call::what() const BSLS_NOTHROW_SPEC
{
    return "bad_function_call";
}

bool bsl::Function_Rep::moveInit(Function_Rep& other)
{
    // This function is called only when it is known that '*this' will get
    // its allocator from 'other'.

    bool inplace = true;

    d_funcManager_p = other.d_funcManager_p;

    if (d_funcManager_p) {
        std::size_t sooFuncSize = d_funcManager_p(e_GET_SIZE, &other,
                                                  PtrOrSize_t()).asSize_t();

        if (sooFuncSize <= sizeof(InplaceBuffer)) {
            // Function is inplace.

            // Initialize the rep using other's allocator.
            other.d_allocManager_p(e_INIT_REP, this, other.d_allocator_p);

            // Move-construct function.  This is a nothrow operation because
            // only nothrow-moveable functors are inplace.
            PtrOrSize_t source =
                d_funcManager_p(e_GET_TARGET,
                                const_cast<Function_Rep*>(&other),
                                PtrOrSize_t());
            d_funcManager_p(e_MOVE_CONSTRUCT, this, source);
        }
        else {
            // Function is not inplace.
            // Just move the pointers from other.
            inplace = false;

            // Pointerwise move contents of 'other' into '*this'
            d_objbuf.d_object_p = other.d_objbuf.d_object_p;
            d_allocManager_p    = other.d_allocManager_p;
            d_allocator_p       = other.d_allocator_p;

            // Set 'other' to be empty, but with its original allocator.

            // Before making any changes to 'other', allocate space (if
            // needed) for a copy of the allocator.  That way, if the
            // allocation fails with an exception, nothing will be altered.
            // The allocator will be separately allocated if it is too big to
            // fit (by itself) into the small object buffer.
            if (d_allocManager_p != &unownedAllocManager) {
                // Allocator is owned by this 'function'.  Check to see if we
                // will be using the small object optimization for the
                // allocator.
                std::size_t allocSize = d_allocManager_p(e_GET_SIZE, this,
                                                     PtrOrSize_t()).asSize_t();
                if (allocSize > sizeof(InplaceBuffer)) {
                    // Allocator will be stored out-of-place.  Allocate space.
                    other.d_allocator_p = static_cast<bslma::Allocator*>(
                        d_allocator_p->allocate(allocSize));
                }
                else {
                    // Allocator will be stored inplace.
                    other.d_allocator_p = reinterpret_cast<bslma::Allocator*>(
                        &other.d_objbuf);
                }
                // Construction of allocator is a nothrow operation.
                d_allocManager_p(e_COPY_CONSTRUCT, &other, d_allocator_p);
            }
            // Else (if unowned allocator) leave allocator pointer unchanged.

            // If got here, allocator has been successfully constructed.
            other.d_funcManager_p     = NULL;
        }
    }
    else {
        // Moving an empty 'function' object.
        // Initialize just the allocator portion of the result
        other.d_allocManager_p(e_INIT_REP, this, other.d_allocator_p);
    }

    return inplace;
}

void bsl::Function_Rep::makeEmpty()
{
    if (! d_funcManager_p) {
        // Already empty.  Nothing to do.
        return;
    }

    // Call destructor on wrapped functor.
    std::size_t sooFuncSize = d_funcManager_p(e_DESTROY, this,
                                              PtrOrSize_t()).asSize_t();
    d_funcManager_p = NULL;

    std::size_t allocSize = d_allocManager_p(e_GET_SIZE, this,
                                             PtrOrSize_t()).asSize_t();
    bslma::Allocator *fromAlloc = d_allocator_p;

    if (sooFuncSize <= sizeof(InplaceBuffer)) {
        // Was inplace functor.  Check if allocator can be deallocated.
        if (allocSize <= sizeof(InplaceBuffer) &&
            sooFuncSize + allocSize > sizeof(InplaceBuffer)) {
            // Allocator was out-of-place, but now fits inplace.  Move
            // out-of-place allocator to inplace and deallocate previous
            // allocator.
            d_allocator_p = reinterpret_cast<bslma::Allocator*>(&d_objbuf);
            d_allocManager_p(e_DESTRUCTIVE_MOVE, this, fromAlloc);
            d_allocator_p->deallocate(fromAlloc);
        }
        return;
    }

    // assert(functor is out-of-place)

    if (d_allocManager_p == &unownedAllocManager) {
        // Was out-of-place functor with unowned allocator.
        // Deallocate functor. (Allocator is unaffected.)
        d_allocator_p->deallocate(d_objbuf.d_object_p);
        return;
    }

    // assert(functor and allocator are both out-of-place)

    // Memory block where (destructed) functor and (valid) allocator live.
    void *memoryBlock = d_objbuf.d_object_p;

    if (allocSize <= sizeof(InplaceBuffer)) {
        // Allocator now inplace now that functor is gone.
        // Move out-of-place allocator to inplace.
        d_allocator_p = reinterpret_cast<bslma::Allocator*>(&d_objbuf);
        d_allocManager_p(e_DESTRUCTIVE_MOVE, this, fromAlloc);
        // Deallocate memory block that contained functor and allocator.
        d_allocator_p->deallocate(memoryBlock);
    }
    else {
        // Allocator is at some offset into 'd_object_p' memory block.  Move
        // allocator to front (offset 0) of memory block.  Do not try to
        // allocate a new memory block (could throw) nor deallocate existing
        // memory block (because it's still in use).
        d_allocator_p = static_cast<bslma::Allocator*>(memoryBlock);
        d_allocManager_p(e_DESTRUCTIVE_MOVE, this, fromAlloc);
    }
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
        return rep->d_allocator_p;
      } break;

      case e_DESTROY: {
        std::size_t sooFuncSize = input.asSize_t();
        if (sooFuncSize > sizeof(InplaceBuffer)) {
            rep->d_allocator_p->deallocate(rep->d_objbuf.d_object_p);
        }
        return PtrOrSize_t();
      } break;

      case e_DESTRUCTIVE_MOVE: {
        // Move just the pointer for unowed allocator.
        rep->d_allocator_p =
            static_cast<bslma::Allocator*>(input.asPtr());
      } break;

      case e_GET_SIZE:     return PtrOrSize_t();
      case e_GET_TARGET:   return rep->d_allocator_p;
      case e_GET_TYPE_ID:
        return const_cast<std::type_info*>(&typeid(bslma::Allocator));

      case e_IS_EQUAL: {
        const bslma::Allocator *inputAlloc =
            static_cast<const bslma::Allocator *>(input.asPtr());
        return inputAlloc == rep->d_allocator_p;
      } break;

      case e_INIT_REP: {
          bslma::Allocator *inputAlloc =
              static_cast<bslma::Allocator *>(input.asPtr());

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

    std::size_t sooFuncSize = 0;
    std::size_t allocSize   = 0;

    if (funcManager_p) {
        // Destructively move the wrapped functor only if '*from' is not empty
        // AND it uses the small-object optimization.
        sooFuncSize = funcManager_p(e_GET_SIZE, from,
                                    PtrOrSize_t()).asSize_t();
        if (sooFuncSize <= sizeof(InplaceBuffer)) {
            // Destructively move inplace wrapped functor.
            funcManager_p(e_DESTRUCTIVE_MOVE, to, &from->d_objbuf);
        }
        else {
            // Just move pointer for out-of-place wrapped functor.
            to->d_objbuf.d_object_p = from->d_objbuf.d_object_p;

            // Short-cut: if functor is out-of-place, the allocator is, too.
            // Just copy allocator pointer and return.
            to->d_allocator_p = from->d_allocator_p;
            return;
        }
    }

    BSLS_ASSERT(sooFuncSize <= sizeof(InplaceBuffer));

    allocSize = allocManager_p(e_GET_SIZE, from, PtrOrSize_t()).asSize_t();
    if (sooFuncSize + allocSize <= sizeof(InplaceBuffer)) {
        // Destructively move inplace allocator.  If unowned, then
        // this move operation will just overwrite 'd_allocator_p'.
        Function_PairBufDesc pairDesc(sooFuncSize, allocSize);
        to->d_allocator_p =
            static_cast<bslma::Allocator*>(pairDesc.second(&to->d_objbuf));
        allocManager_p(e_DESTRUCTIVE_MOVE, to, from->d_allocator_p);
    }
    else {
        // Just move pointer to out-of-place or unowned allocator.
        to->d_allocator_p = from->d_allocator_p;
    }
}

bsl::Function_Rep::~Function_Rep()
{
    // Wrapped function must already have been destroyed.  It cannot be
    // destroyed in this destructor because a 'Function_Rep' may exist with a
    // wrapped function in either a constructed or uninitialized state.  Only
    // the derived class knows whether the wrapped function needs to be
    // destroyed, especially during exception unwinding.

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
    BSLS_ASSERT(d_allocManager_p(e_IS_EQUAL, this,
                                 other.d_allocator_p).asSize_t());

    bsls::ObjectBuffer<Function_Rep> temp;
    destructiveMove(&temp.object(), &other);
    destructiveMove(&other, this);
    destructiveMove(this, &temp.object());
}

const std::type_info&
bsl::Function_Rep::target_type() const BSLS_NOTHROW_SPEC
{
    if (! d_funcManager_p) {
        return typeid(void);
    }

    PtrOrSize_t ret = d_funcManager_p(e_GET_TYPE_ID,
                                      const_cast<Function_Rep*>(this),
                                      PtrOrSize_t());
    return *static_cast<const std::type_info*>(ret.asPtr());
}

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
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
