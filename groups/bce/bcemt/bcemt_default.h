// bcemt_default.h                                                    -*-C++-*-
#ifndef INCLUDED_BCEMT_DEFAULT
#define INCLUDED_BCEMT_DEFAULT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide utilities to set and get default values for BCE.
//
//@CLASSES:
//  bcemt_Default: namespace for utilities managing default BCE values
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@SEE_ALSO: bcemt_threadattributes, bcemt_threadutil
//
//@DESCRIPTION: This component defines a utility 'struct', 'bcemt_Default',
// that is a name space for pure functions used for providing access to, and
// configuring, default values for BCE-relevant parameters.  The
// 'bcemt_Default' utility currently provides static methods to access and
// modify the BCE library's default stack size, as well as functions that
// access the platform's native default stack size and guard size.  The BCE
// default stack size is initially configured to be the platform's native stack
// size.
//
// The stack-size values accessed and managed by this component indicate that a
// stack created with a given size will be able to declare a buffer of that
// size (in bytes) in the thread entry function of the created thread (see
// 'bcemt_threadutil').
//
///Usage
///-----
// This section illustrates the intended usage of this component.
//
///Example 1: Demonstrate Accessing & Modifying the Default Thread Stack Size
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to access both the platform's native and
// BCE configured default stack sizes, and then to set the default stack size
// used by BCE.  Note that the value returned by 'defaultThreadStackSize' may
// be adjusted from that provided by the underlying operating system to reflect
// the actual amount of stack memory available to a created thread.  For
// example, on Itanium platforms (HPUX) the value will be scaled down from the
// operating system supplied value to account for the extra stack space devoted
// to storing registers.  Note that operations creating a thread should perform
// a similar inverse adjustment when configuring the new thread's stack size
// (see 'bcemt_threadutil').
//
// First, we examine the platform's native thread stack size:
//..
//  const int nativeDefault = bcemt_Default::nativeDefaultThreadStackSize();
//
//  assert(nativeDefault > 0);
//..
// Then, we verify that, when 'defaultThreadStackSize' is called, it returns
// the native size:
//..
//  assert(nativeDefault == bcemt_Default::defaultThreadStackSize());
//..
// Next, we define 'newDefaultStackSize' to some size other than the platform's
// native default stack size:
//..
//  const int newDefaultStackSize = nativeDefault * 2;
//..
// Now, we set the default size for BCE to the new size:
//..
//  bcemt_Default::setDefaultThreadStackSize(newDefaultStackSize);
//..
// Finally, we verify that BCE's default thread stack size has been set to the
// value we specified:
//..
//  assert(bcemt_Default::defaultThreadStackSize() == newDefaultStackSize);
//  assert(bcemt_Default::defaultThreadStackSize() != nativeDefault);
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
    // used to manage the configuration of default values used in 'bce'.
    // Specifically, these functions manage the default value of thread stack
    // size and provide access to the platform's native guard size, but may be
    // extended to govern more traits in the future.

    // CLASS METHODS
    static int defaultThreadStackSize();
        // Return the current default thread stack size (in bytes).  If
        // 'setDefaultThreadStackSize' has never been called, this value will
        // be the same value returned by 'nativeDefaultThreadStackSize',
        // otherwise it will be the value set by the last call to
        // 'setDefaultThreadStackSize'.

    static int nativeDefaultThreadStackSize();
        // Return the "native" default thread stack size (in bytes) as
        // determined by the underlying platform.  Note that this value may be
        // influenced by the choice of platform, environment variables,
        // compiler/linker options, or shell configuration, and typically
        // varies wildly among different platforms.

    static int nativeDefaultThreadGuardSize();
        // Return the default thread stack guard size (in bytes) determined by
        // the underlying platfrom.  Note that this value reflects semantics,
        // and may be influenced by the choice of platform, environment
        // variables, compiler/linker options, or shell configuration, and may
        // vary somewhat among different platforms.

    static int recommendedDefaultThreadStackSize();
        // Return a "reasonable" value for the default thread stack size (in
        // bytes), which, unlike 'nativeDefaultThreadStackSize', is constant
        // across all platforms of a given word size.  This value is large
        // enough to guarantee that an automatic array of at least 250 * 1024
        // pointers may be declared in the top level routine of the thread.

    static void setDefaultThreadStackSize(int numBytes);
        // Set the default thread stack size to the specified 'numBytes'.  If a
        // minimum thread stack size is known for the underlying platform (i.e.
        // 'PTHREAD_STACK_MIN' is defined) and 'numBytes' is below that
        // minimum, the stack size will be that minimum.  The behavior is
        // undefined unless '0 <= numBytes'.
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
