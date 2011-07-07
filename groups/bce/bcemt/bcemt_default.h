// bcemt_default.h                                                    -*-C++-*-
#ifndef INCLUDED_BCEMT_DEFAULT
#define INCLUDED_BCEMT_DEFAULT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide configurable default values for BCE
//
//@CLASSES:
//  bcemt_Default: configurable default values for BCE
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@SEE_ALSO: bcemt_threadattributes, bcemt_threadutil
//
//@DESCRIPTION: This component provides configurable default values for
// BCE-relevant parameters.  It currently configures thread stack size, but may
// be expanded to configure other items.
//
// The property 'stackSize' is defined such that a thread spawned with a given
// 'stackSize' will be able to declare a buffer of size
// 'stackSize - OVERHEAD' bytes in the top level function call of the created
// thread, where 'OVERHEAD' is a small integer constant value.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Demonstrate accessing the default thread stack size:
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// First, we examine the native thread stack size:
//..
//  const int nativeDefault = bcemt_Default::nativeDefaultThreadStackSize();
//
//  ASSERT(nativeDefault > 0);
//..
// Then, we verify that when 'defaultThreadStackSize' is called, it returns the
// native size:
//..
//  ASSERT(bcemt_Default::defaultThreadStackSize() == nativeDefault);
//..
// Next, we define 'newDefaultStackSize' to some size other than the native
// default size:
//..
//  const int newDefaultStackSize = nativeDefault * 2;
//..
// Now, we set the default size to the new size:
//..
//  bcemt_Default::setDefaultThreadStackSize(newDefaultStackSize);
//..
// Finally, we verify that the default thread stack size has been set to the
// value we specified:
//..
//  ASSERT(bcemt_Default::defaultThreadStackSize() == newDefaultStackSize);
//  ASSERT(bcemt_Default::defaultThreadStackSize() != nativeDefault);
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

namespace BloombergLP {

                            // ====================
                            // struct bcemt_Default
                            // ====================

struct bcemt_Default {
    // This 'struct' provides a namespace for a suite of functions that are
    // used to manage configuration of default values used in 'bce'.
    // Specifically, these functions manage the default value of thread stack
    // size, but may be extended to govern more traits in the future.

    // CLASS METHODS
    static int defaultThreadStackSize();
        // Return the current default thread stack size (in bytes).  If
        // 'setDefaultThreadStackSize' has never been called, this value will
        // be the same value returned by 'nativeDefaultThreadStackSize',
        // otherwise it will be the value set by the last call to
        // 'setDefaultThreadStackSize'.

    static int nativeDefaultThreadStackSize();
        // Return the default thread stack size (in bytes) determined by the
        // underlying platform.  Note that this value reflects semantics, and
        // may be influenced by the choice of platform, environment variables,
        // compiler/linker options, or shell configuration, and typically
        // varies wildly among different platforms.

    static int nativeDefaultThreadGuardSize();
        // Return the default thread stack guard size (in bytes) determined by
        // the underlying platfrom.  Note that this value reflects semantics,
        // and may be influenced by the choice of platform, environment
        // variables, compiler/linker options, or shell configuration, and may
        // vary somewhat among different platforms.

    static int recommendedDefaultThreadStackSize();
        // Return a reasonable value for the default thread stack size (in
        // bytes), which, unlike 'nativeDefaultThreadStackSize', is constant
        // across all platforms of a given word size.  This value is large
        // enough to guarantee that an automatic array of at least 250 * 1024
        // pointers may be declared in the top level routine of the thread.

    static void setDefaultThreadStackSize(int stackSize);
        // Set the default thread stack size to the specified 'stackSize' (in
        // bytes).  If a minimum thread stack size is known for the underlying
        // platform (ie 'PTHREAD_STACK_MIN' is defined) and 'stackSize' is
        // below that minimum, it will be rounded up to that minimum.  The
        // behavior is undefined unless 'stackSize > 0'.
};

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
