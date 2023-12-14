// bdljsn_jsontestsuiteutil.t.cpp                                     -*-C++-*-
#include <bdljsn_jsontestsuiteutil.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
#include <bsl_filesystem.h>  // TC -1
#endif
#include <bsl_cstdlib.h>  // 'bsl::atoi'
#include <bsl_cstring.h>  // 'bsl::strcmp', 'bsl::strlen'
#include <bsl_fstream.h>
#include <bsl_iomanip.h>  // 'bsl::setw'
#include <bsl_ios.h>
#include <bsl_iostream.h> // 'bsl::cout', 'bsl::cerr', 'bsl::error'
#include <bsl_set.h>
#include <bsl_sstream.h>  // 'bsl::ostringstream'
#include <bsl_string.h>
#include <bsl_utility.h>  // 'bsl::pair'
#include <bsl_vector.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test provides a data table that captures a JSON
// conformance test suite.  Than validating the usage example, there is no
// automatic test.
//
// The manually-run test case '-1' is provided to confirm that the test data
// defined in this table is byte-identical to the content to the downloaded
// files of the test suite.  This test case also provides warnings of
// downloaded files having no table entry and table entries that have no
// corresponding file.
//
// The path to the directory of the downloaded 'test_parsing' files (see
// https://github.com/nst/JSONTestSuite/tree/master/test_parsing) must be
// provided as a command line argument (immediately following the test case
// number) when running the test driver.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] const Datum *data(bsl::size_t index);
// [ 1] bsl::size_t numData();
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// [ 1] CONCERN: LARGE INPUT SANITY CHECK
// [-1] CONCERN: CONTENT VALIDATION

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdljsn::JsonTestSuiteUtil Util;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool         verbose = argc > 2; static_cast<void>(        verbose);
    bool     veryVerbose = argc > 3; static_cast<void>(    veryVerbose);
    bool veryVeryVerbose = argc > 4; static_cast<void>(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   First usage example extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "USAGE EXAMPLE" << endl
                 << "=============" << endl;
        }

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
    typedef bdljsn::JsonTestSuiteUtil           JTSU;
    typedef bdljsn::JsonTestSuiteUtil::Expected Expected;
//..
// Now, use these symbols in a typical table-driven 'for'-loop:
//..
    for (bsl::size_t ti = 0; ti < JTSU::numData(); ++ti) {
        const int         LINE      = JTSU::data(ti)->d_line;
        const char *const TEST_NAME = JTSU::data(ti)->d_testName_p;
        const char *const JSON      = JTSU::data(ti)->d_JSON_p;
        const bsl::size_t LENGTH    = JTSU::data(ti)->d_length;
        const Expected    EXPECTED  = JTSU::data(ti)->d_expected;

        if (veryVerbose) {
            P_(ti) P_(LINE) P_(LENGTH) P(EXPECTED)
            P(TEST_NAME);
            P(JSON)
        }

        // testing code...
    }
//..

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // LARGE INPUT SANITY CHECK
        //
        // Concern:
        //: 1 The two "large" input test cases are initialized to their
        //:   expected values.
        //
        // Plan:
        //: 1 Compare the 'd_JSON_p' and 'd_length' fields to the expected
        //:   values.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Testing:
        //   CONCERN: LARGE INPUT SANITY CHECK
        //   const Datum *data(bsl::size_t index);
        //   bsl::size_t numData();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "LARGE INPUT SANITY CHECK" << endl
                          << "========================" << endl;

        bool foundLeftBrackets    = false;
        bool foundOpenArrayObject = false;

        for (bsl::size_t ti = 0; ti < Util::numData(); ++ti) {
            const char *const TEST_NAME = Util::data(ti)->d_testName_p;
            const char *const JSON      = Util::data(ti)->d_JSON_p;
            const bsl::size_t LENGTH    = Util::data(ti)->d_length;

            if (veryVerbose) {
                P_(ti); P_(TEST_NAME); P(LENGTH);
                P(JSON);
            }

            if (0 == bsl::strcmp(TEST_NAME,
                                 "n_structure_100000_opening_arrays.json")) {
                foundLeftBrackets = true;

                ASSERTV(ti, 68 == ti);

                ASSERTV(LENGTH, 100000 == LENGTH);
                for (bsl::size_t i = 0; i < LENGTH; ++i) {
                    ASSERTV(i, JSON[i], '[' == JSON[i]);
                }
                continue;
            }

            if (0 == bsl::strcmp(TEST_NAME,
                                 "n_structure_open_array_object.json")) {
                foundOpenArrayObject = true;

                ASSERTV(ti, 163 == ti);

                const char        openArrayObjectSubsequence[] = "[{\"\":";
                const bsl::size_t lenOpenArrayObjectSubsequence
                                           = sizeof openArrayObjectSubsequence
                                           - 1; // terminating '/0

                ASSERTV(LENGTH, 250001 == LENGTH);

                for (bsl::size_t i = 0;
                     i < 50000;
                     i += lenOpenArrayObjectSubsequence) {

                    ASSERTV(i, 0 == bsl::strncmp(
                                               openArrayObjectSubsequence,
                                               &JSON[i],
                                               lenOpenArrayObjectSubsequence));
                }

                ASSERTV(JSON[LENGTH - 1],  '\n' == JSON[LENGTH - 1]);
                continue;
            }
        }

        ASSERTV("n_structure_100000_opening_arrays", foundLeftBrackets);
        ASSERTV("n_structure_open_array_object",     foundOpenArrayObject);

        if (verbose) cout << endl << "Negative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            typedef bdljsn::JsonTestSuiteUtil JTSU;

            const bsl::size_t NUMBER_DATA = JTSU::numData();

            ASSERT_FAIL(JTSU::data(NUMBER_DATA));
            ASSERT_PASS(JTSU::data(NUMBER_DATA - 1));
            ASSERT_PASS(JTSU::data(0));
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // CONTENT VALIDATION
        //
        // Concerns:
        //: 1 The 'd_JSON_p' member of each test-point entry matches the
        //:   contents of the JSON Conformance Standard file that is named by
        //:   the 'd_testName_p' member.
        //:
        //:   o The directory of the test files must be supplied as the first
        //:     command-line argument.
        //:
        //: 2 The Conformance test conventions on filenames are preserved by
        //:   the test-point entries.
        //:
        //:   o The leading letters of 'd_testName_p' are either 'y_' or 'n_'
        //:     (we do not use 'i_' files)->
        //:
        //:   o The 'd_isValid' value (expected test result) aligns with the
        //:     first letter of the file name.
        //
        // Plan:
        //: 1 The directory containing the 'test_parser' files of the test
        //:   suite is provided as the second command line argument (i.e., the
        //:   argument following test case number).
        //:
        //: 2 Compare strings constructed from the file contents and the
        //:   'd_JSON_p' members, respectively.  (C-1)
        //:
        //: 3 Confirm that table entries with test names beginning with
        //:   'y_'/'n_'/'i_' have 'd_expected' values that are
        //:   'e_ACCEPT'/'e_REJECT'/'e_EITHER'. respectively.
        //:
        //: 3 Check that every entry in the test suite directory has an entry
        //:   and vice versa.
        //:
        //:   o The table entry with the name
        //:     'y_henry_verschell_smiley_surrogate_smiley.json' is *not* part
        //:     of the official test suite.  Expect one error to be generated
        //:     due to the absence of this file.
        //
        // Testing:
        //   CONCERN: CONTENT VALIDATION
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONTENT VALIDATION" << endl
                          << "==================" << endl;

        if (argc < 3) {
            cerr << "Usage: -1 <'test_parsing' dir> [1] ... # verbosity"
                 << endl;
            testStatus = -2;
            break;
        }

        bsl::string testParsingDir(argv[2]);

        if (veryVerbose) {
            P(testParsingDir)
        }

        if (veryVerbose) {
            cout << "\n"
                 << "Compare JSON in each table entry"
                    "with contents of corresponding file."
                 << "\n"
                 << endl;
        }

        bsl::vector<bsl::string> missingFiles;

        for (bsl::size_t ti = 0; ti < Util::numData(); ++ti) {
            const int            LINE      = Util::data(ti)->d_line;
            const char *const    TEST_NAME = Util::data(ti)->d_testName_p;
            const char *const    JSON      = Util::data(ti)->d_JSON_p;
            const bsl::size_t    LENGTH    = Util::data(ti)->d_length;
            const Util::Expected EXPECTED  = Util::data(ti)->d_expected;

            if (veryVerbose) {
                P_(ti) P_(LINE) P_(LENGTH) P(EXPECTED)
                P(TEST_NAME);
                P(JSON)
            }

            if (2   <= bsl::strlen(TEST_NAME)
            &&  '_' == TEST_NAME[1]) {

                // Per:
                // https://github.com/nst/JSONTestSuite/blob/master/README.md
                //
                // '/test_parsing/'
                // ----------------
                // The name of these files tell if their contents should be
                // accepted or rejected.
                //
                //: o 'y_' content must be accepted by parsers
                //: o 'n_' content must be rejected by parsers
                //: o 'i_' parsers are free to accept or reject content

                switch (TEST_NAME[0]) {
                  case 'y': { ASSERTV(EXPECTED, Util::e_ACCEPT == EXPECTED);
                  } break;
                  case 'n': { ASSERTV(EXPECTED, Util::e_REJECT == EXPECTED);
                  } break;
                  case 'i': { ASSERTV(EXPECTED, Util::e_EITHER == EXPECTED);
                  } break;
                  default:  { ASSERTV(TEST_NAME[0], !"expected value");
                  } break;
                }
            } else {
                ASSERTV(TEST_NAME, !"well-formed 'TEST_NAME'");
            }

            bsl::string PATH = testParsingDir + "/" + bsl::string(TEST_NAME);

            bsl::ifstream ifs(PATH.c_str(),
                              bsl::ios_base::in | bsl::ios_base::binary);

            if (ifs.is_open()) {
                bsl::ostringstream oss;
                oss << ifs.rdbuf();
                bsl::string fileContents = oss.str();
                bsl::string codeContents(JSON, JSON + LENGTH);

                bsl::size_t lengthCode = codeContents.length();
                bsl::size_t lengthFile = fileContents.length();

                ASSERTV(PATH,
                        lengthCode,    lengthFile,
                        lengthCode  == lengthFile);

                ASSERTV(fileContents,   codeContents,
                        fileContents == codeContents);

                if (veryVeryVerbose) {
                    Q(fileContents)
                    for (bsl::size_t i = 0; i < lengthFile; ++i) {
                        const char ch = fileContents.data()[i];
                        const int  ci = static_cast<unsigned>(ch) & 0xFF;
                        bsl::cout
                            << bsl::setw( 5)             <<  i
                            << bsl::setw( 5)             << ch
                            << bsl::setw(15) << bsl::oct << ci
                            << bsl::setw(15) << bsl::dec << ci
                            << bsl::setw(15) << bsl::hex << ci
                            << endl;
                        bsl::cout << bsl::dec;
                    }
                    Q(codeContents)
                    for (bsl::size_t i = 0; i < lengthCode; ++i) {
                        const char ch = codeContents.data()[i];
                        const int  ci = static_cast<unsigned>(ch)& 0xFF;
                        bsl::cout
                            << bsl::setw( 5)             <<  i
                            << bsl::setw( 5)             << ch
                            << bsl::setw(15) << bsl::oct << ci
                            << bsl::setw(15) << bsl::dec << ci
                            << bsl::setw(15) << bsl::hex << ci
                            << endl;
                        bsl::cout << bsl::dec;
                    }
                }

            } else {
                if (veryVerbose) {
                    cout << "NG: Cannot open: " << TEST_NAME << endl;
                }
                missingFiles.push_back(TEST_NAME);
            }
        }

        ASSERTV(missingFiles.size(), 0 == missingFiles.size());

        if (veryVerbose) {
            for (bsl::size_t i = 0; i < missingFiles.size(); ++i) {
                P_(i) P(missingFiles[i])
            }
        }

        if (veryVerbose) {
            cout << "\n"
                 << "Identify files in directory that lack entry in table."
                 << "\n"
                 << "*and*"
                 << "\n"
                 << "Identify entries in table with no corresponding file."
                 << "\n"
                 << endl;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM) \
 && defined(BSLS_PLATFORM_OS_UNIX)
        {
            namespace fs = bsl::filesystem;

            const fs::path         testParsingDirPath(testParsingDir.c_str());
            fs::directory_iterator dirItr(testParsingDirPath);

            using DirectoryFiles = bsl::set<bsl::string>;

            DirectoryFiles directoryFiles;

            for (auto const& dirEntry : dirItr) {

                bsl::string entryName(dirEntry.path().filename().c_str());

                if (veryVerbose) {
                    static bool isFirst = true;
                    if (isFirst) {
                        Q(List of directory entries:)
                    }
                    P(entryName)
                    isFirst = false;
                }

                bsl::pair<DirectoryFiles::iterator, bool> result =
                                              directoryFiles.insert(entryName);
                ASSERT(true == result.second);
            }

            if (veryVerbose) {
                cout << endl
                     << "Number files in driectory                    : "
                     << directoryFiles.size()
                     << endl
                     << endl;
            }

            const DirectoryFiles::iterator end = directoryFiles.end();

            if (veryVerbose) {
                Q(Does each 'd_testName_p' correspond to a directory entry?)
            }

            for (bsl::size_t ti = 0; ti < Util::numData(); ++ti) {
                const char *const TEST_NAME = Util::data(ti)->d_testName_p;

                DirectoryFiles::iterator itr = directoryFiles.find(TEST_NAME);
                if (end == itr) {
                    if (veryVerbose) {
                        cout << "NG: Not in directory : " << TEST_NAME << endl;
                    }
                } else {
                    if (veryVerbose) {
                        cout << "OK: Is  in directory : " << TEST_NAME << endl;
                    }
                    directoryFiles.erase(itr);
                }
            }

            if (veryVerbose) {
                cout << "\n"
                     << "Number files in driectory without table entry: "
                     << directoryFiles.size()
                     << "\n"
                     << endl;
            }
            ASSERTV(directoryFiles.size(), 0 == directoryFiles.size());

            for (auto const& dirEntry : directoryFiles) {
                cout << "??: Absent from table: " << dirEntry << endl;
            }
        }
#else
        if (veryVerbose) {
            cout << "\t"   "SKIPPED: Requires:"                          "\n"
                    "\t\t" "'BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM'" "\n"
                    "\t\t" "'BSLS_PLATFORM_OS_UNIX'"                 << endl;
        }
#endif
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

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
