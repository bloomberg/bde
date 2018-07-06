// bdlf_bind_test0.t.cpp                                              -*-C++-*-

#include <bdlf_bind_test0.h>

// Count
#define BBT_n 0

// S with parameter count appended
#define BBT_C(S) S##0

// Repeat comma-separated S once per number of parameters with number appended
#define BBT_N(S)

// Repeat comma-separated S once per number of parameters
#define BBT_R(S)

#include <bdlf_bind_testn.t.cpp>

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
