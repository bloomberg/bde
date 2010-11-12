.set r0,0; .set SP,1; .set RTOC,2; .set r3,3; .set r4,4
.set r5,5; .set r6,6; .set r7,7; .set r8,8; .set r9,9
.set r10,10; .set r11,11; .set r12,12; .set r13,13; .set r14,14
.set r15,15; .set r16,16; .set r17,17; .set r18,18; .set r19,19
.set r20,20; .set r21,21; .set r22,22; .set r23,23; .set r24,24
.set r25,25; .set r26,26; .set r27,27; .set r28,28; .set r29,29
.set r30,30; .set r31,31
.set fp0,0; .set fp1,1; .set fp2,2; .set fp3,3; .set fp4,4
.set fp5,5; .set fp6,6; .set fp7,7; .set fp8,8; .set fp9,9
.set fp10,10; .set fp11,11; .set fp12,12; .set fp13,13; .set fp14,14
.set fp15,15; .set fp16,16; .set fp17,17; .set fp18,18; .set fp19,19
.set fp20,20; .set fp21,21; .set fp22,22; .set fp23,23; .set fp24,24
.set fp25,25; .set fp26,26; .set fp27,27; .set fp28,28; .set fp29,29
.set fp30,30; .set fp31,31
.set MQ,0; .set XER,1; .set FROM_RTCU,4; .set FROM_RTCL,5; .set FROM_DEC,6
.set LR,8; .set CTR,9; .set TID,17; .set DSISR,18; .set DAR,19; .set TO_RTCU,20
.set TO_RTCL,21; .set TO_DEC,22; .set SDR_0,24; .set SDR_1,25; .set SRR_0,26
.set SRR_1,27
.set BO_dCTR_NZERO_AND_NOT,0; .set BO_dCTR_NZERO_AND_NOT_1,1
.set BO_dCTR_ZERO_AND_NOT,2; .set BO_dCTR_ZERO_AND_NOT_1,3
.set BO_IF_NOT,4; .set BO_IF_NOT_1,5; .set BO_IF_NOT_2,6
.set BO_IF_NOT_3,7; .set BO_dCTR_NZERO_AND,8; .set BO_dCTR_NZERO_AND_1,9
.set BO_dCTR_ZERO_AND,10; .set BO_dCTR_ZERO_AND_1,11; .set BO_IF,12
.set BO_IF_1,13; .set BO_IF_2,14; .set BO_IF_3,15; .set BO_dCTR_NZERO,16
.set BO_dCTR_NZERO_1,17; .set BO_dCTR_ZERO,18; .set BO_dCTR_ZERO_1,19
.set BO_ALWAYS,20; .set BO_ALWAYS_1,21; .set BO_ALWAYS_2,22
.set BO_ALWAYS_3,23; .set BO_dCTR_NZERO_8,24; .set BO_dCTR_NZERO_9,25
.set BO_dCTR_ZERO_8,26; .set BO_dCTR_ZERO_9,27; .set BO_ALWAYS_8,28
.set BO_ALWAYS_9,29; .set BO_ALWAYS_10,30; .set BO_ALWAYS_11,31
.set CR0_LT,0; .set CR0_GT,1; .set CR0_EQ,2; .set CR0_SO,3
.set CR1_FX,4; .set CR1_FEX,5; .set CR1_VX,6; .set CR1_OX,7
.set CR2_LT,8; .set CR2_GT,9; .set CR2_EQ,10; .set CR2_SO,11
.set CR3_LT,12; .set CR3_GT,13; .set CR3_EQ,14; .set CR3_SO,15
.set CR4_LT,16; .set CR4_GT,17; .set CR4_EQ,18; .set CR4_SO,19
.set CR5_LT,20; .set CR5_GT,21; .set CR5_EQ,22; .set CR5_SO,23
.set CR6_LT,24; .set CR6_GT,25; .set CR6_EQ,26; .set CR6_SO,27
.set CR7_LT,28; .set CR7_GT,29; .set CR7_EQ,30; .set CR7_SO,31
.set TO_LT,16; .set TO_GT,8; .set TO_EQ,4; .set TO_LLT,2; .set TO_LGT,1
.machine "ppc64"



    .rename    H.10.NO_SYMBOL{PR},""
    .rename    H.44.bces_AtomicUtilImpl_PowerpcSetInt64Relaxed{TC},"bces_AtomicUtilImpl_PowerpcSetInt64Relaxed"
    .rename    H.48.bces_AtomicUtilImpl_PowerpcAddInt64Relaxed{TC},"bces_AtomicUtilImpl_PowerpcAddInt64Relaxed"
    .rename    H.52.bces_AtomicUtilImpl_PowerpcGetInt64Relaxed{TC},"bces_AtomicUtilImpl_PowerpcGetInt64Relaxed"
    .rename    H.56.bces_AtomicUtilImpl_PowerpcAddInt{TC},"bces_AtomicUtilImpl_PowerpcAddInt"
    .rename    H.60.bces_AtomicUtilImpl_PowerpcTestAndSwap{TC},"bces_AtomicUtilImpl_PowerpcTestAndSwap"
    .rename    H.64.bces_AtomicUtilImpl_PowerpcSetInt64{TC},"bces_AtomicUtilImpl_PowerpcSetInt64"
    .rename    H.68.bces_AtomicUtilImpl_PowerpcSwapInt64{TC},"bces_AtomicUtilImpl_PowerpcSwapInt64"
    .rename    H.72.bces_AtomicUtilImpl_PowerpcGetInt64{TC},"bces_AtomicUtilImpl_PowerpcGetInt64"
    .rename    H.76.bces_AtomicUtilImpl_PowerpcAddInt64{TC},"bces_AtomicUtilImpl_PowerpcAddInt64"
    .rename    H.80.bces_AtomicUtilImpl_PowerpcTestAndSwap64{TC},"bces_AtomicUtilImpl_PowerpcTestAndSwap64"
    .rename    H.84.bces_AtomicUtilImpl_PowerpcSpinLock{TC},"bces_AtomicUtilImpl_PowerpcSpinLock"
    .rename    H.88.bces_AtomicUtilImpl_PowerpcSpinTryLock{TC},"bces_AtomicUtilImpl_PowerpcSpinTryLock"
    .rename    H.92.bces_AtomicUtilImpl_PowerpcSpinUnlock{TC},"bces_AtomicUtilImpl_PowerpcSpinUnlock"


    .file "bces_atomicutilimpl_powerpc.ibm.s"

    .lglobl    H.10.NO_SYMBOL{PR}
    .globl    .bces_AtomicUtilImpl_PowerpcSetInt64Relaxed
    .globl    .bces_AtomicUtilImpl_PowerpcAddInt64Relaxed
    .globl    .bces_AtomicUtilImpl_PowerpcGetInt64Relaxed
    .globl    .bces_AtomicUtilImpl_PowerpcAddInt
    .globl    .bces_AtomicUtilImpl_PowerpcTestAndSwap
    .globl    .bces_AtomicUtilImpl_PowerpcSetInt64
    .globl    .bces_AtomicUtilImpl_PowerpcSwapInt64
    .globl    .bces_AtomicUtilImpl_PowerpcGetInt64
    .globl    .bces_AtomicUtilImpl_PowerpcAddInt64
    .globl    .bces_AtomicUtilImpl_PowerpcTestAndSwap64
    .globl    .bces_AtomicUtilImpl_PowerpcSpinLock
    .globl    .bces_AtomicUtilImpl_PowerpcSpinTryLock
    .globl    .bces_AtomicUtilImpl_PowerpcSpinUnlock
    .globl    bces_AtomicUtilImpl_PowerpcSetInt64Relaxed{DS}
    .globl    bces_AtomicUtilImpl_PowerpcAddInt64Relaxed{DS}
    .globl    bces_AtomicUtilImpl_PowerpcGetInt64Relaxed{DS}
    .globl    bces_AtomicUtilImpl_PowerpcAddInt{DS}
    .globl    bces_AtomicUtilImpl_PowerpcTestAndSwap{DS}
    .globl    bces_AtomicUtilImpl_PowerpcSetInt64{DS}
    .globl    bces_AtomicUtilImpl_PowerpcSwapInt64{DS}
    .globl    bces_AtomicUtilImpl_PowerpcGetInt64{DS}
    .globl    bces_AtomicUtilImpl_PowerpcAddInt64{DS}
    .globl    bces_AtomicUtilImpl_PowerpcTestAndSwap64{DS}
    .globl    bces_AtomicUtilImpl_PowerpcSpinLock{DS}
    .globl    bces_AtomicUtilImpl_PowerpcSpinTryLock{DS}
    .globl    bces_AtomicUtilImpl_PowerpcSpinUnlock{DS}

    .csect    H.10.NO_SYMBOL{PR}

    # void bces_AtomicUtilImpl_PowerpcSetInt64Relaxed(void *aInt, int val);
.bces_AtomicUtilImpl_PowerpcSetInt64Relaxed:
    sldi r4, r4, 32       # shift left r4 so its low 32-bit become the high 32
    insrdi r4, r5, 32, 32 # insert r5 low 32-bit into r4 low 32-bit
                          # r4 has now 'val' as a full 64-bit value
    std r4,0(r3)        # store value in memory (atomic)
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x0000000c        # TB_OFFSET
# End of traceback table

    # long long bces_AtomicUtilImpl_PowerpcAddInt64Relaxed(void *aInt, long long val);
#    .globl .bces_AtomicUtilImpl_PowerpcAddInt64Relaxed
.bces_AtomicUtilImpl_PowerpcAddInt64Relaxed:
    sldi r6, r4, 32       # shift left r4 into r6
                          # so r4 low 32-bit become r6 high 32
    insrdi r6, r5, 32, 32 # insert r5 low 32-bit into r6 low 32-bit
                          # r6 has now 'val' as a full 64-bit value
bces_AtomicUtilImpl_PowerpcAddInt64Relaxed_1:
    ldarx r4,0,r3   #load and reserve
    add r4, r4, r6  #add the operand
    stdcx. r4,0,r3  #store old value if still reserved
    bne- bces_AtomicUtilImpl_PowerpcAddInt64Relaxed_1
    srdi r3, r4, 32         # move high 32-bit of r4 into r3 low 32-bit
                            # NOTE: we're leaving the high 32-bits of r4
                            # still set, but the caller will not see them
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x000000018        # TB_OFFSET
# End of traceback table


    # int bces_AtomicUtilImpl_PowerpcGetInt64Relaxed(const volatile void*);
#    .globl .bces_AtomicUtilImpl_PowerpcGetInt64Relaxed
.bces_AtomicUtilImpl_PowerpcGetInt64Relaxed:
    ld r4, 0(r3)            # load double word (atomic)
    srdi r3, r4, 32         # move high 32-bit of r4 into r3 low 32-bit
                            # NOTE: we're leaving the high 32-bits of r4
                            # still set, but the caller will not see them
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x00000018        # TB_OFFSET
# End of traceback table

    # int bces_AtomicUtilImpl_PowerpcAddInt(void *aInt, int val);
#    .globl .bces_AtomicUtilImpl_PowerpcAddInt
.bces_AtomicUtilImpl_PowerpcAddInt:
    lwsync
bces_AtomicUtilImpl_PowerpcAddInt_1:
    lwarx r5,0,r3   #load and reserve
    add r5, r4, r5  #add the operand
    stwcx. r5,0,r3  #store old value if still reserved
    bne- bces_AtomicUtilImpl_PowerpcAddInt_1
    mr r3, r5
    isync
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x0000001c        # TB_OFFSET
# End of traceback table

    # int bces_AtomicUtilImpl_PowerpcTestAndSwap(void *aInt,
    #                                            int cmpVal,
    #                         int swapVal);
#    .globl .bces_AtomicUtilImpl_PowerpcTestAndSwap
.bces_AtomicUtilImpl_PowerpcTestAndSwap:
    sync                # provide seq consistency
bces_AtomicUtilImpl_PowerpcTestAndSwap_1:
    lwarx r6,0,r3       #load and reserve
    cmpw  r4,r6         #compare the values
    bne- bces_AtomicUtilImpl_PowerpcTestAndSwap_2
    stwcx. r5,0,r3      #Store the new value
    bne- bces_AtomicUtilImpl_PowerpcTestAndSwap_1 #loop if lost reservation
bces_AtomicUtilImpl_PowerpcTestAndSwap_2:
    mr r3, r6
    isync
    blr

    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x00000020        # TB_OFFSET
# End of traceback table

# IMPLEMENTATION NOTES of 64-bit operations:
# ==========================================
#
# This atomic implementation is using 64-bit instructions, so it will *NOT*
# work unless the CPU is a 64-bit processor running in 32-bit mode.
# Since we're in 32-bit mode, the arguments are received in 32-bit registers.
# A 64-bit value is passed through a pair of registers ri contains
# the high 32-bit and ri+1 the low 32-bit.
# e.g 0x1234567890 could be passed as r4 == 0x12 and r5 == 0x34567890
# It takes one instruction to convert a r4:r5 pair into a full 64-bit value
# into r5.
# We use only one instruction to split a 64-bit register into (32-bit r3,
# 32-bit r4): by keeping the return value into a 64-bit r4, we simply have to
# extract the high 32-bit of r4 into r3 low 32-bit.  The high bits of r4 are
# still set, but a 32-bit application will not see them.

# void bces_AtomicUtilImpl_PowerpcSetInt64(long long *aInt, long long val);
    .globl .bces_AtomicUtilImpl_PowerpcSetInt64
.bces_AtomicUtilImpl_PowerpcSetInt64:
    rldimi r5, r4, 32, 64 # r5 has now 'val' as a full 64-bit value
    lwsync
    std r5,0(r3)          # store value in memory (atomic)
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x00000024        # TB_OFFSET
# End of traceback table


# long long bces_AtomicUtilImpl_PowerpcSwapInt64(void *aInt, long long val);
#    .globl .bces_AtomicUtilImpl_PowerpcSwapInt64
.bces_AtomicUtilImpl_PowerpcSwapInt64:
    rldimi r5, r4, 32, 64   # r5 has now 'val' as a full 64-bit value
    lwsync
bces_AtomicUtilImpl_PowerpcSwapInt64_1:
    ldarx r4, 0, r3
    stdcx. r5, 0, r3
    bne- bces_AtomicUtilImpl_PowerpcSwapInt64_1
    srdi r3, r4, 32         # move high 32-bit of r4 into r3 low 32-bit
                            # NOTE: we're leaving the high 32-bits of r4
                            # still set, but the caller will not see them
    isync
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x00000003c        # TB_OFFSET
# End of traceback table

# long long bces_AtomicUtilImpl_PowerpcGetInt64(const volatile void*);
#    .globl .bces_AtomicUtilImpl_PowerpcGetInt64
.bces_AtomicUtilImpl_PowerpcGetInt64:
    sync
bces_AtomicUtilImpl_PowerpcGetInt64_1:
    ld r4, 0(r3)            # load double word (atomic)
    cmpd r4, r4             # create fake data dependency for ordering
    bne- bces_AtomicUtilImpl_PowerpcGetInt64_1
                            # never taken
    srdi r3, r4, 32         # move high 32-bit of r4 into r3 low 32-bit
                            # NOTE: we're leaving the high 32-bits of r4
                            # still set, but the caller will not see them
    isync
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x00000044        # TB_OFFSET
# End of traceback table


# long long bces_AtomicUtilImpl_PowerpcAddInt64(void *aInt, long long val);
#    .globl .bces_AtomicUtilImpl_PowerpcAddInt64
.bces_AtomicUtilImpl_PowerpcAddInt64:
    rldimi r5, r4, 32, 64   # r5 has now 'val' as a full 64-bit value
    lwsync
bces_AtomicUtilImpl_PowerpcAddInt64_1:
    ldarx r4,0,r3   #load and reserve
    add r4, r4, r5  #add the operand
    stdcx. r4,0,r3  #store old value if still reserved
    bne- bces_AtomicUtilImpl_PowerpcAddInt64_1
    srdi r3, r4, 32         # move high 32-bit of r4 into r3 low 32-bit
                            # NOTE: we're leaving the high 32-bits of r4
                            # still set, but the caller will not see them
    isync
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x00000040        # TB_OFFSET
# End of traceback table

# long long bces_AtomicUtilImpl_PowerpcTestAndSwap64(void *aInt,
#                                           long long cmpVal,
#                            long long swapVal);
#    .globl .bces_AtomicUtilImpl_PowerpcTestAndSwap64
.bces_AtomicUtilImpl_PowerpcTestAndSwap64:
    sync                  # provide seq consistency
    rldimi r5, r4, 32, 64 # r5 has now 'cmpVal' as a full 64-bit value
    rldimi r7, r6, 32, 64 # r7 has now 'swapVal' as a full 64-bit value
bces_AtomicUtilImpl_PowerpcTestAndSwap64_1:
    ldarx r4,0,r3         # oad and reserve
    cmpd  r5,r4           # compare the values
    bne- bces_AtomicUtilImpl_PowerpcTestAndSwap64_2
    stdcx. r7,0,r3        # Store the new value
    bne- bces_AtomicUtilImpl_PowerpcTestAndSwap64_1 #loop if lost
                                                    #reservation.  We can't
                                                    # return since we do not
                                                    # have the current value
bces_AtomicUtilImpl_PowerpcTestAndSwap64_2:
    srdi r3, r4, 32         # move high 32-bit of r4 into r3 low 32-bit
                            # NOTE: we're leaving the high 32-bits of r4
                            # still set, but the caller will not see them

    isync
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x0000004c        # TB_OFFSET
# End of traceback table


# void bces_AtomicUtilImpl_PowerpcSpinLock(volatile char *aSpin);
#       .globl .bces_AtomicUtilImpl_PowerpcSpinLock
.bces_AtomicUtilImpl_PowerpcSpinLock:
    lwsync
bces_AtomicUtilImpl_PowerpcSpinLock_1:
    lwarx r4,0,r3    #load and reserve
    cmpwi r4,0          #compare the values
    bne- bces_AtomicUtilImpl_PowerpcSpinLock_1
    stwcx. r3,0,r3   #Store the new value
    bne- bces_AtomicUtilImpl_PowerpcSpinLock_1 #loop if lost reservation

    isync
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x0000001c        # TB_OFFSET
# End of traceback table

    # int  bces_AtomicUtilImpl_PowerpcSpinTryLock(volatile char *aSpin,
    #                                             int retries );
#    .globl .bces_AtomicUtilImpl_PowerpcSpinTryLock
.bces_AtomicUtilImpl_PowerpcSpinTryLock:
    lwsync
bces_AtomicUtilImpl_PowerpcSpinTryLock_1:
    lwarx r5,0,r3    #load and reserve
    cmpwi r5,0       #compare the values
    bne+ bces_AtomicUtilImpl_PowerpcSpinTryLock_2
    stwcx. r3,0,r3   #Store the new value
    bne+ bces_AtomicUtilImpl_PowerpcSpinTryLock_2 #loop if
                              #lost reservation
    li r3, 0
    isync
    blr

bces_AtomicUtilImpl_PowerpcSpinTryLock_2:
    addi r4,r4,-1
    cmpwi r4,0
    bne- bces_AtomicUtilImpl_PowerpcSpinTryLock_1
    li r3, 255

    isync
    blr

    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x000000030        # TB_OFFSET
# End of traceback table

    #void bces_AtomicUtilImpl_PowerpcSpinUnlock(volatile char *aSpin);
    .globl .bces_AtomicUtilImpl_PowerpcSpinUnlock
.bces_AtomicUtilImpl_PowerpcSpinUnlock:
    lwsync
    li r4,0
    stw r4,0(r3)
    isync
    blr
    .long    0x00000000
# traceback table
    .byte    0x00            # VERSION=0
    .byte    0x09            # LANG=TB_CPLUSPLUS
    .byte    0x20            # IS_GL=0,IS_EPROL=0,HAS_TBOFF=1
                    # INT_PROC=0,HAS_CTL=0,TOCLESS=0
                    # FP_PRESENT=0,LOG_ABORT=0
    .byte    0x00            # INT_HNDL=0,NAME_PRESENT=0
                    # USES_ALLOCA=0,CL_DIS_INV=WALK_ONCOND
                    # SAVES_CR=0,SAVES_LR=0
    .byte    0x00            # STORES_BC=0,FPR_SAVED=0
    .byte    0x00            # GPR_SAVED=0
    .byte    0x00            # FIXEDPARMS=0
    .byte    0x00            # FLOATPARMS=0,PARMSONSTK=0
    .long    0x000000010        # TB_OFFSET
# End of traceback table

    .toc                            # 0x00000150
T.44.bces_AtomicUtilImpl_PowerpcSetInt64Relaxed:
    .tc    H.44.bces_AtomicUtilImpl_PowerpcSetInt64Relaxed{TC},bces_AtomicUtilImpl_PowerpcSetInt64Relaxed{DS}
T.48.bces_AtomicUtilImpl_PowerpcAddInt64Relaxed:
    .tc    H.48.bces_AtomicUtilImpl_PowerpcAddInt64Relaxed{TC},bces_AtomicUtilImpl_PowerpcAddInt64Relaxed{DS}
T.52.bces_AtomicUtilImpl_PowerpcGetInt64Relaxed:
    .tc    H.52.bces_AtomicUtilImpl_PowerpcGetInt64Relaxed{TC},bces_AtomicUtilImpl_PowerpcGetInt64Relaxed{DS}
T.56.bces_AtomicUtilImpl_PowerpcAddInt:
    .tc    H.56.bces_AtomicUtilImpl_PowerpcAddInt{TC},bces_AtomicUtilImpl_PowerpcAddInt{DS}
T.60.bces_AtomicUtilImpl_PowerpcTestAndSwap:
    .tc    H.60.bces_AtomicUtilImpl_PowerpcTestAndSwap{TC},bces_AtomicUtilImpl_PowerpcTestAndSwap{DS}
T.64.bces_AtomicUtilImpl_PowerpcSetInt64:
    .tc    H.64.bces_AtomicUtilImpl_PowerpcSetInt64{TC},bces_AtomicUtilImpl_PowerpcSetInt64{DS}
T.68.bces_AtomicUtilImpl_PowerpcSwapInt64:
    .tc    H.68.bces_AtomicUtilImpl_PowerpcSwapInt64{TC},bces_AtomicUtilImpl_PowerpcSwapInt64{DS}
T.72.bces_AtomicUtilImpl_PowerpcGetInt64:
    .tc    H.72.bces_AtomicUtilImpl_PowerpcGetInt64{TC},bces_AtomicUtilImpl_PowerpcGetInt64{DS}
T.76.bces_AtomicUtilImpl_PowerpcAddInt64:
    .tc    H.76.bces_AtomicUtilImpl_PowerpcAddInt64{TC},bces_AtomicUtilImpl_PowerpcAddInt64{DS}
T.80.bces_AtomicUtilImpl_PowerpcTestAndSwap64:
    .tc    H.80.bces_AtomicUtilImpl_PowerpcTestAndSwap64{TC},bces_AtomicUtilImpl_PowerpcTestAndSwap64{DS}
T.84.bces_AtomicUtilImpl_PowerpcSpinLock:
    .tc    H.84.bces_AtomicUtilImpl_PowerpcSpinLock{TC},bces_AtomicUtilImpl_PowerpcSpinLock{DS}
T.88.bces_AtomicUtilImpl_PowerpcSpinTryLock:
    .tc    H.88.bces_AtomicUtilImpl_PowerpcSpinTryLock{TC},bces_AtomicUtilImpl_PowerpcSpinTryLock{DS}
T.92.bces_AtomicUtilImpl_PowerpcSpinUnlock:
    .tc    H.92.bces_AtomicUtilImpl_PowerpcSpinUnlock{TC},bces_AtomicUtilImpl_PowerpcSpinUnlock{DS}



    .csect    bces_AtomicUtilImpl_PowerpcSetInt64Relaxed{DS}
    .long    .bces_AtomicUtilImpl_PowerpcSetInt64Relaxed# "\0\0\0\0"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcSetInt64Relaxed{DS}


    .csect    bces_AtomicUtilImpl_PowerpcAddInt64Relaxed{DS}
    .long    .bces_AtomicUtilImpl_PowerpcAddInt64Relaxed# "\0\0\0\024"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcAddInt64Relaxed{DS}


    .csect    bces_AtomicUtilImpl_PowerpcGetInt64Relaxed{DS}
    .long    .bces_AtomicUtilImpl_PowerpcGetInt64Relaxed# "\0\0\0,"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcGetInt64Relaxed{DS}


    .csect    bces_AtomicUtilImpl_PowerpcAddInt{DS}
    .long    .bces_AtomicUtilImpl_PowerpcAddInt# "\0\0\0D"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcAddInt{DS}


    .csect    bces_AtomicUtilImpl_PowerpcTestAndSwap{DS}
    .long    .bces_AtomicUtilImpl_PowerpcTestAndSwap# "\0\0\0\"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcTestAndSwap{DS}


    .csect    bces_AtomicUtilImpl_PowerpcSetInt64{DS}
    .long    .bces_AtomicUtilImpl_PowerpcSetInt64# "\0\0\0t"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcSetInt64{DS}


    .csect    bces_AtomicUtilImpl_PowerpcSwapInt64{DS}
    .long    .bces_AtomicUtilImpl_PowerpcSwapInt64# "\0\0\0\210"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcSwapInt64{DS}


    .csect    bces_AtomicUtilImpl_PowerpcGetInt64{DS}
    .long    .bces_AtomicUtilImpl_PowerpcGetInt64# "\0\0\0\244"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcGetInt64{DS}


    .csect    bces_AtomicUtilImpl_PowerpcAddInt64{DS}
    .long    .bces_AtomicUtilImpl_PowerpcAddInt64# "\0\0\0\300"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcAddInt64{DS}


    .csect    bces_AtomicUtilImpl_PowerpcTestAndSwap64{DS}
    .long    .bces_AtomicUtilImpl_PowerpcTestAndSwap64# "\0\0\0\334"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcTestAndSwap64{DS}


    .csect    bces_AtomicUtilImpl_PowerpcSpinLock{DS}
    .long    .bces_AtomicUtilImpl_PowerpcSpinLock# "\0\0\0\370"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcSpinLock{DS}


    .csect    bces_AtomicUtilImpl_PowerpcSpinTryLock{DS}
    .long    .bces_AtomicUtilImpl_PowerpcSpinTryLock# "\0\0\001\f"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcSpinTryLock{DS}


    .csect    bces_AtomicUtilImpl_PowerpcSpinUnlock{DS}
    .long    .bces_AtomicUtilImpl_PowerpcSpinUnlock# "\0\0\001$"
    .long    TOC{TC0}                # "\0\0\001P"
    .long    0x00000000              # "\0\0\0\0"
# End    csect    bces_AtomicUtilImpl_PowerpcSpinUnlock{DS}



# End    csect    main{DS}
