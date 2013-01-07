// bdescm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BDESCM_VERSION
#define INCLUDED_BDESCM_VERSION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
//  bdescm_Version: namespace for RCS and SCCS versioning information for 'bde'
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bde' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bde' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'bdescm_Version' 'struct' can be used to query version
// information for the 'bde' package group at runtime.  The following usage
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// '#include' this component header file to get 'bde' version information
// embedded in an executable.  It is only necessary to use one or more 'bde'
// components (and, hence, link in the 'bde' library).
//
///Usage
///-----
// A program can display the version of BDE that was used to build it by
// printing the version string returned by 'bdescm_Version::version()' to
// 'stdout' as follows:
//..
//  bsl::printf("BDE version: %s\n", bdescm_Version::version());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BDESCM_VERSIONTAG
#include <bdescm_versiontag.h>     // 'BDE_VERSION_MAJOR', 'BDE_VERSION_MINOR'
#endif

#ifndef INCLUDED_BSLS_BUILDTARGET
#include <bsls_buildtarget.h>      // need to ensure consistent build options
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif


namespace BloombergLP {

struct bdescm_Version {
    static const char *d_ident;
    static const char *d_what;

#define BDESCM_CONCAT2(a,b,c,d,e,f) a ## b ## c ## d ## e ## f
#define BDESCM_CONCAT(a,b,c,d,e,f)  BDESCM_CONCAT2(a,b,c,d,e,f)

// 'BDESCM_D_VERSION' is a symbol whose name warns users of version mismatch
// linking errors.  Note that the exact string "compiled_this_object" must be
// present in this version coercion symbol.  Tools may look for this pattern to
// warn users of mismatches.
#define BDESCM_D_VERSION BDESCM_CONCAT(                \
                       d_version_BDE_,                 \
                       BDE_VERSION_MAJOR, _,           \
                       BDE_VERSION_MINOR,              \
                       BDE_VERSION_RELEASETYPE_SYMBOL, \
                       _compiled_this_object)

    static const char *BDESCM_D_VERSION;

    static const char *d_dependencies;
    static const char *d_buildInfo;
    static const char *d_timestamp;
    static const char *d_sourceControlInfo;

    static const char *version();
};

inline
const char *bdescm_Version::version()
{
    return BDESCM_D_VERSION;
}

// Force linker to pull in this component's object file.

#if defined(BSLS_PLATFORM_CMP_IBM)
static const char **bdescm_version_assertion =
                                             &bdescm_Version::BDESCM_D_VERSION;
#else
namespace {
    extern const char **const bdescm_version_assertion =
                                             &bdescm_Version::BDESCM_D_VERSION;
}
#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
