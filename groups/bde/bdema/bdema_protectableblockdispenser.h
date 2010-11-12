// bdema_protectableblockdispenser.h      -*-C++-*-
#ifndef INCLUDED_BDEMA_PROTECTABLEBLOCKDISPENSER
#define INCLUDED_BDEMA_PROTECTABLEBLOCKDISPENSER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for allocating and protecting blocks of memory.
//
//@CLASSES:
//  bdema_ProtectableBlockDispenser: protocol to allocate and protect blocks
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@SEE_ALSO: bdema_memoryblockdescriptor, bdema_nativeprotectableblockdispenser
//
//@DESCRIPTION: This component defines a protocol for allocating and
// protecting blocks of memory.  The protocol supports operations to
// 'allocate' and 'deallocate' blocks of memory, as well as operations to
// 'protect' and 'unprotect' the blocks of memory supplied by 'allocate'.
// Here, "protected" memory is memory that is marked as READ-ONLY, and will
// cause a segmentation violation if written to, and "unprotected" memory is
// marked as READ-WRITE and can be written to freely.  The 'allocate' method
// allocates a block of at least the size requested.  The actual size allocated
// is implementation and system dependent, so 'allocate' returns a
// 'bdema_MemoryBlockDescriptor' that holds a pointer to the allocated memory
// as well as the size that was allocated.
//
///Usage
///-----
// The following examples demonstrate how the
// 'bdema_ProtectableBlockDispenser' protocol can be implemented and used.
//
///Usage 1: Trivial example
///- - - - - - - - - - - -
// The following is a simple example of using a
// 'bdema_ProtectableBlockDispenser' to allocate, protect, unprotect, and
// deallocate memory blocks:
//..
//    void doImportantStuff(bdema_ProtectableBlockDispenser *dispenser)
//    {
//        assert(0 != dispenser);
//..
// We now allocate memory for our data using the specified 'dispenser' and
// then protect the data from accidental writes (by another thread, for
// example):
//..
//        const int size = dispenser->minimumBlockSize();
//        bdema_MemoryBlockDescriptor block = dispenser->allocate(size);
//..
// Memory is unprotected when initially allocated from a protectable block
// dispenser, so we can freely write to it:
//..
//        std::strcpy(static_cast<char *>(block.address()), "data");
//
//        dispenser->protect(block);
//..
// But now that the block is protected, it cannot be written to without causing
// a memory fault:
//..
//        // std::strcpy(static_cast<char *>(block.address()), "BAD");
//        // WARNING: THIS WILL CAUSE A SEGMENTATION VIOLATION!
//..
// Note that the memory block must be *unprotected* before being deallocated:
//..
//         dispenser->unprotect(block);
//         dispenser->deallocate(block);
//    }
//..
///Usage 2: Create a class that uses a 'bdema_ProtectableBlockDispenser'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates a more realistic use of a protectable memory
// block dispenser.  The class 'IntegerStack' uses a
// 'bdema_ProtectableBlockDispenser', supplied at construction, to protect the
// data inside the stack from being unintentionally overwritten:
//..
//    class IntegerStack {
//        // This is a trivial implementation of a stack of 'int' values whose
//        // data has READ-ONLY access.  This class does not perform bounds
//        // checking.
//
//        bdema_MemoryBlockDescriptor      d_data;         // block of memory
//                                                         // holding the stack
//
//        int                             *d_cursor_p;     // top of the stack
//
//        bdema_ProtectableBlockDispenser *d_dispenser_p;  // memory dispenser
//                                                         // (held not owned)
//  // ...
//      private:
//        // PRIVATE MANIPULATORS
//        void resize()
//            // Increase the size of the stack by a factor of 2.
//        {
//            bdema_MemoryBlockDescriptor oldData = d_data;
//            d_data = d_dispenser_p->allocate(d_data.size() * 2);
//            d_cursor_p = (int *)((char *)d_data.address() +
//                         ((char *)d_cursor_p - (char *)oldData.address()));
//            std::memcpy(d_data.address(), oldData.address(), oldData.size());
//            d_dispenser_p->deallocate(oldData);
//        }
//
//      public:
//        // CREATORS
//        IntegerStack(int                              initialSize,
//                     bdema_ProtectableBlockDispenser *protectedDispenser)
//            // Create an 'IntegerStack' having the specified 'initialSize'.
//            // Use the specified 'protectedDispenser' to supply memory.  The
//            // behavior is undefined unless '0 <= maxStackSize' and
//            // 'protectedDispenser' is non-null.
//        : d_data()
//        , d_dispenser_p(protectedDispenser)
//        {
//            d_data  = d_dispenser_p->allocate(initialSize * sizeof(int));
//            assert(!d_data.isNull());
//            d_cursor_p = (int *)d_data.address();
//            d_dispenser_p->protect(d_data);
//        }
//..
// We must unprotect the dispenser before modifying or deallocating memory:
//..
//        ~IntegerStack()
//            // Destroy this object and release its memory.
//        {
//            d_dispenser_p->unprotect(d_data);
//            d_dispenser_p->deallocate(d_data);
//        }
//  // ...
//        // MANIPULATORS
//        void push(int value)
//            // Push the specified 'value' onto the stack.
//        {
//            d_dispenser_p->unprotect(d_data);
//            if (sizeof(int) >
//               (char *)d_data.address()+d_data.size()-(char *)d_cursor_p) {
//                resize();
//            }
//            *d_cursor_p++ = value;
//            d_dispenser_p->protect(d_data);
//        }
//
//        int pop()
//            // Remove the top value from the stack and return it.
//        {
//            // The buffer isn't modified so there is no need to unprotect it.
//            const int value = *--d_cursor_p;
//            return value;
//        }
//  // ...
//    };
//..
///Usage 3: Implementing the 'bdema_ProtectableBlockDispenser' Protocol
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we create a dummy implementation of the
// 'bdema_ProtectableBlockDispenser' protocol.  In order to avoid
// platform-specific dependencies, the 'protect' and 'unprotect' methods do not
// provide access protection.  Along with each block of memory returned by this
// dispenser, we maintain a header that tracks the memory's size, allocation
// state, and protection state:
//..
//    // dummyprotectablebufferblockdispenser.h
//
//    class DummyProtectableBufferBlockDispenser :
//                                     public bdema_ProtectableBlockDispenser {
//        // This class returns portions of memory from a single buffer
//        // supplied at construction.
//
//        // DATA
//        char      *d_buffer_p;  // start of the memory block
//        char      *d_cursor_p;  // next free byte
//        const int  d_size;      // size of the buffer
//        const int  d_pageSize;  // size of the blocks
//
//      public:
//        // CREATORS
//        DummyProtectableBufferBlockDispenser(char *buffer,
//                                             int   bufferSize,
//                                             int   pageSize);
//            // Create a 'DummyProtectableBufferBlockdispenser' on the
//            // specified 'buffer' of the specified 'bufferSize' using the
//            // specified 'pageSize.
//
//        ~DummyProtectableBufferBlockDispenser();
//            // Destroy this 'DummyProtectableBufferBlockdispenser'.
//
//        // MANIPULATORS
//        bdema_MemoryBlockDescriptor allocate(size_type size);
//          // Return a descriptor for a newly-allocated block of memory of
//          // at least the specified 'size'.  If 'size' is 0, a null block
//          // descriptor is returned with no other effect.  If this dispenser
//          // cannot return the requested number of bytes, then it will throw
//          // a 'std::bad_alloc' exception in an exception-enabled build, or
//          // else will abort the program in a non-exception build.  The
//          // behavior is undefined unless 'size >= 0'.
//
//        virtual void deallocate(const bdema_MemoryBlockDescriptor& block);
//            // This operations has no effect for this dispenser.  If it can
//            // be determined that the specified 'block' was not returned by
//            // this dispenser then write an error to the console and abort.
//
//        virtual int protect(const bdema_MemoryBlockDescriptor& block);
//            // Protect the memory described by the specified 'block'.  To
//            // avoid platform dependencies, no access protection is applied
//            // to the internal buffer, only the header for the 'block' is
//            // updated to reflect its new state.  If it can be determined
//            // that 'block' was not returned by this dispenser, then write an
//            // error to the console and abort.
//
//        virtual int unprotect(const bdema_MemoryBlockDescriptor& block);
//            // Unprotect the memory described by the specified 'block'.  To
//            // avoid platform dependencies, access protection is not applied
//            // to the internal buffer, only the header for the 'block' is
//            // updated to reflect its new state.  If it can be determined
//            // that 'block' was not returned by this dispenser, then write an
//            // error to the console and abort.
//
//        // ACCESSORS
//        virtual int minimumBlockSize() const;
//            // Return the minimum size of a block returned by this allocator.
//            // This value indicates the size of the block that would be
//            // returned if 'allocate(1)' were called.  Note that this
//            // implementation returns the 'pageSize' supplied at
//            // construction.
//
//        bool isProtected(const bdema_MemoryBlockDescriptor& block) const;
//            // Return the current protection status of this dispenser.
//
//    // ...
//
//    };
//
//    // dummyprotectablebufferblockdispenser.cpp
//    #include <dummyprotectablebufferblockdispenser.h>
//
//    void die(bool condition, const char *stmnt, const char *file, int line)
//        // If the specified 'condition' is 'false', write an error message
//        // with the specified 'stmnt', 'file', and 'line' to the console
//        // and abort the program.  This operation has no effect if
//        // 'condition' is 'true'.
//    {
//        if (condition) {
//            bsl::cout << "Abort:" << stmnt << "  " << file << ":" << line
//                      << bsl::endl;
//            std::exit(-1);
//        }
//    }
//
//    #define VERIFY(X) { die(!(X), #X, __FILE__, __LINE__); }
//      // If the specified 'X' evaluates to 'false', write an error message to
//      // the console and abort the program.  If 'X' evaluates to 'true', this
//      // statement has no effect.  This 'VERIFY' macro is similar to
//      // 'assert', but does not depend on the build target being in a 'DEBUG'
//      // build.
//
//    namespace {
//
//        // The following constants are used for the header bytes.
//        const char READONLY_FLAG      = 0xDD; // marked as read only
//        const char READWRITE_FLAG     = 0XCC; // marked as read write
//        const char ALLOCATED_FLAG     = 0xAA; // marked as allocated
//        const char DEALLOCATED_FLAG   = 0xBB; // marked as freed
//
//        // The following constants are used to tag memory in the buffer.
//        const char UNUSED_BUFFER_BYTE = 0XEE; // an unused buffer byte
//        const char HEADPADDING_BYTE   = 0x11; // padding for a 'Header'
//        const char UNINITIALIZED_BYTE = 0xFF; // newly allocated memory
//        const char FREED_BYTE         = 0xA5; // freed memory
//
//        struct Header {
//            // This structure is used to keep track of the state of an
//            // allocated block of memory.
//
//            char d_allocatedState;  // allocation state; FLAG constant above
//            char d_protectedState;  // protection state; FLAG constant above
//            int  d_size;            // size of the memory allocated
//        };
//
//        union Align {
//            // This structure is used to ensure the alignment of the header
//            // information.
//
//            Header                         d_header;  // memory header
//            bsls_Alignment::MaxAlignedType d_align;   // ensure alignment
//        };
//
//    }  // close unnamed namespace
//
//    // CREATORS
//    DummyProtectableBufferBlockDispenser::
//    DummyProtectableBufferBlockDispenser(char *buffer,
//                                         int   bufferSize,
//                                         int   pageSize)
//    : d_buffer_p(buffer)
//    , d_cursor_p(buffer)
//    , d_size(bufferSize)
//    , d_pageSize(pageSize)
//    {
//        std::memset(buffer, UNUSED_BUFFER_BYTE, bufferSize);
//        d_cursor_p += bsls_Alignment::calculateAlignmentOffset(
//                                  d_cursor_p, bsls_Alignment::MAX_ALIGNMENT);
//    }
//
//    DummyProtectableBufferBlockDispenser::
//    ~DummyProtectableBufferBlockDispenser()
//    {
//    }
//
//    // MANIPULATORS
//    bdema_MemoryBlockDescriptor
//    DummyProtectableBufferBlockDispenser::allocate(size_type size)
//    {
//
//        const int numPages       = (size + d_pageSize - 1) / d_pageSize;
//        const int actualSize     = numPages * d_pageSize;
//        const int sizeWithHeader = actualSize + sizeof(Align);
//
//        if (0 == size) {
//            return bdema_MemoryBlockDescriptor();
//        }
//        if (d_cursor_p + sizeWithHeader > d_buffer_p + d_size) {
//
//            // Not enough free space in the buffer.
//            bslma_Allocator::throwBadAlloc();
//        }
//
//        char *cursor = d_cursor_p;
//
//        // Mark the bits to help with debugging.
//        std::memset(cursor, HEADPADDING_BYTE, sizeof(Align));
//        std::memset(cursor + sizeof(Align), UNINITIALIZED_BYTE, actualSize);
//
//        // Initialize the header for the block we will return.
//        ((Align *)cursor)->d_header.d_allocatedState = ALLOCATED_FLAG;
//        ((Align *)cursor)->d_header.d_protectedState = READWRITE_FLAG;
//        ((Align *)cursor)->d_header.d_size           = actualSize;
//
//        // Update the cursor to the next empty buffer location and then
//        // ensure the proper alignment of the next position.
//        d_cursor_p += sizeWithHeader;
//        d_cursor_p += bsls_Alignment::calculateAlignmentOffset(
//                                  d_cursor_p, bsls_Alignment::MAX_ALIGNMENT);
//
//        // Return the block of memory starting after the header.
//        return bdema_MemoryBlockDescriptor(
//                                         cursor + sizeof(Align), actualSize);
//    }
//
//    void DummyProtectableBufferBlockDispenser::deallocate(
//                                    const bdema_MemoryBlockDescriptor& block)
//    {
//        Align *align = (Align *)(((char *)block.address()) - sizeof(Align));
//        VERIFY(align->d_header.d_allocatedState == ALLOCATED_FLAG);
//        VERIFY(align->d_header.d_size           == block.size());
//        VERIFY(align->d_header.d_protectedState == READWRITE_FLAG);
//
//        align->d_header.d_allocatedState = DEALLOCATED_FLAG;
//        std::memset(block.address(), FREED_BYTE, block.size());
//    }
//..
// In order to avoid any system dependencies, implement 'protect' and
// 'unprotect' without providing system level access protection:
//..
//    int DummyProtectableBufferBlockDispenser::protect(
//                                    const bdema_MemoryBlockDescriptor& block)
//    {
//        Align *align = (Align *)(((char *)block.address()) - sizeof(Align));
//        VERIFY(align->d_header.d_allocatedState == ALLOCATED_FLAG);
//        VERIFY(align->d_header.d_size           == block.size());
//        VERIFY(align->d_header.d_protectedState == READWRITE_FLAG ||
//               align->d_header.d_protectedState == READONLY_FLAG);
//
//        align->d_header.d_protectedState = READONLY_FLAG;
//        return 0;
//    }
//
//    int DummyProtectableBufferBlockDispenser::unprotect(
//                                    const bdema_MemoryBlockDescriptor& block)
//    {
//        Align *align = (Align *)(((char *)block.address()) - sizeof(Align));
//        VERIFY(align->d_header.d_allocatedState == ALLOCATED_FLAG);
//        VERIFY(align->d_header.d_size           == block.size());
//        VERIFY(align->d_header.d_protectedState == READWRITE_FLAG ||
//               align->d_header.d_protectedState == READONLY_FLAG);
//
//        align->d_header.d_protectedState = READWRITE_FLAG;
//        return 0;
//    }
//
//    // ACCESSORS
//    int DummyProtectableBufferBlockDispenser::minimumBlockSize() const
//    {
//        return d_pageSize;
//    }
//
//    bool DummyProtectableBufferBlockDispenser::isProtected(
//                              const bdema_MemoryBlockDescriptor& block) const
//    {
//        // We use the byte address before the returned block to track the
//        // protection status.
//        Align *align = (Align *)(((char *)block.address()) - sizeof(Align));
//        VERIFY(align->d_header.d_allocatedState == ALLOCATED_FLAG);
//        VERIFY(align->d_header.d_size           == block.size());
//        VERIFY(align->d_header.d_protectedState == READWRITE_FLAG ||
//               align->d_header.d_protectedState == READONLY_FLAG);
//        return align->d_header.d_protectedState == READONLY_FLAG;
//    }
//
//  // ...
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_MEMORYBLOCKDESCRIPTOR
#include <bdema_memoryblockdescriptor.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

namespace BloombergLP {

                  // =====================================
                  // class bdema_ProtectableBlockDispenser
                  // =====================================

class bdema_ProtectableBlockDispenser {
    // This class provides a protocol for allocating and deallocating memory
    // blocks that can be protected and unprotected.  Memory that has been
    // "protected" is READ-ONLY and will cause a segmentation violation if
    // written to, while "unprotected" memory is READ-WRITE and can be written
    // to freely.  Both 'protect' and 'unprotect' must be passed a reference to
    // a 'bdema_MemoryBlockDescriptor' that was obtained from a call to
    // 'allocate' on the same dispenser object.

  public:
    // PUBLIC TYPES
    typedef bsls_PlatformUtil::size_type size_type;  // type for block size

    // CREATORS
    virtual ~bdema_ProtectableBlockDispenser();
        // Destroy this 'bdema_ProtectableBlockDispenser'.  Note that the
        // behavior of destroying an allocator while memory is allocated from
        // it is not specified.  (Unless you *know* that it is valid to do so,
        // don't!)

    // MANIPULATORS
    virtual bdema_MemoryBlockDescriptor allocate(size_type numBytes) = 0;
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

    virtual void deallocate(const bdema_MemoryBlockDescriptor& block) = 0;
        // Return the memory described by the specified 'block' to this
        // dispenser.  If 'block' is a null descriptor then this function has
        // no effect.  The behavior is undefined unless 'block' was returned
        // from a call to 'allocate()' on this
        // 'bdema_ProtectableBlockDispenser' object, has not since been
        // deallocated, and is currently in an unprotected state.

    virtual int protect(const bdema_MemoryBlockDescriptor& block) = 0;
        // Set the access protection for the specified 'block' to be READ-ONLY.
        // Return 0 on success and a non-zero value otherwise.  This method has
        // no effect if 'block' is a null descriptor or was already protected;
        // concrete implementations of the 'bdema_ProtectableBlockDispenser'
        // protocol may differ in the status value that is returned in those
        // two cases.  The behavior is undefined unless 'block' was returned
        // from a call to 'allocate()' on this object and has not since been
        // deallocated.

    virtual int unprotect(const bdema_MemoryBlockDescriptor& block) = 0;
        // Set the access protection for the specified 'block' to be
        // READ-WRITE.  Return 0 on success and a non-zero value otherwise.
        // This method has no effect if 'block' is a null descriptor or was
        // already unprotected; concrete implementations of the
        // 'bdema_ProtectableBlockDispenser' protocol may differ in the status
        // value that is returned in those two cases.  The behavior is
        // undefined unless 'block' was returned from a call to 'allocate()' on
        // this object and has not since been deallocated.

    // ACCESSORS
    virtual int minimumBlockSize() const = 0;
        // Return the minimum size of any block returned by this allocator.
        // Note that this value indicates the size of the block returned by
        // 'allocate(1)'.
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
