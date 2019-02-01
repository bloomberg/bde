// bslim_gtestutil.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLIM_GTESTUTIL
#define INCLUDED_BSLIM_GTESTUTIL

#include <bsls_ident.h>

//@PURPOSE: Provide facilities for debugging BDE with gtest.
//
//@DESCRIPTION: The 'bslim_gtestutil' component provides utitlities to
// facilitate testing with Google Test (GTest).
//
///Usage
///-----
// Suppose we have a string 'str' that we want to output:
//..
//  bsl::string str =
//                 "No matter where you go, There you are! -- Buckaroo Banzai";
//..
// Call 'PrintTo', passing the string and a pointer to a 'bsl::ostream':
//..
//  PrintTo(str, &cout);
//  cout << endl;
//..
// Which results in the string being streamed to standard output, surrounded by
// double quotes:
//..
//  "No matter where you go, There you are! -- Buckaroo Banzai"
//..

#include <bslscm_version.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

namespace bsl {

// FREE OPERATORS
void PrintTo(const string&  str,
             ostream       *os_p);
    // Write the specified 'str' to the specified '*os_p', surrounded by double
    // quotes.

void PrintTo(const wstring&  str,
             ostream       *os_p);
    // Write the specified 'wstr' to the specified '*os_p', surrounded by
    // double quotes, writing non-printable characters with '\x...' escapes.

}  // close namespace bsl

#endif

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
