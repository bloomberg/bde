// bcescm_version.cpp          -*-C++-*-
#include <bcescm_version.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcescm_version_cpp,"$Id$ $CSID$")

namespace BloombergLP {

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define REQUIRED_BDE_PATCH 0
#define BDESCM_VERSION_STRING "BLP_LIB_BDE_BDE_" STRINGIFY(BDE_VERSION_MAJOR) \
                                             "." STRINGIFY(BDE_VERSION_MINOR) \
                                             "." STRINGIFY(REQUIRED_BDE_PATCH)

#define BCE_VERSION_PATCH 0
#define BCESCM_VERSION_STRING "BLP_LIB_BDE_BCE_" STRINGIFY(BCE_VERSION_MAJOR) \
                                             "." STRINGIFY(BCE_VERSION_MINOR) \
                                             "." STRINGIFY(BCE_VERSION_PATCH) \
                                             BDE_VERSION_RELEASETYPE

const char *bcescm_Version::d_ident = "$Id: "BCESCM_VERSION_STRING" $";
const char *bcescm_Version::d_what  = "@(#)"BCESCM_VERSION_STRING;

const char *bcescm_Version::d_version           = BCESCM_VERSION_STRING;
const char *bcescm_Version::d_dependencies      = BDESCM_VERSION_STRING;
const char *bcescm_Version::d_buildInfo         = "";
const char *bcescm_Version::d_timestamp         = "";
const char *bcescm_Version::d_sourceControlInfo = "";



}  // close namespace BloombergLP


// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
