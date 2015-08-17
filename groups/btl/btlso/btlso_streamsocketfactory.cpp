// btlso_streamsocketfactory.cpp                                      -*-C++-*-
#include <btlso_streamsocketfactory.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(btlso_streamsocketfactory_cpp,"$Id$ $CSID$")

#ifdef BTE_FOR_TESTING_ONLY
// These dependencies need to be here for the the bde_build.pl script to
// generate the proper makefiles, but do not need to be compiled into the
// component's .o file.  The symbol BTE_FOR_TESTING_ONLY should remain
// undefined, and is here only because '#if 0' is optimized away by the
// bde_build.pl script.

#include <btlso_streamsocket.h>             // for testing only
#include <btlso_socketoptutil.h>            // for testing only
#include <btlso_sockethandle.h>             // for testing only
#include <btlso_platform.h>                 // for testing only
#include <btlso_flag.h>                     // for testing only
#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
