// bdema_protectableblocklist.h                                       -*-C++-*-
#ifndef INCLUDED_BDEMA_PROTECTABLEBLOCKLIST
#define INCLUDED_BDEMA_PROTECTABLEBLOCKLIST

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide allocation and protection for a sequence of memory blocks.
//
//@CLASSES:
//    bdema_ProtectableBlockList: allocate and protect a list of memory blocks
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@SEE_ALSO: bcema_protectablesequentialallocator,
//           bdema_protectableblockdispenser
//
//@DESCRIPTION: This component implements a memory manager mechanism that
// allocates and manages a sequence of memory blocks, each potentially of a
// different size, and provides modifiers to change the access protection of
// the managed memory.  Here, *protected* memory is memory that has
// READ-ONLY access protection, and will cause a segmentation violation if
// written to; *unprotected* memory has READ-WRITE access protection and can be
// written to freely.  Both the block list's 'release()' method and destructor
// unprotect and deallocate the entire sequence of memory blocks without regard
// to the current protection state.  The behavior for 'allocate' and
// 'deallocate' is undefined unless the block list is in the *unprotected*
// state.
//
///Usage
///-----
// The 'bdema_ProtectableBlockList' class is commonly used to supply
// memory to more elaborate memory managers.  In this case we define a rather
// trivial 'myProtectedDataProvider', which provides protected copies of
// memory:
//..
//    class myProtectedDataProvider {
//
//         bdema_ProtectableBlockList d_blockList;
//
//         // NOT IMPLEMENTED
//         myProtectedDataProvider(const myProtectedDataProvider&);
//         myProtectedDataProvider& operator=(
//                                 const myProtectedDataProvider&);
//..
// We will use a 'bdema_ProtectableBlockDispenser' -- e.g. the one returned by
// 'bdema_NativeProtectableBlockDispenser::singleton()' -- to initialize the
// 'bdema_ProtectedSequentialDispenser':
//..
//      public:
//         // CREATORS
//         myProtectedDataProvider(
//                   bdema_ProtectableBlockDispenser *protectedDispenser)
//         : d_blockList(protectedDispenser)
//              // Create a 'myProtectedDataProvider' using the specified
//              // 'protectedDispenser' to allocate memory.
//         {
//         }
//..
// We must 'unprotect' the 'bdema_ProtectableBlockList' just before we use it
// and then 'protect' it again right after its use.  Use the
// 'bdema_ProtectableMemoryScopedGuard' to automate the unprotect/protect
// process.
//
// Note: this implementation wastes a lot of memory by not making use of
// the size of the returned block:
//..
//         // MANIPULATORS
//         void *protectedCopy(const void *data, int numBytes)
//             // Return a protected copy of the specified 'data' of the
//             // specified 'numBytes'.  A call to 'protectedCopy' will
//             // allocate at least (and probably significantly more) than
//             // 'numBytes' of data.  The behavior is undefined unless 'data'
//             // points to a valid memory region of at least 'numBytes'.
//         {
//             d_blockList.unprotect();
//             bdema_MemoryBlockDescriptor block =
//                                              d_blockList.allocate(numBytes);
//             std::memcpy(block.address(), data, numBytes);
//             d_blockList.protect();
//             return block.address();
//         }
//
//         void releaseProtectedCopy(void *data)
//             // Deallocate the specified 'data'.  The behavior is undefined
//             // unless 'data' was returned from a call to 'protectedCopy'.
//         {
//             d_blockList.unprotect();
//             d_blockList.deallocate(data);
//             d_blockList.protect();
//         }
//    };
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_MEMORYBLOCKDESCRIPTOR
#include <bdema_memoryblockdescriptor.h>
#endif

#ifndef INCLUDED_BDEMA_NATIVEPROTECTABLEBLOCKDISPENSER
#include <bdema_nativeprotectableblockdispenser.h>
#endif

#ifndef INCLUDED_BDEMA_PROTECTABLEBLOCKDISPENSER
#include <bdema_protectableblockdispenser.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                   // ================================
                   // class bdema_ProtectableBlockList
                   // ================================

class bdema_ProtectableBlockList {
    // This class implements a memory manager mechanism that allocates and
    // manages a collection of memory blocks, each potentially of a different
    // size, and allows clients to 'protect' and 'unprotect' the memory.  Here,
    // *protected* memory is memory that has READ-ONLY access protection, and
    // will cause a segmentation violation if written to; *unprotected* memory
    // has READ-WRITE access protection and can be written to freely.  The
    // primary manipulator 'allocate' will allocate at least (and most likely
    // significantly more) than the number of bytes requested.  The actual
    // number of bytes allocated is returned in the
    // 'bdema_MemoryBlockDescriptor'.  The behavior for the 'allocate' and
    // 'deallocate' operations is undefined unless the block list is in the
    // *unprotected* state.

    // PRIVATE TYPES
    struct Block {
        Block                              *d_next_p;  // next block
        Block                              *d_prev_p;  // prev block
        int                                 d_size;    // size of allocation
        bsls_AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
    };

    enum {
        BLOCK_HEADER_SIZE =
                         sizeof(Block) - bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT
            // size of the header for allocated blocks.
    };

    // DATA
    bool                             d_isProtected;  // protection state
    Block                           *d_head_p;       // first block
    bdema_ProtectableBlockDispenser *d_dispenser_p;  // block dispenser

    // NOT IMPLEMENTED
    bdema_ProtectableBlockList(const bdema_ProtectableBlockList&);
    bdema_ProtectableBlockList& operator=(const bdema_ProtectableBlockList&);

  private:
    // PRIVATE MANIPULATORS
    bdema_MemoryBlockDescriptor descriptor(Block *block);
        // Return a memory block descriptor for the specified 'block'.

  public:
    // CLASS METHODS
    static int blockHeaderSize();
        // Return the amount of memory in bytes used for book-keeping on each
        // allocation.

    // CREATORS
    explicit bdema_ProtectableBlockList(
                          bdema_ProtectableBlockDispenser *blockDispenser = 0);
        // Create a 'bdema_ProtectableBlockList'.  Optionally specify a
        // 'blockDispenser' to supply protectable blocks of memory.  If
        // 'blockDispenser' is 0, the
        // 'bdema_NativeProtectableBlockDispenser::singleton' is used.

    ~bdema_ProtectableBlockList();
        // Destroy this 'bdema_ProtectableBlockList' and unprotect and
        // deallocate all memory blocks managed by this object.

    // MANIPULATORS
    bdema_MemoryBlockDescriptor allocate(int numBytes);
        // Allocate a memory block whose size is at least the specified
        // 'numBytes', and return a descriptor for the allocated memory block.
        // The 'bdema_MemoryBlockDescriptor' contains the address and actual
        // size of the allocated block.  The behavior is undefined unless
        // 'numBytes >= 0' and the block list is in the unprotected state.
        // Note that the amount of memory allocated is partially governed by
        // the underlying memory block dispenser supplied at construction;
        // hence, the size of the block returned may be *significantly*
        // *larger* than 'numBytes'.

    void deallocate(void *address);
        // Return the block of memory at the specified 'address' to the
        // dispenser associated with this block list.  If 'address' is 0, this
        // function has no effect.  The behavior is undefined unless this block
        // list is in the unprotected state, and 'address' was allocated using
        // this memory manager and has not since been deallocated.

    void release();
        // Unprotect and deallocate all memory blocks managed by this object
        // without regard to its initial state.  After this operation returns
        // the block list will be left in the unprotected state.

    void protect();
        // Set the access protection on the memory managed by this block list
        // to READ-ONLY.  After this call returns, attempts to write to the
        // memory managed by this block list will result in a segmentation
        // violation.  Note that this method has no effect if the memory
        // managed by this block list is already protected.

    void unprotect();
        // Set the access protection on the memory managed by this block list
        // to READ-WRITE.  After this call returns, attempts to write to the
        // memory managed by this block list will not cause a segmentation
        // violation.  Note that this method has no effect if the memory
        // managed by this block list is already unprotected.

    // ACCESSORS
    bool isProtected() const;
        // Return 'true' if the memory manged by this block list is in the
        // protected state, and 'false' otherwise.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                  // --------------------------------
                  // class bdema_ProtectableBlockList
                  // --------------------------------

// PRIVATE MANIPULATORS
inline
bdema_MemoryBlockDescriptor
bdema_ProtectableBlockList::descriptor(Block *block)
{
    BSLS_ASSERT_SAFE(block);

    return bdema_MemoryBlockDescriptor(static_cast<void *>(block),
                                       block->d_size);
}

// CREATORS
inline
bdema_ProtectableBlockList::bdema_ProtectableBlockList(
                               bdema_ProtectableBlockDispenser *blockDispenser)
: d_isProtected(false)
, d_head_p(0)
, d_dispenser_p(
          blockDispenser ? blockDispenser
                         : &bdema_NativeProtectableBlockDispenser::singleton())
{
}

// ACCESSORS
inline
bool bdema_ProtectableBlockList::isProtected() const
{
    return d_isProtected;
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
