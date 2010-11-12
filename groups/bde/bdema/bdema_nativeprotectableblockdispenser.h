// bdema_nativeprotectableblockdispenser.h      -*-C++-*-
#ifndef INCLUDED_BDEMA_NATIVEPROTECTABLEBLOCKDISPENSER
#define INCLUDED_BDEMA_NATIVEPROTECTABLEBLOCKDISPENSER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a platform-specific dispenser of protectable blocks.
//
//@CLASSES:
//    bdema_NativeProtectableBlockDispenser: platform-specific dispenser
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@SEE_ALSO: bdema_protectableblockdispenser
//
//@DESCRIPTION: This component provides a concrete singleton implementation
// of the 'bdema_ProtectableBlockDispenser' protocol:
//..
//            ( bdema_NativeProtectableBlockDispenser )
//                               |          ctor
//                               V
//              ( bdema_ProtectableBlockDispenser )
//                                          dtor
//                                          allocate
//                                          deallocate
//                                          protect
//                                          unprotect
//..
// This platform-dependent implementation provides the ability to protect and
// unprotect the blocks of memory it dispenses.  Here, "protected" memory is
// memory that has READ-ONLY access protection and will cause a segmentation
// violation if written to.  "Unprotected" memory has READ-WRITE access
// protection and can be written to freely.  Note that
// 'bdema_NativeProtectableBlockDispenser' provides a static 'singleton'
// method, which returns a single process-wide dispenser of protectable blocks
// configured for the current platform.
//
///Usage
///-----
// The following is a simple example of using a
// 'bdema_NativeProtectableBlockDispenser' to allocate, protect, unprotect,
// and deallocate memory:
//..
//    bdema_NativeProtectableBlockDispenser *dispenser =
//              &bdema_NativeProtectableBlockDispenser::singleton();
//..
// Allocate a memory block from this dispenser:
//..
//    int size = dispenser->minimumBlockSize();
//    bdema_MemoryBlockDescriptor block = dispenser->allocate(size);
//..
// Unprotect the memory block before modifying it:
//..
//    dispenser->unprotect(block);
//..
// Modify its contents:
//..
//    char *memory = static_cast<char *>(block.ptr());
//    std::memset(memory, 0, size);
//                                   assert(0 == memory[0]);
//    memory[0] = 1;
//                                   assert(1 == memory[0]);
//..
// Once the memory is protected, it cannot be modified:
//..
//    dispenser->protect(block);
//
//    // memory[0] = 2;  // WARNING: THIS WILL CAUSE A SEGMENTATION VIOLATION!
//..
// The block can be unprotected so that it can again be written to:
//..
//    dispenser->unprotect(block);
//
//    memory[0] = 2;
//                                   assert(2 == memory[0]);
//
//    dispenser->deallocate(block);
//..
// Note that the block must also be unprotected in order to deallocate it.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_MEMORYBLOCKDESCRIPTOR
#include <bdema_memoryblockdescriptor.h>
#endif

#ifndef INCLUDED_BDEMA_PROTECTABLEBLOCKDISPENSER
#include <bdema_protectableblockdispenser.h>
#endif

#ifndef INCLUDED_BSLS_OBJECTBUFFER
#include <bsls_objectbuffer.h>
#endif

namespace BloombergLP {

             // ===========================================
             // class bdema_NativeProtectableBlockDispenser
             // ===========================================

class bdema_NativeProtectableBlockDispenser :
                                       public bdema_ProtectableBlockDispenser {
    // This class defines a concrete platform-specific implementation of the
    // 'bdema_ProtectableBlockDispenser' protocol.  This dispenser supports
    // the ability to protect and unprotect the blocks of memory it allocates.
    // Here, "protected" memory is memory that has READ-ONLY access protection
    // and will cause a segmentation violation if written to.  "Unprotected"
    // memory has READ-WRITE access protection and can be written to freely.
    // Both 'protect' and 'unprotect' must be passed a reference to a
    // 'bdema_MemoryBlockDescriptor' that was returned from a call to
    // 'allocate()' and has not since been deallocated.

  private:
    // PRIVATE CLASS METHODS
    static void *initSingleton(
              bsls_ObjectBuffer<bdema_NativeProtectableBlockDispenser> *arena);
        // Construct a 'bdema_NativeProtectableBlockDispenser' in the
        // specified 'arena' in a thread-safe way and return 'arena'.  Note
        // that 'arena' must be suitably aligned.

    // PRIVATE CREATORS
    bdema_NativeProtectableBlockDispenser();
        // Construct a 'bdema_NativeProtectableBlockDispenser'.  Note that
        // all instances of this class share the same underlying resource:
        // use the public 'singleton()' method to obtain a reference to the
        // single, process-wide, instance of this dispenser.

  public:
    // CLASS METHODS
    static bdema_NativeProtectableBlockDispenser& singleton();
        // Return a reference to a modifiable process-wide unique instance of
        // this class.  Note that this instance is guaranteed to be valid from
        // the time this method is first called onward (i.e., not just until
        // exiting 'main').

    static int pageSize();
        // Return the underlying page size used to protect and unprotect
        // memory.  Note that this is a platform-dependent property.  Blocks
        // returned by this dispenser are rounded up to the nearest multiple
        // of the page size.

    // CREATORS
    virtual ~bdema_NativeProtectableBlockDispenser();
        // Destroy this 'bdema_NativeProtectableBlockDispenser'.  Note that
        // the behavior of destroying an allocator while memory is allocated
        // from it is not specified.  (Unless you *know* that it is valid to
        // do so, don't!)  Note that for this concrete implementation,
        // destroying this dispenser object has no effect on allocated memory.

    // MANIPULATORS
    virtual bdema_MemoryBlockDescriptor allocate(size_type size);
        // Allocate an unprotected block of at least the specified 'numBytes'
        // and return a descriptor of the block (containing the address and
        // actual size of the allocated memory).  If 'numBytes' is 0, a null
        // descriptor is returned with no other effect.  If this dispenser
        // cannot return the requested number of bytes, it will throw a
        // 'std::bad_alloc' exception in an exception-enabled build, or else
        // abort the program in a non-exception build.  The behavior is
        // undefined unless 'size >= 0'.  Note that the alignment of the
        // address returned conforms to the platform requirement for any object
        // of the specified 'numBytes'.

    virtual void deallocate(const bdema_MemoryBlockDescriptor& block);
        // Return the memory described by the specified 'block' to this
        // dispenser.  If 'block' is a null descriptor then this function has
        // no effect.  The behavior is undefined unless 'block' was returned
        // from a call to 'allocate()' on this
        // 'bdema_NativeProtectableBlockDispenser' object, has not since been
        // deallocated, and is currently in an unprotected state.

    virtual int protect(const bdema_MemoryBlockDescriptor& block);
        // Set the access protection for the specified 'block' to be READ-ONLY.
        // Return 0 on success and a non-zero value otherwise.  This method has
        // no effect if 'block' is a null descriptor or was already protected;
        // concrete implementations of the 'bdema_ProtectableBlockDispenser'
        // protocol may differ in the status value that is returned in
        // those two cases.  The behavior is undefined unless 'block' was
        // returned from a call to 'allocate()' on this object and has not
        // since been deallocated.

    virtual int unprotect(const bdema_MemoryBlockDescriptor& block);
        // Set the access protection for the specified 'block' to be
        // READ-WRITE.  Return 0 on success and a non-zero value otherwise.
        // This method has no effect if 'block' is a null descriptor or was
        // already unprotected; concrete implementations of the
        // 'bdema_ProtectableBlockDispenser' protocol may differ in the status
        // value that is returned in those two cases.  The behavior is
        // undefined unless 'block' was returned from a call to 'allocate()' on
        // this object and has not since been deallocated.

    // ACCESSORS
    virtual int minimumBlockSize() const;
        // Return the minimum size of any block returned by this allocator.
        // Note that this value indicates the size of the block returned by
        // 'allocate(1)'.
        //
        // For this implementation the minimum block size is equivalent to the
        // system page size.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

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
