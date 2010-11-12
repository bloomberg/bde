// bcemt_threadimpl_win32.cpp                -*-C++-*-

#include <bcemt_threadimpl_win32.h>

#ifdef BCES_PLATFORM__WIN32_THREADS

#include <process.h>     // _begintthreadex, _endthreadex

namespace BloombergLP {

const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle
    bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::INVALID_HANDLE
    = { INVALID_HANDLE_VALUE, 0 };

struct ThreadStartupInfo{
    // Control structure used to pass startup information to the thread entry
    // function.
    bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle d_handle;
    bcemt_ThreadFunction d_function;
    void  *d_threadArg;
    ThreadStartupInfo *d_next;
};

struct ThreadSpecificDestructor {
    // This structure implements a linked list of destructors associated
    // with thread-specific key.
	bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Key d_key;
    bcemt_KeyDestructorFunction d_destructor;
    ThreadSpecificDestructor  *d_next;
};

struct bcemt_Win32Initializer{
    // This structure is used to initializ and de-initialize the BCE threading
    // environment.  At creation, 'bcemt_threadimpl_win32_Initialize' is
    // called to initialize the environment.  When the object is destroyed,
    // it calls 'bcemt_threadimpl_win32_Deinitialize' to cleanup the
    // environment.

    bcemt_Win32Initializer();
        // Initialize the BCE threading environment.

    ~bcemt_Win32Initializer();
        // Deinitialize the BCE threading environment.
};

enum InitializationState {
    INITIALIZED = 0      // The threading enviroment has been initialized
    , UNINITIALIZED = 1  // The threading enviroment has not been initialized
    , INITIALIZING = 2   // The threading enviroment is currently initializing
    , DEINITIALIZED = -1 // The threading enviroment has been de-initialized.
};


static volatile ThreadStartupInfo *startupInfoCache=0;
static DWORD ThreadInfoTLSIndex=0;
static ThreadSpecificDestructor *Destructors=0;
static CRITICAL_SECTION threadSpecificDestructorsListLock;
static volatile long initializationState=UNINITIALIZED;
static bcemt_Win32Initializer initializer;

static inline int bcemt_threadimpl_win32_Initialize()
    // This function is used to initialize the BCE threading environment If
    // the environment has already been initialized, it returns immediately
    // with a 0 result.  Otherwise if the enviroment is currently being
    // initialized from some other thread, then it waits until the environment
    // is initialized and returns.
{
    if (initializationState == INITIALIZED) return 0;
    else {
        long res;
        do {
            res = InterlockedCompareExchange(&initializationState,
                                             INITIALIZING,
                                             UNINITIALIZED);
            if (INITIALIZING == res) ::Sleep(0);
            else break;
        } while (1);

        if (res == UNINITIALIZED) {
            ThreadInfoTLSIndex = TlsAlloc();
            InitializeCriticalSection(&threadSpecificDestructorsListLock);
            InterlockedExchange(&initializationState, INITIALIZED);
        }
        return initializationState == INITIALIZED ? 0 : 1;
    }
}

static void bcemt_threadimpl_win32_Deinitialize()
    // This function de-initializes the BCE threading enviroment and releases
    // all resources allocated by the environment.  Note that once the
    // environment has been de-initialized, it cannot be re-initialized.  This
    // prevents static objects from inadvertently re-initializing
    // re-initializing the environment when they are destroyed.
{
    if (InterlockedExchange(&initializationState, DEINITIALIZED) !=
        INITIALIZED) return;

    TlsFree(ThreadInfoTLSIndex);

    EnterCriticalSection(&threadSpecificDestructorsListLock);
    ThreadSpecificDestructor *d=Destructors;
    Destructors = 0;
    LeaveCriticalSection(&threadSpecificDestructorsListLock);
    DeleteCriticalSection(&threadSpecificDestructorsListLock);

    while (d) {
      ThreadSpecificDestructor *t=d;
      d = d->d_next;
      delete t;
    }

    ThreadStartupInfo *head ;
    head = (ThreadStartupInfo*)InterlockedExchange(
                                        (volatile LONG*)&startupInfoCache, 0);
    while (head) {
      ThreadStartupInfo *t = head;
      head = head->d_next;
      delete t;
    }
}

bcemt_Win32Initializer::bcemt_Win32Initializer()
{
    bcemt_threadimpl_win32_Initialize();
}

bcemt_Win32Initializer::~bcemt_Win32Initializer()
{
    bcemt_threadimpl_win32_Deinitialize();
}

static ThreadStartupInfo* allocStartupInfo()
    // This function provides an efficient allocator for 'ThreadStartupInfo'
    // objects.  The implementation uses atomic operations to manage a pool
    // objects without any locking overhead.
{
    ThreadStartupInfo *head;
    head = (ThreadStartupInfo*)InterlockedExchangeAdd(
                                      (volatile LONG*)&startupInfoCache, 0);
    while (head) {
        LONG t;
        t = InterlockedCompareExchange((volatile LONG*)&startupInfoCache,
                                        (LONG)head->d_next, (LONG)head);
        if (t == (LONG)head) break;
        else head = (ThreadStartupInfo*)t;
    }
    if (!head)  head = new ThreadStartupInfo;
    return head;
}

static void freeStartupInfo(ThreadStartupInfo* item)
    // This function provides an efficient deallocator for 'ThreadStartupInfo'
    // objects.  The implementation uses atomic operations to manage a pool
    // objects without any locking overhead.
{
    item->d_next = 0;

    while(1) {
        ThreadStartupInfo *t;
        t = (ThreadStartupInfo*)InterlockedCompareExchange(
                                          (volatile LONG*)&startupInfoCache,
                                          (LONG)item, (LONG)item->d_next);
        if (t == item->d_next) break;
        item->d_next = t;
    }
}

static void invokeDestructors()
    // This function is called when a BCE thread terminates to clean up any
    // thread specific data that have associated destructor functions.  It
    // iterates through all registered destructor functions and invokes each
    // destructor that has a non-zero key value.
{
    if (initializationState != INITIALIZED) return;

    EnterCriticalSection(&threadSpecificDestructorsListLock);
    ThreadSpecificDestructor *d = Destructors;
    while (d) {
        void * value = TlsGetValue(d->d_key);
        if (value) d->d_destructor(value);
        d = d->d_next;
    }
    LeaveCriticalSection(&threadSpecificDestructorsListLock);
}

static unsigned _stdcall ThreadEntry( void *arg )
    // This function is the entrypoint for all BCE thead functions.
{
    ThreadStartupInfo   startInfo;

    std::memcpy(&startInfo, arg, sizeof(startInfo));

    freeStartupInfo((ThreadStartupInfo*)arg);
    TlsSetValue(ThreadInfoTLSIndex, &startInfo.d_handle);
    void *ret = startInfo.d_function(startInfo.d_threadArg);
    invokeDestructors();
    return (unsigned)ret;
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::create(
         bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle *handle,
         const bcemt_AttributeImpl<bces_Platform::Win32Threads>& attribute,
         bcemt_ThreadFunction function,
         void *userData)
{
    if (bcemt_threadimpl_win32_Initialize()) return 1;

    ThreadStartupInfo *startInfo = allocStartupInfo();

    startInfo->d_threadArg      = userData;
    startInfo->d_function       = function;
    handle->d_handle = (HANDLE)_beginthreadex(0, attribute.stackSize(),
                                              ThreadEntry, startInfo, 1,
                                              (unsigned int*)&handle->d_id );
    if (handle->d_handle == (HANDLE)-1) {
        freeStartupInfo(startInfo);
        return( 1 );
    }
    if (attribute.detachedState() ==
        bcemt_AttributeImpl<bces_Platform::Win32Threads>::CREATE_DETACHED ) {
	    HANDLE tmpHandle = handle->d_handle;
	    handle->d_handle  = 0;
	    startInfo->d_handle = *handle;
	    ResumeThread(tmpHandle);
	    CloseHandle(tmpHandle);
    } else {
	    startInfo->d_handle = *handle;
	    ResumeThread(handle->d_handle);
    }
    return 0;
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::join(
           bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle &handle,
           void **status)
{
    // Cannot self - join
    if(!handle.d_handle || handle.d_id == GetCurrentThreadId()) {
        return 1;
    }
    if (WaitForSingleObject(handle.d_handle, INFINITE) != WAIT_OBJECT_0) {
        return 2;
    }
    DWORD exitStatus;
    DWORD res = GetExitCodeThread(handle.d_handle,&exitStatus);
    if(status) *status = (void*)exitStatus;
    CloseHandle(handle.d_handle);
    handle.d_handle = 0;
    return( res == FALSE ? 3 : 0 );
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::detach(
             bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle &handle)
{
    if (handle.d_handle == GetCurrentThread() && 
	    handle.d_id == GetCurrentThreadId()) {
        bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle *realHandle;
        realHandle = 
	    (bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle*)
                 TlsGetValue(ThreadInfoTLSIndex);
	    if (!realHandle || !realHandle->d_handle) return 1;
	    if (!CloseHandle(realHandle->d_handle) ) 
	        return 1;
	    handle.d_handle = 0;
	    return 0;
    }
    else if (handle.d_handle) {
	if (!CloseHandle(handle.d_handle) ) 
	    return 1;
	handle.d_handle = 0;
	return 0;
    }
    return 2;
}

void bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::exit(void *status)
{
    invokeDestructors();
    _endthreadex((unsigned)status);
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::createKey(
                                                                      Key *key,
                                        bcemt_KeyDestructorFunction destructor)
{
    // It is not uncommon for applications to have global object that make
    // calls to create a thread-specific key.  It is possible that those
    // objects are initialized before the BCE threading environemt, so make
    // sure to do so first.
    if (bcemt_threadimpl_win32_Initialize()) return 1;

    *key = TlsAlloc();
    if (*key == TLS_OUT_OF_INDEXES) return 1;
    if (destructor) {
        ThreadSpecificDestructor *d = new ThreadSpecificDestructor;
        d->d_key = *key;
        d->d_destructor = destructor;
        EnterCriticalSection(&threadSpecificDestructorsListLock);
        d->d_next = Destructors;
        Destructors = d;
        LeaveCriticalSection(&threadSpecificDestructorsListLock);
    }
    return 0;
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::deleteKey(Key &key)
{
    ThreadSpecificDestructor *d = Destructors;
    ThreadSpecificDestructor *prev = 0;
    if (!TlsFree(key)) return 1;
    EnterCriticalSection(&threadSpecificDestructorsListLock);
    while (d) {
        if (d->d_key == key) {
            if (prev) {
                prev->d_next = d->d_next;
            }
            else {
                Destructors = d->d_next;
            }
            delete d;
            break;
        }
        prev = d;
        d = d->d_next;
    }
    LeaveCriticalSection(&threadSpecificDestructorsListLock);
    return 0;
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::isEqual(
          const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle &lhs,
          const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle &rhs)
{
    if (lhs.d_id == rhs.d_id) return 1;
    return 0;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
