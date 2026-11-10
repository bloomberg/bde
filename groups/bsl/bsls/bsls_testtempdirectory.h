// bsls_testtempdirectory.h                                           -*-C++-*-
#ifndef INCLUDED_BSLS_TESTTEMPDIRECTORY
#define INCLUDED_BSLS_TESTTEMPDIRECTORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a per-test-case temporary directory for test drivers.
//
//@CLASSES:
//  bsls::TestTempDirectory: guard for a test case's temporary directory
//
//@SEE_ALSO: bdls_tempdirectoryguard
//
//@DESCRIPTION: This component provides a scoped guard,
// `bsls::TestTempDirectory`, that creates a uniquely named directory for the
// temporary files of one test case, and removes that directory, and
// everything in it, when the guard is destroyed.
//
// The directory is created in the system temporary directory: on Unix, the
// directory named by the `TMPDIR` environment variable, or `/tmp` if `TMPDIR`
// is unset or empty; on Windows, the directory returned by `GetTempPath`.  The
// name of the directory is:
// ```
//  <user>-<test driver name>-<test case>-<process id>-<suffix>
// ```
// where `<user>` is the value of the `USER` environment variable, or of
// `LOGNAME` if `USER` is unset or empty (`USERNAME` on Windows), or `unknown`
// if neither is set, and `<suffix>` is a sequence of characters chosen to make
// the name unique.  Putting the temporary files of each test case in its own
// directory keeps the system temporary directory uncluttered, and the name
// identifies the owner of a directory that is left behind when a test driver
// crashes.
//
// Failure to create the directory is fatal: a message is printed to `stderr`
// and the program is aborted.
//
// THIS COMPONENT MUST NOT BE USED OUTSIDE OF BDE TEST DRIVERS
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Writing a Temporary File in a Test Case
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose test case 3 of the test driver for `xyza_widget` must write data to
// a file and read it back.  First, we create a guard for the temporary
// directory of the test case:
// ```
//  const int testCase = 3;
//
//  bsls::TestTempDirectory tempDir("xyza_widget", testCase);
// ```
// Then, we build the name of a file in that directory:
// ```
//  char fileName[bsls::TestTempDirectory::k_PATH_BUFFER_SIZE];
//  assert(0 == tempDir.makePath(fileName, sizeof fileName, "data.txt"));
// ```
// Next, we write the file and read it back:
// ```
//  FILE *fp = fopen(fileName, "w+");
//  assert(fp);
//
//  fputs("Hello, world!\n", fp);
//  rewind(fp);
//
//  char buffer[32];
//  assert(fgets(buffer, sizeof buffer, fp));
//  assert(0 == strcmp("Hello, world!\n", buffer));
//
//  fclose(fp);
// ```
// Finally, when `tempDir` goes out of scope, the directory and `data.txt` are
// removed.

#include <bsls_platform.h>

#include <stddef.h>

namespace BloombergLP {
namespace bsls {

                          // =======================
                          // class TestTempDirectory
                          // =======================

/// This class implements a scoped guard that creates a uniquely named
/// temporary directory for one test case at construction, and removes the
/// directory, and everything in it, at destruction.
class TestTempDirectory {

  public:
    // PUBLIC CONSTANTS
    enum {
        k_PATH_BUFFER_SIZE = 4096  // size of a buffer large enough to hold a
                                   // path in the temporary directory
    };

  private:
    // DATA
    char d_path[k_PATH_BUFFER_SIZE];  // path of the directory

  private:
    // NOT IMPLEMENTED
    TestTempDirectory(const TestTempDirectory&);
    TestTempDirectory& operator=(const TestTempDirectory&);

  public:
    // CREATORS

    /// Create a uniquely named directory in the system temporary directory
    /// for the temporary files of the specified `testCase` of the test
    /// driver having the specified `testDriverName`.  Print a message to
    /// `stderr` and abort the program if the directory cannot be created.
    /// The behavior is undefined unless `testDriverName` is a non-empty
    /// null-terminated string containing only characters that are valid in
    /// a file name.
    TestTempDirectory(const char *testDriverName, int testCase);

    /// Remove the directory created at construction, and everything in it.
    ~TestTempDirectory();

    // ACCESSORS

    /// Load into the specified `result` buffer of the specified `size` the
    /// path of the file having the specified `name` in the directory created
    /// at construction: `path()`, a directory separator, and `name`.  Return
    /// 0 on success, and a non-zero value, with `result` holding the empty
    /// string if `0 < size`, if `size` is too small to hold the path and its
    /// terminating null character.  The behavior is undefined unless
    /// `result` addresses at least `size` writable bytes and `name` is a
    /// null-terminated string.  Note that, unlike `snprintf` with `path()`,
    /// this does not draw `-Wformat-truncation` warnings.
    int makePath(char *result, size_t size, const char *name) const;

    /// Return the path of the directory created at construction.  The path
    /// is in the system temporary directory, and does not end with a
    /// directory separator.
    const char *path() const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                          // -----------------------
                          // class TestTempDirectory
                          // -----------------------

// ACCESSORS
inline
const char *TestTempDirectory::path() const
{
    return d_path;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
