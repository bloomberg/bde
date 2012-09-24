// bdef_bind_test.cpp               -*-C++-*-
#include <bdef_bind_test.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdef_bind_test_cpp,"$Id$ $CSID$")

#include <bdef_placeholder.h>    // for testing only

namespace BloombergLP {

                      // ------------------------------
                      // class bdef_Bind_TestSlotsAlloc
                      // ------------------------------

// CLASS METHODS
bool bdef_Bind_TestSlotsAlloc::verifySlots(
        const bslma_Allocator* const *EXPECTED,
        bool                          verboseFlag)
{
    bool equalFlag = true;
#if !defined(BSLS_PLATFORM_CMP_MSVC)
    // Note: the calls to 'verifyAllocSlots' are disabled on Windows.
    // Their success depends on the "Return Value Optimization" (RVO)
    // which Windows does not seem to be applying.

    for (int i = 0; i < NUM_SLOTS; ++i) {
        if (EXPECTED[i] != getSlot(i)) {
            equalFlag = false;
            break;
        }
    }
#endif // !defined(BSLS_PLATFORM_CMP_MSVC)

    if (verboseFlag || !equalFlag) {
        bsl::printf("\tAllocSlots:");
        for (int i = 0; i < NUM_SLOTS; ++i) {
            bsl::printf(" %s", (getSlot(i) == getZ0()) ? "Z0"
                            : ((getSlot(i) == getZ1()) ? "Z1"
                            : ((getSlot(i) == getZ2()) ? "Z2"
                            : "Z?"))); 
        }
        bsl::printf("\n");
    }

    return equalFlag;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
