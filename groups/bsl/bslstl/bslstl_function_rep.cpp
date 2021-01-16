// bslstl_function_rep.cpp                                            -*-C++-*-

#include <bslstl_function_rep.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>

#include <bslstl_referencewrapper.h>  // For testing only

namespace BloombergLP {

#if !defined(BSLS_PLATFORM_CMP_MSVC) && !defined(BSLS_PLATFORM_CMP_SUN)
const std::size_t bslstl::Function_Rep::k_NON_SOO_SMALL_SIZE;
#endif

bslstl::Function_Rep::~Function_Rep()
{
    if (! d_funcManager_p) {
        // Empty object.

        // Invariant: d_invoker_p is non-null only if d_funcManager_p is
        // non-null.  (See state table in class declaration)
        BSLS_ASSERT(! d_invoker_p);
        return;                                                       // RETURN
    }

    if (d_invoker_p) {
        // Call destructor on wrapped functor.
        d_funcManager_p(e_DESTROY, this, 0);
    }

    if (calcSooFuncSize() > sizeof(InplaceBuffer) && d_objbuf.d_object_p) {
        d_allocator.mechanism()->deallocate(d_objbuf.d_object_p);
    }
}

void bslstl::Function_Rep::allocateBuf(std::size_t sooFuncSize)
{
    BSLS_ASSERT((! d_funcManager_p) && (! d_invoker_p));

    if (sooFuncSize > sizeof(InplaceBuffer)) {
        // If 'sooFuncSize == k_NON_SOO_SMALL_SIZE', it would imply a
        // zero-sized object, which is impossible, or an object so large that
        // it would not fit in memory.
        BSLS_ASSERT_SAFE(sooFuncSize != k_NON_SOO_SMALL_SIZE);

        std::size_t funcSize = (sooFuncSize > k_NON_SOO_SMALL_SIZE ?
                                sooFuncSize - k_NON_SOO_SMALL_SIZE :
                                sooFuncSize);
        d_objbuf.d_object_p = d_allocator.mechanism()->allocate(funcSize);
    }
}

void bslstl::Function_Rep::copyInit(const Function_Rep& original)
{
    // Precondition: '*this' is empty.
    BSLS_ASSERT(0 == d_invoker_p);

    // This call to 'calcSooFuncSize' is the only call through 'original's
    // function manager.
    std::size_t sooFuncSize = original.calcSooFuncSize();

    allocateBuf(sooFuncSize);

    // Set the manager pointer after a successful allocation.
    d_funcManager_p = original.d_funcManager_p;

    if (d_funcManager_p) {
        BSLS_ASSERT(original.d_invoker_p);  // Must be fully constructed.

        void *source = (sooFuncSize <= sizeof(InplaceBuffer) ?
                        &original.d_objbuf : original.d_objbuf.d_object_p);
        d_funcManager_p(e_COPY_CONSTRUCT, this, source);

        // Set d_invoker_p pointer only after functor is successfully copied
        d_invoker_p = original.d_invoker_p;
    }
}

void bslstl::Function_Rep::moveInit(Function_Rep *from)
{
    // Precondition: '*this' is empty.
    BSLS_ASSERT(0 == d_invoker_p);

    if (! from->d_invoker_p) {
        // '*from' is empty
        return;                                                       // RETURN
    }

    if (d_allocator != from->d_allocator) {
        copyInit(*from);
        return;                                                       // RETURN
    }

    // No operation past this point is allowed to throw.

    // Destructively move the wrapped functor.
    if (from->isInplace()) {
        // '*from' uses the small-object optimization.
        // Destructively move inplace wrapped functor.
        from->d_funcManager_p(e_DESTRUCTIVE_MOVE, this, &from->d_objbuf);
    }
    else {
        // '*from' does not use the small-object optimization.
        // Just move pointer for out-of-place wrapped functor.
        this->d_objbuf.d_object_p = from->d_objbuf.d_object_p;
        from->d_objbuf.d_object_p = 0;
    }

    // Set d_funcManager_p pointer after functor is successfully moved
    this->d_funcManager_p = from->d_funcManager_p;
    from->d_funcManager_p = 0;
    this->d_invoker_p     = from->d_invoker_p;
    from->d_invoker_p     = 0;
}

void bslstl::Function_Rep::makeEmpty()
{
    // This function should not be called on a partially-constructed object.
    // Either 'd_funcManager_p' and 'd_invoker_p' are both NULL, or they are
    // both non-NULL.
    BSLS_ASSERT((! d_funcManager_p) == (! d_invoker_p));

    if (! d_funcManager_p) {
        // Already empty.  Nothing to do.
        return;                                                       // RETURN
    }

    // Call destructor on wrapped functor.
    std::size_t sooFuncSize = d_funcManager_p(e_DESTROY, this, 0);
    d_invoker_p = 0;

    if (sooFuncSize > sizeof(InplaceBuffer)) {
        // Deallocate functor.
        d_allocator.mechanism()->deallocate(d_objbuf.d_object_p);
        d_objbuf.d_object_p = 0;
    }
    d_funcManager_p = 0;
}

void bslstl::Function_Rep::swap(Function_Rep& other) BSLS_KEYWORD_NOEXCEPT
{
    // Swap will fail if allocators are not compatible
    BSLS_ASSERT(this->get_allocator() == other.get_allocator());

    Function_Rep temp(get_allocator());

    // Each call to 'moveInit' takes an empty 'this' argument on entry
    // leaves an empty 'from' argument on return.
    temp.moveInit(&other);
    other.moveInit(this);
    this->moveInit(&temp);
}

bool bslstl::Function_Rep::isInplace() const BSLS_KEYWORD_NOEXCEPT
{
    if (! d_funcManager_p) {
        // Always return true when invoked on an empty object.
        return true;                                                  // RETURN
    }

    const std::size_t sooFuncSize =
        d_funcManager_p(e_GET_SIZE, const_cast<Function_Rep*>(this), 0);
    return sooFuncSize <= sizeof(InplaceBuffer);
}

const std::type_info&
bslstl::Function_Rep::target_type() const BSLS_KEYWORD_NOEXCEPT
{
    if (! d_funcManager_p) {
        return typeid(void);                                          // RETURN
    }

    const std::type_info* ret =
        d_funcManager_p(e_GET_TYPE_ID, const_cast<Function_Rep*>(this), 0);
    return *ret;
}

}  // close enterprise namespace

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
