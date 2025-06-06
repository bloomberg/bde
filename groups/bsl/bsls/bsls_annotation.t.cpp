// bsls_annotation.t.cpp                                              -*-C++-*-
#include <bsls_annotation.h>

#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>  // `calloc`, `realloc`, `atoi`
#include <string.h>  // `strcmp`

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
// `BSLS_ANNOTATION_TRIGGER_*` preprocessor variables (each undefined by
// default), and check the build output for the proper behavior.  In each case,
// the concerns are:
//
//  - Did the build succeed or not?
//
//  - Was the expected warning observed, or not?
//
//  - Was the expected suppression of some warning, suppressed or not?
//
//  - For annotations taking arguments, do the results show if the arguments
//    were properly passed to the underlying compiler directives?
//
// The single run-time "test" provided by this test driver, the BREATHING TEST,
// does nothing.
//
// The controlling preprocessor variables are:
//
//  - `BSLS_ANNOTATION_TRIGGER_ERROR`: if defined, use the
//    `BSLS_ANNOTATION_ERROR(message)` annotation.  Note that the task should
//    *not* build and the compiler output should show the specified `message`.
//
//    - Maintenance note: This is the only test that causes compiler failure.
//      If others are added, each will require an individual controlling
//      preprocessor variable.
//
//  - `BSLS_ANNOTATION_TRIGGER_OTHER`, if defined, use all the annotations
//    defined in this component, except those expected to cause compile-time
//    failure.
//
// For each annotation, `BSLS_ANNOTATION_XXXX`, we create a function named
// `test_XXXX` to which annotation `BSLS_ANNOTATION_XXXX` is applied.  For the
// two annotations that are also applicable to variables and types, we
// additionally create `test_XXXX_variable` and `test_XXXX_type`.  These
// entities are exercised in several ways:
//
//  - Some are just declared and, if appropriate, defined, with no other usage.
//    For example, the `BSLS_ANNOTATION_ALLOC_SIZE(x)` is a hint for compiler
//    optimization; no compiler message expected.  Another example is
//    `BSLS_ANNOTATION_UNUSED`.  For that annotation there must be no other
//    usage to check if the usual compiler warning message is suppressed.
//
//  - For other test functions, variables, and types, a function, variable, or
//    type (as appropriated) named `use_with_warning_message_XXXX` is defined
//    such that a warning message should be generated.
//
//  - Finally, for some `use_with_warning_message_XXXX` entities, there is a
//    corresponding `use_without_diagnostic_message_XXXX` is defined to create
//    a context where annotation `BSLS_ANNOTATION_XXXX` must *not* result in a
//    compiler message.
//
// The table below classifies each of the annotations provided by this
// component by the entities to which it can be applied (i.e., function,
// variable, and type) and the expected result (optimization, error, warning,
// conditional warning, absence of warning).  The tag(s) found in the
// right-most column appear as comments throughout this test driver.  They can
// be used as an aid to navigation to the test code for each annotation, and an
// aid to assuring test coverage.
// ```
//  No  Annotation                            E Result     Tag
//  --  ------------------------------------  - --------   ----------
//   1  BSLS_ANNOTATION_ALLOC_SIZE(x)         F optim.      1fo
//   2  BSLS_ANNOTATION_ALLOC_SIZE_MUL(x, y)  F optim.      2fo
//   3  BSLS_ANNOTATION_ERROR("msg")          F error       3fe
//   4  BSLS_ANNOTATION_WARNING("msg")        F warn        4fw
//   5  BSLS_ANNOTATION_PRINTF(s, n)          F warn cond.  5fwy, 5fwn
//   6  BSLS_ANNOTATION_SCANF(s, n)           F warn cond.  6fwy, 6fwn
//   7  BSLS_ANNOTATION_FORMAT(n)             F warn cond.  7fwy, 7fwn
//   8  BSLS_ANNOTATION_ARG_NON_NULL(...)     F warn cond.  8fwy, 8fwn
//   9  BSLS_ANNOTATION_ARGS_NON_NULL         F warn cond.  9fwy, 9fwn
//  10  BSLS_ANNOTATION_NULL_TERMINATED       F warn cond. 10fwy,10fwn
//  11  BSLS_ANNOTATION_NULL_TERMINATED_AT(x) F warn cond. 11fwy,11fwn
//  12  BSLS_ANNOTATION_WARN_UNUSED_RESULT    F warn cond. 12fwy,12fwn
//  13  BSLS_ANNOTATION_NODISCARD             F warn cond. 13fwy,13fwn
//  14  BSLS_ANNOTATION_DEPRECATED            F warn       14fw
//                                            V warn       14vw
//                                            T warn       14tw
//  15 BSLS_ANNOTATION_UNUSED                 F warn not   15fwn
//                                            V warn not   15vwn
//                                            T warn not   15twn
//  16 BSLS_ANNOTATION_FALLTHROUGH            F warn not   16fwn
//  17 BSLS_ANNOTATION_NORETURN               F error      17fe
//  18 BSLS_ANNOTATION_ANALYZER_NORETURN      F warn       18f
// ```
// Note that all annotations are defined as empty unless one is on a conforming
// compiler (e.g., `BSLS_PLATFORM_CMP_GNU` is defined).  Also note that there
// are minimum version requirements for several annotations; otherwise they are
// undefined.  Finally note that, if defined, the values for
// `BSLS_PLATFORM_CMP_GNU` and `BSLS_PLATFORM_CMP_VER_MAJOR` are output when
// the BREATHING TEST is run in verbose mode.
// ```
//  +--------------------------------+------------------------------------+
//  | Minimum                        |                                    |
//  | `BSLS_PLATFORM_CMP_VER_MAJOR` | Annotations                        |
//  +--------------------------------+------------------------------------+
//  |                         30100  |`BSLS_ANNOTATION_DEPRECATED`        |
//  +--------------------------------+------------------------------------+
//  |                         30300  |`BSLS_ANNOTATION_ARG_NON_NULL`      |
//  |                                |`BSLS_ANNOTATION_ARGS_NON_NULL`     |
//  +--------------------------------+------------------------------------+
//  |                         30400  |`BSLS_ANNOTATION_WARN_UNUSED_RESULT`|
//  +--------------------------------+------------------------------------+
//  |                         40000  |`BSLS_ANNOTATION_NULL_TERMINATED`   |
//  |                                |`BSLS_ANNOTATION_NULL_TERMINATED_AT`|
//  +--------------------------------+------------------------------------+
//  |                         40300  |`BSLS_ANNOTATION_ALLOC_SIZE`        |
//  |                                |`BSLS_ANNOTATION_ALLOC_SIZE_MUL`    |
//  +--------------------------------+------------------------------------+
// ```
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

#define STRINGIFY2(...) "" #__VA_ARGS__
#define STRINGIFY(a) STRINGIFY2(a)

// ============================================================================
//                  DECLARATION/DEFINITION OF ANNOTATED FUNCTIONS
// ----------------------------------------------------------------------------

void *test_ALLOC_SIZE(void *ptr, size_t size) BSLS_ANNOTATION_ALLOC_SIZE(2);
void *test_ALLOC_SIZE(void *ptr, size_t size)                        // { 1fo }
{
    return realloc(ptr, size);
}

void *test_ALLOC_SIZE_MUL(size_t count, size_t size)
                                          BSLS_ANNOTATION_ALLOC_SIZE_MUL(1, 2);
void *test_ALLOC_SIZE_MUL(size_t count, size_t size)                 // { 2fo }
{
    return calloc(count, size);
}

int test_ERROR() BSLS_ANNOTATION_ERROR
                               ("myExpectedError: Do not call `test_WARNING`");
int test_ERROR()
{
    return 1;
}

int test_WARNING()
              BSLS_ANNOTATION_WARNING("myWarning: Do not call `test_WARNING`");
int test_WARNING()
{
    return 1;
}

void test_PRINTF(const char *pattern, ...) BSLS_ANNOTATION_PRINTF(1, 2);
void test_PRINTF(const char *pattern, ...)
{
    (void) pattern;
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
        return "Nom: %s";                                             // RETURN
    }

    return "translateFormat: bad locale or format argument - no translation";
}

char test_ARG1_NON_NULL(void *p, void *q, void *r)
                                              BSLS_ANNOTATION_ARG_NON_NULL(1);
char test_ARG1_NON_NULL(void *p, void *q, void *r)
{
    char c = *reinterpret_cast<char *>(p);
    (void) q;
    (void) r;

    return c;
}

char test_ARG2_NON_NULL(void *p, void *q, void *r)
                                              BSLS_ANNOTATION_ARG_NON_NULL(2);
char test_ARG2_NON_NULL(void *p, void *q, void *r)
{
    (void) p;
    char c = *reinterpret_cast<char *>(q);
    (void) r;

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

void test_NULL_TERMINATED_AT2(void *, ...)
                                         BSLS_ANNOTATION_NULL_TERMINATED_AT(2);
void test_NULL_TERMINATED_AT2(void *, ...)
{
}

void test_NULL_TERMINATED_AT3(void *, ...)
                                         BSLS_ANNOTATION_NULL_TERMINATED_AT(3);
void test_NULL_TERMINATED_AT3(void *, ...)
{
}

BSLS_ANNOTATION_WARN_UNUSED_RESULT
int test_WARN_UNUSED_RESULT();
int test_WARN_UNUSED_RESULT()
{
    return 1;
}

BSLS_ANNOTATION_NODISCARD
int test_NODISCARD();
int test_NODISCARD()
{
    return 1;
}

void test_DEPRECATED_function() BSLS_ANNOTATION_DEPRECATED;
void test_DEPRECATED_function()
{
}

static
void test_UNUSED_function() BSLS_ANNOTATION_UNUSED;                  // {15fwn}
void test_UNUSED_function()
{
}

static
int test_FALLTHROUGH_function(int i);                                // {16fwn}
int test_FALLTHROUGH_function(int i)
{
    switch (i)
    {
      case 0:
        if (true) {
#if !defined(BSLS_PLATFORM_CMP_MSVC)
// Microsoft does not support this annotation placement.
            BSLS_ANNOTATION_FALLTHROUGH;
#endif
        }
        else {
            return 7;                                                 // RETURN
        }
      case 1:
        return 0;                                                     // RETURN
      default:
        return 1;                                                     // RETURN
    }
}

// ============================================================================
//                  DEFINITION OF ANNOTATED VARIABLES
// ----------------------------------------------------------------------------

int test_DEPRECATED_variable BSLS_ANNOTATION_DEPRECATED;

static
int test_UNUSED_variable     BSLS_ANNOTATION_UNUSED;                 // {15vwn}

// ============================================================================
//                  DEFINITION OF ANNOTATED TYPES
// ----------------------------------------------------------------------------

struct Test_DEPRECATED_type {
    int d_d;
} BSLS_ANNOTATION_DEPRECATED;

struct Test_UNUSED_type {
    int d_d;
} BSLS_ANNOTATION_UNUSED;

// ============================================================================
//                  USAGE WITH NO EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

void use_without_diagnostic_message_PRINTF()                         // { 5fwn}
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

void use_without_diagnostic_message_ARG1_NON_NULL()                  // { 8fwn}
{
    char buffer1[2];
    test_ARG1_NON_NULL(buffer1, NULL, NULL);
}

void use_without_diagnostic_message_ARG2_NON_NULL()
{
    char buffer2[2];
    test_ARG2_NON_NULL(NULL, buffer2, NULL);
}

void use_without_diagnostic_message_ARGS_NON_NULL()                  // { 9fwn}
{
    char buffer1[2];
    char buffer2[2];

    test_ARGS_NON_NULL(buffer1, buffer2);
}

void use_without_diagnostic_message_NULL_TERMINATED()                // {10fwn}
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    test_NULL_TERMINATED(buffer1, buffer2, buffer3, buffer4, NULL);
}

void use_without_diagnostic_message_NULL_TERMINATED_AT2()            // {11fwn}
{
    char buffer1[2];
    char buffer2[2];
    char buffer4[2];
    char buffer5[2];
    test_NULL_TERMINATED_AT2(buffer1, buffer2, NULL, buffer4, buffer5);
}

void use_without_diagnostic_message_NULL_TERMINATED_AT3()
{
    char buffer1[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULL_TERMINATED_AT3(buffer1, NULL, buffer3, buffer4, buffer5);
}

int use_without_diagnostic_message_WARN_UNUSED_RESULT()              // {12fwn}
{
    return test_WARN_UNUSED_RESULT();
}

int use_without_diagnostic_message_NODISCARD()                       // {13fwn}
{
    return test_NODISCARD();
}

int use_without_diagnostic_message_FALLTHROUGH()                    // {16fwwn}
{
    return test_FALLTHROUGH_function(17);
}

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#ifdef BSLS_ANNOTATION_TRIGGER_OTHER

int use_with_warning_message_Warning = test_WARNING();               // { 4fw }

void use_with_warning_message_PRINTF()                               // { 5fwy}
{
    test_PRINTF("%s", 3.14159);
    test_PRINTF("%d", "string");
    test_PRINTF("%f", "other string");
}

void use_with_warning_message_SCANF()                                // { 6fwy}
{
    char   buffer[20];
    int    i;
    double d;

    test_SCANF("%s", &i);
    test_SCANF("%d", buffer);
    test_SCANF("%f", buffer);
}

void use_with_warning_message_FORMAT()                               // { 7fwy}
{
    test_PRINTF(test_FORMAT("FR", "Name: %s"), 3);
}

void use_with_warning_message_ARG1_NON_NULL()                        // { 8fwy}
{
    test_ARG1_NON_NULL(NULL, NULL, NULL);
}

void use_with_warning_message_ARG2_NON_NULL()
{
    test_ARG2_NON_NULL(NULL, NULL, NULL);
}

void use_with_warning_message_ARGS_NON_NULL()                        // { 9fwy}
{
    char buffer1[2];
    char buffer2[2];

    test_ARGS_NON_NULL(NULL, buffer2);
    test_ARGS_NON_NULL(buffer1, NULL);
    test_ARGS_NON_NULL(NULL, NULL);
}

void use_with_warning_message_NULL_TERMINATED()                      // {10fwy}
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULL_TERMINATED(buffer1, buffer2, buffer3, buffer4, buffer5);
}

void use_with_warning_message_NULL_TERMINATED_AT2()                  // {11fwy}
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULL_TERMINATED_AT2(buffer1, buffer2, buffer3, buffer4, buffer5);
}
void use_with_warning_message_NULL_TERMINATED_AT3()
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULL_TERMINATED_AT3(buffer1, buffer2, buffer3, buffer4, buffer5);
}

void use_with_warning_message_WARN_UNUSED_RESULT()                   // {12fwy}
{
    test_WARN_UNUSED_RESULT();
}

void use_with_warning_message_NODISCARD()                            // {13fwy}
{
    test_NODISCARD();
}

void use_with_warning_message_DEPRECATED_function()                  // {14fw }
{
    test_DEPRECATED_function();
}

void use_with_warning_message_DEPRECATED_variable()                  // {14vw }
{
    test_DEPRECATED_variable;
}

int use_with_warning_message_DEPRECATED_type()                       // {14tw }
{
    Test_DEPRECATED_type instance_of_DEPRECATED_TYPE;
    instance_of_DEPRECATED_TYPE.d_d = 0;
    return instance_of_DEPRECATED_TYPE.d_d;
}

#endif

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER ERRORS
// ----------------------------------------------------------------------------

#ifdef BSLS_ANNOTATION_TRIGGER_ERROR

int use_with_error_message_Error = test_ERROR();                     // { 3fe }

static
BSLS_ANNOTATION_NORETURN void test_NORETURN_function()                // {17fe}
{
}

#endif

// ----------------------------------------------------------------------------

using namespace BloombergLP;
namespace std {}
using namespace std;

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

/// Print a diagnostic message to standard output if any of the preprocessor
/// flags of interest are defined, and their value if a value had been set.
/// An "Enter" and "Leave" message is printed unconditionally so there is
/// some report even if all of the flags are undefined.
static void printFlags()
{
    printf("printFlags: Enter\n");

    printf("\n  printFlags: bsls_annotation Macros\n");

    printf("\n  BSLS_ANNOTATION_ALLOC_SIZE(x): ");
#ifdef BSLS_ANNOTATION_ALLOC_SIZE
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_ALLOC_SIZE(x)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_ALLOC_SIZE_MUL(x, y): ");
#ifdef BSLS_ANNOTATION_ALLOC_SIZE_MUL
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_ALLOC_SIZE_MUL(x, y)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_ARGS_NON_NULL: ");
#ifdef BSLS_ANNOTATION_ARGS_NON_NULL
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_ARGS_NON_NULL) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_ARG_NON_NULL(...): ");
#ifdef BSLS_ANNOTATION_ARG_NON_NULL
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_ARG_NON_NULL(...)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_DEPRECATED: ");
#ifdef BSLS_ANNOTATION_DEPRECATED
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_DEPRECATED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_ERROR(x): ");
#ifdef BSLS_ANNOTATION_ERROR
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_ERROR(x)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_FALLTHROUGH: ");
#ifdef BSLS_ANNOTATION_FALLTHROUGH
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_FALLTHROUGH) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_FORMAT(arg): ");
#ifdef BSLS_ANNOTATION_FORMAT
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_FORMAT(arg)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_NODISCARD: ");
#ifdef BSLS_ANNOTATION_NODISCARD
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_NODISCARD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_NORETURN: ");
#ifdef BSLS_ANNOTATION_NORETURN
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_NORETURN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_NULL_TERMINATED: ");
#ifdef BSLS_ANNOTATION_NULL_TERMINATED
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_NULL_TERMINATED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_NULL_TERMINATED_AT(x): ");
#ifdef BSLS_ANNOTATION_NULL_TERMINATED_AT
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_NULL_TERMINATED_AT(x)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_PRINTF(fmt, arg): ");
#ifdef BSLS_ANNOTATION_PRINTF
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_PRINTF(fmt, arg)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_SCANF(fmt, arg): ");
#ifdef BSLS_ANNOTATION_SCANF
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_SCANF(fmt, arg)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_UNUSED: ");
#ifdef BSLS_ANNOTATION_UNUSED
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_UNUSED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_USED: ");
#ifdef BSLS_ANNOTATION_USED
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_USED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_WARNING(x): ");
#ifdef BSLS_ANNOTATION_WARNING
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_WARNING(x)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_ANNOTATION_WARN_UNUSED_RESULT: ");
#ifdef BSLS_ANNOTATION_WARN_UNUSED_RESULT
    printf("%s\n", STRINGIFY(BSLS_ANNOTATION_WARN_UNUSED_RESULT) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n  printFlags: bsls_annotation Referenced Macros\n");

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
    printf("%s\n",
              STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
    printf("%s\n",
             STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
    printf("%s\n",
                STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
    printf("%s\n",
                 STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_CLANG: ");
#ifdef BSLS_PLATFORM_CMP_CLANG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_CLANG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_GNU: ");
#ifdef BSLS_PLATFORM_CMP_GNU
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_GNU) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_HP: ");
#ifdef BSLS_PLATFORM_CMP_HP
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_HP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_IBM: ");
#ifdef BSLS_PLATFORM_CMP_IBM
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_IBM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  BSLS_PLATFORM_CMP_MSVC: ");
#ifdef BSLS_PLATFORM_CMP_MSVC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_MSVC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n  __has_warning: ");
#ifdef __has_warning
    printf("DEFINED\n");
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\nprintFlags: Leave\n");
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;

    (void)        veryVerbose;  // unused variable warning
    (void)    veryVeryVerbose;  // unused variable warning

    printf( "TEST %s CASE %d\n", __FILE__, test);

    if (veryVeryVerbose) {
        printFlags();
    }

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        // 1. This test driver does *not* build when the
        //    `BSLS_ANNOTATION_TRIGGER_ERROR` preprocessor variable is defined
        //    and all expected output appears.
        //
        // 2. This test driver builds with all expected compiler warning
        //    messages and no unexpected warnings when the
        //    `BSLS_ANNOTATION_TRIGGER_OTHER` preprocessor variable is defined.
        //
        // Plan:
        // 1. Build with `BSLS_ANNOTATION_TRIGGER_ERROR` defined and externally
        //    confirm that compilation of this task failed and the compiler
        //    output shows the expected message.  (C-1)
        //
        // 2. Build with `BSLS_ANNOTATION_TRIGGER_WARNINGS` defined and
        //    externally examine compiler output for expected warnings and the
        //    absence of warnings expected to be suppressed.  (C-2)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) {
            printf("\nThere are no run-time tests for this component."
                   "\nManually run build-time tests using a conforming "
                   "compiler.");

            if (!veryVeryVerbose) printFlags();

            ASSERT(true); // remove unused warning for `aSsErT`
        }

      } break;
      default: {
        fprintf( stderr, "WARNING: CASE `%d` NOT FOUND.\n" , test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf( stderr, "Error, non-zero test status = %d.\n", testStatus );
    }

    return testStatus;
}

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
