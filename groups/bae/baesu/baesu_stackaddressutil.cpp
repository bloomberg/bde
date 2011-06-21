// baesu_stackaddressutil.cpp                                         -*-C++-*-
#include <baesu_stackaddressutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baesu_stackaddressutil_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>

#if defined(BSLS_PLATFORM__OS_UNIX)

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>

#endif

#if defined(BSLS_PLATFORM__OS_AIX)

# include <ucontext.h>

#elif defined(BSLS_PLATFORM__OS_HPUX)

# include <bdema_heapbypassallocator.h>
# include <bsls_platformutil.h>

# include <uwx.h>
# include <uwx_self.h>
# include <unwind.h>

#elif defined(BSLS_PLATFORM__OS_LINUX)

#include <execinfo.h>
#include <link.h>

#elif defined(BSLS_PLATFORM__OS_SOLARIS)

# include <sys/frame.h>
# include <sys/stack.h>
# include <link.h>
# include <thread.h>

#elif defined(BSLS_PLATFORM__OS_WINDOWS)

#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>

#pragma optimize("", off)

#endif

//=============================================================================
//                  Debugging trace macro: 'TRACE_PRINTF'
//=============================================================================

#undef  DEVELOPMENT_TRACE_FLAG
#define DEVELOPMENT_TRACE_FLAG 0

#if DEVELOPMENT_TRACE_FLAG == 1
// debugging trace ON

# include <stdio.h>

# define TRACE_PRINTF printf

#else

# define TRACE_PRINTF (void)

#endif

#if defined(BSLS_PLATFORM__OS_WINDOWS)

namespace {

namespace U {

                        // ===========================
                        // struct WinDbgHelpFuncHandle
                        // ===========================

struct WinDbgHelpFuncHandle {
    // This struct contains the handle of the DLL and a collection of function
    // ptrs that will point to functions loaded at run time from it.  It is
    // necessary on Withows ot load the library 'dbghelp.dll' and find function
    // pointers within that shared library, thus enabling us to call these
    // functions.  This class provides a repository for the DLL library handle
    // and the function pointers we need within it.

    // TYPES
    typedef DWORD __stdcall SymSetOptionsProc(DWORD);
    typedef BOOL  __stdcall SymInitializeProc(HANDLE, PCSTR, BOOL);
    typedef BOOL  __stdcall StackWalk64Proc(DWORD,
                                            HANDLE,
                                            HANDLE,
                                            LPSTACKFRAME64,
                                            PVOID,
                                            PREAD_PROCESS_MEMORY_ROUTINE64,
                                            PFUNCTION_TABLE_ACCESS_ROUTINE64,
                                            PGET_MODULE_BASE_ROUTINE64,
                                            PTRANSLATE_ADDRESS_ROUTINE64);
    typedef BOOL  __stdcall SymCleanupProc(HANDLE);

    // DATA
    HMODULE                 d_moduleHandle;  // handle of the DLL that we will
                                             // load the functions from

    SymSetOptionsProc      *d_symSetOptions; // 'SymSetOptions' func

    SymInitializeProc      *d_symInitialize; // 'SymInitialize' func

    StackWalk64Proc        *d_stackWalk64;   // 'StackWalk64' func

    SymCleanupProc         *d_symCleanup;    // 'SymCleanup' func

    PFUNCTION_TABLE_ACCESS_ROUTINE64
                            d_symFunctionTableAccess64;
                                             // 'SymFunctionTableAccess64' func

    PGET_MODULE_BASE_ROUTINE64
                            d_symGetModuleBase64;
                                             // 'SymGetModuleBase64' func
};

}  // close namespace U

}  // close unnamed namespace


static
int loadDll(U::WinDbgHelpFuncHandle *funcHandle)
    // Open 'dbghelp.dll' and initialize all the function pointers in this
    // object to refer to functions in the dll.  Return 0 on success, and a
    // non-zero value otherwise.
{
    typedef U::WinDbgHelpFuncHandle FH;

    funcHandle->d_moduleHandle = LoadLibraryA("dbghelp.dll");
    if (0 == funcHandle->d_moduleHandle) {
        return -1;                                                    // RETURN
    }

    HMODULE moduleHandle = funcHandle->d_moduleHandle;

    funcHandle->d_symSetOptions = (FH::SymSetOptionsProc *)
                               GetProcAddress(moduleHandle, "SymSetOptions");
    funcHandle->d_symInitialize = (FH::SymInitializeProc *)
                               GetProcAddress(moduleHandle, "SymInitialize");
    funcHandle->d_stackWalk64 = (FH::StackWalk64Proc *)
                                 GetProcAddress(moduleHandle, "StackWalk64");
    funcHandle->d_symCleanup = (FH::SymCleanupProc *)
                                  GetProcAddress(moduleHandle, "SymCleanup");

    funcHandle->d_symFunctionTableAccess64 = (PFUNCTION_TABLE_ACCESS_ROUTINE64)
                    GetProcAddress(moduleHandle, "SymFunctionTableAccess64");
    funcHandle->d_symGetModuleBase64 = (PGET_MODULE_BASE_ROUTINE64)
                          GetProcAddress(moduleHandle, "SymGetModuleBase64");

    if   (NULL == funcHandle->d_symSetOptions
       || NULL == funcHandle->d_symInitialize
       || NULL == funcHandle->d_stackWalk64
       || NULL == funcHandle->d_symCleanup
       || NULL == funcHandle->d_symFunctionTableAccess64
       || NULL == funcHandle->d_symGetModuleBase64) {
        FreeLibrary(funcHandle->d_moduleHandle);
        funcHandle->d_moduleHandle = NULL;
        return -1;                                                    // RETURN
    }

    return 0;
}

// Windows
#endif

namespace BloombergLP {

                              // ------------------
                              // baesu_StackAddress
                              // ------------------


// CLASS METHODS
#if defined(BSLS_PLATFORM__OS_AIX)

int baesu_StackAddressUtil::getStackAddresses(void **buffer,
                                              int    maxFrames)
{
    BSLS_ASSERT(0 <= maxFrames);

    struct AixFrame {
        AixFrame  *d_nextFrame;
        void      *d_pad;
        void      *d_returnAddress;
    };

    ucontext_t context;
    int rc = getcontext(&context);
    if (rc) {
        return -1;                                                    // RETURN
    }

    int frameIndex = 0;

    const AixFrame *frame = (AixFrame *)context.uc_mcontext.jmp_context.gpr[1];
    if (0 == frame) {
        return frameIndex;                                            // RETURN
    }
    frame = frame->d_nextFrame;
    while (frame && frameIndex < maxFrames) {
        TRACE_PRINTF("Frame: %p\n", frame);
        buffer[frameIndex++] = frame->d_returnAddress;
        const AixFrame *nextFrame = frame->d_nextFrame;
        if (nextFrame <= frame) {
            break;
        }
        frame = nextFrame;
    }

    return frameIndex;
}

// AIX
#endif
#if defined(BSLS_PLATFORM__OS_HPUX)

                            // -----------------------
                            // HP allocation callbacks
                            // -----------------------

extern "C" {

static
void *allocationCallBack(void *allocator, size_t size)
    // Use 'allocator' to allocate a segment of memory that is at least 'size'
    // in bytes.  Return a pointer to the segment.  The behavior is undefined
    // unless 'locator' is of type 'bslma_Allocator *'.  Note that a pointer to
    // this function is passed to the 'uwx' debugging routines provided by HP
    // to allow this component to specify how 'uwx' is to allocate memory.
{
    return ((bslma_Allocator *) allocator)->allocate(size);
}

static
void freeCallBack(void *allocator, void *segmentPtr)
    // This function is a noop.  When we pass 'allocationCallBack' to 'uwx', we
    // are required to specify a 'free' function too.  Since this component is
    // only used with 'bdema_HeapBypassAllocator', which frees all memory it
    // has allocated upon destruction of the allocator and doesn't free
    // individual segments, this routine does nothing.
{
    ;
}

}  // extern "C"

int baesu_StackAddressUtil::getStackAddresses(void **buffer,
                                              int    maxFrames)
{
    BSLS_ASSERT(0 <= maxFrames);

    int rc;
    bdema_HeapBypassAllocator allocator;

    uwx_config cfg = { UWX_CONFIG_ALLOC,        // flags
                       &allocationCallBack,     // alloc callback
                       &freeCallBack,           // free callback
                       &allocator };            // arg to pass to alloc & free
                                                // cb's

    uwx_env *env = uwx_init_config(&cfg, &rc);
    if (0 == env || UWX_OK != rc) {
        return -101;                                                  // RETURN
    }

    uwx_self_info *info = uwx_self_init_info(env);
    if (0 == info) {
        return -105;                                                  // RETURN
    }

    rc = uwx_register_callbacks(env,
                                (intptr_t) info,
                                &uwx_self_copyin,
                                &uwx_self_lookupip);
    if (UWX_OK != rc) {
        return -111;                                                  // RETURN
    }

    rc = uwx_self_init_context(env);
    if (UWX_OK != rc) {
        return -115;                                                  // RETURN
    }

    for (int i = 0; i < maxFrames; ++i) {
        rc = uwx_step(env);
        if (UWX_BOTTOM == rc) {
            return i;                                                 // RETURN
        }
        if (UWX_OK != rc) {
            return -121;                                              // RETURN
        }

        uint64_t ip;
        rc = uwx_get_reg(env, UWX_REG_IP, &ip);
        if (UWX_OK != rc) {
            return -125;                                              // RETURN
        }

        buffer[i] = (void *) (bsls_PlatformUtil::UintPtr) ip;
    }

    return maxFrames;
}

// HPUX
#endif
#ifdef BSLS_PLATFORM__OS_LINUX

int baesu_StackAddressUtil::getStackAddresses(void    **buffer,
                                              int       maxFrames)
{
    BSLS_ASSERT(0 <= maxFrames);

    if (0 >= maxFrames) {
        // Call 'backtrace' to make sure that it has been dynamically loaded
        // if it wasn't already.  Note the first time 'backtrace' is called,
        // it calls 'dlopen', which calls 'malloc'.  Also note that handling
        // the 'maxFrames == 0' case allows the caller to call this function
        // with arguments '(0, 0)' on any platform to ensure that any dynamic
        // loading has occured, which is useful for debugging.

        void *p;
        backtrace(&p, 1);
        return 0;                                                     // RETURN
    }

    return backtrace(buffer, maxFrames);
}

// LINUX
#endif
#if defined(BSLS_PLATFORM__OS_SOLARIS)

extern "C" {
extern void *thr_probe_getfunc_addr;
#pragma weak thr_probe_getfunc_addr
    // 'thr_probe_getfunc_addr' is a Solaris function not documented in an
    // include file that is used to obtain the base of the call stack.
    // '#pragma weak' means if the shared library is not statically linked in
    // with the executable, taking the address to the function will return 0.

extern char **_environ;
    // An alternate way to get the stack base if 'thr_probe_getfunc_addr' is
    // not loaded.
}

void baesu_StackAddressUtil_SparcAsmDummy()
    // This routine is never called.  It just provides a place to put the
    // assembler routine 'baesu_StackAddressUtil_flushWinAndGetFP', which
    // flushes the stack registers of the sparc processor to memory.  Note this
    // routine must be global to avoid the compiler issuing
    // 'static function never called' warnings.
{
    // 'flushw' which didn't work, has been replaced with 'ta 0x03'.  According
    // to the Sparc assembly doc at
    // 'http://download.oracle.com/docs/cd/E19641-01/802-1947/802-1947.pdf',
    // 'flushw' should do everything we want (flush all the register windows to
    // RAM), but empirically it was found that it was failing to do so.
    // 'ta 0x3' (Trap unconditionally with code 3), it was found, does the
    // trick.  It was also found that recursing deeply also works, but that is
    // more brute force.

    asm volatile(".global baesu_StackAddressUtil_flushAndGetFP\n"
                 ".type baesu_StackAddressUtil_flushAndGetFP,#function\n"
                 "baesu_StackAddressUtil_flushAndGetFP:\n"
                 "ta 0x03\n"
                 "mov %o6, %o0\n"
                 "retl\n"
                 "nop\n"
                 );
}

extern "C" char *baesu_StackAddressUtil_flushAndGetFP();

int baesu_StackAddressUtil::getStackAddresses(void    **buffer,
                                              int       maxFrames)
{
    BSLS_ASSERT(0 <= maxFrames);

    // 'STACK_BIAS' is a sparc constant defined in '/usr/include/sys/stack.h'.
    // The type 'frame' is a struct declared in '<sys/frame.h>'.

    const frame *framePtr = (frame *) (void *)
                         (baesu_StackAddressUtil_flushAndGetFP() + STACK_BIAS);
    int frameIndex = 0;

    // Calculate the base of the stack.  It is preferable to call
    // 'thr_stksegment', but it and 'thr_probe_getfunc_addr' may not be loaded.
    // Since 'thr_probe_getfunc_addr' is declard with '#pragma weak', a pointer
    // to it will be null if those routines arent't loaded.  Take the pointer,
    // and if it isn't null, call 'thr_stksegment'.  If it null, use the less
    // optimal approach of using '_environ' to find the base of the stack.

    void *baseOfStack;
    void *probeAddr = &thr_probe_getfunc_addr;
    if (probeAddr) {
        stack_t thrstack;
        if (0 != thr_stksegment(&thrstack)) {
            return frameIndex;                                        // RETURN
        }
        baseOfStack = thrstack.ss_sp;
    }
    else {
        baseOfStack = _environ;
    }
    TRACE_PRINTF("framePtr=%p, bos=%p\n", framePtr, baseOfStack);

    if (!framePtr) {
        return frameIndex;                                            // RETURN
    }
    BSLS_ASSERT(framePtr < baseOfStack);
    while (frameIndex < maxFrames && framePtr < baseOfStack) {
        void * pc = (void *) framePtr->fr_savpc;
        if (!pc) {
            BSLS_ASSERT(frameIndex > 0);
            break;
        }
        buffer[frameIndex++] = (void *) pc;
        struct frame *nextFP = (struct frame *) (void *)
                                    ((char *) framePtr->fr_savfp + STACK_BIAS);
        TRACE_PRINTF("saved %p nextfp %p\n", pc, nextFP);
        if (nextFP <= framePtr) {
            break;
        }
        framePtr = nextFP;
    }

    return frameIndex;
}

// SOLARIS
#endif
#ifdef BSLS_PLATFORM__OS_WINDOWS

int baesu_StackAddressUtil::getStackAddresses(void    **buffer,
                                              int       maxFrames)
{
    BSLS_ASSERT(0 <= maxFrames);

    U::WinDbgHelpFuncHandle api;
    if (0 != loadDll(&api)) {
        return 0;                                                     // RETURN
    }

    (*api.d_symSetOptions)(SYMOPT_NO_PROMPTS
                         | SYMOPT_LOAD_LINES
                         | SYMOPT_DEFERRED_LOADS);

    //                   | SYMOPT_DEBUG);

    HANDLE hProcess = GetCurrentProcess();

    // Thanks to SYMOPT_DEFERRED_LOADS no manual enumeration of libraries is
    // necessary, 'api' will only load what is actually required

    BOOL rc = (*api.d_symInitialize)(hProcess, NULL, TRUE);
    if (!rc) {
        TRACE_PRINTF("Could not init symbols for %p (%08x)\n",
                                                     hProcess, GetLastError());
        return -131;                                                  // RETURN
    }

    // See 'http://msdn.microsoft.com/en-us/library/ms680313(VS.85).aspx' for
    // details.

#if !defined(BSLS_PLATFORM__CPU_X86) && !defined(BSLS_PLATFORM__CPU_X86_64)
#   error unrecognized architecture
#elif defined(BSLS_PLATFORM__CPU_64_BIT)
    // x86 compatible cpu, 64 bit executable

    const int machine = IMAGE_FILE_MACHINE_AMD64;
#elif defined(BSLS_PLATFORM__CPU_32_BIT)
    // x86 compatible cpu, 32 bit executable

    const int machine = IMAGE_FILE_MACHINE_I386;
#else
#   error unrecognized architecture
#endif

    CONTEXT winContext;

#if   defined(BSLS_PLATFORM__OS_WINXP)
    // RtlCaptureContext is not implemented before XP or Server 2003

    RtlCaptureContext(&winContext);

#elif defined(BSLS_PLATFORM__OS_WIN2K) || \
      defined(BSLS_PLATFORM__OS_WINNT) || \
      defined(BSLS_PLATFORM__OS_WIN9X)

    winContext.ContextFlags = CONTEXT_CONTROL;
    __asm {
        mov winContext.Ebp, ebp;
        call here;
  here: pop eax;
        mov winContext.Eip, eax;
        mov winContext.SegCs, 0;
        mov word ptr winContext.SegCs, cs;
        pushfd;
        pop eax;
        mov winContext.EFlags, eax;
        mov winContext.Esp, esp;
        mov winContext.SegSs, 0;
        mov word ptr winContext.SegSs, ss;
    }

#else

# error unrecognized platform

#endif

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(stackFrame));

    // 'ip' is instruction pointer, 'bp' is base pointer (== frame pointer),
    // 'sp' is pointer to stack top

#ifdef BSLS_PLATFORM__CPU_64_BIT
    stackFrame.AddrPC.Offset    = (DWORD64) winContext.Rip;
    stackFrame.AddrFrame.Offset = (DWORD64) winContext.Rbp;
    stackFrame.AddrStack.Offset = (DWORD64) winContext.Rsp;
#else
    stackFrame.AddrPC.Offset    = (DWORD64) winContext.Eip;
    stackFrame.AddrFrame.Offset = (DWORD64) winContext.Ebp;
    stackFrame.AddrStack.Offset = (DWORD64) winContext.Esp;
#endif

    stackFrame.AddrPC.Mode    = AddrModeFlat;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Mode = AddrModeFlat;
    int stackFrameIndex;
    for (stackFrameIndex = 0; stackFrameIndex < maxFrames; ++stackFrameIndex) {
        int rc = api.d_stackWalk64(machine,
                                   hProcess,
                                   GetCurrentThread(),
                                   &stackFrame,
                                   &winContext,
                                   NULL,
                                   api.d_symFunctionTableAccess64,
                                   api.d_symGetModuleBase64,
                                   NULL);
        if (!rc) {
            break;
        }
        buffer[stackFrameIndex] = (void *) stackFrame.AddrPC.Offset;
    }

    (*api.d_symCleanup)(hProcess);

    return stackFrameIndex;
}

// WINDOWS
#endif

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
