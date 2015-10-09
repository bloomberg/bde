// bsls_asserttest.cpp                                                -*-C++-*-
#include <bsls_asserttest.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_asserttestexception.h>
#include <bsls_bsltestutil.h>   // for testing purposes only
#include <bsls_platform.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------------------------------------

#ifdef BSLS_ASSERTTEST_NORETURN
#error BSLS_ASSERTTEST_NORETURN must be a macro scoped locally to this file
#endif

// Note that a portable syntax for 'noreturn' will be available once we have
// access to conforming C++0x compilers.
//# define BSLS_ASSERTTEST_NORETURN [[noreturn]]

#if defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLS_ASSERTTEST_NORETURN __declspec(noreturn)
#else
#   define BSLS_ASSERTTEST_NORETURN
#endif

//-----------------------------------------------------------------------------
// STATIC HELPER FUNCTIONS
static
void printError(const char *text, const char *file, int line)
    // Print a formatted error message to 'stderr' using the specified
    // expression 'text', 'file' name, and 'line' number.  If either 'text' or
    // 'file' is empty (""), or null (0), replace it with some informative,
    // "human-readable" text, before formatting.
{
    // Note that we deliberately use 'stdio' rather than 'iostream' to avoid
    // issues pertaining to memory allocation for file-scope 'static' objects
    // such as 'std::cerr'.  Also note that the '<iostream>' components are
    // located in a package higher in the levelization of 'bsl'.

    if (!text) {
        text = "(* Unspecified Expression Text *)";
    }
    else if (!*text) {
        text = "(* Empty Expression Text *)";
    }

    if (!file) {
        file = "(* Unspecified File Name *)";
    }
    else if (!*file) {
        file = "(* Empty File Name *)";
    }

    fprintf(stderr,
            "Assertion failed: %s, file %s, line %d\n", text, file, line);

    fflush(stderr);  // Not necessary for the default 'stderr', but just
                     // in case it has been reopened as a buffered stream.
}

static
bool isPathSeparator(char c)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    static const char pathSeparators[] = { ':', '/', '\\' };
#else
    static const char pathSeparators[] = { '/' };
#endif
    for (int i = 0; i != sizeof(pathSeparators); ++i) {
        if (pathSeparators[i] == c) {
            return true;                                              // RETURN
        }
    }
    return false;
}

static
bool extractComponentName(const char **componentName,
                          int         *length,
                          const char  *filename)
    // Return 'true' if the specified 'filename' corresponds to a valid
    // filename for a BDE component, and 'false' otherwise.  If 'filename'
    // corresponds to a valid component name, 'componentName' will be set to
    // point to the start of that component name within 'filename', and
    // 'length' will store the length of that component name.  Note that this
    // sub-string will *not* be null-terminated.
{
    // A component filename is a filename ending in one of the following set of
    // file extensions: '.h', '.cpp', '.t.cpp'.
    // The component name is the component filename minus the extension and any
    // leading pathname.

    // The basic algorithm searches for the end of the 'filename' and then
    // iterates backwards.  First we verify that the filename has a valid
    // extension for a component: '.cpp', '.t.cpp', or '.h'.  Then we keep
    // searching backwards for the first path separator, which will depend on
    // the filesystem the compiler is running on.  The string between the last
    // path separator (if any) and the period starting the file extension mark
    // out the component name to be returned.

    if (!componentName || !length || !filename) {
         printf("passed at least one null pointer\n");
         return false;                                                // RETURN
    }

    const char *end = filename;
    for (; *end; ++end) {
        // Find the end of the string.
    }
    if (3 > (end - filename)) {
        printf("filename is too short\n");
        return false;                                                 // RETURN
    }

    --end;
    if ('h' == *end) {
        if ('.' != *--end) {
             printf("filename is not a header\n");
             return false;                                            // RETURN
        }
    }
    else if ('p' == *end) {
        if (4 > (end - filename)) {
             printf("filename is not long enough for a .cpp\n");
             return false;                                            // RETURN
        }

        if ('p' != *--end) {
             printf("filename is not a .cpp(1)\n");
             return false;                                            // RETURN
        }

        if ('c' != *--end) {
             printf("filename is not a .cpp(2)\n");
             return false;                                            // RETURN
        }

        if ('.' != *--end) {
             printf("filename is not a .cpp(3)\n");
             return false;                                            // RETURN
        }

        if (2 < (end - filename)) {
            const char *cursor = end;
            if ('t' == *--cursor) {
                if ('.' == *--cursor) {
                     end = cursor;
                }
            }
        }
    }
    else {
         printf("filename is not recognized\n");
         return false;                                                // RETURN
    }

    const char *cursor = end;
    while (cursor != filename) {
        --cursor;
        if (isPathSeparator(*cursor)) {
            ++cursor;
            break;
        }
    }

    *componentName = cursor;
    *length = static_cast<int>(end - cursor);
    return true;
}

//-----------------------------------------------------------------------------

namespace BloombergLP {

namespace bsls {

                            // ----------------
                            // class AssertTest
                            // ----------------

// CLASS METHODS
bool AssertTest::isValidAssertBuild(const char *specString)
{
    if (specString) {
        switch (*specString) {
          case 'S':
          case 'A':
          case 'O':
            return '\0' == specString[1]
                || ('2' == specString[1] && '\0' == specString[2]);   // RETURN
        }
    }
    return false;
}

bool AssertTest::isValidExpected(char specChar)
{
    return 'F' == specChar || 'P' == specChar;
}

                        // Testing Apparatus

bool AssertTest::tryProbe(char expectedResult)
{
    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to 'tryProbe': '%c'\n",
               expectedResult);
    }

    return 'P' == expectedResult;
}

bool AssertTest::catchProbe(char                        expectedResult,
                            const AssertTestException&  caughtException,
                            const char                 *componentFileName)
{
    // First we must validate each argument, in order to write a diagnostic
    // message to the console.

    bool validArguments = true;

    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to 'catchProbe': '%c'\n",
               expectedResult);
        validArguments = false;
    }

    const char *text = caughtException.expression();
    const char *file = caughtException.filename();

    const char *exceptionComponent;
    int exceptionNameLength;
    if (file && !extractComponentName(&exceptionComponent,
                                      &exceptionNameLength,
                                      file)) {
        printf("Bad component name in exception caught by catchProbe: %s\n",
               file);
        validArguments = false;
    }

    validArguments = validArguments && caughtException.lineNumber() > 0
                                    && text
                                    && *text;

    if (!validArguments) {
        printError(text, file, caughtException.lineNumber());
    }

    // Note that 'componentFileName' is permitted to be NULL.

    const char *thisComponent;
    int thisNameLength;
    if (componentFileName && !extractComponentName(&thisComponent,
                                                   &thisNameLength,
                                                   componentFileName)) {
        printf("Bad component name for test driver in catchProbe: %s\n",
               componentFileName);
        validArguments = false;
    }

    // After diagnosing any invalid arguments, we can now return a result.

    if ('F' != expectedResult || !validArguments) {
        return false;                                                 // RETURN
    }

    // If 'componentFilename' is null then it is deemed to match any filename.

    if (0 != componentFileName) {
        // Two component filenames match if they are the same component name,
        // regardless of path, and regardless of file-extension.

        if (thisNameLength != exceptionNameLength
         || 0 != strncmp(thisComponent, exceptionComponent, thisNameLength)) {
            return false;                                             // RETURN
        }
    }

    return true;
}

bool AssertTest::tryProbeRaw(char expectedResult)
{
    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to a 'tryProbeRaw': '%c'\n",
               expectedResult);
    }

    return 'P' == expectedResult;
}

bool AssertTest::catchProbeRaw(char expectedResult)
{
    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to a 'catchProbeRaw': '%c'\n",
               expectedResult);
    }

    return 'F' == expectedResult;
}

BSLS_ASSERTTEST_NORETURN
void AssertTest::failTestDriver(const char *text,
                                const char *file,
                                int         line)
{
#ifdef BDE_BUILD_TARGET_EXC
    throw AssertTestException(text, file, line);
#else
    printError(text, file, line);
    abort();
#endif
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
