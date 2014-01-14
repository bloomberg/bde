// btescm_version.cpp          -*-C++-*-
#include <btescm_version.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(btescm_version_cpp,"$Id$ $CSID$")

#include <bdescm_patchversion.h> // BDESCM_PATCHVERSION_PATCH

#include <btescm_versiontag.h>

#include <bdescm_versiontag.h>
#include <bcescm_versiontag.h>

namespace BloombergLP {

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define REQUIRED_BDE_PATCH 0
#define BDESCM_VERSION_STRING "BLP_LIB_BDE_BDE_" STRINGIFY(BDE_VERSION_MAJOR) \
                                             "." STRINGIFY(BDE_VERSION_MINOR) \
                                             "." STRINGIFY(REQUIRED_BDE_PATCH)

#define REQUIRED_BCE_PATCH 0
#define BCESCM_VERSION_STRING "BLP_LIB_BDE_BCE_" STRINGIFY(BCE_VERSION_MAJOR) \
                                             "." STRINGIFY(BCE_VERSION_MINOR) \
                                             "." STRINGIFY(REQUIRED_BCE_PATCH)

#define BTE_VERSION_PATCH BDESCM_PATCHVERSION_PATCH
#define BTESCM_VERSION_STRING "BLP_LIB_BDE_BTE_" STRINGIFY(BTE_VERSION_MAJOR) \
                                             "." STRINGIFY(BTE_VERSION_MINOR) \
                                             "." STRINGIFY(BTE_VERSION_PATCH)

const char *btescm_Version::s_ident = "$Id: " BTESCM_VERSION_STRING " $";
const char *btescm_Version::s_what  = "@(#)" BTESCM_VERSION_STRING;

const char *btescm_Version::s_version           = BTESCM_VERSION_STRING;
const char *btescm_Version::s_dependencies      =
                               BDESCM_VERSION_STRING " " BCESCM_VERSION_STRING;
const char *btescm_Version::s_buildInfo         = "";
const char *btescm_Version::s_timestamp         = "";
const char *btescm_Version::s_sourceControlInfo = "";



}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
