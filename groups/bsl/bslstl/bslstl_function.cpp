// bslstl_function.cpp                  -*-C++-*-

#include <bslstl_function.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_objectbuffer.h>

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

            // Move-construct function
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

            d_objbuf.d_object_p = other.d_objbuf.d_object_p;
            d_allocManager_p    = other.d_allocManager_p;
            d_allocator_p       = other.d_allocator_p;

            // Now re-initialize 'other' as an empty object
            other.d_funcManager_p = NULL;
            d_allocManager_p(e_INIT_REP, &other, d_allocator_p);
        }
    }
    else {
        // Moving an empty 'function' object.
        // Initialize just the allocator portion of the result
        other.d_allocManager_p(e_INIT_REP, this, other.d_allocator_p);
    }

    return inplace;
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

      case e_GET_SIZE:     return PtrOrSize_t();
      case e_GET_TARGET:   return rep->d_allocator_p;
      case e_GET_TYPE_ID:  return &typeid(bslma::Allocator);

      case e_INIT_REP: {
          bslma::Allocator *inputAlloc = static_cast<bslma::Allocator *>(
              const_cast<void *>(input.asPtr()));

          std::size_t sooFuncSize = rep->d_funcManager_p ?
              rep->d_funcManager_p(e_GET_SIZE, rep,
                                   PtrOrSize_t()).asSize_t() : 0;

          rep->initRep(sooFuncSize, inputAlloc,
                       integral_constant<AllocCategory, e_BSLMA_ALLOC_PTR>());

      } break;

    } // end switch

    return PtrOrSize_t();
}

void bsl::Function_Rep::destructiveMove(Function_Rep *to,
                                        Function_Rep *from) BSLS_NOTHROW_SPEC
{
    // TBD: This is a temporary implementation.  The real implementation will
    // avoid the possibility of exceptions being thrown and taking advantage
    // of bitwise-moveable.
    to->moveInit(*from);
    from->~Function_Rep();
}

bsl::Function_Rep::~Function_Rep()
{
    // Assert class invariants
    BSLS_ASSERT(d_allocator_p);
    BSLS_ASSERT(d_allocManager_p);

    // Integral function size cast to pointer type.
    PtrOrSize_t sooFuncSize;

    if (d_funcManager_p) {
        // e_DESTROY returns the size of the object that was destroyed.
        sooFuncSize = d_funcManager_p(e_DESTROY, this, PtrOrSize_t());
    }

    d_allocManager_p(e_DESTROY, this, sooFuncSize);
}

void bsl::Function_Rep::swap(Function_Rep& other) BSLS_NOTHROW_SPEC
{
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
