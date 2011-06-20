// bcemt_default.h                                                    -*-C++-*-
#ifndef INCLUDED_BCEMT_DEFAULT
#define INCLUDED_BCEMT_DEFAULT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide default values for BCE thread attributes
//
//@CLASSES:
//  bcemt_Default: default values for BCE thread attributes
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@SEE_ALSO: bcemt_threadattributes, bcemt_threadutil
//
//@DESCRIPTION: This component provides default values for BCE-relevant
// parameters.  It currently configures thread stack size, but may be expanded
// to configure other items.
//
// The thread stack size is defined such that a thread spawned with a given
// 'stackSize' will be able to declare a variable of size
// 'stackSize - overhead' in bytes in the top level routine called by the
// thread, where 'overhead' is a small constant value.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1:
/// - - - - -
// The default thread stack size defaults to the native default
//..
// First, we examine the native thread stack size:
//..
//  const int native = bcemt_Default::nativeDefaultThreadStackSize();
//  ASSERT(native > 0);
//..
// Then, we set 'setSize' to some size other than the native size:
//..
//  int setSize = 1 << 18;
//  if (native == setSize) {
//      setSize *= 2;
//  }
//..
// Next, we verify that when 'defaultThreadStackSize' is called, it returns the
// native size:
//..
//  ASSERT(bcemt_Default::defaultThreadStackSize() != setSize);
//  ASSERT(bcemt_Default::defaultThreadStackSize() == native);
//..
// Then, we set the default size to a size other than the native size.
//..
//  bcemt_Default::setDefaultThreadStackSize(setSize);
//..
// Finally, we verify that the default thread stack size has been set to the
// value we specified:
//..
//  ASSERT(bcemt_Default::defaultThreadStackSize() == setSize);
//  ASSERT(bcemt_Default::defaultThreadStackSize() != native);
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

    static int recommendedDefaultThreadStackSize();
        // Return a reasonable value for the default thread stack size (in
        // bytes), which, unlike 'nativeDefaultThreadStackSize', is constant
        // across all platforms of a given word size.  This value is large
        // enough to guarantee that an automatic array of at least 250 * 1024
        // pointers may be declared in the top level routine of the thread.

    static void setDefaultThreadStackSize(int stackSize);
        // Set the default thread stack size to the specified 'stackSize' (in
        // bytes).  If 'stackSize' is below the minimum that the underlying
        // threads implementation requires for a thread, it will be rounded up
        // to that minimum.  The behavior is undefined unless there is exactly
        // one thread of control in the current process, and unless
        // 'stackSize > 0'.
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
