// bslscm_versiontag.cpp                                              -*-C++-*-
#include <bslscm_versiontag.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

// IMPLEMENTATION NOTE:
//
// The actual output format from 'BSL_MAKE_VERSION' is deliberately
// underspecified in the header, as it is subject to change without notice.
//
// The current output from 'BSL_MAKE_VERSION(MA, MINR)' is the integer
// 'MAMINR00', e.g., 'BSL_MAKE_VERSION(12, 9998) == 12999800'.

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
