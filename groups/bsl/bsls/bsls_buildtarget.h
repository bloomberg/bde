// bsls_buildtarget.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLS_BUILDTARGET
#define INCLUDED_BSLS_BUILDTARGET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide build-target information in the object file.
//
//@CLASSES:
// bsls_ExcBuildTarget: type name for identifying exception builds
//  bsls_MtBuildTarget: type name for identifying multi-threaded builds
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: The purpose of this component is to cause a link-time error
// when trying to link an executable with incompatible libraries.  This
// component defines type names that indicate the build target parameters.
// These parameters include whether this build was exception-enabled (which is
// the case unless overridden by defining the 'BDE_BUILD_TARGET_NO_EXC' macro),
// and whether it was multi-threaded or not (it is enabled unless overridden by
// defining the 'BDE_BUILD_TARGET_NO_MT' macro).  The types defined by this
// component should not be used directly.

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

// Default to an exception-enabled build unless 'BDE_BUILD_TARGET_NO_EXC' is
// defined.
#ifndef BDE_BUILD_TARGET_NO_EXC

#ifndef BDE_BUILD_TARGET_EXC
#define BDE_BUILD_TARGET_EXC
#endif

struct bsls_YesExcBuildTarget {
    static const int d_isExcBuildTarget;
};
typedef bsls_YesExcBuildTarget bsls_ExcBuildTarget;

#else

#ifdef BDE_BUILD_TARGET_EXC
#error Do not define both BDE_BUILD_TARGET_EXC and BDE_BUILD_TARGET_NO_EXC
#endif

struct bsls_NoExcBuildTarget {
    static const int d_isExcBuildTarget;
};
typedef bsls_NoExcBuildTarget bsls_ExcBuildTarget;

#endif

// Default to a threaded (MT) build unless 'BDE_BUILD_TARGET_NO_MT' is defined.
#ifndef BDE_BUILD_TARGET_NO_MT

#ifndef BDE_BUILD_TARGET_MT
#define BDE_BUILD_TARGET_MT
#endif

struct bsls_YesMtBuildTarget {
    static const int d_isMtBuildTarget;
};
typedef bsls_YesMtBuildTarget bsls_MtBuildTarget;

#else

#ifdef BDE_BUILD_TARGET_MT
#error Do not define both BDE_BUILD_TARGET_MT and BDE_BUILD_TARGET_NO_MT
#endif

struct bsls_NoMtBuildTarget {
    static const int d_isMtBuildTarget;
};
typedef bsls_NoMtBuildTarget bsls_MtBuildTarget;

#endif

#ifdef BSLS_PLATFORM__CPU_64_BIT

struct bsls_Yes64BitBuildTarget {
    static const int d_is64BitBuildTarget;
};
typedef bsls_Yes64BitBuildTarget bsls_64BitBuildTarget;

#else

struct bsls_No64BitBuildTarget {
    static const int d_is64BitBuildTarget;
};
typedef bsls_No64BitBuildTarget bsls_64BitBuildTarget;

#endif

// Force linker to pull in this component's object file.

#if defined(BSLS_PLATFORM__CMP_IBM)
static const int *bsls_buildtarget_assertion1 =
                                      &bsls_ExcBuildTarget::d_isExcBuildTarget;
static const int *bsls_buildtarget_assertion2 =
                                        &bsls_MtBuildTarget::d_isMtBuildTarget;
#else
namespace {
    extern const int *const bsls_buildtarget_assertion1 =
                                      &bsls_ExcBuildTarget::d_isExcBuildTarget;
    extern const int *const bsls_buildtarget_assertion2 =
                                        &bsls_MtBuildTarget::d_isMtBuildTarget;
}
#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
