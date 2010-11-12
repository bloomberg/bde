// bdescm_version.cpp          -*-C++-*-
#include <bdescm_version.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdescm_version_cpp,"$Id$ $CSID$")

namespace BloombergLP {

#define BDE_VERSION_PATCH 0

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define BDESCM_VERSION_STRING "BLP_LIB_BDE_BDE_" STRINGIFY(BDE_VERSION_MAJOR) \
                                             "." STRINGIFY(BDE_VERSION_MINOR) \
                                             "." STRINGIFY(BDE_VERSION_PATCH) \
                                             BDE_VERSION_RELEASETYPE

const char *bdescm_Version::d_ident = "$Id: "BDESCM_VERSION_STRING" $";
const char *bdescm_Version::d_what  = "@(#)"BDESCM_VERSION_STRING;

const char *bdescm_Version::BDESCM_D_VERSION    = BDESCM_VERSION_STRING;
const char *bdescm_Version::d_dependencies      = "";
const char *bdescm_Version::d_buildInfo         = "";
const char *bdescm_Version::d_timestamp         = "";
const char *bdescm_Version::d_sourceControlInfo = "";

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
