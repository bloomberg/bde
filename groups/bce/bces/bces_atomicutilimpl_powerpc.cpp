// bces_atomicutilimpl_powerpc.cpp -*-C++-*-
#include <bces_atomicutilimpl_powerpc.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bces_atomicutilimpl_powerpc_cpp,"$Id$ $CSID$")

namespace BloombergLP {

#if defined(BSLS_PLATFORM__CPU_POWERPC) && \
    BSLS_PLATFORM__CMP_VER_MAJOR >= 0xa00

// xlc 10+

// Can't inline because DRQS 16073004.  Should be inlined after this is solved.

int bces_AtomicUtilImpl_PowerpcGetInt(const volatile int* value)
{
    int result;
    __asm__ __volatile__ (
             "sync\n\t"
             "1:\n\t"
// This should be just a regular load but a bug in Power5/Power5+ forces us to
// use a lwarx.  see
// http://www.rdrop.com/users/paulmck/scalability/paper/N2745r.2010.01.12a.html
             "lwarx %0, %2, %1\n\t"
             "cmpw %0, %0\n\t" // create data dependency for load/load ordering
             "bne- 1b\n\t"     // never taken
             "isync\n\t"
           : "=r"(result)
           : "r"(value), "i"(0)
           : "cr0", "ctr");
    return result;
}

void bces_AtomicUtilImpl_PowerpcSetInt(volatile int *aInt, int val)
{
    __asm__ __volatile__ (
             "lwsync\n\t"
             "stw %1, %0\n\t"
           : "=m"(*aInt)
           : "r"(val));
}

#if !defined(BSLS_PLATFORM__CPU_64_BIT)

int bces_AtomicUtilImpl_PowerpcSwapInt(volatile int *aInt,
                                       int val)
{
    int result;
    __asm__ __volatile__(
             "lwsync\n\t"
             "1:\n\t"
             "lwarx %0, %3, %1\n\t"
             "stwcx. %2, %3, %1\n\t"
             "bne- 1b\n\t"
             "isync\n\t"
          : "=&r"(result)
          : "r"(aInt), "r"(val), "i"(0)
          : "cr0", "ctr");
    return result;
}

#else

void bces_AtomicUtilImpl_PowerpcSetInt64(
                                       volatile bsls_PlatformUtil::Int64 *aInt,
                                       bsls_PlatformUtil::Int64           val)
{
    __asm__ __volatile__ (
             "lwsync\n\t"
             "std %1, %0\n\t"
           : "=m"(*aInt)
           : "r"(val));
}

bsls_PlatformUtil::Int64 bces_AtomicUtilImpl_PowerpcGetInt64(
                               const volatile bsls_PlatformUtil::Int64 *value)
{
    bsls_PlatformUtil::Int64 result;
    __asm__ __volatile__(
             "sync\n\t"
             "1:\n\t"

// This should be just a regular load but a bug in Power5/Power5+ forces us to
// use a ldarx.  see
// http://www.rdrop.com/users/paulmck/scalability/paper/N2745r.2010.01.12a.html

             "ldarx %0, %2, %1\n\t"
             "cmpd %0, %0\n" // create data dependency for load/load ordering
             "bne- 1b\n\t"   // never taken
             "isync\n\t"
           : "=r"(result)
           : "r"(value), "i"(0)
           : "cr0", "ctr");
    return result;
}

#endif // BSLS_PLATFORM__CPU_64_BIT

#endif // BSLS_PLATFORM__CPU_POWERPC

} // namespace BloombergLP {

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
