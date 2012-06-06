// bsls_buildtarget.cpp                                               -*-C++-*-
#include <bsls_buildtarget.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

extern const int BSLS_BUILDTARGET_IS_UNIQUE = 1;

#ifdef BDE_BUILD_TARGET_EXC

const int bsls::YesExcBuildTarget::d_isExcBuildTarget = 1;

#else

const int bsls::NoExcBuildTarget::d_isExcBuildTarget = 0;

#endif

#ifdef BDE_BUILD_TARGET_MT

const int bsls::YesMtBuildTarget::d_isMtBuildTarget = 1;

#else

const int bsls::NoMtBuildTarget::d_isMtBuildTarget = 0;

#endif

#ifdef BSLS_PLATFORM__CPU_64_BIT

const int bsls::Yes64BitBuildTarget::d_is64BitBuildTarget = 1;

#else

const int bsls::No64BitBuildTarget::d_is64BitBuildTarget = 0;

#endif

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
