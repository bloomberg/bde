// bbescm_version.cpp                                                 -*-C++-*-
#include <bdes_ident.h>
BDES_IDENT_RCSID(bbescm_version_cpp,"$Id$ $CSID$")


#include <bbescm_version.h>
#include <bbescm_versiontag.h>

#include <bdescm_patchversion.h> // BDESCM_PATCHVERSION_PATCH

namespace BloombergLP {

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define REQUIRED_BDE_PATCH 0
#define BDESCM_VERSION_STRING "BLP_LIB_BDE_BDE_" STRINGIFY(BDE_VERSION_MAJOR) \
                                             "." STRINGIFY(BDE_VERSION_MINOR) \
                                             "." STRINGIFY(REQUIRED_BDE_PATCH)

#define BBE_VERSION_PATCH BDESCM_PATCHVERSION_PATCH
#define BBESCM_VERSION_STRING "BLP_LIB_BDE_BBE_" STRINGIFY(BBE_VERSION_MAJOR) \
                                             "." STRINGIFY(BBE_VERSION_MINOR) \
                                             "." STRINGIFY(BBE_VERSION_PATCH)

const char *bbescm_Version::s_ident = "$Id: " BBESCM_VERSION_STRING " $";
const char *bbescm_Version::s_what  = "@(#)"  BBESCM_VERSION_STRING;

const char *bbescm_Version::s_version           = BBESCM_VERSION_STRING;
const char *bbescm_Version::s_dependencies      = BDESCM_VERSION_STRING;
const char *bbescm_Version::s_buildInfo         = "";
const char *bbescm_Version::s_timestamp         = "";
const char *bbescm_Version::s_sourceControlInfo = "";

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
