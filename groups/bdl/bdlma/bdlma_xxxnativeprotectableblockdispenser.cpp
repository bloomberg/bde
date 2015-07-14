// bdlma_xxxnativeprotectableblockdispenser.cpp    -*-C++-*-
#include <bdlma_xxxnativeprotectableblockdispenser.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlma_xxxnativeprotectableblockdispenser_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>

#ifdef TEST
#include <bslma_testallocator.h>                 // for testing only
#include <bslma_testallocatorexception.h>        // for testing only
#endif

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_bslexceptionutil.h>

#include <bsl_new.h>

#ifdef BSLS_PLATFORM_OS_UNIX
    #include <stdlib.h>    // 'valloc()'
    #include <sys/mman.h>  // 'mprotect()'
    #include <unistd.h>    // 'sysconf()'
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
    #include <windows.h>   // 'GetSystemInfo', 'VirtualAlloc', 'VirtualFree'
#endif

namespace BloombergLP {

namespace {

#ifdef BSLS_PLATFORM_OS_UNIX

int getUnixPageSizeHelper()
{
    return sysconf(_SC_PAGESIZE);
}

#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS

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
#ifdef BSLS_PLATFORM_OS_UNIX
                          getUnixPageSizeHelper();
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS
                          getWindowsPageSizeHelper();
#endif
   return pageSize;
}

inline
bsls::Types::size_type allocationSize(bsls::Types::size_type requestedSize)
// Return the size to allocate for the specified 'requestedSize' based on the
// system page size returned by 'getSystemPageSize'.
{
    const int pageSize = getSystemPageSize();
    const int numPages = (requestedSize + pageSize - 1) / pageSize;

    return numPages * pageSize;
}

}  // close unnamed namespace

namespace bdlma {
                  // -------------------------------------------
                  // class NativeProtectableBlockDispenser
                  // -------------------------------------------

// PRIVATE CLASS METHODS
inline
void *NativeProtectableBlockDispenser::initSingleton(
              bsls::ObjectBuffer<NativeProtectableBlockDispenser> *arena)
{
    // Thread-safe initialization of singleton.
    //
    // A 'NativeProtectableBlockDispenser' contains no data
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

    typedef bsls::ObjectBuffer<NativeProtectableBlockDispenser>
                               bdema_NativeProtectableBlockDispenser_Singleton;

    bdema_NativeProtectableBlockDispenser_Singleton stackTemp;
    void *v = new (&stackTemp) NativeProtectableBlockDispenser;
    // needed to make xlC8 work!
    *arena =
           *(static_cast<bdema_NativeProtectableBlockDispenser_Singleton*>(v));
    // bsls::ObjectBuffer<T> assignment is a bit-wise copy.
    return arena;
}

int NativeProtectableBlockDispenser::pageSize()
{
    return getSystemPageSize();
}

// PRIVATE CREATORS
NativeProtectableBlockDispenser::NativeProtectableBlockDispenser()
{
}

// CLASS METHODS
NativeProtectableBlockDispenser&
NativeProtectableBlockDispenser::singleton()
{
    // Initializing 'dummy' will cause 'initSingleton' to be called exactly
    // once.  The 'singleton' object itself has a no-op constructor.

    static bsls::ObjectBuffer<NativeProtectableBlockDispenser> singleton;
    static void *dummy = initSingleton(&singleton);
    (void) dummy;  // eliminate unused variable warning
    return singleton.object();
}

// CREATORS
NativeProtectableBlockDispenser::~NativeProtectableBlockDispenser()
{
}
}  // close package namespace

// MANIPULATORS

#ifdef BSLS_PLATFORM_OS_UNIX

namespace bdlma {
MemoryBlockDescriptor
NativeProtectableBlockDispenser::allocate(size_type size)
{
    if (0 == size) {
        return MemoryBlockDescriptor();
    }

    size_type actualSize = allocationSize(size);
    void *ptr = valloc(actualSize);

    if (0 == ptr) {
        bsls::BslExceptionUtil::throwBadAlloc();
    }

    return MemoryBlockDescriptor(ptr, actualSize);
}

void NativeProtectableBlockDispenser::deallocate(
                                      const MemoryBlockDescriptor& block)
{
    free(block.address());
}

int NativeProtectableBlockDispenser::protect(
                                      const MemoryBlockDescriptor& block)
{
    return mprotect(static_cast<char *>(block.address()),
                    block.size(),
                    PROT_READ);
}

int NativeProtectableBlockDispenser::unprotect(
                                      const MemoryBlockDescriptor& block)
{
    return mprotect(static_cast<char *>(block.address()),
                    block.size(),
                    PROT_READ | PROT_WRITE);
}
}  // close package namespace

#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS

namespace bdlma {
MemoryBlockDescriptor
NativeProtectableBlockDispenser::allocate(size_type size)
{
    BSLS_ASSERT(0 <= size);

    if (0 == size) {
        return MemoryBlockDescriptor();
    }

    size_type actualSize = allocationSize(size);
    void *ptr = VirtualAlloc(0,
                             actualSize,
                             MEM_COMMIT | MEM_RESERVE,
                             PAGE_READWRITE);

    if (0 == ptr) {
        bsls::BslExceptionUtil::throwBadAlloc();
    }

    return MemoryBlockDescriptor(ptr, actualSize);
}

void NativeProtectableBlockDispenser::deallocate(
                                      const MemoryBlockDescriptor& block)
{
    VirtualFree(block.address(), 0, MEM_RELEASE);
}

int NativeProtectableBlockDispenser::protect(
                                      const MemoryBlockDescriptor& block)
{
    DWORD oldProtect;

    return !VirtualProtect(block.address(),
                           block.size(),
                           PAGE_READONLY,
                           &oldProtect);
}

int NativeProtectableBlockDispenser::unprotect(
                                      const MemoryBlockDescriptor& block)
{
    DWORD oldProtect;

    return !VirtualProtect(block.address(),
                           block.size(),
                           PAGE_READWRITE,
                           &oldProtect);
}
}  // close package namespace

#endif

namespace bdlma {
int NativeProtectableBlockDispenser::minimumBlockSize() const
{
    return getSystemPageSize();
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
