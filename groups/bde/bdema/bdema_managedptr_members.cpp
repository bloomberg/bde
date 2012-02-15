// bdema_managedptr_members.cpp                                       -*-C++-*-
#include <bdema_managedptr_members.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_managedptr_cpp,"$Id$ $CSID$")

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

void bdema_ManagedPtr_Members::move(bdema_ManagedPtr_Members *other)
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

void bdema_ManagedPtr_Members::moveAssign(bdema_ManagedPtr_Members *other)
{
    BSLS_ASSERT(other);

    // Must protect against self-assignment due to destructive move
    if (this != other) {
        runDeleter();
        move(other);
    }
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

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
