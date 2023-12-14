// bdljsn_jsontestsuiteutil.h                                         -*-C++-*-
#ifndef INCLUDED_BDLJSN_JSONTESTSUITEUTIL
#define INCLUDED_BDLJSN_JSONTESTSUITEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide JSON Test Suite for BDE table-driven testing.
//
//@CLASSES:
//  bdljsn::JsonTestSuiteUtil: data/types representing the JSON Test Suite
//
//@SEE_ALSO: 'bdljsn_jsonutil'
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bdljsn::JsonTestSuiteUtil', that encapsulates the JSON Test Suite found at:
// https://github.com/nst/JSONTestSuite/tree/master.  Note that the test suite
// is itself an appendix to the article *Parsing* *JSON* *is* *a* *Minefield*
// by Nicolas Seriot (see https://seriot.ch/projects/parsing_json.html).
//
// The test points are constructed from the files found under:
// https://github.com/nst/JSONTestSuite/blob/master/test_parsers.  The name of
// these files indicate whether or not their contents should be accepted or
// rejected.
//..
//  +--------+-------+----------------------------------------------+
//  | Prefix | Count | Expected Result                              |
//  +--------+-------+----------------------------------------------+
//  | 'y_'   |    95 | content must be accepted by parsers          |
//  | 'n_'   |   188 | content must be rejected by parsers          |
//  | 'i_'   |    35 | parsers are free to accept or reject content |
//  +--------+-------+----------------------------------------------+
//..
// Note that this component provides one additional 'y_' (not counted above),
// 'y_henry_verschell_smiley_surrogate_smiley.json', that does not exist in
// downloaded test suite.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use
/// - - - - - - - - - -
// Generally, BDE table-drive testing uses tables defined locally in the test
// driver.  To conveniently use the table defined in the
// 'bdljsn_jsontestsuiteutil' component, some small adaptation to the test
// driver is recommended.
//
// First, create aliases for symbols conventionally used in BDE table-driven
// tests:
//..
//  typedef bdljsn::JsonTestSuiteUtil           JTSU;
//  typedef bdljsn::JsonTestSuiteUtil::Expected Expected;
//..
// Now, use these symbols in a typical table-driven 'for'-loop:
//..
//  for (bsl::size_t ti = 0; ti < JTSU::numData(); ++ti) {
//      const int         LINE      = JTSU::data(ti)->d_line;
//      const char *const TEST_NAME = JTSU::data(ti)->d_testName_p;
//      const char *const JSON      = JTSU::data(ti)->d_JSON_p;
//      const bsl::size_t LENGTH    = JTSU::data(ti)->d_length;
//      const Expected    EXPECTED  = JTSU::data(ti)->d_expected;
//
//      if (veryVerbose) {
//          P_(ti) P_(LINE) P_(LENGTH) P(EXPECTED)
//          P(TEST_NAME);
//          P(JSON)
//      }
//
//      // testing code...
//  }
//..

#include <bsl_cstddef.h>  // 'bsl::size_t'

namespace BloombergLP {
namespace bdljsn {

                        // ========================
                        // struct JsonTestSuiteUtil
                        // ========================

struct JsonTestSuiteUtil {
    // This utility 'struct' provides a namespace for the test points of the
    // *JSON* *Test* *Suite*.

  public:
    // TYPES
    enum Expected {
        e_EITHER = -1
      , e_REJECT =  0
      , e_ACCEPT =  1
    };

    struct Datum {
        // This aggregate type describes a test-point of the *JSON* *Test*
        // *Suite*.

        int          d_line;
        const char  *d_testName_p;  // name of the JSON test file
        const char  *d_JSON_p;      // contents of the JSON test file
        bsl::size_t  d_length;      // length of the 'd_JSON_p' contents
        Expected     d_expected;    // accept/reject/either
    };

  private:
    // CLASS DATA
    static       Datum       s_data[];    // 'y_'/'n_'/'i_' test points
    static const bsl::size_t s_numData;   // number of test points

  public:
    // CLASS METHODS
    static const Datum *data(bsl::size_t index);
        // Return the test-point 'Datum' for the specified 'index'.  The
        // behavior is undefined unless '0 <= index < numData()'.

    static bsl::size_t numData();
        // Return the number of test points in the *JSON* *Test* *Suite*.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
