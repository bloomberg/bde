// bslscm_version.cpp                                                 -*-C++-*-
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

const char *bslscm::Version::d_ident = "$Id: " BSLSCM_VERSION_STRING " $";
const char *bslscm::Version::d_what  = "@(#)"  BSLSCM_VERSION_STRING;

const char *bslscm::Version::BSLSCM_D_VERSION    = BSLSCM_VERSION_STRING;
const char *bslscm::Version::d_dependencies      = "";
const char *bslscm::Version::d_buildInfo         = "";
const char *bslscm::Version::d_timestamp         = "";
const char *bslscm::Version::d_sourceControlInfo = "";

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
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
