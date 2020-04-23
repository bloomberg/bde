// bslstl_map_test2.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLSTL_MAP_TEST2
#define INCLUDED_BSLSTL_MAP_TEST2

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for the 'bslstl_map.t.cpp' test driver.
//
//@CLASSES:
//
//@SEE_ALSO: bslstl_map
//
//@DESCRIPTION: This component provides a third translation unit for 'map' test
// cases to reduce the test-driver's build time.  The usage example test, and
// test cases 1-8 are in the original component test driver.  Test cases 9-27
// are in the 'bslstl_map_test1' component's test driver.  Test case 28 is in
// this subordinate component's test driver.  The remaining tests (29 and
// higher) are in the 'bslstl_map_test3' component's test driver.
//
// As the number of test concerns grows, and the number of types to instantiate
// 'map' with in order to gain complete code coverage grows, we have exceeded
// the limits of certain compilers.  For example, gcc will not produce a valid
// '.o' file as the number of template instantiations exceeds some internal
// tables.  In order to continue providing full coverage, some of the standard
// value-semantic type test cases (1-8) are handled by the primary component
// test driver, and the remaining standard tests, and tests for all remaining
// functionality are moved into three additional component, that comprise only
// a test driver.

#include <bslstl_map.h>

#endif
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
