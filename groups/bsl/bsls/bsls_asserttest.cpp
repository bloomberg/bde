// bsls_asserttest.cpp                                                -*-C++-*-
#include <bsls_asserttest.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assertimputil.h>
#include <bsls_bslsourcenameparserutil.h>
#include <bsls_bsltestutil.h>        // for testing only
#include <bsls_log.h>
#include <bsls_logseverity.h>
#include <bsls_macroincrement.h>     // for testing only
#include <bsls_platform.h>

#include <stdio.h>   // Must use C (not C++) headers in bsl
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------------------------------------
// STATIC HELPER FUNCTIONS

namespace BloombergLP {
namespace bsls {

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
    // such as 'cerr'.  Also note that the '<iostream>' components are located
    // in a package higher in the levelization of 'bsl'.

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

    Log::logFormattedMessage(
        LogSeverity::e_ERROR,
        file,
        line,
        "Assertion failed: %s",
        text);
}

class ComponentName {
    // A minimalistic reference type to a non-null terminated string determined
    // by a pointer and a length.  It is used to store the component name that
    // is "parsed out" of a source file name (without copying it).  This
    // reference type makes 'areTheSameComponent' easier to understand.

  private:
    // DATA
    const char  *d_str_p;
    size_t       d_length;

  public:
    // CREATORS
    explicit ComponentName(const char* sourceFilename);
        // Create an empty 'ComponentName' object and fill it from the
        // specified 'sourceFilename'.  If 'sourceFilename' is 0 or points to
        // an empty string set this object to the empty state.  Otherwise if
        // parsing of 'sourceFilename' succeeds fill this object to refer to
        // the resulting component name.  If parsing fails set this object to
        // the error state state, and print a brief error message to 'stdout'.
        // The parsing expect 'sourceFilename' to conform to Lakos Large Scale
        // C++ Design source file naming rules does not fully validate, so:
        // The resulting component name referred by this object may be a valid,
        // but unspecified string.


    // ACCESSORS
    bool isEmpty() const;
        // We use this method to indicate a null pointer source name string, as
        // well as empty source name strings, because there is no difference in
        // handling those two cases.

    bool isError() const;
        // For brevity we store errors as a non-zero 'd_length' with a zero
        // 'd_str_p' pointer.

    size_t length() const;
        // Return the number of characters of the component name referred by
        // this object.  Note that this function may return 0 if this object
        // is in the empty state, or an unspecified error code if this object
        // is in an error state, therefore is it not advised to use this
        // accessor if 'isEmpty() || isError()'.

    const char *str() const;
        // Return the pointer to the first character of the component name.
        // Note that this function may return 0 (null pointer) if this object
        // is in the empty state or in an error state, therefore is it not
        // advised to use this accessor if 'isEmpty() || isError()'.  Note that
        // the returned component name will not be null terminated.
};

// CREATORS
ComponentName::ComponentName(const char *sourceFilename)
{
    // null or empty allowed by default
    if (0 == sourceFilename || sourceFilename[0] == '\0') {
        d_str_p  = 0;
        d_length = 0;
        return;                                                       // RETURN
    }

    const int rc = BslSourceNameParserUtil::getComponentName(&d_str_p,
                                                             &d_length,
                                                             sourceFilename);
    if (rc != 0) {
        puts(BslSourceNameParserUtil::errorMessage(rc));
        d_str_p  = 0;
        d_length = static_cast<size_t>(rc);
    }
}

// ACCESSORS
bool ComponentName::isEmpty() const
{
    return d_str_p == 0 && d_length == 0;
}

bool ComponentName::isError() const
{
    return d_str_p == 0 && d_length != 0;
}

size_t ComponentName::length() const
{
    return d_length;
}

const char* ComponentName::str() const
{
    return d_str_p;
}

// FREE FUNCTIONS
bool operator==(const ComponentName& lhs, const ComponentName& rhs)
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ComponentName' objects have the same
    // value if their 'length' is the same, and their 'str' points to 'length'
    // characters that compare equal.  Since the compare operation dereferences
    // this type, and this type has two non-reference values (empty and error):
    // The behavior is undefined if either 'lhs' or 'rhs' is empty or an error.
{
    return lhs.length() == rhs.length() &&
           0 == memcmp(lhs.str(), rhs.str(), lhs.length());
}

static
bool areTheSameComponent(const ComponentName& throwName,
                         const ComponentName& testName)
    // Return 'true' if the specified 'throwName' and 'testName' component
    // names are considered the same in testing:
    //: o If either of the names was missing, we assume they are the same.
    //: o If both component names are provided they must match, including case.
{
    // For backwards compatibility reasons we do not verify if 'testName' is
    // actually a test driver, or if either is not the name of a subordinate
    // test component source (note that by definition the test driver is not
    // part of a component).  (Subordinate test components should not contain
    // executable code of any kind.)  We also do not check if the component
    // names are valid or not, as that might also break existing code due to
    // Hyrum's law.

    return testName.isEmpty()     // Considered valid, no need to compare
#ifndef BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS
        || throwName.isEmpty()     // Considered valid, no need to compare
#endif
        || throwName == testName;  // Component names must match
}

}  // close unnamed namespace

                              // ----------------
                              // class AssertTest
                              // ----------------

// CLASS METHODS

                    // Test Specification Validation

bool AssertTest::isValidAssertBuild(const char *specString)
{
    if (specString && isValidExpectedLevel(specString[0])) {
        return '\0' == specString[1]
            || ('2' == specString[1] && '\0' == specString[2]);       // RETURN
    }
    return false;
}

bool AssertTest::isValidExpected(char specChar)
{
    return 'F' == specChar || 'P' == specChar;
}

bool AssertTest::isValidExpectedLevel(char specChar)
{
    static const char k_VALIDS[] = { 'S', 'O', 'A', 'I' };
    return memchr(k_VALIDS, specChar, sizeof k_VALIDS) != 0;
}

                            // Testing Apparatus

bool AssertTest::catchProbe(char                        expectedResult,
                            bool                        checkLevel,
                            char                        expectedLevel,
                            const AssertTestException&  caughtException,
                            const char                 *testDriverFileName)
{
    bool validArguments = true;  // Assume success

    //----------------------------------------------------
    // Extract the throwing component name and validate it

    const char * const  throwingFilename(caughtException.filename());
    const ComponentName throwingComponent(throwingFilename);
    if (throwingComponent.isError()) {
        printf("Component source file name is malformed in the violation "
               "exception caught by 'bsls::AssertTest::catchProbe': \"%s\"\n",
               caughtException.filename());
        validArguments = false;
    }
#ifndef BSLS_ASSERTIMPUTIL_AVOID_STRING_CONSTANTS
    // Unless we save memory by dropping file names they should be present in
    // the exception:
    else if (throwingComponent.isEmpty()) {
        puts("Unexpected empty asserting-component source file name.");
        validArguments = false;
    }
#endif

    //----------------------------------------------------------------------
    // Validate throwing line number and the assertion text of the exception

    const int assertLineNumber = caughtException.lineNumber();
    if (assertLineNumber <= 0) {
        puts("Invalid assertion line number.");
        validArguments = false;
    }

    const char * const assertText = caughtException.expression();
    if (0 == assertText || 0 == *assertText) {
        puts("Null or empty assert text/expression.");
        validArguments = false;
    }

    //-------------------------------------------------------------------
    // Print error to 'stderror' in case the exception was not built well

    if (!validArguments) {
        printError(assertText, throwingFilename, assertLineNumber);
    }

    // 'testDriverFileName' may be 0/'nullptr', but not an empty string.
    if (testDriverFileName && testDriverFileName[0] == '\0') {
        puts("Empty test source file name.");
        // Return early here, no sense to parse an invalid file name
        return false;                                                 // RETURN
    }

    const ComponentName componentOfTestDriver(testDriverFileName);
    if (componentOfTestDriver.isError()) {
        printf("Bad component source file name for test driver in "
               "'bsls::AssertTest::catchProbe': \"%s\"\n",
               testDriverFileName);
        return false;                                                 // RETURN
    }

    //--------------------------------------------------------------------
    // All sanity-validations finished, return an error if any has failed.

    // We return here as the validation until now check for errors of more
    // Machiavelli, and rare kind (won't happen if the proper macros are used
    // and files are named properly).  The validation this conditional return
    // checks for more likely errors that would come from bugs in the tested
    // component, or its test driver.  For example bad expression, calling a
    // narrow-contract function in an asserted-expression and violate its
    // contract (so not the tested component throws) etc.

    if (!validArguments) {
        return false;                                                 // RETURN
    }

    // After diagnosing any invalid component-related arguments we can delegate
    // to catchProbeRaw to check for common issues.

    if (!catchProbeRaw(expectedResult,
                       checkLevel,
                       expectedLevel,
                       caughtException)) {
        return false;                                                 // RETURN
    }

    // Finally it makes sense if the exception comes from the component the
    // test driver belongs to.

    return areTheSameComponent(throwingComponent, componentOfTestDriver);
}

bool AssertTest::catchProbeRaw(char                       expectedResult,
                               bool                       checkLevel,
                               char                       expectedLevel,
                               const AssertTestException &caughtException)
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
        puts("Unexpected assertion failure.");
        return false;                                                 // RETURN
    }

#if defined(BSLS_ASSERTTEST_CAN_CHECK_LEVELS)
    if (checkLevel)
    {
        const char *level = caughtException.level();
        switch (expectedLevel)
        {
          case 'S':
            if (strcmp(level, Review::k_LEVEL_SAFE) != 0 &&
                strcmp(level, Assert::k_LEVEL_SAFE) != 0)
            {
                printf("Expected SAFE failure but got level:%s\n", level);
                return false;                                         // RETURN
            }
            break;
          case 'A':
            // if built in safe mode it's possible for a 'BSLS_ASSERT_SAFE' to
            // prevent execution from reaching a 'BSLS_ASSERT', so both levels
            // are allowed
            if (strcmp(level, Review::k_LEVEL_REVIEW) != 0 &&
                strcmp(level, Assert::k_LEVEL_ASSERT) != 0 &&
                strcmp(level, Review::k_LEVEL_SAFE) != 0 &&
                strcmp(level, Assert::k_LEVEL_SAFE) != 0)
            {
                printf("Expected ASSERT failure but got level:%s\n", level);
                return false;                                         // RETURN
            }
            break;
          case 'O':
            // if built in safe mode it's possible for a 'BSLS_ASSERT_SAFE' or
            // 'BSLS_ASSERT' to prevent execution from reaching a
            // 'BSLS_ASSERT_OPT', so all levels are allowed
            if (strcmp(level, Review::k_LEVEL_OPT) != 0 &&
                strcmp(level, Assert::k_LEVEL_OPT) != 0 &&
                strcmp(level, Review::k_LEVEL_REVIEW) != 0 &&
                strcmp(level, Assert::k_LEVEL_ASSERT) != 0 &&
                strcmp(level, Review::k_LEVEL_SAFE) != 0 &&
                strcmp(level, Assert::k_LEVEL_SAFE) != 0)
            {
                printf("Expected OPT failure but got level:%s\n", level);
                return false;                                         // RETURN
            }
            break;
        case 'I':
            // Any level that is enabled might prevent execution from reaching
            // a 'BSLS_ASSERT_INVOKE', so all levels are allowed
            break;
        }
    }
#else
    // Currently we are not able to extract level properly from a CPP20
    // contract violation.
    (void)checkLevel;
    (void)caughtException;
#endif // BSLS_ASSERTTEST_CAN_CHECK_LEVELS
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

BSLS_ANNOTATION_NORETURN
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
