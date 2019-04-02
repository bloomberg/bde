// bsla_allocsize.cpp                                                 -*-C++-*-
#include <bsla_allocsize.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

// ----------------------------------------------------------------------------
// Implementation Note:
//
// gcc-7.3.0 and gcc-8.3.0 seem to have a bug with this feature, when declaring
// that annotation on a function winds up with that annotation applying to that
// function AND the one after it.
//
// The feature doesn't see to work on glang-7.0.1.
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
