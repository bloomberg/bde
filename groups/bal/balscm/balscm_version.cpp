// balscm_version.cpp          -*-C++-*-
#include <balscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balscm_version_cpp,"$Id$ $CSID$")

#include <bslscm_patchversion.h> // BDESCM_PATCHVERSION_PATCH

#include <balscm_versiontag.h>

#include <bslscm_versiontag.h>

namespace BloombergLP {

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define REQUIRED_BDE_PATCH 0
#define BDESCM_VERSION_STRING "BLP_LIB_BDE_BDE_" STRINGIFY(BSL_VERSION_MAJOR) \
                                             "." STRINGIFY(BSL_VERSION_MINOR) \
                                             "." STRINGIFY(REQUIRED_BDE_PATCH)

#define REQUIRED_BCE_PATCH 0
#define BCESCM_VERSION_STRING "BLP_LIB_BDE_BCE_" STRINGIFY(BCE_VERSION_MAJOR) \
                                             "." STRINGIFY(BCE_VERSION_MINOR) \
                                             "." STRINGIFY(REQUIRED_BCE_PATCH)

#define BAL_VERSION_PATCH BDESCM_PATCHVERSION_PATCH
#define BALSCM_VERSION_STRING "BLP_LIB_BDE_BAL_" STRINGIFY(BAL_VERSION_MAJOR) \
                                             "." STRINGIFY(BAL_VERSION_MINOR) \
                                             "." STRINGIFY(BAL_VERSION_PATCH)

const char *balscm::Version::s_ident = "$Id: " BALSCM_VERSION_STRING " $";
const char *balscm::Version::s_what  = "@(#)" BALSCM_VERSION_STRING;

const char *balscm::Version::s_version           = BALSCM_VERSION_STRING;
const char *balscm::Version::s_dependencies      =
                               BDESCM_VERSION_STRING " " BCESCM_VERSION_STRING;
const char *balscm::Version::s_buildInfo         = "";
const char *balscm::Version::s_timestamp         = "";
const char *balscm::Version::s_sourceControlInfo = "";

#undef BALSCM_VERSION_STRING

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
