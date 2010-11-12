// bslscm_version.cpp          -*-C++-*-
#include <bslscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

#define BSL_VERSION_PATCH 0

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define BSLSCM_VERSION_STRING "BLP_LIB_BSL_BSL_" STRINGIFY(BSL_VERSION_MAJOR) \
                                             "." STRINGIFY(BSL_VERSION_MINOR) \
                                             "." STRINGIFY(BSL_VERSION_PATCH)

const char *bslscm_Version::d_ident = "$Id: "BSLSCM_VERSION_STRING" $";
const char *bslscm_Version::d_what  = "@(#)"BSLSCM_VERSION_STRING;

const char *bslscm_Version::BSLSCM_D_VERSION    = BSLSCM_VERSION_STRING;
const char *bslscm_Version::d_dependencies      = "";
const char *bslscm_Version::d_buildInfo         = "";
const char *bslscm_Version::d_timestamp         = "";
const char *bslscm_Version::d_sourceControlInfo = "";

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
