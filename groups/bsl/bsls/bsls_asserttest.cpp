// bsls_asserttest.cpp                                                -*-C++-*-
#include <bsls_asserttest.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <bsls_asserttestexception.h>
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

#if defined(BSLS_PLATFORM__CMP_MSVC)
#   define BSLS_ASSERTTEST_NORETURN __declspec(noreturn)
#else
#   define BSLS_ASSERTTEST_NORETURN
#endif

//-----------------------------------------------------------------------------

static
void printError(const char *text, const char *file, int line)
    // Print a formatted error message to 'stderr' using the specified
    // expression 'text', 'file' name, and 'line' number.  If either
    // 'text' or 'file' is empty ("") or null (0), replace it with some
    // informative, "human-readable" text, before formatting.
{
    // Note that we deliberately use 'stdio' rather than 'iostream' to
    // avoid issues pertaining to memory allocation for file-scope 'static'
    // objects such as 'std::cerr'.

    // It should also be noted that the <iostream> components are located in a
    // package higher in the levelization of this group.

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
#ifdef BSLS_PLATFORM__OS_WINDOWS
    static const char pathSeparators[] = {':', '/', '\\'}; 
#else
    static const char pathSeparators[] = { '/' };
#endif
    for (int i = 0; i != sizeof(pathSeparators); ++i) {
        if (pathSeparators[i] == c) {
            return true;
        }
    }
    return false;
}

static
bool extractComponentName(const char ** componentName,
                          int * length,
                          const char * filename)
    // Returns 'true' if the passed 'filename' corresponds to a valid filename
    // for a BDE component, and 'false' otherwise.  If 'filename' corresponds
    // to a valid component name, 'componentName' will be set to point to the
    // start of that component name within 'filename', and 'length' will store
    // the length of that component name.  Note that this sub-string will *not*
    // be null-terminated.
{
    // A component filename is a filename ending in one of the
    // following set of file extensions: '.h', '.cpp', '.t.cpp'.
    // The component name is the component filename minus the extension
    // and any leading pathname.

    // The basic algorithm searches for the end of the 'filename' and then
    // iterates backwards.  First we verify that the filename has a
    // valid extension for a component: '.cpp', '.t.cpp' or '.h'.  Then we
    // keep searching backwards for the first path separator, which will
    // depend on the filesystem the compiler is running on.  The string
    // between the last path separator (if any) and the period starting
    // the file extension mark out the component name to be returned.
    if(!componentName || !length || !filename) {
         printf("passed at least one null pointer\n");
         return false;
    }

    const char * end = filename;
    for ( ; *end; ++end ) {} // find the end of the string
    if (3 > (end - filename)) {
        printf("filename is too short\n");
        return false;
    }

    --end;
    if ('h' == *end) {
        if ('.' != *--end) {
             printf("filename is not a header\n");
             return false;
        }
    }
    else if ('p' == *end) {
        if (4 > (end - filename)) {
             printf("filename is not long enough for a .cpp\n");
             return false;
        }

        if ('p' != *--end) {
             printf("filename is not a .cpp(2)\n");
             return false;
        }

        if ('c' != *--end) {
             printf("filename is not a .cpp(2)\n");
             return false;
        }

        if ('.' != *--end) {
             printf("filename is not a .cpp(3)\n");
             return false;
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
         return false;
    }

    const char * cursor = end;
    while (cursor != filename) {
        --cursor;
        if (isPathSeparator(*cursor)) {
            ++cursor;
            break;
        }
    }

    *componentName = cursor;
    *length = end - cursor;
    return true;
}

//-----------------------------------------------------------------------------

namespace BloombergLP {

                            // ---------------------
                            // class bsls_AssertTest
                            // ---------------------

// CLASS METHODS
bool bsls_AssertTest::isValidAssertBuild(const char *specString) 
{
    if (specString) {
        switch (*specString) {
          case 'S' :
          case 'A' :
          case 'O' :
                return '\0' == specString[1]
                    || ('2' == specString[1] && '\0' == specString[2]);
        }
    }
    return false;
}

bool bsls_AssertTest::isValidExpected(char specChar)
{
    return 'F' == specChar || 'P' == specChar;
}

                        // Testing Apparatus

bool bsls_AssertTest::tryProbe(char expectedResult)
{
    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to a tryProbe: '%c'\n",
               expectedResult);
    }

    return 'P' == expectedResult;
}

bool bsls_AssertTest::catchProbe(
                            char                             expectedResult,
                            const bsls_AssertTestException&  caughtException,
                            const char                      *componentFileName)
{
    bool validArguments = true;

    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to a catchProbe: '%c'\n",
               expectedResult);
        validArguments = false;
    }

    const char *text = caughtException.expression();
    const char *file = caughtException.filename();

    const char *exceptionComponent;
    int exceptionNameLength;
    if (file && !extractComponentName(
                            &exceptionComponent, &exceptionNameLength, file)) {
        printf("Bad component name in exception caught by catchProbe: %s\n",
               file);
        validArguments = false;
    }

    validArguments = validArguments && caughtException.lineNumber() > 0
                                    && text && *text;

    if (!validArguments) {
        printError(text, file, caughtException.lineNumber());
    }

    const char *thisComponent;
    int thisNameLength;
    if (componentFileName && !extractComponentName(
                         &thisComponent, &thisNameLength, componentFileName)) {
        printf("Bad component name for test driver in catchProbe: %s\n",
               componentFileName);
        validArguments = false;
    }

    if ('F' != expectedResult || !validArguments) {
        return false;
    }

    // If componentFilename is null then it is deemed to match, which is the
    // exact behavior of catchProbeRaw
    if(0 != componentFileName) {
        // Two component filenames match if they are the same component name,
        // regardless of path, and regardless of file-extension.
        // We have established both filenames are valid and non-empty, next we
        // scan backwards from the end of each filename to the end of the
        // component name.
    
        if (thisNameLength != exceptionNameLength || 
           0 != strncmp(thisComponent, exceptionComponent, thisNameLength)) {
               return false;
        }
    }

    return true;
}

bool bsls_AssertTest::tryProbeRaw(char expectedResult)
{
    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to a tryProbe: '%c'\n",
               expectedResult);
    }

    return 'P' == expectedResult;
}

bool bsls_AssertTest::catchProbeRaw(char expectedResult)
{
    if (!isValidExpected(expectedResult)) {
        printf("Invalid 'expectedResult' passed to a catchProbe: '%c'\n",
               expectedResult);
    }

    if ('F' != expectedResult) {
        return false;
    }

    return true;
}


BSLS_ASSERTTEST_NORETURN
void bsls_AssertTest::failTestDriver(const char *text, 
                                     const char *file, 
                                     int         line) 
{
#ifdef BDE_BUILD_TARGET_EXC
    throw bsls_AssertTestException(text, file, line);
#else
    printError(text, file, line);
    abort();
#endif

}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
