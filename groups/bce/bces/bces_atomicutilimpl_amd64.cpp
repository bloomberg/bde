// bces_atomicutilimpl_amd64.cpp -*-C++-*-
#include <bces_atomicutilimpl_amd64.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bces_atomicutilimpl_amd64_cpp,"$Id$ $CSID$")

namespace BloombergLP {

#if defined(BSLS_PLATFORM__CPU_X86_64)      // x86_64 only

#if defined(BSLS_PLATFORM__CMP_GNU)

int bces_AtomicUtilImpl_Amd64SwapInt(volatile int *aInt,
                                     int           val)
{
    // This function uses a weaker inline assembly constraint than required,
    // therefore it's un-inlined to reduce the possibility of a compiler
    // optimization that takes advantage of that weaker constraint to produce
    // code with the behavior different from intended.

    __asm__ __volatile__ (
            "       lock xchgl %1, %0       \n\t"
                    : "=m" (*aInt),   // needs "+m" but it causes a GCC
                                      // compiler error in optimized builds
                                      // (DRQS #21987142)
                      "+r" (val)
                    :
                    : "memory");
    return val;
}

#elif defined(BSLS_PLATFORM__CMP_SUN)

// Please see this component's header file for a brief explanation of
// why things are this way.

int bces_AtomicUtilImpl_Amd64GetInt(volatile const int *aInt)
{
    return *aInt;
}

static void bces_AtomicUtilImpl_Amd64_Common()
{
    asm(".text                                                             \n\
.globl bces_AtomicUtilImpl_Amd64SetInt                                     \n\
.type   bces_AtomicUtilImpl_Amd64SetInt, @function                         \n\
bces_AtomicUtilImpl_Amd64SetInt:                                           \n\
    movl %esi,(%rdi)                                                       \n\
    mfence                                                                 \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64SetInt, .-bces_AtomicUtilImpl_Amd64SetInt \n\
.globl bces_AtomicUtilImpl_Amd64SwapInt                                    \n\
.type   bces_AtomicUtilImpl_Amd64SwapInt, @function                        \n\
bces_AtomicUtilImpl_Amd64SwapInt:                                          \n\
    lock;xchgl %esi,(%rdi)                                                 \n\
    movl    %esi, %eax                                                     \n\
    ret                                                                    \n\
.size  bces_AtomicUtilImpl_Amd64SwapInt, .-bces_AtomicUtilImpl_Amd64SwapInt\n\
.globl bces_AtomicUtilImpl_Amd64GetInt                                     \n\
.type   bces_AtomicUtilImpl_Amd64GetInt, @function                         \n\
bces_AtomicUtilImpl_Amd64GetInt:                                           \n\
    movl    (%rdi), %eax                                                   \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64GetInt, .-bces_AtomicUtilImpl_Amd64GetInt \n\
.globl bces_AtomicUtilImpl_Amd64AddInt                                     \n\
.type   bces_AtomicUtilImpl_Amd64AddInt, @function                         \n\
bces_AtomicUtilImpl_Amd64AddInt:                                           \n\
    movl    %esi, %eax                                                     \n\
    lock;xaddl %eax, (%rdi)                                                \n\
    addl    %esi, %eax                                                     \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64AddInt, .-bces_AtomicUtilImpl_Amd64AddInt \n\
.globl bces_AtomicUtilImpl_Amd64TestAndSwap                                \n\
.type   bces_AtomicUtilImpl_Amd64TestAndSwap, @function                    \n\
bces_AtomicUtilImpl_Amd64TestAndSwap:                                      \n\
    movl    %esi, %eax                                                     \n\
    lock;cmpxchgl %edx, (%rdi)                                             \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64TestAndSwap,\
 .-bces_AtomicUtilImpl_Amd64TestAndSwap \n\
.globl bces_AtomicUtilImpl_Amd64SetInt64                                   \n\
.type   bces_AtomicUtilImpl_Amd64SetInt64, @function                       \n\
bces_AtomicUtilImpl_Amd64SetInt64:                                         \n\
    lock;xchgq %rsi,(%rdi)                                                 \n\
    movq    %rsi, %rax                                                     \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64SetInt64,\
 .-bces_AtomicUtilImpl_Amd64SetInt64 \n\
.globl bces_AtomicUtilImpl_Amd64SwapInt64                                  \n\
.type   bces_AtomicUtilImpl_Amd64SwapInt64, @function                      \n\
bces_AtomicUtilImpl_Amd64SwapInt64:                                        \n\
    lock;xchgq %rsi,(%rdi)                                                 \n\
    movq    %rsi, %rax                                                     \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64SwapInt64,\
 .-bces_AtomicUtilImpl_Amd64SwapInt64 \n\
.globl bces_AtomicUtilImpl_Amd64GetInt64                                   \n\
.type   bces_AtomicUtilImpl_Amd64GetInt64, @function                       \n\
bces_AtomicUtilImpl_Amd64GetInt64:                                         \n\
    movq    (%rdi), %rax                                                   \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64GetInt64,\
 .-bces_AtomicUtilImpl_Amd64GetInt64 \n\
.globl bces_AtomicUtilImpl_Amd64AddInt64                                   \n\
.type   bces_AtomicUtilImpl_Amd64AddInt64, @function                       \n\
bces_AtomicUtilImpl_Amd64AddInt64:                                         \n\
    movq    %rsi, %rax                                                     \n\
    lock;xaddq %rax, (%rdi)                                                \n\
    addq    %rsi, %rax                                                     \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64AddInt64,\
 .-bces_AtomicUtilImpl_Amd64AddInt64 \n\
.globl bces_AtomicUtilImpl_Amd64TestAndSwap64                              \n\
.type   bces_AtomicUtilImpl_Amd64TestAndSwap64, @function                  \n\
bces_AtomicUtilImpl_Amd64TestAndSwap64:                                    \n\
    movq    %rsi, %rax                                                     \n\
    lock;cmpxchgq %rdx, (%rdi)                                             \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64TestAndSwap64,\
 .-bces_AtomicUtilImpl_Amd64TestAndSwap64 \n\
.globl bces_AtomicUtilImpl_Amd64SpinLock                                   \n\
.type   bces_AtomicUtilImpl_Amd64SpinLock, @function                       \n\
bces_AtomicUtilImpl_Amd64SpinLock:                                         \n\
    movb    $0xff, %ah                                                     \n\
1:  xorb    %al, %al                                                       \n\
    lock;cmpxchgb %ah, (%rdi)                                              \n\
    jnz 1b                                                                 \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64SpinLock,\
 .-bces_AtomicUtilImpl_Amd64SpinLock \n\
.globl bces_AtomicUtilImpl_Amd64SpinTryLock                                \n\
.type   bces_AtomicUtilImpl_Amd64SpinTryLock, @function                    \n\
bces_AtomicUtilImpl_Amd64SpinTryLock:                                      \n\
    movb    $0xff, %ah                                                     \n\
    movl    %esi, %ecx                                                     \n\
1:  xorb    %al, %al                                                       \n\
    lock;cmpxchgb %ah, (%rdi)                                              \n\
    loopnz  1b                                                             \n\
    xorb    %ah, %ah                                                       \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64SpinTryLock,\
 .-bces_AtomicUtilImpl_Amd64SpinTryLock \n\
.globl bces_AtomicUtilImpl_Amd64SpinUnlock                                 \n\
.type   bces_AtomicUtilImpl_Amd64SpinUnlock, @function                     \n\
bces_AtomicUtilImpl_Amd64SpinUnlock:                                       \n\
    movl    $0, %edx                                                       \n\
    lock;xchgb %dl,(%rdi)                                                  \n\
    ret                                                                    \n\
.size   bces_AtomicUtilImpl_Amd64SpinUnlock,\
 .-bces_AtomicUtilImpl_Amd64SpinUnlock \n\
    ");
}

#endif
#endif  // defined(BSLS_PLATFORM__CPU_X86_64)

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
