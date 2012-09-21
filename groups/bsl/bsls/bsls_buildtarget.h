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
//  bsls::ExcBuildTarget: type name for identifying exception builds
//  bsls::MtBuildTarget: type name for identifying multi-threaded builds
//
//@MACROS:
//  BDE_BUILD_TARGET_EXC: flag for exception-enabled builds
//  BDE_BUILD_TARGET_MT:  flag for multi-threaded builds
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

namespace bsls {

struct YesExcBuildTarget {
    static const int d_isExcBuildTarget;
};
typedef YesExcBuildTarget ExcBuildTarget;

}  // close package namespace

typedef bsls::YesExcBuildTarget bsls_YesExcBuildTarget;
    // This alias is defined for backward compatibility.

#else

#ifdef BDE_BUILD_TARGET_EXC
#error Do not define both BDE_BUILD_TARGET_EXC and BDE_BUILD_TARGET_NO_EXC
#endif

namespace bsls {

struct NoExcBuildTarget {
    static const int d_isExcBuildTarget;
};
typedef NoExcBuildTarget ExcBuildTarget;

}  // close package namespace

typedef bsls::NoExcBuildTarget bsls_NoExcBuildTarget;
    // This alias is defined for backward compatibility.

#endif

// Default to a threaded (MT) build unless 'BDE_BUILD_TARGET_NO_MT' is defined.
#ifndef BDE_BUILD_TARGET_NO_MT

#ifndef BDE_BUILD_TARGET_MT
#define BDE_BUILD_TARGET_MT
#endif

namespace bsls {

struct YesMtBuildTarget {
    static const int d_isMtBuildTarget;
};
typedef YesMtBuildTarget MtBuildTarget;

}  // close package namespace

typedef bsls::YesMtBuildTarget bsls_YesMtBuildTarget;
    // This alias is defined for backward compatibility.

#else

#ifdef BDE_BUILD_TARGET_MT
#error Do not define both BDE_BUILD_TARGET_MT and BDE_BUILD_TARGET_NO_MT
#endif

namespace bsls {

struct NoMtBuildTarget {
    static const int d_isMtBuildTarget;
};
typedef NoMtBuildTarget MtBuildTarget;

}  // close package namespace

typedef bsls::NoMtBuildTarget bsls_NoMtBuildTarget;
    // This alias is defined for backward compatibility.

#endif

#ifdef BSLS_PLATFORM_CPU_64_BIT

namespace bsls {

struct Yes64BitBuildTarget {
    static const int d_is64BitBuildTarget;
};

}  // close package namespace

typedef bsls::Yes64BitBuildTarget bsls_64BitBuildTarget;

typedef bsls::Yes64BitBuildTarget bsls_Yes64BitBuildTarget;
    // This alias is defined for backward compatibility.

#else

namespace bsls {

struct No64BitBuildTarget {
    static const int d_is64BitBuildTarget;
};

}  // close package namespace

typedef bsls::No64BitBuildTarget bsls_64BitBuildTarget;

typedef bsls::No64BitBuildTarget bsls_No64BitBuildTarget;
    // This alias is defined for backward compatibility.

#endif

// Force linker to pull in this component's object file.

#if defined(BSLS_PLATFORM_CMP_IBM)
static const int *bsls_buildtarget_assertion1 =
                                     &bsls::ExcBuildTarget::d_isExcBuildTarget;
static const int *bsls_buildtarget_assertion2 =
                                       &bsls::MtBuildTarget::d_isMtBuildTarget;
#else
namespace {
    extern const int *const bsls_buildtarget_assertion1 =
                                     &bsls::ExcBuildTarget::d_isExcBuildTarget;
    extern const int *const bsls_buildtarget_assertion2 =
                                       &bsls::MtBuildTarget::d_isMtBuildTarget;
}
#endif

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bsls_ExcBuildTarget
#undef bsls_ExcBuildTarget
#endif
#define bsls_ExcBuildTarget bsls::ExcBuildTarget
    // This alias is defined for backward compatibility.

#ifdef bsls_MtBuildTarget
#undef bsls_MtBuildTarget
#endif
#define bsls_MtBuildTarget bsls::MtBuildTarget
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
