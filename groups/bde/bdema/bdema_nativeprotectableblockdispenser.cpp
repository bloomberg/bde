// bdema_nativeprotectableblockdispenser.cpp    -*-C++-*-
#include <bdema_nativeprotectableblockdispenser.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_nativeprotectableblockdispenser_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>

#ifdef TEST
#include <bslma_testallocator.h>                 // for testing only
#include <bslma_testallocatorexception.h>        // for testing only
#endif

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>

#include <bsl_new.h>

#ifdef BSLS_PLATFORM__OS_UNIX
    #include <stdlib.h>    // 'valloc()'
    #include <sys/mman.h>  // 'mprotect()'
    #include <unistd.h>    // 'sysconf()'
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS
    #include <windows.h>   // 'GetSystemInfo', 'VirtualAlloc', 'VirtualFree'
#endif

namespace BloombergLP {

namespace {

#ifdef BSLS_PLATFORM__OS_UNIX

int getUnixPageSizeHelper()
{
    return sysconf(_SC_PAGESIZE);
}

#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS

int getWindowsPageSizeHelper()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
}

#endif

inline
int getSystemPageSize()
{
    static int pageSize =
#ifdef BSLS_PLATFORM__OS_UNIX
                          getUnixPageSizeHelper();
#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS
                          getWindowsPageSizeHelper();
#endif
   return pageSize;
}

inline
bsls_PlatformUtil::size_type allocationSize(
                                    bsls_PlatformUtil::size_type requestedSize)
// Return the size to allocate for the specified 'requestedSize' based on the
// system page size returned by 'getSystemPageSize'.
{
    const int pageSize = getSystemPageSize();
    const int numPages = (requestedSize + pageSize - 1) / pageSize;

    return numPages * pageSize;
}

}  // close unnamed namespace

                  // -------------------------------------------
                  // class bdema_NativeProtectableBlockDispenser
                  // -------------------------------------------

// PRIVATE CLASS METHODS
inline
void *bdema_NativeProtectableBlockDispenser::initSingleton(
               bsls_ObjectBuffer<bdema_NativeProtectableBlockDispenser> *arena)
{
    // Thread-safe initialization of singleton.
    //
    // A 'bdema_NativeProtectableBlockDispenser' contains no data
    // members but does contain a vtbl pointer.  During construction, the vtbl
    // pointer is first set to the base class's vtbl before it is set to its
    // final, derived-class value.  If two threads try to initialize the same
    // singleton, the one that finishes first may be in for a rude awakening
    // as the second thread temporarily changes the vtbl ptr to point to the
    // base class's vtbl.
    //
    // We solve this problem by initializing the singleton on the stack and
    // then bit-copying it into its final location.  The stack initialization
    // is safe because each thread has its own stack.  The bit-copy is safe
    // because the only thing that is being copied is the vtbl pointer, which
    // is the same for all threads.  (I.e., in case of a race, the second
    // thread copies the exact same data over the results of the first
    // thread's copy.)

    typedef bsls_ObjectBuffer<bdema_NativeProtectableBlockDispenser>
                               bdema_NativeProtectableBlockDispenser_Singleton;

    bdema_NativeProtectableBlockDispenser_Singleton stackTemp;
    void *v = new (&stackTemp) bdema_NativeProtectableBlockDispenser;
    // needed to make xlC8 work!
    *arena =
           *(static_cast<bdema_NativeProtectableBlockDispenser_Singleton*>(v));
    // bsls_ObjectBuffer<T> assignment is a bit-wise copy.
    return arena;
}

int bdema_NativeProtectableBlockDispenser::pageSize()
{
    return getSystemPageSize();
}

// PRIVATE CREATORS
bdema_NativeProtectableBlockDispenser::bdema_NativeProtectableBlockDispenser()
{
}

// CLASS METHODS
bdema_NativeProtectableBlockDispenser&
bdema_NativeProtectableBlockDispenser::singleton()
{
    // Initializing 'dummy' will cause 'initSingleton' to be called exactly
    // once.  The 'singleton' object itself has a no-op constructor.

    static bsls_ObjectBuffer<bdema_NativeProtectableBlockDispenser> singleton;
    static void *dummy = initSingleton(&singleton);
    (void) dummy;  // eliminate unused variable warning
    return singleton.object();
}

// CREATORS
bdema_NativeProtectableBlockDispenser::~bdema_NativeProtectableBlockDispenser()
{
}

// MANIPULATORS

#ifdef BSLS_PLATFORM__OS_UNIX

bdema_MemoryBlockDescriptor
bdema_NativeProtectableBlockDispenser::allocate(size_type size)
{
    BSLS_ASSERT(0 <= size);

    if (0 == size) {
        return bdema_MemoryBlockDescriptor();
    }

    size_type actualSize = allocationSize(size);
    void *ptr = valloc(actualSize);

    if (0 == ptr) {
        bslma_Allocator::throwBadAlloc();
    }

    return bdema_MemoryBlockDescriptor(ptr, actualSize);
}

void bdema_NativeProtectableBlockDispenser::deallocate(
                                      const bdema_MemoryBlockDescriptor& block)
{
    free(block.address());
}

int bdema_NativeProtectableBlockDispenser::protect(
                                      const bdema_MemoryBlockDescriptor& block)
{
    return mprotect(static_cast<char *>(block.address()),
                    block.size(),
                    PROT_READ);
}

int bdema_NativeProtectableBlockDispenser::unprotect(
                                      const bdema_MemoryBlockDescriptor& block)
{
    return mprotect(static_cast<char *>(block.address()),
                    block.size(),
                    PROT_READ | PROT_WRITE);
}

#endif

#ifdef BSLS_PLATFORM__OS_WINDOWS

bdema_MemoryBlockDescriptor
bdema_NativeProtectableBlockDispenser::allocate(size_type size)
{
    BSLS_ASSERT(0 <= size);

    if (0 == size) {
        return bdema_MemoryBlockDescriptor();
    }

    size_type actualSize = allocationSize(size);
    void *ptr = VirtualAlloc(0,
                             actualSize,
                             MEM_COMMIT | MEM_RESERVE,
                             PAGE_READWRITE);

    if (0 == ptr) {
        bslma_Allocator::throwBadAlloc();
    }

    return bdema_MemoryBlockDescriptor(ptr, actualSize);
}

void bdema_NativeProtectableBlockDispenser::deallocate(
                                      const bdema_MemoryBlockDescriptor& block)
{
    VirtualFree(block.address(), 0, MEM_RELEASE);
}

int bdema_NativeProtectableBlockDispenser::protect(
                                      const bdema_MemoryBlockDescriptor& block)
{
    DWORD oldProtect;

    return !VirtualProtect(block.address(),
                           block.size(),
                           PAGE_READONLY,
                           &oldProtect);
}

int bdema_NativeProtectableBlockDispenser::unprotect(
                                      const bdema_MemoryBlockDescriptor& block)
{
    DWORD oldProtect;

    return !VirtualProtect(block.address(),
                           block.size(),
                           PAGE_READWRITE,
                           &oldProtect);
}

#endif

int bdema_NativeProtectableBlockDispenser::minimumBlockSize() const
{
    return getSystemPageSize();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
