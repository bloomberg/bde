// balst_stackaddressutil.cpp                                         -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balst_stackaddressutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balst_stackaddressutil_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>

#if defined(BSLS_PLATFORM_OS_UNIX)

#include <bsl_climits.h>
#include <bsl_cerrno.h>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#endif

#if defined(BSLS_PLATFORM_OS_AIX)

# include <ucontext.h>

#elif defined(BSLS_PLATFORM_OS_HPUX)

# include <bdlma_heapbypassallocator.h>
# include <bsls_types.h>

# include <uwx.h>
# include <uwx_self.h>
# include <unwind.h>

#elif defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)

#include <execinfo.h>

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

# include <sys/frame.h>
# include <sys/stack.h>
# include <link.h>
# include <thread.h>
# include <setjmp.h>

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

#include <balst_dbghelpdllimpl_windows.h>

#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>

#pragma optimize("", off)

#endif

// ============================================================================
//                   Debugging trace macro: 'TRACE_PRINTF'
// ============================================================================

#undef  DEVELOPMENT_TRACE_FLAG
#define DEVELOPMENT_TRACE_FLAG 0

#if DEVELOPMENT_TRACE_FLAG == 1
// debugging trace ON

# include <stdio.h>

# define TRACE_PRINTF printf

#else

# define TRACE_PRINTF (void)

#endif

namespace BloombergLP {

                              // ------------------
                              // balst::StackAddress
                              // ------------------


// CLASS METHODS
#if defined(BSLS_PLATFORM_OS_AIX)

namespace balst {

int StackAddressUtil::getStackAddresses(void **buffer,
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

}  // close package namespace

// AIX
#endif
#if defined(BSLS_PLATFORM_OS_HPUX)

                            // -----------------------
                            // HP allocation callbacks
                            // -----------------------

extern "C" {

static
void *allocationCallBack(void *allocator, size_t size)
    // Use 'allocator' to allocate a segment of memory that is at least 'size'
    // in bytes.  Return a pointer to the segment.  The behavior is undefined
    // unless 'locator' is of type 'bslma::Allocator *'.  Note that a pointer
    // to this function is passed to the 'uwx' debugging routines provided by
    // HP to allow this component to specify how 'uwx' is to allocate memory.
{
    return ((bslma::Allocator *)allocator)->allocate(size);
}

static
void freeCallBack(void *allocator, void *segmentPtr)
    // This function is a noop.  When we pass 'allocationCallBack' to 'uwx', we
    // are required to specify a 'free' function too.  Since this component is
    // only used with 'bdlma::HeapBypassAllocator', which frees all memory it
    // has allocated upon destruction of the allocator and doesn't free
    // individual segments, this routine does nothing.
{
    ;
}

}  // extern "C"

namespace balst {

int StackAddressUtil::getStackAddresses(void **buffer,
                                              int    maxFrames)
{
    BSLS_ASSERT(0 <= maxFrames);

    int rc;
    bdlma::HeapBypassAllocator allocator;

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

        buffer[i] = (void *) (bsls::Types::UintPtr) ip;
    }

    return maxFrames;
}

}  // close package namespace

// HPUX
#endif
#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)

namespace balst {

int StackAddressUtil::getStackAddresses(void    **buffer,
                                              int       maxFrames)
{
    BSLS_ASSERT(0 <= maxFrames);

    if (0 >= maxFrames) {
        // Call 'backtrace' to make sure that it has been dynamically loaded
        // if it wasn't already.  Note that on Linux, the first time
        //  'backtrace' is called, it calls 'dlopen', which calls 'malloc'.
        // It may be the same on Darwin.  Also note that handling the
        // 'maxFrames == 0' case allows the caller to call this function
        // with arguments '(0, 0)' on any platform to ensure that any dynamic
        // loading has occured, which is useful for debugging.

        void *p;
        backtrace(&p, 1);
        return 0;                                                     // RETURN
    }

    return backtrace(buffer, maxFrames);
}

}  // close package namespace

// LINUX & DARWIN

#endif
#if defined(BSLS_PLATFORM_OS_SOLARIS)

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

#if defined(BSLS_PLATFORM_CPU_SPARC)

void balst_StackAddressUtil_SparcAsmDummy()
    // This routine is never called.  It just provides a place to put the
    // assembler routine 'balst_StackAddressUtil_flushWinAndGetFP', which
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

    asm volatile(".global balst_StackAddressUtil_flushAndGetFP\n"
                 ".type balst_StackAddressUtil_flushAndGetFP,#function\n"
                 "balst_StackAddressUtil_flushAndGetFP:\n"
                 "ta 0x03\n"
                 "mov %o6, %o0\n"
                 "retl\n"
                 "nop\n"
                 );
}

extern "C" char *balst_StackAddressUtil_flushAndGetFP();

#endif

namespace balst {
int StackAddressUtil::getStackAddresses(void    **buffer,
                                              int       maxFrames)
{
    BSLS_ASSERT(0 <= maxFrames);

    // 'STACK_BIAS' is a constant defined in '/usr/include/sys/stack.h'.
    // The type 'frame' is a struct declared in '<sys/frame.h>'.

    int frameIndex = 0;

#if defined(BSLS_PLATFORM_CPU_SPARC)
    const frame *framePtr = (frame *) (void *)
                         (balst_StackAddressUtil_flushAndGetFP() + STACK_BIAS);

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

#else //  defined(BSLS_PLATFORM_CPU_X86_64) || defined(BSLS_PLATFORM_CPU_X86)

    jmp_buf ctx;
    setjmp(ctx);
    long fpVal = ((long*)(ctx))[5];
    const frame *framePtr = (struct frame*)((char*)(fpVal) + STACK_BIAS);
    void* baseOfStack = _environ;

#endif
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

}  // close package namespace

// SOLARIS
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)

namespace balst {

int StackAddressUtil::getStackAddresses(void    **buffer,
                                        int       maxFrames)
{
    BSLS_ASSERT(0 <= maxFrames);

    bslmt::QLockGuard guard(&DbghelpDllImpl_Windows::qLock());

    DbghelpDllImpl_Windows::symSetOptions(SYMOPT_NO_PROMPTS
                                          | SYMOPT_LOAD_LINES
                                          | SYMOPT_DEFERRED_LOADS);

    //                                    | SYMOPT_DEBUG);

    // See 'http://msdn.microsoft.com/en-us/library/ms680313(VS.85).aspx' for
    // details.

#if !defined(BSLS_PLATFORM_CPU_X86) && !defined(BSLS_PLATFORM_CPU_X86_64)
#   error unrecognized architecture
#elif defined(BSLS_PLATFORM_CPU_64_BIT)
    // x86 compatible cpu, 64 bit executable

    enum { MACHINE = IMAGE_FILE_MACHINE_AMD64 };
#elif defined(BSLS_PLATFORM_CPU_32_BIT)
    // x86 compatible cpu, 32 bit executable

    enum { MACHINE = IMAGE_FILE_MACHINE_I386 };
#else
#   error unrecognized architecture
#endif

    CONTEXT winContext;

#if   defined(BSLS_PLATFORM_OS_WINXP)
    // RtlCaptureContext is not implemented before XP or Server 2003

    RtlCaptureContext(&winContext);

#elif defined(BSLS_PLATFORM_OS_WIN2K) || \
      defined(BSLS_PLATFORM_OS_WINNT) || \
      defined(BSLS_PLATFORM_OS_WIN9X)

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

#ifdef BSLS_PLATFORM_CPU_64_BIT
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
    HANDLE currentThread = GetCurrentThread();

    for (stackFrameIndex = 0; stackFrameIndex < maxFrames; ++stackFrameIndex){
        bool rc = balst::DbghelpDllImpl_Windows::stackWalk64(MACHINE,
                                                             currentThread,
                                                             &stackFrame,
                                                             &winContext);
        if (!rc) {
            break;
        }
        buffer[stackFrameIndex] = (void *) stackFrame.AddrPC.Offset;
    }

    return stackFrameIndex;
}

}  // close package namespace

// WINDOWS
#endif

}  // close enterprise namespace

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
