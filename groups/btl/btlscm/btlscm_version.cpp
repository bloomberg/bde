// btlscm_version.cpp                                                 -*-C++-*-
#include <btlscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlscm_version_cpp,"$Id$ $CSID$")

#include <btlscm_versiontag.h>
#include <bdlscm_versiontag.h>

#include <bslscm_patchversion.h> // BSLSCM_PATCHVERSION_PATCH

namespace BloombergLP {

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define REQUIRED_BDL_PATCH 0
#define BDLSCM_VERSION_STRING "BLP_LIB_BDE_BDL_" STRINGIFY(BDL_VERSION_MAJOR) \
                                             "." STRINGIFY(BDL_VERSION_MINOR) \
                                             "." STRINGIFY(REQUIRED_BDL_PATCH)

#define BTL_VERSION_PATCH BSLSCM_PATCHVERSION_PATCH
#define BTLSCM_VERSION_STRING "BLP_LIB_BDE_BTL_" STRINGIFY(BTL_VERSION_MAJOR) \
                                             "." STRINGIFY(BTL_VERSION_MINOR) \
                                             "." STRINGIFY(BTL_VERSION_PATCH)

const char *btlscm::Version::s_ident = "$Id: " BTLSCM_VERSION_STRING " $";
const char *btlscm::Version::s_what  = "@(#)" BTLSCM_VERSION_STRING;

const char *btlscm::Version::s_version           = BTLSCM_VERSION_STRING;
const char *btlscm::Version::s_dependencies      = BDLSCM_VERSION_STRING;
const char *btlscm::Version::s_buildInfo         = "";
const char *btlscm::Version::s_timestamp         = "";
const char *btlscm::Version::s_sourceControlInfo = "";

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2015
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
