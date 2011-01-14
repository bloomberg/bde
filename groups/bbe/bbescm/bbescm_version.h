// bbescm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BBESCM_VERSION
#define INCLUDED_BBESCM_VERSION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
//  bbescm_Version: namespace for RCS and SCCS versioning information for 'bbe'
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bbe' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bbe' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'bbescm_Version' struct can be used to query version
// information for the 'bbe' package group at runtime.  The following USAGE
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// "#include" this component header file to get 'bbe' version information
// embedded in an executable.  It is only necessary to use one or more 'bbe'
// components (and, hence, link in the 'bbe' library).

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

struct bbescm_Version {
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
const char *bbescm_Version::version()
{
    return d_version;
}

// Force linker to pull in this component's object file.

#if defined(BSLS_PLATFORM__CMP_IBM)
static const char **bbescm_version_assertion = &bbescm_Version::d_version;
#else
namespace {
    extern const char **const bbescm_version_assertion =
                                                    &bbescm_Version::d_version;
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
