// bdema_managedptr_factorydeleter.h                                  -*-C++-*-
#ifndef INCLUDED_BDEMA_MANAGEDPTR_FACTORYDELETER
#define INCLUDED_BDEMA_MANAGEDPTR_FACTORYDELETER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a basic deleter for the managed pointer class.
//
//@CLASSES:
//  bdema_ManagedPtr_FactoryDeleter: deduced deleter used by bdema_ManagedPtr
//
//@AUTHOR: Alisdair Meredith (ameredith1@bloomberg.net)
//
//@SEE_ALSO: bdema_managedptr
//
//@DESCRIPTION: This component provides a class used as an implementation
// detail by 'bdema_ManagedPtr' to produce a type-erasing deleter function
// that will destroy an object of a parameterized type, using a factory of
// another parameterized type that provides a 'deleteObject' method.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                   // ======================================
                   // struct bdema_ManagedPtr_FactoryDeleter
                   // ======================================

template <class BDEMA_TYPE, typename BDEMA_FACTORY>
struct bdema_ManagedPtr_FactoryDeleter {
    // This utility provides a general deleter for objects that provide
    // a 'deleteObject' operation (e.g., 'bslma_Allocator', 'bdema_Pool').

    static void deleter(void *object, void *factory);
        // Deleter function that deletes the specified 'object' by invoking
        // 'deleteObject' on the specified 'factory', casting 'object' to
        // 'BDEMA_TYPE *' and 'factory' to 'FACTORY *'.  Note that the behavior
        // is undefined if '0 == object', or if 'factory' does not point to an
        // an object of type 'FACTORY', or 'object' does not point to a
        // complete object of type 'BDEMA_TYPE' unless 'BDEMA_TYPE' has a
        // virtual destructor and 'object' points to an object whose dynamic
        // type derives from 'BDEMA_TYPE.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // --------------------------------------
                    // struct bdema_ManagedPtr_FactoryDeleter
                    // --------------------------------------

template <class BDEMA_TYPE, typename BDEMA_FACTORY>
inline
void bdema_ManagedPtr_FactoryDeleter<BDEMA_TYPE, BDEMA_FACTORY>::deleter(
                                                   void *object, void *factory)
{
    BSLS_ASSERT_SAFE(0 != object);
    BSLS_ASSERT_SAFE(0 != factory);

    reinterpret_cast<BDEMA_FACTORY *>(factory)->deleteObject(
                                       reinterpret_cast<BDEMA_TYPE *>(object));
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
