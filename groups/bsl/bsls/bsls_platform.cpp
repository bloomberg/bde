// bsls_platform.cpp                                                  -*-C++-*-
#include <bsls_platform.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

                        // --------------------
                        // class bsls::Platform
                        // --------------------

// This class has no implementation; it is merely a place-holder for this
// component's logical name space.

// The compiler flags set up by the 'bsls_platform' header file will be sanity
// checked against those those passed in by the build tool.

// Ensure compiler flags are set when they have to be.
#if defined(BSLS_PLATFORM_CMP_MSVC)
    #if defined(BDE_BUILD_TARGET_EXC) && !defined(_CPPUNWIND)
        #error "Exception build without exception handling enabled (/GX)."
    #endif
#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
