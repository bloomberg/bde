// baesu_stackaddressutil.h                                           -*-C++-*-
#ifndef INCLUDED_BAESU_STACKADDRESSUTIL
#define INCLUDED_BAESU_STACKADDRESSUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a utility for obtaining return addresses from the stack
//
//@CLASSES:
//   baesu_StackAddressUtil: utilities for obtaining addresses from the stack
//
//@SEE_ALSO: baesu_stacktrace, baesu_stacktraceframe
//
//@AUTHOR: Oleg Semenov, Bill Chapman
//
//@DESCRIPTION: This component provides namespace containing a function that
// populates an array with an ordered sequence of return addresses from the
// current thread's function call stack.  Each return address points to the
// (text) memory location of the first instruction to be executed upon
// returning from a called routine.

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

                        // ============================
                        // class baesu_StackAddressUtil
                        // ============================

namespace BloombergLP {

struct baesu_StackAddressUtil {
    // This struct provides a namespace for the function to obtain return
    // addresses from the stack.

    // On some platforms, 'getStackAddresses' finds a frame representing
    // 'getStackAddresses' itself.  This frame is usually unwanted.
    // 'BAESU_IGNORE_FRAMES' instructs the caller as to whether the first frame
    // is such an unwanted frame.

#if defined(BSLS_PLATFORM__OS_LINUX) || defined(BSLS_PLATFORM__OS_WINDOWS)
    enum { BAESU_IGNORE_FRAMES = 1 };
#else
    enum { BAESU_IGNORE_FRAMES = 0 };
#endif

    // CLASS METHODS
    static
    int getStackAddresses(void   **buffer,
                          int      maxFrames);
        // Get an ordered sequence of return addresses from the current
        // thread's function call stack and load them into the specified array
        // 'buffer', which is at least the specified 'maxFrames' in length.  A
        // return address is an address stored on the stack that points to the
        // first instruction that will be executed after the called subroutine
        // returns.  If there are more than 'maxFrames' frames on the stack,
        // only the return addresses for the 'maxFrames' most recent routine
        // calls are stored.  When this routine completes, 'buffer' will
        // contain an ordered sequence of return addresses, sorted such that
        // recent calls occur in the array before calls which took place before
        // them.  Return the number of stack frames stored into 'buffer' on
        // success, and a negative value otherwise.  The behavior is undefined
        // unless 'buffer' has room for at least 'maxFrames' addresses.
};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
