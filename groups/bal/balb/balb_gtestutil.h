// balb_gtestutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BALB_GTESTUTIL
#define INCLUDED_BALB_GTESTUTIL

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

#include <balscm_version.h>

#include <bsl_ostream.h>
#include <bsl_string.h>

namespace bsl {

// FREE OPERATORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
void PrintTo(const basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  str,
             basic_ostream<CHAR_TYPE, CHAR_TRAITS>                *os_p);
    // Write the specified 'str' to the specified '*os_p', surrounded by double
    // quotes.  All 7-bit character values that can be represented in a C
    // string literal constant with backslash sequences other than '\?' and
    // '\'' are represented by their backslash sequence.  Character values over
    // 0x7f and non-escaped values below 0x20 (space) are represented by hex
    // backslash sequences.

}  // close namespace bsl

// ============================================================================
//                       FUNCTION TEMPLATE DEFINITIONS
// ============================================================================

// FREE OPERATORS
template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
void bsl::PrintTo(
               const bsl::basic_string<CHAR_TYPE,CHAR_TRAITS,ALLOCATOR>&  str,
               bsl::basic_ostream<CHAR_TYPE, CHAR_TRAITS>                *os_p)
{
    *os_p << '"' << str << '"' << bsl::flush;
}

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
