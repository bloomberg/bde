// bdesu_memoryutil.h -*-C++-*-
#ifndef INCLUDED_BDESU_MEMORYUTIL
#define INCLUDED_BDESU_MEMORYUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a set of portable utilities for memory manipulation
//
//@CLASSES:
//      bdesu_MemoryUtil: struct which scopes memory system utilities.
//
//@SEE_ALSO: bdema_protectableblocklist,
//           bcema_protectablesequentialallocator
//
//@AUTHOR: Andrei Basov (abasov), Oleg Semenov (osemenov)
//
//@DESCRIPTION: This component defines a platform-independent interface for
// memory manipulation, providing utilities for querying page size,
// allocating/deallocating page-aligned memory, and utility to change memory
// protection.
//
///Usage
///-----
// First, allocate one page of memory.
//..
//  int pageSize = bdesu_MemoryUtil::pageSize();
//  char *data = (char*)bdesu_MemoryUtil::allocate(pageSize);
//..
// Write into the allocated buffer.
//..
//  data[0] = '1';
//..
//
// Make the memory write protected
//..
//  bdesu_MemoryUtil::protect(data, pageSize,
//                            bdesu_MemoryUtil::BDESU_ACCESS_READ);
//..
//
// Verify that data still could be read.
//..
//  assert('1' == data[0]);
//..
//
// Once again, try writing into the buffer.  This should crash our process.
//..
//  data[0] = '2';
//..
//
// Restore read/write access and free the allocated memory.  Actually, this
// will never be executed, as the process has already crashed.
//..
//  bdesu_MemoryUtil::protect(data, pageSize,
//                            bdesu_MemoryUtil::BDESU_ACCESS_READ_WRITE);
//  bdesu_MemoryUtil::free(data);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

namespace BloombergLP {

                       // =======================
                       // struct bdesu_MemoryUtil
                       // =======================

struct bdesu_MemoryUtil {

    // TYPES
    enum Access {
        BDESU_ACCESS_NONE          = 0,
        BDESU_ACCESS_READ          = 0x1,
        BDESU_ACCESS_WRITE         = 0x2,
        BDESU_ACCESS_EXECUTE       = 0x4,
        BDESU_ACCESS_READ_WRITE    = BDESU_ACCESS_READ | BDESU_ACCESS_WRITE,
        BDESU_ACCESS_READ_EXECUTE  = BDESU_ACCESS_READ | BDESU_ACCESS_EXECUTE,
        BDESU_ACCESS_WRITE_EXECUTE = BDESU_ACCESS_WRITE | BDESU_ACCESS_EXECUTE,
        BDESU_ACCESS_READ_WRITE_EXECUTE
                                   = BDESU_ACCESS_READ | BDESU_ACCESS_WRITE
                                   | BDESU_ACCESS_EXECUTE

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
      , ACCESS_READ               = BDESU_ACCESS_READ
      , ACCESS_WRITE              = BDESU_ACCESS_WRITE
      , ACCESS_EXECUTE            = BDESU_ACCESS_EXECUTE
      , ACCESS_NONE               = BDESU_ACCESS_NONE
      , ACCESS_READ_WRITE         = BDESU_ACCESS_READ_WRITE
      , ACCESS_READ_EXECUTE       = BDESU_ACCESS_READ_EXECUTE
      , ACCESS_WRITE_EXECUTE      = BDESU_ACCESS_WRITE_EXECUTE
      , ACCESS_READ_WRITE_EXECUTE = BDESU_ACCESS_READ_WRITE_EXECUTE
#endif
    };

    // CLASS METHODS
    static int pageSize();
        // Return the memory page size of the platform.

    static int protect(void *address, int numBytes, int mode);
        // Change the access protection on a region of memory starting at
        // the specified 'address' and 'numBytes' long, according to
        // specified 'mode', making memory readable if '(mode &
        // ACCESS_READ)' is nonzero and writable if '(mode & ACCESS_WRITE)' is
        // nonzero.  Return 0 on success, and a nonzero value otherwise.  The
        // behavior is undefined if 'addr' is not aligned on a page boundary,
        // if 'numBytes' is not a multiple of 'pageSize()', or if 'numBytes' is
        // 0.  Note that some platforms do not support certain protection
        // modes, e.g.,  on some platforms the memory cannot be made writable
        // but unreadable, or readable but non-executable.  On these platforms
        // the actual access protection set on the region might be more
        // permissive than the specified one.  Also note that most memory
        // allocators do not expect memory protection on allocated memory to be
        // changed, so you must reset protection back to ACCESS_READ_WRITE
        // before releasing the memory.

    static void *allocate(int numBytes);
        // Allocate an area of memory of size 'numBytes', aligned on a page
        // boundary.  Return a pointer to allocated memory on success, and a
        // null pointer otherwise.  Note that the allocated memory is
        // readable and writable, and read/write access to this memory, if
        // revoked, must be restored before deallocation.

    static int deallocate(void *address);
        // Deallocate a memory area at the specified 'address' previously
        // allocated with the 'allocate' method.  Return 0 on success, and a
        // nonzero value otherwise.  The behavior is undefined if read or write
        // access to any memory in this area has been revoked and not restored.
        // Note that deallocating memory does not change memory protection.
};

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
