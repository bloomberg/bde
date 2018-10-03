// bsls_stackaddressutil.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bsls_stackaddressutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#if defined(BSLS_PLATFORM_OS_UNIX)

#include <limits.h>
#include <errno.h>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#endif

#if defined(BSLS_PLATFORM_OS_AIX)

# include <ucontext.h>

#elif defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)

#include <execinfo.h>

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

# include <sys/frame.h>
# include <sys/stack.h>
# include <link.h>
# include <thread.h>
# include <setjmp.h>

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

#include <bsls_dbghelpdllimpl_windows.h>

#include <windows.h>
#include <intrin.h>
#include <dbghelp.h>

#pragma optimize("", off)

#endif

namespace BloombergLP {

                            // ----------------
                            // StackAddressUtil
                            // ----------------


// CLASS METHODS
#if defined(BSLS_PLATFORM_OS_AIX)

namespace bsls {

int StackAddressUtil::getStackAddresses(void **buffer,
                                        int    maxFrames)
{
    assert(0 <= maxFrames);

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

#if defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_DARWIN)

namespace bsls {

int StackAddressUtil::getStackAddresses(void **buffer,
                                        int    maxFrames)
{
    assert(0 <= maxFrames);

    if (0 >= maxFrames) {
        // Call 'backtrace' to make sure that it has been dynamically loaded if
        // it wasn't already.  Note that on Linux, the first time
        //  'backtrace' is called, it calls 'dlopen', which calls 'malloc'.
        // It may be the same on Darwin.  Also note that handling the
        // 'maxFrames == 0' case allows the caller to call this function with
        // arguments '(0, 0)' on any platform to ensure that any dynamic
        // loading has occurred, which is useful for debugging.

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

void bsls_StackAddressUtil_SparcAsmDummy()
    // This routine is never called.  It just provides a place to put the
    // assembler routine 'bsls_StackAddressUtil_flushWinAndGetFP', which
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

    asm volatile(".global bsls_StackAddressUtil_flushAndGetFP\n"
                 ".type bsls_StackAddressUtil_flushAndGetFP,#function\n"
                 "bsls_StackAddressUtil_flushAndGetFP:\n"
                 "ta 0x03\n"
                 "mov %o6, %o0\n"
                 "retl\n"
                 "nop\n"
                 );
}

extern "C" char *bsls_StackAddressUtil_flushAndGetFP();

#endif

namespace bsls {
int StackAddressUtil::getStackAddresses(void **buffer,
                                        int    maxFrames)
{
    assert(0 <= maxFrames);

    // 'STACK_BIAS' is a constant defined in '/usr/include/sys/stack.h'.  The
    // type 'frame' is a struct declared in '<sys/frame.h>'.

    int frameIndex = 0;

#if defined(BSLS_PLATFORM_CPU_SPARC)
    const frame *framePtr = (frame *) (void *)
                          (bsls_StackAddressUtil_flushAndGetFP() + STACK_BIAS);

    // Calculate the base of the stack.  It is preferable to call
    // 'thr_stksegment', but it and 'thr_probe_getfunc_addr' may not be loaded.
    // Since 'thr_probe_getfunc_addr' is declared with '#pragma weak', a
    // pointer to it will be null if those routines aren't loaded.  Take the
    // pointer, and if it isn't null, call 'thr_stksegment'.  If it null, use
    // the less optimal approach of using '_environ' to find the base of the
    // stack.

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

#else // defined(BSLS_PLATFORM_CPU_X86_64) || defined(BSLS_PLATFORM_CPU_X86)

    jmp_buf ctx;
    setjmp(ctx);
    long fpVal = ((long*)(ctx))[5];
    const frame *framePtr = (struct frame*)((char*)(fpVal) + STACK_BIAS);
    void* baseOfStack = _environ;

#endif

    if (!framePtr) {
        return frameIndex;                                            // RETURN
    }
    assert(framePtr < baseOfStack);
    while (frameIndex < maxFrames && framePtr < baseOfStack) {
        void * pc = (void *) framePtr->fr_savpc;
        if (!pc) {
            assert(frameIndex > 0);
            break;
        }
        buffer[frameIndex++] = (void *) pc;
        struct frame *nextFP = (struct frame *) (void *)
                                    ((char *) framePtr->fr_savfp + STACK_BIAS);
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

namespace bsls {

int StackAddressUtil::getStackAddresses(void    **buffer,
                                        int       maxFrames)
{
    assert(0 <= maxFrames);

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
    memset(&winContext, 0, sizeof(winContext));

#if   defined(BSLS_PLATFORM_OS_WINXP)
    // RtlCaptureContext is not implemented before XP or Server 2003

    RtlCaptureContext(&winContext);

#elif defined(BSLS_PLATFORM_OS_WIN2K) ||                                     \
      defined(BSLS_PLATFORM_OS_WINNT) ||                                     \
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
        bool rc = bsls::DbghelpDllImpl_Windows::stackWalk64(MACHINE,
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


namespace bsls {

                            // ----------------
                            // StackAddressUtil
                            // ----------------

// CLASS METHODS
void StackAddressUtil::formatCheapStack(char       *output,
                                        int         length,
                                        const char *taskname)
{
    assert(0 <= length);

    if (0 == length) {
        return;                                                       // RETURN
    }

    enum { k_BUFFER_LENGTH = 100 };
    void *buffer[k_BUFFER_LENGTH];

    // We need to prevent the optimizer from inlining the call to
    // 'getStackAddresses' so that we'll be sure exactly how many frames to
    // ignore.  So take a ptr to the function, and then do a weird identity
    // transform on it that the optimizer can't figure out is an identity
    // transform.

    typedef int (*GSAFunc)(void **, int);    // 'Get Stack Addresses Func type'
    typedef Types::UintPtr UintPtr;

    // Create 'garbageMask', set to random garbage, low-order bit set.

    UintPtr       uu = reinterpret_cast<UintPtr>(&formatCheapStack);
    const UintPtr garbageMask = (uu ^ (uu >> 12)) | 1;

    // 'gsa' is, and always will be, a pointer to 'getStackAddresses'.

    GSAFunc gsa  = &getStackAddresses;
    UintPtr ugsa = reinterpret_cast<UintPtr>(gsa);

    // This loop will toggle some of the low order 4 bits of 'ugsa', but it
    // will toggle each modified bit 8 times, so we'll finish with 'ugsa'
    // having the value it started the loop with.

    for (uu = 0; 0 == ((garbageMask << 4) & uu); ++uu) {
        ugsa ^= garbageMask & uu;
    }

    // Now we assign 'gsa' to the value it already has, but the optimizer
    // doesn't understand that.

    gsa = reinterpret_cast<GSAFunc>(ugsa);

    // Now the optimizer has no idea where 'gsa' points to (it points to
    // '&getStackAddresses'), so it can't possibly inline the call.

    int numAddresses = (*gsa)(buffer, k_BUFFER_LENGTH);

    char *out     = output;
    int   rem     = length;
    int   printed = 0;
    *out = '\0';

    if (1 == rem) {
        return;                                                       // RETURN
    }

#if defined(BSLS_PLATFORM_CMP_MSVC) && \
    !defined(BSLS_LIBRARYFEATURES_HAS_C99_SNPRINTF)

    // We want to use '_snprintf' and make sure if we use all the space we
    // still leave a null terminator at the end of the last buffer.

#   define snprintf _snprintf
    *(out + rem - 1) = '\0';
    rem--;

#endif

    if (numAddresses < 0) {
        // 'getStackAddresses' has failed, just output diagnostics.

        snprintf(out, rem, "Unable to obtain call stack.");
        return;                                                       // RETURN
    }


    printed = snprintf(out, rem, "Please run \"/bb/bin/showfunc.tsk ");
    rem -= printed;
    out += printed;

    if (printed < 0 || rem <= 0) {
        return;                                                       // RETURN
    }

    if (0 == taskname) {
        taskname = "<binary_name_here>";
    }

    printed = snprintf(out, rem, "%s", taskname);
    rem -= printed;
    out += printed;

    if (printed < 0 || rem <= 0) {
        return;                                                       // RETURN
    }

    for (int i = 1 + k_IGNORE_FRAMES;
         i < numAddresses && rem > 0;
         ++i) {
        uintptr_t stackValue = reinterpret_cast<uintptr_t>(buffer[i]);

        printed = snprintf(out,
                           rem,
                           " " BSLS_BSLTESTUTIL_FORMAT_PTR,
                           stackValue);

        rem -= printed;
        out += printed;

        if (printed < 0 || rem <= 0) {
            return;                                                   // RETURN
        }

    }

    snprintf(out, rem, "\" to see the stack trace.\n");
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
