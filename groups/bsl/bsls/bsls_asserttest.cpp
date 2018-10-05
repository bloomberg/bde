// bsls_asserttest.cpp                                                -*-C++-*-
#include <bsls_asserttest.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assertimputil.h>
#include <bsls_asserttestexception.h>
#include <bsls_bsltestutil.h>   // for testing purposes only
#include <bsls_platform.h>
#include <bsls_log.h>
#include <bsls_logseverity.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------------------------------------

#ifdef BSLS_ASSERTTEST_NORETURN
#error BSLS_ASSERTTEST_NORETURN must be a macro scoped locally to this file
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN)
#   define BSLS_ASSERTTEST_NORETURN [[noreturn]]
#elif defined(BSLS_PLATFORM_CMP_MSVC)
#   define BSLS_ASSERTTEST_NORETURN __declspec(noreturn)
#else
#   define BSLS_ASSERTTEST_NORETURN
#endif

//-----------------------------------------------------------------------------
// STATIC HELPER FUNCTIONS

namespace BloombergLP {

namespace {

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

    BloombergLP::bsls::Log::logFormattedMessage(
        BloombergLP::bsls::LogSeverity::e_ERROR,
        file,
        line,
        "Assertion failed: %s",
        text);
}

static
bool isPathSeparator(char c)
    // Return 'true' if the specified 'c' is a valid path separator on the
    // current platform, otherwise return 'false'.
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
bool extractTestedComponentName(const char **testedComponentName,
                                int         *length,
                                const char  *filename)
    // Return 'true' if the specified 'filename' corresponds to a valid
    // filename for a BDE component, and 'false' otherwise.  If 'filename'
    // corresponds to a valid component name, the specified
    // 'testedComponentName' will be set to point to the start of the "tested"
    // component name within 'filename', and the specified 'length' will store
    // the length of that component name.  The "Tested" component is generally
    // the full component name, but for subordinate test components (those
    // where the subordinate part begins with "test") it is the parent
    // component.  Note that this sub-string will *not* be null-terminated.
{
    // A component filename is a filename ending in one of the following set of
    // file extensions: '.h', '.cpp', '.t.cpp'.  The component name is the
    // component filename minus the extension and any leading pathname.

    // The basic algorithm searches for the end of the 'filename' and then
    // iterates backwards.  First we verify that the filename has a valid
    // extension for a component: '.cpp', '.t.cpp', or '.h'.  Then we keep
    // searching backwards for the first path separator, which will depend on
    // the filesystem the compiler is running on.  The string between the last
    // path separator (if any) and the period starting the file extension mark
    // out the component name to be returned.

    // After the full component name is found check to see if it is a
    // subordinate test component and remove the '_test.*' part if it is.

    if (!testedComponentName || !length || !filename) {
        printf("passed at least one null pointer\n");
        return false;                                                 // RETURN
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
            return false;                                             // RETURN
        }
    }
    else if ('p' == *end) {
        if (4 > (end - filename)) {
            printf("filename is not long enough for a .cpp\n");
            return false;                                             // RETURN
        }

        if ('p' != *--end) {
            printf("filename is not a .cpp(1)\n");
            return false;                                             // RETURN
        }

        if ('c' != *--end) {
            printf("filename is not a .cpp(2)\n");
            return false;                                             // RETURN
        }

        if ('.' != *--end) {
            printf("filename is not a .cpp(3)\n");
            return false;                                             // RETURN
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
        return false;                                                 // RETURN
    }

    const char *cursor = end;
    while (cursor != filename) {
        --cursor;
        if (isPathSeparator(*cursor)) {
            ++cursor;
            break;
        }
    }

    // after
    int underscorecount = 0;
    for (const char *c = cursor; c != end; ++c)
    {
        if (*c  == '_') { ++underscorecount; }
    }

    if (underscorecount >= 2)
    {
        // this is a subordinate component.

        const char *subordcursor = end;
        while (subordcursor != cursor) {
            --subordcursor;
            if (*subordcursor == '_') {
                ++subordcursor;
                break;
            }
        }

        if (end - subordcursor >= 4
            && subordcursor[0] == 't'
            && subordcursor[1] == 'e'
            && subordcursor[2] == 's'
            && subordcursor[3] == 't')
        {
            // This is a subordinate test component.
            end = subordcursor;
            --end;
        }
    }

    *testedComponentName = cursor;
    *length = static_cast<int>(end - cursor);
    return true;
}

}  // close unnamed namespace

//-----------------------------------------------------------------------------

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

bool AssertTest::isValidExpectedLevel(char specChar)
{
    return 'S' == specChar || 'A' == specChar || 'O' == specChar;
}

                            // Testing Apparatus

bool AssertTest::catchProbe(char                        expectedResult,
                            bool                        checkLevel,
                            char                        expectedLevel,
                            const AssertTestException&  caughtException,
                            const char                 *componentFileName)
{
    // First we see if the component names passed in are valid.

    bool validArguments = true;

    const char *text = caughtException.expression();
    const char *file = caughtException.filename();

    const char *exceptionComponent = NULL;
    int         exceptionNameLength = 0;
    if (file && *file && !extractTestedComponentName(&exceptionComponent,
                                                     &exceptionNameLength,
                                                     file)) {
        printf("Bad component name in exception caught by catchProbe: %s\n",
               file);
        validArguments = false;
    }

    validArguments = validArguments && caughtException.lineNumber() > 0
                                    && text
                                    && *text;

#ifndef BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS
    validArguments = validArguments && exceptionComponent
                                    && *exceptionComponent;
#endif

    if (!validArguments) {
        printError(text, file, caughtException.lineNumber());
    }

    // Note that 'componentFileName' is permitted to be NULL.

    const char *thisComponent;
    int         thisNameLength;
    if (componentFileName && !extractTestedComponentName(&thisComponent,
                                                         &thisNameLength,
                                                         componentFileName)) {
        printf("Bad component name for test driver in catchProbe: %s\n",
               componentFileName);
        validArguments = false;
    }

    if (!validArguments) {
        return false;                                                 // RETURN
    }

    // After diagnosing any invalid component-related arguments we can delegate
    // to catchProbeRaw to check the common issues.

    if (!catchProbeRaw(expectedResult,
                       checkLevel,
                       expectedLevel,
                       caughtException)) {
        return false;                                                 // RETURN
    }

    // If 'exceptionComponent' is null (only when macros are built in a more
    // limitted mode that does not include the full filename) we cannot do any
    // additional component checking.

    // If 'componentFilename' is null then it is deemed to match any filename.

    if ((0 != exceptionComponent) && (0 != componentFileName)) {
        // Two component filenames match if they are the same component name,
        // regardless of path, and regardless of file-extension.

        if (thisNameLength != exceptionNameLength
         || 0 != strncmp(thisComponent, exceptionComponent, thisNameLength)) {

            printf("Failure in component %.*s but expected component %.*s\n",
                   exceptionNameLength,
                   exceptionComponent,
                   thisNameLength,
                   thisComponent);
            return false;                                             // RETURN
        }
    }

    return true;
}

bool AssertTest::catchProbeRaw(
                              char                             expectedResult,
                              bool                             checkLevel,
                              char                             expectedLevel,
                              const bsls::AssertTestException &caughtException)
{
    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to a 'catchProbeRaw': '%c'\n",
               expectedResult);
        return false;                                                 // RETURN
    }

    if (!isValidExpectedLevel(expectedLevel)) {
        printf("Invalid 'expectedLevel' passed to 'catchProbeRaw': '%c'\n",
               expectedLevel);
        return false;                                                 // RETURN
    }

    if ('F' != expectedResult)
    {
        printf("Unexpected assertion failure.\n");
        return false;                                                 // RETURN
    }

    if (checkLevel)
    {
        const char *level = caughtException.level();
        switch (expectedLevel)
        {
          case 'S':
            if (strcmp(level,bsls::Review::k_LEVEL_SAFE) != 0 &&
                strcmp(level,bsls::Assert::k_LEVEL_SAFE) != 0)
            {
                printf("Expected SAFE failure but got level:%s\n",level);
                return false;                                         // RETURN
            }
            break;
          case 'A':
            // if built in safe mode it's possible for a 'BSLS_ASSERT_SAFE' to
            // prevent execution from reaching a 'BSLS_ASSERT', so both levels
            // are allowed
            if (strcmp(level,bsls::Review::k_LEVEL_REVIEW) != 0 &&
                strcmp(level,bsls::Assert::k_LEVEL_ASSERT) != 0 &&
                strcmp(level,bsls::Review::k_LEVEL_SAFE) != 0 &&
                strcmp(level,bsls::Assert::k_LEVEL_SAFE) != 0)
            {
                printf("Expected ASSERT failure but got level:%s\n",level);
                return false;                                         // RETURN
            }
            break;
          case 'O':
            // if built in safe mode it's possible for a 'BSLS_ASSERT_SAFE' or
            // 'BSLS_ASSERT' to prevent execution from reaching a
            // 'BSLS_ASSERT_OPT', so all levels are allowed
            if (strcmp(level,bsls::Review::k_LEVEL_OPT) != 0 &&
                strcmp(level,bsls::Assert::k_LEVEL_OPT) != 0 &&
                strcmp(level,bsls::Review::k_LEVEL_REVIEW) != 0 &&
                strcmp(level,bsls::Assert::k_LEVEL_ASSERT) != 0 &&
                strcmp(level,bsls::Review::k_LEVEL_SAFE) != 0 &&
                strcmp(level,bsls::Assert::k_LEVEL_SAFE) != 0)
            {
                printf("Expected OPT failure but got level:%s\n",level);
                return false;                                         // RETURN
            }
            break;
        }
    }

    return true;
}

bool AssertTest::tryProbe(char expectedResult, char expectedLevel)
{
    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to 'tryProbe': '%c'\n",
               expectedResult);
        return false;                                                 // RETURN
    }

    if (!isValidExpectedLevel(expectedLevel)) {
        printf("Invalid 'expectedLevel' passed to 'tryProbe': '%c'\n",
               expectedLevel);
        return false;                                                 // RETURN
    }

    if ('P' != expectedResult) {
        printf("Expression passed that was expected to fail.\n");
        return false;                                                 // RETURN
    }

    return true;
}

bool AssertTest::tryProbeRaw(char expectedResult, char expectedLevel)
{
    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to 'tryProbeRaw': '%c'\n",
               expectedResult);
        return false;                                                 // RETURN
    }

    if (!isValidExpectedLevel(expectedLevel)) {
        printf("Invalid 'expectedLevel' passed to 'tryProbeRaw': '%c'\n",
               expectedLevel);
        return false;                                                 // RETURN
    }

    if ('P' != expectedResult) {
        printf("Expression passed that was expected to fail.\n");
        return false;                                                 // RETURN
    }

    return true;
}

                        // Testing Failure Handlers

BSLS_ASSERTTEST_NORETURN
void AssertTest::failTestDriver(const AssertViolation &violation)
{
#ifdef BDE_BUILD_TARGET_EXC
    throw AssertTestException(violation.comment(),
                              violation.fileName(),
                              violation.lineNumber(),
                              violation.assertLevel());
#else
    printError(violation.comment(),
               violation.fileName(),
               violation.lineNumber());
    abort();
#endif
}

void AssertTest::failTestDriverByReview(const ReviewViolation &violation)
{
#ifdef BDE_BUILD_TARGET_EXC
    throw AssertTestException(violation.comment(),
                              violation.fileName(),
                              violation.lineNumber(),
                              violation.reviewLevel());
#else
    printError(violation.comment(),
               violation.fileName(),
               violation.lineNumber());
    abort();
#endif
}

}  // close package namespace

}  // close enterprise namespace

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
