// bslstl_hashtable_test2.h                                           -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASHTABLE_TEST2
#define INCLUDED_BSLSTL_HASHTABLE_TEST2

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide support for the 'bslstl_hashtable.t.cpp' test driver.
//
//@CLASSES:
//
//@SEE_ALSO: bslstl_hashtable
//
//@DESCRIPTION: This component provides a third translation unit for
// 'HashTable' test cases to reduce the test-driver's build time.  Test cases
// 1-10 plus the usage example test, are in the original component test driver,
// test cases 11-13 are in the 'bslstl_hashtable_test1' component's test
// driver, and the remaining tests (14 and higher) are in this subordinate
// component's test driver.
//
// As the number of test concerns grows, and the number of types to instantiate
// 'HashTable' with in order to gain complete code coverage grows, we have
// exceeded the limits of certain compilers.  For example, gcc will not produce
// a valid '.o' file as the number of template instantiations exceeds some
// internal tables.  In order to continue providing full coverage, the standard
// value-semantic type test cases 1-10 are handled by the primary component
// test driver, and the tests for all remaining functionality are moved into
// two components, that comprise only a test driver.

#include <bslstl_hashtable.h>

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
