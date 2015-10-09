// bsls_buildtarget.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLS_BUILDTARGET
#define INCLUDED_BSLS_BUILDTARGET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide build-target information in object files.
//
//@MACROS:
//  BDE_BUILD_TARGET_EXC: flag identifying exception-enabled builds
//  BDE_BUILD_TARGET_NO_EXC: flag identifying exception-disabled builds
//  BDE_BUILD_TARGET_MT: flag identifying multi-threaded builds
//  BDE_BUILD_TARGET_NO_MT: flag identifying builds that do not support threads
//  BDE_OMIT_DEPRECATED: flag for omitting deprecated code from BDE source
//
//@DESCRIPTION: The purpose of this component is to cause a link-time error
// when trying to link an executable with incompatible libraries.  This
// component defines type names that indicate two build target parameters.
// These parameters determine whether the build was exception-enabled (which is
// the case unless overridden by defining the 'BDE_BUILD_TARGET_NO_EXC' macro),
// and whether it was multi-threaded (which is enabled unless overridden by
// defining the 'BDE_BUILD_TARGET_NO_MT' macro).  The types defined by this
// component should not be used directly.
//
///Deprecation Control Macros
///--------------------------
// In addition to the 'BDE_BUILD_TARGET_*' macros that determine the link-time
// compatibility of different libraries built on BDE, this component documents
// a macro that determines whether deprecated interfaces are available to
// programs built on BDE:
//
//: 'BDE_OMIT_DEPRECATED':
//:     This macro, if defined, indicates that all deprecated code is excluded
//:     from a build of the library.
//
//
///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

#ifndef INCLUDED_BSLS_LINKCOERCION
#include <bsls_linkcoercion.h>
#endif

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

struct BuildTargetExcYes {
    static const int s_isBuildTargetExc;
};
typedef BuildTargetExcYes BuildTargetExc;

}  // close package namespace

#else

#ifdef BDE_BUILD_TARGET_EXC
#error Do not define both BDE_BUILD_TARGET_EXC and BDE_BUILD_TARGET_NO_EXC
#endif

namespace bsls {

struct BuildTargetExcNo {
    static const int s_isBuildTargetExc;
};
typedef BuildTargetExcNo BuildTargetExc;

}  // close package namespace

#endif

// Default to a threaded (MT) build unless 'BDE_BUILD_TARGET_NO_MT' is defined.

#ifndef BDE_BUILD_TARGET_NO_MT

#ifndef BDE_BUILD_TARGET_MT
#define BDE_BUILD_TARGET_MT
#endif

namespace bsls {

struct BuildTargetMtYes {
    static const int s_isBuildTargetMt;
};
typedef BuildTargetMtYes BuildTargetMt;

}  // close package namespace

#else

#ifdef BDE_BUILD_TARGET_MT
#error Do not define both BDE_BUILD_TARGET_MT and BDE_BUILD_TARGET_NO_MT
#endif

namespace bsls {

struct BuildTargetMtNo {
    static const int s_isBuildTargetMt;
};
typedef BuildTargetMtNo BuildTargetMt;

}  // close package namespace

#endif

// Force linker to pull in this component's object file.

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(
                                      const int,
                                      bsls_buildtarget_assertion1,
                                      bsls::BuildTargetExc::s_isBuildTargetExc)

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(
                                        const int,
                                        bsls_buildtarget_assertion2,
                                        bsls::BuildTargetMt::s_isBuildTargetMt)

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
