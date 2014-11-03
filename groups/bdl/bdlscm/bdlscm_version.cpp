// bdlscm_version.cpp                                                 -*-C++-*-
#include <bdlscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlscm_version_cpp,"$Id$ $CSID$")

namespace BloombergLP {

#define BDL_VERSION_PATCH 1

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define BDLSCM_VERSION_STRING "BLP_LIB_BDL_BDL_" STRINGIFY(BDL_VERSION_MAJOR) \
                                             "." STRINGIFY(BDL_VERSION_MINOR) \
                                             "." STRINGIFY(BDL_VERSION_PATCH)

const char *bdlscm::Version::s_ident = "$Id: " BDLSCM_VERSION_STRING " $";
const char *bdlscm::Version::s_what  = "@(#)" BDLSCM_VERSION_STRING;

const char *bdlscm::Version::BDLSCM_S_VERSION    = BDLSCM_VERSION_STRING;
const char *bdlscm::Version::s_dependencies      = "";
const char *bdlscm::Version::s_buildInfo         = "";
const char *bdlscm::Version::s_timestamp         = "";
const char *bdlscm::Version::s_sourceControlInfo = "";

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
