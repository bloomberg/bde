// bsls_stackaddressutil.cpp                                          -*-C++-*-
#include <bsls_stackaddressutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#if defined(BSLS_PLATFORM_OS_UNIX)

#include <limits.h>
#include <errno.h>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#endif

#if defined(BSLS_PLATFORM_OS_AIX)

# include <ucontext.h>
# include <unistd.h>
# include <procinfo.h>
# include <sys/types.h>
# include <sys/ldr.h>
# include <xcoff.h>

#elif defined(BSLS_PLATFORM_OS_LINUX)

# include <execinfo.h>

#elif defined(BSLS_PLATFORM_OS_DARWIN)

# include <execinfo.h>
# include <libproc.h>

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

# include <sys/frame.h>
# include <sys/stack.h>
# include <link.h>
# include <thread.h>
# include <setjmp.h>
# include <stdlib.h>

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

# include <windows.h>
# include <winnt.h>

#pragma optimize("", off)

#endif

namespace {

#if defined(BSLS_PLATFORM_OS_AIX) || \
    defined(BSLS_PLATFORM_OS_DARWIN)
int getProcessId()
    // Return the platform-specific process id.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(::getpid());
#endif
}
#endif

int getProcessName(char *output, int length)
    // Load the system specific process name for the currently running process
    // into the specified 'output' bufffer having the specified 'length'.
    // Return 0 on success, and a non-zero value otherwise.  The behavior is
    // undefined unless '0 <= length' and 'output' has the capacity for at
    // least 'length' bytes.  Note that this has been adapted from
    // 'bdls_processutil' to work within the constraints of the level of this
    // component.
{
#if defined(BSLS_PLATFORM_OS_AIX)

    assert(0 != output);
    assert(0 < length);

    struct procentry64 procBuf;
    procBuf.pi_pid = getProcessId();

    // '::getargs' should fill the beginning of 'output' with null-terminated
    // 'argv[0]', which might be a relative path.

    int rc = ::getargs(&procBuf, sizeof(procBuf), output, length);
    output[length-1] = '\0'; // null terminate if 'output' was too short for
                             // process name

    return rc;

#elif defined(BSLS_PLATFORM_OS_DARWIN)

    char pidPathBuf[PROC_PIDPATHINFO_MAXSIZE];
    memset(pidPathBuf, '\0', PROC_PIDPATHINFO_MAXSIZE);

    int numChars = ::proc_pidpath(getProcessId(),
                                  pidPathBuf,
                                  PROC_PIDPATHINFO_MAXSIZE);
    if (numChars <= 0) {
        return -1;                                                    // RETURN
    }
    assert(numChars <= PROC_PIDPATHINFO_MAXSIZE);

    snprintf(output, length, "%s", pidPathBuf);
    return 0;

#elif defined(BSLS_PLATFORM_OS_LINUX)

    // We read '::program_invocation_name', which will just yield 'argv[0]',
    // which may or may not be a relative path.

    int printed = snprintf(output, length, "%s", ::program_invocation_name);
    return (printed >= 0) ? 0 : -1;

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

    // '::getexecname' will return 'argv[0]' with symlinks resolved, or 0 if it
    // fails.

    const char *execName = ::getexecname();
    if (!execName) {
        return -1;                                                    // RETURN
    }

    snprintf(output, length, "%s", execName);
    return 0;

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    DWORD resultlength = GetModuleFileNameA(0, output, length);
    if (resultlength < 0) {
        return -1;                                                    // RETURN
    }

    if (resultlength == length) {
        output[length-1] = '\0'; // null terminate if result was truncated,
                                 // which is not guaranteed on all windows
                                 // versions.
    }
    return 0;
#else
# error    Unrecognized Platform
#endif

}

#if defined(BSLS_PLATFORM_OS_AIX)
uintptr_t initStackOffset()
    // Return the offset that should be passed to 'showfunc.tsk' with the '-o'
    // argument in order for that task to properly locate function names.
    // Return '0' if any errors are encountered in attempting to determine the
    // offset.
{
    // On any failures we just return 0.

    // First we call 'loadquery' to get the primary module filename (which we
    // we use to examine 'xcoff' information) and the text segment location of
    // that module ('ldinfo_textorg' in the first returned 'ld_info' object).
    enum { BUF_SIZE = (8 << 10) - 64 };
    char ldInfoBuf[BUF_SIZE];

    if (loadquery(L_GETINFO, ldInfoBuf, BUF_SIZE) == -1) {
        return 0;
    }

    ld_info *currInfo = (ld_info*)(&ldInfoBuf[0]);

    // With the 'ldinfo_filename' returned by 'loadquery' we can now search for
    // 'xcoff' information to determine where the actual text segment resides
    // in the executable file.  Note that we don't need to handle multiple
    // loaded modules (shared objects) since 'showfunc.tsk' doesn't itself deal
    // with those.  This is replicating behavior from the 'sysutil' library's
    // 'cheap_stack_trace_addr' component.

    // See 'balst_stacktraceresolverimpl_xcoff' for further details of the
    // 'xcoff' format and how to proecss more of the information than just what
    // is being extracted here.

    struct FileDescriptor {
        int fd;
        FileDescriptor(const char *name) : fd(open64(name, O_RDONLY)) { }
        ~FileDescriptor() { if (fd >= 0) { close(fd); } }
        operator int() const { return fd; }
    } fd(currInfo->ldinfo_filename);

    if (fd < 0) {
        return 0;                                                     // RETURN
    }

    FILHDR fhdr;
    if (read(fd, &fhdr, sizeof(fhdr)) != sizeof(fhdr)) {
        return 0;                                                     // RETURN
    }

    if (fhdr.f_magic != U802TOCMAGIC) {
        return 0;                                                     // RETURN
    }

    if (!fhdr.f_opthdr) {
        return 0;                                                     // RETURN
    }

    AOUTHDR auxhdr;
    if (fhdr.f_opthdr > sizeof(auxhdr)) {
        // something is wrong with this file, the size in 'f_opthdr' should
        // never be larger than the 'AOUTHDR' struct.

        return 0;                                                     // RETURN
    }

    // Make sure everything in 'auxhdr' is 0-initialized in case there is only
    // a partial header in the file.
    memset(&auxhdr,0,sizeof(auxhdr));

    if (read(fd, &auxhdr, fhdr.f_opthdr) != fhdr.f_opthdr) {
        return 0;                                                     // RETURN
    }

    // identify current file location
    off64_t textoffset = lseek(fd, 0, SEEK_CUR);
    if (textoffset < 0) {
        return 0;                                                     // RETURN
    }

    // add sntext to get location of text segment
    textoffset += (auxhdr.o_sntext-1) * sizeof(SCNHDR);

    SCNHDR texthdr;
    if (pread(fd, &texthdr, sizeof(texthdr), textoffset) != sizeof(texthdr)) {
        return 0;                                                     // RETURN
    }

    // base address in current process of text segment
    uintptr_t start_addr = (uintptr_t)currInfo->ldinfo_textorg;

    // text segment address in executable file
    uintptr_t start_text = static_cast<uintptr_t>(auxhdr.text_start)
        - static_cast<off64_t>(texthdr.s_scnptr);

    // offset to alter each address by so that 'showfunc.tsk' will find the
    // correct function information
    return start_addr - start_text;
}

uintptr_t getStackOffset()
{
    static uintptr_t stack_offset = initStackOffset();

    return stack_offset;
}
#endif // BSLS_PLATFORM_OS_AIX


}  // close unnamed namespace



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

    int ret = backtrace(buffer, maxFrames);
    if (0 == ret) {
        void *p;
        backtrace(&p, 1);
        return 0;                                                     // RETURN
    }

    return ret;
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
    return RtlCaptureStackBackTrace(1, maxFrames, buffer, (PDWORD) 0);
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
    // ignore.  So take a pointer to the function, and then do a weird identity
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

#if defined(BSLS_PLATFORM_OS_AIX)
    // On AIX cheapstack expects the stack addresses to be offset based on
    // where the text segment was actually loaded - see DRQS 19990260, DRQS
    // 39366273, and DRQS 121184813.
    //
    // Since this is hard to replicate, we are going to instead use the '-o'
    // argument to 'showfunc.tsk', which has to come before the task name, to
    // provide the offset if it is not '0'.

    static uintptr_t stackOffset = getStackOffset();
    if (stackOffset != 0) {
        printed = snprintf(out,
                           rem,
                           "-o 0x" BSLS_BSLTESTUTIL_FORMAT_PTR " ",
                           stackOffset);
        out += printed;
        rem -= printed;
        if (printed < 0 || rem <= 0) {
            return;                                                   // RETURN
        }
    }
#endif



    if (0 != taskname) {
        printed = snprintf(out, rem, "%s", taskname);
        rem -= printed;
        out += printed;
    }
    else {
        int rc = getProcessName(out, rem);
        if (rc != 0) {
            printed = snprintf(out, rem, "<binary_name_here>");
            rem -= printed;
            out += printed;
        }
        else {
            int tasklen = static_cast<int>(strlen(out));
            out += tasklen;
            rem -= tasklen;
        }
    }


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
