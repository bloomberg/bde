// bdlscm_version.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLSCM_VERSION
#define INCLUDED_BDLSCM_VERSION

#ifndef INCLUDED_BDLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide source control management (versioning) information.
//
//@CLASSES:
// bdlscm::Version: namespace for RCS and SCCS versioning information for 'bdl'
//
//@DESCRIPTION: This component provides source control management (versioning)
// information for the 'bdl' package group.  In particular, this component
// embeds RCS-style and SCCS-style version strings in binary executable files
// that use one or more components from the 'bdl' package group.  This version
// information may be extracted from binary files using common UNIX utilities
// (e.g., 'ident' and 'what').  In addition, the 'version' 'static' member
// function in the 'bdlscm::Version' 'struct' can be used to query version
// information for the 'bdl' package group at runtime.  The following usage
// examples illustrate these two basic capabilities.
//
// Note that unless the 'version' method will be called, it is not necessary to
// '#include' this component header file to get 'bdl' version information
// embedded in an executable.  It is only necessary to use one or more 'bdl'
// components (and, hence, link in the 'bdl' library).
//
///Usage
///-----
// A program can display the version of BDL that was used to build it by
// printing the version string returned by 'bdlscm::Version::version()' to
// 'stdout' as follows:
//..
//  bsl::printf("BDL version: %s\n", bdlscm::Version::version());
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BDLSCM_VERSIONTAG
#include <bdlscm_versiontag.h>     // 'BDL_VERSION_MAJOR', 'BDL_VERSION_MINOR'
#endif

#ifndef INCLUDED_BSLS_BUILDTARGET
#include <bsls_buildtarget.h>      // need to ensure consistent build options
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif


namespace BloombergLP {
namespace bdlscm {

struct Version {
    static const char *d_ident;
    static const char *d_what;

#define BDLSCM_CONCAT2(a,b,c,d,e,f) a ## b ## c ## d ## e ## f
#define BDLSCM_CONCAT(a,b,c,d,e,f)  BDLSCM_CONCAT2(a,b,c,d,e,f)

// 'BDLSCM_D_VERSION' is a symbol whose name warns users of version mismatch
// linking errors.  Note that the exact string "compiled_this_object" must be
// present in this version coercion symbol.  Tools may look for this pattern to
// warn users of mismatches.
#define BDLSCM_D_VERSION BDLSCM_CONCAT(                \
                       d_version_BDL_,                 \
                       BDL_VERSION_MAJOR, _,           \
                       BDL_VERSION_MINOR,              \
                       BDL_VERSION_RELEASETYPE_SYMBOL, \
                       _compiled_this_object)

    static const char *BDLSCM_D_VERSION;

    static const char *d_dependencies;
    static const char *d_buildInfo;
    static const char *d_timestamp;
    static const char *d_sourceControlInfo;

    static const char *version();
};

inline
const char *Version::version()
{
    return BDLSCM_D_VERSION;
}

}  // close package namespace

// Force linker to pull in this component's object file.

#if defined(BSLS_PLATFORM_CMP_IBM)
static const char **bdlscm_version_assertion =
                                            &bdlscm::Version::BDLSCM_D_VERSION;
#else
namespace {
    extern const char **const bdlscm_version_assertion =
                                            &bdlscm::Version::BDLSCM_D_VERSION;
}
#endif

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
