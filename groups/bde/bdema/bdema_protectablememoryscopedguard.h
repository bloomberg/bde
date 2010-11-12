// bdema_protectablememoryscopedguard.h                               -*-C++-*-
#ifndef INCLUDED_BDEMA_PROTECTABLEMEMORYSCOPEDGUARD
#define INCLUDED_BDEMA_PROTECTABLEMEMORYSCOPEDGUARD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a scoped guard to temporarily unprotect memory for writing.
//
//@CLASSES:
//  bdema_ProtectableMemoryScopedGuard: used to temporarily unprotect memory
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@SEE_ALSO: bcema_protectablesequentialallocator,
//           bcema_protectablemultipoolallocator,
//           bdema_protectableblocklist
//
//@DESCRIPTION: This component provides a generic proctor to automatically
// unprotect and protect memory allocated from an external protectable memory
// manager.  The protectable memory manager can be of any type that provides
// the following methods:
//..
//    void unprotect();
//    void protect();
//..
// For example, two such types are 'bdema_ProtectableBlockList' and
// 'bcema_ProtectableSequentialAllocator'.
// The 'bdema_ProtectableMemoryScopedGuard' class implements the "construction
// is acquisition, destruction is release" idiom.  Upon construction, a
// 'bdema_ProtectableMemoryScopedGuard' automatically calls 'unprotect'
// on the user-supplied protectable memory manager; upon destruction, 'protect'
// is called on the manager.
//
///Usage
///-----
// This usage example defines a function, 'setMemory', that safely assigns a
// 'value' to the memory in a 'protectedBuffer' that had been allocated from
// the specified 'blockList':
//..
//    void setMemory(char                       *protectedBuffer,
//                   int                         bufferLength,
//                   char                        value,
//                   bdema_ProtectableBlockList *blockList)
//        // Set all bytes in the specified 'protectedBuffer' having the
//        // specified 'bufferLength' to the specified 'value' using the
//        // specified 'blockList' to handle memory protection.  The
//        // 'protectedBuffer' will be returned to a protected (READ-ONLY)
//        // state before this call returns (regardless of whether or not it
//        // was initially protected).  The behavior is undefined unless
//        // 'protectedBuffer' was allocated from 'blockList'.
//    {
//..
// Instantiate a scoped guard, unprotecting (if necessary) the memory
// previously supplied by 'blockList', allowing us to write to the memory that
// 'blockList' is managing (e.g., 'protectedBuffer'):
//..
//        bdema_ProtectableMemoryScopedGuard<bdema_ProtectableBlockList>
//                                                            guard(blockList);
//..
// Write to the memory:
//..
//        for (int i = 0; i < bufferLength; ++i) {
//            protectedBuffer[i] = value;
//        }
//    }
//..
// When 'guard' goes out of scope, the memory managed by 'blockList' is again
// put into a protected state as a side-effect of the destruction of 'guard'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                // ========================================
                // class bdema_ProtectableMemoryScopedGuard
                // ========================================

template <class MANAGER_TYPE>
class bdema_ProtectableMemoryScopedGuard {
    // This class provides a guard for working with protected memory.  When
    // the guard is instantiated it *unprotects* the object whose address is
    // supplied at construction; when the guard is destroyed it *protects*
    // that object.

    // DATA
    MANAGER_TYPE *d_memoryManager_p;  // wrapped memory manager (held, not
                                      // owned)

    // NOT IMPLEMENTED
    bdema_ProtectableMemoryScopedGuard(
                                    const bdema_ProtectableMemoryScopedGuard&);
    bdema_ProtectableMemoryScopedGuard& operator=(
                                    const bdema_ProtectableMemoryScopedGuard&);

  public:
    // CREATORS
    bdema_ProtectableMemoryScopedGuard(MANAGER_TYPE *memoryManager);
        // Create a scoped guard and *unprotect* the specified 'memoryManager'
        // of parameterized 'MANAGER_TYPE'.  'MANAGER_TYPE' must provide both
        // 'protect' and 'unprotect' methods.  If 'memoryManager' is a
        // protectable memory allocator, subsequent writes to memory managed by
        // the allocator will not cause a segmentation violation until the
        // manager is put into the "protected" state.

    ~bdema_ProtectableMemoryScopedGuard();
        // Destroy this scoped guard and *protect* the memory manager that was
        // supplied at construction of this guard.  If the memory manager is a
        // protectable memory allocator, subsequent writes to memory managed by
        // that allocator will cause a segmentation violation until the manager
        // is put into the "unprotected" state.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

            // ----------------------------------------
            // class bdema_ProtectableMemoryScopedGuard
            // ----------------------------------------

// CREATORS
template <class MANAGER_TYPE>
inline
bdema_ProtectableMemoryScopedGuard<MANAGER_TYPE>::
bdema_ProtectableMemoryScopedGuard(MANAGER_TYPE *memoryManager)
: d_memoryManager_p(memoryManager)
{
    BSLS_ASSERT_SAFE(d_memoryManager_p);

    d_memoryManager_p->unprotect();
}

template <class MANAGER_TYPE>
inline
bdema_ProtectableMemoryScopedGuard<MANAGER_TYPE>::
~bdema_ProtectableMemoryScopedGuard()
{
    BSLS_ASSERT_SAFE(d_memoryManager_p);

    d_memoryManager_p->protect();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
