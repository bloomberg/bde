// baescm_version.cpp          -*-C++-*-
#include <baescm_version.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baescm_version_cpp,"$Id$ $CSID$")

#include <baescm_versiontag.h>

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

#define BAE_VERSION_PATCH 0
#define BAESCM_VERSION_STRING "BLP_LIB_BDE_BAE_" STRINGIFY(BAE_VERSION_MAJOR) \
                                             "." STRINGIFY(BAE_VERSION_MINOR) \
                                             "." STRINGIFY(BAE_VERSION_PATCH) \
                                             BDE_VERSION_RELEASETYPE

const char *baescm_Version::d_ident = "$Id: "BAESCM_VERSION_STRING" $";
const char *baescm_Version::d_what  = "@(#)"BAESCM_VERSION_STRING;

const char *baescm_Version::d_version           = BAESCM_VERSION_STRING;
const char *baescm_Version::d_dependencies      =
                                 BDESCM_VERSION_STRING" "BCESCM_VERSION_STRING;
const char *baescm_Version::d_buildInfo         = "";
const char *baescm_Version::d_timestamp         = "";
const char *baescm_Version::d_sourceControlInfo = "";

#undef BAESCM_VERSION_STRING

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
