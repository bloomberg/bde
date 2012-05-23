// bbescm_version.cpp          -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(bbescm_version_cpp,"$Id$ $CSID$")


#include <bbescm_version.h>
#include <bbescm_versiontag.h>

namespace BloombergLP {

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define REQUIRED_BDE_PATCH 0
#define BDESCM_VERSION_STRING "BLP_LIB_BDE_BDE_" STRINGIFY(BDE_VERSION_MAJOR) \
                                             "." STRINGIFY(BDE_VERSION_MINOR) \
                                             "." STRINGIFY(REQUIRED_BDE_PATCH)

#define BBE_VERSION_PATCH 0
#define BBESCM_VERSION_STRING "BLP_LIB_BDE_BBE_" STRINGIFY(BBE_VERSION_MAJOR) \
                                             "." STRINGIFY(BBE_VERSION_MINOR) \
                                             "." STRINGIFY(BBE_VERSION_PATCH) \
                                             BDE_VERSION_RELEASETYPE

const char *bbescm_Version::d_ident = "$Id: "BBESCM_VERSION_STRING" $";
const char *bbescm_Version::d_what  = "@(#)"BBESCM_VERSION_STRING;

const char *bbescm_Version::d_version           = BBESCM_VERSION_STRING;
const char *bbescm_Version::d_dependencies      = BDESCM_VERSION_STRING;
const char *bbescm_Version::d_buildInfo         = "";
const char *bbescm_Version::d_timestamp         = "";
const char *bbescm_Version::d_sourceControlInfo = "";

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
