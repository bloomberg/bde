// bdlf_bind.09.t.cpp                                                 -*-C++-*-

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// Testing 'bdlf::Bind' using 7-argument functions.  See the test plan in
// 'bdlf_bind.00.t.cpp'.
// ----------------------------------------------------------------------------

// Count
#define BBT_n 7

// S with parameter count appended
#define BBT_C(S) S##7

// Repeat comma-separated S once per number of parameters with number appended
#define BBT_N(S) S##1,S##2,S##3,S##4,S##5,S##6,S##7

// Repeat comma-separated S once per number of parameters
#define BBT_R(S) S,   S,   S,   S,   S,   S,   S

// Generate a test driver
#define BDLF_BIND_00T_AS_GENERATOR

#include <bdlf_bind.00.t.cpp>

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
