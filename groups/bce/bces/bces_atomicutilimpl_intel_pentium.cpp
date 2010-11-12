// bces_atomicutilimpl_intel_pentium.cpp -*-C++-*-
#include <bces_atomicutilimpl_intel_pentium.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bces_atomicutilimpl_intel_pentium_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>

namespace BloombergLP {

#if defined(BSLS_PLATFORM__CPU_X86)

#if defined(__GNUC__) && defined(__MMX__) && !defined(__SSE__)

// This can't be inlined because 'emms' will wipe out the FPU state.  The
// compiler has to save this state before a function call but would not do it
// if inlined.

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumGetInt64(
                              volatile const bsls_PlatformUtil::Int64 *aInt)
{
    register bsls_PlatformUtil::Int64 result;
    __asm__ __volatile__ (
                          "lock addl $0,0(%%esp)\n\t"  // barrier
                          "movq %1,%%mm0\n\t"
                          "movq %%mm0, %0\n\t"
                          "emms\n\t"
                          : "=m"(result)
                          : "m"(*aInt)
                          : "memory", "cc", "mm0");
    return result;
}

void  bces_AtomicUtilImpl_IntelPentiumSetInt64(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64           val)
{
    __asm__ __volatile__ (
                          "movq %1,%0\n\t"
                          "emms\n\t"
                          "lock addl $0,0(%%esp)\n\t"  // barrier
                          : "=m"(*aInt)
                          : "y"(val)
                          : "memory", "cc");
}

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_IntelPentiumGetInt64Relaxed(
                              volatile const bsls_PlatformUtil::Int64 *aInt)
{
    register bsls_PlatformUtil::Int64 result;
    __asm__ __volatile__ (
                          "movq %1,%%mm0\n\t"
                          "movq %%mm0, %0\n\t"
                          "emms\n\t"
                          : "=m"(result)
                          : "m"(*aInt)
                          : "mm0");
    return result;
}

void  bces_AtomicUtilImpl_IntelPentiumSetInt64Relaxed(
                                      volatile bsls_PlatformUtil::Int64 *aInt,
                                      bsls_PlatformUtil::Int64           val)
{
    __asm__ __volatile__ (
                          "movq %1,%0\n\t"
                          "emms\n\t"
                          : "=m"(*aInt)
                          : "y"(val)
                          : );
}

#elif defined(BSLS_PLATFORM__OS_SOLARIS)

int bces_AtomicUtilImpl_IntelPentiumGetInt(volatile const int *aInt)
{
    return *aInt;
}

static void bces_AtomicUtilImpl_IntelPentium_Common()
{
    asm("                                                                  \n\
    .text                                                                  \n\
.globl bces_AtomicUtilImpl_IntelPentiumSwapInt                             \n\
    .type   bces_AtomicUtilImpl_IntelPentiumSwapInt, @function             \n\
bces_AtomicUtilImpl_IntelPentiumSwapInt:                                   \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    movl    8(%ebp), %ecx                                                  \n\
    movl    12(%ebp), %edx                                                 \n\
    movl    8(%ebp), %eax                                                  \n\
    lock;xchgl %edx,(%ecx)                                                 \n\
    movl    %edx, %eax                                                     \n\
    movl    %eax, 12(%ebp)                                                 \n\
    movl    12(%ebp), %eax                                                 \n\
    popl    %ebp                                                           \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumSwapInt,\
 .-bces_AtomicUtilImpl_IntelPentiumSwapInt\n\
.globl bces_AtomicUtilImpl_IntelPentiumGetInt                              \n\
    .type   bces_AtomicUtilImpl_IntelPentiumGetInt, @function              \n\
bces_AtomicUtilImpl_IntelPentiumGetInt:                                    \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    movl    8(%ebp), %eax                                                  \n\
    movl    (%eax), %eax                                                   \n\
    popl    %ebp                                                           \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumGetInt,\
 .-bces_AtomicUtilImpl_IntelPentiumGetInt\n\
.globl bces_AtomicUtilImpl_IntelPentiumAddInt                              \n\
    .type   bces_AtomicUtilImpl_IntelPentiumAddInt, @function              \n\
bces_AtomicUtilImpl_IntelPentiumAddInt:                                    \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    subl    $4, %esp                                                       \n\
    movl    12(%ebp), %eax                                                 \n\
    movl    %eax, -4(%ebp)                                                 \n\
    movl    8(%ebp), %edx                                                  \n\
    movl    12(%ebp), %eax                                                 \n\
    lock;xaddl %eax, (%edx)                                                \n\
    movl    %eax, 12(%ebp)                                                 \n\
    movl    -4(%ebp), %eax                                                 \n\
    addl    12(%ebp), %eax                                                 \n\
    leave                                                                  \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumAddInt,\
 .-bces_AtomicUtilImpl_IntelPentiumAddInt\n\
.globl bces_AtomicUtilImpl_IntelPentiumAddIntRelaxed                       \n\
    .type   bces_AtomicUtilImpl_IntelPentiumAddIntRelaxed, @function       \n\
bces_AtomicUtilImpl_IntelPentiumAddIntRelaxed:                             \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    subl    $4, %esp                                                       \n\
    movl    12(%ebp), %eax                                                 \n\
    movl    %eax, -4(%ebp)                                                 \n\
    movl    8(%ebp), %edx                                                  \n\
    movl    12(%ebp), %eax                                                 \n\
    lock;xaddl %eax, (%edx)                                                \n\
    movl    %eax, 12(%ebp)                                                 \n\
    movl    -4(%ebp), %eax                                                 \n\
    addl    12(%ebp), %eax                                                 \n\
    leave                                                                  \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumAddIntRelaxed,\
 .-bces_AtomicUtilImpl_IntelPentiumAddIntRelaxed\n\
.globl bces_AtomicUtilImpl_IntelPentiumTestAndSwap                         \n\
    .type   bces_AtomicUtilImpl_IntelPentiumTestAndSwap, @function         \n\
bces_AtomicUtilImpl_IntelPentiumTestAndSwap:                               \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    pushl   %ebx                                                           \n\
    subl    $4, %esp                                                       \n\
    movl    12(%ebp), %edx                                                 \n\
    movl    16(%ebp), %ecx                                                 \n\
    movl    8(%ebp), %eax                                                  \n\
    movl    %eax, -8(%ebp)                                                 \n\
    movl    %edx, %eax                                                     \n\
    movl    -8(%ebp), %ebx                                                 \n\
    lock;cmpxchgl %ecx, (%ebx)                                             \n\
    movl    %eax, %edx                                                     \n\
    movl    %edx, 12(%ebp)                                                 \n\
    movl    12(%ebp), %eax                                                 \n\
    addl    $4, %esp                                                       \n\
    popl    %ebx                                                           \n\
    popl    %ebp                                                           \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumTestAndSwap,\
 .-bces_AtomicUtilImpl_IntelPentiumTestAndSwap\n\
.globl bces_AtomicUtilImpl_IntelPentiumSwapInt64                           \n\
    .type   bces_AtomicUtilImpl_IntelPentiumSwapInt64, @function           \n\
bces_AtomicUtilImpl_IntelPentiumSwapInt64:                                 \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    pushl   %edi                                                           \n\
    pushl   %ebx                                                           \n\
    movl    8(%ebp), %edi                                                  \n\
    movl    12(%ebp), %ebx                                                 \n\
    movl    12(%ebp), %eax                                                 \n\
    movl    16(%ebp), %edx                                                 \n\
    movl    %edx, %eax                                                     \n\
    movl    %eax, %edx                                                     \n\
    sarl    $31, %edx                                                      \n\
    movl    %eax, %ecx                                                     \n\
    movl (%edi), %eax                                                      \n\
    movl 4(%edi), %edx                                                     \n\
1:  lock;cmpxchg8b (%edi)                                                  \n\
    jnz 1b                                                                 \n\
    popl    %ebx                                                           \n\
    popl    %edi                                                           \n\
    popl    %ebp                                                           \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumSwapInt64,\
 .-bces_AtomicUtilImpl_IntelPentiumSwapInt64\n\
.globl bces_AtomicUtilImpl_IntelPentiumGetInt64                            \n\
    .type   bces_AtomicUtilImpl_IntelPentiumGetInt64, @function            \n\
bces_AtomicUtilImpl_IntelPentiumGetInt64:                                  \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    pushl   %edi                                                           \n\
    movl    8(%ebp), %edi                                                  \n\
    movl %ebx, %eax                                                        \n\
    movl %ecx, %edx                                                        \n\
    lock;cmpxchg8b (%edi)                                                  \n\
    popl    %edi                                                           \n\
    popl    %ebp                                                           \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumGetInt64,\
 .-bces_AtomicUtilImpl_IntelPentiumGetInt64\n\
.globl bces_AtomicUtilImpl_IntelPentiumAddInt64                            \n\
    .type   bces_AtomicUtilImpl_IntelPentiumAddInt64, @function            \n\
bces_AtomicUtilImpl_IntelPentiumAddInt64:                                  \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    pushl   %edi                                                           \n\
    pushl   %esi                                                           \n\
    pushl   %ebx                                                           \n\
    movl    8(%ebp), %edi                                                  \n\
    leal    12(%ebp), %esi                                                 \n\
    movl (%edi), %eax                                                      \n\
    movl 4(%edi), %edx                                                     \n\
1:  movl %eax, %ebx                                                        \n\
    movl %edx, %ecx                                                        \n\
    addl (%esi), %ebx                                                      \n\
    adcl 4(%esi), %ecx                                                     \n\
    lock;cmpxchg8b (%edi)                                                  \n\
    jnz 1b                                                                 \n\
    movl %ebx, %eax                                                        \n\
    movl %ecx, %edx                                                        \n\
    popl    %ebx                                                           \n\
    popl    %esi                                                           \n\
    popl    %edi                                                           \n\
    popl    %ebp                                                           \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumAddInt64,\
 .-bces_AtomicUtilImpl_IntelPentiumAddInt64\n\
.globl bces_AtomicUtilImpl_IntelPentiumTestAndSwap64                       \n\
    .type   bces_AtomicUtilImpl_IntelPentiumTestAndSwap64, @function       \n\
bces_AtomicUtilImpl_IntelPentiumTestAndSwap64:                             \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    pushl   %edi                                                           \n\
    pushl   %esi                                                           \n\
    pushl   %ebx                                                           \n\
    subl    $20, %esp                                                      \n\
    movl    8(%ebp), %edi                                                  \n\
    movl    12(%ebp), %eax                                                 \n\
    movl    16(%ebp), %edx                                                 \n\
    movl    %eax, -32(%ebp)                                                \n\
    movl    %edx, -28(%ebp)                                                \n\
    movl    20(%ebp), %esi                                                 \n\
    movl    20(%ebp), %eax                                                 \n\
    movl    24(%ebp), %edx                                                 \n\
    movl    %edx, %eax                                                     \n\
    movl    %eax, %edx                                                     \n\
    sarl    $31, %edx                                                      \n\
    movl    %eax, -24(%ebp)                                                \n\
    movl    %edx, -20(%ebp)                                                \n\
    movl    -32(%ebp), %eax                                                \n\
    movl    -28(%ebp), %edx                                                \n\
    movl    %esi, %ebx                                                     \n\
    movl    -24(%ebp), %ecx                                                \n\
    lock;cmpxchg8b (%edi)                                                  \n\
    movl    %eax, -24(%ebp)                                                \n\
    movl    %edx, -20(%ebp)                                                \n\
    movl    -24(%ebp), %eax                                                \n\
    movl    -20(%ebp), %edx                                                \n\
    addl    $20, %esp                                                      \n\
    popl    %ebx                                                           \n\
    popl    %esi                                                           \n\
    popl    %edi                                                           \n\
    popl    %ebp                                                           \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumTestAndSwap64,\
 .-bces_AtomicUtilImpl_IntelPentiumTestAndSwap64\n\
.globl bces_AtomicUtilImpl_IntelPentiumSpinLock                            \n\
    .type   bces_AtomicUtilImpl_IntelPentiumSpinLock, @function            \n\
bces_AtomicUtilImpl_IntelPentiumSpinLock:                                  \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    movl    8(%ebp), %edx                                                  \n\
    movb $0xff, %ah                                                        \n\
1:  xorb %al,%al                                                           \n\
    lock;cmpxchgb %ah,(%edx)                                               \n\
    jnz 1b                                                                 \n\
    popl    %ebp                                                           \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumSpinLock,\
 .-bces_AtomicUtilImpl_IntelPentiumSpinLock\n\
.globl bces_AtomicUtilImpl_IntelPentiumSpinTryLock                         \n\
    .type   bces_AtomicUtilImpl_IntelPentiumSpinTryLock, @function         \n\
bces_AtomicUtilImpl_IntelPentiumSpinTryLock:                               \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    subl    $8, %esp                                                       \n\
    movl    8(%ebp), %eax                                                  \n\
    movl    %eax, -8(%ebp)                                                 \n\
    movl    12(%ebp), %ecx                                                 \n\
    movl    -4(%ebp), %eax                                                 \n\
    movl    -8(%ebp), %edx                                                 \n\
    movl $0xff00, %eax                                                     \n\
1:  xorb %al,%al                                                           \n\
    lock;cmpxchgb %ah,(%edx)                                               \n\
    loopnz 1b                                                              \n\
    xorb %ah,%ah                                                           \n\
    movl    %eax, -4(%ebp)                                                 \n\
    movl    -4(%ebp), %eax                                                 \n\
    leave                                                                  \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumSpinTryLock,\
 .-bces_AtomicUtilImpl_IntelPentiumSpinTryLock\n\
.globl bces_AtomicUtilImpl_IntelPentiumSpinUnlock                          \n\
    .type   bces_AtomicUtilImpl_IntelPentiumSpinUnlock, @function          \n\
bces_AtomicUtilImpl_IntelPentiumSpinUnlock:                                \n\
    pushl   %ebp                                                           \n\
    movl    %esp, %ebp                                                     \n\
    movl    8(%ebp), %eax                                                  \n\
    movl    $0, %edx                                                       \n\
    lock;xchgb %dl,(%eax)                                                  \n\
    popl    %ebp                                                           \n\
    ret                                                                    \n\
    .size   bces_AtomicUtilImpl_IntelPentiumSpinUnlock,\
 .-bces_AtomicUtilImpl_IntelPentiumSpinUnlock\n\
    ");
}
#endif // Solaris

#endif // X86


}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
