// bsla_allocsize.t.cpp                                               -*-C++-*-
#include <bsla_allocsize.h>

#include <bsls_bsltestutil.h>

#include <bsls_types.h>

#include <stddef.h>  // 'size_t'
#include <stdio.h>
#include <stdlib.h>  // 'calloc', 'realloc', 'malloc', 'atoi'
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
//
// The table below classifies each of the annotations provided by this
// component by the entities to which it can be applied (i.e., function,
// variable, and type) and the expected result (optimization, error, warning,
// conditional warning, absence of warning).  The tag(s) found in the
// right-most column appear as comments throughout this test driver.  They can
// be used as an aid to navigation to the test code for each annotation, and an
// aid to assuring test coverage.
//..
//  No  Annotation                            E Result
//  --  ------------------------------------  - --------
//   1  BSLA_ALLOCSIZE(x)                     F optim.
//   2  BSLA_ALLOCSIZEMUL(x, y)               F optim.
//..
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

// ============================================================================
//                  USAGE WITH NO EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

///Usage
///-----
//
///Example 1: Communicating to the Compiler via Annotations
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, the optimizer has an intimate understanding of 'malloc' so it will
// figure out what we're doing with or withoutt the annotation, so we write
// our own 'myMalloc' that does a complex transform on the result to confuse
// the optimizer.
//..
    void *myMalloc(size_t size)
        // Allocate and return a block 'size' bytes long.
    {
        size_t ret = reinterpret_cast<size_t>(::malloc(size));

        // Do a complex transform on 'ret', this will repeatedly toggle bits in
        // 'ret', but it works out that each bit that is toggled is toggled an
        // even number of times, so the net transform is the identity
        // transform, but the optimizer won't be able to figure that out.

        const size_t loopGuard  = 0x1f70f960;    // garbage
        const size_t toggleMask = 0x97283741;    // garbage
        for (size_t ii = 0; 0 == (ii & loopGuard); ++ii) {
            ret ^= (toggleMask & ii);
        }

        return reinterpret_cast<void *>(ret);
    }
//..
// Then, we declare a function 'annotatedAlloc' that allocates memory and fills
// it with a desired byte.  We use the 'BSLA_ALLOCSIZE' annotation to indicate
// that the size of the returned block is equal to the value of the first
// argument to the function.
//..
    void *annotatedAlloc(size_t bufferSize, char fillChar) BSLA_ALLOCSIZE(1);
        // Fill the specified 'buffer' of specified length "bufferSize' with
        // garbage and return a pointer to it.
//
    void *annotatedAlloc(size_t bufferSize, char fillChar)
    {
        char * const ret = static_cast<char *>(myMalloc(bufferSize));
        char * const end = ret + bufferSize;
        for (char *pc = ret; pc < end; ++pc) {
            *pc = fillChar;
        }
        return ret;
    }
//..
// Next, we declare an identical function 'nonAnnotatedAlloc' without the
// annotation.
//..
    void *nonAnnotatedAlloc(size_t bufferSize, char fillChar)
    {
        char * const ret = static_cast<char *>(myMalloc(bufferSize));
        char * const end = ret + bufferSize;
        for (char *pc = ret; pc < end; ++pc) {
            *pc = fillChar;
        }

        return ret;
    }
//..

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER WARNINGS
// ----------------------------------------------------------------------------

#if U_TRIGGER_WARNINGS

#endif

// ============================================================================
//                  USAGE WITH EXPECTED COMPILER ERRORS
// ----------------------------------------------------------------------------

#if U_TRIGGER_ERRORS

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

    printf("\n\n------------------------------\n");
    printf(    "printFlags: *_IS_ACTIVE Macros\n\n");

    P(BSLA_ALLOCSIZE_IS_ACTIVE);
    P(BSLA_ALLOCSIZEMUL_IS_ACTIVE);

    printf("\n\n---------------------------------------------\n");
    printf(    "printFlags: bsls_annotation Referenced Macros\n");

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

    printf("\nBSLS_PLATFORM_CMP_VERSION: ");
#ifdef BSLS_PLATFORM_CMP_VERSION
    printf("%s\n", STRINGIFY(BSLS_PLATFORM_CMP_VERSION) );
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
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// Then, we call 'annotatedAlloc' and 'nonAnnotatedAlloc' to allocate some
// memory:
//..
    void *annotatedMem   = verbose
                         ? annotatedAlloc(20, 'w')
                         : annotatedAlloc(40, '8');
    void *nonAnnoatedMem = verbose
                         ? nonAnnotatedAlloc(20, 'a')
                         : nonAnnotatedAlloc(40, 'f');
//..
// Next, the 'BSLA_ALLOCSIZE' and 'BSLA_ALLOCSIZEMUL' annotations are only
// useful with respect to influencing the result of the '__builtin_object_size'
// compiler builtin, so, if 'BSLA_ALLOCSIZE_IS_ACTIVE' is 'true', then we can
// call '__builtin_object_size'.  Note that the feature is only active on
// optmized builds.
//..
    static bsls::Types::IntPtr ambi[4], nmbi[4];
//
    #if BSLA_ALLOCSIZE_IS_ACTIVE
//..
// Then, the function definition of '__builtin_object_size__' is as follows:
//..
//  size_t __builtin_object_size(void *p, int type);
//      // Return the size of the memory at the specified 'p'.  The specified
//      // 'type' must be in the range '[ 0 .. 3 ]'.
//      //: o If the low order bit of 'type' is clear, then if 'p' is known to
//      //:   point to a field in a struct, return the size of only that field,
//      //:   otherwise consider the answer 'unknown'.
//      //:
//      //: o If the lower order bit is set, take the size of the largest
//      //:   enclosing block, or the size of the dynamically allocated memory
//      //:   block.
//      //:
//      //: o If the high order bit is clear, return the maximum of the
//      //:   possible values.
//      //:
//      //: o If the high order bit is set, return the minimum of the possible
//      //:   values.
//      // If the compiler doesn't know the size, then if 'type < 2', return
//      // '(size_t) -1', otherwise return 0.
//..
// Now, we call '__builtin_object_size' on the result of the call to our
// annotated function.  TBD: eliminate the 'ASSERTV' and make into a proper
// usage example.
//..
        ambi[0] = __builtin_object_size(annotatedMem, 0);
        ambi[1] = __builtin_object_size(annotatedMem, 1);
        ambi[2] = __builtin_object_size(annotatedMem, 2);
        ambi[3] = __builtin_object_size(annotatedMem, 3);
        ASSERT(ambi[0] == -1);             // not a member, 'unknown'
        ASSERT(ambi[1] == 40);             // max of possible values
        ASSERT(ambi[2] == 0);              // not a member, 'unknown'
        ASSERT(ambi[3] == 20);             // min of possible values
//..
// Finally, we call '__builtin_object_size' on the result of the call to our
// non-annotated function and see that it never knows what's going on.
//..
        nmbi[0] = __builtin_object_size(nonAnnoatedMem, 0);
        nmbi[1] = __builtin_object_size(nonAnnoatedMem, 1);
        nmbi[2] = __builtin_object_size(nonAnnoatedMem, 2);
        nmbi[3] = __builtin_object_size(nonAnnoatedMem, 3);

        ASSERT(nmbi[0] == -1);
        ASSERT(nmbi[1] == -1);
        ASSERT(nmbi[2] == 0);
        ASSERT(nmbi[3] == 0);
//..
    #endif

    P(ambi[0]);    P(ambi[1]);    P(ambi[2]);    P(ambi[3]);
    P(nmbi[0]);    P(nmbi[1]);    P(nmbi[2]);    P(nmbi[3]);

    ::free(annotatedMem);
    ::free(nonAnnoatedMem);
      } break;
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
