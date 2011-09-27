// bdema_managedptr.cpp                                               -*-C++-*-
#include <bdema_managedptr.h>
#include <bdema_allocator.h>       // for testing only
#include <bdema_testallocator.h>   // for testing only

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_managedptr_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>                // for testing only
#include <bslma_defaultallocatorguard.h>    // for testing only
#include <bslma_testallocator.h>            // for testing only
#include <bslma_testallocatormonitor.h>     // for testing only

// Design notes
// ------------
// These notes are recorded here as they are not part of the public interface,
// but reflect certain design decisions taken when implementing this component.
// A number of redundant constructors were removed from earlier designs in
// order to produce the leanest component with minimal template bloat.  One
// side effect of this is that some signatures, such as the constructor for
// converting from a managed-pointer-of-a-different-type, are implicit and so
// no longer clearly documented in a clear place of their own.
// A second design decision was to implement aliasing as member-function
// templates taking references to arbitrary managed pointer types.  The other
// approach considered was to take 'bdema_ManagedPtr_Ref' objects by value,
// like the move operations.  This was ruled out due to the static-assert in
// the conversion-to-_Ref operator, that enforces a type-compatibility
// relationship between the aliased type and the managed pointer target type.
// This restriction is not present in the current contract, and the risks
// opened by removing the static-assert and allowing general conversions
// everywhere (as undefined behavior) were seen as too large.

namespace BloombergLP {

bdema_ManagedPtr_Members::bdema_ManagedPtr_Members(
                                               bdema_ManagedPtr_Members& other)
: d_obj_p(other.d_obj_p)
{
    if(d_obj_p) {
        d_deleter = other.d_deleter;
    }
    other.clear();
}

void bdema_ManagedPtr_Members::clear()
{
    d_obj_p = 0;
}

void bdema_ManagedPtr_Members::move(bdema_ManagedPtr_Members& other)
{
    // if 'other.d_obj_p' is null then 'other.d_deleter' may not be initialized
    // but 'set' takes care of that concern.  deleter passed by ref, so no read
    // of uninitialized memory occurs

    BSLS_ASSERT(this != &other);

    d_obj_p = other.d_obj_p;
    if (other.d_obj_p) {
        d_deleter = other.d_deleter;
    }

    other.clear();
}

void bdema_ManagedPtr_Members::set(void        *object,
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

void bdema_ManagedPtr_Members::swap(bdema_ManagedPtr_Members & other)
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

        bdema_ManagedPtrDeleter tmp = d_deleter;
        d_deleter                   = other.d_deleter;
        other.d_deleter             = tmp;
    }
}

                    // ----------------------------------
                    // struct bdema_ManagedPtrNoOpDeleter
                    // ----------------------------------

void bdema_ManagedPtrNoOpDeleter::deleter(void *, void*)
{
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
