// bdls_memoryutil.h                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLS_MEMORYUTIL
#define INCLUDED_BDLS_MEMORYUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a set of portable utilities for memory manipulation.
//
//@CLASSES:
//      bdls::MemoryUtil: struct which scopes memory system utilities.
//
//@DESCRIPTION: This component, 'bdls::MemoryUtil', defines a
// platform-independent interface for memory manipulation, providing utilities
// for querying page size, allocating/deallocating page-aligned memory, and
// utility to change memory protection.
//
///Usage
///-----
// First, allocate one page of memory.
//..
//  int pageSize = bdls::MemoryUtil::pageSize();
//  char *data = (char*)bdls::MemoryUtil::allocate(pageSize);
//..
// Write into the allocated buffer.
//..
//  data[0] = '1';
//..
//
// Make the memory write protected
//..
//  bdls::MemoryUtil::protect(data, pageSize,
//                            bdls::MemoryUtil::k_ACCESS_READ);
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
//  bdls::MemoryUtil::protect(data, pageSize,
//                            bdls::MemoryUtil::k_ACCESS_READ_WRITE);
//  bdls::MemoryUtil::free(data);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

namespace BloombergLP {

namespace bdls {
                             // =================
                             // struct MemoryUtil
                             // =================

struct MemoryUtil {

    // TYPES
    enum Access {
        k_ACCESS_NONE          = 0,
        k_ACCESS_READ          = 0x1,
        k_ACCESS_WRITE         = 0x2,
        k_ACCESS_EXECUTE       = 0x4,
        k_ACCESS_READ_WRITE    = k_ACCESS_READ | k_ACCESS_WRITE,
        k_ACCESS_READ_EXECUTE  = k_ACCESS_READ | k_ACCESS_EXECUTE,
        k_ACCESS_WRITE_EXECUTE = k_ACCESS_WRITE | k_ACCESS_EXECUTE,
        k_ACCESS_READ_WRITE_EXECUTE
                                   = k_ACCESS_READ | k_ACCESS_WRITE
                                   | k_ACCESS_EXECUTE

    };

    // CLASS METHODS
    static int pageSize();
        // Return the memory page size of the platform.

    static int protect(void *address, int numBytes, int mode);
        // Change the access protection on a region of memory starting at the
        // specified 'address' and 'numBytes' long, according to specified
        // 'mode', making memory readable if '(mode & ACCESS_READ)' is nonzero
        // and writable if '(mode & ACCESS_WRITE)' is nonzero.  Return 0 on
        // success, and a nonzero value otherwise.  The behavior is undefined
        // if 'addr' is not aligned on a page boundary, if 'numBytes' is not a
        // multiple of 'pageSize()', or if 'numBytes' is 0.  Note that some
        // platforms do not support certain protection modes, e.g., on some
        // platforms the memory cannot be made writable but unreadable, or
        // readable but non-executable.  On these platforms the actual access
        // protection set on the region might be more permissive than the
        // specified one.  Also note that most memory allocators do not expect
        // memory protection on allocated memory to be changed, so you must
        // reset protection back to ACCESS_READ_WRITE before releasing the
        // memory.

    static void *allocate(int numBytes);
        // Allocate an area of memory of the specified size 'numBytes', aligned
        // on a page boundary.  Return a pointer to allocated memory on
        // success, and a null pointer otherwise.  Note that the allocated
        // memory is readable and writable, and read/write access to this
        // memory, if revoked, must be restored before deallocation.

    static int deallocate(void *address);
        // Deallocate a memory area at the specified 'address' previously
        // allocated with the 'allocate' method.  Return 0 on success, and a
        // nonzero value otherwise.  The behavior is undefined if read or write
        // access to any memory in this area has been revoked and not restored.
        // Note that deallocating memory does not change memory protection.
};
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
