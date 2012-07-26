// bsls_annotation.t.cpp                                              -*-C++-*-
#include <bsls_annotation.h>

#include <cstdlib>
#include <iostream>

// Uncomment next line to test annotations that can cause compiler warnings.
//#define BSLS_ANNOTATION_TRIGGER_OTHER

// Uncomment next line to test annotations that can cause compiler errors.
//#define BSLS_ANNOTATION_TRIGGER_ERROR

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// This test driver serves as a framework for manually checking the annotations
// (macros) defined in this component.  The tester must repeatedly rebuild this
// task using a compliant compiler, each time defining a different
// 'BSLS_ANNOTATION_TRIGGER_*' preprocessor variables (each undefined by
// default), and check the build output for the proper behavior (e.g., did the
// build succeed or not? was the expected warning observed? was a warning
// suppressed as expected?)  The single run-time "test" provided by this test
// driver, the BREATHING TEST, does nothing.
//
// The controlling preprocessor variables are:
//
//: o 'BSLS_ANNOTATION_TRIGGER_ERROR': if defined, use the
//:   'BSLS_ANNOTATION_ERROR(message)' annotation.  Note that the task should
//:   *not* build and the compiler output should show the specified 'message'.
//:
//:   o Maintenance note: This is the only test that causes compiler failure.
//:     If others are added, each will require an individual controlling
//:     preprocessor variable.
//:
//: o 'BSLS_ANNOTATION_TRIGGER_OTHER', if defined, use all the annotations
//:   defined in this component, except those expected to cause compile-time
//:   failure.
//
// For each annotation, 'BSLS_ANNOTATION_XXXX', we create a function named
// 'test_XXXX' to which annotation 'BSLS_ANNOTATION_XXXX' is applied.  For the
// two annotations that are also applicable to variables and tpes, we
// additionally create 'test_XXXX_variable' and 'test_XXXX_type'.  These
// entities are exercised in several ways:
//
//: o Some are just declared and, if appropriate, defined, with no other usage.
//:   For example, the 'BSLS_ANNOTATION_ALLOC_SIZE(x)' is a hint for compiler
//:   optimization; no compiler message expected.  Another example is
//:   'BSLS_ANNOTATION_UNUSED'.  For that annotation there must be no other
//:   usage to check if the usual compiler warning message is suppressed.
//:
//: o For other test functions, variables, and types, a function, variable, or
//:   type (as appropriated) named 'use_with_warning_message_XXXX' is defined
//:   such that a warning message should be generated.
//:
//: o Finally, for some 'use_with_warning_message_XXXX' entities, there is a
//:   corresponding 'use_without_diagnostic_message_XXXX' is defined to create
//:   a context where annotation 'BSLS_ANNOTATION_XXXX' must *not* result in a
//:   compiler message.
//
// The table below classifies each of the annotations provided by this
// component by the entities to which it can be applied (i.e., function,
// variable, and type) and the expected result (optimization, error, warning,
// conditional warning, absence of warning).  The tag(s) found in the right-most
// column appear as comments throughout this test driver.  They can be used
// as an aid to navigation to the test code for each annotation, and an aid
// to assuring test coverage.
//..
//  No  Annotation                            E Result     Tag
//  --  ------------------------------------  - --------   ----------
//   1  BSLS_ANNOTATION_ALLOC_SIZE(x)         F optim.      1fo
//   2  BSLS_ANNOTATION_ALLOC_SIZE_MUL(x, y)  F optim.      2fo
//   3  BSLS_ANNOTATION_ERROR("msg")          F error       3e
//   4  BSLS_ANNOTATION_WARNING("msg")        F warn        4w
//   5  BSLS_ANNOTATION_PRINTF(s, n)          F warn cond.  5fwy, 5fwn
//   6  BSLS_ANNOTATION_SCANF(s, n)           F warn cond.  6fwy, 6fwn
//   7  BSLS_ANNOTATION_FORMAT(n)             F warn cond.  7fwy, 7fwn
//   8  BSLS_ANNOTATION_ARG_NON_NULL(...)     F warn cond.  8fwy, 8fwn
//   9  BSLS_ANNOTATION_ARGS_NON_NULL         F warn cond.  9fwy, 9fwn
//  10  BSLS_ANNOTATION_NULL_TERMINATED       F warn cond. 10fwy,10fwn  TBD
//  11  BSLS_ANNOTATION_NULL_TERMINATED_AT(x) F warn cond. 11fwy,11fwn  TBD
//  12  BSLS_ANNOTATION_WARN_UNUSED_RESULT    F warn cond. 12fwy,12fwn
//  13  BSLS_ANNOTATION_DEPRECATED            F warn       13fw
//                                            V warn       13vw
//                                            T warn       13tw
//  14 BSLS_ANNOTATION_UNUSED                 F warn not   14fwn  TBD
//                                            T warn not   14vwn
//                                            T warn not   14twn
//..
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                  STANDARD BDE TEST STATUS APPARATUS
// ----------------------------------------------------------------------------

static int testStatus = 0;

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

void *test_ALLOC_SIZE(void *ptr, size_t size) BSLS_ANNOTATION_ALLOC_SIZE(2);
void *test_ALLOC_SIZE(void *ptr, size_t size)                         // { 1fo}
{
    return realloc(ptr, size);
}

void *test_ALLOC_SIZE_MUL(size_t count, size_t size)
                                          BSLS_ANNOTATION_ALLOC_SIZE_MUL(1, 2);
void *test_ALLOC_SIZE_MUL(size_t count, size_t size)                  // { 2fo}
{
    return calloc(count, size);
}

int test_ERROR() BSLS_ANNOTATION_ERROR
                               ("myExpectedError: Do not call 'test_WARNING'");
int test_ERROR()
{
    return 1;
}

int test_WARNING()
              BSLS_ANNOTATION_WARNING("myWarning: Do not call 'test_WARNING'");
int test_WARNING()
{
    return 1;
}

void test_PRINTF(const char *pattern, ...) BSLS_ANNOTATION_PRINTF(1, 2);
void test_PRINTF(const char * /*pattern*/, ...)
{
}

void test_SCANF(const char *, ...) BSLS_ANNOTATION_SCANF(1, 2);
void test_SCANF(const char *, ...)
{
}

const char *test_FORMAT(const char *locale, const char *format)
                                                     BSLS_ANNOTATION_FORMAT(2);
const char *test_FORMAT(const char *locale, const char *format)
{
    if (0 == strcmp(locale, "FR") && 0 == strcmp(format, "Name: %s")) {
        return "Nom: %s";
    }

    return "translateFormat: bad locale or format argument - no translation";
}

char test_ARG_NON_NULL(void *p) BSLS_ANNOTATION_ARG_NON_NULL(1);
char test_ARG_NON_NULL(void *p)
{
    char c = *reinterpret_cast<char *>(p);

    return c;
}

void test_ARGS_NON_NULL(void *, void *) BSLS_ANNOTATION_ARGS_NON_NULL;
void test_ARGS_NON_NULL(void *, void *)
{
}

void test_NULL_TERMINATED(void *, ...) BSLS_ANNOTATION_NULL_TERMINATED;
void test_NULL_TERMINATED(void *, ...)
{
}

void test_NULL_TERMINATED_AT(void *, ...)
                                         BSLS_ANNOTATION_NULL_TERMINATED_AT(2);
void test_NULL_TERMINATED_AT(void *, ...)
{
}

int test_WARN_UNUSED_RESULT() BSLS_ANNOTATION_WARN_UNUSED_RESULT;
int test_WARN_UNUSED_RESULT()
{
    return 1;
}

void test_DEPRECATED_function() BSLS_ANNOTATION_DEPRECATED;
void test_DEPRECATED_function()
{
}

static
void test_UNUSED_function() BSLS_ANNOTATION_UNUSED;
void test_UNUSED_function()
{
}

// ============================================================================
//                  DEFINITION OF ANNOTATED VARIABLES
// ----------------------------------------------------------------------------

int test_DEPRECATED_variable BSLS_ANNOTATION_DEPRECATED;

static
int test_UNUSED_variable     BSLS_ANNOTATION_UNUSED;

// ============================================================================
//                  DEFINITION OF ANNOTATED TYPES
// ----------------------------------------------------------------------------

struct test_DEPRECATED_type {
    int d_d;
} BSLS_ANNOTATION_DEPRECATED;

struct test_UNUSED_type {
    int d_d;
} BSLS_ANNOTATION_UNUSED;

// ============================================================================
//                  USAGE WITH NO EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

void use_without_diagnostic_message_PRINTF()                          // {5fwn}
{
    test_PRINTF("%s", "string");
    test_PRINTF("%d", 1);
    test_PRINTF("%f", 3.14159);
}

void use_without_diagnostic_message_SCANF()                          // { 6fwn}
{
    char   buffer[20];
    int    i;
    double d;

    test_SCANF("%s",  buffer);
    test_SCANF("%d",  &i);
    test_SCANF("%lf", &d);
}

void use_without_diagnostic_message_FORMAT()                         // { 7fwn}
{
    test_PRINTF(test_FORMAT("FR", "Name: %s"), "Michael Bloomberg");
}

void use_without_diagnostic_message_ARG_NON_NULL()                   // { 8fwn}
{
    char buffer[2];
    test_ARG_NON_NULL(buffer);
}

void use_without_diagnostic_message_ARGS_NON_NULL()                  // { 9fwn}
{
    char buffer1[2];
    char buffer2[2];

    test_ARGS_NON_NULL(buffer1, buffer2);
}

int use_without_diagnostic_message_WARN_UNUSED_RESULT()              // {12fwn}
{
    return test_WARN_UNUSED_RESULT();
}

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#ifdef BSLS_ANNOTATION_TRIGGER_OTHER

int use_with_warning_message_Warning = test_WARNING();                  // {4w}

void use_with_warning_message_PRINTF()                                // {5fwy}
{
    test_PRINTF("%s", 3.14159);
    test_PRINTF("%d", "string");
    test_PRINTF("%f", "other string");
}

void use_with_warning_message_SCANF()                                 // {6fwy}
{
    char   buffer[20];
    int    i;
    double d;

    test_SCANF("%s", &i);
    test_SCANF("%d", buffer);
    test_SCANF("%f", buffer);
}

void use_with_warning_message_FORMAT()                                // {7fwy}
{
    test_PRINTF(test_FORMAT("FR", "Name: %s"), 3);
}

void use_with_warning_message_ARG_NON_NULL()                          // {8fwy}
{
    test_ARG_NON_NULL(NULL);
}

void use_with_warning_message_ARGS_NON_NULL()                         // {9fwy}
{
    char buffer1[2];
    char buffer2[2];

    test_ARGS_NON_NULL(NULL, buffer2);
    test_ARGS_NON_NULL(buffer1, NULL);
    test_ARGS_NON_NULL(NULL, NULL);
}

void use_with_warning_message_WARN_UNUSED_RESULT()                   // {12fwy}
{
    test_WARN_UNUSED_RESULT();
}

void use_with_warning_message_DEPRECATED_function()                  // {13fw}
{
    test_DEPRECATED_function();
}

void use_with_warning_message_DEPRECATED_variable()                // {13v }
{
    test_DEPRECATED_variable;
}

test_DEPRECATED_type use_with_warning_message_DEPRECATED_type;      // {13t }


#endif

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER ERRORS
// ----------------------------------------------------------------------------

#ifdef BSLS_ANNOTATION_TRIGGER_ERROR

int use_with_error_message_Error = test_ERROR();                       // { 3e}

#endif

// ----------------------------------------------------------------------------

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int    test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 This test driver does *not* build when the
        //:   'BSLS_ANNOTATION_TRIGGER_ERROR' preprocessor variable is defined
        //:   and all expected output appears.
        //:
        //: 2 This test driver builds with all expected compiler warning
        //:   messages and no unexpected warnings when the
        //:   'BSLS_ANNOTATION_TRIGGER_OTHER' preprocessor variable is defined.
        //
        // Plan:
        //: 1 Build with 'BSLS_ANNOTATION_TRIGGER_ERROR' defined and externally
        //:   confirm that compilation of this task failed and the compiler
        //:   output shows the expected message.  (C-1)
        //:
        //: 2 Build with 'BSLS_ANNOTATION_TRIGGER_WARNINGS' defined and
        //:   externally examine compiler output for expected warnings and
        //:   the absence of warnings expected to be suppressed.  (C-2)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) {
            cout << "\nThere are no run-time tests for this component."
                    "\nRun build-time tests manually."
                    << endl;
        }

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
