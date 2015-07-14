// btlscm_version.cpp          -*-C++-*-
#include <btlscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlscm_version_cpp,"$Id$ $CSID$")

#include <bslscm_patchversion.h> // BDESCM_PATCHVERSION_PATCH

#include <btlscm_versiontag.h>

#include <bdlscm_versiontag.h>

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

#define BTE_VERSION_PATCH BDESCM_PATCHVERSION_PATCH
#define BTESCM_VERSION_STRING "BLP_LIB_BDE_BTE_" STRINGIFY(BTE_VERSION_MAJOR) \
                                             "." STRINGIFY(BTE_VERSION_MINOR) \
                                             "." STRINGIFY(BTE_VERSION_PATCH)

const char *btlscm::Version::s_ident = "$Id: " BTESCM_VERSION_STRING " $";
const char *btlscm::Version::s_what  = "@(#)" BTESCM_VERSION_STRING;

const char *btlscm::Version::s_version           = BTESCM_VERSION_STRING;
const char *btlscm::Version::s_dependencies      =
                               BDESCM_VERSION_STRING " " BCESCM_VERSION_STRING;
const char *btlscm::Version::s_buildInfo         = "";
const char *btlscm::Version::s_timestamp         = "";
const char *btlscm::Version::s_sourceControlInfo = "";



}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
