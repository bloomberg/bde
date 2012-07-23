// bsls_annotation.t.cpp                                              -*-C++-*-

// *** The format of this component test driver is non-standard. ***

#include <bsls_annotation.h>

// Check include guard integrity.
#include <bsls_annotation.h>

#ifndef INCLUDED_BSLS_ANNOTATION
#error Include guard INCLUDED_BSLS_ANNOTATION misspelled.
#endif

#include <cstdlib>
#include <iostream>

// Define 'BSLS_ANNOTATION_TRIGGER_WARNINGS' to trigger warning messages during
// compilation.
#define BSLS_ANNOTATION_TRIGGER_WARNINGS

// Define 'BSLS_ANNOTATION_TRIGGER_ERRORS' to trigger errors -- on a
// compliant compiler, *this will result in compilation errors* and compilation
// failure.
//#define BSLS_ANNOTATION_TRIGGER_ERRORS

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// There is no easy way to do runtime checks of this component.  Instead, we
// will make declarations using every compiler annotation at compile time.
// We will use these declared functions, objects, or types in ways that would
// cause warnings on a compliant compiler if 'BSLS_ANNOTATION_TRIGGER_WARNINGS'
// is defined, and in ways that would cause compile-time errors if
// 'BSLS_ANNOTATION_TRIGGER_ERRORS' is defined.
//
// For each annotation, 'BSLS_ANNOTATION_XXXXX', we will create up to 3 kinds
// of functions, variables, and/or types.
//
//: o A 'test_XXXXX' function, 'test_XXXXX_variable' variable, and/or
//:   'test_XXXXX_type' type.
//:   o This set of declarations uses the 'BSLS_ANNOTATION_XXXXX' macro, and
//:     should not trigger any diagnostics.
//:
//: o A function called 'use_without_diagnostic_message_XXXXX'.
//:   o As the name states, even on a conforming compiler this function uses
//:     the corresponding 'test_XXXXX_*' name in a way that should not cause
//:     diagnostics.
//:
//: o A function or variable called 'use_with_warning_message_XXXXX' (if
//:   'BSLS_ANNOTATION_TRIGGER_WARNINGS' is defined).
//:   o On a conforming compiler, this will cause a warning to be issued by the
//:     compiler, since the use violates the annotation.
//:
//: o A function or variable called 'use_with_error_message_XXXXX' (if
//:   'BSLS_ANNOTATION_TRIGGER_ERRORS' is defined).
//:   o On a conforming compiler, this will cause an error to be issued by the
//:     compiler, since the use violates the annotation.  This will also cause
//:     the compilation to fail (but, paradoxically, the test to succeed).
// ----------------------------------------------------------------------------
// [ 1] Breathing Test (nothing)
//-----------------------------------------------------------------------------

//==========================================================================
//                  STANDARD BDE TEST STATUS APPARATUS
//--------------------------------------------------------------------------

static int testStatus = 0;

//==========================================================================
//                  TEST COMPILER ANNOTATIONS
//--------------------------------------------------------------------------

void *test_ALLOC_SIZE(void *ptr, size_t size) BSLS_ANNOTATION_ALLOC_SIZE(2);
void *test_ALLOC_SIZE(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void *test_ALLOC_SIZE_MUL(size_t count, size_t size)
                                          BSLS_ANNOTATION_ALLOC_SIZE_MUL(1, 2);
void *test_ALLOC_SIZE_MUL(size_t count, size_t size)
{
    return calloc(count, size);
}

int test_ERROR() BSLS_ANNOTATION_ERROR("Do not call 'test_ERROR'");
int test_ERROR()
{
    return 1;
}

int test_WARNING() BSLS_ANNOTATION_WARNING("Do not call 'test_WARNING'");
int test_WARNING()
{
    return 1;
}

void test_PRINTF(const char *pattern, ...) BSLS_ANNOTATION_PRINTF(1, 2);
void test_PRINTF(const char * /*pattern*/, ...)
{
}

void use_without_diagnostic_message_PRINTF()
{
    test_PRINTF("%s", "string");
    test_PRINTF("%d", 1);
    test_PRINTF("%f", 3.14159);
}

void test_SCANF(const char *, ...) BSLS_ANNOTATION_SCANF(1, 2);
void test_SCANF(const char *, ...)
{
}

void use_without_diagnostic_message_SCANF()
{
    char   buffer[20];
    int    i;
    double d;

    test_SCANF("%s", buffer);
    test_SCANF("%d", &i);
    test_SCANF("%lf", &d);
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

void use_without_diagnostic_message_FORMAT()
{
    test_PRINTF(test_FORMAT("FR", "Name: %s"), "Michael Bloomberg");
}

char test_ARG_NON_NULL(void *p) BSLS_ANNOTATION_ARG_NON_NULL(1);
char test_ARG_NON_NULL(void *p)
{
    char c = *reinterpret_cast<char *>(p);

    return c;
}

void use_without_diagnostic_message_ARG_NON_NULL()
{
    char buffer[2];

    test_ARG_NON_NULL(buffer);
}

void test_ARGS_NON_NULL(void *, void *) BSLS_ANNOTATION_ARGS_NON_NULL;
void test_ARGS_NON_NULL(void *, void *)
{
}

void use_without_diagnostic_message_ARGS_NON_NULL()
{
    char buffer1[2];
    char buffer2[2];

    test_ARGS_NON_NULL(buffer1, buffer2);
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

int use_without_diagnostic_message_WARN_UNUSED_RESULT()
{
    return test_WARN_UNUSED_RESULT();
}

void test_DEPRECATED_fn() BSLS_ANNOTATION_DEPRECATED;
void test_DEPRECATED_fn()
{
}

struct test_DEPRECATED_type {
    int d_d;
} BSLS_ANNOTATION_DEPRECATED;

int test_DEPRECATED_variable BSLS_ANNOTATION_DEPRECATED;

int test_UNUSED_variable BSLS_ANNOTATION_UNUSED;

#ifdef BSLS_ANNOTATION_TRIGGER_WARNINGS

int use_with_warning_message_Warning = test_WARNING();

void use_with_warning_message_PRINTF()
{
    test_PRINTF("%s", 3.14159);
    test_PRINTF("%d", "string");
    test_PRINTF("%f", "other string");
}

void use_with_warning_message_SCANF()
{
    char buffer[20];
    int i;
    double d;

    test_SCANF("%s", &i);
    test_SCANF("%d", buffer);
    test_SCANF("%f", buffer);
}

void use_with_warning_message_FORMAT()
{
    test_PRINTF(test_FORMAT("FR", "Name: %s"), 3);
}

void use_with_warning_message_ARG_NON_NULL()
{
    test_ARG_NON_NULL(NULL);
}

void use_with_warning_message_ARGS_NON_NULL()
{
    char buffer1[2];
    char buffer2[2];

    test_ARGS_NON_NULL(NULL, buffer2);
    test_ARGS_NON_NULL(buffer1, NULL);
    test_ARGS_NON_NULL(NULL, NULL);
}

void use_with_warning_message_WARN_UNUSED_RESULT()
{
    test_WARN_UNUSED_RESULT();
}

void use_with_warning_message_DEPRECATED_fn()
{
    test_DEPRECATED_fn();
}

test_DEPRECATED_type use_with_warning_message_DEPRECATED_type;

void use_with_warning_message_DEPRECTATED_variable()
{
    test_DEPRECATED_variable;
}

#endif

#ifdef BSLS_ANNOTATION_TRIGGER_ERRORS

int use_with_error_message_Error = test_ERROR();

#endif

using namespace BloombergLP;
using namespace std;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int            test = argc > 1 ? std::atoi(argv[1]) : 0;
    int         verbose = argc > 2;

    std::cout << "TEST " << __FILE__ << " CASE " << test << std::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: That there is no actual runtime test for this component.
        //
        // Plan:
        //   Do nothing at run-time.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) {
            cout << "\nThere is no run-time test for this component" << endl;
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
