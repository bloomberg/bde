// bslma_managedptr_members.cpp                                       -*-C++-*-
#include <bslma_managedptr_members.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslma_allocator.h>                  // for testing only
#include <bslma_default.h>                    // for testing only
#include <bslma_managedptr_factorydeleter.h>  // for testing only
#include <bslma_testallocator.h>              // for testing only
#include <bslma_testallocatormonitor.h>       // for testing only

namespace BloombergLP {
namespace bslma {

ManagedPtr_Members::ManagedPtr_Members(ManagedPtr_Members& other)
: d_obj_p(other.d_obj_p)
{
    if (d_obj_p) {
        d_deleter = other.d_deleter;
    }
    other.clear();
}

void ManagedPtr_Members::clear()
{
    d_obj_p = 0;
}

void ManagedPtr_Members::move(ManagedPtr_Members *other)
{
    // if 'other.d_obj_p' is null then 'other.d_deleter' may not be initialized
    // but 'set' takes care of that concern.  deleter passed by ref, so no read
    // of uninitialized memory occurs

    BSLS_ASSERT(other);
    BSLS_ASSERT(this != other);

    d_obj_p = other->d_obj_p;
    if (other->d_obj_p) {
        d_deleter = other->d_deleter;
    }

    other->clear();
}

void ManagedPtr_Members::moveAssign(ManagedPtr_Members *other)
{
    BSLS_ASSERT(other);

    // Must protect against self-assignment due to destructive move.

    if (this != other) {
        runDeleter();
        move(other);
    }
}

void ManagedPtr_Members::set(void        *object,
                             void        *factory,
                             DeleterFunc  deleter)
{
    // Note that 'factory' may be null if 'deleter' supports it, so cannot be
    // asserted here.

    BSLS_ASSERT_OPT(0 != deleter || 0 == object);

    d_obj_p = object;
    if (object) {
        d_deleter.set(object, factory, deleter);
    }
}

void ManagedPtr_Members::swap(ManagedPtr_Members & other)
{
    if (!d_obj_p) {
        d_obj_p       = other.d_obj_p;
        d_deleter     = other.d_deleter;
        other.d_obj_p = 0;
    }
    else if (!other.d_obj_p) {
        other.d_obj_p   = d_obj_p;
        other.d_deleter = d_deleter;
        d_obj_p         = 0;
    }
    else {
        void *tmp_p     = d_obj_p;
        d_obj_p         = other.d_obj_p;
        other.d_obj_p   = tmp_p;

        ManagedPtrDeleter tmp = d_deleter;
        d_deleter             = other.d_deleter;
        other.d_deleter       = tmp;
    }
}

}  // close package namespace
}  // close enterprise namespace


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
