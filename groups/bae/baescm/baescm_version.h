// baescm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BAESCM_VERSION
#define INCLUDED_BAESCM_VERSION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
//  baescm_Version: namespace for RCS and SCCS versioning information for 'bae'
//
//@AUTHOR: Jeffrey Mendelsohn (jmendels)
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bae' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bae' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'baescm_Version' struct can be used to query version
// information for the 'bae' package group at runtime.  The following USAGE
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// "#include" this component header file to get 'bae' version information
// embedded in an executable.  It is only necessary to use one or more 'bae'
// components (and, hence, link in the 'bae' library).
//
///USAGE 1
///-------
// The version of the 'bae' package group linked into a program can be
// obtained at runtime using the 'version' 'static' member function as follows:
//..
//        #include <baescm_version.h>
//
//        assert(0 != baescm_Version::version());
//
//        bsl::cout << "BAE version: " << baescm_Version::version()
//                  << bsl::endl;
//..
// Output similar to the following will be printed to 'stdout':
//..
//        BAE version: BLP_LIB_BAE_BAE_0.01.0
//..
// The "0.01.0" portion of the string distinguishes different versions of the
// 'bae' package group.
//
///USAGE 2
///-------
// The versioning information embedded into a binary file by this component can
// be examined under UNIX using several well-known utilities.  For example:
//..
//        $ ident a.out
//        a.out:
//             $Id: BLP_LIB_BAE_BAE_0.01.0 $
//
//        $ what a.out | grep BAE
//                BLP_LIB_BAE_BAE_0.01.0
//
//        $ strings a.out | grep BAE
//        $Id: BLP_LIB_BAE_BAE_0.01.0 $
//        @(#)BLP_LIB_BAE_BAE_0.01.0
//        BLP_LIB_BAE_BAE_0.01.0
//..
// Note that 'ident' and 'what' typically will display many version strings
// unrelated to 'bae' depending on the libraries used by 'a.out'.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

struct baescm_Version {
    // This struct provides a namespace for (1) source control management
    // (versioning) information that is embedded in binary executable files,
    // and (2) a facility to query that information at runtime.

    static const char *d_ident;              // RCS-style version string
    static const char *d_what;               // SCCS-style version string

    static const char *d_version;            // BDE-style version string
    static const char *d_dependencies;       // available for future use
    static const char *d_buildInfo;          // available for future use
    static const char *d_timestamp;          // available for future use
    static const char *d_sourceControlInfo;  // available for future use

    // CLASS METHODS
    static const char *version();
        // Return the address of a character string that identifies the version
        // of the 'bae' package group in use.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CLASS METHODS
inline
const char *baescm_Version::version()
{
    return d_version;
}

// Force linker to pull in this component's object file.

#if defined(BSLS_PLATFORM_CMP_IBM)
static const char **baescm_version_assertion = &baescm_Version::d_version;
#else
namespace {
    extern const char **const baescm_version_assertion =
                                                    &baescm_Version::d_version;
}
#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
