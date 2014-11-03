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

    BSLS_ASSERT_SAFE(0 != deleter || 0 == object);

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
// Copyright 2013 Bloomberg Finance L.P.
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
