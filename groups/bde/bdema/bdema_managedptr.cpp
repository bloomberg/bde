// bdema_managedptr.cpp            -*-C++-*-
#include <bdema_managedptr.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_managedptr_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>                // for testing only
#include <bslma_defaultallocatorguard.h>    // for testing only
#include <bslma_testallocator.h>            // for testing only


namespace BloombergLP {

bdema_ManagedPtr_Members::bdema_ManagedPtr_Members(
                                               bdema_ManagedPtr_Members& other)
: d_obj_p(other.d_obj_p)
{
    if(d_obj_p) {    
        d_deleter = other.d_deleter;
    }
    other.rawClear();
}

void bdema_ManagedPtr_Members::init(void *ptr,
                                    const bdema_ManagedPtrDeleter& rep)
{
    d_obj_p = ptr;
    if (ptr) {
        d_deleter = rep;
    }
    else {
        d_deleter.clear();
    }
}

void bdema_ManagedPtr_Members::init(void *ptr, 
                                    void *object, 
                                    void *factory, 
                                    DeleterFunc deleter)
{
    d_obj_p = ptr;
    if (ptr) {
        d_deleter.set(object, factory, deleter);
    }
    else {
        d_deleter.clear();
    }
}

void bdema_ManagedPtr_Members::reset()
{
    runDeleter();
    rawClear();
}

void bdema_ManagedPtr_Members::reset(void *ptr,
                                     const bdema_ManagedPtrDeleter &rep)
{
    runDeleter();
    init(ptr, rep);
}

void bdema_ManagedPtr_Members::reset(bdema_ManagedPtr_Members& other)
{   
    // if 'other.d_obj_p' is null then 'other.d_deleter' may not be initialized
    // but 'set' takes care of that concern.  deleter passed by ref, so no read
    // of uninitialized memory occurs

    runDeleter();
    init(other.d_obj_p, other.d_deleter);
    other.rawClear();
}

void bdema_ManagedPtr_Members::reset(void        *ptr,
                                     void        *object,
                                     void        *factory,
                                     DeleterFunc  deleter)
{
    BSLS_ASSERT_SAFE(0 != factory || 0 == ptr);
    BSLS_ASSERT_SAFE(0 != deleter || 0 == ptr);

    runDeleter();
    init(ptr, object, factory, deleter);
}

void bdema_ManagedPtr_Members::swap(bdema_ManagedPtr_Members & other) {
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


}
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
