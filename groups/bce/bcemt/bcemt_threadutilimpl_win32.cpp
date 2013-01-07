// bcemt_threadutilimpl_win32.cpp                                     -*-C++-*-
#include <bcemt_threadutilimpl_win32.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadutilimpl_win32_cpp,"$Id$ $CSID$")

#ifdef BCES_PLATFORM_WIN32_THREADS

#include <windows.h>

#include <bcemt_configuration.h>
#include <bcemt_threadattributes.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>  // 'memcpy'

#include <process.h>      // '_begintthreadex', '_endthreadex'

#if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BSLS_PLATFORM_CPU_64_BIT)
    // On 64-bit Windows, we have to deal with the fact that Windows ThreadProc
    // thread procedures only return a 32-bit DWORD value.  We use an
    // intermediate map to store the actual 'void *' return or exit values,
    // so they can be retrieved by join.

    #define BCEMT_USE_RETURN_VALUE_MAP
    #include <bsl_map.h>

#endif

namespace BloombergLP {

// CLASS DATA
const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle
    bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::INVALID_HANDLE =
                                                   { INVALID_HANDLE_VALUE, 0 };

struct ThreadStartupInfo{
    // Control structure used to pass startup information to the thread entry
    // function.

    bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle  d_handle;
    bcemt_ThreadFunction                                       d_function;
    void                                                      *d_threadArg;
    ThreadStartupInfo                                         *d_next;
};

struct ThreadSpecificDestructor {
    // This structure implements a linked list of destructors associated
    // with thread-specific key.

    bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Key  d_key;
    bcemt_KeyDestructorFunction                             d_destructor;
    ThreadSpecificDestructor                               *d_next;
};

struct bcemt_Win32Initializer {
    // This structure is used to initialize and de-initialize the BCE threading
    // environment.  At creation, 'bcemt_threadutil_win32_Initialize' is
    // called to initialize the environment.  When the object is destroyed,
    // it calls 'bcemt_threadutil_win32_Deinitialize' to cleanup the
    // environment.

    bcemt_Win32Initializer();
        // Initialize the BCE threading environment.

    ~bcemt_Win32Initializer();
        // De-initialize the BCE threading environment.
};

enum InitializationState {
    INITIALIZED   =  0  // threading environment has been initialized
  , UNINITIALIZED =  1  // threading environment has not been initialized
  , INITIALIZING  =  2  // threading environment is currently initializing
  , DEINITIALIZED = -1  // threading environment has been de-initialized
};

static void *volatile            s_startupInfoCache = 0;
static DWORD                     s_threadInfoTLSIndex = 0;
static ThreadSpecificDestructor *s_destructors = 0;
static CRITICAL_SECTION          s_threadSpecificDestructorsListLock;
static volatile long             s_initializationState = UNINITIALIZED;

#ifdef BCEMT_USE_RETURN_VALUE_MAP
static CRITICAL_SECTION          s_returnValueMapLock;

struct HandleLess {
    bool operator()(HANDLE x, HANDLE y) const
        // compare two 'HANDLE' objects
    {
        return (unsigned long long)(x) < (unsigned long long)(y);
    }
};

// Access to this map will be serialized with 's_returnValueMapLock'.  It must
// be declared before 's_initializer' - the 's_initializer' destructor will
// empty the map.  's_returnValueMapValid' will be false if this module's
// static initialization has not run, or static cleanup has occurred.

static volatile bool            s_returnValueMapValid = false;
typedef bsl::map<DWORD, void *> TReturnValueMap;
static TReturnValueMap          s_returnValueMap;
#endif

static bcemt_Win32Initializer   s_initializer;

static inline
int bcemt_threadutil_win32_Initialize()
    // This function is used to initialize the BCE threading environment If
    // the environment has already been initialized, it returns immediately
    // with a 0 result.  Otherwise if the environment is currently being
    // initialized from some other thread, then it waits until the environment
    // is initialized and returns.
{
    if (INITIALIZED == s_initializationState) {
        return 0;                                                     // RETURN
    }
    else {
        long result;
        do {
            result = InterlockedCompareExchange(&s_initializationState,
                                                INITIALIZING,
                                                UNINITIALIZED);
            if (INITIALIZING == result) {
                ::Sleep(0);
            }
            else {
                break;
            }
        } while (1);

        if (UNINITIALIZED == result) {
            s_threadInfoTLSIndex = TlsAlloc();
            InitializeCriticalSection(&s_threadSpecificDestructorsListLock);
#ifdef BCEMT_USE_RETURN_VALUE_MAP
            InitializeCriticalSection(&s_returnValueMapLock);
            s_returnValueMapValid = true;
#endif
            InterlockedExchange(&s_initializationState, INITIALIZED);
        }
        return INITIALIZED == s_initializationState ? 0 : 1;          // RETURN
    }
}

static void bcemt_threadutil_win32_Deinitialize()
    // This function de-initializes the BCE threading environment and releases
    // all resources allocated by the environment.  Note that once the
    // environment has been de-initialized, it cannot be re-initialized.  This
    // prevents static objects from inadvertently re-initializing
    // re-initializing the environment when they are destroyed.
{
    if (InterlockedExchange(&s_initializationState, DEINITIALIZED)
                                                              != INITIALIZED) {
        return;                                                       // RETURN
    }

    TlsFree(s_threadInfoTLSIndex);

    EnterCriticalSection(&s_threadSpecificDestructorsListLock);
    ThreadSpecificDestructor *d = s_destructors;
    s_destructors = 0;
    LeaveCriticalSection(&s_threadSpecificDestructorsListLock);
    DeleteCriticalSection(&s_threadSpecificDestructorsListLock);

#ifdef BCEMT_USE_RETURN_VALUE_MAP
    EnterCriticalSection(&s_returnValueMapLock);
    s_returnValueMap.erase(s_returnValueMap.begin(), s_returnValueMap.end());
    s_returnValueMapValid = false;
    LeaveCriticalSection(&s_returnValueMapLock);
    // NOT deleted, so static objects can spawn threads.
    // DeleteCriticalSection(&s_returnValueMapLock);
#endif

    while (d) {
        ThreadSpecificDestructor *t = d;
        d = d->d_next;
        delete t;
    }

    ThreadStartupInfo *head;
    head = (ThreadStartupInfo *)InterlockedExchangePointer(&s_startupInfoCache,
                                                           0);
    while (head) {
        ThreadStartupInfo *t = head;
        head = head->d_next;
        delete t;
    }
}

bcemt_Win32Initializer::bcemt_Win32Initializer()
{
    bcemt_threadutil_win32_Initialize();
}

bcemt_Win32Initializer::~bcemt_Win32Initializer()
{
    bcemt_threadutil_win32_Deinitialize();
}

static ThreadStartupInfo *allocStartupInfo()
    // This function provides an efficient allocator for 'ThreadStartupInfo'
    // objects.  The implementation uses atomic operations to manage a pool
    // objects without any locking overhead.
{
    ThreadStartupInfo *head;
    head = (ThreadStartupInfo *)InterlockedCompareExchangePointer(
                                                    &s_startupInfoCache, 0, 0);
    while (head) {
        void *t;
        t = InterlockedCompareExchangePointer(&s_startupInfoCache,
                                              head->d_next, head);
        if (t == head) {
            break;
        }
        else {
            head = (ThreadStartupInfo *)t;
        }
    }
    if (!head) {
        head = new ThreadStartupInfo;
    }
    return head;
}

static void freeStartupInfo(ThreadStartupInfo *item)
    // This function provides an efficient deallocator for 'ThreadStartupInfo'
    // objects.  The implementation uses atomic operations to manage a pool
    // objects without any locking overhead.
{
    item->d_next = 0;

    while (1) {
        ThreadStartupInfo *t;
        t = (ThreadStartupInfo*)InterlockedCompareExchangePointer(
                                                           &s_startupInfoCache,
                                                           item,
                                                           item->d_next);
        if (t == item->d_next) {
            break;
        }
        item->d_next = t;
    }
}

static void invokeDestructors()
    // This function is called when a BCE thread terminates to clean up any
    // thread specific data that have associated destructor functions.  It
    // iterates through all registered destructor functions and invokes each
    // destructor that has a non-zero key value.
{
    if (s_initializationState != INITIALIZED) {
        return;                                                       // RETURN
    }

    EnterCriticalSection(&s_threadSpecificDestructorsListLock);
    ThreadSpecificDestructor *d = s_destructors;
    while (d) {
        void *value = TlsGetValue(d->d_key);
        if (value) {
            d->d_destructor(value);
        }
        d = d->d_next;
    }
    LeaveCriticalSection(&s_threadSpecificDestructorsListLock);
}

static unsigned _stdcall ThreadEntry(void *arg)
    // This function is the entry point for all BCE thread functions.
{
    ThreadStartupInfo startInfo;

    bsl::memcpy(&startInfo, arg, sizeof(startInfo));

    freeStartupInfo((ThreadStartupInfo *)arg);
    TlsSetValue(s_threadInfoTLSIndex, &startInfo.d_handle);
    void *ret = startInfo.d_function(startInfo.d_threadArg);
    invokeDestructors();
#ifdef BCEMT_USE_RETURN_VALUE_MAP
    EnterCriticalSection(&s_returnValueMapLock);
    if (s_returnValueMapValid) {
        s_returnValueMap[startInfo.d_handle.d_id] = ret;
    }
    LeaveCriticalSection(&s_returnValueMapLock);
#endif
    return (unsigned)(bsls_PlatformUtil::IntPtr)ret;
}

            // -------------------------------------------------------
            // class bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>
            // -------------------------------------------------------

// CLASS METHODS
int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::create(
                                                Handle               *thread,
                                                bcemt_ThreadFunction  function,
                                                void                 *userData)
{
    bcemt_ThreadAttributes attribute;
    return create(thread, attribute, function, userData);
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::create(
          bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle *handle,
          const bcemt_ThreadAttributes&                              attribute,
          bcemt_ThreadFunction                                       function,
          void                                                      *userData)
{
    if (bcemt_threadutil_win32_Initialize()) {
        return 1;                                                     // RETURN
    }

    ThreadStartupInfo *startInfo = allocStartupInfo();

    int stackSize = attribute.stackSize();
    if (bcemt_ThreadAttributes::BCEMT_UNSET_STACK_SIZE == stackSize) {
        stackSize = bcemt_Configuration::defaultThreadStackSize();
        if (bcemt_ThreadAttributes::BCEMT_UNSET_STACK_SIZE == stackSize) {
            stackSize = bcemt_Configuration::nativeDefaultThreadStackSize();
        }
    }

    BSLS_ASSERT_OPT(stackSize >= 0);    // 0 is a valid stack size to pass to
                                        // _beginthreadex -- it means 'the same
                                        // size as the main thread'.  This is
                                        // is not documented bde behavior, but
                                        // allow it just in case anyone was
                                        // depending on it.

    startInfo->d_threadArg = userData;
    startInfo->d_function  = function;
    handle->d_handle = (HANDLE)_beginthreadex(
                                             0,
                                             stackSize,
                                             ThreadEntry,
                                             startInfo,
                                             STACK_SIZE_PARAM_IS_A_RESERVATION,
                                             (unsigned int *)&handle->d_id);
    if ((HANDLE)-1 == handle->d_handle) {
        freeStartupInfo(startInfo);
        return 1;                                                     // RETURN
    }
    if (bcemt_ThreadAttributes::BCEMT_CREATE_DETACHED ==
                                                   attribute.detachedState()) {
        HANDLE tmpHandle = handle->d_handle;
        handle->d_handle  = 0;
        startInfo->d_handle = *handle;
        ResumeThread(tmpHandle);
        CloseHandle(tmpHandle);
    }
    else {
        startInfo->d_handle = *handle;
        ResumeThread(handle->d_handle);
    }
    return 0;
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::join(
           bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle&   handle,
           void                                                       **status)
{
    // Cannot self - join

    if (!handle.d_handle || handle.d_id == GetCurrentThreadId()) {
        return 1;                                                     // RETURN
    }
    if (WaitForSingleObject(handle.d_handle, INFINITE) != WAIT_OBJECT_0) {
        return 2;                                                     // RETURN
    }
    DWORD exitStatus;
    DWORD result = GetExitCodeThread(handle.d_handle,&exitStatus);

#ifdef BCEMT_USE_RETURN_VALUE_MAP
    // In this case, we ignore 'exitStatus', but we're still fetching
    // it in to get the 'result' value

    if (status) {
        EnterCriticalSection(&s_returnValueMapLock);
        if (s_returnValueMapValid) {
            *status = s_returnValueMap[handle.d_id];
        }
        LeaveCriticalSection(&s_returnValueMapLock);
    }
#else
    if (status) {
        *status = (void *)exitStatus;
    }
#endif // def BCEMT_USE_RETURN_VALUE_MAP
    CloseHandle(handle.d_handle);
    handle.d_handle = 0;

    return FALSE == result ? 3 : 0;
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::detach(
             bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& handle)
{
    if (handle.d_handle == GetCurrentThread()
     && handle.d_id     == GetCurrentThreadId()) {
        bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle *realHandle;
        realHandle =
            (bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle *)
                                             TlsGetValue(s_threadInfoTLSIndex);
        if (!realHandle || !realHandle->d_handle) {
            return 1;                                                 // RETURN
        }
        if (!CloseHandle(realHandle->d_handle)) {
            return 1;                                                 // RETURN
        }
        handle.d_handle = 0;
        return 0;                                                     // RETURN
    }
    else if (handle.d_handle) {
        if (!CloseHandle(handle.d_handle)) {
            return 1;                                                 // RETURN
        }
        handle.d_handle = 0;
        return 0;                                                     // RETURN
    }
    return 2;
}

void bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::exit(void *status)
{
    invokeDestructors();
#ifdef BCEMT_USE_RETURN_VALUE_MAP
    EnterCriticalSection(&s_returnValueMapLock);
    if (s_returnValueMapValid) {
        s_returnValueMap[GetCurrentThreadId()] = status;
    }
    LeaveCriticalSection(&s_returnValueMapLock);
#endif
    _endthreadex((unsigned)(bsls_PlatformUtil::IntPtr)status);
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::createKey(
                                       Key                         *key,
                                       bcemt_KeyDestructorFunction  destructor)
{
    // It is not uncommon for applications to have global objects that make
    // calls to create thread-specific keys.  It is possible that those objects
    // are initialized before the BCE threading environment, so make sure to do
    // so first.

    if (bcemt_threadutil_win32_Initialize()) {
        return 1;                                                     // RETURN
    }

    *key = TlsAlloc();
    if (TLS_OUT_OF_INDEXES == *key) {
        return 1;                                                     // RETURN
    }
    if (destructor) {
        ThreadSpecificDestructor *d = new ThreadSpecificDestructor;
        d->d_key = *key;
        d->d_destructor = destructor;
        EnterCriticalSection(&s_threadSpecificDestructorsListLock);
        d->d_next = s_destructors;
        s_destructors = d;
        LeaveCriticalSection(&s_threadSpecificDestructorsListLock);
    }
    return 0;
}

int bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::deleteKey(Key& key)
{
    ThreadSpecificDestructor *prev = 0;
    if (!TlsFree(key)) {
        return 1;                                                     // RETURN
    }
    EnterCriticalSection(&s_threadSpecificDestructorsListLock);
    ThreadSpecificDestructor *d = s_destructors;
    while (d) {
        if (d->d_key == key) {
            if (prev) {
                prev->d_next = d->d_next;
            }
            else {
                s_destructors = d->d_next;
            }
            delete d;
            break;
        }
        prev = d;
        d = d->d_next;
    }
    LeaveCriticalSection(&s_threadSpecificDestructorsListLock);
    return 0;
}

bool bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::areEqual(
            const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& a,
            const bcemt_ThreadUtilImpl<bces_Platform::Win32Threads>::Handle& b)
{
    return a.d_id == b.d_id;
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_WIN32_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
