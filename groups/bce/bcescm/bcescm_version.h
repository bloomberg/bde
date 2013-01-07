// bcescm_version.h                 -*-C++-*-
#ifndef INCLUDED_BCESCM_VERSION
#define INCLUDED_BCESCM_VERSION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
//  bcescm_Version: namespace for RCS and SCCS versioning information for 'bce'
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bce' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bce' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'bcescm_Version' struct can be used to query version
// information for the 'bce' package group at runtime.
//
// Note that unless the 'version' method will be called, it is not necessary to
// "#include" this component header file to get 'bce' version information
// embedded in an executable.  It is only necessary to use one or more 'bce'
// components (and, hence, link in the 'bce' library).

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BCESCM_VERSIONTAG
#include <bcescm_versiontag.h>
#endif

#ifndef INCLUDED_BDESCM_VERSIONTAG
#include <bdescm_versiontag.h> // BDE_VERSION_RELEASETYPE
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif


namespace BloombergLP {

struct bcescm_Version {
    static const char *d_ident;
    static const char *d_what;
    static const char *d_version;
    static const char *d_dependencies;
    static const char *d_buildInfo;
    static const char *d_timestamp;
    static const char *d_sourceControlInfo;

    static const char *version();
};

inline
const char *bcescm_Version::version()
{
    return d_version;
}

// Force linker to pull in this component's object file.

#if defined(BSLS_PLATFORM_CMP_IBM)
static const char **bcescm_version_assertion =
                                       &bcescm_Version::d_version;
#else
namespace {
    extern const char **const bcescm_version_assertion =
                                       &bcescm_Version::d_version;
}
#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
