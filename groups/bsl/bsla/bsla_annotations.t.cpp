// bsla_annotations.t.cpp                                             -*-C++-*-
#include <bsla_annotations.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stdio.h>
#include <stdlib.h>  // 'calloc', 'realloc', 'atoi'
#include <string.h>  // 'strcmp'

// Set this preprocessor variable to 1 to enable compile warnings being
// generated, 0 to disable them.

#define U_TRIGGER_WARNINGS 0

// Set this preprocessor variable to 1 to enable compile errors being
// generated, 0 to disable them.

#define U_TRIGGER_ERRORS 0

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// This test driver serves as a framework for manually checking the annotations
// (macros) defined in this component.  The tester must repeatedly rebuild this
// task using a compliant compiler, each time defining different values of the
// boolean 'U_TRIGGER_WARNINGS' and 'U_TRIGGER_ERRORS' preprocessor variables.
// In each case, the concerns are:
//
//: o Did the build succeed or not?
//:
//: o Was the expected warning observed, or not?
//:
//: o Was the expected suppression of some warning, suppressed or not?
//:
//: o For annotations taking arguments, do the results show if the arguments
//:   were properly passed to the underlying compiler directives?
//
// The single run-time "test" provided by this test driver, the BREATHING TEST,
// does nothing.
//
// The controlling preprocessor variables are:
//
//: o 'U_TRIGGER_ERRORS': if defined, use the 'BSLA_ERROR(message)' annotation.
//:   Note that the task should *not* build and the compiler output should show
//:   the specified 'message'.
//:
//:   o Maintenance note: This is the only test that causes compiler failure.
//:     If others are added, each will require an individual controlling
//:     preprocessor variable.
//:
//: o 'U_TRIGGER_WARNINGS', if defined, use all the annotations defined in this
//:   component, except those expected to cause compile-time failure.
//
// For each annotation, 'BSLA_XXXX', we create a function named 'test_XXXX' to
// which annotation 'BSLA_XXXX' is applied.  For the two annotations that are
// also applicable to variables and types, we additionally create
// 'test_XXXX_variable' and 'test_XXXX_type'.  These entities are exercised in
// several ways:
//
//: o Some are just declared and, if appropriate, defined, with no other usage.
//:   For example, the 'BSLA_ALLOCSIZE(x)' is a hint for compiler optimization;
//:   no compiler message expected.  Another example is 'BSLA_UNUSED'.  For
//:   that annotation there must be no other usage to check if the usual
//:   compiler warning message is suppressed.
//:
//: o For other test functions, variables, and types, a function, variable, or
//:   type (as appropriated) named 'use_with_warning_message_XXXX' is defined
//:   such that a warning message should be generated.
//:
//: o Finally, for some 'use_with_warning_message_XXXX' entities, there is a
//:   corresponding 'use_without_diagnostic_message_XXXX' is defined to create
//:   a context where annotation 'BSLA_XXXX' must *not* result in a compiler
//:   message.
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ----------------------------------------------------------------------------

namespace bsls = BloombergLP::bsls;

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

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

void *test_ALLOCSIZE(void *ptr, size_t size) BSLA_ALLOCSIZE(2);
    // Test the 'BSLA_ALLOCSIZE' macro.  Allocate a new block of memory of the
    // specified 'size' and copy the minimum of 'size' and the size of the
    // preexisting block at the specfied 'ptr' from 'ptr' to the new block,
    // free the block at 'ptr', and return a pointer to the new block.  The
    // 'BSLA_ALLOCSIZE' macro provides a hint to some compilers as to the size
    // of the returned block.

void *test_ALLOCSIZE(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void *test_ALLOCSIZEMUL(size_t count, size_t size) BSLA_ALLOCSIZEMUL(1, 2);
    // Test the 'BSLA_ALLOCSIZE' macro.  Allocate and return a new block of
    // memory whose size is the product of the specified 'size' and 'count'.
    // The 'BSLA_ALLOCSIZEMUL' macro provides a hint to some compilers as to
    // the size of the returned block.

void *test_ALLOCSIZEMUL(size_t count, size_t size)
{
    return calloc(count, size);
}

char test_ARG1_NON_NULL(void *p, void *q, void *r) BSLA_NONNULLARG(1);
char test_ARG1_NON_NULL(void *p, void *q, void *r)
{
    char c = *reinterpret_cast<char *>(p);
    (void) q;
    (void) r;

    return c;
}

char test_ARG2_NON_NULL(void *p, void *q, void *r) BSLA_NONNULLARG(2, 3);
char test_ARG2_NON_NULL(void *p, void *q, void *r)
{
    (void) p;
    (void) q;
    (void) r;

    return 'a';
}

void test_ARGS_NON_NULL(void *, void *) BSLA_NONNULLARGS;
void test_ARGS_NON_NULL(void *, void *)
{
}

void test_DEPRECATED_function() BSLA_DEPRECATED;
void test_DEPRECATED_function()
{
}

int test_ERROR() BSLA_ERROR("myError: Do not call 'test_ERROR'");
int test_ERROR()
{
    return 1;
}

static
int test_FALLTHROUGH_function(int i);
int test_FALLTHROUGH_function(int i)
{
    switch (i)
    {
      case 0: {
        if (true) {
#if BSLA_FALLTHROUGH_IS_ACTIVE
            BSLA_FALLTHROUGH;
#else
            return 3;                                                 // RETURN
#endif
        }
        else {
            return 7;                                                 // RETURN
        }
      }
      case 1: {
        return 0;                                                     // RETURN
      }
      default: {
        return 1;                                                     // RETURN
      }
    }
}

const char *test_FORMAT(const char *locale, const char *format)
                                                     BSLA_FORMAT(2);
const char *test_FORMAT(const char *locale, const char *format)
{
    if (0 == strcmp(locale, "FR") && 0 == strcmp(format, "Name: %s")) {
        return "Nom: %s";                                             // RETURN
    }

    return "translateFormat: bad locale or format argument - no translation";
}

int test_NODISCARD() BSLA_NODISCARD;
int test_NODISCARD()
{
    return 1;
}

void test_NULLTERMINATED(void *, ...) BSLA_NULLTERMINATED;
void test_NULLTERMINATED(void *, ...)
{
}

void test_NULLTERMINATEDAT2(void *, ...)
                                         BSLA_NULLTERMINATEDAT(2);
void test_NULLTERMINATEDAT2(void *, ...)
{
}

void test_NULLTERMINATEDAT3(void *, ...)
                                         BSLA_NULLTERMINATEDAT(3);
void test_NULLTERMINATEDAT3(void *, ...)
{
}

void test_PRINTF(const char *pattern, ...) BSLA_PRINTF(1, 2);
void test_PRINTF(const char *pattern, ...)
{
    (void) pattern;
}

void test_SCANF(const char *, ...) BSLA_SCANF(1, 2);
void test_SCANF(const char *, ...)
{
}

static
void test_UNUSED_function_no_warning() BSLA_UNUSED;
void test_UNUSED_function_no_warning()
{
}

static
void test_UNUSED_function_warning();
void test_UNUSED_function_warning()
{
}

int test_WARNING() BSLA_WARNING("myWarning: Do not call 'test_WARNING'");
int test_WARNING()
{
    return 1;
}

// ============================================================================
//                  DEFINITION OF ANNOTATED VARIABLES
// ----------------------------------------------------------------------------

int test_DEPRECATED_variable BSLA_DEPRECATED;

static
int test_UNUSED_variable_no_warning     BSLA_UNUSED;

static
int test_UNUSED_variable_warning;

#if BSLA_USED_IS_ACTIVE

static
int test_USED_variable_no_warning BSLA_USED;

#endif

// ============================================================================
//                  DEFINITION OF ANNOTATED TYPES
// ----------------------------------------------------------------------------

struct Test_DEPRECATED_type {
    int d_d;
} BSLA_DEPRECATED;

namespace {

struct Test_UNUSED_type_no_warning {
    int d_d;
} BSLA_UNUSED;

struct Test_UNUSED_type_warning {
    int d_d;
};

}  // close unnamed namespace

// ============================================================================
//                  USAGE WITH NO EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

void use_without_diagnostic_message_ARG1_NON_NULL()
{
    char buffer1[2];
    test_ARG1_NON_NULL(buffer1, NULL, NULL);
}

void use_without_diagnostic_message_ARG2_NON_NULL()
{
    char buffer2[2] = { 'a', 0 };
    char ret = test_ARG2_NON_NULL(NULL, buffer2, buffer2 + 1);
    ASSERT('a' == ret);
}

void use_without_diagnostic_message_ARGS_NON_NULL()
{
    char buffer1[2];
    char buffer2[2];

    test_ARGS_NON_NULL(buffer1, buffer2);
}

int use_without_diagnostic_message_FALLTHROUGH()
{
    return test_FALLTHROUGH_function(17);
}

void use_without_diagnostic_message_FORMAT()
{
    test_PRINTF(test_FORMAT("FR", "Name: %s"), "Michael Bloomberg");
}

int use_without_diagnostic_message_NODISCARD()
{
    return test_NODISCARD();
}

void use_without_diagnostic_message_NULLTERMINATED()
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    test_NULLTERMINATED(buffer1, buffer2, buffer3, buffer4, NULL);
}

void use_without_diagnostic_message_NULLTERMINATEDAT2()
{
    char buffer1[2];
    char buffer2[2];
    char buffer4[2];
    char buffer5[2];
    test_NULLTERMINATEDAT2(buffer1, buffer2, NULL, buffer4, buffer5);
}

void use_without_diagnostic_message_NULLTERMINATEDAT3()
{
    char buffer1[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULLTERMINATEDAT3(buffer1, NULL, buffer3, buffer4, buffer5);
}

BSLA_NORETURN void use_without_diagnostic_message_NORETURN()
{
    ::exit(1);
}

void use_without_diagnostic_message_PRINTF()
{
    test_PRINTF("%s", "string");
    test_PRINTF("%d", 1);
    test_PRINTF("%f", 3.14159);
}

void use_without_diagnostic_message_SCANF()
{
    char   buffer[20];
    int    i;
    double d;

    test_SCANF("%s",  buffer);
    test_SCANF("%d",  &i);
    test_SCANF("%lf", &d);
}

#if !U_TRIGGER_WARNINGS

void use_without_diagnostic_message_UNUSED()
{
    (void) Test_UNUSED_type_warning();

    (void) test_UNUSED_variable_warning;
    (void) test_UNUSED_function_warning();

# if !BSLA_UNUSED_IS_ACTIVE

    (void) Test_UNUSED_type_no_warning();

    (void) test_UNUSED_variable_no_warning;
    (void) test_UNUSED_function_no_warning();

# endif
}

#endif

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if U_TRIGGER_WARNINGS

void use_with_warning_message_ARG1_NON_NULL()
{
    test_ARG1_NON_NULL(NULL, NULL, NULL);
}

void use_with_warning_message_ARG2_NON_NULL()
{
    test_ARG2_NON_NULL(NULL, NULL, NULL);
}

void use_with_warning_message_ARGS_NON_NULL()
{
    char buffer1[2];
    char buffer2[2];

    test_ARGS_NON_NULL(NULL, buffer2);
    test_ARGS_NON_NULL(buffer1, NULL);
    test_ARGS_NON_NULL(NULL, NULL);
}

void use_with_warning_message_DEPRECATED_function()
{
    test_DEPRECATED_function();
}

int use_with_warning_message_DEPRECATED_type()
{
    Test_DEPRECATED_type instance_of_DEPRECATED_TYPE;
    instance_of_DEPRECATED_TYPE.d_d = 0;
    return instance_of_DEPRECATED_TYPE.d_d;
}

void use_with_warning_message_DEPRECATED_variable()
{
    (void) test_DEPRECATED_variable;
}

int use_with_warning_message_FALLTHROUGH(int i)
{
    switch (i)
    {
      case 0: {
        return 0;                                                     // RETURN
      }
#if U_TRIGGER_WARNINGS
      case 1: {
        if (true) {
            (void) i;
        }
        else {
            return 7;                                                 // RETURN
        }
      }
#endif
      default: {
        return 1;                                                     // RETURN
      }
    }
}

void use_with_warning_message_FORMAT()
{
    test_PRINTF(test_FORMAT("FR", "Name: %s"), 3);
}

void use_with_warning_message_NODISCARD()
{
    test_NODISCARD();
}

void use_with_warning_message_NORETURN()
{
}


BSLA_NORETURN void use_with_error_message_NORETURN_function()
{
}

void use_with_warning_message_NULLTERMINATED()
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULLTERMINATED(buffer1, buffer2, buffer3, buffer4, buffer5);
}

void use_with_warning_message_NULLTERMINATEDAT2()
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULLTERMINATEDAT2(buffer1, buffer2, buffer3, buffer4, buffer5);
}
void use_with_warning_message_NULLTERMINATEDAT3()
{
    char buffer1[2];
    char buffer2[2];
    char buffer3[2];
    char buffer4[2];
    char buffer5[2];
    test_NULLTERMINATEDAT3(buffer1, buffer2, buffer3, buffer4, buffer5);
}

void use_with_warning_message_PRINTF()
{
    test_PRINTF("%s", 3.14159);
    test_PRINTF("%d", "string");
    test_PRINTF("%f", "other string");
}

void use_with_warning_message_SCANF()
{
    char   buffer[20];
    int    i;

    test_SCANF("%s", &i);
    test_SCANF("%d", buffer);
    test_SCANF("%f", buffer);
}

static
int test_UNUSED_variable_warning;

void use_with_warning_message_WARNING()
{
    (void) test_WARNING();
}

#endif

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER ERRORS
// ----------------------------------------------------------------------------

#if U_TRIGGER_ERRORS

void use_with_error_message_Error()
{
    (void) test_ERROR();
}

#endif

// ============================================================================
//                              HELPER FUNCTIONS
// ----------------------------------------------------------------------------

static void printFlags()
    // Print a diagnostic message to standard output if any of the preprocessor
    // flags of interest are defined, and their value if a value had been set.
    // An "Enter" and "Leave" message is printed unconditionally so there is
    // some report even if all of the flags are undefined.
{
    printf("printFlags: Enter\n");

    printf("\nprintFlags: bsls_annotation Macros\n");

    printf("\nBSLA_ALLOCSIZE(x): ");
#ifdef BSLA_ALLOCSIZE
    printf("%s\n", STRINGIFY(BSLA_ALLOCSIZE(x)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_ALLOCSIZEMUL(x, y): ");
#ifdef BSLA_ALLOCSIZEMUL
    printf("%s\n", STRINGIFY(BSLA_ALLOCSIZEMUL(x, y)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_ARGS_NON_NULL: ");
#ifdef BSLA_ARGS_NON_NULL
    printf("%s\n", STRINGIFY(BSLA_ARGS_NON_NULL) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_ARG_NON_NULL(...): ");
#ifdef BSLA_ARG_NON_NULL
    printf("%s\n", STRINGIFY(BSLA_ARG_NON_NULL(...)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_DEPRECATED: ");
#ifdef BSLA_DEPRECATED
    printf("%s\n", STRINGIFY(BSLA_DEPRECATED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_ERROR(x): ");
#ifdef BSLA_ERROR
    printf("%s\n", STRINGIFY(BSLA_ERROR(x)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_FALLTHROUGH: ");
#ifdef BSLA_FALLTHROUGH
    printf("%s\n", STRINGIFY(BSLA_FALLTHROUGH) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_FORMAT(arg): ");
#ifdef BSLA_FORMAT
    printf("%s\n", STRINGIFY(BSLA_FORMAT(arg)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_NODISCARD: ");
#ifdef BSLA_NODISCARD
    printf("%s\n", STRINGIFY(BSLA_NODISCARD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_NORETURN: ");
#ifdef BSLA_NORETURN
    printf("%s\n", STRINGIFY(BSLA_NORETURN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_NULLTERMINATED: ");
#ifdef BSLA_NULLTERMINATED
    printf("%s\n", STRINGIFY(BSLA_NULLTERMINATED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_NULLTERMINATEDAT(x): ");
#ifdef BSLA_NULLTERMINATEDAT
    printf("%s\n", STRINGIFY(BSLA_NULLTERMINATEDAT(x)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_PRINTF(fmt, arg): ");
#ifdef BSLA_PRINTF
    printf("%s\n", STRINGIFY(BSLA_PRINTF(fmt, arg)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_SCANF(fmt, arg): ");
#ifdef BSLA_SCANF
    printf("%s\n", STRINGIFY(BSLA_SCANF(fmt, arg)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_UNUSED: ");
#ifdef BSLA_UNUSED
    printf("%s\n", STRINGIFY(BSLA_UNUSED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_USED: ");
#ifdef BSLA_USED
    printf("%s\n", STRINGIFY(BSLA_USED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLA_WARNING(x): ");
#ifdef BSLA_WARNING
    printf("%s\n", STRINGIFY(BSLA_WARNING(x)) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_ALLOCSIZE_IS_ACTIVE);
    P(BSLA_ALLOCSIZEMUL_IS_ACTIVE);
    P(BSLA_DEPRECATED_IS_ACTIVE);
    P(BSLA_ERROR_IS_ACTIVE);
    P(BSLA_FALLTHROUGH_IS_ACTIVE);
    P(BSLA_FORMAT_IS_ACTIVE);
    P(BSLA_NODISCARD_IS_ACTIVE);
    P(BSLA_NONNULLARG_IS_ACTIVE);
    P(BSLA_NONNULLARGS_IS_ACTIVE);
    P(BSLA_NORETURN_IS_ACTIVE);
    P(BSLA_NULLTERMINATEDAT_IS_ACTIVE);
    P(BSLA_NULLTERMINATED_IS_ACTIVE);
    P(BSLA_PRINTF_IS_ACTIVE);
    P(BSLA_SCANF_IS_ACTIVE);
    P(BSLA_UNUSED_IS_ACTIVE);
    P(BSLA_USED_IS_ACTIVE);
    P(BSLA_WARNING_IS_ACTIVE);

    printf("\n\n---------------------------------------------\n");
    printf(    "printFlags: bsls_annotation Referenced Macros\n");

    printf("\nBSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH
    printf("%s\n",
              STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED
    printf("%s\n",
             STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD
    printf("%s\n",
                STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NODISCARD) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN: ");
#ifdef BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN
    printf("%s\n",
                 STRINGIFY(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_NORETURN) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_PLATFORM_CMP_CLANG: ");
#ifdef BSLS_PLATFORM_CMP_CLANG
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_CLANG) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_PLATFORM_CMP_GNU: ");
#ifdef BSLS_PLATFORM_CMP_GNU
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_GNU) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_PLATFORM_CMP_HP: ");
#ifdef BSLS_PLATFORM_CMP_HP
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_HP) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_PLATFORM_CMP_IBM: ");
#ifdef BSLS_PLATFORM_CMP_IBM
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_IBM) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_PLATFORM_CMP_MSVC: ");
#ifdef BSLS_PLATFORM_CMP_MSVC
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_MSVC) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\nBSLS_PLATFORM_CMP_VERSION: ");
#ifdef BSLS_PLATFORM_CMP_VERSION
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_VERSION) );
#else
    printf("UNDEFINED\n");
#endif

    printf("\n__has_feature: ");
#ifdef __has_feature
    printf("DEFINED\n");
#else
    printf("UNDEFINED\n");
#endif

    printf("\n__has_warning: ");
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
        //: 1 This test driver does *not* build when the 'U_TRIGGER_ERRORS'
        //:   preprocessor variable is defined to 1 and all expected output
        //:   appears.
        //:
        //: 2 This test driver builds with all expected compiler warning
        //:   messages and no unexpected warnings when the 'U_TRIGGER_WARNINGS'
        //:   preprocessor variable is defined to 1.
        //:
        //: 3 When 'U_TRIGGER_WARNINGS' and 'U_TRIGGER_ERRORS' are both defined
        //:   to 0, the compile is successful and with no warnings.
        //
        // Plan:
        //: 1 Build with 'U_TRIGGER_ERRORS' defined to and externally confirm
        //:   that compilation of this task failed and the compiler output
        //:   shows the expected message.  (C-1)
        //:
        //: 2 Build with 'U_TRIGGER_WARNINGS' defined to and externally examine
        //:   compiler output for expected warnings and the absence of warnings
        //:   expected to be suppressed.  (C-2)
        //:
        //: 3 Build with 'U_TRIGGER_ERRORS' and 'U_TRIGGER_WARNINGS' both
        //:   defined to 0 and observe that the compile is successful with no
        //:   warnings.
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

            ASSERT(true); // remove unused warning for 'aSsErT'
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
// Copyright 2019 Bloomberg Finance L.P.
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
