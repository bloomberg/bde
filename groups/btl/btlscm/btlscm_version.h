// btlscm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BTLSCM_VERSION
#define INCLUDED_BTLSCM_VERSION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
//  btlscm::Version: namespace for RCS and SCCS versioning info for 'btl'
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'btl' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'btl' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'btlscm::Version' struct can be used to query version
// information for the 'btl' package group at runtime.  The following USAGE
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// "#include" this component header file to get 'btl' version information
// embedded in an executable.  It is only necessary to use one or more 'btl'
// components (and, hence, link in the 'btl' library).
//
///Usage
///-----
// A program can display the version of BTL that was used to build it by
// printing the version string returned by 'btlscm::Version::version()' to
// 'stdout' as follows:
//..
//  bsl::printf("BTL version: %s\n", btlscm::Version::version());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_LINKCOERCION
#include <bsls_linkcoercion.h>
#endif

namespace BloombergLP {

namespace btlscm {

struct Version {
    static const char *s_ident;
    static const char *s_what;

    static const char *s_version;
    static const char *s_dependencies;
    static const char *s_buildInfo;
    static const char *s_timestamp;
    static const char *s_sourceControlInfo;

    static const char *version();
};

inline
const char *Version::version()
{
    return s_version;
}

}  // close package namespace

// Force linker to pull in this component's object file.

BSLS_LINKCOERCION_FORCE_SYMBOL_DEPENDENCY(const char *,
                                          btlscm_version_assertion,
                                          btlscm::Version::s_version);

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2015
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
